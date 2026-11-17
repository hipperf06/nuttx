/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h7xx_pinmap.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_PINMAP_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_PINMAP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Default alternate function selections for the USART peripherals.
 *
 * These definitions are provided so that a minimal console can be brought
 * up without board specific pin definitions.  The actual pin assignment is
 * board specific: a board port should provide its own definitions of the
 * GPIO_USARTn_TX / GPIO_USARTn_RX macros in its board.h, which will take
 * precedence over the defaults below.
 *
 * The default mapping follows the typical eval-board wiring:
 *   USART0: PA9 (TX, AF7) / PA10 (RX, AF7)
 *   USART1: PA2 (TX, AF7) / PA3 (RX, AF7)
 *   USART2: PB10 (TX, AF7) / PB11 (RX, AF7)
 *   UART3:  PC10 (TX, AF7) / PC11 (RX, AF7)
 *   UART4:  PC12 (TX, AF7) / PD2 (RX, AF7)
 *   USART5: PC6 (TX, AF7) / PC7 (RX, AF7)
 */

#ifndef GPIO_USART0_TX
#  define GPIO_USART0_TX  (GPIO_CFG_MODE_AF | GPIO_CFG_PUPD_PULLUP | \
                           GPIO_CFG_AF_7 | GPIO_CFG_PORT_A | GPIO_CFG_PIN_9)
#endif
#ifndef GPIO_USART0_RX
#  define GPIO_USART0_RX  (GPIO_CFG_MODE_AF | GPIO_CFG_PUPD_PULLUP | \
                           GPIO_CFG_AF_7 | GPIO_CFG_PORT_A | GPIO_CFG_PIN_10)
#endif

#ifndef GPIO_USART1_TX
#  define GPIO_USART1_TX  (GPIO_CFG_MODE_AF | GPIO_CFG_PUPD_PULLUP | \
                           GPIO_CFG_AF_7 | GPIO_CFG_PORT_A | GPIO_CFG_PIN_2)
#endif
#ifndef GPIO_USART1_RX
#  define GPIO_USART1_RX  (GPIO_CFG_MODE_AF | GPIO_CFG_PUPD_PULLUP | \
                           GPIO_CFG_AF_7 | GPIO_CFG_PORT_A | GPIO_CFG_PIN_3)
#endif

#ifndef GPIO_USART2_TX
#  define GPIO_USART2_TX  (GPIO_CFG_MODE_AF | GPIO_CFG_PUPD_PULLUP | \
                           GPIO_CFG_AF_7 | GPIO_CFG_PORT_B | GPIO_CFG_PIN_10)
#endif
#ifndef GPIO_USART2_RX
#  define GPIO_USART2_RX  (GPIO_CFG_MODE_AF | GPIO_CFG_PUPD_PULLUP | \
                           GPIO_CFG_AF_7 | GPIO_CFG_PORT_B | GPIO_CFG_PIN_11)
#endif

#ifndef GPIO_UART3_TX
#  define GPIO_UART3_TX   (GPIO_CFG_MODE_AF | GPIO_CFG_PUPD_PULLUP | \
                           GPIO_CFG_AF_7 | GPIO_CFG_PORT_C | GPIO_CFG_PIN_10)
#endif
#ifndef GPIO_UART3_RX
#  define GPIO_UART3_RX   (GPIO_CFG_MODE_AF | GPIO_CFG_PUPD_PULLUP | \
                           GPIO_CFG_AF_7 | GPIO_CFG_PORT_C | GPIO_CFG_PIN_11)
#endif

#ifndef GPIO_UART4_TX
#  define GPIO_UART4_TX   (GPIO_CFG_MODE_AF | GPIO_CFG_PUPD_PULLUP | \
                           GPIO_CFG_AF_7 | GPIO_CFG_PORT_C | GPIO_CFG_PIN_12)
#endif
#ifndef GPIO_UART4_RX
#  define GPIO_UART4_RX   (GPIO_CFG_MODE_AF | GPIO_CFG_PUPD_PULLUP | \
                           GPIO_CFG_AF_7 | GPIO_CFG_PORT_D | GPIO_CFG_PIN_2)
#endif

#ifndef GPIO_USART5_TX
#  define GPIO_USART5_TX  (GPIO_CFG_MODE_AF | GPIO_CFG_PUPD_PULLUP | \
                           GPIO_CFG_AF_7 | GPIO_CFG_PORT_C | GPIO_CFG_PIN_6)
#endif
#ifndef GPIO_USART5_RX
#  define GPIO_USART5_RX  (GPIO_CFG_MODE_AF | GPIO_CFG_PUPD_PULLUP | \
                           GPIO_CFG_AF_7 | GPIO_CFG_PORT_C | GPIO_CFG_PIN_7)
#endif

/* UART6 / UART7 are usually multiplexed with ENET or TLI on typical
 * packages; no default pins are provided and a board port must define
 * GPIO_UART6_TX/RX and GPIO_UART7_TX/RX to use them.
 */

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_PINMAP_H */
