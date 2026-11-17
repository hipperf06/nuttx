/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h7xx_gpio.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_GPIO_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_GPIO_H

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

/* GPIO port base addresses.
 *
 * The GD32H7xx provides ten GPIO ports: A, B, C, D, E, F, G, H, J and K
 * (there is no port "I").
 */

#define GD32H7_GPIOA                  GD32H7_GPIOA_BASE
#define GD32H7_GPIOB                  GD32H7_GPIOB_BASE
#define GD32H7_GPIOC                  GD32H7_GPIOC_BASE
#define GD32H7_GPIOD                  GD32H7_GPIOD_BASE
#define GD32H7_GPIOE                  GD32H7_GPIOE_BASE
#define GD32H7_GPIOF                  GD32H7_GPIOF_BASE
#define GD32H7_GPIOG                  GD32H7_GPIOG_BASE
#define GD32H7_GPIOH                  GD32H7_GPIOH_BASE
#define GD32H7_GPIOJ                  GD32H7_GPIOJ_BASE
#define GD32H7_GPIOK                  GD32H7_GPIOK_BASE

/* Register offsets ------------------------------------------------------ */

#define GD32H7_GPIO_CTL_OFFSET        0x0000  /* GPIO port control register */
#define GD32H7_GPIO_OMODE_OFFSET      0x0004  /* GPIO port output mode register */
#define GD32H7_GPIO_OSPD_OFFSET       0x0008  /* GPIO port output speed register */
#define GD32H7_GPIO_PUD_OFFSET        0x000c  /* GPIO port pull-up/down register */
#define GD32H7_GPIO_ISTAT_OFFSET      0x0010  /* GPIO port input status register */
#define GD32H7_GPIO_OCTL_OFFSET       0x0014  /* GPIO port output control register */
#define GD32H7_GPIO_BOP_OFFSET        0x0018  /* GPIO port bit operation register */
#define GD32H7_GPIO_LOCK_OFFSET       0x001c  /* GPIO port configuration lock register */
#define GD32H7_GPIO_AFSEL0_OFFSET     0x0020  /* GPIO AF select register 0 (pins 0-7) */
#define GD32H7_GPIO_AFSEL1_OFFSET     0x0024  /* GPIO AF select register 1 (pins 8-15) */
#define GD32H7_GPIO_BC_OFFSET         0x0028  /* GPIO bit clear register */
#define GD32H7_GPIO_TG_OFFSET         0x002c  /* GPIO port bit toggle register */
#define GD32H7_GPIO_IFL_OFFSET        0x0030  /* GPIO input filtering register */
#define GD32H7_GPIO_IFTP_OFFSET       0x0034  /* GPIO input filtering type register */

/* Register access macros ------------------------------------------------ */

#define GD32H7_GPIO_CTL(gpiox)        (getreg32((gpiox) + GD32H7_GPIO_CTL_OFFSET))
#define GD32H7_GPIO_OMODE(gpiox)      (getreg32((gpiox) + GD32H7_GPIO_OMODE_OFFSET))
#define GD32H7_GPIO_OSPD(gpiox)       (getreg32((gpiox) + GD32H7_GPIO_OSPD_OFFSET))
#define GD32H7_GPIO_PUD(gpiox)        (getreg32((gpiox) + GD32H7_GPIO_PUD_OFFSET))
#define GD32H7_GPIO_ISTAT(gpiox)      (getreg32((gpiox) + GD32H7_GPIO_ISTAT_OFFSET))
#define GD32H7_GPIO_OCTL(gpiox)       (getreg32((gpiox) + GD32H7_GPIO_OCTL_OFFSET))
#define GD32H7_GPIO_AFSEL0(gpiox)     (getreg32((gpiox) + GD32H7_GPIO_AFSEL0_OFFSET))
#define GD32H7_GPIO_AFSEL1(gpiox)     (getreg32((gpiox) + GD32H7_GPIO_AFSEL1_OFFSET))

/* Pin configuration field extraction ------------------------------------- */

#define GD32H7_GPIO_MODE_MASK(pin)    (0x3U << ((pin) << 1))
#define GD32H7_GPIO_PUPD_MASK(pin)    (0x3U << ((pin) << 1))
#define GD32H7_GPIO_OSPD_MASK(pin)    (0x3U << ((pin) << 1))
#define GD32H7_GPIO_OMODE_BIT(pin)    (0x1U << (pin))
#define GD32H7_GPIO_AF_MASK(pin)      (0xfU << (((pin) & 0x7) << 2))

/* Alternate function numbers (AF0-AF15) */

#define GD32H7_GPIO_AF_0              0x00
#define GD32H7_GPIO_AF_1              0x01
#define GD32H7_GPIO_AF_2              0x02
#define GD32H7_GPIO_AF_3              0x03
#define GD32H7_GPIO_AF_4              0x04
#define GD32H7_GPIO_AF_5              0x05
#define GD32H7_GPIO_AF_6              0x06
#define GD32H7_GPIO_AF_7              0x07
#define GD32H7_GPIO_AF_8              0x08
#define GD32H7_GPIO_AF_9              0x09
#define GD32H7_GPIO_AF_10             0x0a
#define GD32H7_GPIO_AF_11             0x0b
#define GD32H7_GPIO_AF_12             0x0c
#define GD32H7_GPIO_AF_13             0x0d
#define GD32H7_GPIO_AF_14             0x0e
#define GD32H7_GPIO_AF_15             0x0f

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_GPIO_H */
