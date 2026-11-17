/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h7xx_syscfg.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_SYSCFG_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_SYSCFG_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "chip.h"

#if defined(CONFIG_GD32H7_GD32H73x)
#  include "hardware/gd32h73x_memorymap.h"
#else
#  error "Unsupported GD32H7 memory map"
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* SYSCFG base address */

#define GD32H7_SYSCFG                 GD32H7_SYSCFG_BASE

/* Register offsets ------------------------------------------------------ */

#define GD32H7_SYSCFG_PMCFG_OFFSET        0x0004  /* Peripheral mode configuration */
#define GD32H7_SYSCFG_EXTISS0_OFFSET      0x0008  /* EXTI source selection register 0 */
#define GD32H7_SYSCFG_EXTISS1_OFFSET      0x000c  /* EXTI source selection register 1 */
#define GD32H7_SYSCFG_EXTISS2_OFFSET      0x0010  /* EXTI source selection register 2 */
#define GD32H7_SYSCFG_EXTISS3_OFFSET      0x0014  /* EXTI source selection register 3 */
#define GD32H7_SYSCFG_LKCTL_OFFSET        0x0018  /* Lock control register */
#define GD32H7_SYSCFG_CPSCTL_OFFSET       0x0020  /* I/O compensation control */
#define GD32H7_SYSCFG_CPSCCCFG_OFFSET     0x0028  /* I/O compensation cell code config */
#define GD32H7_SYSCFG_SRAMCFG0_OFFSET     0x0064  /* SRAM configuration register 0 */
#define GD32H7_SYSCFG_SRAMCFG1_OFFSET     0x0068  /* SRAM configuration register 1 */
#define GD32H7_SYSCFG_USERCFG_OFFSET      0x0300  /* User configuration register */

/* Register addresses ---------------------------------------------------- */

#define GD32H7_SYSCFG_PMCFG               (GD32H7_SYSCFG + GD32H7_SYSCFG_PMCFG_OFFSET)
#define GD32H7_SYSCFG_EXTISS0             (GD32H7_SYSCFG + GD32H7_SYSCFG_EXTISS0_OFFSET)
#define GD32H7_SYSCFG_EXTISS1             (GD32H7_SYSCFG + GD32H7_SYSCFG_EXTISS1_OFFSET)
#define GD32H7_SYSCFG_EXTISS2             (GD32H7_SYSCFG + GD32H7_SYSCFG_EXTISS2_OFFSET)
#define GD32H7_SYSCFG_EXTISS3             (GD32H7_SYSCFG + GD32H7_SYSCFG_EXTISS3_OFFSET)
#define GD32H7_SYSCFG_LKCTL               (GD32H7_SYSCFG + GD32H7_SYSCFG_LKCTL_OFFSET)
#define GD32H7_SYSCFG_CPSCTL              (GD32H7_SYSCFG + GD32H7_SYSCFG_CPSCTL_OFFSET)
#define GD32H7_SYSCFG_SRAMCFG0            (GD32H7_SYSCFG + GD32H7_SYSCFG_SRAMCFG0_OFFSET)
#define GD32H7_SYSCFG_SRAMCFG1            (GD32H7_SYSCFG + GD32H7_SYSCFG_SRAMCFG1_OFFSET)

/* SRAMCFG1 bits ---------------------------------------------------------- */

#define SYSCFG_SRAMCFG1_TCM_WAITSTATE     (1 << 0)  /* TCM wait state configuration */

/* SYSCFG_EXTISSx: four bits per EXTI line; the value selects the GPIO port
 * (see the EXTI source port definitions below).
 */

#define SYSCFG_EXTISS_EXTI0_SS            (0xf << 0)
#define SYSCFG_EXTISS_EXTI1_SS            (0xf << 4)
#define SYSCFG_EXTISS_EXTI2_SS            (0xf << 8)
#define SYSCFG_EXTISS_EXTI3_SS            (0xf << 12)
#define SYSCFG_EXTISS_EXTI4_SS            (0xf << 0)
#define SYSCFG_EXTISS_EXTI5_SS            (0xf << 4)
#define SYSCFG_EXTISS_EXTI6_SS            (0xf << 8)
#define SYSCFG_EXTISS_EXTI7_SS            (0xf << 12)
#define SYSCFG_EXTISS_EXTI8_SS            (0xf << 0)
#define SYSCFG_EXTISS_EXTI9_SS            (0xf << 4)
#define SYSCFG_EXTISS_EXTI10_SS           (0xf << 8)
#define SYSCFG_EXTISS_EXTI11_SS           (0xf << 12)
#define SYSCFG_EXTISS_EXTI12_SS           (0xf << 0)
#define SYSCFG_EXTISS_EXTI13_SS           (0xf << 4)
#define SYSCFG_EXTISS_EXTI14_SS           (0xf << 8)
#define SYSCFG_EXTISS_EXTI15_SS           (0xf << 12)

/* EXTI source port select values.
 *
 * NOTE: There is no GPIOI on the GD32H7xx; GPIOJ and GPIOK use the
 * select values 9 and 10 respectively.
 */

#define SYSCFG_EXTI_SOURCE_GPIOA          0x0
#define SYSCFG_EXTI_SOURCE_GPIOB          0x1
#define SYSCFG_EXTI_SOURCE_GPIOC          0x2
#define SYSCFG_EXTI_SOURCE_GPIOD          0x3
#define SYSCFG_EXTI_SOURCE_GPIOE          0x4
#define SYSCFG_EXTI_SOURCE_GPIOF          0x5
#define SYSCFG_EXTI_SOURCE_GPIOG          0x6
#define SYSCFG_EXTI_SOURCE_GPIOH          0x7
#define SYSCFG_EXTI_SOURCE_GPIOJ          0x9
#define SYSCFG_EXTI_SOURCE_GPIOK          0x0a

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_SYSCFG_H */
