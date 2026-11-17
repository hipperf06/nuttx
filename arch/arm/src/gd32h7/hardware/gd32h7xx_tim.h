/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h7xx_tim.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_TIM_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_TIM_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "chip.h"
#include "hardware/gd32h73x_memorymap.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Timer instances */

#define GD32H7_TIMER0                 GD32H7_TIMER0_BASE
#define GD32H7_TIMER1                 GD32H7_TIMER1_BASE
#define GD32H7_TIMER2                 GD32H7_TIMER2_BASE
#define GD32H7_TIMER3                 GD32H7_TIMER3_BASE
#define GD32H7_TIMER4                 GD32H7_TIMER4_BASE
#define GD32H7_TIMER5                 GD32H7_TIMER5_BASE
#define GD32H7_TIMER6                 GD32H7_TIMER6_BASE
#define GD32H7_TIMER7                 GD32H7_TIMER7_BASE
#define GD32H7_TIMER14                GD32H7_TIMER14_BASE
#define GD32H7_TIMER15                GD32H7_TIMER15_BASE
#define GD32H7_TIMER16                GD32H7_TIMER16_BASE
#define GD32H7_TIMER22                GD32H7_TIMER22_BASE
#define GD32H7_TIMER23                GD32H7_TIMER23_BASE
#define GD32H7_TIMER30                GD32H7_TIMER30_BASE
#define GD32H7_TIMER31                GD32H7_TIMER31_BASE
#define GD32H7_TIMER40                GD32H7_TIMER40_BASE
#define GD32H7_TIMER41                GD32H7_TIMER41_BASE
#define GD32H7_TIMER42                GD32H7_TIMER42_BASE
#define GD32H7_TIMER43                GD32H7_TIMER43_BASE
#define GD32H7_TIMER44                GD32H7_TIMER44_BASE
#define GD32H7_TIMER50                GD32H7_TIMER50_BASE
#define GD32H7_TIMER51                GD32H7_TIMER51_BASE

/* Register offsets ------------------------------------------------------ */

#define GD32H7_TIM_CTL0_OFFSET        0x0000  /* Control register 0 */
#define GD32H7_TIM_CTL1_OFFSET        0x0004  /* Control register 1 */
#define GD32H7_TIM_SMCFG_OFFSET       0x0008  /* Slave mode config */
#define GD32H7_TIM_DMAINTEN_OFFSET    0x000c  /* DMA/interrupt enable */
#define GD32H7_TIM_INTF_OFFSET        0x0010  /* Interrupt flag */
#define GD32H7_TIM_SWEVG_OFFSET       0x0014  /* Software event generate */
#define GD32H7_TIM_CHCTL0_OFFSET      0x0018  /* Channel control 0 */
#define GD32H7_TIM_CHCTL1_OFFSET      0x001c  /* Channel control 1 */
#define GD32H7_TIM_CHCTL2_OFFSET      0x0020  /* Channel control 2 */
#define GD32H7_TIM_CNT_OFFSET         0x0024  /* Counter */
#define GD32H7_TIM_PSC_OFFSET         0x0028  /* Prescaler */
#define GD32H7_TIM_CAR_OFFSET         0x002c  /* Counter auto reload */
#define GD32H7_TIM_CREP0_OFFSET       0x0030  /* Counter repetition 0 */
#define GD32H7_TIM_CH0CV_OFFSET       0x0034  /* Channel 0 capture/compare */
#define GD32H7_TIM_CH1CV_OFFSET       0x0038  /* Channel 1 capture/compare */
#define GD32H7_TIM_CH2CV_OFFSET       0x003c  /* Channel 2 capture/compare */
#define GD32H7_TIM_CH3CV_OFFSET       0x0040  /* Channel 3 capture/compare */
#define GD32H7_TIM_CCHP_OFFSET        0x0044  /* Complementary protection */
#define GD32H7_TIM_DMACFG_OFFSET      0x00e0  /* DMA configuration (burst) */

/* Register addresses ---------------------------------------------------- */

#define GD32H7_TIM_CTL0(t)            ((t) + GD32H7_TIM_CTL0_OFFSET)
#define GD32H7_TIM_CTL1(t)            ((t) + GD32H7_TIM_CTL1_OFFSET)
#define GD32H7_TIM_SMCFG(t)           ((t) + GD32H7_TIM_SMCFG_OFFSET)
#define GD32H7_TIM_DMAINTEN(t)        ((t) + GD32H7_TIM_DMAINTEN_OFFSET)
#define GD32H7_TIM_INTF(t)            ((t) + GD32H7_TIM_INTF_OFFSET)
#define GD32H7_TIM_SWEVG(t)           ((t) + GD32H7_TIM_SWEVG_OFFSET)
#define GD32H7_TIM_CHCTL0(t)          ((t) + GD32H7_TIM_CHCTL0_OFFSET)
#define GD32H7_TIM_CHCTL1(t)          ((t) + GD32H7_TIM_CHCTL1_OFFSET)
#define GD32H7_TIM_CHCTL2(t)          ((t) + GD32H7_TIM_CHCTL2_OFFSET)
#define GD32H7_TIM_CNT(t)             ((t) + GD32H7_TIM_CNT_OFFSET)
#define GD32H7_TIM_PSC(t)             ((t) + GD32H7_TIM_PSC_OFFSET)
#define GD32H7_TIM_CAR(t)             ((t) + GD32H7_TIM_CAR_OFFSET)
#define GD32H7_TIM_CREP0(t)           ((t) + GD32H7_TIM_CREP0_OFFSET)
#define GD32H7_TIM_CH0CV(t)           ((t) + GD32H7_TIM_CH0CV_OFFSET)
#define GD32H7_TIM_CH1CV(t)           ((t) + GD32H7_TIM_CH1CV_OFFSET)
#define GD32H7_TIM_CH2CV(t)           ((t) + GD32H7_TIM_CH2CV_OFFSET)
#define GD32H7_TIM_CH3CV(t)           ((t) + GD32H7_TIM_CH3CV_OFFSET)
#define GD32H7_TIM_CCHP(t)            ((t) + GD32H7_TIM_CCHP_OFFSET)
#define GD32H7_TIM_DMACFG(t)          ((t) + GD32H7_TIM_DMACFG_OFFSET)

/* CTL0 */

#define TIM_CTL0_CEN                  (1 << 0)   /* Counter enable */
#define TIM_CTL0_UPDIS                (1 << 1)   /* Update disable */
#define TIM_CTL0_UPS                  (1 << 2)   /* Update source */
#define TIM_CTL0_SPM                  (1 << 3)   /* Single pulse mode */
#define TIM_CTL0_DIR                  (1 << 4)   /* Counting direction */
#define TIM_CTL0_CAM_SHIFT            (5)        /* Center-aligned mode */
#define TIM_CTL0_CAM_MASK             (3 << TIM_CTL0_CAM_SHIFT)
#define TIM_CTL0_ARSE                 (1 << 7)   /* Auto-reload shadow enable */
#define TIM_CTL0_CKDIV_SHIFT          (8)        /* Clock division */
#define TIM_CTL0_CKDIV_MASK           (3 << TIM_CTL0_CKDIV_SHIFT)

/* DMAINTEN / INTF / SWEVG */

#define TIM_INT_UP                    (1 << 0)
#define TIM_INT_CH0                   (1 << 1)
#define TIM_INT_CH1                   (1 << 2)
#define TIM_INT_CH2                   (1 << 3)
#define TIM_INT_CH3                   (1 << 4)
#define TIM_INT_CMT                   (1 << 5)
#define TIM_INT_TRG                   (1 << 6)
#define TIM_INT_BRK                   (1 << 7)

/* DMA/interrupt enable */

#define TIM_DMAINTEN_UPDEN            (1 << 8)   /* Update DMA request */
#define TIM_DMAINTEN_CH0DEN           (1 << 9)   /* Channel 0 DMA request */
#define TIM_DMAINTEN_CH1DEN           (1 << 10)  /* Channel 1 DMA request */
#define TIM_DMAINTEN_CH2DEN           (1 << 11)  /* Channel 2 DMA request */
#define TIM_DMAINTEN_CH3DEN           (1 << 12)  /* Channel 3 DMA request */

/* DMA configuration (burst start address / transfer count) */

#define TIM_DMACFG_DMATA_SHIFT        (0)
#define TIM_DMACFG_DMATA_MASK         (0x3f << TIM_DMACFG_DMATA_SHIFT)
#define TIM_DMACFG_DMATC_SHIFT        (8)
#define TIM_DMACFG_DMATC_MASK         (0x3f << TIM_DMACFG_DMATC_SHIFT)

/* DMA burst base addresses (register offset / 4) */

#define TIM_DMABASE_CTL0              0x00
#define TIM_DMABASE_CH0CV             0x0d
#define TIM_DMABASE_CH1CV             0x0e
#define TIM_DMABASE_CH2CV             0x0f
#define TIM_DMABASE_CH3CV             0x10

#define TIM_INTF_UPIF                 (1 << 0)
#define TIM_INTF_CH0IF                (1 << 1)
#define TIM_INTF_CH1IF                (1 << 2)
#define TIM_INTF_CH2IF                (1 << 3)
#define TIM_INTF_CH3IF                (1 << 4)
#define TIM_INTF_CMTIF                (1 << 5)
#define TIM_INTF_TRGIF                (1 << 6)
#define TIM_INTF_BRKIF                (1 << 7)
#define TIM_INTF_CH0OF                (1 << 9)
#define TIM_INTF_CH1OF                (1 << 10)
#define TIM_INTF_CH2OF                (1 << 11)
#define TIM_INTF_CH3OF                (1 << 12)

#define TIM_SWEVG_UPG                 (1 << 0)
#define TIM_SWEVG_CH0G                (1 << 1)
#define TIM_SWEVG_CH1G                (1 << 2)
#define TIM_SWEVG_CH2G                (1 << 3)
#define TIM_SWEVG_CH3G                (1 << 4)

/* CHCTL0/CHCTL1 (output compare) */

#define TIM_CHCTL0_CH0MS_SHIFT        (0)
#define TIM_CHCTL0_CH0MS_MASK         (3 << TIM_CHCTL0_CH0MS_SHIFT)
#define TIM_CHCTL0_CH0COMSEN          (1 << 3)
#define TIM_CHCTL0_CH0COMCTL_SHIFT    (4)
#define TIM_CHCTL0_CH0COMCTL_MASK     (7 << TIM_CHCTL0_CH0COMCTL_SHIFT)
#define TIM_CHCTL0_CH1MS_SHIFT        (8)
#define TIM_CHCTL0_CH1MS_MASK         (3 << TIM_CHCTL0_CH1MS_SHIFT)
#define TIM_CHCTL0_CH1COMSEN          (1 << 11)
#define TIM_CHCTL0_CH1COMCTL_SHIFT    (12)
#define TIM_CHCTL0_CH1COMCTL_MASK     (7 << TIM_CHCTL0_CH1COMCTL_SHIFT)

#define TIM_CHCTL1_CH2MS_SHIFT        (0)
#define TIM_CHCTL1_CH2MS_MASK         (3 << TIM_CHCTL1_CH2MS_SHIFT)
#define TIM_CHCTL1_CH2COMSEN          (1 << 3)
#define TIM_CHCTL1_CH2COMCTL_SHIFT    (4)
#define TIM_CHCTL1_CH2COMCTL_MASK     (7 << TIM_CHCTL1_CH2COMCTL_SHIFT)
#define TIM_CHCTL1_CH3MS_SHIFT        (8)
#define TIM_CHCTL1_CH3MS_MASK         (3 << TIM_CHCTL1_CH3MS_SHIFT)
#define TIM_CHCTL1_CH3COMSEN          (1 << 11)
#define TIM_CHCTL1_CH3COMCTL_SHIFT    (12)
#define TIM_CHCTL1_CH3COMCTL_MASK     (7 << TIM_CHCTL1_CH3COMCTL_SHIFT)

/* CHCTL2 */

#define TIM_CHCTL2_CH0EN              (1 << 0)
#define TIM_CHCTL2_CH0P               (1 << 1)
#define TIM_CHCTL2_CH0NEN             (1 << 2)
#define TIM_CHCTL2_CH0NP              (1 << 3)
#define TIM_CHCTL2_CH1EN              (1 << 4)
#define TIM_CHCTL2_CH1P               (1 << 5)
#define TIM_CHCTL2_CH1NEN             (1 << 6)
#define TIM_CHCTL2_CH1NP              (1 << 7)
#define TIM_CHCTL2_CH2EN              (1 << 8)
#define TIM_CHCTL2_CH2P               (1 << 9)
#define TIM_CHCTL2_CH2NEN             (1 << 10)
#define TIM_CHCTL2_CH2NP              (1 << 11)
#define TIM_CHCTL2_CH3EN              (1 << 12)
#define TIM_CHCTL2_CH3P               (1 << 13)
#define TIM_CHCTL2_CH3NEN             (1 << 14)
#define TIM_CHCTL2_CH3NP              (1 << 15)

/* Output compare modes (value placed in CHxCOMCTL) */

#define TIM_OCMODE_TIMING             (0 << 4)
#define TIM_OCMODE_ACTIVE             (1 << 4)
#define TIM_OCMODE_INACTIVE           (2 << 4)
#define TIM_OCMODE_TOGGLE             (3 << 4)
#define TIM_OCMODE_PWM0               (6 << 4)
#define TIM_OCMODE_PWM1               (7 << 4)

/* CCHP */

#define TIM_CCHP_POEN                 (1 << 15)
#define TIM_CCHP_OAEN                 (1 << 14)

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_TIM_H */
