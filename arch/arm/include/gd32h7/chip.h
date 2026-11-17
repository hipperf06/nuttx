/****************************************************************************
 * arch/arm/include/gd32h7/chip.h
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

#ifndef __ARCH_ARM_INCLUDE_GD32H7_CHIP_H
#define __ARCH_ARM_INCLUDE_GD32H7_CHIP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

/* Check the GD32H7 family configuration.
 * It must be done in arch/arm/src/gd32h7/Kconfig !
 */

#if defined(CONFIG_GD32H7_GD32H73x)

/* GD32H737 / GD32H757: 600MHz Cortex-M7, double precision FPU */

#  define GD32_NGPIO_PORTS              10  /* GPIOA-H, GPIOJ, GPIOK */
#  define GD32_NCRC                     1   /* CRC calculation unit */
#  define GD32_NTRNG                    1   /* True random number generator */
#  define GD32_NDMA                     2   /* DMA0, DMA1 (plus MDMA) */
#  define GD32_NMDMA                    1   /* Master DMA */
#  define GD32_NADC                     3   /* 12-bit ADC0-2 */
#  define GD32_NDAC                     2   /* 12-bit DAC0,1 */
#  define GD32_NATIMER                  2   /* Advanced timers TIMER0, TIMER7 */
#  define GD32_NGTIMER                  4   /* General timers TIMER1-4 */
#  define GD32_NBTIMER                  2   /* Basic timers TIMER5, TIMER6 */
#  define GD32_NUSART                   8   /* USART0-2,5, UART3,4,6,7 */
#  define GD32_NI2C                     4   /* I2C0-3 */
#  define GD32_NSPI                     6   /* SPI0-5 */
#  define GD32_NDCI                     1   /* Digital camera interface */
#  define GD32_NTLI                     1   /* TFT-LCD interface (TLI) */
#  define GD32_NSDIO                    2   /* SDIO0, SDIO1 */
#  define GD32_NEXMC                    1   /* External memory controller */
#  define GD32_NOSPI                    2   /* Octo-SPI 0,1 */
#  define GD32_NCAN                     3   /* CAN0-2 */
#  define GD32_NETHERNET                2   /* ENET0, ENET1 */
#  define GD32_NUSBHS                   2   /* USBHS0, USBHS1 */
#  define GD32_NSAI                     3   /* SAI0-2 */

#else
#  error "Unknown GD32H7 chip type"
#endif

/* Get customizations for each supported chip and provide alternate function
 * pin-mapping.
 *
 * NOTE: Each GPIO pin may serve either for general purpose I/O or for a
 * specific alternate function.  Each pin may be assigned to one of up to
 * 16 alternate functions (AF0-AF15).
 */

/* Alternate function pin descriptions ***************************************************/
/* TODO: Provide the alternate function map for the selected GD32H7 part when the
 * pin multiplexing table is implemented in hardware/gd32h7xx_pinmap.h.
 */

/* NVIC priority levels *****************************************************/

/* The Cortex-M7 in the GD32H7xx implements four bits of interrupt priority. */

#define NVIC_SYSH_PRIORITY_MIN     0xf0 /* All bits set in minimum priority */
#define NVIC_SYSH_PRIORITY_DEFAULT 0x80 /* Midpoint is the default */
#define NVIC_SYSH_PRIORITY_MAX     0x00 /* Zero is maximum priority */
#define NVIC_SYSH_PRIORITY_STEP    0x10 /* Four bits of interrupt priority used */

#endif /* __ARCH_ARM_INCLUDE_GD32H7_CHIP_H */
