/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_dac.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_GD32H7XX_DAC_H
#define __ARCH_ARM_SRC_GD32H7_GD32H7XX_DAC_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>

#include "hardware/gd32h73x_memorymap.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define GD32H7_DAC0                   GD32H7_DAC0_BASE

/* Register offsets ------------------------------------------------------ */

#define GD32H7_DAC_CTL0_OFFSET        0x0000  /* Control register 0 */
#define GD32H7_DAC_SWT_OFFSET         0x0004  /* Software trigger */
#define GD32H7_DAC_OUT0_R12DH_OFFSET  0x0008  /* OUT0 12-bit right data */
#define GD32H7_DAC_OUT1_R12DH_OFFSET  0x0014  /* OUT1 12-bit right data */
#define GD32H7_DAC_OUT0_DO_OFFSET     0x002c  /* OUT0 data output */
#define GD32H7_DAC_OUT1_DO_OFFSET     0x0030  /* OUT1 data output */
#define GD32H7_DAC_STAT0_OFFSET       0x0034  /* Status register 0 */

/* Register addresses ---------------------------------------------------- */

#define GD32H7_DAC_CTL0(d)            ((d) + GD32H7_DAC_CTL0_OFFSET)
#define GD32H7_DAC_SWT(d)             ((d) + GD32H7_DAC_SWT_OFFSET)
#define GD32H7_DAC_OUT0_R12DH(d)      ((d) + GD32H7_DAC_OUT0_R12DH_OFFSET)
#define GD32H7_DAC_OUT1_R12DH(d)      ((d) + GD32H7_DAC_OUT1_R12DH_OFFSET)
#define GD32H7_DAC_OUT0_DO(d)         ((d) + GD32H7_DAC_OUT0_DO_OFFSET)
#define GD32H7_DAC_OUT1_DO(d)         ((d) + GD32H7_DAC_OUT1_DO_OFFSET)
#define GD32H7_DAC_STAT0(d)           ((d) + GD32H7_DAC_STAT0_OFFSET)

/* CTL0 */

#define DAC_CTL0_DEN0                 (1 << 0)   /* OUT0 enable */
#define DAC_CTL0_DTEN0                (1 << 1)   /* OUT0 trigger enable */
#define DAC_CTL0_DEN1                 (1 << 16)  /* OUT1 enable */
#define DAC_CTL0_DTEN1                (1 << 17)   /* OUT1 trigger enable */

/* SWT */

#define DAC_SWT_SWTR0                 (1 << 0)   /* OUT0 software trigger */
#define DAC_SWT_SWTR1                 (1 << 1)   /* OUT1 software trigger */

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

int      gd32_dac_initialize(int channel);
int      gd32_dac_setvalue(int channel, uint16_t value);
uint16_t gd32_dac_getvalue(int channel);

#ifdef CONFIG_DAC
struct dac_dev_s;
FAR struct dac_dev_s *gd32_dac_register(FAR const char *path, int channel);
#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __ARCH_ARM_SRC_GD32H7_GD32H7XX_DAC_H */
