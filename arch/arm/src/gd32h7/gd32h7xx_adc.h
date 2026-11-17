/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_adc.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_GD32H7XX_ADC_H
#define __ARCH_ARM_SRC_GD32H7_GD32H7XX_ADC_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>

#include "chip.h"
#include "hardware/gd32h73x_memorymap.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* ADC instances */

#define GD32H7_ADC0                   GD32H7_ADC0_BASE
#define GD32H7_ADC1                   GD32H7_ADC1_BASE
#define GD32H7_ADC2                   GD32H7_ADC2_BASE

/* Register offsets ------------------------------------------------------ */

#define GD32H7_ADC_STAT_OFFSET        0x0000  /* Status */
#define GD32H7_ADC_CTL0_OFFSET        0x0004  /* Control 0 */
#define GD32H7_ADC_CTL1_OFFSET        0x0008  /* Control 1 */
#define GD32H7_ADC_RSQ0_OFFSET        0x0024  /* Routine sequence 0 */
#define GD32H7_ADC_RSQ1_OFFSET        0x0028  /* Routine sequence 1 */
#define GD32H7_ADC_RSQ8_OFFSET        0x0044  /* Routine sequence 8 */
#define GD32H7_ADC_RDATA_OFFSET       0x0064  /* Routine data */

/* Register addresses ---------------------------------------------------- */

#define GD32H7_ADC_STAT(a)            ((a) + GD32H7_ADC_STAT_OFFSET)
#define GD32H7_ADC_CTL0(a)            ((a) + GD32H7_ADC_CTL0_OFFSET)
#define GD32H7_ADC_CTL1(a)            ((a) + GD32H7_ADC_CTL1_OFFSET)
#define GD32H7_ADC_RSQ0(a)            ((a) + GD32H7_ADC_RSQ0_OFFSET)
#define GD32H7_ADC_RSQ1(a)            ((a) + GD32H7_ADC_RSQ1_OFFSET)
#define GD32H7_ADC_RSQ8(a)            ((a) + GD32H7_ADC_RSQ8_OFFSET)
#define GD32H7_ADC_RDATA(a)           ((a) + GD32H7_ADC_RDATA_OFFSET)

/* STAT */

#define ADC_STAT_EOC                  (1 << 1)   /* End of conversion */
#define ADC_STAT_EOIC                 (1 << 2)   /* End of inserted conversion */
#define ADC_STAT_STIC                 (1 << 3)   /* Inserted start flag */
#define ADC_STAT_STRC                 (1 << 4)   /* Routine start flag */

/* CTL0 */

#define ADC_CTL0_EOCIE                (1 << 5)   /* EOC interrupt enable */
#define ADC_CTL0_SM                   (1 << 8)   /* Scan mode */
#define ADC_CTL0_DRES_SHIFT           (24)       /* Data resolution */
#define ADC_CTL0_DRES_MASK            (3 << ADC_CTL0_DRES_SHIFT)
#  define ADC_DRES_14BIT              (0 << ADC_CTL0_DRES_SHIFT)
#  define ADC_DRES_12BIT              (1 << ADC_CTL0_DRES_SHIFT)
#  define ADC_DRES_10BIT              (2 << ADC_CTL0_DRES_SHIFT)
#  define ADC_DRES_8BIT               (3 << ADC_CTL0_DRES_SHIFT)

/* CTL1 */

#define ADC_CTL1_ADCON                (1 << 0)   /* ADC converter on */
#define ADC_CTL1_CTN                  (1 << 1)   /* Continuous mode */
#define ADC_CTL1_CLB                  (1 << 2)   /* Calibration */
#define ADC_CTL1_RSTCLB               (1 << 3)   /* Reset calibration */
#define ADC_CTL1_DMA                  (1 << 8)   /* DMA request enable */
#define ADC_CTL1_DDM                  (1 << 9)   /* DMA disable mode */
#define ADC_CTL1_SWICST               (1 << 22)  /* Inserted channel start */
#define ADC_CTL1_SWRCST               (1 << 30)  /* Routine channel start */

/* RSQ0: routine sequence length (RL = length - 1) */

#define ADC_RSQ0_RL_SHIFT             (20)
#define ADC_RSQ0_RL_MASK              (0xf << ADC_RSQ0_RL_SHIFT)

/* Each routine-sequence slot is 16 bits: [15:10] sample time, [4:0] channel */

#define ADC_RSQ_SLOT_SHIFT            (10)
#define ADC_RSQ_SLOT(chan, smp)       ((((uint32_t)(smp) << ADC_RSQ_SLOT_SHIFT)) | \
                                       ((uint32_t)(chan) & 0x1f))

/* Sample time encoding: the 10-bit RSMPN field holds the sample cycle count
 * in units of the ADC clock.  Valid range is 0..809 (ADC0/1) and 0..638
 * (ADC2); the value is used as-is.
 */

#define ADC_SAMPLETIME_CYCLES(n)      ((uint32_t)(n))

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

int gd32_adc_initialize(int port);
int gd32_adc_read_channel(int port, uint8_t channel, uint32_t smp);

/* Register the ADC as a NuttX analog device (/dev/adcN) */

#ifdef CONFIG_ADC
FAR struct adc_dev_s *gd32_adc_register(int port, uint8_t channel,
                                        uint32_t smp);
#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __ARCH_ARM_SRC_GD32H7_GD32H7XX_ADC_H */
