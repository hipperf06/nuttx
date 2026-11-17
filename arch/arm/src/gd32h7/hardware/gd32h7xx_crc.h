/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h7xx_crc.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_CRC_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_CRC_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "chip.h"
#include "hardware/gd32h73x_memorymap.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define GD32H7_CRC                    GD32H7_CRC_BASE

#define GD32H7_CRC_DATA_OFFSET        0x0000  /* Data register */
#define GD32H7_CRC_FDATA_OFFSET       0x0004  /* Free data register */
#define GD32H7_CRC_CTL_OFFSET         0x0008  /* Control register */
#define GD32H7_CRC_IDATA_OFFSET       0x0010  /* Independent data register */
#define GD32H7_CRC_POLY_OFFSET        0x0014  /* Polynomial register */

#define GD32H7_CRC_DATA               (GD32H7_CRC + GD32H7_CRC_DATA_OFFSET)
#define GD32H7_CRC_FDATA              (GD32H7_CRC + GD32H7_CRC_FDATA_OFFSET)
#define GD32H7_CRC_CTL                (GD32H7_CRC + GD32H7_CRC_CTL_OFFSET)
#define GD32H7_CRC_IDATA              (GD32H7_CRC + GD32H7_CRC_IDATA_OFFSET)
#define GD32H7_CRC_POLY               (GD32H7_CRC + GD32H7_CRC_POLY_OFFSET)

/* CRC_CTL */

#define CRC_CTL_RST                   (1 << 0)   /* Reset the CRC calculator */
#define CRC_CTL_PS_SHIFT              (3)        /* Polynomial size */
#define CRC_CTL_PS_MASK               (3 << CRC_CTL_PS_SHIFT)
#  define CRC_CTL_PS_32BIT            (0 << CRC_CTL_PS_SHIFT)
#  define CRC_CTL_PS_16BIT            (1 << CRC_CTL_PS_SHIFT)
#  define CRC_CTL_PS_8BIT             (2 << CRC_CTL_PS_SHIFT)
#  define CRC_CTL_PS_7BIT             (3 << CRC_CTL_PS_SHIFT)
#define CRC_CTL_REV_I_SHIFT           (5)        /* Input reflection */
#define CRC_CTL_REV_I_MASK            (3 << CRC_CTL_REV_I_SHIFT)
#  define CRC_CTL_REV_I_NONE          (0 << CRC_CTL_REV_I_SHIFT)
#  define CRC_CTL_REV_I_BYTE          (1 << CRC_CTL_REV_I_SHIFT)
#  define CRC_CTL_REV_I_HALFWORD      (2 << CRC_CTL_REV_I_SHIFT)
#  define CRC_CTL_REV_I_WORD          (3 << CRC_CTL_REV_I_SHIFT)
#define CRC_CTL_REV_O                 (1 << 7)   /* Output reflection */

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_CRC_H */
