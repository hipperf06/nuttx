/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_i2c.c
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

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <debug.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/clock.h>
#include <nuttx/wdog.h>
#include <nuttx/i2c/i2c_master.h>

#include <arch/board/board.h>

#include "chip.h"
#include "gd32h7xx_i2c.h"
#include "gd32h7xx_rcu.h"
#include "gd32h7xx.h"

#ifdef CONFIG_GD32H7_I2C

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#undef HAVE_I2C

#if defined(CONFIG_GD32H7_I2C0) || defined(CONFIG_GD32H7_I2C1) || \
    defined(CONFIG_GD32H7_I2C2) || defined(CONFIG_GD32H7_I2C3)
#  define HAVE_I2C 1
#endif

/* Timeout in loop iterations for waiting on a flag.  Roughly tens of
 * milliseconds at any reasonable core clock.
 */

#define I2C_TIMEOUT_POLL   100000

/* The I2C peripheral clock is CK_APB1. */

#define I2C_PCLK           GD32_PCLK1_FREQUENCY

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct gd32_i2c_priv_s
{
  struct i2c_master_s dev;       /* I2C master device (visible part) */
  uint32_t            base;      /* I2C base address */
  uint32_t            frequency; /* Requested frequency */
  uint32_t            timing;    /* TIMING register value */
  sem_t               semisem;   /* Semaphore to enforce single access */
  mutex_t             lock;      /* Held during a transfer */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int  i2c_transfer(FAR struct i2c_master_s *dev,
                         FAR struct i2c_msg_s *msgs, int count);
static int  i2c_setup(FAR struct i2c_master_s *dev);
static int  i2c_shutdown(FAR struct i2c_master_s *dev);
#ifdef CONFIG_I2C_RESET
static int  i2c_reset(FAR struct i2c_master_s *dev);
#endif

static void gd32_i2c_clock_enable(uint32_t base);
static void gd32_i2c_clock_disable(uint32_t base);
static void gd32_i2c_reset(uint32_t base);
static void gd32_i2c_setfrequency(FAR struct gd32_i2c_priv_s *priv,
                                  uint32_t frequency);
static int  gd32_i2c_waitflags(FAR struct gd32_i2c_priv_s *priv,
                               uint32_t setmask, uint32_t clrmask);
static int  gd32_i2c_waitstatus(FAR struct gd32_i2c_priv_s *priv);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct i2c_ops_s gd32_i2c_ops =
{
  .transfer     = i2c_transfer,
  .setup        = i2c_setup,
  .shutdown     = i2c_shutdown,
#ifdef CONFIG_I2C_RESET
  .reset        = i2c_reset,
#endif
};

#ifdef CONFIG_GD32H7_I2C0
static struct gd32_i2c_priv_s g_i2c0_priv =
{
  .dev     =
  {
    .ops   = &gd32_i2c_ops,
  },
  .base    = GD32H7_I2C0,
  .semisem = SEM_INITIALIZER(1),
  .lock    = NXMUTEX_INITIALIZER,
};
#endif

#ifdef CONFIG_GD32H7_I2C1
static struct gd32_i2c_priv_s g_i2c1_priv =
{
  .dev     =
  {
    .ops   = &gd32_i2c_ops,
  },
  .base    = GD32H7_I2C1,
  .semisem = SEM_INITIALIZER(1),
  .lock    = NXMUTEX_INITIALIZER,
};
#endif

#ifdef CONFIG_GD32H7_I2C2
static struct gd32_i2c_priv_s g_i2c2_priv =
{
  .dev     =
  {
    .ops   = &gd32_i2c_ops,
  },
  .base    = GD32H7_I2C2,
  .semisem = SEM_INITIALIZER(1),
  .lock    = NXMUTEX_INITIALIZER,
};
#endif

#ifdef CONFIG_GD32H7_I2C3
static struct gd32_i2c_priv_s g_i2c3_priv =
{
  .dev     =
  {
    .ops   = &gd32_i2c_ops,
  },
  .base    = GD32H7_I2C3,
  .semisem = SEM_INITIALIZER(1),
  .lock    = NXMUTEX_INITIALIZER,
};
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_i2c_clock_enable / disable / reset
 *
 ****************************************************************************/

static void gd32_i2c_clock_enable(uint32_t base)
{
  switch (base)
    {
      default:
        return;
#ifdef CONFIG_GD32H7_I2C0
      case GD32H7_I2C0:
        gd32_rcu_periph_clock_enable(RCU_I2C0_EN);
        break;
#endif
#ifdef CONFIG_GD32H7_I2C1
      case GD32H7_I2C1:
        gd32_rcu_periph_clock_enable(RCU_I2C1_EN);
        break;
#endif
#ifdef CONFIG_GD32H7_I2C2
      case GD32H7_I2C2:
        gd32_rcu_periph_clock_enable(RCU_I2C2_EN);
        break;
#endif
#ifdef CONFIG_GD32H7_I2C3
      case GD32H7_I2C3:
        gd32_rcu_periph_clock_enable(RCU_I2C3_EN);
        break;
#endif
    }
}

static void gd32_i2c_clock_disable(uint32_t base)
{
  switch (base)
    {
      default:
        return;
#ifdef CONFIG_GD32H7_I2C0
      case GD32H7_I2C0:
        gd32_rcu_periph_clock_disable(RCU_I2C0_EN);
        break;
#endif
#ifdef CONFIG_GD32H7_I2C1
      case GD32H7_I2C1:
        gd32_rcu_periph_clock_disable(RCU_I2C1_EN);
        break;
#endif
#ifdef CONFIG_GD32H7_I2C2
      case GD32H7_I2C2:
        gd32_rcu_periph_clock_disable(RCU_I2C2_EN);
        break;
#endif
#ifdef CONFIG_GD32H7_I2C3
      case GD32H7_I2C3:
        gd32_rcu_periph_clock_disable(RCU_I2C3_EN);
        break;
#endif
    }
}

static void gd32_i2c_reset(uint32_t base)
{
  uint32_t rcu_rst;

  switch (base)
    {
      default:
        return;
#ifdef CONFIG_GD32H7_I2C0
      case GD32H7_I2C0:
        rcu_rst = RCU_I2C0_RST;
        break;
#endif
#ifdef CONFIG_GD32H7_I2C1
      case GD32H7_I2C1:
        rcu_rst = RCU_I2C1_RST;
        break;
#endif
#ifdef CONFIG_GD32H7_I2C2
      case GD32H7_I2C2:
        rcu_rst = RCU_I2C2_RST;
        break;
#endif
#ifdef CONFIG_GD32H7_I2C3
      case GD32H7_I2C3:
        rcu_rst = RCU_I2C3_RST;
        break;
#endif
    }

  gd32_rcu_periph_reset(rcu_rst);
}

/****************************************************************************
 * Name: gd32_i2c_setfrequency
 *
 * Description:
 *   Compute and program the TIMING register for the requested bus speed.
 *
 *   The I2C kernel clock is CK_APB1 divided by (PSC + 1).  The SCL low and
 *   high periods are counted in units of that divided clock:
 *
 *     t_pclk = 1 / I2C_PCLK
 *     t_i2cclk = (PSC + 1) * t_pclk
 *     t_scl_high = (SCLH + 1) * t_i2cclk
 *     t_scl_low  = (SCLL + 1) * t_i2cclk
 *
 *   A simple synchronous design: choose PSC so that t_i2cclk is close to
 *   1us (1MHz kernel clock), then pick SCLH/SCLL for 50% duty at the
 *   requested frequency.
 *
 ****************************************************************************/

static void gd32_i2c_setfrequency(FAR struct gd32_i2c_priv_s *priv,
                                  uint32_t frequency)
{
  uint64_t i2cclk_ns;
  uint32_t psc;
  uint32_t sclh;
  uint32_t scll;
  uint32_t period_ns;
  uint32_t regval;

  DEBUGASSERT(frequency > 0);

  if (frequency == priv->frequency && priv->timing != 0)
    {
      return;
    }

  /* Choose PSC so that (PSC+1) * t_pclk <= 1us (kernel clock ~ 1MHz) */

  psc = (I2C_PCLK / 1000000) - 1;
  if (psc > 15)
    {
      psc = 15;
    }

  i2cclk_ns = ((uint64_t)(psc + 1) * 1000000000ull) / I2C_PCLK;
  if (i2cclk_ns == 0)
    {
      i2cclk_ns = 1;
    }

  /* Total SCL period at 50% duty */

  period_ns = 1000000000U / frequency;
  period_ns /= 2;

  sclh = (uint32_t)(period_ns / i2cclk_ns) - 1;
  scll = sclh;

  if (sclh > 255)
    {
      sclh = 255;
    }

  if (scll > 255)
    {
      scll = 255;
    }

  priv->timing = (I2C_TIMING_PSC(psc) | I2C_TIMING_SCLH(sclh) |
                  I2C_TIMING_SCLL(scll) | I2C_TIMING_SDADELY(1) |
                  I2C_TIMING_SCLDELY(1));
  priv->frequency = frequency;

  i2cinfo("psc=%lu sclh=%lu scll=%lu timing=%08" PRIx32 "\n",
          (unsigned long)psc, (unsigned long)sclh, (unsigned long)scll,
          priv->timing);

  /* Program the TIMING register (valid when I2CEN=0) */

  regval = getreg32(GD32H7_I2C_CTL0(priv->base));
  if (regval & I2C_CTL0_I2CEN)
    {
      regval &= ~I2C_CTL0_I2CEN;
      putreg32(regval, GD32H7_I2C_CTL0(priv->base));
    }

  putreg32(priv->timing, GD32H7_I2C_TIMING(priv->base));
}

/****************************************************************************
 * Name: gd32_i2c_waitflags
 *
 * Description:
 *   Wait until all bits in setmask are set and all bits in clrmask are
 *   clear, or the poll timeout expires.
 *
 * Returned Value:
 *   OK on success, -ETIMEDOUT on timeout.
 *
 ****************************************************************************/

static int gd32_i2c_waitflags(FAR struct gd32_i2c_priv_s *priv,
                              uint32_t setmask, uint32_t clrmask)
{
  volatile uint32_t timeout = I2C_TIMEOUT_POLL;
  uint32_t regval;

  while (timeout-- > 0)
    {
      regval = getreg32(GD32H7_I2C_STAT(priv->base));

      if ((regval & setmask) == setmask &&
          (regval & clrmask) == 0)
        {
          return OK;
        }

      /* Bail out on bus errors */

      if (regval & I2C_STAT_ERR_MASK)
        {
          return -EIO;
        }
    }

  return -ETIMEDOUT;
}

/****************************************************************************
 * Name: gd32_i2c_waitstatus
 *
 * Description:
 *   Wait until the transfer completes (TC set with NACK clear) or an error
 *   occurs.
 *
 ****************************************************************************/

static int gd32_i2c_waitstatus(FAR struct gd32_i2c_priv_s *priv)
{
  return gd32_i2c_waitflags(priv, I2C_STAT_TC, I2C_STAT_NACK);
}

/****************************************************************************
 * Name: i2c_transfer
 *
 * Description:
 *   Perform a sequence of I2C transfers.
 *
 * Input Parameters:
 *   dev   - Device-specific state data
 *   msgs  - A pointer to a set of message descriptors to perform
 *   count - The number of descriptors in the array
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/

static int i2c_transfer(FAR struct i2c_master_s *dev,
                        FAR struct i2c_msg_s *msgs, int count)
{
  FAR struct gd32_i2c_priv_s *priv = (FAR struct gd32_i2c_priv_s *)dev;
  uint32_t startgen = 0;
  uint32_t stopbit = 0;
  uint32_t regval;
  int ret = OK;
  int i;

  DEBUGASSERT(priv != NULL && msgs != NULL && count > 0);

  /* Ensure that address or flags don't change meanwhile */

  ret = nxmutex_lock(&priv->lock);
  if (ret < 0)
    {
      return ret;
    }

  /* Clear any pending error flags from a previous transfer */

  putreg32(I2C_STATC_BERRC | I2C_STATC_LOSTARBC | I2C_STATC_OUERRC |
           I2C_STATC_PECERRC | I2C_STATC_TIMEOUTC | I2C_STATC_NACKC,
           GD32H7_I2C_STATC(priv->base));

  for (i = 0; i < count; i++)
    {
      bool is_last = (i == count - 1);

      /* Wait until the bus is not busy */

      ret = gd32_i2c_waitflags(priv, 0, I2C_STAT_I2CBSY);
      if (ret < 0)
        {
          goto err_with_bus;
        }

      /* Program the slave address and the transfer size */

      regval = getreg32(GD32H7_I2C_CTL1(priv->base));
      regval &= ~(I2C_CTL1_SADDRESS_MASK | I2C_CTL1_BYTENUM_MASK |
                  I2C_CTL1_AUTOEND | I2C_CTL1_RELOAD |
                  I2C_CTL1_START | I2C_CTL1_STOP | I2C_CTL1_TRDIR);
      regval |= I2C_CTL1_SADDRESS(msgs[i].addr << 1);
      regval |= I2C_CTL1_BYTENUM(msgs[i].length);

      if ((msgs[i].flags & I2C_M_READ) != 0)
        {
          regval |= I2C_CTL1_TRDIR;
        }

      /* Use automatic END on the last message */

      if (is_last)
        {
          regval |= I2C_CTL1_AUTOEND;
          stopbit = I2C_CTL1_STOP;
        }

      putreg32(regval, GD32H7_I2C_CTL1(priv->base));

      /* Generate START (only on the first message, or if a repeated start
       * is required after a direction change).
       */

      if (i == 0)
        {
          startgen = 1;
        }
      else if (((msgs[i].flags & I2C_M_READ) !=
                (msgs[i - 1].flags & I2C_M_READ)) ||
               ((msgs[i].flags & I2C_M_NOSTART) == 0 &&
                msgs[i].addr != msgs[i - 1].addr))
        {
          startgen = 1;
        }

      if (startgen)
        {
          regval = getreg32(GD32H7_I2C_CTL1(priv->base));
          regval |= I2C_CTL1_START;
          putreg32(regval, GD32H7_I2C_CTL1(priv->base));
          startgen = 0;
        }

      if ((msgs[i].flags & I2C_M_READ) != 0)
        {
          /* Read data */

          uint32_t n = msgs[i].length;

          while (n > 0)
            {
              ret = gd32_i2c_waitflags(priv, I2C_STAT_RBNE, 0);
              if (ret < 0)
                {
                  goto err_with_bus;
                }

              *msgs[i].buffer++ =
                (uint8_t)(getreg32(GD32H7_I2C_RDATA(priv->base)) &
                          I2C_RDATA_MASK);
              n--;
            }
        }
      else
        {
          /* Write data */

          uint32_t n = msgs[i].length;

          while (n > 0)
            {
              ret = gd32_i2c_waitflags(priv, I2C_STAT_TBE, 0);
              if (ret < 0)
                {
                  goto err_with_bus;
                }

              putreg32(*msgs[i].buffer++ & I2C_TDATA_MASK,
                       GD32H7_I2C_TDATA(priv->base));
              n--;
            }

          /* Wait for the transfer to complete (TC or NACK) */

          ret = gd32_i2c_waitstatus(priv);
          if (ret < 0)
            {
              goto err_with_bus;
            }
        }

      /* For reads, wait for the transfer to complete */

      if ((msgs[i].flags & I2C_M_READ) != 0)
        {
          ret = gd32_i2c_waitstatus(priv);
          if (ret < 0)
            {
              goto err_with_bus;
            }
        }

      /* Check for NACK */

      regval = getreg32(GD32H7_I2C_STAT(priv->base));
      if (regval & I2C_STAT_NACK)
        {
          putreg32(I2C_STATC_NACKC, GD32H7_I2C_STATC(priv->base));
          ret = -ENODEV;
          goto err_with_bus;
        }
    }

err_with_bus:

  /* Generate STOP if the automatic end was not used or on error */

  if (ret < 0)
    {
      putreg32(I2C_CTL1_STOP, GD32H7_I2C_CTL1(priv->base));
      gd32_i2c_waitflags(priv, 0, I2C_STAT_I2CBSY);
    }
  else if (stopbit != 0)
    {
      /* Wait for the STOP condition to complete */

      gd32_i2c_waitflags(priv, 0, I2C_STAT_I2CBSY);
    }

  UNUSED(stopbit);

  nxmutex_unlock(&priv->lock);
  return ret;
}

/****************************************************************************
 * Name: i2c_setup
 *
 * Description:
 *   (Re-)enable the I2C peripheral and its clock.
 *
 ****************************************************************************/

static int i2c_setup(FAR struct i2c_master_s *dev)
{
  FAR struct gd32_i2c_priv_s *priv = (FAR struct gd32_i2c_priv_s *)dev;
  uint32_t regval;

  gd32_i2c_clock_enable(priv->base);

  /* Enable the I2C with the analog noise filter enabled */

  regval = getreg32(GD32H7_I2C_CTL0(priv->base));
  regval &= ~I2C_CTL0_ANOFF;
  regval |= I2C_CTL0_I2CEN;
  putreg32(regval, GD32H7_I2C_CTL0(priv->base));

  return OK;
}

/****************************************************************************
 * Name: i2c_shutdown
 *
 * Description:
 *   Disable the I2C peripheral and its clock.
 *
 ****************************************************************************/

static int i2c_shutdown(FAR struct i2c_master_s *dev)
{
  FAR struct gd32_i2c_priv_s *priv = (FAR struct gd32_i2c_priv_s *)dev;

  /* Disable I2C */

  modifyreg32(GD32H7_I2C_CTL0(priv->base), I2C_CTL0_I2CEN, 0);

  gd32_i2c_clock_disable(priv->base);
  return OK;
}

#ifdef CONFIG_I2C_RESET
/****************************************************************************
 * Name: i2c_reset
 *
 * Description:
 *   Reset the I2C peripheral and bus.
 *
 ****************************************************************************/

static int i2c_reset(FAR struct i2c_master_s *dev)
{
  FAR struct gd32_i2c_priv_s *priv = (FAR struct gd32_i2c_priv_s *)dev;
  uint32_t frequency = priv->frequency;

  if (frequency == 0)
    {
      frequency = 100000;
    }

  /* Disable and re-enable the peripheral, then reprogram the timing */

  modifyreg32(GD32H7_I2C_CTL0(priv->base), I2C_CTL0_I2CEN, 0);

  gd32_i2c_reset(priv->base);
  gd32_i2c_clock_enable(priv->base);

  priv->frequency = 0;   /* Force reprogramming of TIMING */
  gd32_i2c_setfrequency(priv, frequency);

  /* Re-enable the peripheral */

  modifyreg32(GD32H7_I2C_CTL0(priv->base), 0, I2C_CTL0_I2CEN);

  return OK;
}
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_i2cbus_initialize
 *
 * Description:
 *   Initialize one I2C bus.
 *
 ****************************************************************************/

FAR struct i2c_master_s *gd32_i2cbus_initialize(int port)
{
  FAR struct gd32_i2c_priv_s *priv = NULL;
  uint32_t regval;
  int ret;

#ifdef CONFIG_GD32H7_I2C0
  if (port == 0)
    {
      priv = &g_i2c0_priv;
    }
  else
#endif
#ifdef CONFIG_GD32H7_I2C1
  if (port == 1)
    {
      priv = &g_i2c1_priv;
    }
  else
#endif
#ifdef CONFIG_GD32H7_I2C2
  if (port == 2)
    {
      priv = &g_i2c2_priv;
    }
  else
#endif
#ifdef CONFIG_GD32H7_I2C3
  if (port == 3)
    {
      priv = &g_i2c3_priv;
    }
  else
#endif
    {
      i2cerr("ERROR: Unsupported I2C port: %d\n", port);
      return NULL;
    }

  /* Get exclusive access to the I2C */

  ret = nxsem_wait_uninterruptible(&priv->semisem);
  if (ret < 0)
    {
      return NULL;
    }

  /* Enable the I2C clock */

  gd32_i2c_clock_enable(priv->base);

  /* Reset the peripheral */

  gd32_i2c_reset(priv->base);

  /* Configure the timing register for 100kHz */

  priv->frequency = 0;
  gd32_i2c_setfrequency(priv, 100000);

  /* Enable the I2C with the analog noise filter enabled */

  regval = getreg32(GD32H7_I2C_CTL0(priv->base));
  regval &= ~I2C_CTL0_ANOFF;
  regval |= I2C_CTL0_I2CEN;
  putreg32(regval, GD32H7_I2C_CTL0(priv->base));

  /* Release the I2C */

  nxsem_post(&priv->semisem);
  return &priv->dev;
}

/****************************************************************************
 * Name: gd32_i2cbus_uninitialize
 *
 * Description:
 *   Un-initialize one I2C bus.
 *
 ****************************************************************************/

int gd32_i2cbus_uninitialize(FAR struct i2c_master_s *dev)
{
  return i2c_shutdown(dev);
}

#endif /* CONFIG_GD32H7_I2C */
