/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h7xx_exti.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_EXTI_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_EXTI_H

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

/* EXTI base address */

#define GD32H7_EXTI                   GD32H7_EXTI_BASE

/* The EXTI provides two register groups:
 *
 *   Group 0: lines  0-31 (register offsets 0x0000 - 0x0014)
 *   Group 1: lines 32-37 (register offsets 0x0018 - 0x002c)
 *
 * Each group has the same layout: INTEN, EVEN, RTEN, FTEN, SWIEV, PD.
 */

#define GD32H7_EXTI_GROUP0_OFFSET     0x0000
#define GD32H7_EXTI_GROUP1_OFFSET     0x0018

#define GD32H7_EXTI_INTEN(g)          (GD32H7_EXTI + (g) + 0x0000)
#define GD32H7_EXTI_EVEN(g)           (GD32H7_EXTI + (g) + 0x0004)
#define GD32H7_EXTI_RTEN(g)           (GD32H7_EXTI + (g) + 0x0008)
#define GD32H7_EXTI_FTEN(g)           (GD32H7_EXTI + (g) + 0x000c)
#define GD32H7_EXTI_SWIEV(g)          (GD32H7_EXTI + (g) + 0x0010)
#define GD32H7_EXTI_PD(g)             (GD32H7_EXTI + (g) + 0x0014)

/* Group encoding helpers: an EXTI line number is encoded as
 * (group_offset << 8) | bit_number
 */

#define GD32H7_EXTI_LINE(g, b)        (((g) << 8) | (b))
#define GD32H7_EXTI_GROUP(line)       ((line) >> 8)
#define GD32H7_EXTI_BIT(line)         (1U << ((line) & 0x1f))

/* GPIO EXTI lines (lines 0-15 of group 0) */

#define GD32H7_EXTI_LINE0             GD32H7_EXTI_LINE(GD32H7_EXTI_GROUP0_OFFSET, 0)
#define GD32H7_EXTI_LINE1             GD32H7_EXTI_LINE(GD32H7_EXTI_GROUP0_OFFSET, 1)
#define GD32H7_EXTI_LINE2             GD32H7_EXTI_LINE(GD32H7_EXTI_GROUP0_OFFSET, 2)
#define GD32H7_EXTI_LINE3             GD32H7_EXTI_LINE(GD32H7_EXTI_GROUP0_OFFSET, 3)
#define GD32H7_EXTI_LINE4             GD32H7_EXTI_LINE(GD32H7_EXTI_GROUP0_OFFSET, 4)
#define GD32H7_EXTI_LINE5             GD32H7_EXTI_LINE(GD32H7_EXTI_GROUP0_OFFSET, 5)
#define GD32H7_EXTI_LINE6             GD32H7_EXTI_LINE(GD32H7_EXTI_GROUP0_OFFSET, 6)
#define GD32H7_EXTI_LINE7             GD32H7_EXTI_LINE(GD32H7_EXTI_GROUP0_OFFSET, 7)
#define GD32H7_EXTI_LINE8             GD32H7_EXTI_LINE(GD32H7_EXTI_GROUP0_OFFSET, 8)
#define GD32H7_EXTI_LINE9             GD32H7_EXTI_LINE(GD32H7_EXTI_GROUP0_OFFSET, 9)
#define GD32H7_EXTI_LINE10            GD32H7_EXTI_LINE(GD32H7_EXTI_GROUP0_OFFSET, 10)
#define GD32H7_EXTI_LINE11            GD32H7_EXTI_LINE(GD32H7_EXTI_GROUP0_OFFSET, 11)
#define GD32H7_EXTI_LINE12            GD32H7_EXTI_LINE(GD32H7_EXTI_GROUP0_OFFSET, 12)
#define GD32H7_EXTI_LINE13            GD32H7_EXTI_LINE(GD32H7_EXTI_GROUP0_OFFSET, 13)
#define GD32H7_EXTI_LINE14            GD32H7_EXTI_LINE(GD32H7_EXTI_GROUP0_OFFSET, 14)
#define GD32H7_EXTI_LINE15            GD32H7_EXTI_LINE(GD32H7_EXTI_GROUP0_OFFSET, 15)

/* Trigger modes */

#define GD32H7_EXTI_TRIG_RISING       0
#define GD32H7_EXTI_TRIG_FALLING      1
#define GD32H7_EXTI_TRIG_BOTH         2
#define GD32H7_EXTI_TRIG_NONE         3

/* Modes */

#define GD32H7_EXTI_INTERRUPT         0
#define GD32H7_EXTI_EVENT             1

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_EXTI_H */
