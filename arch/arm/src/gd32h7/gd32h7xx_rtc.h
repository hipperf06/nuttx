/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_rtc.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_GD32H7XX_RTC_H
#define __ARCH_ARM_SRC_GD32H7_GD32H7XX_RTC_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "hardware/gd32h73x_memorymap.h"
#include "gd32h7xx_rcu.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define GD32H7_RTC                    GD32H7_RTC_BASE

/* Register offsets ------------------------------------------------------ */

#define GD32H7_RTC_TIME_OFFSET        0x0000  /* Time register */
#define GD32H7_RTC_DATE_OFFSET        0x0004  /* Date register */
#define GD32H7_RTC_CTL_OFFSET         0x0008  /* Control register */
#define GD32H7_RTC_STAT_OFFSET        0x000c  /* Status register */
#define GD32H7_RTC_PSC_OFFSET         0x0010  /* Prescaler register */
#define GD32H7_RTC_WUT_OFFSET         0x0014  /* Wakeup timer */
#define GD32H7_RTC_ALRM0TD_OFFSET     0x001c  /* Alarm 0 time/date */
#define GD32H7_RTC_WPK_OFFSET         0x0024  /* Write protection key */

/* Register addresses ---------------------------------------------------- */

#define GD32H7_RTC_TIME               (GD32H7_RTC + GD32H7_RTC_TIME_OFFSET)
#define GD32H7_RTC_DATE               (GD32H7_RTC + GD32H7_RTC_DATE_OFFSET)
#define GD32H7_RTC_CTL                (GD32H7_RTC + GD32H7_RTC_CTL_OFFSET)
#define GD32H7_RTC_STAT               (GD32H7_RTC + GD32H7_RTC_STAT_OFFSET)
#define GD32H7_RTC_PSC                (GD32H7_RTC + GD32H7_RTC_PSC_OFFSET)
#define GD32H7_RTC_WUT                (GD32H7_RTC + GD32H7_RTC_WUT_OFFSET)
#define GD32H7_RTC_ALRM0TD            (GD32H7_RTC + GD32H7_RTC_ALRM0TD_OFFSET)
#define GD32H7_RTC_WPK                (GD32H7_RTC + GD32H7_RTC_WPK_OFFSET)

/* TIME (BCD) */

#define RTC_TIME_SCU_SHIFT            (0)      /* Seconds units */
#define RTC_TIME_SCT_SHIFT            (4)      /* Seconds tens */
#define RTC_TIME_MNU_SHIFT            (8)      /* Minutes units */
#define RTC_TIME_MNT_SHIFT            (12)     /* Minutes tens */
#define RTC_TIME_HRU_SHIFT            (16)     /* Hours units */
#define RTC_TIME_HRT_SHIFT            (20)     /* Hours tens */
#define RTC_TIME_PM                   (1 << 22)

/* DATE (BCD) */

#define RTC_DATE_DAYU_SHIFT           (0)      /* Date units */
#define RTC_DATE_DAYT_SHIFT           (4)      /* Date tens */
#define RTC_DATE_MONU_SHIFT           (8)      /* Month units */
#define RTC_DATE_MONT                 (1 << 12)
#define RTC_DATE_DOW_SHIFT            (13)     /* Day of week */
#define RTC_DATE_YRU_SHIFT            (16)     /* Year units */
#define RTC_DATE_YRT_SHIFT            (20)     /* Year tens */

/* CTL */

#define RTC_CTL_BPSHAD                (1 << 5)  /* Bypass shadow registers */
#define RTC_CTL_ALRM0EN               (1 << 8)  /* Alarm 0 enable */
#define RTC_CTL_ALRM0IE               (1 << 12) /* Alarm 0 interrupt enable */

/* STAT */

#define RTC_STAT_ALRM0WF              (1 << 0)  /* Alarm 0 write flag */
#define RTC_STAT_RSYNF                (1 << 5)  /* Registers synchronized */
#define RTC_STAT_INITF                (1 << 6)  /* Initialization flag */
#define RTC_STAT_INITM                (1 << 7)  /* Initialization mode */
#define RTC_STAT_ALRM0F               (1 << 8)  /* Alarm 0 flag */

/* PSC */

#define RTC_PSC_FACTOR_S_SHIFT        (0)       /* Sync prescaler (0..0x7fff) */
#define RTC_PSC_FACTOR_A_SHIFT        (16)      /* Async prescaler (0..0x7f) */

/* Write protection keys */

#define RTC_UNLOCK_KEY1               0xca
#define RTC_UNLOCK_KEY2               0x53
#define RTC_LOCK_KEY                  0xff

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

int  gd32_rtc_initialize(void);
int  gd32_rtc_getdatetime(FAR struct tm *tp);
int  gd32_rtc_setdatetime(FAR const struct tm *tp);

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __ARCH_ARM_SRC_GD32H7_GD32H7XX_RTC_H */
