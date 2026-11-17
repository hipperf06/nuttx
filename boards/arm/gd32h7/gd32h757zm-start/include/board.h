/****************************************************************************
 * boards/arm/gd32h7/gd32h757zm-start/include/board.h
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

#ifndef __BOARDS_ARM_GD32H7_GD32H757ZM_START_INCLUDE_BOARD_H
#define __BOARDS_ARM_GD32H7_GD32H757ZM_START_INCLUDE_BOARD_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#ifndef __ASSEMBLY__
#  include <stdint.h>
#endif

/* Do not include GD32H7 header files here */

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Clocking *****************************************************************/

/* The GD32H757ZM-START board features a single 25MHz HXTAL crystal.
 *
 * This is the default (600MHz) configuration:
 *
 *   System clock source           : PLL0 (HXTAL)
 *   SYSCLK(Hz)                    : 600000000   Determined by PLL config
 *   AHB(Hz)                       : 300000000   (GD32_AHB_FREQUENCY)
 *   AHB Prescaler                 : 2
 *   APB2(Hz)                      : 300000000   (GD32_PCLK2_FREQUENCY)
 *   APB2 Prescaler                : 1
 *   APB1/3/4(Hz)                  : 150000000   (GD32_PCLK1/3/4_FREQUENCY)
 *   APB1/3/4 Prescaler            : 2
 *   HXTAL value(Hz)               : 25000000    (GD32_HXTAL_FREQUENCY)
 *   PLL0PSC                       : 5
 *   PLL0N                         : 120
 *   PLL0P                         : 1
 *   PLL0Q                         : 4   (150MHz, SPI/SDIO clock tree input)
 *   PLL0R                         : 2   (300MHz)
 *
 * NOTE: The PLL0PSC/PLL0N/PLL0P/PLL0Q/PLL0R definitions are consumed by
 * the RCU driver in arch/arm/src/gd32h7/gd32h7xx_rcu.c.
 */

#define GD32_HXTAL_FREQUENCY       25000000ul
#define GD32_LXTAL_FREQUENCY       32768ul

#define GD32_IRC64M_VALUE          64000000ul
#define GD32_IRC48M_VALUE          48000000ul
#define GD32_IRC32K_VALUE          32000ul
#define GD32_LPIRC4M_VALUE         4000000ul

#if defined(CONFIG_GD32H7_600MHZ)

/* Main PLL0 Configuration.
 *
 * PLL source is HXTAL 25MHz:
 *
 *   PLL0_VCO = (GD32_HXTAL_FREQUENCY / PLL0PSC) * PLL0N
 *            = (25,000,000 / 5) * 120
 *            = 600,000,000
 *   SYSCLK   = PLL0_VCO / PLL0P = 600,000,000 / 1 = 600 MHz
 *   PLL0Q    = PLL0_VCO / 4 = 150 MHz
 *   PLL0R    = PLL0_VCO / 2 = 300 MHz
 */

#  define GD32_PLL0PSC             5
#  define GD32_PLL0N               120
#  define GD32_PLL0P               1
#  define GD32_PLL0Q               4
#  define GD32_PLL0R               2

#  define GD32_SYSCLK_FREQUENCY    600000000ul

#elif defined(CONFIG_GD32H7_300MHZ)

/* Main PLL0 Configuration.
 *
 * PLL source is HXTAL 25MHz:
 *
 *   PLL0_VCO = (25,000,000 / 5) * 60 = 300,000,000
 *   SYSCLK   = PLL0_VCO / PLL0P = 300,000,000 / 1 = 300 MHz
 *   PLL0Q    = PLL0_VCO / 2 = 150 MHz
 *   PLL0R    = PLL0_VCO / 2 = 150 MHz
 */

#  define GD32_PLL0PSC             5
#  define GD32_PLL0N               60
#  define GD32_PLL0P               1
#  define GD32_PLL0Q               2
#  define GD32_PLL0R               2

#  define GD32_SYSCLK_FREQUENCY    300000000ul

#else

/* Fall back to the internal 64MHz RC oscillator (no PLL).  This matches
 * the CONFIG_GD32H7_USE_IRC64M clock source selection.
 */

#  define GD32_SYSCLK_FREQUENCY    64000000ul

#endif

/* AHB clock (HCLK) is SYSCLK / 2 for the 600MHz configuration (300MHz),
 * SYSCLK / 1 for the 300MHz and 64MHz configurations.
 */

#if defined(CONFIG_GD32H7_600MHZ)
#  define GD32_AHB_FREQUENCY       (GD32_SYSCLK_FREQUENCY / 2)
#else
#  define GD32_AHB_FREQUENCY       GD32_SYSCLK_FREQUENCY
#endif

/* APB2 clock (PCLK2) is AHB / 1 in all configurations.
 * NOTE: At CK_SYS > 200MHz the official system clock code uses APB2 =
 * AHB / 1 and APB1/3/4 = AHB / 2.
 */

#if defined(CONFIG_GD32H7_600MHZ)
#  define GD32_PCLK2_FREQUENCY     GD32_AHB_FREQUENCY
#  define GD32_PCLK1_FREQUENCY     (GD32_AHB_FREQUENCY / 2)
#  define GD32_PCLK3_FREQUENCY     (GD32_AHB_FREQUENCY / 2)
#  define GD32_PCLK4_FREQUENCY     (GD32_AHB_FREQUENCY / 2)
#else
#  define GD32_PCLK2_FREQUENCY     GD32_AHB_FREQUENCY
#  define GD32_PCLK1_FREQUENCY     GD32_AHB_FREQUENCY
#  define GD32_PCLK3_FREQUENCY     GD32_AHB_FREQUENCY
#  define GD32_PCLK4_FREQUENCY     GD32_AHB_FREQUENCY
#endif

/* Timers driven from APB1 will be twice PCLK1 (with the default TIMER
 * prescaler selection, RCU_TIMER_PSC_MUL2).
 */

#define GD32_APB1_TIMER1_CLKIN     (2*GD32_PCLK1_FREQUENCY)
#define GD32_APB1_TIMER2_CLKIN     (2*GD32_PCLK1_FREQUENCY)
#define GD32_APB1_TIMER3_CLKIN     (2*GD32_PCLK1_FREQUENCY)
#define GD32_APB1_TIMER4_CLKIN     (2*GD32_PCLK1_FREQUENCY)
#define GD32_APB1_TIMER5_CLKIN     (2*GD32_PCLK1_FREQUENCY)
#define GD32_APB1_TIMER6_CLKIN     (2*GD32_PCLK1_FREQUENCY)

/* Timers driven from APB2 will be twice PCLK2 */

#define GD32_APB2_TIMER0_CLKIN     (2*GD32_PCLK2_FREQUENCY)
#define GD32_APB2_TIMER7_CLKIN     (2*GD32_PCLK2_FREQUENCY)
#define GD32_APB2_TIMER14_CLKIN    (2*GD32_PCLK2_FREQUENCY)
#define GD32_APB2_TIMER15_CLKIN    (2*GD32_PCLK2_FREQUENCY)
#define GD32_APB2_TIMER16_CLKIN    (2*GD32_PCLK2_FREQUENCY)

/* PX4IO co-processor link (FMUv6C wiring: PG9 = TX, PG14 = RX, AF8) */

#ifndef GPIO_USART5_TX
#  define GPIO_USART5_TX \
    (GPIO_CFG_MODE_AF | GPIO_CFG_AF_8 | GPIO_CFG_SPEED_85MHZ | \
     GPIO_CFG_PUPD_PULLUP | GPIO_CFG_PORT_G | GPIO_CFG_PIN_9)
#endif

#ifndef GPIO_USART5_RX
#  define GPIO_USART5_RX \
    (GPIO_CFG_MODE_AF | GPIO_CFG_AF_8 | GPIO_CFG_SPEED_85MHZ | \
     GPIO_CFG_PUPD_PULLUP | GPIO_CFG_PORT_G | GPIO_CFG_PIN_14)
#endif

/* LED definitions **********************************************************/

/* FMUv6C wiring (same board as the Pixhawk 6C):
 *
 *   LED1 red   PD10  (active low, anode to 3.3V)
 *   LED2 blue  PD11  (active low, anode to 3.3V)
 */

#define BOARD_LED1                 0   /* red  */
#define BOARD_LED2                 1   /* blue */

#define BOARD_NLEDS                2

#define BOARD_LED1_BIT             (1 << BOARD_LED1)
#define BOARD_LED2_BIT             (1 << BOARD_LED2)

/* LED index values for use with board_userled_all() */

/* LED definitions for the auto-LEDs interface:
 *
 *   SYMBOL                Meaning                   LED1 (PE5)
 *   --------------------- ------------------------  ----------
 *   LED_STARTED           NuttX has been started    OFF
 *   LED_HEAPALLOCATE      Heap has been allocated   ON
 *   LED_IRQSENABLED       Interrupts enabled        OFF
 *   LED_STACKCREATED      Idle stack created        ON
 *   LED_INIRQ             In an interrupt           No change
 *   LED_SIGNAL            In a signal handler       No change
 *   LED_ASSERTION         An assertion failed       No change
 *   LED_PANIC             The system has crashed    FLASH
 *
 * NOTE: LED_IDLE is intentionally not defined: the up_idle() implementation
 * in the chip layer only references the LED when LED_IDLE is defined.
 */

#define LED_STARTED                0
#define LED_HEAPALLOCATE           1
#define LED_IRQSENABLED            2
#define LED_STACKCREATED           3
#define LED_INIRQ                  4
#define LED_SIGNAL                 5
#define LED_ASSERTION              6
#define LED_PANIC                  7

/* Button definitions *******************************************************/
/* The GD32H757ZM-START board has no general-purpose user button wired to a
 * known GPIO; button support is not enabled in the default configuration.
 */

/* UART pins ****************************************************************/

/* USART0 is wired to the on-board COM port / debugger VCOM:
 *
 *   PA9  USART0_TX AF7
 *   PA10 USART0_RX AF7
 *
 * These definitions override the defaults in the chip pinmap header
 * (hardware/gd32h7xx_pinmap.h guards them with #ifndef).
 */

#define GPIO_USART0_TX \
  (GPIO_CFG_MODE_AF | GPIO_CFG_PUPD_PULLUP | GPIO_CFG_AF_7 | \
   GPIO_CFG_PORT_A | GPIO_CFG_PIN_9)

#define GPIO_USART0_RX \
  (GPIO_CFG_MODE_AF | GPIO_CFG_PUPD_PULLUP | GPIO_CFG_AF_7 | \
   GPIO_CFG_PORT_A | GPIO_CFG_PIN_10)

#endif /* __BOARDS_ARM_GD32H7_GD32H757ZM_START_INCLUDE_BOARD_H */
