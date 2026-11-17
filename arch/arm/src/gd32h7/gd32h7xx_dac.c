/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_dac.c
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
#include <nuttx/analog/dac.h>
#include <nuttx/analog/ioctl.h>

#include "arm_internal.h"

#include "gd32h7xx_dac.h"
#include "gd32h7xx_rcu.h"

#ifdef CONFIG_GD32H7_DAC

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_dac_initialize
 *
 * Description:
 *   Enable the DAC clock and one output channel in software-triggered,
 *   buffered mode.
 *
 * Input Parameters:
 *   channel - DAC output channel (0 or 1)
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/

int gd32_dac_initialize(int channel)
{
  uint32_t regval;

  DEBUGASSERT(channel == 0 || channel == 1);

  gd32_rcu_periph_clock_enable(RCU_DAC_EN);

  regval = getreg32(GD32H7_DAC_CTL0(GD32H7_DAC0));

  if (channel == 0)
    {
      regval |= DAC_CTL0_DEN0;
      regval &= ~DAC_CTL0_DTEN0;
    }
  else
    {
      regval |= DAC_CTL0_DEN1;
      regval &= ~DAC_CTL0_DTEN1;
    }

  putreg32(regval, GD32H7_DAC_CTL0(GD32H7_DAC0));
  return OK;
}

/****************************************************************************
 * Name: gd32_dac_setvalue
 *
 * Description:
 *   Write a 12-bit value to the given DAC output.
 *
 ****************************************************************************/

int gd32_dac_setvalue(int channel, uint16_t value)
{
  DEBUGASSERT(channel == 0 || channel == 1);

  if (channel == 0)
    {
      putreg32(value & 0x0fff, GD32H7_DAC_OUT0_R12DH(GD32H7_DAC0));
      putreg32(DAC_SWT_SWTR0, GD32H7_DAC_SWT(GD32H7_DAC0));
    }
  else
    {
      putreg32(value & 0x0fff, GD32H7_DAC_OUT1_R12DH(GD32H7_DAC0));
      putreg32(DAC_SWT_SWTR1, GD32H7_DAC_SWT(GD32H7_DAC0));
    }

  return OK;
}

/****************************************************************************
 * Name: gd32_dac_getvalue
 *
 * Description:
 *   Read back the current output value of the given DAC channel.
 *
 ****************************************************************************/

uint16_t gd32_dac_getvalue(int channel)
{
  DEBUGASSERT(channel == 0 || channel == 1);

  if (channel == 0)
    {
      return (uint16_t)(getreg32(GD32H7_DAC_OUT0_DO(GD32H7_DAC0)) & 0x0fff);
    }
  else
    {
      return (uint16_t)(getreg32(GD32H7_DAC_OUT1_DO(GD32H7_DAC0)) & 0x0fff);
    }
}

#ifdef CONFIG_DAC

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct gd32_dacdev_s
{
  struct dac_dev_s dev;       /* NuttX DAC device (must be first) */
  uint8_t channel;            /* DAC channel (0 or 1) */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void dac_reset(FAR struct dac_dev_s *dev);
static int  dac_setup(FAR struct dac_dev_s *dev);
static void dac_shutdown(FAR struct dac_dev_s *dev);
static void dac_txint(FAR struct dac_dev_s *dev, bool enable);
static int  dac_send(FAR struct dac_dev_s *dev, FAR struct dac_msg_s *msg);
static int  dac_ioctl(FAR struct dac_dev_s *dev, int cmd,
                      unsigned long arg);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct dac_ops_s g_dacops =
{
  .ao_reset    = dac_reset,
  .ao_setup    = dac_setup,
  .ao_shutdown = dac_shutdown,
  .ao_txint    = dac_txint,
  .ao_send     = dac_send,
  .ao_ioctl    = dac_ioctl,
};

static struct gd32_dacdev_s g_dacdev =
{
  .dev = { .ad_ops = &g_dacops, },
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void dac_reset(FAR struct dac_dev_s *dev)
{
  FAR struct gd32_dacdev_s *priv = (FAR struct gd32_dacdev_s *)dev;

  gd32_dac_initialize(priv->channel);
}

static int dac_setup(FAR struct dac_dev_s *dev)
{
  FAR struct gd32_dacdev_s *priv = (FAR struct gd32_dacdev_s *)dev;

  return gd32_dac_initialize(priv->channel);
}

static void dac_shutdown(FAR struct dac_dev_s *dev)
{
  FAR struct gd32_dacdev_s *priv = (FAR struct gd32_dacdev_s *)dev;

  if (priv->channel == 0)
    {
      modifyreg32(GD32H7_DAC_CTL0(GD32H7_DAC0), DAC_CTL0_DEN0, 0);
    }
  else
    {
      modifyreg32(GD32H7_DAC_CTL0(GD32H7_DAC0), DAC_CTL0_DEN1, 0);
    }
}

static void dac_txint(FAR struct dac_dev_s *dev, bool enable)
{
  /* No DMA underrun interrupt support in this simple driver */

  UNUSED(dev);
  UNUSED(enable);
}

static int dac_send(FAR struct dac_dev_s *dev, FAR struct dac_msg_s *msg)
{
  FAR struct gd32_dacdev_s *priv = (FAR struct gd32_dacdev_s *)dev;

  DEBUGASSERT(msg->am_channel == 0);

  gd32_dac_setvalue(priv->channel, (uint16_t)(msg->am_data & 0x0fff));

  /* The value is applied immediately, so acknowledge the message */

  dac_txdone(dev);
  return OK;
}

static int dac_ioctl(FAR struct dac_dev_s *dev, int cmd, unsigned long arg)
{
  return -ENOTTY;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_dac_register
 *
 * Description:
 *   Register one DAC channel as a NuttX analog device.
 *
 * Input Parameters:
 *   path    - Device path (e.g. "/dev/dac0")
 *   channel - DAC output channel (0 or 1)
 *
 * Returned Value:
 *   Pointer to the DAC device on success; NULL on failure.
 *
 ****************************************************************************/

FAR struct dac_dev_s *gd32_dac_register(FAR const char *path, int channel)
{
  FAR struct gd32_dacdev_s *priv = &g_dacdev;
  int ret;

  priv->channel = (uint8_t)channel;

  ret = gd32_dac_initialize(channel);
  if (ret < 0)
    {
      return NULL;
    }

  ret = dac_register(path, &priv->dev);
  if (ret < 0)
  {
    return NULL;
  }

  return &priv->dev;
}

#endif /* CONFIG_DAC */

#endif /* CONFIG_GD32H7_DAC */
