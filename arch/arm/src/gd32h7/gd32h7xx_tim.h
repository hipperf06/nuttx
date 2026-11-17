/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_tim.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_GD32H7XX_TIM_H
#define __ARCH_ARM_SRC_GD32H7_GD32H7XX_TIM_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>

#include <arch/board/board.h>

#include "hardware/gd32h7xx_tim.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Timer kernel clock.
 *
 * On the GD32H7 the timer kernel clock is the APB clock multiplied by two
 * whenever the corresponding APB prescaler is not one.  The board clock
 * configuration used by this port has APB1 = AHB / 2 and APB2 = AHB, hence:
 *
 *   TIMER1-6/22/23/30/31/50/51 (APB1): 2 x PCLK1
 *   TIMER0/7/14/15/16/40-44    (APB2):     PCLK2
 */

#define GD32H7_TIMER_APB1_CLOCK   (2 * GD32_PCLK1_FREQUENCY)
#define GD32H7_TIMER_APB2_CLOCK   (GD32_PCLK2_FREQUENCY)

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

/* Enable/disable and reset the clock of one timer */

void gd32_tim_clock_enable(uint32_t timbase);
void gd32_tim_clock_disable(uint32_t timbase);
void gd32_tim_reset(uint32_t timbase);

/* Return the timer kernel clock frequency for the given timer base */

uint32_t gd32_tim_getclock(uint32_t timbase);

/****************************************************************************
 * Name: gd32_tim_dma_burst_config
 *
 * Description:
 *   Configure the timer DMA burst: the DMA controller will write "count"
 *   consecutive words starting at the register selected by dbase into the
 *   timer.  dbase is the register offset divided by 4 (see TIM_DMABASE_*).
 *
 ****************************************************************************/

void gd32_tim_dma_burst_config(uint32_t timbase, uint32_t dbase,
                               uint32_t count);

/****************************************************************************
 * Name: gd32_tim_dma_request_enable
 *
 * Description:
 *   Enable/disable the timer DMA requests.  chmask has one bit per channel
 *   (bit 0 = channel 0); a zero chmask only enables the update request.
 *
 ****************************************************************************/

void gd32_tim_dma_request_enable(uint32_t timbase, uint32_t chmask,
                                 bool enable);

/* Program the time base (prescaler and auto-reload) and force an update */

void gd32_tim_setperiod(uint32_t timbase, uint32_t psc, uint32_t arr);

/* Configure one channel for PWM mode 0 output */

void gd32_tim_pwm_config(uint32_t timbase, int channel);

/* Set the compare value (duty) of one PWM channel */

void gd32_tim_pwm_setduty(uint32_t timbase, int channel, uint32_t cv);

/* Start/stop the counter */

void gd32_tim_start(uint32_t timbase);
void gd32_tim_stop(uint32_t timbase);

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __ARCH_ARM_SRC_GD32H7_GD32H7XX_TIM_H */
