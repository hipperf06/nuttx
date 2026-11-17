/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h7xx_wdg.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_WDG_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_WDG_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "chip.h"
#include "hardware/gd32h73x_memorymap.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Free watchdog timer (FWDGT) ------------------------------------------- */

#define GD32H7_FWDGT                  GD32H7_FWDGT_BASE

#define GD32H7_FWDGT_CTL_OFFSET       0x0000
#define GD32H7_FWDGT_PSC_OFFSET       0x0004
#define GD32H7_FWDGT_RLD_OFFSET       0x0008
#define GD32H7_FWDGT_STAT_OFFSET      0x000c
#define GD32H7_FWDGT_WND_OFFSET       0x0010

#define GD32H7_FWDGT_CTL              (GD32H7_FWDGT + GD32H7_FWDGT_CTL_OFFSET)
#define GD32H7_FWDGT_PSC              (GD32H7_FWDGT + GD32H7_FWDGT_PSC_OFFSET)
#define GD32H7_FWDGT_RLD              (GD32H7_FWDGT + GD32H7_FWDGT_RLD_OFFSET)
#define GD32H7_FWDGT_STAT             (GD32H7_FWDGT + GD32H7_FWDGT_STAT_OFFSET)
#define GD32H7_FWDGT_WND              (GD32H7_FWDGT + GD32H7_FWDGT_WND_OFFSET)

/* FWDGT command values written to CTL */

#define FWDGT_CTL_CMD_RELOAD          0x0000aaaa  /* Reload counter */
#define FWDGT_CTL_CMD_ENABLE          0x00005555  /* Enable write access */
#define FWDGT_CTL_CMD_START           0x0000cccc  /* Start counter */

/* FWDGT prescaler field (PSC) */

#define FWDGT_PSC_DIV4                (0 << 0)
#define FWDGT_PSC_DIV8                (1 << 0)
#define FWDGT_PSC_DIV16               (2 << 0)
#define FWDGT_PSC_DIV32               (3 << 0)
#define FWDGT_PSC_DIV64               (4 << 0)
#define FWDGT_PSC_DIV128              (5 << 0)
#define FWDGT_PSC_DIV256              (6 << 0)
#define FWDGT_PSC_MASK                (7 << 0)

#define FWDGT_RLD_MASK                0x00000fff

/* FWDGT status */

#define FWDGT_STAT_PUD                (1 << 0)
#define FWDGT_STAT_RUD                (1 << 1)
#define FWDGT_STAT_WUD                (1 << 2)

/* Window watchdog timer (WWDGT) ----------------------------------------- */

#define GD32H7_WWDGT                  GD32H7_WWDGT_BASE

#define GD32H7_WWDGT_CTL_OFFSET       0x0000
#define GD32H7_WWDGT_CFG_OFFSET       0x0004
#define GD32H7_WWDGT_STAT_OFFSET      0x0008

#define GD32H7_WWDGT_CTL              (GD32H7_WWDGT + GD32H7_WWDGT_CTL_OFFSET)
#define GD32H7_WWDGT_CFG              (GD32H7_WWDGT + GD32H7_WWDGT_CFG_OFFSET)
#define GD32H7_WWDGT_STAT             (GD32H7_WWDGT + GD32H7_WWDGT_STAT_OFFSET)

#define WWDGT_CTL_CNT_SHIFT           (0)
#define WWDGT_CTL_CNT_MASK            (0x7f << WWDGT_CTL_CNT_SHIFT)
#define WWDGT_CTL_WDGTEN              (1 << 7)

#define WWDGT_CFG_WIN_SHIFT           (0)
#define WWDGT_CFG_WIN_MASK            (0x7f << WWDGT_CFG_WIN_SHIFT)
#define WWDGT_CFG_PSC_SHIFT           (7)
#define WWDGT_CFG_PSC_MASK            (3 << WWDGT_CFG_PSC_SHIFT)
#define WWDGT_CFG_EWIE                (1 << 9)

#define WWDGT_STAT_EWIF               (1 << 0)

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_WDG_H */
