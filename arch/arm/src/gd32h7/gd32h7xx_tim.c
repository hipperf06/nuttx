/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_tim.c
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

#include <nuttx/irq.h>
#include <nuttx/arch.h>

#include "arm_internal.h"

#include "gd32h7xx_tim.h"
#include "gd32h7xx_rcu.h"

/****************************************************************************
 * Private Data
 ****************************************************************************/

struct gd32_timinfo_s
{
  uint32_t base;     /* Timer base address */
  uint32_t rcuen;    /* RCU clock enable selector */
  uint32_t rcurst;   /* RCU reset selector */
  uint32_t clock;    /* Kernel clock frequency */
};

static const struct gd32_timinfo_s g_timinfo[] =
{
  { GD32H7_TIMER0,  RCU_TIMER0_EN,  RCU_TIMER0_RST,
    GD32H7_TIMER_APB2_CLOCK },
  { GD32H7_TIMER1,  RCU_TIMER1_EN,  RCU_TIMER1_RST,
    GD32H7_TIMER_APB1_CLOCK },
  { GD32H7_TIMER2,  RCU_TIMER2_EN,  RCU_TIMER2_RST,
    GD32H7_TIMER_APB1_CLOCK },
  { GD32H7_TIMER3,  RCU_TIMER3_EN,  RCU_TIMER3_RST,
    GD32H7_TIMER_APB1_CLOCK },
  { GD32H7_TIMER4,  RCU_TIMER4_EN,  RCU_TIMER4_RST,
    GD32H7_TIMER_APB1_CLOCK },
  { GD32H7_TIMER5,  RCU_TIMER5_EN,  RCU_TIMER5_RST,
    GD32H7_TIMER_APB1_CLOCK },
  { GD32H7_TIMER6,  RCU_TIMER6_EN,  RCU_TIMER6_RST,
    GD32H7_TIMER_APB1_CLOCK },
  { GD32H7_TIMER7,  RCU_TIMER7_EN,  RCU_TIMER7_RST,
    GD32H7_TIMER_APB2_CLOCK },
  { GD32H7_TIMER14, RCU_TIMER14_EN, RCU_TIMER14_RST,
    GD32H7_TIMER_APB2_CLOCK },
  { GD32H7_TIMER15, RCU_TIMER15_EN, RCU_TIMER15_RST,
    GD32H7_TIMER_APB2_CLOCK },
  { GD32H7_TIMER16, RCU_TIMER16_EN, RCU_TIMER16_RST,
    GD32H7_TIMER_APB2_CLOCK },
  { GD32H7_TIMER22, RCU_TIMER22_EN, RCU_TIMER22_RST,
    GD32H7_TIMER_APB1_CLOCK },
  { GD32H7_TIMER23, RCU_TIMER23_EN, RCU_TIMER23_RST,
    GD32H7_TIMER_APB1_CLOCK },
  { GD32H7_TIMER30, RCU_TIMER30_EN, RCU_TIMER30_RST,
    GD32H7_TIMER_APB1_CLOCK },
  { GD32H7_TIMER31, RCU_TIMER31_EN, RCU_TIMER31_RST,
    GD32H7_TIMER_APB1_CLOCK },
  { GD32H7_TIMER40, RCU_TIMER40_EN, RCU_TIMER40_RST,
    GD32H7_TIMER_APB2_CLOCK },
  { GD32H7_TIMER41, RCU_TIMER41_EN, RCU_TIMER41_RST,
    GD32H7_TIMER_APB2_CLOCK },
  { GD32H7_TIMER42, RCU_TIMER42_EN, RCU_TIMER42_RST,
    GD32H7_TIMER_APB2_CLOCK },
  { GD32H7_TIMER43, RCU_TIMER43_EN, RCU_TIMER43_RST,
    GD32H7_TIMER_APB2_CLOCK },
  { GD32H7_TIMER44, RCU_TIMER44_EN, RCU_TIMER44_RST,
    GD32H7_TIMER_APB2_CLOCK },
  { GD32H7_TIMER50, RCU_TIMER50_EN, RCU_TIMER50_RST,
    GD32H7_TIMER_APB1_CLOCK },
  { GD32H7_TIMER51, RCU_TIMER51_EN, RCU_TIMER51_RST,
    GD32H7_TIMER_APB1_CLOCK },
};

#define NTIMERS (sizeof(g_timinfo) / sizeof(g_timinfo[0]))

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static FAR const struct gd32_timinfo_s *gd32_tim_lookup(uint32_t timbase)
{
  int i;

  for (i = 0; i < NTIMERS; i++)
    {
      if (g_timinfo[i].base == timbase)
        {
          return &g_timinfo[i];
        }
    }

  DEBUGASSERT(0);
  return NULL;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_tim_clock_enable
 ****************************************************************************/

void gd32_tim_clock_enable(uint32_t timbase)
{
  FAR const struct gd32_timinfo_s *info = gd32_tim_lookup(timbase);

  if (info != NULL)
    {
      gd32_rcu_periph_clock_enable(info->rcuen);
    }
}

/****************************************************************************
 * Name: gd32_tim_clock_disable
 ****************************************************************************/

void gd32_tim_clock_disable(uint32_t timbase)
{
  FAR const struct gd32_timinfo_s *info = gd32_tim_lookup(timbase);

  if (info != NULL)
    {
      gd32_rcu_periph_clock_disable(info->rcuen);
    }
}

/****************************************************************************
 * Name: gd32_tim_reset
 ****************************************************************************/

void gd32_tim_reset(uint32_t timbase)
{
  FAR const struct gd32_timinfo_s *info = gd32_tim_lookup(timbase);

  if (info != NULL)
    {
      gd32_rcu_periph_reset(info->rcurst);
    }
}

/****************************************************************************
 * Name: gd32_tim_getclock
 ****************************************************************************/

uint32_t gd32_tim_getclock(uint32_t timbase)
{
  FAR const struct gd32_timinfo_s *info = gd32_tim_lookup(timbase);

  return info != NULL ? info->clock : 0;
}

/****************************************************************************
 * Name: gd32_tim_setperiod
 *
 * Description:
 *   Program the prescaler and the auto-reload register.  The counter clock
 *   is timer_clock / (psc + 1); the update event frequency is that divided
 *   by (arr + 1).
 *
 ****************************************************************************/

void gd32_tim_setperiod(uint32_t timbase, uint32_t psc, uint32_t arr)
{
  DEBUGASSERT(psc <= 0xffff && arr <= 0xffff);

  putreg32(psc & 0xffff, GD32H7_TIM_PSC(timbase));
  putreg32(arr & 0xffff, GD32H7_TIM_CAR(timbase));

  /* Force an update so that the new prescaler value is loaded */

  putreg32(TIM_SWEVG_UPG, GD32H7_TIM_SWEVG(timbase));
  putreg32(0, GD32H7_TIM_INTF(timbase));
}

/****************************************************************************
 * Name: gd32_tim_pwm_config
 *
 * Description:
 *   Configure one channel as a PWM mode 0 output with active-high polarity.
 *   Channel numbering is 0..3.
 *
 ****************************************************************************/

void gd32_tim_pwm_config(uint32_t timbase, int channel)
{
  uint32_t regval;

  DEBUGASSERT(channel >= 0 && channel <= 3);

  regval = getreg32(GD32H7_TIM_CHCTL2(timbase));
  regval &= ~(TIM_CHCTL2_CH0P << (4 * channel));
  regval |=  (TIM_CHCTL2_CH0EN << (4 * channel));
  putreg32(regval, GD32H7_TIM_CHCTL2(timbase));

  /* Set the output compare mode to PWM0 */

  if (channel < 2)
    {
      uint32_t shift = (channel == 0) ? 0 : 8;

      regval = getreg32(GD32H7_TIM_CHCTL0(timbase));
      regval &= ~(TIM_CHCTL0_CH0COMCTL_MASK << shift);
      regval |=  (TIM_CHCTL0_CH0COMSEN << shift);
      regval |=  (TIM_OCMODE_PWM0 << shift);
      putreg32(regval, GD32H7_TIM_CHCTL0(timbase));
    }
  else
    {
      uint32_t shift = (channel == 2) ? 0 : 8;

      regval = getreg32(GD32H7_TIM_CHCTL1(timbase));
      regval &= ~(TIM_CHCTL1_CH2COMCTL_MASK << shift);
      regval |=  (TIM_CHCTL1_CH2COMSEN << shift);
      regval |=  (TIM_OCMODE_PWM0 << shift);
      putreg32(regval, GD32H7_TIM_CHCTL1(timbase));
    }
}

/****************************************************************************
 * Name: gd32_tim_pwm_setduty
 ****************************************************************************/

void gd32_tim_pwm_setduty(uint32_t timbase, int channel, uint32_t cv)
{
  DEBUGASSERT(channel >= 0 && channel <= 3);

  putreg32(cv & 0xffff, timbase + GD32H7_TIM_CH0CV_OFFSET +
                       (4 * channel));
}

/****************************************************************************
 * Name: gd32_tim_start / gd32_tim_stop
 ****************************************************************************/

void gd32_tim_start(uint32_t timbase)
{
  putreg32(TIM_CTL0_CEN, GD32H7_TIM_CTL0(timbase));
}

void gd32_tim_stop(uint32_t timbase)
{
  uint32_t regval = getreg32(GD32H7_TIM_CTL0(timbase));

  regval &= ~TIM_CTL0_CEN;
  putreg32(regval, GD32H7_TIM_CTL0(timbase));
}

/****************************************************************************
 * Name: gd32_tim_dma_burst_config
 ****************************************************************************/

void gd32_tim_dma_burst_config(uint32_t timbase, uint32_t dbase,
                               uint32_t count)
{
  uint32_t regval;

  if (count == 0)
    {
      count = 1;
    }

  regval = ((count - 1) << TIM_DMACFG_DMATC_SHIFT) |
           ((dbase << TIM_DMACFG_DMATA_SHIFT) & TIM_DMACFG_DMATA_MASK);

  putreg32(regval, GD32H7_TIM_DMACFG(timbase));
}

/****************************************************************************
 * Name: gd32_tim_dma_request_enable
 ****************************************************************************/

void gd32_tim_dma_request_enable(uint32_t timbase, uint32_t chmask,
                                 bool enable)
{
  uint32_t bits = TIM_DMAINTEN_UPDEN;

  if ((chmask & 1) != 0)
    {
      bits |= TIM_DMAINTEN_CH0DEN;
    }

  if ((chmask & 2) != 0)
    {
      bits |= TIM_DMAINTEN_CH1DEN;
    }

  if ((chmask & 4) != 0)
    {
      bits |= TIM_DMAINTEN_CH2DEN;
    }

  if ((chmask & 8) != 0)
    {
      bits |= TIM_DMAINTEN_CH3DEN;
    }

  if (enable)
    {
      modifyreg32(GD32H7_TIM_DMAINTEN(timbase), 0, bits);
    }
  else
    {
      modifyreg32(GD32H7_TIM_DMAINTEN(timbase), bits, 0);
    }
}
