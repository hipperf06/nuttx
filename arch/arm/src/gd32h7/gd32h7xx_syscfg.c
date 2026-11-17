/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_syscfg.c
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

#include "arm_internal.h"

#include "chip.h"
#include "gd32h7xx.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_syscfg_exti_line_config
 *
 * Description:
 *   Select the GPIO port connected to an EXTI line (lines 0-15).
 *
 * Input Parameters:
 *  - port:  GPIO port select value (SYSCFG_EXTI_SOURCE_GPIOx)
 *  - pin:   GPIO pin number (0-15)
 *
 ****************************************************************************/

void gd32_syscfg_exti_line_config(uint8_t port, uint8_t pin)
{
  uint32_t regaddr;
  uint32_t regval;
  uint32_t shift;
  uint32_t mask;

  /* Enable the SYSCFG clock */

  gd32_rcu_periph_clock_enable(RCU_SYSCFG_EN);

  /* Each EXTISSx register holds the source selection of four EXTI lines,
   * four bits per line.
   */

  regaddr = GD32H7_SYSCFG + GD32H7_SYSCFG_EXTISS0_OFFSET + ((pin / 4) * 4);
  shift   = (pin % 4) * 4;
  mask    = 0xf << shift;

  regval = getreg32(regaddr);
  regval &= ~mask;
  regval |= ((uint32_t)port << shift);
  putreg32(regval, regaddr);
}
