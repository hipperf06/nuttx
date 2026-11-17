/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_rcu.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_GD32H7XX_RCU_H
#define __ARCH_ARM_SRC_GD32H7_GD32H7XX_RCU_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include "chip.h"

#if defined(CONFIG_GD32H7_GD32H73x)
#  include "hardware/gd32h7xx_rcu.h"
#else
#  error "Unknown GD32H7 chip"
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Peripheral clock selection encoding: (register offset << 6) | bit position.
 * The register offset is relative to the RCU base address.
 */

#define RCU_PERI_REG_SHIFT        (6)

/* AHB1 peripherals */

#define RCU_AHB1EN_OFFSET         GD32H7_RCU_AHB1EN_OFFSET
#define RCU_AHB2EN_OFFSET         GD32H7_RCU_AHB2EN_OFFSET
#define RCU_AHB3EN_OFFSET         GD32H7_RCU_AHB3EN_OFFSET
#define RCU_AHB4EN_OFFSET         GD32H7_RCU_AHB4EN_OFFSET
#define RCU_APB1EN_OFFSET         GD32H7_RCU_APB1EN_OFFSET
#define RCU_APB2EN_OFFSET         GD32H7_RCU_APB2EN_OFFSET
#define RCU_APB3EN_OFFSET         GD32H7_RCU_APB3EN_OFFSET
#define RCU_APB4EN_OFFSET         GD32H7_RCU_APB4EN_OFFSET
#define RCU_ADDAPB2EN_OFFSET      GD32H7_RCU_ADDAPB2EN_OFFSET

#define RCU_AHB1RST_OFFSET        GD32H7_RCU_AHB1RST_OFFSET
#define RCU_AHB2RST_OFFSET        GD32H7_RCU_AHB2RST_OFFSET
#define RCU_AHB3RST_OFFSET        GD32H7_RCU_AHB3RST_OFFSET
#define RCU_AHB4RST_OFFSET        GD32H7_RCU_AHB4RST_OFFSET
#define RCU_APB1RST_OFFSET        GD32H7_RCU_APB1RST_OFFSET
#define RCU_APB2RST_OFFSET        GD32H7_RCU_APB2RST_OFFSET
#define RCU_APB3RST_OFFSET        GD32H7_RCU_APB3RST_OFFSET
#define RCU_APB4RST_OFFSET        GD32H7_RCU_APB4RST_OFFSET
#define RCU_ADDAPB2RST_OFFSET     GD32H7_RCU_ADDAPB2RST_OFFSET

/* Helper macros to build the peripheral clock / reset selectors */

#define RCU_PERI(regoff, bitpos)  (((regoff) << RCU_PERI_REG_SHIFT) | (bitpos))

/* GPIO port clocks (AHB4) */

#define RCU_GPIOA_EN              RCU_PERI(RCU_AHB4EN_OFFSET, 0)
#define RCU_GPIOB_EN              RCU_PERI(RCU_AHB4EN_OFFSET, 1)
#define RCU_GPIOC_EN              RCU_PERI(RCU_AHB4EN_OFFSET, 2)
#define RCU_GPIOD_EN              RCU_PERI(RCU_AHB4EN_OFFSET, 3)
#define RCU_GPIOE_EN              RCU_PERI(RCU_AHB4EN_OFFSET, 4)
#define RCU_GPIOF_EN              RCU_PERI(RCU_AHB4EN_OFFSET, 5)
#define RCU_GPIOG_EN              RCU_PERI(RCU_AHB4EN_OFFSET, 6)
#define RCU_GPIOH_EN              RCU_PERI(RCU_AHB4EN_OFFSET, 7)
#define RCU_GPIOJ_EN              RCU_PERI(RCU_AHB4EN_OFFSET, 8)
#define RCU_GPIOK_EN              RCU_PERI(RCU_AHB4EN_OFFSET, 9)

/* DMA clocks (AHB1) */

#define RCU_DMA0_EN               RCU_PERI(RCU_AHB1EN_OFFSET, 21)
#define RCU_DMA1_EN               RCU_PERI(RCU_AHB1EN_OFFSET, 22)
#define RCU_DMAMUX_EN             RCU_PERI(RCU_AHB1EN_OFFSET, 23)

/* DMA resets */

#define RCU_DMA0_RST              RCU_PERI(RCU_AHB1RST_OFFSET, 21)
#define RCU_DMA1_RST              RCU_PERI(RCU_AHB1RST_OFFSET, 22)
#define RCU_DMAMUX_RST            RCU_PERI(RCU_AHB1RST_OFFSET, 23)

/* SPI clocks: SPI0/3/4/5 are on APB2, SPI1/2 on APB1 */

#define RCU_SPI0_EN               RCU_PERI(RCU_APB2EN_OFFSET, 12)
#define RCU_SPI1_EN               RCU_PERI(RCU_APB1EN_OFFSET, 14)
#define RCU_SPI2_EN               RCU_PERI(RCU_APB1EN_OFFSET, 15)
#define RCU_SPI3_EN               RCU_PERI(RCU_APB2EN_OFFSET, 13)
#define RCU_SPI4_EN               RCU_PERI(RCU_APB2EN_OFFSET, 20)
#define RCU_SPI5_EN               RCU_PERI(RCU_APB2EN_OFFSET, 21)

/* SPI resets */

#define RCU_SPI0_RST              RCU_PERI(RCU_APB2RST_OFFSET, 12)
#define RCU_SPI1_RST              RCU_PERI(RCU_APB1RST_OFFSET, 14)
#define RCU_SPI2_RST              RCU_PERI(RCU_APB1RST_OFFSET, 15)
#define RCU_SPI3_RST              RCU_PERI(RCU_APB2RST_OFFSET, 13)
#define RCU_SPI4_RST              RCU_PERI(RCU_APB2RST_OFFSET, 20)
#define RCU_SPI5_RST              RCU_PERI(RCU_APB2RST_OFFSET, 21)

/* I2C clocks / resets (all APB1) */

#define RCU_I2C0_EN               RCU_PERI(RCU_APB1EN_OFFSET, 21)
#define RCU_I2C1_EN               RCU_PERI(RCU_APB1EN_OFFSET, 22)
#define RCU_I2C2_EN               RCU_PERI(RCU_APB1EN_OFFSET, 23)
#define RCU_I2C3_EN               RCU_PERI(RCU_APB1EN_OFFSET, 24)

#define RCU_I2C0_RST              RCU_PERI(RCU_APB1RST_OFFSET, 21)
#define RCU_I2C1_RST              RCU_PERI(RCU_APB1RST_OFFSET, 22)
#define RCU_I2C2_RST              RCU_PERI(RCU_APB1RST_OFFSET, 23)
#define RCU_I2C3_RST              RCU_PERI(RCU_APB1RST_OFFSET, 24)

/* Timer clocks: TIMER1-6/22/23/30/31/50/51 are on APB1, the remaining
 * timers (TIMER0/7/14/15/16/40-44) are on APB2.
 */

#define RCU_TIMER1_EN             RCU_PERI(RCU_APB1EN_OFFSET, 0)
#define RCU_TIMER2_EN             RCU_PERI(RCU_APB1EN_OFFSET, 1)
#define RCU_TIMER3_EN             RCU_PERI(RCU_APB1EN_OFFSET, 2)
#define RCU_TIMER4_EN             RCU_PERI(RCU_APB1EN_OFFSET, 3)
#define RCU_TIMER5_EN             RCU_PERI(RCU_APB1EN_OFFSET, 4)
#define RCU_TIMER6_EN             RCU_PERI(RCU_APB1EN_OFFSET, 5)
#define RCU_TIMER22_EN            RCU_PERI(RCU_APB1EN_OFFSET, 6)
#define RCU_TIMER23_EN            RCU_PERI(RCU_APB1EN_OFFSET, 7)
#define RCU_TIMER30_EN            RCU_PERI(RCU_APB1EN_OFFSET, 8)
#define RCU_TIMER31_EN            RCU_PERI(RCU_APB1EN_OFFSET, 9)
#define RCU_TIMER50_EN            RCU_PERI(RCU_APB1EN_OFFSET, 10)
#define RCU_TIMER51_EN            RCU_PERI(RCU_APB1EN_OFFSET, 11)

#define RCU_TIMER1_RST            RCU_PERI(RCU_APB1RST_OFFSET, 0)
#define RCU_TIMER2_RST            RCU_PERI(RCU_APB1RST_OFFSET, 1)
#define RCU_TIMER3_RST            RCU_PERI(RCU_APB1RST_OFFSET, 2)
#define RCU_TIMER4_RST            RCU_PERI(RCU_APB1RST_OFFSET, 3)
#define RCU_TIMER5_RST            RCU_PERI(RCU_APB1RST_OFFSET, 4)
#define RCU_TIMER6_RST            RCU_PERI(RCU_APB1RST_OFFSET, 5)
#define RCU_TIMER22_RST           RCU_PERI(RCU_APB1RST_OFFSET, 6)
#define RCU_TIMER23_RST           RCU_PERI(RCU_APB1RST_OFFSET, 7)
#define RCU_TIMER30_RST           RCU_PERI(RCU_APB1RST_OFFSET, 8)
#define RCU_TIMER31_RST           RCU_PERI(RCU_APB1RST_OFFSET, 9)
#define RCU_TIMER50_RST           RCU_PERI(RCU_APB1RST_OFFSET, 10)
#define RCU_TIMER51_RST           RCU_PERI(RCU_APB1RST_OFFSET, 11)

#define RCU_TIMER0_EN             RCU_PERI(RCU_APB2EN_OFFSET, 0)
#define RCU_TIMER7_EN             RCU_PERI(RCU_APB2EN_OFFSET, 1)
#define RCU_TIMER14_EN            RCU_PERI(RCU_APB2EN_OFFSET, 16)
#define RCU_TIMER15_EN            RCU_PERI(RCU_APB2EN_OFFSET, 17)
#define RCU_TIMER16_EN            RCU_PERI(RCU_APB2EN_OFFSET, 18)
#define RCU_TIMER40_EN            RCU_PERI(RCU_APB2EN_OFFSET, 25)
#define RCU_TIMER41_EN            RCU_PERI(RCU_APB2EN_OFFSET, 26)
#define RCU_TIMER42_EN            RCU_PERI(RCU_APB2EN_OFFSET, 27)
#define RCU_TIMER43_EN            RCU_PERI(RCU_APB2EN_OFFSET, 28)
#define RCU_TIMER44_EN            RCU_PERI(RCU_APB2EN_OFFSET, 29)

#define RCU_TIMER0_RST            RCU_PERI(RCU_APB2RST_OFFSET, 0)
#define RCU_TIMER7_RST            RCU_PERI(RCU_APB2RST_OFFSET, 1)
#define RCU_TIMER14_RST           RCU_PERI(RCU_APB2RST_OFFSET, 16)
#define RCU_TIMER15_RST           RCU_PERI(RCU_APB2RST_OFFSET, 17)
#define RCU_TIMER16_RST           RCU_PERI(RCU_APB2RST_OFFSET, 18)
#define RCU_TIMER40_RST           RCU_PERI(RCU_APB2RST_OFFSET, 25)
#define RCU_TIMER41_RST           RCU_PERI(RCU_APB2RST_OFFSET, 26)
#define RCU_TIMER42_RST           RCU_PERI(RCU_APB2RST_OFFSET, 27)
#define RCU_TIMER43_RST           RCU_PERI(RCU_APB2RST_OFFSET, 28)
#define RCU_TIMER44_RST           RCU_PERI(RCU_APB2RST_OFFSET, 29)

/* ADC (APB2) */

#define RCU_ADC0_EN               RCU_PERI(RCU_APB2EN_OFFSET, 8)
#define RCU_ADC1_EN               RCU_PERI(RCU_APB2EN_OFFSET, 9)
#define RCU_ADC2_EN               RCU_PERI(RCU_APB2EN_OFFSET, 10)

#define RCU_ADC0_RST              RCU_PERI(RCU_APB2RST_OFFSET, 8)
#define RCU_ADC1_RST              RCU_PERI(RCU_APB2RST_OFFSET, 9)
#define RCU_ADC2_RST              RCU_PERI(RCU_APB2RST_OFFSET, 10)

/* DAC (APB1) */

#define RCU_DAC_EN                RCU_PERI(RCU_APB1EN_OFFSET, 29)
#define RCU_DAC_RST               RCU_PERI(RCU_APB1RST_OFFSET, 29)

/* CAN (APB2 additional register block) */

#define RCU_CAN0_EN               RCU_PERI(RCU_ADDAPB2EN_OFFSET, 0)
#define RCU_CAN1_EN               RCU_PERI(RCU_ADDAPB2EN_OFFSET, 1)
#define RCU_CAN2_EN               RCU_PERI(RCU_ADDAPB2EN_OFFSET, 2)

#define RCU_CAN0_RST              RCU_PERI(RCU_ADDAPB2RST_OFFSET, 0)
#define RCU_CAN1_RST              RCU_PERI(RCU_ADDAPB2RST_OFFSET, 1)
#define RCU_CAN2_RST              RCU_PERI(RCU_ADDAPB2RST_OFFSET, 2)

/* AHB2 / AHB3 / AHB4 peripherals */

#define RCU_TRNG_EN               RCU_PERI(RCU_AHB2EN_OFFSET, 6)
#define RCU_TRNG_RST              RCU_PERI(RCU_AHB2RST_OFFSET, 6)

#define RCU_EXMC_EN               RCU_PERI(RCU_AHB3EN_OFFSET, 0)
#define RCU_EXMC_RST              RCU_PERI(RCU_AHB3RST_OFFSET, 0)

#define RCU_SDIO0_EN              RCU_PERI(RCU_AHB3EN_OFFSET, 2)
#define RCU_SDIO0_RST             RCU_PERI(RCU_AHB3RST_OFFSET, 2)

#define RCU_CRC_EN                RCU_PERI(RCU_AHB4EN_OFFSET, 14)
#define RCU_CRC_RST               RCU_PERI(RCU_AHB4RST_OFFSET, 14)

/* APB3 / APB4 peripherals */

#define RCU_WWDGT_EN              RCU_PERI(RCU_APB3EN_OFFSET, 1)

#define RCU_PMU_EN                RCU_PERI(RCU_APB4EN_OFFSET, 4)

/* UART clocks: USART0 and USART5 are on APB2, all others on APB1 */

#define RCU_USART0_EN             RCU_PERI(RCU_APB2EN_OFFSET, 4)
#define RCU_USART1_EN             RCU_PERI(RCU_APB1EN_OFFSET, 17)
#define RCU_USART2_EN             RCU_PERI(RCU_APB1EN_OFFSET, 18)
#define RCU_UART3_EN              RCU_PERI(RCU_APB1EN_OFFSET, 19)
#define RCU_UART4_EN              RCU_PERI(RCU_APB1EN_OFFSET, 20)
#define RCU_USART5_EN             RCU_PERI(RCU_APB2EN_OFFSET, 5)
#define RCU_UART6_EN              RCU_PERI(RCU_APB1EN_OFFSET, 30)
#define RCU_UART7_EN              RCU_PERI(RCU_APB1EN_OFFSET, 31)

/* USART resets */

#define RCU_USART0_RST            RCU_PERI(RCU_APB2RST_OFFSET, 4)
#define RCU_USART1_RST            RCU_PERI(RCU_APB1RST_OFFSET, 17)
#define RCU_USART2_RST            RCU_PERI(RCU_APB1RST_OFFSET, 18)
#define RCU_UART3_RST             RCU_PERI(RCU_APB1RST_OFFSET, 19)
#define RCU_UART4_RST             RCU_PERI(RCU_APB1RST_OFFSET, 20)
#define RCU_USART5_RST            RCU_PERI(RCU_APB2RST_OFFSET, 5)
#define RCU_UART6_RST             RCU_PERI(RCU_APB1RST_OFFSET, 30)
#define RCU_UART7_RST             RCU_PERI(RCU_APB1RST_OFFSET, 31)

/* SYSCFG clock (APB4) */

#define RCU_SYSCFG_EN             RCU_PERI(RCU_APB4EN_OFFSET, 0)
#define RCU_SYSCFG_RST            RCU_PERI(RCU_APB4RST_OFFSET, 0)

/* PMU clock (APB4) */

#define RCU_PMU_EN                RCU_PERI(RCU_APB4EN_OFFSET, 4)

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifndef __ASSEMBLY__

#if defined(__cplusplus)
extern "C"
{
#endif

/****************************************************************************
 * Name: gd32_clockconfig
 *
 * Description:
 *   Called to initialize the GD32H7.  This does whatever setup is needed to
 *   put the MCU in a usable state.  This includes the initialization of
 *   clocking using the settings in board.h.
 *
 ****************************************************************************/

void gd32_clockconfig(void);

/****************************************************************************
 * Name: gd32_board_clockconfig
 *
 * Description:
 *   Any GD32 board may replace the "standard" board clock configuration
 *   logic with its own, custom clock configuration logic.
 *
 ****************************************************************************/

#ifdef CONFIG_ARCH_BOARD_GD32H7_CUSTOM_CLOCKCONFIG
void gd32_board_clockconfig(void);
#endif

/****************************************************************************
 * Name: gd32_rcu_periph_clock_enable
 *
 * Description:
 *   Enable the peripheral clock. The peripheral selector is the encoded
 *   (register offset, bit position) value from the RCU_PERI() macro.
 *
 ****************************************************************************/

void gd32_rcu_periph_clock_enable(uint32_t periph);

/****************************************************************************
 * Name: gd32_rcu_periph_clock_disable
 *
 * Description:
 *   Disable the peripheral clock.
 *
 ****************************************************************************/

void gd32_rcu_periph_clock_disable(uint32_t periph);

/****************************************************************************
 * Name: gd32_rcu_periph_reset_enable / disable / reset
 *
 * Description:
 *   Assert, release, or pulse the reset of one peripheral.  The selector
 *   uses the RCU_PERI(offset, bit) encoding with one of the RCU_xxxRST
 *   offsets (for example RCU_TIMER1_RST).
 *
 ****************************************************************************/

void gd32_rcu_periph_reset_enable(uint32_t periph);
void gd32_rcu_periph_reset_disable(uint32_t periph);
void gd32_rcu_periph_reset(uint32_t periph);

/****************************************************************************
 * Name: gd32_rcu_clock_freq_get
 *
 * Description:
 *   Return the frequency in Hz of the requested clock domain.
 *
 ****************************************************************************/

enum gd32_rcu_clock_e
{
  GD32_RCU_CKSYS = 0,   /* System clock */
  GD32_RCU_CKAHB,       /* AHB bus clock */
  GD32_RCU_CKAPB1,      /* APB1 clock */
  GD32_RCU_CKAPB2,      /* APB2 clock */
  GD32_RCU_CKAPB3,      /* APB3 clock */
  GD32_RCU_CKAPB4,      /* APB4 clock */
};

uint32_t gd32_rcu_clock_freq_get(enum gd32_rcu_clock_e domain);

/****************************************************************************
 * Name: gd32_rcu_lxtal_config
 *
 * Description:
 *   Enable the 32.768kHz LXTAL oscillator and wait for it to stabilize.
 *
 * Returned Value:
 *   Zero (OK) on success; -ETIMEDOUT if the oscillator does not start.
 *
 ****************************************************************************/

int  gd32_rcu_lxtal_config(void);

/****************************************************************************
 * Name: gd32_rcu_rtc_clock_config
 *
 * Description:
 *   Select LXTAL as the RTC clock source and enable the RTC clock.
 *
 ****************************************************************************/

void gd32_rcu_rtc_clock_config(void);

/****************************************************************************
 * Name: gd32_rcu_clock_monitor_enable
 *
 * Description:
 *   Enable/disable the HXTAL clock monitor (CKM).
 *
 ****************************************************************************/

void gd32_rcu_clock_monitor_enable(bool enable);

#if defined(__cplusplus)
}
#endif
#endif /* __ASSEMBLY__ */
#endif /* __ARCH_ARM_SRC_GD32H7_GD32H7XX_RCU_H */
