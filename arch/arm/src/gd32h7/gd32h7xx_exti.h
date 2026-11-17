/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_exti.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_GD32H7XX_EXTI_H
#define __ARCH_ARM_SRC_GD32H7_GD32H7XX_EXTI_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <nuttx/irq.h>

#include "chip.h"
#include "hardware/gd32h7xx_exti.h"

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifndef __ASSEMBLY__

#if defined(__cplusplus)
extern "C"
{
#endif

/****************************************************************************
 * Name: gd32_exti_init
 *
 * Description:
 *   Initialize one EXTI line (interrupt or event, with trigger mode).
 *
 * Input Parameters:
 *  - linex:      EXTI line number (GD32H7_EXTI_LINEx)
 *  - exti_mode:  GD32H7_EXTI_INTERRUPT or GD32H7_EXTI_EVENT
 *  - trig_type:  GD32H7_EXTI_TRIG_RISING / FALLING / BOTH / NONE
 *
 ****************************************************************************/

void gd32_exti_init(uint32_t linex, uint8_t exti_mode, uint8_t trig_type);

/****************************************************************************
 * Name: gd32_exti_interrupt_enable / gd32_exti_interrupt_disable
 *
 * Description:
 *   Enable / disable the interrupts from EXTI line x.
 *
 ****************************************************************************/

void gd32_exti_interrupt_enable(uint32_t linex);
void gd32_exti_interrupt_disable(uint32_t linex);

/****************************************************************************
 * Name: gd32_exti_event_enable / gd32_exti_event_disable
 *
 * Description:
 *   Enable / disable the events from EXTI line x.
 *
 ****************************************************************************/

void gd32_exti_event_enable(uint32_t linex);
void gd32_exti_event_disable(uint32_t linex);

/****************************************************************************
 * Name: gd32_exti_software_interrupt_enable
 *
 * Description:
 *   Generate a software interrupt/event from EXTI line x.
 *
 ****************************************************************************/

void gd32_exti_software_interrupt_enable(uint32_t linex);
void gd32_exti_software_interrupt_disable(uint32_t linex);

/****************************************************************************
 * Name: gd32_exti_flag_get / gd32_exti_flag_clear
 *
 * Description:
 *   Get / clear the pending flag of EXTI line x.
 *
 ****************************************************************************/

bool gd32_exti_flag_get(uint32_t linex);
void gd32_exti_flag_clear(uint32_t linex);

/****************************************************************************
 * Name: gd32_gpioexti_attach / gd32_gpioexti_detach
 *
 * Description:
 *   Attach / detach an interrupt handler to a GPIO EXTI interrupt.
 *   The cfgset argument is a bit-encoded GPIO configuration as used by
 *   gd32_configgpio(); only the port and pin fields are used.  The
 *   appropriate EXTI IRQ (line 0-4 individually, 5-9 shared, 10-15 shared)
 *   is selected automatically.
 *
 ****************************************************************************/

int gd32_gpioexti_attach(uint32_t cfgset, xcpt_t irqhandler, void *arg);
void gd32_gpioexti_detach(uint32_t cfgset);

#if defined(__cplusplus)
}
#endif
#endif /* __ASSEMBLY__ */
#endif /* __ARCH_ARM_SRC_GD32H7_GD32H7XX_EXTI_H */
