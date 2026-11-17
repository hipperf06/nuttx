/****************************************************************************
 * boards/arm/gd32h7/gd32h757zm-start/src/gd32h7xx_bringup.c
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

#include <syslog.h>
#include <errno.h>

#include <nuttx/board.h>
#include <arch/board/board.h>

#ifdef CONFIG_ANALOG
#  include <nuttx/analog/adc.h>
#  include <nuttx/analog/dac.h>
#endif
#ifdef CONFIG_PWM
#  include <nuttx/timers/pwm.h>
#endif
#ifdef CONFIG_CAN
#  include <nuttx/can/can.h>
#endif
#ifdef CONFIG_MMCSD
#  include <nuttx/mmcsd.h>
#endif
#ifdef CONFIG_WATCHDOG
#  include <nuttx/timers/watchdog.h>
#endif
#ifdef CONFIG_RTC
#  include <nuttx/timers/rtc.h>
#endif

#include "gd32h7xx.h"

#include "gd32h757zm_start.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_bringup
 *
 * Description:
 *   Perform architecture-specific initialization
 *
 *   CONFIG_BOARD_LATE_INITIALIZE=y :
 *     Called from board_late_initialize().
 *
 ****************************************************************************/

int gd32_bringup(void)
{
  int ret = OK;

#ifdef CONFIG_FS_PROCFS
  /* Mount the procfs file system */

  ret = nx_mount(NULL, GD32_PROCFS_MOUNTPOINT, "procfs", 0, NULL);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to mount procfs at %s: %d\n",
             GD32_PROCFS_MOUNTPOINT, ret);
    }
#endif

#ifdef CONFIG_GD32H7_ADC0
  /* Register ADC0 as /dev/adc0.  The converted channel and sample time are
   * board specific; CHANNEL 5 / 100 cycles are safe defaults that can be
   * overridden through board.h (GD32_ADC0_CHANNEL / GD32_ADC0_SAMPLETIME).
   */

  {
#ifndef GD32_ADC0_CHANNEL
#  define GD32_ADC0_CHANNEL    5
#endif
#ifndef GD32_ADC0_SAMPLETIME
#  define GD32_ADC0_SAMPLETIME 100
#endif

    if (gd32_adc_register(0, GD32_ADC0_CHANNEL,
                          GD32_ADC0_SAMPLETIME) == NULL)
      {
        syslog(LOG_ERR, "ERROR: Failed to register /dev/adc0\n");
      }
  }
#endif

#ifdef CONFIG_GD32H7_DAC
  /* Register DAC channel 0 as /dev/dac0 */

  if (gd32_dac_register("/dev/dac0", 0) == NULL)
    {
      syslog(LOG_ERR, "ERROR: Failed to register /dev/dac0\n");
    }
#endif

#ifdef CONFIG_GD32H7_PWM
  /* Register the PWM timer selected by the board (GD32_PWM_TIMER, default
   * TIMER1) with the number of channels in GD32_PWM_NCHANNELS (default 4).
   */

  {
#ifndef GD32_PWM_TIMER
#  define GD32_PWM_TIMER      GD32H7_TIMER1
#endif
#ifndef GD32_PWM_NCHANNELS
#  define GD32_PWM_NCHANNELS  4
#endif

    FAR struct pwm_lowerhalf_s *pwm;

    pwm = gd32_pwm_initialize(GD32_PWM_TIMER, GD32_PWM_NCHANNELS);
    if (pwm == NULL)
      {
        syslog(LOG_ERR, "ERROR: Failed to initialize PWM\n");
      }
    else
      {
        ret = pwm_register("/dev/pwm0", pwm);
        if (ret < 0)
          {
            syslog(LOG_ERR, "ERROR: pwm_register failed: %d\n", ret);
          }
      }
  }
#endif

#ifdef CONFIG_GD32H7_CAN0
  /* Register CAN0 as /dev/can0 */

  if (gd32_can_initialize(0) == NULL)
    {
      syslog(LOG_ERR, "ERROR: Failed to register /dev/can0\n");
    }
#endif

#ifdef CONFIG_GD32H7_SDIO
  {
    FAR struct sdio_dev_s *sdio;

    sdio = gd32_sdio_initialize(0);
    if (sdio != NULL)
      {
        ret = mmcsd_slotinitialize(0, sdio);
        if (ret < 0)
          {
            syslog(LOG_ERR, "ERROR: mmcsd_slotinitialize failed: %d\n",
                   ret);
          }
      }
  }
#endif

#ifdef CONFIG_GD32H7_FWDGT
  {
    FAR void *wdog;

    wdog = watchdog_register("/dev/watchdog", gd32_fwdgt_initialize());
    if (wdog == NULL)
      {
        syslog(LOG_ERR, "ERROR: Failed to register /dev/watchdog\n");
      }
  }
#endif

  return ret;
}

/****************************************************************************
 * Name: board_app_initialize
 *
 * Description:
 *   Application (boardctl) initialization entry point.  Some NuttX based
 *   stacks (PX4 in particular) call boardctl(BOARDIOC_INIT) instead of the
 *   board_late_initialize() hook, so provide the same bring-up here.
 *
 ****************************************************************************/

#ifdef CONFIG_BOARDCTL
int board_app_initialize(uintptr_t arg)
{
  UNUSED(arg);
  return gd32_bringup();
}
#endif
