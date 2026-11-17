/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h7xx_pmu.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_PMU_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_PMU_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "chip.h"
#include "hardware/gd32h73x_memorymap.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define GD32H7_PMU                    GD32H7_PMU_BASE

#define GD32H7_PMU_CTL0_OFFSET        0x0000
#define GD32H7_PMU_CS_OFFSET          0x0004
#define GD32H7_PMU_CTL1_OFFSET        0x0008
#define GD32H7_PMU_CTL2_OFFSET        0x0010
#define GD32H7_PMU_CTL3_OFFSET        0x0014
#define GD32H7_PMU_PAR_OFFSET         0x0018

#define GD32H7_PMU_CTL0               (GD32H7_PMU + GD32H7_PMU_CTL0_OFFSET)
#define GD32H7_PMU_CS                 (GD32H7_PMU + GD32H7_PMU_CS_OFFSET)
#define GD32H7_PMU_CTL1               (GD32H7_PMU + GD32H7_PMU_CTL1_OFFSET)
#define GD32H7_PMU_CTL2               (GD32H7_PMU + GD32H7_PMU_CTL2_OFFSET)
#define GD32H7_PMU_CTL3               (GD32H7_PMU + GD32H7_PMU_CTL3_OFFSET)
#define GD32H7_PMU_PAR                (GD32H7_PMU + GD32H7_PMU_PAR_OFFSET)

/* PMU_CTL0 */

#define PMU_CTL0_STBMOD               (1 << 1)   /* Standby mode */
#define PMU_CTL0_WURST                (1 << 2)   /* Wakeup flag reset */
#define PMU_CTL0_STBRST               (1 << 3)   /* Standby flag reset */
#define PMU_CTL0_LVDEN                (1 << 4)   /* Low voltage detector enable */
#define PMU_CTL0_LVDT_SHIFT           (5)
#define PMU_CTL0_LVDT_MASK            (7 << PMU_CTL0_LVDT_SHIFT)
#define PMU_CTL0_BKPWEN               (1 << 8)   /* Backup domain write enable */

/* PMU_CS */

#define PMU_CS_WUF                    (1 << 0)   /* Wakeup flag */
#define PMU_CS_STBF                   (1 << 1)   /* Standby flag */
#define PMU_CS_LVDF                   (1 << 2)   /* LVD flag */
#define PMU_CS_WUPEN0                 (1 << 8)
#define PMU_CS_WUPEN1                 (1 << 9)
#define PMU_CS_WUPEN3                 (1 << 11)
#define PMU_CS_WUPEN5                 (1 << 13)

/* PMU_CTL1 - backup domain */

#define PMU_CTL1_BKPVSEN              (1 << 0)   /* Backup voltage supply enable */
#define PMU_CTL1_VBTMEN               (1 << 4)   /* VBAT mode enable */

/* PMU_CTL2 - core voltage regulator */

#define PMU_CTL2_BYPASS               (1 << 0)   /* LDO bypass (SMPS direct) */
#define PMU_CTL2_LDOEN                (1 << 1)   /* LDO enable */

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_PMU_H */
