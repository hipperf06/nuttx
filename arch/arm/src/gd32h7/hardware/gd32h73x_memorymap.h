/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h73x_memorymap.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H73X_MEMORYMAP_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H73X_MEMORYMAP_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* System memory map (GD32H737 / GD32H757) --------------------------------
 *
 * 0x00000000 - 0x0000ffff  ITCM RAM (64KiB)
 * 0x08000000 - 0x080fffff  Main FLASH (GD32H737: 1MiB, GD32H757: 2MiB)
 * 0x20000000 - 0x2000ffff  DTCM RAM (64KiB)
 * 0x24000000 - 0x2407ffff  AXI SRAM (512KiB)
 * 0x30000000 - 0x3007ffff  SRAM0-3 (aggregate, see note below)
 * 0x40000000 - 0x4001ffff  APB1 / APB2 peripherals
 * 0x40020000 - 0x4003ffff  AHB1 peripherals
 * 0x48000000 - 0x4802ffff  AHB2 peripherals
 * 0x50000000               APB3 peripherals
 * 0x51000000 - 0x5200bfff  AHB3 peripherals (FMC, EXMC, OSPI ...)
 * 0x58000000 - 0x5801ffff  APB4 peripherals
 * 0x58020000 - 0x5802ffff  AHB4 peripherals (GPIO, RCU ...)
 * 0xe0000000 - 0xe00fffff  Cortex-M7 core peripherals
 *
 * NOTE: The split of the SRAM0-3 aggregate block follows the reference
 * manual.  Verify the per-variant sizes against the datasheet of the
 * selected part before using these regions as heap.
 */

/* Tightly coupled memory */

#define GD32H7_ITCM_BASE              0x00000000      /* 64KiB ITCM RAM */
#define GD32H7_ITCM_SIZE              0x00010000

#define GD32H7_DTCM_BASE              0x20000000      /* 64KiB DTCM RAM */
#define GD32H7_DTCM_SIZE              0x00010000

/* Main FLASH */

#define GD32H7_FLASH_BASE             0x08000000      /* Main FLASH base address */
#define GD32H7_FLASH_PADDR_BASE       0x08000000      /* Physical FLASH base address */

#ifdef CONFIG_GD32H7_FLASH_1M
#  define GD32H7_FLASH_SIZE           0x00100000      /* 1MiB FLASH */
#else
#  define GD32H7_FLASH_SIZE           0x00200000      /* 2MiB FLASH */
#endif

#define GD32H7_FLASH_END              (GD32H7_FLASH_BASE + GD32H7_FLASH_SIZE)

/* System SRAM (AXI SRAM) */

#define GD32H7_AXISRAM_BASE           0x24000000      /* 512KiB AXI SRAM */
#define GD32H7_AXISRAM_SIZE           0x00080000

/* SRAM0-3 */

#define GD32H7_SRAM0_BASE             0x30000000      /* 192KiB SRAM0 */
#define GD32H7_SRAM0_SIZE             0x00030000
#define GD32H7_SRAM1_BASE             0x30030000      /* 64KiB SRAM1 */
#define GD32H7_SRAM1_SIZE             0x00010000
#define GD32H7_SRAM2_BASE             0x30040000      /* 64KiB SRAM2 */
#define GD32H7_SRAM2_SIZE             0x00010000
#define GD32H7_SRAM3_BASE             0x30050000      /* 64KiB SRAM3 */
#define GD32H7_SRAM3_SIZE             0x00010000

/* Backup SRAM */

#define GD32H7_BKPSRAM_BASE           0x38800000      /* 4KiB backup SRAM */
#define GD32H7_BKPSRAM_SIZE           0x00001000

/* APB1 peripherals (0x40000000 - 0x4000ffff) */

#define GD32H7_TIMER1_BASE            0x40000000      /* TIMER1 */
#define GD32H7_TIMER2_BASE            0x40000400      /* TIMER2 */
#define GD32H7_TIMER3_BASE            0x40000800      /* TIMER3 */
#define GD32H7_TIMER4_BASE            0x40000c00      /* TIMER4 */
#define GD32H7_TIMER5_BASE            0x40001000      /* TIMER5 */
#define GD32H7_TIMER6_BASE            0x40001400      /* TIMER6 */
#define GD32H7_TIMER22_BASE           0x4000e000      /* TIMER22 */
#define GD32H7_TIMER23_BASE           0x4000e400      /* TIMER23 */
#define GD32H7_TIMER30_BASE           0x4000e800      /* TIMER30 */
#define GD32H7_TIMER31_BASE           0x4000ec00      /* TIMER31 */
#define GD32H7_TIMER50_BASE           0x4000f000      /* TIMER50 */
#define GD32H7_TIMER51_BASE           0x4000f400      /* TIMER51 */
#define GD32H7_SPI1_BASE              0x40003800      /* SPI1 */
#define GD32H7_SPI2_BASE              0x40003c00      /* SPI2 */
#define GD32H7_RSPDIF_BASE            0x40004000      /* RSPDIF */
#define GD32H7_USART1_BASE            0x40004400      /* USART1 */
#define GD32H7_USART2_BASE            0x40004800      /* USART2 */
#define GD32H7_UART3_BASE             0x40004c00      /* UART3 */
#define GD32H7_UART4_BASE             0x40005000      /* UART4 */
#define GD32H7_I2C0_BASE              0x40005400      /* I2C0 */
#define GD32H7_I2C1_BASE              0x40005800      /* I2C1 (per UM) */
#define GD32H7_I2C2_BASE              0x40005c00      /* I2C2 (per UM) */
#define GD32H7_I2C3_BASE              0x40006000      /* I2C3 (per UM) */
#define GD32H7_DAC0_BASE              0x40007400      /* DAC0 */
#define GD32H7_UART6_BASE             0x40007800      /* UART6 */
#define GD32H7_UART7_BASE             0x40007c00      /* UART7 */
#define GD32H7_CTC_BASE               0x40008400      /* CTC */
#define GD32H7_MDIO_BASE              0x40009400      /* MDIO */

/* APB2 peripherals (0x40010000 - 0x4001ffff) */

#define GD32H7_TIMER0_BASE            0x40010000      /* TIMER0 */
#define GD32H7_TIMER7_BASE            0x40010400      /* TIMER7 */
#define GD32H7_TIMER14_BASE           0x40014000      /* TIMER14 */
#define GD32H7_TIMER15_BASE           0x40014400      /* TIMER15 */
#define GD32H7_TIMER16_BASE           0x40014800      /* TIMER16 */
#define GD32H7_TIMER40_BASE           0x4001d000      /* TIMER40 */
#define GD32H7_TIMER41_BASE           0x4001d400      /* TIMER41 */
#define GD32H7_TIMER42_BASE           0x4001d800      /* TIMER42 */
#define GD32H7_TIMER43_BASE           0x4001dc00      /* TIMER43 */
#define GD32H7_TIMER44_BASE           0x4001f000      /* TIMER44 */

#define GD32H7_ADC0_BASE              0x40012400      /* ADC0 */
#define GD32H7_ADC1_BASE              0x40012500      /* ADC1 */
#define GD32H7_ADC2_BASE              0x40012600      /* ADC2 */
#define GD32H7_USART0_BASE            0x40011000      /* USART0 */
#define GD32H7_USART5_BASE            0x40011400      /* USART5 */
#define GD32H7_SPI0_BASE              0x40013000      /* SPI0 */
#define GD32H7_SPI1_BASE              0x40003800      /* SPI1 */
#define GD32H7_SPI2_BASE              0x40003c00      /* SPI2 */
#define GD32H7_SPI3_BASE              0x40013400      /* SPI3 */
#define GD32H7_SPI4_BASE              0x40015000      /* SPI4 */
#define GD32H7_SPI5_BASE              0x40015400      /* SPI5 */
#define GD32H7_SAI0_BASE              0x40015800      /* SAI0 */
#define GD32H7_CAN0_BASE              0x4001a000      /* CAN0 (per UM) */
#define GD32H7_CAN1_BASE              0x4001a400      /* CAN1 (per UM) */
#define GD32H7_CAN2_BASE              0x4001a800      /* CAN2 (per UM) */

/* APB3 peripherals (0x50000000 - 0x5000ffff) */

#define GD32H7_TLI_BASE               0x50001000      /* TLI */
#define GD32H7_WWDGT_BASE             0x50003000      /* WWDGT */

/* AHB1 peripherals (0x40020000 - 0x4003ffff) */

#define GD32H7_DMA0_BASE              0x40020000      /* DMA0 */
#define GD32H7_DMA1_BASE              0x40020400      /* DMA1 */
#define GD32H7_DMAMUX_BASE            0x40020800      /* DMAMUX */
#define GD32H7_EFUSE_BASE             0x40022800      /* EFUSE */
#define GD32H7_ENET0_BASE             0x40028000      /* ENET0 */
#define GD32H7_USBHS0_BASE            0x40040000      /* USBHS0 */

/* AHB2 peripherals (0x48000000 - 0x4802ffff) */

#define GD32H7_DCI_BASE               0x48020000      /* DCI */
#define GD32H7_CAU_BASE               0x48021000      /* CAU */
#define GD32H7_HAU_BASE               0x48021400      /* HAU */
#define GD32H7_TRNG_BASE              0x48021800      /* TRNG */
#define GD32H7_SDIO0_BASE             0x48022400      /* SDIO0 */
#define GD32H7_TMU_BASE               0x48024400      /* TMU */
#define GD32H7_FAC_BASE               0x48024800      /* FAC */

/* AHB3 peripherals (0x51000000 - 0x5200bfff) */

#define GD32H7_AXIM_BASE              0x51000000      /* AXIM */
#define GD32H7_MDMA_BASE              0x52000000      /* MDMA */
#define GD32H7_IPA_BASE               0x52001000      /* IPA */
#define GD32H7_FMC_BASE               0x52002000      /* Flash memory controller */
#define GD32H7_FLEXRAMC_BASE          0x52003000      /* FLEXRAMC */
#define GD32H7_EXMC_BASE              0x52004000      /* EXMC */
#define GD32H7_OSPI0_BASE             0x52005000      /* OSPI0 */
#define GD32H7_OSPM_BASE              0x5200b400      /* OSPM */
#define GD32H7_RTDEC_BASE             0x5200b800      /* RTDEC */

/* APB4 peripherals (0x58000000 - 0x5801ffff) */

#define GD32H7_EXTI_BASE              0x58000000      /* EXTI */
#define GD32H7_SYSCFG_BASE            0x58000400      /* SYSCFG */
#define GD32H7_CMP_BASE               0x58003800      /* CMP */
#define GD32H7_VREF_BASE              0x58003c00      /* VREF */
#define GD32H7_RTC_BASE               0x58004000      /* RTC */
#define GD32H7_FWDGT_BASE             0x58004800      /* FWDGT */
#define GD32H7_PMU_BASE               0x58005800      /* PMU */
#define GD32H7_LPDTS_BASE             0x58006800      /* LPDTS */

/* AHB4 peripherals (0x58020000 - 0x5802ffff) */

#define GD32H7_GPIOA_BASE             0x58020000      /* GPIOA */
#define GD32H7_GPIOB_BASE             0x58020400      /* GPIOB */
#define GD32H7_GPIOC_BASE             0x58020800      /* GPIOC */
#define GD32H7_GPIOD_BASE             0x58020c00      /* GPIOD */
#define GD32H7_GPIOE_BASE             0x58021000      /* GPIOE */
#define GD32H7_GPIOF_BASE             0x58021400      /* GPIOF */
#define GD32H7_GPIOG_BASE             0x58021800      /* GPIOG */
#define GD32H7_GPIOH_BASE             0x58021c00      /* GPIOH */
#define GD32H7_GPIOJ_BASE             0x58022400      /* GPIOJ */
#define GD32H7_GPIOK_BASE             0x58022800      /* GPIOK */
#define GD32H7_RCU_BASE               0x58024400      /* RCU */
#define GD32H7_CRC_BASE               0x58024c00      /* CRC */
#define GD32H7_HWSEM_BASE             0x58026400      /* HWSEM */

/* Option byte and debug memory map */

#define GD32H7_OB_BASE                0x1ffff800      /* Option bytes */
#define GD32H7_DBG_BASE               0xe00e1000      /* DBG */

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H73X_MEMORYMAP_H */
