/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_usart.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_GD32H7XX_USART_H
#define __ARCH_ARM_SRC_GD32H7_GD32H7XX_USART_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include "chip.h"

#if defined(CONFIG_GD32H7_GD32H73x)
#  include "hardware/gd32h7xx_usart.h"
#else
#  error "Unsupported GD32H7 UART"
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Make sure that we have not enabled more U[S]ARTs than are supported by
 * the device.
 */

#if GD32_NUSART < 8
#  undef CONFIG_GD32H7_UART7
#endif
#if GD32_NUSART < 7
#  undef CONFIG_GD32H7_UART6
#endif
#if GD32_NUSART < 6
#  undef CONFIG_GD32H7_USART5
#endif
#if GD32_NUSART < 5
#  undef CONFIG_GD32H7_UART4
#endif
#if GD32_NUSART < 4
#  undef CONFIG_GD32H7_UART3
#endif
#if GD32_NUSART < 3
#  undef CONFIG_GD32H7_USART2
#endif
#if GD32_NUSART < 2
#  undef CONFIG_GD32H7_USART1
#endif
#if GD32_NUSART < 1
#  undef CONFIG_GD32H7_USART0
#endif

/* Sanity checks */

#if !defined(CONFIG_GD32H7_USART0)
#  undef CONFIG_GD32H7_USART0_SERIALDRIVER
#endif
#if !defined(CONFIG_GD32H7_USART1)
#  undef CONFIG_GD32H7_USART1_SERIALDRIVER
#endif
#if !defined(CONFIG_GD32H7_USART2)
#  undef CONFIG_GD32H7_USART2_SERIALDRIVER
#endif
#if !defined(CONFIG_GD32H7_UART3)
#  undef CONFIG_GD32H7_UART3_SERIALDRIVER
#endif
#if !defined(CONFIG_GD32H7_UART4)
#  undef CONFIG_GD32H7_UART4_SERIALDRIVER
#endif
#if !defined(CONFIG_GD32H7_USART5)
#  undef CONFIG_GD32H7_USART5_SERIALDRIVER
#endif
#if !defined(CONFIG_GD32H7_UART6)
#  undef CONFIG_GD32H7_UART6_SERIALDRIVER
#endif
#if !defined(CONFIG_GD32H7_UART7)
#  undef CONFIG_GD32H7_UART7_SERIALDRIVER
#endif

/* Is the serial driver enabled? */

#if defined(CONFIG_GD32H7_USART0_SERIALDRIVER) || defined(CONFIG_GD32H7_USART1_SERIALDRIVER) || \
    defined(CONFIG_GD32H7_USART2_SERIALDRIVER) || defined(CONFIG_GD32H7_UART3_SERIALDRIVER)  || \
    defined(CONFIG_GD32H7_UART4_SERIALDRIVER)  || defined(CONFIG_GD32H7_USART5_SERIALDRIVER) || \
    defined(CONFIG_GD32H7_UART6_SERIALDRIVER)  || defined(CONFIG_GD32H7_UART7_SERIALDRIVER)
#  define HAVE_SERIALDRIVER 1
#endif

/* Is there a serial console? */

#if defined(CONFIG_USART0_SERIAL_CONSOLE) && defined(CONFIG_GD32H7_USART0_SERIALDRIVER)
#  define CONSOLE_UART 0
#  define HAVE_CONSOLE 1
#elif defined(CONFIG_USART1_SERIAL_CONSOLE) && defined(CONFIG_GD32H7_USART1_SERIALDRIVER)
#  define CONSOLE_UART 1
#  define HAVE_CONSOLE 1
#elif defined(CONFIG_USART2_SERIAL_CONSOLE) && defined(CONFIG_GD32H7_USART2_SERIALDRIVER)
#  define CONSOLE_UART 2
#  define HAVE_CONSOLE 1
#elif defined(CONFIG_UART3_SERIAL_CONSOLE) && defined(CONFIG_GD32H7_UART3_SERIALDRIVER)
#  define CONSOLE_UART 3
#  define HAVE_CONSOLE 1
#elif defined(CONFIG_UART4_SERIAL_CONSOLE) && defined(CONFIG_GD32H7_UART4_SERIALDRIVER)
#  define CONSOLE_UART 4
#  define HAVE_CONSOLE 1
#elif defined(CONFIG_USART5_SERIAL_CONSOLE) && defined(CONFIG_GD32H7_USART5_SERIALDRIVER)
#  define CONSOLE_UART 5
#  define HAVE_CONSOLE 1
#elif defined(CONFIG_UART6_SERIAL_CONSOLE) && defined(CONFIG_GD32H7_UART6_SERIALDRIVER)
#  define CONSOLE_UART 6
#  define HAVE_CONSOLE 1
#elif defined(CONFIG_UART7_SERIAL_CONSOLE) && defined(CONFIG_GD32H7_UART7_SERIALDRIVER)
#  define CONSOLE_UART 7
#  define HAVE_CONSOLE 1
#else
#  undef CONSOLE_UART
#  undef HAVE_CONSOLE
#endif

#endif /* __ARCH_ARM_SRC_GD32H7_GD32H7XX_USART_H */
