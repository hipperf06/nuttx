/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_wdg.c
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
#include <assert.h>
#include <errno.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/timers/watchdog.h>

#include "arm_internal.h"

#include "hardware/gd32h7xx_wdg.h"
#include "gd32h7xx_wdg.h"

#ifdef CONFIG_GD32H7_FWDGT

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* The free watchdog runs from the internal IRC40K oscillator (40 kHz). */

#define FWDGT_IRC40K        40000

/* Maximum reload value of the 12-bit down counter */

#define FWDGT_RLD_MAX       0x0fff

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct gd32_wdg_s
{
  struct watchdog_lowerhalf_s dev;    /* Lower half device (first) */
  uint32_t timeout_ms;                /* Current timeout in milliseconds */
  bool     started;                   /* Timer is running */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int      gd32_start(FAR struct watchdog_lowerhalf_s *lower);
static int      gd32_stop(FAR struct watchdog_lowerhalf_s *lower);
static int      gd32_keepalive(FAR struct watchdog_lowerhalf_s *lower);
static int      gd32_getstatus(FAR struct watchdog_lowerhalf_s *lower,
                               FAR struct watchdog_status_s *status);
static int      gd32_settimeout(FAR struct watchdog_lowerhalf_s *lower,
                                uint32_t timeout);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct watchdog_ops_s g_wdgops =
{
  .start      = gd32_start,
  .stop       = gd32_stop,
  .keepalive  = gd32_keepalive,
  .getstatus  = gd32_getstatus,
  .settimeout = gd32_settimeout,
};

static struct gd32_wdg_s g_wdgdev;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_wdg_configure
 *
 * Description:
 *   Select the prescaler and reload value that best match the requested
 *   timeout.
 *
 ****************************************************************************/

static void gd32_wdg_configure(FAR struct gd32_wdg_s *priv)
{
  static const uint8_t psc[] =
  {
    FWDGT_PSC_DIV4, FWDGT_PSC_DIV8, FWDGT_PSC_DIV16, FWDGT_PSC_DIV32,
    FWDGT_PSC_DIV64, FWDGT_PSC_DIV128, FWDGT_PSC_DIV256
  };

  static const uint32_t div[] = { 4, 8, 16, 32, 64, 128, 256 };

  uint32_t rld = FWDGT_RLD_MAX;
  uint32_t pscval = psc[6];
  int i;

  for (i = 0; i < 7; i++)
    {
      uint64_t ticks = ((uint64_t)FWDGT_IRC40K * priv->timeout_ms) /
                        (1000ull * div[i]);

      if (ticks <= FWDGT_RLD_MAX)
        {
          rld = (uint32_t)ticks;
          pscval = psc[i];
          break;
        }
    }

  if (rld == 0)
    {
      rld = 1;
    }

  /* Unlock the watchdog, program prescaler and reload, refresh counter */

  putreg32(FWDGT_CTL_CMD_ENABLE, GD32H7_FWDGT_CTL);
  putreg32(pscval, GD32H7_FWDGT_PSC);
  putreg32(rld & FWDGT_RLD_MASK, GD32H7_FWDGT_RLD);
  putreg32(FWDGT_CTL_CMD_RELOAD, GD32H7_FWDGT_CTL);
}

/****************************************************************************
 * Name: gd32_start
 ****************************************************************************/

static int gd32_start(FAR struct watchdog_lowerhalf_s *lower)
{
  FAR struct gd32_wdg_s *priv = (FAR struct gd32_wdg_s *)lower;

  gd32_wdg_configure(priv);
  putreg32(FWDGT_CTL_CMD_START, GD32H7_FWDGT_CTL);
  priv->started = true;
  return OK;
}

/****************************************************************************
 * Name: gd32_stop
 ****************************************************************************/

static int gd32_stop(FAR struct watchdog_lowerhalf_s *lower)
{
  FAR struct gd32_wdg_s *priv = (FAR struct gd32_wdg_s *)lower;

  /* The free watchdog cannot be stopped once started.  Freeze it in debug
   * mode only; the best we can do from software is to keep it alive.
   */

  priv->started = false;
  putreg32(FWDGT_CTL_CMD_RELOAD, GD32H7_FWDGT_CTL);
  return OK;
}

/****************************************************************************
 * Name: gd32_keepalive
 ****************************************************************************/

static int gd32_keepalive(FAR struct watchdog_lowerhalf_s *lower)
{
  putreg32(FWDGT_CTL_CMD_RELOAD, GD32H7_FWDGT_CTL);
  return OK;
}

/****************************************************************************
 * Name: gd32_getstatus
 ****************************************************************************/

static int gd32_getstatus(FAR struct watchdog_lowerhalf_s *lower,
                          FAR struct watchdog_status_s *status)
{
  FAR struct gd32_wdg_s *priv = (FAR struct gd32_wdg_s *)lower;

  status->flags = priv->started ? WDFLAGS_ACTIVE : 0;
  status->timeout = priv->timeout_ms;
  status->timeleft = priv->timeout_ms;
  return OK;
}

/****************************************************************************
 * Name: gd32_settimeout
 ****************************************************************************/

static int gd32_settimeout(FAR struct watchdog_lowerhalf_s *lower,
                           uint32_t timeout)
{
  FAR struct gd32_wdg_s *priv = (FAR struct gd32_wdg_s *)lower;

  if (timeout == 0)
    {
      return -EINVAL;
    }

  priv->timeout_ms = timeout;

  if (priv->started)
    {
      gd32_wdg_configure(priv);
      putreg32(FWDGT_CTL_CMD_RELOAD, GD32H7_FWDGT_CTL);
    }

  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_fwdgt_initialize
 *
 * Description:
 *   Initialize the free watchdog timer lower half.
 *
 * Returned Value:
 *   A pointer to the watchdog lower-half interface, or NULL on failure.
 *
 ****************************************************************************/

FAR struct watchdog_lowerhalf_s *gd32_fwdgt_initialize(void)
{
  FAR struct gd32_wdg_s *priv = &g_wdgdev;

  priv->dev.ops    = &g_wdgops;
  priv->timeout_ms = 1000;
  priv->started    = false;

  return &priv->dev;
}

#endif /* CONFIG_GD32H7_FWDGT */
