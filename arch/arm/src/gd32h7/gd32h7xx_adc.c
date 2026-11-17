/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_adc.c
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
#include <errno.h>
#include <assert.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/analog/adc.h>
#include <nuttx/analog/ioctl.h>

#include "arm_internal.h"

#include "gd32h7xx_adc.h"
#include "gd32h7xx_rcu.h"

#ifdef CONFIG_GD32H7_ADC

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define ADC_TIMEOUT       1000000

/* ADC kernel clock (CKADC) - the ADC clock is derived from the AHB bus
 * through the ADC prescaler configured in the board clock setup.  The port
 * currently runs the ADC from the APB2 clock domain divider set by the
 * board; adjust this if the board clock configuration changes.
 */

#ifndef ADC0_KERNEL_CLOCK
#  define ADC0_KERNEL_CLOCK  (GD32_PCLK2_FREQUENCY / 4)
#endif

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const uint32_t g_adcrcu[] =
{
#ifdef CONFIG_GD32H7_ADC0
  RCU_ADC0_EN,
#else
  0,
#endif
#ifdef CONFIG_GD32H7_ADC1
  RCU_ADC1_EN,
#else
  0,
#endif
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static FAR uint32_t gd32_adc_getbase(int port)
{
  switch (port)
    {
      case 0:
        return GD32H7_ADC0;
      case 1:
        return GD32H7_ADC1;
      case 2:
        return GD32H7_ADC2;
      default:
        return 0;
    }
}

static uint32_t gd32_adc_getrcu(int port)
{
  if (port < (int)(sizeof(g_adcrcu) / sizeof(g_adcrcu[0])))
    {
      return g_adcrcu[port];
    }

  return 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_adc_initialize
 *
 * Description:
 *   Enable the clock and power up one ADC.
 *
 * Input Parameters:
 *   port - ADC number (0..2)
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/

int gd32_adc_initialize(int port)
{
  uint32_t base = gd32_adc_getbase(port);
  uint32_t rcu;
  uint32_t regval;

  if (base == 0)
    {
      return -EINVAL;
    }

  rcu = gd32_adc_getrcu(port);
  if (rcu == 0)
    {
      return -EINVAL;
    }

  gd32_rcu_periph_clock_enable(rcu);

  /* Reset calibration and enable the converter */

  regval = getreg32(GD32H7_ADC_CTL1(base));
  regval |= ADC_CTL1_RSTCLB;
  putreg32(regval, GD32H7_ADC_CTL1(base));

  regval = getreg32(GD32H7_ADC_CTL1(base));
  regval |= ADC_CTL1_ADCON;
  putreg32(regval, GD32H7_ADC_CTL1(base));

  return OK;
}

/****************************************************************************
 * Name: gd32_adc_read_channel
 *
 * Description:
 *   Perform one single conversion on the given channel.
 *
 * Input Parameters:
 *   port    - ADC number (0..2)
 *   channel - ADC channel number (0..20)
 *   smp     - Sample time in ADC clock cycles (0..809)
 *
 * Returned Value:
 *   The conversion result (>= 0), or a negated errno value on failure.
 *
 ****************************************************************************/

int gd32_adc_read_channel(int port, uint8_t channel, uint32_t smp)
{
  uint32_t base = gd32_adc_getbase(port);
  volatile uint32_t timeout = ADC_TIMEOUT;
  uint32_t regval;

  if (base == 0 || channel > 20 || smp > 809)
    {
      return -EINVAL;
    }

  /* Program a single-rank routine sequence (RL = 1 - 1 = 0) with the
   * requested channel and sample time in slot 0 of RSQ8.
   */

  regval = getreg32(GD32H7_ADC_RSQ0(base));
  regval &= ~ADC_RSQ0_RL_MASK;
  regval |= (0 << ADC_RSQ0_RL_SHIFT);
  putreg32(regval, GD32H7_ADC_RSQ0(base));

  putreg32(ADC_RSQ_SLOT(channel, smp), GD32H7_ADC_RSQ8(base));

  /* Start the conversion */

  regval = getreg32(GD32H7_ADC_CTL1(base));
  regval &= ~ADC_CTL1_CTN;
  regval |= ADC_CTL1_SWRCST;
  putreg32(regval, GD32H7_ADC_CTL1(base));

  /* Wait for end of conversion */

  while (timeout-- > 0)
    {
      regval = getreg32(GD32H7_ADC_STAT(base));

      if (regval & ADC_STAT_EOC)
        {
          uint32_t data = getreg32(GD32H7_ADC_RDATA(base));

          /* Clear the flag by writing zero to it */

          regval = getreg32(GD32H7_ADC_STAT(base));
          regval &= ~ADC_STAT_EOC;
          putreg32(regval, GD32H7_ADC_STAT(base));

          return (int)(data & 0x3fff);
        }
    }

  return -ETIMEDOUT;
}

#ifdef CONFIG_ADC

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct gd32_adcdev_s
{
  struct adc_dev_s dev;       /* NuttX ADC device (must be first) */
  FAR const struct adc_callback_s *cb;  /* Upper-half callbacks */
  uint32_t base;              /* ADC base address */
  uint8_t  channel;           /* Channel converted by this instance */
  uint32_t smp;               /* Sample time in ADC clock cycles */
  int      irq;               /* Interrupt number */
  bool     started;           /* Continuous conversion running */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int  adc_bind(FAR struct adc_dev_s *dev,
                     FAR const struct adc_callback_s *callback);
static void adc_reset(FAR struct adc_dev_s *dev);
static int  adc_setup(FAR struct adc_dev_s *dev);
static void adc_shutdown(FAR struct adc_dev_s *dev);
static void adc_rxint(FAR struct adc_dev_s *dev, bool enable);
static int  adc_ioctl(FAR struct adc_dev_s *dev, int cmd,
                      unsigned long arg);
static int  adc_interrupt(int irq, FAR void *context, FAR void *arg);

static void adc_start_conversion(FAR struct gd32_adcdev_s *priv);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct adc_ops_s g_adcops =
{
  .ao_bind     = adc_bind,
  .ao_reset    = adc_reset,
  .ao_setup    = adc_setup,
  .ao_shutdown = adc_shutdown,
  .ao_rxint    = adc_rxint,
  .ao_ioctl    = adc_ioctl,
};

#ifdef CONFIG_GD32H7_ADC0
static struct gd32_adcdev_s g_adc0dev =
{
  .dev = { .ad_ops = &g_adcops, },
  .base = GD32H7_ADC0,
  .irq  = GD32_IRQ_ADC0_1,
};
#endif

#ifdef CONFIG_GD32H7_ADC1
static struct gd32_adcdev_s g_adc1dev =
{
  .dev = { .ad_ops = &g_adcops, },
  .base = GD32H7_ADC1,
  .irq  = GD32_IRQ_ADC0_1,
};
#endif

#ifdef CONFIG_GD32H7_ADC2
static struct gd32_adcdev_s g_adc2dev =
{
  .dev = { .ad_ops = &g_adcops, },
  .base = GD32H7_ADC2,
  .irq  = GD32_IRQ_ADC2,
};
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: adc_start_conversion
 *
 * Description:
 *   Program a single-rank routine sequence and start a continuous
 *   conversion on the configured channel.
 *
 ****************************************************************************/

static void adc_start_conversion(FAR struct gd32_adcdev_s *priv)
{
  uint32_t regval;

  /* RL = 0 (one conversion), slot 0 in RSQ8 */

  regval = getreg32(GD32H7_ADC_RSQ0(priv->base));
  regval &= ~ADC_RSQ0_RL_MASK;
  putreg32(regval, GD32H7_ADC_RSQ0(priv->base));

  putreg32(ADC_RSQ_SLOT(priv->channel, priv->smp),
           GD32H7_ADC_RSQ8(priv->base));

  /* Enable end-of-conversion interrupt and start continuous conversion */

  modifyreg32(GD32H7_ADC_CTL0(priv->base), 0, ADC_CTL0_EOCIE);

  regval = getreg32(GD32H7_ADC_CTL1(priv->base));
  regval |= (ADC_CTL1_ADCON | ADC_CTL1_CTN | ADC_CTL1_SWRCST);
  putreg32(regval, GD32H7_ADC_CTL1(priv->base));

  priv->started = true;
}

/****************************************************************************
 * Name: adc_bind
 ****************************************************************************/

static int adc_bind(FAR struct adc_dev_s *dev,
                    FAR const struct adc_callback_s *callback)
{
  FAR struct gd32_adcdev_s *priv = (FAR struct gd32_adcdev_s *)dev;

  DEBUGASSERT(priv != NULL);
  priv->cb = callback;
  return OK;
}

/****************************************************************************
 * Name: adc_reset
 ****************************************************************************/

static void adc_reset(FAR struct adc_dev_s *dev)
{
  FAR struct gd32_adcdev_s *priv = (FAR struct gd32_adcdev_s *)dev;

  adc_shutdown(dev);
  gd32_adc_initialize(priv->base == GD32H7_ADC0 ? 0 :
                      priv->base == GD32H7_ADC1 ? 1 : 2);
}

/****************************************************************************
 * Name: adc_setup
 ****************************************************************************/

static int adc_setup(FAR struct adc_dev_s *dev)
{
  FAR struct gd32_adcdev_s *priv = (FAR struct gd32_adcdev_s *)dev;
  int ret;

  ret = irq_attach(priv->irq, adc_interrupt, priv);
  if (ret < 0)
    {
      return ret;
    }

  up_enable_irq(priv->irq);

  adc_start_conversion(priv);
  return OK;
}

/****************************************************************************
 * Name: adc_shutdown
 ****************************************************************************/

static void adc_shutdown(FAR struct adc_dev_s *dev)
{
  FAR struct gd32_adcdev_s *priv = (FAR struct gd32_adcdev_s *)dev;

  /* Stop continuous conversion and disable the interrupt */

  modifyreg32(GD32H7_ADC_CTL1(priv->base),
              ADC_CTL1_CTN | ADC_CTL1_SWRCST, 0);
  modifyreg32(GD32H7_ADC_CTL0(priv->base), ADC_CTL0_EOCIE, 0);
  modifyreg32(GD32H7_ADC_CTL1(priv->base), ADC_CTL1_ADCON, 0);

  up_disable_irq(priv->irq);
  irq_detach(priv->irq);

  priv->started = false;
}

/****************************************************************************
 * Name: adc_rxint
 ****************************************************************************/

static void adc_rxint(FAR struct adc_dev_s *dev, bool enable)
{
  FAR struct gd32_adcdev_s *priv = (FAR struct gd32_adcdev_s *)dev;

  if (enable)
    {
      modifyreg32(GD32H7_ADC_CTL0(priv->base), 0, ADC_CTL0_EOCIE);
    }
  else
    {
      modifyreg32(GD32H7_ADC_CTL0(priv->base), ADC_CTL0_EOCIE, 0);
    }
}

/****************************************************************************
 * Name: adc_ioctl
 ****************************************************************************/

static int adc_ioctl(FAR struct adc_dev_s *dev, int cmd, unsigned long arg)
{
  FAR struct gd32_adcdev_s *priv = (FAR struct gd32_adcdev_s *)dev;

  switch (cmd)
    {
      case ANIOC_GET_NCHANNELS:
        return 1;

      case ANIOC_TRIGGER:
        adc_start_conversion(priv);
        return OK;

      default:
        return -ENOTTY;
    }
}

/****************************************************************************
 * Name: adc_interrupt
 ****************************************************************************/

static int adc_interrupt(int irq, FAR void *context, FAR void *arg)
{
  FAR struct gd32_adcdev_s *priv = (FAR struct gd32_adcdev_s *)arg;
  uint32_t stat;
  uint32_t data;

  DEBUGASSERT(priv != NULL);

  stat = getreg32(GD32H7_ADC_STAT(priv->base));
  if ((stat & ADC_STAT_EOC) == 0)
    {
      return OK;
    }

  data = getreg32(GD32H7_ADC_RDATA(priv->base)) & 0x3fff;

  /* Clear EOC by writing zero to it */

  putreg32(stat & ~ADC_STAT_EOC, GD32H7_ADC_STAT(priv->base));

  if (priv->cb != NULL && priv->cb->au_receive != NULL)
    {
      priv->cb->au_receive(&priv->dev, priv->channel, (int32_t)data);
    }

  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_adc_register
 *
 * Description:
 *   Register one ADC as a NuttX analog device (/dev/adcN).
 *
 * Input Parameters:
 *   port    - ADC number (0..2)
 *   channel - ADC input channel to convert (0..20)
 *   smp     - Sample time in ADC clock cycles (1..809)
 *
 * Returned Value:
 *   Pointer to the ADC device on success; NULL on failure.
 *
 ****************************************************************************/

FAR struct adc_dev_s *gd32_adc_register(int port, uint8_t channel,
                                        uint32_t smp)
{
  FAR struct gd32_adcdev_s *priv;
  char path[16];
  int ret;

  switch (port)
    {
#ifdef CONFIG_GD32H7_ADC0
      case 0:
        priv = &g_adc0dev;
        break;
#endif
#ifdef CONFIG_GD32H7_ADC1
      case 1:
        priv = &g_adc1dev;
        break;
#endif
#ifdef CONFIG_GD32H7_ADC2
      case 2:
        priv = &g_adc2dev;
        break;
#endif
      default:
        return NULL;
    }

  priv->channel = channel;
  priv->smp     = smp;

  ret = gd32_adc_initialize(port);
  if (ret < 0)
    {
      return NULL;
    }

  snprintf(path, sizeof(path), "/dev/adc%d", port);

  ret = adc_register(path, &priv->dev);
  if (ret < 0)
    {
      return NULL;
    }

  return &priv->dev;
}

#endif /* CONFIG_ADC */

#endif /* CONFIG_GD32H7_ADC */
