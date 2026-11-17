/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_can.c
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
#include <errno.h>
#include <assert.h>
#include <string.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/can/can.h>
#include <nuttx/kmalloc.h>

#include <arch/board/board.h>

#include "chip.h"
#include "arm_internal.h"
#include "gd32h7xx_can.h"
#include "gd32h7xx_rcu.h"

#ifdef CONFIG_CAN

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define CAN_TIMEOUT       0x001fffff

/* Number of TX mailboxes used by this driver (slots 0..3) */

#define CAN_TX_MBOX       4

/* Bit timing for the classical CAN mode: sample point at ~75% of the bit
 * time with 8 time quanta per bit.  The prescaler is derived from the
 * requested baud rate at runtime.
 */

#define CAN_TQ_PER_BIT    8
#define CAN_PTS           1
#define CAN_PBS1          4
#define CAN_PBS2          2
#define CAN_SJW           1

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct gd32_can_s
{
  uint8_t port;              /* CAN port number (0..2) */
  uint32_t base;             /* CAN base address */
  uint32_t baud;             /* Configured baud rate */
  uint32_t psc;              /* Baud rate prescaler */
  uint8_t txmb[CAN_TX_MBOX]; /* Mailbox -> message association (index into
                              * the upper-half tx cache; 0xff = free) */
  bool loopback;             /* Loopback (self-test) mode */
  bool silent;               /* Silent (listen-only) mode */
  FAR struct can_dev_s *dev; /* Upper half driver */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void can_reset(FAR struct can_dev_s *dev);
static int  can_setup(FAR struct can_dev_s *dev);
static void can_shutdown(FAR struct can_dev_s *dev);
static void can_rxint(FAR struct can_dev_s *dev, bool enable);
static void can_txint(FAR struct can_dev_s *dev, bool enable);
static int  can_ioctl(FAR struct can_dev_s *dev, int cmd,
                      unsigned long arg);
static int  can_send(FAR struct can_dev_s *dev,
                     FAR struct can_msg_s *msg);
static bool can_txready(FAR struct can_dev_s *dev);
static bool can_txempty(FAR struct can_dev_s *dev);

static int  gd32_can_interrupt(int irq, FAR void *context,
                               FAR void *arg);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct can_ops_s g_canops =
{
  .co_reset         = can_reset,
  .co_setup         = can_setup,
  .co_shutdown      = can_shutdown,
  .co_rxint         = can_rxint,
  .co_txint         = can_txint,
  .co_ioctl         = can_ioctl,
  .co_remoterequest = NULL,
  .co_send          = can_send,
  .co_txready       = can_txready,
  .co_txempty       = can_txempty,
};

#ifdef CONFIG_GD32H7_CAN0
static struct gd32_can_s g_can0priv =
{
  .port = 0,
  .base = GD32H7_CAN0,
};
#endif

#ifdef CONFIG_GD32H7_CAN1
static struct gd32_can_s g_can1priv =
{
  .port = 1,
  .base = GD32H7_CAN1,
};
#endif

#ifdef CONFIG_GD32H7_CAN2
static struct gd32_can_s g_can2priv =
{
  .port = 2,
  .base = GD32H7_CAN2,
};
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_can_enter_inactive / gd32_can_exit_inactive
 ****************************************************************************/

static int gd32_can_enter_inactive(uint32_t base)
{
  volatile uint32_t timeout = CAN_TIMEOUT;
  uint32_t regval;

  regval = getreg32(GD32H7_CAN_CTL0(base));
  regval &= ~(CAN_CTL0_CANDIS);
  regval |= (CAN_CTL0_HALT | CAN_CTL0_INAMOD);
  putreg32(regval, GD32H7_CAN_CTL0(base));

  while (timeout-- > 0)
    {
      regval = getreg32(GD32H7_CAN_CTL0(base));
      if ((regval & (CAN_CTL0_NRDY | CAN_CTL0_INAS)) ==
          (CAN_CTL0_NRDY | CAN_CTL0_INAS))
        {
          return OK;
        }
    }

  return -ETIMEDOUT;
}

static int gd32_can_exit_inactive(uint32_t base)
{
  volatile uint32_t timeout = CAN_TIMEOUT;
  uint32_t regval;

  regval = getreg32(GD32H7_CAN_CTL0(base));
  regval &= ~(CAN_CTL0_HALT | CAN_CTL0_INAMOD);
  putreg32(regval, GD32H7_CAN_CTL0(base));

  while (timeout-- > 0)
    {
      if ((getreg32(GD32H7_CAN_CTL0(base)) & CAN_CTL0_INAS) == 0)
        {
          return OK;
        }
    }

  return -ETIMEDOUT;
}

/****************************************************************************
 * Name: gd32_can_bittiming
 *
 * Description:
 *   Program the bit timing register for the requested baud rate.
 *
 ****************************************************************************/

static int gd32_can_bittiming(FAR struct gd32_can_s *priv)
{
  uint32_t pclk;
  uint32_t psc;
  uint32_t regval;

  /* The CAN kernel clock is CK_APB1 in this port */

  pclk = GD32_PCLK1_FREQUENCY;

  psc = (pclk / CAN_TQ_PER_BIT) / priv->baud;
  if (psc == 0 || psc > 1024)
    {
      return -EINVAL;
    }

  regval = getreg32(GD32H7_CAN_BT(priv->base));
  regval &= ~(0x7ffu << CAN_BT_BAUDPSC_SHIFT);
  regval |= CAN_BT(psc, CAN_SJW, CAN_PTS, CAN_PBS1, CAN_PBS2);
  putreg32(regval, GD32H7_CAN_BT(priv->base));

  priv->psc  = psc;
  priv->baud = pclk / CAN_TQ_PER_BIT / psc;
  return OK;
}

/****************************************************************************
 * Name: gd32_can_bittiming_set
 *
 * Description:
 *   Program an explicit bit timing (used by CANIOC_SET_BITTIMING).
 *
 ****************************************************************************/

static int gd32_can_bittiming_set(FAR struct gd32_can_s *priv,
                                  uint32_t baud, uint8_t tseg1,
                                  uint8_t tseg2, uint8_t sjw)
{
  uint32_t pclk;
  uint32_t tq;
  uint32_t psc;
  uint32_t regval;

  if (baud == 0 || tseg1 < 1 || tseg2 < 1 || sjw < 1)
    {
      return -EINVAL;
    }

  pclk = GD32_PCLK1_FREQUENCY;
  tq   = (uint32_t)tseg1 + tseg2 + 1;

  psc = pclk / (baud * tq);
  if (psc == 0 || psc > 1024)
    {
      return -EINVAL;
    }

  /* Enter inactive mode before touching the bit timing */

  if (gd32_can_enter_inactive(priv->base) < 0)
    {
      return -ETIMEDOUT;
    }

  regval = getreg32(GD32H7_CAN_BT(priv->base));
  regval &= ~((0x7ffu << CAN_BT_BAUDPSC_SHIFT) |
              (0x1fu << CAN_BT_SJW_SHIFT) |
              (0x3fu << CAN_BT_PTS_SHIFT) |
              (0x1fu << CAN_BT_PBS1_SHIFT) |
              (0x1fu << CAN_BT_PBS2_SHIFT));
  regval |= CAN_BT(psc, sjw, CAN_PTS, tseg1, tseg2);
  putreg32(regval, GD32H7_CAN_BT(priv->base));

  priv->psc  = psc;
  priv->baud = pclk / (psc * tq);

  return gd32_can_exit_inactive(priv->base);
}

/****************************************************************************
 * Name: gd32_can_connmode
 *
 * Description:
 *   Apply loopback / silent mode.  Must be called in inactive mode.
 *
 ****************************************************************************/

static void gd32_can_connmode(FAR struct gd32_can_s *priv)
{
  uint32_t ctl0 = getreg32(GD32H7_CAN_CTL0(priv->base));
  uint32_t ctl1 = getreg32(GD32H7_CAN_CTL1(priv->base));

  ctl1 &= ~CAN_CTL1_LSCMOD;   /* Loopback / silent */

  if (priv->loopback || priv->silent)
    {
      ctl1 |= CAN_CTL1_LSCMOD;
    }

  if (priv->silent)
    {
      ctl1 |= CAN_CTL1_MMOD;  /* Monitor mode */
    }
  else
    {
      ctl1 &= ~CAN_CTL1_MMOD;
    }

  /* Self reception required in loopback mode */

  if (priv->loopback)
    {
      ctl0 &= ~CAN_CTL0_SRDIS;
    }
  else
    {
      ctl0 |= CAN_CTL0_SRDIS;
    }

  putreg32(ctl0, GD32H7_CAN_CTL0(priv->base));
  putreg32(ctl1, GD32H7_CAN_CTL1(priv->base));
}

/****************************************************************************
 * Name: gd32_can_rxinterrupt
 *
 * Description:
 *   An interrupt was received indicating that a message is available in
 *   the Rx FIFO (slot CAN_FIFO_SLOT).
 *
 ****************************************************************************/

static void gd32_can_rxinterrupt(FAR struct gd32_can_s *priv)
{
  FAR uint32_t *fifo = (FAR uint32_t *)GD32H7_CAN_RAM(priv->base) +
                       GD32H7_CAN_FIFO_SLOT * GD32H7_CAN_MB_WORDS;
  struct can_hdr_s hdr;
  uint32_t mdes0;
  uint32_t mdes1;
  uint32_t data[2];
  uint8_t dlc;
  int i;

  mdes0 = fifo[0];
  mdes1 = fifo[1];
  data[0] = fifo[2];
  data[1] = fifo[3];

  memset(&hdr, 0, sizeof(hdr));

  dlc = (mdes0 >> CAN_MDES0_DLC_SHIFT) & 0xf;
  hdr.ch_dlc   = dlc;
  hdr.ch_rtr   = (mdes0 & CAN_MDES0_RTR) != 0;
  hdr.ch_extid = (mdes0 & CAN_MDES0_IDE) != 0;

  if (hdr.ch_extid)
    {
      hdr.ch_id = mdes1 & CAN_MDES1_ID_EXD_MASK;
    }
  else
    {
      hdr.ch_id = (mdes1 >> CAN_MDES1_ID_STD_SHIFT) & 0x7ff;
    }

  /* Deliver the message to the upper half */

  can_receive(priv->dev, &hdr, (FAR uint8_t *)data);

  /* Clear the FIFO status and re-arm the mailbox for reception */

  putreg32(CAN_STAT_MS5_RFNE, GD32H7_CAN_STAT(priv->base));

  mdes0 &= ~(0xfu << CAN_MDES0_CODE_SHIFT);
  mdes0 |= (CAN_MB_RX_EMPTY << CAN_MDES0_CODE_SHIFT);
  fifo[0] = mdes0;
  fifo[1] = mdes1;
  UNUSED(i);
}

/****************************************************************************
 * Name: gd32_can_txinterrupt
 *
 * Description:
 *   A TX mailbox completed transmission; notify the upper half.
 *
 ****************************************************************************/

static void gd32_can_txinterrupt(FAR struct gd32_can_s *priv, int mb)
{
  FAR uint32_t *mdes = (FAR uint32_t *)GD32H7_CAN_RAM(priv->base) +
                       mb * GD32H7_CAN_MB_WORDS;
  uint32_t mdes0;

  /* Clear the mailbox status flag */

  putreg32(CAN_STAT_MS(mb), GD32H7_CAN_STAT(priv->base));

  /* Return the mailbox to the TX inactive pool */

  mdes0 = mdes[0];
  mdes0 &= ~(0xfu << CAN_MDES0_CODE_SHIFT);
  mdes0 |= (CAN_MB_TX_INACTIVE << CAN_MDES0_CODE_SHIFT);
  mdes[0] = mdes0;

  /* Notify the upper half that the transmit is done */

  can_txdone(priv->dev);
}

/****************************************************************************
 * Name: gd32_can_interrupt
 ****************************************************************************/

static int gd32_can_interrupt(int irq, FAR void *context, FAR void *arg)
{
  FAR struct gd32_can_s *priv = (FAR struct gd32_can_s *)arg;
  uint32_t stat;
  int mb;

  DEBUGASSERT(priv != NULL);

  stat = getreg32(GD32H7_CAN_STAT(priv->base));

  /* Handle Rx FIFO events first */

  if (stat & CAN_STAT_MS5_RFNE)
    {
      gd32_can_rxinterrupt(priv);
      stat = getreg32(GD32H7_CAN_STAT(priv->base));
    }

  /* Handle TX mailbox completion */

  for (mb = 0; mb < CAN_TX_MBOX; mb++)
    {
      if (stat & CAN_STAT_MS(mb))
        {
          gd32_can_txinterrupt(priv, mb);
        }
    }

  return OK;
}

/****************************************************************************
 * Name: can_reset
 ****************************************************************************/

static void can_reset(FAR struct can_dev_s *dev)
{
  FAR struct gd32_can_s *priv = (FAR struct gd32_can_s *)dev->cd_priv;
  uint32_t regval;

  /* Software reset */

  regval = getreg32(GD32H7_CAN_CTL0(priv->base));
  putreg32(regval | CAN_CTL0_SWRST, GD32H7_CAN_CTL0(priv->base));
  while (getreg32(GD32H7_CAN_CTL0(priv->base)) & CAN_CTL0_SWRST)
    {
    }
}

/****************************************************************************
 * Name: can_setup
 ****************************************************************************/

static int can_setup(FAR struct can_dev_s *dev)
{
  FAR struct gd32_can_s *priv = (FAR struct gd32_can_s *)dev->cd_priv;
  uint32_t regval;
  int ret;
  int i;

  /* Software reset */

  can_reset(dev);

  /* Enter inactive mode for configuration */

  ret = gd32_can_enter_inactive(priv->base);
  if (ret < 0)
    {
      return ret;
    }

  /* Disable self reception, configure the mailbox RAM for 32 slots,
   * and enable the Rx FIFO (slot 5).
   */

  regval = getreg32(GD32H7_CAN_CTL0(priv->base));
  regval &= ~CAN_CTL0_MSZ_MASK;
  regval |= (31 << CAN_CTL0_MSZ_SHIFT);
  regval |= CAN_CTL0_SRDIS;
  regval |= CAN_CTL0_RFEN;
  putreg32(regval, GD32H7_CAN_CTL0(priv->base));

  /* Accept all frames in the public filter */

  putreg32(0xffffffff, GD32H7_CAN_RMPUBF(priv->base));

  /* Bit timing */

  ret = gd32_can_bittiming(priv);
  if (ret < 0)
    {
      return ret;
    }

  /* Initialize the TX mailboxes to TX inactive and the Rx FIFO slot to
   * RX empty.
   */

  for (i = 0; i < CAN_TX_MBOX; i++)
    {
      FAR uint32_t *mdes = (FAR uint32_t *)GD32H7_CAN_RAM(priv->base) +
                           i * GD32H7_CAN_MB_WORDS;

      mdes[0] = CAN_MB_TX_INACTIVE << CAN_MDES0_CODE_SHIFT;
      mdes[1] = 0;
      mdes[2] = 0;
      mdes[3] = 0;
      priv->txmb[i] = 0xff;

      /* Clear any pending status */

      putreg32(CAN_STAT_MS(i), GD32H7_CAN_STAT(priv->base));
    }
  {
    FAR uint32_t *fifo = (FAR uint32_t *)GD32H7_CAN_RAM(priv->base) +
                         GD32H7_CAN_FIFO_SLOT * GD32H7_CAN_MB_WORDS;

    fifo[0] = CAN_MB_RX_EMPTY << CAN_MDES0_CODE_SHIFT;
    fifo[1] = 0;
    fifo[2] = 0;
    fifo[3] = 0;

    putreg32(CAN_STAT_MS(GD32H7_CAN_FIFO_SLOT),
             GD32H7_CAN_STAT(priv->base));
  }

  /* Attach the message interrupt */

  {
    int irq;

    switch (priv->port)
      {
        case 0:
          irq = GD32_IRQ_CAN0_MESSAGE;
          break;
        case 1:
          irq = GD32_IRQ_CAN1_MESSAGE;
          break;
        default:
          irq = GD32_IRQ_CAN2_MESSAGE;
          break;
      }

    ret = irq_attach(irq, gd32_can_interrupt, priv);
    if (ret < 0)
      {
        return ret;
      }

    up_enable_irq(irq);
  }

  /* Leave inactive mode */

  ret = gd32_can_exit_inactive(priv->base);
  return ret;
}

/****************************************************************************
 * Name: can_shutdown
 ****************************************************************************/

static void can_shutdown(FAR struct can_dev_s *dev)
{
  FAR struct gd32_can_s *priv = (FAR struct gd32_can_s *)dev->cd_priv;
  int irq;

  /* Disable interrupts */

  switch (priv->port)
    {
      case 0:
        irq = GD32_IRQ_CAN0_MESSAGE;
        break;
      case 1:
        irq = GD32_IRQ_CAN1_MESSAGE;
        break;
      default:
        irq = GD32_IRQ_CAN2_MESSAGE;
        break;
    }

  up_disable_irq(irq);
  irq_detach(irq);

  /* Halt the controller */

  gd32_can_enter_inactive(priv->base);
}

/****************************************************************************
 * Name: can_rxint
 ****************************************************************************/

static void can_rxint(FAR struct can_dev_s *dev, bool enable)
{
  FAR struct gd32_can_s *priv = (FAR struct gd32_can_s *)dev->cd_priv;
  uint32_t regval;

  regval = getreg32(GD32H7_CAN_INTEN(priv->base));

  if (enable)
    {
      regval |= CAN_STAT_MS(GD32H7_CAN_FIFO_SLOT);
    }
  else
    {
      regval &= ~CAN_STAT_MS(GD32H7_CAN_FIFO_SLOT);
    }

  putreg32(regval, GD32H7_CAN_INTEN(priv->base));
}

/****************************************************************************
 * Name: can_txint
 ****************************************************************************/

static void can_txint(FAR struct can_dev_s *dev, bool enable)
{
  FAR struct gd32_can_s *priv = (FAR struct gd32_can_s *)dev->cd_priv;
  uint32_t regval;
  int i;

  regval = getreg32(GD32H7_CAN_INTEN(priv->base));

  for (i = 0; i < CAN_TX_MBOX; i++)
    {
      if (enable)
        {
          regval |= CAN_STAT_MS(i);
        }
      else
        {
          regval &= ~CAN_STAT_MS(i);
        }
    }

  putreg32(regval, GD32H7_CAN_INTEN(priv->base));
}

/****************************************************************************
 * Name: can_ioctl
 ****************************************************************************/

static int can_ioctl(FAR struct can_dev_s *dev, int cmd, unsigned long arg)
{
  FAR struct gd32_can_s *priv = (FAR struct gd32_can_s *)dev->cd_priv;
  int ret = -ENOTTY;

  switch (cmd)
    {
      /* Get the current bit timing */

      case CANIOC_GET_BITTIMING:
        {
          FAR struct canioc_bittiming_s *bt =
            (FAR struct canioc_bittiming_s *)arg;

          if (bt == NULL)
            {
              ret = -EINVAL;
              break;
            }

          bt->bt_baud = priv->baud;
          bt->bt_tseg1 = CAN_PBS1;
          bt->bt_tseg2 = CAN_PBS2;
          bt->bt_sjw   = CAN_SJW;
          ret = OK;
        }
        break;

      /* Change the bit timing */

      case CANIOC_SET_BITTIMING:
        {
          FAR const struct canioc_bittiming_s *bt =
            (FAR const struct canioc_bittiming_s *)arg;

          if (bt == NULL)
            {
              ret = -EINVAL;
              break;
            }

          ret = gd32_can_bittiming_set(priv, bt->bt_baud, bt->bt_tseg1,
                                       bt->bt_tseg2, bt->bt_sjw);
        }
        break;

      /* Loopback / silent mode */

      case CANIOC_GET_CONNMODES:
        {
          FAR struct canioc_connmodes_s *bm =
            (FAR struct canioc_connmodes_s *)arg;

          if (bm == NULL)
            {
              ret = -EINVAL;
              break;
            }

          bm->bm_loopback = priv->loopback;
          bm->bm_silent   = priv->silent;
          ret = OK;
        }
        break;

      case CANIOC_SET_CONNMODES:
        {
          FAR const struct canioc_connmodes_s *bm =
            (FAR const struct canioc_connmodes_s *)arg;

          if (bm == NULL)
            {
              ret = -EINVAL;
              break;
            }

          priv->loopback = bm->bm_loopback;
          priv->silent   = bm->bm_silent;

          if (gd32_can_enter_inactive(priv->base) < 0)
            {
              ret = -ETIMEDOUT;
              break;
            }

          gd32_can_connmode(priv);
          ret = gd32_can_exit_inactive(priv->base);
        }
        break;

      /* Recover from bus-off by clearing the halt/inactive request */

      case CANIOC_BUSOFF_RECOVERY:
        ret = gd32_can_exit_inactive(priv->base);
        break;

      /* Flush: return all TX mailboxes to the inactive state */

      case CANIOC_IFLUSH:
      case CANIOC_OFLUSH:
        {
          int mb;

          for (mb = 0; mb < CAN_TX_MBOX; mb++)
            {
              FAR uint32_t *mdes =
                (FAR uint32_t *)GD32H7_CAN_RAM(priv->base) +
                mb * GD32H7_CAN_MB_WORDS;
              uint32_t mdes0 = mdes[0];

              mdes0 &= ~(0xfu << CAN_MDES0_CODE_SHIFT);
              mdes0 |= (CAN_MB_TX_INACTIVE << CAN_MDES0_CODE_SHIFT);
              mdes[0] = mdes0;

              putreg32(CAN_STAT_MS(mb), GD32H7_CAN_STAT(priv->base));
            }

          ret = OK;
        }
        break;

      default:
        ret = -ENOTTY;
        break;
    }

  return ret;
}

/****************************************************************************
 * Name: can_send
 ****************************************************************************/

static int can_send(FAR struct can_dev_s *dev, FAR struct can_msg_s *msg)
{
  FAR struct gd32_can_s *priv = (FAR struct gd32_can_s *)dev->cd_priv;
  FAR uint32_t *mdes = NULL;
  FAR uint8_t *cf = msg->cm_data;
  uint32_t mdes0 = 0;
  uint32_t mdes1;
  uint32_t data[2] = { 0, 0 };
  int mb;
  int i;

  /* Find a free TX mailbox */

  for (mb = 0; mb < CAN_TX_MBOX; mb++)
    {
      mdes = (FAR uint32_t *)GD32H7_CAN_RAM(priv->base) +
             mb * GD32H7_CAN_MB_WORDS;

      if (((mdes[0] >> CAN_MDES0_CODE_SHIFT) & 0xf) == CAN_MB_TX_INACTIVE)
        {
          break;
        }
    }

  if (mb >= CAN_TX_MBOX)
    {
      return -EBUSY;
    }

  /* Build the descriptor */

  if (msg->cm_hdr.ch_extid)
    {
      mdes0 |= CAN_MDES0_IDE | CAN_MDES0_SRR;
      mdes1 = (msg->cm_hdr.ch_id & CAN_MDES1_ID_EXD_MASK);
    }
  else
    {
      mdes1 = (msg->cm_hdr.ch_id & 0x7ff) << CAN_MDES1_ID_STD_SHIFT;
    }

  if (msg->cm_hdr.ch_rtr)
    {
      mdes0 |= CAN_MDES0_RTR;
    }

  mdes0 |= ((uint32_t)msg->cm_hdr.ch_dlc << CAN_MDES0_DLC_SHIFT);
  mdes0 |= (CAN_MB_TX_DATA << CAN_MDES0_CODE_SHIFT);

  /* Pack data little-endian into two words */

  for (i = 0; i < 4 && i < msg->cm_hdr.ch_dlc; i++)
    {
      data[0] |= (uint32_t)cf[i] << (8 * i);
    }

  for (i = 4; i < 8 && i < msg->cm_hdr.ch_dlc; i++)
    {
      data[1] |= (uint32_t)cf[i] << (8 * (i - 4));
    }

  /* Load the mailbox (data words first, descriptor last so the hardware
   * does not see a half-written message).
   */

  mdes[1] = mdes1;
  mdes[2] = data[0];
  mdes[3] = data[1];
  mdes[0] = mdes0;

  return OK;
}

/****************************************************************************
 * Name: can_txready
 *
 * Description:
 *   Return true if a message can be queued for transmission.
 *
 ****************************************************************************/

static bool can_txready(FAR struct can_dev_s *dev)
{
  FAR struct gd32_can_s *priv = (FAR struct gd32_can_s *)dev->cd_priv;
  FAR uint32_t *mdes;
  int mb;

  for (mb = 0; mb < CAN_TX_MBOX; mb++)
    {
      mdes = (FAR uint32_t *)GD32H7_CAN_RAM(priv->base) +
             mb * GD32H7_CAN_MB_WORDS;

      if (((mdes[0] >> CAN_MDES0_CODE_SHIFT) & 0xf) == CAN_MB_TX_INACTIVE)
        {
          return true;
        }
    }

  return false;
}

/****************************************************************************
 * Name: can_txempty
 ****************************************************************************/

static bool can_txempty(FAR struct can_dev_s *dev)
{
  return can_txready(dev);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_can_initialize
 *
 * Description:
 *   Register the CAN port for use with the NuttX CAN driver.
 *
 * Input Parameters:
 *   port - CAN port number (0..2)
 *
 * Returned Value:
 *   A pointer to the CAN device, or NULL on failure.
 *
 ****************************************************************************/

FAR struct can_dev_s *gd32_can_initialize(int port)
{
  FAR struct gd32_can_s *priv = NULL;
  uint32_t rcu_en;
  int irq;
  int ret;

#ifdef CONFIG_GD32H7_CAN0
  if (port == 0)
    {
      priv = &g_can0priv;
      rcu_en = RCU_CAN0_EN;
    }
  else
#endif
#ifdef CONFIG_GD32H7_CAN1
  if (port == 1)
    {
      priv = &g_can1priv;
      rcu_en = RCU_CAN1_EN;
    }
  else
#endif
#ifdef CONFIG_GD32H7_CAN2
  if (port == 2)
    {
      priv = &g_can2priv;
      rcu_en = RCU_CAN2_EN;
    }
  else
#endif
    {
      /* Unsupported port */

      return NULL;
    }

  /* Allocate the device structure */

  priv->dev = kmm_zalloc(sizeof(struct can_dev_s));
  if (priv->dev == NULL)
    {
      return NULL;
    }

  priv->dev->cd_ops  = &g_canops;
  priv->dev->cd_priv = priv;

  /* Enable the CAN clock */

  gd32_rcu_periph_clock_enable(rcu_en);

  switch (priv->port)
    {
      case 0:
        irq = GD32_IRQ_CAN0_MESSAGE;
        break;
      case 1:
        irq = GD32_IRQ_CAN1_MESSAGE;
        break;
      default:
        irq = GD32_IRQ_CAN2_MESSAGE;
        break;
    }

  /* Register the CAN device */

  ret = can_register("/dev/can0", priv->dev);
  if (ret < 0)
    {
      kmm_free(priv->dev);
      priv->dev = NULL;
      return NULL;
    }

  UNUSED(irq);
  return priv->dev;
}

#endif /* CONFIG_CAN */
