/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h7xx_fmc.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_FMC_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_FMC_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "chip.h"
#include "hardware/gd32h73x_memorymap.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define GD32H7_FMC                    GD32H7_FMC_BASE

/* Register offsets ------------------------------------------------------ */

#define GD32H7_FMC_KEY_OFFSET         0x0004  /* Flash key register */
#define GD32H7_FMC_OBKEY_OFFSET       0x0008  /* Option byte key */
#define GD32H7_FMC_CTL_OFFSET         0x000c  /* Control register */
#define GD32H7_FMC_STAT_OFFSET        0x0010  /* Status register */
#define GD32H7_FMC_ADDR_OFFSET        0x0014  /* Address register */

/* Register addresses ---------------------------------------------------- */

#define GD32H7_FMC_KEY                (GD32H7_FMC + GD32H7_FMC_KEY_OFFSET)
#define GD32H7_FMC_OBKEY              (GD32H7_FMC + GD32H7_FMC_OBKEY_OFFSET)
#define GD32H7_FMC_CTL                (GD32H7_FMC + GD32H7_FMC_CTL_OFFSET)
#define GD32H7_FMC_STAT               (GD32H7_FMC + GD32H7_FMC_STAT_OFFSET)
#define GD32H7_FMC_ADDR               (GD32H7_FMC + GD32H7_FMC_ADDR_OFFSET)

/* Unlock keys */

#define FMC_KEY1                      0x45670123
#define FMC_KEY2                      0xcdef89ab

/* FMC_CTL */

#define FMC_CTL_LK                    (1 << 0)   /* Flash lock */
#define FMC_CTL_PG                    (1 << 1)   /* Program enable */
#define FMC_CTL_SER                   (1 << 2)   /* Sector erase */
#define FMC_CTL_MER                   (1 << 3)   /* Mass erase */
#define FMC_CTL_PGCHEN                (1 << 4)   /* Programming area check */
#define FMC_CTL_START                 (1 << 7)   /* Start operation */

/* FMC_STAT */

#define FMC_STAT_BUSY                 (1 << 0)   /* Operation in progress */
#define FMC_STAT_ENDF                 (1 << 16)  /* Operation complete */
#define FMC_STAT_WPERR                (1 << 17)  /* Write protection error */
#define FMC_STAT_PGSERR               (1 << 18)  /* Program sequence error */
#define FMC_STAT_ERRORS               (FMC_STAT_WPERR | FMC_STAT_PGSERR)

/* Flash geometry: the main flash is divided into 8KiB sectors (up to 256
 * sectors for a 2MiB part).
 */

#define GD32H7_FLASH_SECTOR_SIZE      0x2000

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_FMC_H */
