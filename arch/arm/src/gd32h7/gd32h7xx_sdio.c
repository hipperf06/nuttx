/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_sdio.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/sdio.h>
#include <nuttx/wdog.h>

#include <arch/board/board.h>

#include "chip.h"
#include "arm_internal.h"
#include "hardware/gd32h7xx_sdio.h"
#include "gd32h7xx_sdio.h"
#include "gd32h7xx_rcu.h"

#ifdef CONFIG_GD32H7_SDIO

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define SDIO_CMDTIMEOUT       (100 * 1000)   /* 100ms in units of 10us */
#define SDIO_LONGRESP         4              /* Number of long response words */

/* The SDIO kernel clock is CK_AHB in this port */

#define SDIO_KERNEL_CLOCK     GD32_AHB_FREQUENCY

/* The SDIOWAIT_* event bits and SDIO_EVENT* masks come from <nuttx/sdio.h> */

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct gd32_devinfo_s
{
  uint32_t base;                 /* SDIO register base */
  FAR uint8_t *dbuf;             /* Transfer buffer (single-buffer mode) */
  size_t   dlen;                 /* Remaining bytes of the transfer */
  uint32_t waitflags;            /* Set of SDIOWAIT_* events */
  sem_t    waitsem;              /* Signalled by the interrupt handler */

  /* Media change callback (only used by NuttX flavours that support it) */

  void   (*callback)(FAR void *arg);
  FAR void *cbarg;
  sdio_eventset_t cbevents;      /* Events the callback is armed for */

  bool     widebus;              /* Wide bus enabled */
  int      irq;                  /* SDIO interrupt number */
  bool     attached;             /* Interrupt handler attached */
};

struct gd32_sdio_s
{
  struct sdio_dev_s dev;         /* Must be first */
  struct gd32_devinfo_s info;
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void sdio_reset(FAR struct sdio_dev_s *dev);
static sdio_capset_t sdio_capabilities(FAR struct sdio_dev_s *dev);
static sdio_statset_t sdio_status(FAR struct sdio_dev_s *dev);
static void sdio_widebus(FAR struct sdio_dev_s *dev, bool enable);
static void sdio_clock(FAR struct sdio_dev_s *dev,
                       enum sdio_clock_e rate);
static int  sdio_attach(FAR struct sdio_dev_s *dev);
static int  sdio_sendcmd(FAR struct sdio_dev_s *dev, uint32_t cmd,
                         uint32_t arg);
static int  sdio_recv_r1(FAR struct sdio_dev_s *dev, uint32_t cmd,
                         FAR uint32_t *r1);
static int  sdio_recv_r2(FAR struct sdio_dev_s *dev, uint32_t cmd,
                         FAR uint32_t r2[4]);
static int  sdio_recv_r3(FAR struct sdio_dev_s *dev, uint32_t cmd,
                         FAR uint32_t *r3);
static int  sdio_recv_r4(FAR struct sdio_dev_s *dev, uint32_t cmd,
                         FAR uint32_t *r4);
static int  sdio_recv_r5(FAR struct sdio_dev_s *dev, uint32_t cmd,
                         FAR uint32_t *r5);
static int  sdio_recv_r6(FAR struct sdio_dev_s *dev, uint32_t cmd,
                         FAR uint32_t *r6);
static int  sdio_recv_r7(FAR struct sdio_dev_s *dev, uint32_t cmd,
                         FAR uint32_t *r7);
static int  sdio_recvsetup(FAR struct sdio_dev_s *dev, FAR uint8_t *buffer,
                           size_t nbytes);
static int  sdio_sendsetup(FAR struct sdio_dev_s *dev,
                           FAR const uint8_t *buffer, size_t nbytes);
static int  sdio_cancel(FAR struct sdio_dev_s *dev);
static int  sdio_waitresponse(FAR struct sdio_dev_s *dev, uint32_t cmd);
static void sdio_waitenable(FAR struct sdio_dev_s *dev,
                            sdio_eventset_t eventset, uint32_t timeout);
static sdio_eventset_t sdio_eventwait(FAR struct sdio_dev_s *dev);
#if defined(CONFIG_SCHED_WORKQUEUE) && defined(CONFIG_SCHED_HPWORK)
static void sdio_callbackenable(FAR struct sdio_dev_s *dev,
                                sdio_eventset_t eventset);
static int  sdio_registercallback(FAR struct sdio_dev_s *dev,
                                  void (*callback)(FAR void *arg),
                                  FAR void *arg);
#endif
static int  sdio_interrupt(int irq, FAR void *context, FAR void *arg);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct gd32_sdio_s g_sdiodev =
{
  .dev    =
  {
    .mutex = NXMUTEX_INITIALIZER,

    .reset        = sdio_reset,
    .capabilities = sdio_capabilities,
    .status       = sdio_status,
    .widebus      = sdio_widebus,
    .clock        = sdio_clock,
    .attach       = sdio_attach,
    .sendcmd      = sdio_sendcmd,
    .recv_r1      = sdio_recv_r1,
    .recv_r2      = sdio_recv_r2,
    .recv_r3      = sdio_recv_r3,
    .recv_r4      = sdio_recv_r4,
    .recv_r5      = sdio_recv_r5,
    .recv_r6      = sdio_recv_r6,
    .recv_r7      = sdio_recv_r7,
    .waitresponse = sdio_waitresponse,
    .recvsetup    = sdio_recvsetup,
    .sendsetup    = sdio_sendsetup,
    .cancel       = sdio_cancel,
    .waitenable   = sdio_waitenable,
    .eventwait    = sdio_eventwait,
#if defined(CONFIG_SCHED_WORKQUEUE) && defined(CONFIG_SCHED_HPWORK)
    .callbackenable   = sdio_callbackenable,
    .registercallback = sdio_registercallback,
#endif
  },
  .info   =
  {
    .base    = GD32H7_SDIO0,
    .waitsem = SEM_INITIALIZER(0),
    .irq     = GD32_IRQ_SDIO0,
  },
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_sdio_putfifo / gd32_sdio_getfifo
 ****************************************************************************/

static void gd32_sdio_putfifo(FAR struct gd32_devinfo_s *priv,
                              uint32_t word)
{
  putreg32(word, GD32H7_SDIO_FIFO(priv->base));
}

static uint32_t gd32_sdio_getfifo(FAR struct gd32_devinfo_s *priv)
{
  return getreg32(GD32H7_SDIO_FIFO(priv->base));
}

/****************************************************************************
 * Name: gd32_sdio_pollfifo
 *
 * Description:
 *   Poll-transfer the configured buffer through the data FIFO.  This is
 *   the non-DMA fallback path.
 *
 ****************************************************************************/

static int gd32_sdio_pollfifo(FAR struct gd32_devinfo_s *priv)
{
  volatile uint32_t timeout = SDIO_CMDTIMEOUT * 50;
  uint32_t words;
  uint32_t i;
  bool recv = (getreg32(GD32H7_SDIO_DATACTL(priv->base)) &
               SDIO_DATACTL_DATADIR) != 0;

  words = (priv->dlen + 3) / 4;

  while (words > 0 && timeout-- > 0)
    {
      uint32_t stat = getreg32(GD32H7_SDIO_STAT(priv->base));

      if (stat & SDIO_STAT_DATA_ERRS)
        {
          return -EIO;
        }

      if (recv)
        {
          if (!(stat & SDIO_STAT_RFE))
            {
              uint32_t word = gd32_sdio_getfifo(priv);
              uint32_t n = priv->dlen < 4 ? priv->dlen : 4;

              memcpy(priv->dbuf, &word, n);
              priv->dbuf += n;
              priv->dlen -= n;
              words--;
            }
        }
      else
        {
          if (!(stat & SDIO_STAT_TFF))
            {
              uint32_t word = 0;
              uint32_t n = priv->dlen < 4 ? priv->dlen : 4;

              memcpy(&word, priv->dbuf, n);
              gd32_sdio_putfifo(priv, word);
              priv->dbuf += n;
              priv->dlen -= n;
              words--;
            }
        }
    }

  /* Wait for the data end flag */

  timeout = SDIO_CMDTIMEOUT * 20;
  while (timeout-- > 0)
    {
      uint32_t stat = getreg32(GD32H7_SDIO_STAT(priv->base));

      if (stat & SDIO_STAT_DTEND)
        {
          putreg32(SDIO_STAT_DTEND, GD32H7_SDIO_INTC(priv->base));
          priv->waitflags |= SDIOWAIT_TRANSFERDONE;
          return OK;
        }

      if (stat & SDIO_STAT_DATA_ERRS)
        {
          return -EIO;
        }
    }

  UNUSED(i);
  return -ETIMEDOUT;
}

/****************************************************************************
 * Name: sdio_reset
 ****************************************************************************/

static void sdio_reset(FAR struct sdio_dev_s *dev)
{
  FAR struct gd32_devinfo_s *priv = &((FAR struct gd32_sdio_s *)dev)->info;
  uint32_t regval;

  gd32_rcu_periph_clock_enable(RCU_SDIO0_EN);

  /* Reset the SDIO peripheral */

  gd32_rcu_periph_reset(RCU_SDIO0_RST);

  /* Power on the card */

  putreg32(SDIO_PWRCTL_PWRVAL, GD32H7_SDIO_PWRCTL(priv->base));

  /* Configure the initial 400kHz identification clock.  The kernel clock
   * is divided by (CLKCTL + 2).
   */

  regval = SDIO_KERNEL_CLOCK / (400 * 1000) - 2;
  if (regval > 0x3ff)
    {
      regval = 0x3ff;
    }

  putreg32(regval | SDIO_CLKCTL_CLKEN | SDIO_CLKCTL_HWEN,
           GD32H7_SDIO_CLKCTL(priv->base));

  /* Long data timeout */

  putreg32(0x00f00000, GD32H7_SDIO_DATATO(priv->base));
}

/****************************************************************************
 * Name: sdio_capabilities
 ****************************************************************************/

static sdio_capset_t sdio_capabilities(FAR struct sdio_dev_s *dev)
{
  return SDIO_CAPS_4BIT;
}

/****************************************************************************
 * Name: sdio_status
 ****************************************************************************/

static sdio_statset_t sdio_status(FAR struct sdio_dev_s *dev)
{
  return SDIO_STATUS_PRESENT;
}

/****************************************************************************
 * Name: sdio_widebus
 ****************************************************************************/

static void sdio_widebus(FAR struct sdio_dev_s *dev, bool enable)
{
  FAR struct gd32_devinfo_s *priv = &((FAR struct gd32_sdio_s *)dev)->info;
  uint32_t regval = getreg32(GD32H7_SDIO_CLKCTL(priv->base));

  regval &= ~SDIO_CLKCTL_BUSMODE_MASK;
  regval |= enable ? SDIO_BUSMODE_4BIT : SDIO_BUSMODE_1BIT;
  putreg32(regval, GD32H7_SDIO_CLKCTL(priv->base));

  priv->widebus = enable;
}

/****************************************************************************
 * Name: sdio_clock
 ****************************************************************************/

static void sdio_clock(FAR struct sdio_dev_s *dev,
                       enum sdio_clock_e rate)
{
  FAR struct gd32_devinfo_s *priv = &((FAR struct gd32_sdio_s *)dev)->info;
  uint32_t freq;
  uint32_t regval;

  switch (rate)
    {
      case CLOCK_SD_TRANSFER_4BIT:
      case CLOCK_SD_TRANSFER_1BIT:
        freq = 25 * 1000 * 1000;
        break;

      case CLOCK_IDMODE:
      default:
        freq = 400 * 1000;
        break;
    }

  regval = getreg32(GD32H7_SDIO_CLKCTL(priv->base));
  regval &= ~(SDIO_CLKCTL_DIV_MASK | SDIO_CLKCTL_CLKEN);

  regval |= SDIO_KERNEL_CLOCK / freq - 2;
  putreg32(regval, GD32H7_SDIO_CLKCTL(priv->base));

  regval |= SDIO_CLKCTL_CLKEN;
  putreg32(regval, GD32H7_SDIO_CLKCTL(priv->base));
}

/****************************************************************************
 * Name: sdio_attach
 ****************************************************************************/

static int sdio_attach(FAR struct sdio_dev_s *dev)
{
  FAR struct gd32_devinfo_s *priv = &((FAR struct gd32_sdio_s *)dev)->info;
  int ret;

  if (priv->attached)
    {
      return OK;
    }

  ret = irq_attach(priv->irq, sdio_interrupt, priv);
  if (ret < 0)
    {
      return ret;
    }

  up_enable_irq(priv->irq);
  priv->attached = true;
  return OK;
}

/****************************************************************************
 * Name: sdio_interrupt
 *
 * Description:
 *   SDIO interrupt handler.  Data transfer completion and errors are
 *   signalled to the waiting thread through the semaphore.
 *
 ****************************************************************************/

static int sdio_interrupt(int irq, FAR void *context, FAR void *arg)
{
  FAR struct gd32_devinfo_s *priv = (FAR struct gd32_devinfo_s *)arg;
  uint32_t stat;

  DEBUGASSERT(priv != NULL);

  stat = getreg32(GD32H7_SDIO_STAT(priv->base));

  if (stat & SDIO_STAT_DTEND)
    {
      putreg32(SDIO_STAT_DTEND, GD32H7_SDIO_INTC(priv->base));
      priv->waitflags |= SDIOWAIT_TRANSFERDONE;
      nxsem_post(&priv->waitsem);
    }
  else if (stat & SDIO_STAT_DATA_ERRS)
    {
      putreg32(stat & SDIO_STAT_DATA_ERRS, GD32H7_SDIO_INTC(priv->base));
      priv->waitflags |= SDIOWAIT_ERROR;
      nxsem_post(&priv->waitsem);
    }

  return OK;
}

/****************************************************************************
 * Name: sdio_sendcmd
 ****************************************************************************/

static int sdio_sendcmd(FAR struct sdio_dev_s *dev, uint32_t cmd,
                        uint32_t arg)
{
  FAR struct gd32_devinfo_s *priv = &((FAR struct gd32_sdio_s *)dev)->info;
  uint32_t regval;
  uint32_t resp = SDIO_RESP_NONE;

  /* Determine the expected response type from the command index */

  switch (cmd & MMCSD_RESPONSE_MASK)
    {
      case MMCSD_R1B_RESPONSE:
      case MMCSD_R1_RESPONSE:
      case MMCSD_R3_RESPONSE:
      case MMCSD_R4_RESPONSE:
      case MMCSD_R5_RESPONSE:
      case MMCSD_R6_RESPONSE:
      case MMCSD_R7_RESPONSE:
        resp = SDIO_RESP_SHORT;
        break;

      case MMCSD_R2_RESPONSE:
        resp = SDIO_RESP_LONG;
        break;

      default:
      case MMCSD_NO_RESPONSE:
        resp = SDIO_RESP_NONE;
        break;
    }

  /* Clear the pending flags */

  putreg32(SDIO_INTC_ALL, GD32H7_SDIO_INTC(priv->base));
  priv->waitflags = 0;

  /* Program the argument and the command */

  putreg32(arg, GD32H7_SDIO_CMDAGMT(priv->base));

  regval = (cmd & MMCSD_CMDIDX_MASK) | SDIO_CMDCTL_CSMEN | resp;
  putreg32(regval, GD32H7_SDIO_CMDCTL(priv->base));
  return OK;
}

/****************************************************************************
 * Name: sdio_waitresponse
 ****************************************************************************/

static int sdio_waitresponse(FAR struct sdio_dev_s *dev, uint32_t cmd)
{
  FAR struct gd32_devinfo_s *priv = &((FAR struct gd32_sdio_s *)dev)->info;
  volatile uint32_t timeout = SDIO_CMDTIMEOUT;
  uint32_t stat;

  while (timeout-- > 0)
    {
      stat = getreg32(GD32H7_SDIO_STAT(priv->base));

      if (stat & SDIO_STAT_CMDTMOUT)
        {
          putreg32(SDIO_STAT_CMDTMOUT, GD32H7_SDIO_INTC(priv->base));
          return -ETIMEDOUT;
        }

      if (stat & SDIO_STAT_CCRCERR)
        {
          putreg32(SDIO_STAT_CCRCERR, GD32H7_SDIO_INTC(priv->base));

          /* R1/R3/R7 allow CRC errors on some commands; report it */

          return -EIO;
        }

      if (stat & SDIO_STAT_CMDRECV)
        {
          putreg32(SDIO_STAT_CMDRECV, GD32H7_SDIO_INTC(priv->base));
          priv->waitflags |= SDIOWAIT_RESPONSEDONE;
          return OK;
        }
    }

  return -ETIMEDOUT;
}

/****************************************************************************
 * Name: sdio_recv_r1 / r2 / r3 / r6 / r7
 ****************************************************************************/

static int sdio_recv_r1(FAR struct sdio_dev_s *dev, uint32_t cmd,
                        FAR uint32_t *r1)
{
  FAR struct gd32_devinfo_s *priv = &((FAR struct gd32_sdio_s *)dev)->info;
  int ret;

  ret = sdio_waitresponse(dev, cmd);
  if (ret == OK)
    {
      *r1 = getreg32(GD32H7_SDIO_RESP0(priv->base));
    }

  return ret;
}

static int sdio_recv_r2(FAR struct sdio_dev_s *dev, uint32_t cmd,
                        FAR uint32_t r2[4])
{
  FAR struct gd32_devinfo_s *priv = &((FAR struct gd32_sdio_s *)dev)->info;
  int ret;
  int i;

  ret = sdio_waitresponse(dev, cmd);
  if (ret == OK)
    {
      for (i = 0; i < SDIO_LONGRESP; i++)
        {
          r2[i] = getreg32(GD32H7_SDIO_RESP0(priv->base) + 4 * i);
        }
    }

  return ret;
}

static int sdio_recv_r3(FAR struct sdio_dev_s *dev, uint32_t cmd,
                        FAR uint32_t *r3)
{
  return sdio_recv_r1(dev, cmd, r3);
}

static int sdio_recv_r4(FAR struct sdio_dev_s *dev, uint32_t cmd,
                        FAR uint32_t *r4)
{
  return sdio_recv_r1(dev, cmd, r4);
}

static int sdio_recv_r5(FAR struct sdio_dev_s *dev, uint32_t cmd,
                        FAR uint32_t *r5)
{
  return sdio_recv_r1(dev, cmd, r5);
}

static int sdio_recv_r6(FAR struct sdio_dev_s *dev, uint32_t cmd,
                        FAR uint32_t *r6)
{
  return sdio_recv_r1(dev, cmd, r6);
}

static int sdio_recv_r7(FAR struct sdio_dev_s *dev, uint32_t cmd,
                        FAR uint32_t *r7)
{
  return sdio_recv_r1(dev, cmd, r7);
}

/****************************************************************************
 * Name: sdio_recvsetup / sdio_sendsetup
 ****************************************************************************/

static int sdio_recvsetup(FAR struct sdio_dev_s *dev, FAR uint8_t *buffer,
                          size_t nbytes)
{
  FAR struct gd32_devinfo_s *priv = &((FAR struct gd32_sdio_s *)dev)->info;
  uint32_t regval;

  priv->dbuf = buffer;
  priv->dlen = nbytes;

  /* Program the data path: card -> host, block mode */

  putreg32(nbytes, GD32H7_SDIO_DATALEN(priv->base));

  regval = getreg32(GD32H7_SDIO_DATACTL(priv->base));
  regval &= ~(SDIO_DATACTL_BLKSZ_MASK | SDIO_DATACTL_DATADIR |
              SDIO_DATACTL_TRANSMOD_BLOCK | SDIO_DATACTL_DMAEN);
  regval |= SDIO_DATACTL_DATAEN | SDIO_DATACTL_DATADIR;
  regval |= (9 << SDIO_DATACTL_BLKSZ_SHIFT);  /* 512-byte blocks */
  putreg32(regval, GD32H7_SDIO_DATACTL(priv->base));
  return OK;
}

static int sdio_sendsetup(FAR struct sdio_dev_s *dev,
                          FAR const uint8_t *buffer, size_t nbytes)
{
  FAR struct gd32_devinfo_s *priv = &((FAR struct gd32_sdio_s *)dev)->info;
  uint32_t regval;

  priv->dbuf = (FAR uint8_t *)buffer;
  priv->dlen = nbytes;

  putreg32(nbytes, GD32H7_SDIO_DATALEN(priv->base));

  regval = getreg32(GD32H7_SDIO_DATACTL(priv->base));
  regval &= ~(SDIO_DATACTL_BLKSZ_MASK | SDIO_DATACTL_DATADIR |
              SDIO_DATACTL_DMAEN);
  regval |= SDIO_DATACTL_DATAEN;
  regval |= (9 << SDIO_DATACTL_BLKSZ_SHIFT);
  putreg32(regval, GD32H7_SDIO_DATACTL(priv->base));
  return OK;
}

/****************************************************************************
 * Name: sdio_cancel
 ****************************************************************************/

static int sdio_cancel(FAR struct sdio_dev_s *dev)
{
  FAR struct gd32_devinfo_s *priv = &((FAR struct gd32_sdio_s *)dev)->info;

  /* Reset the data path */

  modifyreg32(GD32H7_SDIO_DATACTL(priv->base), 0, SDIO_DATACTL_FIFOREST);
  priv->dlen = 0;
  priv->waitflags |= SDIOWAIT_ERROR;
  return OK;
}

/****************************************************************************
 * Name: sdio_waitenable / sdio_eventwait
 ****************************************************************************/

static void sdio_waitenable(FAR struct sdio_dev_s *dev,
                            sdio_eventset_t eventset, uint32_t timeout)
{
  FAR struct gd32_devinfo_s *priv = &((FAR struct gd32_sdio_s *)dev)->info;

  /* In this polling implementation, kick off the FIFO transfer right away
   * when a data event is requested.
   */

  priv->waitflags = 0;

  if (eventset & (SDIOWAIT_TRANSFERDONE | SDIOWAIT_CMDDONE))
    {
      /* Enable the end-of-transfer and data error interrupts so that the
       * ISR can signal completion, then push the data through the FIFO.
       */

      modifyreg32(GD32H7_SDIO_INTEN(priv->base), 0,
                  SDIO_STAT_DTEND | SDIO_STAT_DATA_ERRS);

      if (priv->dlen > 0)
        {
          int ret = gd32_sdio_pollfifo(priv);

          if (ret < 0)
            {
              priv->waitflags |= (ret == -ETIMEDOUT) ? SDIOWAIT_TIMEOUT :
                                                       SDIOWAIT_ERROR;
            }
        }
      else
        {
          priv->waitflags |= SDIOWAIT_TRANSFERDONE;
        }
    }
  else if (eventset & (SDIOWAIT_CMDDONE | SDIOWAIT_RESPONSEDONE))
    {
      priv->waitflags |= SDIOWAIT_CMDDONE | SDIOWAIT_RESPONSEDONE;
    }
}

static sdio_eventset_t sdio_eventwait(FAR struct sdio_dev_s *dev)
{
  FAR struct gd32_devinfo_s *priv = &((FAR struct gd32_sdio_s *)dev)->info;
  sdio_eventset_t eventset = 0;

  /* If the transfer is still in progress, wait for the interrupt handler
   * to signal completion (1 second explicit timeout).
   */

  if ((priv->waitflags & (SDIOWAIT_TRANSFERDONE | SDIOWAIT_ERROR |
                          SDIOWAIT_TIMEOUT)) == 0)
    {
      if (nxsem_tickwait_uninterruptible(&priv->waitsem,
                                         MSEC2TICK(1000)) < 0)
        {
          priv->waitflags |= SDIOWAIT_TIMEOUT;
        }
    }

  /* Mask the transfer interrupts again */

  modifyreg32(GD32H7_SDIO_INTEN(priv->base),
              SDIO_STAT_DTEND | SDIO_STAT_DATA_ERRS, 0);

  if (priv->waitflags & SDIOWAIT_ERROR)
    {
      eventset |= SDIOWAIT_ERROR;
    }

  if (priv->waitflags & SDIOWAIT_TIMEOUT)
    {
      eventset |= SDIOWAIT_TIMEOUT;
    }

  if (priv->waitflags & SDIOWAIT_RESPONSEDONE)
    {
      eventset |= SDIOWAIT_RESPONSEDONE;
    }

  if (priv->waitflags & SDIOWAIT_TRANSFERDONE)
    {
      eventset |= SDIOWAIT_TRANSFERDONE;
    }

  priv->waitflags = 0;
  return eventset;
}

/****************************************************************************
 * Name: sdio_callbackenable
 *
 * Description:
 *   Arm/disarm the media-change callback.  This driver has no card-detect
 *   pin wired up, so events are recorded but never fired; mmcsd then falls
 *   back to polling the SDIO_STATUS_PRESENT bit.
 *
 ****************************************************************************/

#if defined(CONFIG_SCHED_WORKQUEUE) && defined(CONFIG_SCHED_HPWORK)

static void sdio_callbackenable(FAR struct sdio_dev_s *dev,
                                sdio_eventset_t eventset)
{
  FAR struct gd32_devinfo_s *priv = &((FAR struct gd32_sdio_s *)dev)->info;

  priv->cbevents = eventset;
}

/****************************************************************************
 * Name: sdio_registercallback
 *
 * Description:
 *   Register the media-change callback used by the older NuttX mmcsd
 *   implementation.
 *
 ****************************************************************************/

static int sdio_registercallback(FAR struct sdio_dev_s *dev,
                                 void (*callback)(FAR void *arg),
                                 FAR void *arg)
{
  FAR struct gd32_devinfo_s *priv = &((FAR struct gd32_sdio_s *)dev)->info;

  priv->callback = callback;
  priv->cbarg    = arg;
  priv->cbevents = 0;
  return OK;
}

#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_sdio_initialize
 *
 * Description:
 *   Initialize SDIO for media access.
 *
 * Returned Value:
 *   A pointer to the SDIO device interface, or NULL on failure.
 *
 ****************************************************************************/

FAR struct sdio_dev_s *gd32_sdio_initialize(int slot)
{
  FAR struct gd32_sdio_s *dev = &g_sdiodev;

  if (slot != 0)
    {
      return NULL;
    }

  return &dev->dev;
}

#endif /* CONFIG_GD32H7_SDIO */
