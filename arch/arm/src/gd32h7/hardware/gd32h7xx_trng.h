/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h7xx_trng.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_TRNG_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_TRNG_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "chip.h"
#include "hardware/gd32h73x_memorymap.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define GD32H7_TRNG                   GD32H7_TRNG_BASE

#define GD32H7_TRNG_CTL_OFFSET        0x0000
#define GD32H7_TRNG_STAT_OFFSET       0x0004
#define GD32H7_TRNG_DATA_OFFSET       0x0008
#define GD32H7_TRNG_HTCFG_OFFSET      0x0010

#define GD32H7_TRNG_CTL               (GD32H7_TRNG + GD32H7_TRNG_CTL_OFFSET)
#define GD32H7_TRNG_STAT              (GD32H7_TRNG + GD32H7_TRNG_STAT_OFFSET)
#define GD32H7_TRNG_DATA              (GD32H7_TRNG + GD32H7_TRNG_DATA_OFFSET)
#define GD32H7_TRNG_HTCFG             (GD32H7_TRNG + GD32H7_TRNG_HTCFG_OFFSET)

/* TRNG_CTL */

#define TRNG_CTL_TRNGEN               (1 << 2)   /* Random number generator enable */
#define TRNG_CTL_IE                   (1 << 3)   /* Interrupt enable */
#define TRNG_CTL_CED                  (1 << 5)   /* Clock error detection */
#define TRNG_CTL_RTEN                 (1 << 7)   /* Repetition test enable */
#define TRNG_CTL_INIT                 (1 << 8)   /* Initialization */
#define TRNG_CTL_PPEN                 (1 << 9)   /* Post processing enable */
#define TRNG_CTL_CONDEN               (1 << 10)  /* Conditioning enable */
#define TRNG_CTL_CONDRST              (1 << 30)  /* Conditioning reset */
#define TRNG_CTL_LK                   (1 << 31)  /* Register lock */

/* TRNG_STAT */

#define TRNG_STAT_DRDY                (1 << 0)   /* Data ready */
#define TRNG_STAT_CECS                (1 << 1)   /* Clock error current status */
#define TRNG_STAT_SECS                (1 << 2)   /* Seed error current status */

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_TRNG_H */
