/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_rtc.c
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
#include <time.h>
#include <errno.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>

#include "nvic.h"
#include "arm_internal.h"

#include "gd32h7xx_rtc.h"
#include "gd32h7xx_pmu.h"
#include "gd32h7xx_rcu.h"

#ifdef CONFIG_GD32H7_RTC

/****************************************************************************
 * Public Data
 ****************************************************************************/

/* g_rtc_enabled is set true after the RTC has successfully initialized */

volatile bool g_rtc_enabled = false;

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* The RTC runs from a 32.768 kHz LXTAL crystal; the (async, sync) prescaler
 * pair below divides it to 1 Hz: 32768 / (127 + 1) / (255 + 1) = 1.
 */

#define RTC_ASYNC_PRESCALER           127
#define RTC_SYNC_PRESCALER            255

#define RTC_INIT_TIMEOUT              1000000

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static uint8_t bcd2bin(uint32_t bcd)
{
  return (uint8_t)(((bcd & 0xf0) >> 4) * 10 + (bcd & 0x0f));
}

static uint32_t bin2bcd(uint32_t bin)
{
  uint32_t tens = bin / 10;
  uint32_t units = bin % 10;

  return (tens << 4) | units;
}

/****************************************************************************
 * Name: gd32_rtc_enter_init / gd32_rtc_exit_init
 ****************************************************************************/

static int gd32_rtc_enter_init(void)
{
  volatile uint32_t timeout = RTC_INIT_TIMEOUT;

  /* Unlock write protection */

  putreg32(RTC_UNLOCK_KEY1, GD32H7_RTC_WPK);
  putreg32(RTC_UNLOCK_KEY2, GD32H7_RTC_WPK);

  /* Enter initialization mode */

  modifyreg32(GD32H7_RTC_STAT, 0, RTC_STAT_INITM);

  while (timeout-- > 0)
    {
      if (getreg32(GD32H7_RTC_STAT) & RTC_STAT_INITF)
        {
          return OK;
        }
    }

  return -ETIMEDOUT;
}

static void gd32_rtc_exit_init(void)
{
  modifyreg32(GD32H7_RTC_STAT, RTC_STAT_INITM, 0);
  putreg32(RTC_LOCK_KEY, GD32H7_RTC_WPK);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_rtc_initialize
 *
 * Description:
 *   Enable the LXTAL oscillator and the RTC clock, and configure the
 *   prescalers.  The RTC calendar is preserved across resets (it lives in
 *   the backup domain).
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/

int gd32_rtc_initialize(void)
{
  uint32_t regval;
  int ret;

  gd32_pmu_initialize();

  /* Enable the LXTAL oscillator and wait until it is stable */

  ret = gd32_rcu_lxtal_config();
  if (ret < 0)
    {
      return ret;
    }

  /* Select the RTC clock source (LXTAL) and enable the RTC.  If the RTC
   * was already configured (RTCEN set), leave the calendar untouched.
   */

  regval = getreg32(GD32H7_RCU_BDCTL);
  if ((regval & RCU_BDCTL_RTCEN) == 0)
    {
      /* RTC not running: configure the prescalers */

      ret = gd32_rtc_enter_init();
      if (ret < 0)
        {
          return ret;
        }

      regval = (RTC_SYNC_PRESCALER << RTC_PSC_FACTOR_S_SHIFT) |
               (RTC_ASYNC_PRESCALER << RTC_PSC_FACTOR_A_SHIFT);
      putreg32(regval, GD32H7_RTC_PSC);

      gd32_rtc_exit_init();

      /* Select LXTAL as the RTC clock source and enable the RTC clock */

      gd32_rcu_rtc_clock_config();
    }

  /* Bypass the shadow registers so that TIME/DATE read the calendar
   * directly (avoids the RSynF synchronization wait after wakeups).
   */

  modifyreg32(GD32H7_RTC_CTL, 0, RTC_CTL_BPSHAD);

  g_rtc_enabled = true;
  return OK;
}

/****************************************************************************
 * Name: up_rtc_initialize
 *
 * Description:
 *   Implements the up_rtc_initialize() interface required by the NuttX
 *   clock subsystem.
 *
 ****************************************************************************/

int up_rtc_initialize(void)
{
  return gd32_rtc_initialize();
}

/****************************************************************************
 * Name: up_rtc_getdatetime
 *
 * Description:
 *   Implements the up_rtc_getdatetime() interface required by the NuttX
 *   clock subsystem.
 *
 ****************************************************************************/

int up_rtc_getdatetime(FAR struct tm *tp)
{
  return gd32_rtc_getdatetime(tp);
}

/****************************************************************************
 * Name: gd32_rtc_getdatetime
 *
 * Description:
 *   Read the current calendar date and time.
 *
 * Input Parameters:
 *   tp - The location to return the date and time
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/

int gd32_rtc_getdatetime(FAR struct tm *tp)
{
  uint32_t time;
  uint32_t date;

  /* Read the time and date registers (shadow bypass mode: read time,
   * then date, then re-read time until it does not roll over).
   */

  do
    {
      time = getreg32(GD32H7_RTC_TIME);
      date = getreg32(GD32H7_RTC_DATE);
    }
  while (time != getreg32(GD32H7_RTC_TIME));

  tp->tm_sec  = bcd2bin((time >> RTC_TIME_SCU_SHIFT) & 0x7f);
  tp->tm_min  = bcd2bin((time >> RTC_TIME_MNU_SHIFT) & 0x7f);
  tp->tm_hour = bcd2bin((time >> RTC_TIME_HRU_SHIFT) & 0x3f);
  tp->tm_mday = bcd2bin((date >> RTC_DATE_DAYU_SHIFT) & 0x3f);
  tp->tm_mon  = bcd2bin(((date & RTC_DATE_MONT) >> 4) |
                        ((date >> RTC_DATE_MONU_SHIFT) & 0x0f)) - 1;
  tp->tm_year = bcd2bin((date >> RTC_DATE_YRU_SHIFT) & 0xff) + 100;
  tp->tm_wday = (date >> RTC_DATE_DOW_SHIFT) & 0x07;

  if (tp->tm_wday == 0)
    {
      tp->tm_wday = 7;
    }

  tp->tm_wday--;
  tp->tm_yday = 0;
  tp->tm_isdst = 0;

  return OK;
}

/****************************************************************************
 * Name: gd32_rtc_setdatetime
 *
 * Description:
 *   Set the RTC to the provided date and time.
 *
 * Input Parameters:
 *   tp - The time to set
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/

int gd32_rtc_setdatetime(FAR const struct tm *tp)
{
  uint32_t time;
  uint32_t date;
  int ret;

  ret = gd32_rtc_enter_init();
  if (ret < 0)
    {
      return ret;
    }

  time = ((uint32_t)bin2bcd((uint32_t)tp->tm_sec) << RTC_TIME_SCU_SHIFT) |
         ((uint32_t)bin2bcd((uint32_t)tp->tm_min) << RTC_TIME_MNU_SHIFT) |
         ((uint32_t)bin2bcd((uint32_t)tp->tm_hour) << RTC_TIME_HRU_SHIFT);

  date = ((uint32_t)bin2bcd((uint32_t)(tp->tm_mday)) << RTC_DATE_DAYU_SHIFT) |
         ((uint32_t)bin2bcd((uint32_t)(tp->tm_mon + 1)) << RTC_DATE_MONU_SHIFT) |
         ((uint32_t)bin2bcd((uint32_t)(tp->tm_year - 100)) << RTC_DATE_YRU_SHIFT) |
         ((uint32_t)((tp->tm_wday + 1) & 0x7) << RTC_DATE_DOW_SHIFT);

  putreg32(time, GD32H7_RTC_TIME);
  putreg32(date, GD32H7_RTC_DATE);

  gd32_rtc_exit_init();
  return OK;
}

/****************************************************************************
 * Name: up_rtc_settime
 *
 * Description:
 *   Implements the up_rtc_settime() interface required by the NuttX clock
 *   subsystem.
 *
 ****************************************************************************/

#ifdef CONFIG_RTC_DATETIME
int up_rtc_settime(FAR const struct timespec *ts)
{
  struct tm tm_now;
  time_t sec = ts->tv_sec;

  gmtime_r(&sec, &tm_now);
  return gd32_rtc_setdatetime(&tm_now);
}
#endif

#endif /* CONFIG_GD32H7_RTC */
