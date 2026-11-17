/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_pwm.c
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
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/arch.h>

/* NuttX API compatibility:
 *
 * Newer NuttX always exposes the per-channel duty array in struct
 * pwm_info_s (CONFIG_PWM_NCHANNELS comes from Kconfig).  Older NuttX
 * (e.g. the tree bundled with PX4) only has the array when
 * CONFIG_PWM_MULTICHAN is selected, and otherwise exposes a single
 * info->duty field.  The check has to happen before <nuttx/timers/pwm.h>
 * is included, because that header defines CONFIG_PWM_NCHANNELS itself
 * when the config does not.
 */

#if defined(CONFIG_PWM_MULTICHAN) || defined(CONFIG_PWM_NCHANNELS)
#  define GD32_PWM_HAS_CHANNELS 1
#else
#  define GD32_PWM_HAS_CHANNELS 0
#endif

#include <nuttx/timers/pwm.h>

#include "gd32h7xx_tim.h"
#include "gd32h7xx_pwm.h"

#ifdef CONFIG_GD32H7_PWM

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define PWM_MAX_CHANNELS  4

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct gd32_pwm_s
{
  struct pwm_lowerhalf_s dev;   /* Lower half device (must be first) */
  uint32_t timbase;             /* Timer base address */
  uint8_t  nchannels;           /* Number of output channels */
  uint32_t frequency;           /* Last programmed frequency */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int pwm_setup(FAR struct pwm_lowerhalf_s *dev);
static int pwm_shutdown(FAR struct pwm_lowerhalf_s *dev);
static int pwm_start(FAR struct pwm_lowerhalf_s *dev,
                     FAR const struct pwm_info_s *info);
static int pwm_stop(FAR struct pwm_lowerhalf_s *dev);
static int pwm_ioctl(FAR struct pwm_lowerhalf_s *dev, int cmd,
                     unsigned long arg);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct pwm_ops_s g_pwmops =
{
  .setup    = pwm_setup,
  .shutdown = pwm_shutdown,
  .start    = pwm_start,
  .stop     = pwm_stop,
  .ioctl    = pwm_ioctl,
};

static struct gd32_pwm_s g_pwmdev;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: pwm_setup
 ****************************************************************************/

static int pwm_setup(FAR struct pwm_lowerhalf_s *dev)
{
  FAR struct gd32_pwm_s *priv = (FAR struct gd32_pwm_s *)dev;
  int i;

  gd32_tim_clock_enable(priv->timbase);
  gd32_tim_reset(priv->timbase);

  for (i = 0; i < priv->nchannels; i++)
    {
      gd32_tim_pwm_config(priv->timbase, i);
    }

  return OK;
}

/****************************************************************************
 * Name: pwm_shutdown
 ****************************************************************************/

static int pwm_shutdown(FAR struct pwm_lowerhalf_s *dev)
{
  FAR struct gd32_pwm_s *priv = (FAR struct gd32_pwm_s *)dev;

  gd32_tim_stop(priv->timbase);
  gd32_tim_clock_disable(priv->timbase);
  return OK;
}

/****************************************************************************
 * Name: pwm_start
 *
 * Description:
 *   Compute the prescaler/auto-reload pair for the requested frequency and
 *   program the per-channel duty cycles.
 *
 ****************************************************************************/

static int pwm_start(FAR struct pwm_lowerhalf_s *dev,
                     FAR const struct pwm_info_s *info)
{
  FAR struct gd32_pwm_s *priv = (FAR struct gd32_pwm_s *)dev;
  uint32_t timclk;
  uint32_t psc;
  uint32_t arr;
  uint32_t period;
  uint64_t ticks;
  int i;

  DEBUGASSERT(info != NULL && info->frequency > 0);

  timclk = gd32_tim_getclock(priv->timbase);
  if (timclk == 0)
    {
      return -EINVAL;
    }

  /* Choose the smallest prescaler such that the period fits in 16 bits */

  ticks = (uint64_t)timclk / info->frequency;
  if (ticks == 0)
    {
      return -EINVAL;
    }

  psc = 0;
  while (ticks / (psc + 1) > 0x10000 && psc < 0xffff)
    {
      psc++;
    }

  arr = (uint32_t)(ticks / (psc + 1));
  if (arr == 0)
    {
      arr = 1;
    }

  if (arr > 0xffff)
    {
      arr = 0xffff;
    }

  period = arr + 1;

  gd32_tim_stop(priv->timbase);
  gd32_tim_setperiod(priv->timbase, psc, arr);

  for (i = 0; i < priv->nchannels; i++)
    {
#if GD32_PWM_HAS_CHANNELS
      ub16_t chduty = info->channels[i].duty;
#else
      ub16_t chduty = (i == 0) ? info->duty : 0;
#endif
      uint32_t duty = (uint32_t)(((uint64_t)chduty *
                                  (uint64_t)period) >> 16);

      if (duty > period)
        {
          duty = period;
        }

      gd32_tim_pwm_setduty(priv->timbase, i, duty);
    }

  priv->frequency = info->frequency;
  gd32_tim_start(priv->timbase);
  return OK;
}

/****************************************************************************
 * Name: pwm_stop
 ****************************************************************************/

static int pwm_stop(FAR struct pwm_lowerhalf_s *dev)
{
  FAR struct gd32_pwm_s *priv = (FAR struct gd32_pwm_s *)dev;

  gd32_tim_stop(priv->timbase);
  return OK;
}

/****************************************************************************
 * Name: pwm_ioctl
 ****************************************************************************/

static int pwm_ioctl(FAR struct pwm_lowerhalf_s *dev, int cmd,
                     unsigned long arg)
{
  return -ENOTTY;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_pwm_initialize
 *
 * Description:
 *   Initialize the PWM lower-half on the given timer.
 *
 * Input Parameters:
 *   timbase   - One of GD32H7_TIMERx
 *   nchannels - Number of PWM channels to enable (1..4)
 *
 * Returned Value:
 *   A pointer to the PWM lower-half interface, or NULL on failure.
 *
 ****************************************************************************/

FAR struct pwm_lowerhalf_s *gd32_pwm_initialize(uint32_t timbase,
                                                int nchannels)
{
  FAR struct gd32_pwm_s *priv = &g_pwmdev;

  if (nchannels <= 0 || nchannels > PWM_MAX_CHANNELS)
    {
      return NULL;
    }

  priv->dev.ops    = &g_pwmops;
  priv->timbase    = timbase;
  priv->nchannels  = (uint8_t)nchannels;
  priv->frequency  = 0;

  return &priv->dev;
}

#endif /* CONFIG_GD32H7_PWM */
