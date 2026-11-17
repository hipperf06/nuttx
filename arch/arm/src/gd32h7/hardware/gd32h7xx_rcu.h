/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h7xx_rcu.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_RCU_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_RCU_H

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

/* Register base address */

#define GD32H7_RCU                    GD32H7_RCU_BASE

/* Register offsets ------------------------------------------------------ */

#define GD32H7_RCU_CTL_OFFSET         0x0000  /* Control register */
#define GD32H7_RCU_PLL0_OFFSET        0x0004  /* PLL0 register */
#define GD32H7_RCU_CFG0_OFFSET        0x0008  /* Clock configuration register 0 */
#define GD32H7_RCU_INT_OFFSET         0x000c  /* Clock interrupt register */
#define GD32H7_RCU_AHB1RST_OFFSET     0x0010  /* AHB1 reset register */
#define GD32H7_RCU_AHB2RST_OFFSET     0x0014  /* AHB2 reset register */
#define GD32H7_RCU_AHB3RST_OFFSET     0x0018  /* AHB3 reset register */
#define GD32H7_RCU_AHB4RST_OFFSET     0x001c  /* AHB4 reset register */
#define GD32H7_RCU_APB1RST_OFFSET     0x0020  /* APB1 reset register */
#define GD32H7_RCU_APB2RST_OFFSET     0x0024  /* APB2 reset register */
#define GD32H7_RCU_APB3RST_OFFSET     0x0028  /* APB3 reset register */
#define GD32H7_RCU_APB4RST_OFFSET     0x002c  /* APB4 reset register */
#define GD32H7_RCU_AHB1EN_OFFSET      0x0030  /* AHB1 enable register */
#define GD32H7_RCU_AHB2EN_OFFSET      0x0034  /* AHB2 enable register */
#define GD32H7_RCU_AHB3EN_OFFSET      0x0038  /* AHB3 enable register */
#define GD32H7_RCU_AHB4EN_OFFSET      0x003c  /* AHB4 enable register */
#define GD32H7_RCU_APB1EN_OFFSET      0x0040  /* APB1 enable register */
#define GD32H7_RCU_APB2EN_OFFSET      0x0044  /* APB2 enable register */
#define GD32H7_RCU_APB3EN_OFFSET      0x0048  /* APB3 enable register */
#define GD32H7_RCU_APB4EN_OFFSET      0x004c  /* APB4 enable register */
#define GD32H7_RCU_AHB1SPEN_OFFSET    0x0050  /* AHB1 sleep mode enable */
#define GD32H7_RCU_AHB2SPEN_OFFSET    0x0054  /* AHB2 sleep mode enable */
#define GD32H7_RCU_AHB3SPEN_OFFSET    0x0058  /* AHB3 sleep mode enable */
#define GD32H7_RCU_AHB4SPEN_OFFSET    0x005c  /* AHB4 sleep mode enable */
#define GD32H7_RCU_APB1SPEN_OFFSET    0x0060  /* APB1 sleep mode enable */
#define GD32H7_RCU_APB2SPEN_OFFSET    0x0064  /* APB2 sleep mode enable */
#define GD32H7_RCU_APB3SPEN_OFFSET    0x0068  /* APB3 sleep mode enable */
#define GD32H7_RCU_APB4SPEN_OFFSET    0x006c  /* APB4 sleep mode enable */
#define GD32H7_RCU_BDCTL_OFFSET       0x0070  /* Backup domain control */
#define GD32H7_RCU_RSTSCK_OFFSET      0x0074  /* Reset source / clock register */
#define GD32H7_RCU_PLLADDCTL_OFFSET   0x0080  /* PLL additional control */
#define GD32H7_RCU_PLL1_OFFSET        0x0084  /* PLL1 register */
#define GD32H7_RCU_PLL2_OFFSET        0x0088  /* PLL2 register */
#define GD32H7_RCU_CFG1_OFFSET        0x008c  /* Clock configuration register 1 */
#define GD32H7_RCU_CFG2_OFFSET        0x0090  /* Clock configuration register 2 */
#define GD32H7_RCU_CFG3_OFFSET        0x0094  /* Clock configuration register 3 */
#define GD32H7_RCU_PLLALL_OFFSET      0x0098  /* PLL configuration register */
#define GD32H7_RCU_PLL0FRA_OFFSET     0x009c  /* PLL0 fraction configuration */
#define GD32H7_RCU_PLL1FRA_OFFSET     0x00a0  /* PLL1 fraction configuration */
#define GD32H7_RCU_PLL2FRA_OFFSET     0x00a4  /* PLL2 fraction configuration */
#define GD32H7_RCU_ADDCTL0_OFFSET     0x00c0  /* Additional clock control 0 */
#define GD32H7_RCU_ADDCTL1_OFFSET     0x00c4  /* Additional clock control 1 */
#define GD32H7_RCU_ADDINT_OFFSET      0x00cc  /* Additional clock interrupt */
#define GD32H7_RCU_CFG4_OFFSET        0x00d0  /* Clock configuration register 4 */
#define GD32H7_RCU_USBCLKCTL_OFFSET   0x00d4  /* USB clock control */
#define GD32H7_RCU_PLLUSBCFG_OFFSET   0x00d8  /* PLLUSB configuration */
#define GD32H7_RCU_ADDAPB2RST_OFFSET  0x00e0  /* APB2 additional reset */
#define GD32H7_RCU_ADDAPB2EN_OFFSET   0x00e4  /* APB2 additional enable */
#define GD32H7_RCU_ADDAPB2SPEN_OFFSET 0x00e8  /* APB2 additional sleep enable */
#define GD32H7_RCU_CFG5_OFFSET        0x00f0  /* Clock configuration register 5 */

/* Register addresses ---------------------------------------------------- */

#define GD32H7_RCU_CTL                (GD32H7_RCU + GD32H7_RCU_CTL_OFFSET)
#define GD32H7_RCU_PLL0               (GD32H7_RCU + GD32H7_RCU_PLL0_OFFSET)
#define GD32H7_RCU_CFG0               (GD32H7_RCU + GD32H7_RCU_CFG0_OFFSET)
#define GD32H7_RCU_INT                (GD32H7_RCU + GD32H7_RCU_INT_OFFSET)
#define GD32H7_RCU_AHB1RST            (GD32H7_RCU + GD32H7_RCU_AHB1RST_OFFSET)
#define GD32H7_RCU_AHB2RST            (GD32H7_RCU + GD32H7_RCU_AHB2RST_OFFSET)
#define GD32H7_RCU_AHB3RST            (GD32H7_RCU + GD32H7_RCU_AHB3RST_OFFSET)
#define GD32H7_RCU_AHB4RST            (GD32H7_RCU + GD32H7_RCU_AHB4RST_OFFSET)
#define GD32H7_RCU_APB1RST            (GD32H7_RCU + GD32H7_RCU_APB1RST_OFFSET)
#define GD32H7_RCU_APB2RST            (GD32H7_RCU + GD32H7_RCU_APB2RST_OFFSET)
#define GD32H7_RCU_APB3RST            (GD32H7_RCU + GD32H7_RCU_APB3RST_OFFSET)
#define GD32H7_RCU_APB4RST            (GD32H7_RCU + GD32H7_RCU_APB4RST_OFFSET)
#define GD32H7_RCU_AHB1EN             (GD32H7_RCU + GD32H7_RCU_AHB1EN_OFFSET)
#define GD32H7_RCU_AHB2EN             (GD32H7_RCU + GD32H7_RCU_AHB2EN_OFFSET)
#define GD32H7_RCU_AHB3EN             (GD32H7_RCU + GD32H7_RCU_AHB3EN_OFFSET)
#define GD32H7_RCU_AHB4EN             (GD32H7_RCU + GD32H7_RCU_AHB4EN_OFFSET)
#define GD32H7_RCU_APB1EN             (GD32H7_RCU + GD32H7_RCU_APB1EN_OFFSET)
#define GD32H7_RCU_APB2EN             (GD32H7_RCU + GD32H7_RCU_APB2EN_OFFSET)
#define GD32H7_RCU_APB3EN             (GD32H7_RCU + GD32H7_RCU_APB3EN_OFFSET)
#define GD32H7_RCU_APB4EN             (GD32H7_RCU + GD32H7_RCU_APB4EN_OFFSET)
#define GD32H7_RCU_AHB1SPEN           (GD32H7_RCU + GD32H7_RCU_AHB1SPEN_OFFSET)
#define GD32H7_RCU_AHB2SPEN           (GD32H7_RCU + GD32H7_RCU_AHB2SPEN_OFFSET)
#define GD32H7_RCU_AHB3SPEN           (GD32H7_RCU + GD32H7_RCU_AHB3SPEN_OFFSET)
#define GD32H7_RCU_AHB4SPEN           (GD32H7_RCU + GD32H7_RCU_AHB4SPEN_OFFSET)
#define GD32H7_RCU_APB1SPEN           (GD32H7_RCU + GD32H7_RCU_APB1SPEN_OFFSET)
#define GD32H7_RCU_APB2SPEN           (GD32H7_RCU + GD32H7_RCU_APB2SPEN_OFFSET)
#define GD32H7_RCU_APB3SPEN           (GD32H7_RCU + GD32H7_RCU_APB3SPEN_OFFSET)
#define GD32H7_RCU_APB4SPEN           (GD32H7_RCU + GD32H7_RCU_APB4SPEN_OFFSET)
#define GD32H7_RCU_BDCTL              (GD32H7_RCU + GD32H7_RCU_BDCTL_OFFSET)
#define GD32H7_RCU_RSTSCK             (GD32H7_RCU + GD32H7_RCU_RSTSCK_OFFSET)
#define GD32H7_RCU_PLLADDCTL          (GD32H7_RCU + GD32H7_RCU_PLLADDCTL_OFFSET)
#define GD32H7_RCU_PLL1               (GD32H7_RCU + GD32H7_RCU_PLL1_OFFSET)
#define GD32H7_RCU_PLL2               (GD32H7_RCU + GD32H7_RCU_PLL2_OFFSET)
#define GD32H7_RCU_CFG1               (GD32H7_RCU + GD32H7_RCU_CFG1_OFFSET)
#define GD32H7_RCU_CFG2               (GD32H7_RCU + GD32H7_RCU_CFG2_OFFSET)
#define GD32H7_RCU_CFG3               (GD32H7_RCU + GD32H7_RCU_CFG3_OFFSET)
#define GD32H7_RCU_PLLALL             (GD32H7_RCU + GD32H7_RCU_PLLALL_OFFSET)
#define GD32H7_RCU_PLL0FRA            (GD32H7_RCU + GD32H7_RCU_PLL0FRA_OFFSET)
#define GD32H7_RCU_PLL1FRA            (GD32H7_RCU + GD32H7_RCU_PLL1FRA_OFFSET)
#define GD32H7_RCU_PLL2FRA            (GD32H7_RCU + GD32H7_RCU_PLL2FRA_OFFSET)
#define GD32H7_RCU_ADDCTL0            (GD32H7_RCU + GD32H7_RCU_ADDCTL0_OFFSET)
#define GD32H7_RCU_ADDCTL1            (GD32H7_RCU + GD32H7_RCU_ADDCTL1_OFFSET)
#define GD32H7_RCU_ADDINT             (GD32H7_RCU + GD32H7_RCU_ADDINT_OFFSET)
#define GD32H7_RCU_CFG4               (GD32H7_RCU + GD32H7_RCU_CFG4_OFFSET)
#define GD32H7_RCU_USBCLKCTL          (GD32H7_RCU + GD32H7_RCU_USBCLKCTL_OFFSET)
#define GD32H7_RCU_PLLUSBCFG          (GD32H7_RCU + GD32H7_RCU_PLLUSBCFG_OFFSET)
#define GD32H7_RCU_ADDAPB2RST         (GD32H7_RCU + GD32H7_RCU_ADDAPB2RST_OFFSET)
#define GD32H7_RCU_ADDAPB2EN          (GD32H7_RCU + GD32H7_RCU_ADDAPB2EN_OFFSET)
#define GD32H7_RCU_ADDAPB2SPEN        (GD32H7_RCU + GD32H7_RCU_ADDAPB2SPEN_OFFSET)
#define GD32H7_RCU_CFG5               (GD32H7_RCU + GD32H7_RCU_CFG5_OFFSET)

/* Register bit definitions ---------------------------------------------- */

/* RCU_CTL */

#define RCU_CTL_IRC64MADJ             (0x7f << 0)           /* Bits 0-6: IRC64M trim adjust value */
#define RCU_CTL_IRC64MCALIB           (0x1ff << 7)          /* Bits 7-15: IRC64M calibration value */
#define RCU_CTL_HXTALEN               (1 << 16)             /* Bit 16: HXTAL enable */
#define RCU_CTL_HXTALSTB              (1 << 17)             /* Bit 17: HXTAL stabilization flag */
#define RCU_CTL_HXTALBPS              (1 << 18)             /* Bit 18: HXTAL bypass mode enable */
#define RCU_CTL_CKMEN                 (1 << 19)             /* Bit 19: HXTAL clock monitor enable */
#define RCU_CTL_PLL0EN                (1 << 24)             /* Bit 24: PLL0 enable */
#define RCU_CTL_PLL0STB               (1 << 25)             /* Bit 25: PLL0 stabilization flag */
#define RCU_CTL_PLL1EN                (1 << 26)             /* Bit 26: PLL1 enable */
#define RCU_CTL_PLL1STB               (1 << 27)             /* Bit 27: PLL1 stabilization flag */
#define RCU_CTL_PLL2EN                (1 << 28)             /* Bit 28: PLL2 enable */
#define RCU_CTL_PLL2STB               (1 << 29)             /* Bit 29: PLL2 stabilization flag */
#define RCU_CTL_IRC64MEN              (1 << 30)             /* Bit 30: IRC64M enable */
#define RCU_CTL_IRC64MSTB             (1 << 31)             /* Bit 31: IRC64M stabilization flag */

/* RCU_PLL0 */

#define RCU_PLL0_PLL0PSC              (0x3f << 0)           /* Bits 0-5: PLL0 VCO source clock prescaler */
#define RCU_PLL0_PLL0N                (0x1ff << 6)          /* Bits 6-14: PLL0 VCO clock multiplication factor */
#define RCU_PLL0_PLL0P                (0x7f << 16)          /* Bits 16-22: PLL0P output division factor */
#define RCU_PLL0_PLL0R                (0x7f << 24)          /* Bits 24-30: PLL0R output division factor */
#define RCU_PLL0_PLLSTBSRC            (1 << 31)             /* Bit 31: PLLs stabilization signal source */

/* RCU_CFG0 */

#define RCU_CFG0_SCS                  (0x3 << 0)            /* Bits 0-1: System clock switch */
#define RCU_CFG0_SCSS                 (0x3 << 2)            /* Bits 2-3: System clock switch status */
#define RCU_CFG0_AHBPSC               (0xf << 4)            /* Bits 4-7: AHB prescaler */
#define RCU_CFG0_APB1PSC              (0x7 << 10)           /* Bits 10-12: APB1 prescaler */
#define RCU_CFG0_APB2PSC              (0x7 << 13)           /* Bits 13-15: APB2 prescaler */
#define RCU_CFG0_RTCDIV               (0x3f << 16)          /* Bits 16-21: RTC divider from HXTAL */
#define RCU_CFG0_APB4PSC              (0x7 << 24)           /* Bits 24-26: APB4 prescaler */
#define RCU_CFG0_APB3PSC              (0x7 << 27)           /* Bits 27-29: APB3 prescaler */
#define RCU_CFG0_I2C0SEL              (0x3 << 30)           /* Bits 30-31: I2C0 clock source */

#define RCU_CFG0_SCS_IRC64MDIV        (0 << 0)              /* CK_SYS = IRC64MDIV */
#define RCU_CFG0_SCS_HXTAL            (1 << 0)              /* CK_SYS = HXTAL */
#define RCU_CFG0_SCS_LPIRC4M          (2 << 0)              /* CK_SYS = LPIRC4M */
#define RCU_CFG0_SCS_PLL0P            (3 << 0)              /* CK_SYS = PLL0P */

#define RCU_CFG0_SCSS_IRC64MDIV       (0 << 2)
#define RCU_CFG0_SCSS_HXTAL           (1 << 2)
#define RCU_CFG0_SCSS_LPIRC4M         (2 << 2)
#define RCU_CFG0_SCSS_PLL0P           (3 << 2)

#define RCU_CFG0_AHBPSC_DIV1          (0 << 4)
#define RCU_CFG0_AHBPSC_DIV2          (8 << 4)
#define RCU_CFG0_AHBPSC_DIV4          (9 << 4)
#define RCU_CFG0_AHBPSC_DIV8          (10 << 4)
#define RCU_CFG0_AHBPSC_DIV16         (11 << 4)
#define RCU_CFG0_AHBPSC_DIV64         (12 << 4)
#define RCU_CFG0_AHBPSC_DIV128        (13 << 4)
#define RCU_CFG0_AHBPSC_DIV256        (14 << 4)
#define RCU_CFG0_AHBPSC_DIV512        (15 << 4)

#define RCU_CFG0_APB1PSC_DIV1         (0 << 10)
#define RCU_CFG0_APB1PSC_DIV2         (4 << 10)
#define RCU_CFG0_APB1PSC_DIV4         (5 << 10)
#define RCU_CFG0_APB1PSC_DIV8         (6 << 10)
#define RCU_CFG0_APB1PSC_DIV16        (7 << 10)

#define RCU_CFG0_APB2PSC_DIV1         (0 << 13)
#define RCU_CFG0_APB2PSC_DIV2         (4 << 13)
#define RCU_CFG0_APB2PSC_DIV4         (5 << 13)
#define RCU_CFG0_APB2PSC_DIV8         (6 << 13)
#define RCU_CFG0_APB2PSC_DIV16        (7 << 13)

#define RCU_CFG0_APB3PSC_DIV1         (0 << 27)
#define RCU_CFG0_APB3PSC_DIV2         (4 << 27)
#define RCU_CFG0_APB3PSC_DIV4         (5 << 27)
#define RCU_CFG0_APB3PSC_DIV8         (6 << 27)
#define RCU_CFG0_APB3PSC_DIV16        (7 << 27)

#define RCU_CFG0_APB4PSC_DIV1         (0 << 24)
#define RCU_CFG0_APB4PSC_DIV2         (4 << 24)
#define RCU_CFG0_APB4PSC_DIV4         (5 << 24)
#define RCU_CFG0_APB4PSC_DIV8         (6 << 24)
#define RCU_CFG0_APB4PSC_DIV16        (7 << 24)

/* RCU_PLLALL */

#define RCU_PLLALL_PLL0RNG            (0x3 << 0)            /* Bits 0-1: PLL0 input clock range */
#define RCU_PLLALL_PLL0VCOSEL         (1 << 2)              /* Bit 2: PLL0 VCO selection */
#define RCU_PLLALL_PLL1RNG            (0x3 << 4)            /* Bits 4-5: PLL1 input clock range */
#define RCU_PLLALL_PLL1VCOSEL         (1 << 6)              /* Bit 6: PLL1 VCO selection */
#define RCU_PLLALL_PLL2RNG            (0x3 << 8)            /* Bits 8-9: PLL2 input clock range */
#define RCU_PLLALL_PLL2VCOSEL         (1 << 10)             /* Bit 10: PLL2 VCO selection */
#define RCU_PLLALL_PLLSEL             (0x3 << 16)           /* Bits 16-17: PLLs clock source selection */

#define RCU_PLLALL_PLLSRC_IRC64MDIV   (0 << 16)
#define RCU_PLLALL_PLLSRC_LPIRC4M     (1 << 16)
#define RCU_PLLALL_PLLSRC_HXTAL       (2 << 16)

#define RCU_PLL0RNG_1M_2M             (0 << 0)
#define RCU_PLL0RNG_2M_4M             (1 << 0)
#define RCU_PLL0RNG_4M_8M             (2 << 0)
#define RCU_PLL0RNG_8M_16M            (3 << 0)

/* RCU_PLLADDCTL */

#define RCU_PLLADDCTL_PLL0Q           (0x7f << 0)           /* Bits 0-6: PLL0Q output division factor */
#define RCU_PLLADDCTL_PLL1Q           (0x7f << 8)           /* Bits 8-14: PLL1Q output division factor */
#define RCU_PLLADDCTL_PLL2Q           (0x7f << 16)          /* Bits 16-22: PLL2Q output division factor */
#define RCU_PLLADDCTL_PLL0QEN         (1 << 23)             /* Bit 23: PLL0Q divider output enable */
#define RCU_PLLADDCTL_PLL0REN         (1 << 24)             /* Bit 24: PLL0R divider output enable */
#define RCU_PLLADDCTL_PLL0PEN         (1 << 25)             /* Bit 25: PLL0P divider output enable */

/* RCU_ADDCTL1 */

#define RCU_ADDCTL1_LPIRC4MEN         (1 << 0)              /* Bit 0: LPIRC4M clock enable */
#define RCU_ADDCTL1_LPIRC4MSTB        (1 << 1)              /* Bit 1: LPIRC4M stabilization flag */
#define RCU_ADDCTL1_LPIRC4MTRIM       (0x3f << 2)           /* Bits 2-7: LPIRC4M trim adjust value */
#define RCU_ADDCTL1_LPIRC4MCAL        (0xff << 8)           /* Bits 8-15: LPIRC4M calibration value */
#define RCU_ADDCTL1_IRC64MDIV         (0x3 << 16)           /* Bits 16-17: IRC64M clock divider */
#define RCU_ADDCTL1_LPIRC4MDSPEN      (1 << 20)             /* Bit 20: LPIRC4M clock enable in deep-sleep */
#define RCU_ADDCTL1_PLLUSBHS0EN       (1 << 28)             /* Bit 28: PLLUSBHS0 enable */
#define RCU_ADDCTL1_PLLUSBHS0STB      (1 << 29)             /* Bit 29: PLLUSBHS0 stabilization flag */
#define RCU_ADDCTL1_PLLUSBHS1EN       (1 << 30)             /* Bit 30: PLLUSBHS1 enable */
#define RCU_ADDCTL1_PLLUSBHS1STB      (1 << 31)             /* Bit 31: PLLUSBHS1 stabilization flag */

#define RCU_IRC64M_DIV1               (0 << 16)
#define RCU_IRC64M_DIV2               (1 << 16)
#define RCU_IRC64M_DIV4               (2 << 16)
#define RCU_IRC64M_DIV8               (3 << 16)

/* RCU_BDCTL */

#define RCU_BDCTL_LXTALEN             (1 << 0)              /* Bit 0: LXTAL enable */
#define RCU_BDCTL_LXTALSTB            (1 << 1)              /* Bit 1: LXTAL stabilization flag */
#define RCU_BDCTL_LXTALBPS            (1 << 2)              /* Bit 2: LXTAL bypass mode enable */
#define RCU_BDCTL_LXTALDRI            (0x3 << 3)            /* Bits 3-4: LXTAL drive capability */
#define RCU_BDCTL_LCKMEN              (1 << 5)              /* Bit 5: LXTAL clock monitor enable */
#define RCU_BDCTL_LCKMD               (1 << 6)              /* Bit 6: LXTAL clock failure detection flag */
#define RCU_BDCTL_RTCSRC              (0x3 << 8)            /* Bits 8-9: RTC clock entry selection */
#define RCU_BDCTL_RTCEN               (1 << 15)             /* Bit 15: RTC clock enable */
#define RCU_BDCTL_BKPRST              (1 << 16)             /* Bit 16: Backup domain reset */

/* RCU_RSTSCK */

#define RCU_RSTSCK_IRC32KEN           (1 << 0)              /* Bit 0: IRC32K enable */
#define RCU_RSTSCK_IRC32KSTB          (1 << 1)              /* Bit 1: IRC32K stabilization flag */
#define RCU_RSTSCK_RSTFC              (1 << 24)             /* Bit 24: Reset flag clear */
#define RCU_RSTSCK_BORRSTF            (1 << 25)             /* Bit 25: BOR reset flag */
#define RCU_RSTSCK_EPRSTF             (1 << 26)             /* Bit 26: External pin reset flag */
#define RCU_RSTSCK_PORRSTF            (1 << 27)             /* Bit 27: Power reset flag */
#define RCU_RSTSCK_SWRSTF             (1 << 28)             /* Bit 28: Software reset flag */
#define RCU_RSTSCK_FWDGTRSTF          (1 << 29)             /* Bit 29: FWDGT reset flag */
#define RCU_RSTSCK_WWDGTRSTF          (1 << 30)             /* Bit 30: WWDGT reset flag */
#define RCU_RSTSCK_LPRSTF             (1 << 31)             /* Bit 31: Low-power reset flag */

/* AHB1 enable bits */

#define RCU_AHB1EN_ENET1EN            (1 << 0)
#define RCU_AHB1EN_USBHS0EN           (1 << 14)
#define RCU_AHB1EN_DMA0EN             (1 << 21)
#define RCU_AHB1EN_DMA1EN             (1 << 22)
#define RCU_AHB1EN_DMAMUXEN           (1 << 23)
#define RCU_AHB1EN_ENET0EN            (1 << 25)
#define RCU_AHB1EN_USBHS1EN           (1 << 29)

/* AHB1 reset bits */

#define RCU_AHB1RST_ENET1RST          (1 << 0)
#define RCU_AHB1RST_USBHS0RST         (1 << 14)
#define RCU_AHB1RST_DMA0RST           (1 << 21)
#define RCU_AHB1RST_DMA1RST           (1 << 22)
#define RCU_AHB1RST_DMAMUXRST         (1 << 23)
#define RCU_AHB1RST_ENET0RST          (1 << 25)
#define RCU_AHB1RST_USBHS1RST         (1 << 29)

/* AHB2 enable bits */

#define RCU_AHB2EN_DCIEN              (1 << 0)
#define RCU_AHB2EN_FACEN              (1 << 1)
#define RCU_AHB2EN_SDIO1EN            (1 << 2)
#define RCU_AHB2EN_CAUEN              (1 << 3)
#define RCU_AHB2EN_HAUEN              (1 << 4)
#define RCU_AHB2EN_TRNGEN             (1 << 6)
#define RCU_AHB2EN_TMUEN              (1 << 7)
#define RCU_AHB2EN_RAMECCMU1EN        (1 << 8)

/* AHB2 reset bits */

#define RCU_AHB2RST_DCIRST            (1 << 0)
#define RCU_AHB2RST_FACRST            (1 << 1)
#define RCU_AHB2RST_SDIO1RST          (1 << 2)
#define RCU_AHB2RST_CAURST            (1 << 3)
#define RCU_AHB2RST_HAURST            (1 << 4)
#define RCU_AHB2RST_TRNGRST           (1 << 6)
#define RCU_AHB2RST_TMURST            (1 << 7)

/* AHB3 enable bits */

#define RCU_AHB3EN_EXMCEN             (1 << 0)
#define RCU_AHB3EN_IPAEN              (1 << 1)
#define RCU_AHB3EN_SDIO0EN            (1 << 2)
#define RCU_AHB3EN_MDMAEN             (1 << 3)
#define RCU_AHB3EN_OSPIMEN            (1 << 4)
#define RCU_AHB3EN_OSPI0EN            (1 << 5)
#define RCU_AHB3EN_OSPI1EN            (1 << 6)
#define RCU_AHB3EN_RTDEC0EN           (1 << 8)
#define RCU_AHB3EN_RTDEC1EN           (1 << 9)
#define RCU_AHB3EN_RAMECCMU0EN        (1 << 10)
#define RCU_AHB3EN_CPUEN              (1 << 15)

/* AHB3 reset bits */

#define RCU_AHB3RST_EXMCRST           (1 << 0)
#define RCU_AHB3RST_IPARST            (1 << 1)
#define RCU_AHB3RST_SDIO0RST          (1 << 2)
#define RCU_AHB3RST_MDMARST           (1 << 3)
#define RCU_AHB3RST_OSPIMRST          (1 << 4)
#define RCU_AHB3RST_OSPI0RST          (1 << 5)
#define RCU_AHB3RST_OSPI1RST          (1 << 6)
#define RCU_AHB3RST_RTDEC0RST         (1 << 8)
#define RCU_AHB3RST_RTDEC1RST         (1 << 9)

/* AHB4 enable bits (GPIO ports A-H, J, K) */

#define RCU_AHB4EN_PAEN               (1 << 0)
#define RCU_AHB4EN_PBEN               (1 << 1)
#define RCU_AHB4EN_PCEN               (1 << 2)
#define RCU_AHB4EN_PDEN               (1 << 3)
#define RCU_AHB4EN_PEEN               (1 << 4)
#define RCU_AHB4EN_PFEN               (1 << 5)
#define RCU_AHB4EN_PGEN               (1 << 6)
#define RCU_AHB4EN_PHEN               (1 << 7)
#define RCU_AHB4EN_PJEN               (1 << 8)
#define RCU_AHB4EN_PKEN               (1 << 9)
#define RCU_AHB4EN_BKPSRAMEN          (1 << 13)
#define RCU_AHB4EN_CRCEN              (1 << 14)
#define RCU_AHB4EN_HWSEMEN            (1 << 15)

/* AHB4 reset bits */

#define RCU_AHB4RST_PARST             (1 << 0)
#define RCU_AHB4RST_PBRST             (1 << 1)
#define RCU_AHB4RST_PCRST             (1 << 2)
#define RCU_AHB4RST_PDRST             (1 << 3)
#define RCU_AHB4RST_PERST             (1 << 4)
#define RCU_AHB4RST_PFRST             (1 << 5)
#define RCU_AHB4RST_PGRST             (1 << 6)
#define RCU_AHB4RST_PHRST             (1 << 7)
#define RCU_AHB4RST_PJRST             (1 << 8)
#define RCU_AHB4RST_PKRST             (1 << 9)
#define RCU_AHB4RST_CRCRST            (1 << 14)
#define RCU_AHB4RST_HWSEMRST          (1 << 15)

/* APB1 enable bits */

#define RCU_APB1EN_TIMER1EN           (1 << 0)
#define RCU_APB1EN_TIMER2EN           (1 << 1)
#define RCU_APB1EN_TIMER3EN           (1 << 2)
#define RCU_APB1EN_TIMER4EN           (1 << 3)
#define RCU_APB1EN_TIMER5EN           (1 << 4)
#define RCU_APB1EN_TIMER6EN           (1 << 5)
#define RCU_APB1EN_TIMER22EN          (1 << 6)
#define RCU_APB1EN_TIMER23EN          (1 << 7)
#define RCU_APB1EN_TIMER30EN          (1 << 8)
#define RCU_APB1EN_TIMER31EN          (1 << 9)
#define RCU_APB1EN_TIMER50EN          (1 << 10)
#define RCU_APB1EN_TIMER51EN          (1 << 11)
#define RCU_APB1EN_RSPDIFEN           (1 << 13)
#define RCU_APB1EN_SPI1EN             (1 << 14)
#define RCU_APB1EN_SPI2EN             (1 << 15)
#define RCU_APB1EN_MDIOEN             (1 << 16)
#define RCU_APB1EN_USART1EN           (1 << 17)
#define RCU_APB1EN_USART2EN           (1 << 18)
#define RCU_APB1EN_UART3EN            (1 << 19)
#define RCU_APB1EN_UART4EN            (1 << 20)
#define RCU_APB1EN_I2C0EN             (1 << 21)
#define RCU_APB1EN_I2C1EN             (1 << 22)
#define RCU_APB1EN_I2C2EN             (1 << 23)
#define RCU_APB1EN_I2C3EN             (1 << 24)
#define RCU_APB1EN_CTCEN              (1 << 27)
#define RCU_APB1EN_DACHOLDEN          (1 << 28)
#define RCU_APB1EN_DACEN              (1 << 29)
#define RCU_APB1EN_UART6EN            (1 << 30)
#define RCU_APB1EN_UART7EN            (1 << 31)

/* APB1 reset bits */

#define RCU_APB1RST_TIMER1RST         (1 << 0)
#define RCU_APB1RST_TIMER2RST         (1 << 1)
#define RCU_APB1RST_TIMER3RST         (1 << 2)
#define RCU_APB1RST_TIMER4RST         (1 << 3)
#define RCU_APB1RST_TIMER5RST         (1 << 4)
#define RCU_APB1RST_TIMER6RST         (1 << 5)
#define RCU_APB1RST_TIMER22RST        (1 << 6)
#define RCU_APB1RST_TIMER23RST        (1 << 7)
#define RCU_APB1RST_TIMER30RST        (1 << 8)
#define RCU_APB1RST_TIMER31RST        (1 << 9)
#define RCU_APB1RST_TIMER50RST        (1 << 10)
#define RCU_APB1RST_TIMER51RST        (1 << 11)
#define RCU_APB1RST_RSPDIFRST         (1 << 13)
#define RCU_APB1RST_SPI1RST           (1 << 14)
#define RCU_APB1RST_SPI2RST           (1 << 15)
#define RCU_APB1RST_MDIORST           (1 << 16)
#define RCU_APB1RST_USART1RST         (1 << 17)
#define RCU_APB1RST_USART2RST         (1 << 18)
#define RCU_APB1RST_UART3RST          (1 << 19)
#define RCU_APB1RST_UART4RST          (1 << 20)
#define RCU_APB1RST_I2C0RST           (1 << 21)
#define RCU_APB1RST_I2C1RST           (1 << 22)
#define RCU_APB1RST_I2C2RST           (1 << 23)
#define RCU_APB1RST_I2C3RST           (1 << 24)
#define RCU_APB1RST_CTCRST            (1 << 27)
#define RCU_APB1RST_DACHOLDRST        (1 << 28)
#define RCU_APB1RST_DACRST            (1 << 29)
#define RCU_APB1RST_UART6RST          (1 << 30)
#define RCU_APB1RST_UART7RST          (1 << 31)

/* APB2 enable bits */

#define RCU_APB2EN_TIMER0EN           (1 << 0)
#define RCU_APB2EN_TIMER7EN           (1 << 1)
#define RCU_APB2EN_USART0EN           (1 << 4)
#define RCU_APB2EN_USART5EN           (1 << 5)
#define RCU_APB2EN_ADC0EN             (1 << 8)
#define RCU_APB2EN_ADC1EN             (1 << 9)
#define RCU_APB2EN_ADC2EN             (1 << 10)
#define RCU_APB2EN_SPI0EN             (1 << 12)
#define RCU_APB2EN_SPI3EN             (1 << 13)
#define RCU_APB2EN_TIMER14EN          (1 << 16)
#define RCU_APB2EN_TIMER15EN          (1 << 17)
#define RCU_APB2EN_TIMER16EN          (1 << 18)
#define RCU_APB2EN_HPDFEN             (1 << 19)
#define RCU_APB2EN_SPI4EN             (1 << 20)
#define RCU_APB2EN_SPI5EN             (1 << 21)
#define RCU_APB2EN_SAI0EN             (1 << 22)
#define RCU_APB2EN_SAI1EN             (1 << 23)
#define RCU_APB2EN_SAI2EN             (1 << 24)
#define RCU_APB2EN_TIMER40EN          (1 << 25)
#define RCU_APB2EN_TIMER41EN          (1 << 26)
#define RCU_APB2EN_TIMER42EN          (1 << 27)
#define RCU_APB2EN_TIMER43EN          (1 << 28)
#define RCU_APB2EN_TIMER44EN          (1 << 29)
#define RCU_APB2EN_EDOUTEN            (1 << 30)
#define RCU_APB2EN_TRGSELEN           (1 << 31)

/* APB2 reset bits */

#define RCU_APB2RST_TIMER0RST         (1 << 0)
#define RCU_APB2RST_TIMER7RST         (1 << 1)
#define RCU_APB2RST_USART0RST         (1 << 4)
#define RCU_APB2RST_USART5RST         (1 << 5)
#define RCU_APB2RST_ADC0RST           (1 << 8)
#define RCU_APB2RST_ADC1RST           (1 << 9)
#define RCU_APB2RST_ADC2RST           (1 << 10)
#define RCU_APB2RST_SPI0RST           (1 << 12)
#define RCU_APB2RST_SPI3RST           (1 << 13)
#define RCU_APB2RST_TIMER14RST        (1 << 16)
#define RCU_APB2RST_TIMER15RST        (1 << 17)
#define RCU_APB2RST_TIMER16RST        (1 << 18)
#define RCU_APB2RST_HPDFRST           (1 << 19)
#define RCU_APB2RST_SPI4RST           (1 << 20)
#define RCU_APB2RST_SPI5RST           (1 << 21)
#define RCU_APB2RST_SAI0RST           (1 << 22)
#define RCU_APB2RST_SAI1RST           (1 << 23)
#define RCU_APB2RST_SAI2RST           (1 << 24)
#define RCU_APB2RST_TIMER40RST        (1 << 25)
#define RCU_APB2RST_TIMER41RST        (1 << 26)
#define RCU_APB2RST_TIMER42RST        (1 << 27)
#define RCU_APB2RST_TIMER43RST        (1 << 28)
#define RCU_APB2RST_TIMER44RST        (1 << 29)
#define RCU_APB2RST_EDOUTRST          (1 << 30)
#define RCU_APB2RST_TRIGSELRST        (1 << 31)

/* APB3 enable / reset bits */

#define RCU_APB3EN_TLIEN              (1 << 0)
#define RCU_APB3EN_WWDGTEN            (1 << 1)
#define RCU_APB3RST_TLIRST            (1 << 0)
#define RCU_APB3RST_WWDGTRST          (1 << 1)

/* APB4 enable / reset bits */

#define RCU_APB4EN_SYSCFGEN           (1 << 0)
#define RCU_APB4EN_CMPEN              (1 << 1)
#define RCU_APB4EN_VREFEN             (1 << 2)
#define RCU_APB4EN_LPDTSEN            (1 << 3)
#define RCU_APB4EN_PMUEN              (1 << 4)
#define RCU_APB4RST_SYSCFGRST         (1 << 0)
#define RCU_APB4RST_CMPRST            (1 << 1)
#define RCU_APB4RST_VREFRST           (1 << 2)
#define RCU_APB4RST_LPDTSRST          (1 << 3)
#define RCU_APB4RST_PMURST            (1 << 4)

/* Additional APB2 enable / reset bits (CAN0-2) */

#define RCU_ADDAPB2EN_CAN0EN          (1 << 0)
#define RCU_ADDAPB2EN_CAN1EN          (1 << 1)
#define RCU_ADDAPB2EN_CAN2EN          (1 << 2)
#define RCU_ADDAPB2RST_CAN0RST        (1 << 0)
#define RCU_ADDAPB2RST_CAN1RST        (1 << 1)
#define RCU_ADDAPB2RST_CAN2RST        (1 << 2)

/* USART clock source selection (RCU_CFG1) */

#define RCU_CFG1_USART0SEL            (0x3 << 0)
#define RCU_CFG1_USART1SEL            (0x3 << 18)
#define RCU_CFG1_USART2SEL            (0x3 << 20)
#define RCU_CFG1_USART5SEL            (0x3 << 22)

#define RCU_CFG1_USARTSEL_APB         (0)
#define RCU_CFG1_USARTSEL_AHB         (1)
#define RCU_CFG1_USARTSEL_LXTAL       (2)
#define RCU_CFG1_USARTSEL_IRC64MDIV   (3)

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_RCU_H */
