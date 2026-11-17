/****************************************************************************
 * arch/arm/include/gd32h7/gd32h7xx_irq.h
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

/* This file should never be included directly but, rather,
 * only indirectly through nuttx/irq.h
 */

#ifndef __ARCH_ARM_INCLUDE_GD32H7_GD32H7XX_IRQ_H
#define __ARCH_ARM_INCLUDE_GD32H7_GD32H7XX_IRQ_H

/****************************************************************************
 * Pre-processor Prototypes
 ****************************************************************************/

/* External interrupts (vectors >= 16).
 *
 * The IRQ numbers follow the GD32H7xx User Manual interrupt table
 * (equivalent to IRQn_Type in the official GD32H7xx firmware library V1.4.0).
 */

#define GD32_IRQ_WWDGT              (GD32_IRQ_EXINT + 0)   /* Vector  16: Window watchdog timer */
#define GD32_IRQ_VAVD_LVD_VOVD      (GD32_IRQ_EXINT + 1)   /* Vector  17: AVD/LVD/OVD through EXTI line */
#define GD32_IRQ_TAMPER_STAMP_LXTAL (GD32_IRQ_EXINT + 2)   /* Vector  18: RTC tamper, timestamp, LXTAL stuck */
#define GD32_IRQ_RTC_WKUP           (GD32_IRQ_EXINT + 3)   /* Vector  19: RTC wakeup */
#define GD32_IRQ_FMC                (GD32_IRQ_EXINT + 4)   /* Vector  20: Flash memory controller */
#define GD32_IRQ_RCU                (GD32_IRQ_EXINT + 5)   /* Vector  21: Reset and clock unit */
#define GD32_IRQ_EXTI0              (GD32_IRQ_EXINT + 6)   /* Vector  22: EXTI line 0 */
#define GD32_IRQ_EXTI1              (GD32_IRQ_EXINT + 7)   /* Vector  23: EXTI line 1 */
#define GD32_IRQ_EXTI2              (GD32_IRQ_EXINT + 8)   /* Vector  24: EXTI line 2 */
#define GD32_IRQ_EXTI3              (GD32_IRQ_EXINT + 9)   /* Vector  25: EXTI line 3 */
#define GD32_IRQ_EXTI4              (GD32_IRQ_EXINT + 10)  /* Vector  26: EXTI line 4 */
#define GD32_IRQ_DMA0_CHANNEL0      (GD32_IRQ_EXINT + 11)  /* Vector  27: DMA0 channel 0 */
#define GD32_IRQ_DMA0_CHANNEL1      (GD32_IRQ_EXINT + 12)  /* Vector  28: DMA0 channel 1 */
#define GD32_IRQ_DMA0_CHANNEL2      (GD32_IRQ_EXINT + 13)  /* Vector  29: DMA0 channel 2 */
#define GD32_IRQ_DMA0_CHANNEL3      (GD32_IRQ_EXINT + 14)  /* Vector  30: DMA0 channel 3 */
#define GD32_IRQ_DMA0_CHANNEL4      (GD32_IRQ_EXINT + 15)  /* Vector  31: DMA0 channel 4 */
#define GD32_IRQ_DMA0_CHANNEL5      (GD32_IRQ_EXINT + 16)  /* Vector  32: DMA0 channel 5 */
#define GD32_IRQ_DMA0_CHANNEL6      (GD32_IRQ_EXINT + 17)  /* Vector  33: DMA0 channel 6 */
#define GD32_IRQ_ADC0_1             (GD32_IRQ_EXINT + 18)  /* Vector  34: ADC0 and ADC1 */
#define GD32_IRQ_EXTI5_9            (GD32_IRQ_EXINT + 23)  /* Vector  39: EXTI line 5 to 9 */
#define GD32_IRQ_TIMER0_BRK         (GD32_IRQ_EXINT + 24)  /* Vector  40: TIMER0 break */
#define GD32_IRQ_TIMER0_UP          (GD32_IRQ_EXINT + 25)  /* Vector  41: TIMER0 update */
#define GD32_IRQ_TIMER0_TRG_CMT     (GD32_IRQ_EXINT + 26)  /* Vector  42: TIMER0 trigger and commutation */
#define GD32_IRQ_TIMER0_CHANNEL     (GD32_IRQ_EXINT + 27)  /* Vector  43: TIMER0 capture compare */
#define GD32_IRQ_TIMER1             (GD32_IRQ_EXINT + 28)  /* Vector  44: TIMER1 */
#define GD32_IRQ_TIMER2             (GD32_IRQ_EXINT + 29)  /* Vector  45: TIMER2 */
#define GD32_IRQ_TIMER3             (GD32_IRQ_EXINT + 30)  /* Vector  46: TIMER3 */
#define GD32_IRQ_I2C0_EV            (GD32_IRQ_EXINT + 31)  /* Vector  47: I2C0 event */
#define GD32_IRQ_I2C0_ER            (GD32_IRQ_EXINT + 32)  /* Vector  48: I2C0 error */
#define GD32_IRQ_I2C1_EV            (GD32_IRQ_EXINT + 33)  /* Vector  49: I2C1 event */
#define GD32_IRQ_I2C1_ER            (GD32_IRQ_EXINT + 34)  /* Vector  50: I2C1 error */
#define GD32_IRQ_SPI0               (GD32_IRQ_EXINT + 35)  /* Vector  51: SPI0 */
#define GD32_IRQ_SPI1               (GD32_IRQ_EXINT + 36)  /* Vector  52: SPI1 */
#define GD32_IRQ_USART0             (GD32_IRQ_EXINT + 37)  /* Vector  53: USART0 global and wakeup */
#define GD32_IRQ_USART1             (GD32_IRQ_EXINT + 38)  /* Vector  54: USART1 global and wakeup */
#define GD32_IRQ_USART2             (GD32_IRQ_EXINT + 39)  /* Vector  55: USART2 global and wakeup */
#define GD32_IRQ_EXTI10_15          (GD32_IRQ_EXINT + 40)  /* Vector  56: EXTI line 10 to 15 */
#define GD32_IRQ_RTC_ALARM          (GD32_IRQ_EXINT + 41)  /* Vector  57: RTC alarm */
#define GD32_IRQ_TIMER7_BRK         (GD32_IRQ_EXINT + 43)  /* Vector  59: TIMER7 break */
#define GD32_IRQ_TIMER7_UP          (GD32_IRQ_EXINT + 44)  /* Vector  60: TIMER7 update */
#define GD32_IRQ_TIMER7_TRG_CMT     (GD32_IRQ_EXINT + 45)  /* Vector  61: TIMER7 trigger and commutation */
#define GD32_IRQ_TIMER7_CHANNEL     (GD32_IRQ_EXINT + 46)  /* Vector  62: TIMER7 capture compare */
#define GD32_IRQ_DMA0_CHANNEL7      (GD32_IRQ_EXINT + 47)  /* Vector  63: DMA0 channel 7 */
#define GD32_IRQ_EXMC               (GD32_IRQ_EXINT + 48)  /* Vector  64: EXMC */
#define GD32_IRQ_SDIO0              (GD32_IRQ_EXINT + 49)  /* Vector  65: SDIO0 */
#define GD32_IRQ_TIMER4             (GD32_IRQ_EXINT + 50)  /* Vector  66: TIMER4 */
#define GD32_IRQ_SPI2               (GD32_IRQ_EXINT + 51)  /* Vector  67: SPI2 */
#define GD32_IRQ_UART3              (GD32_IRQ_EXINT + 52)  /* Vector  68: UART3 */
#define GD32_IRQ_UART4              (GD32_IRQ_EXINT + 53)  /* Vector  69: UART4 */
#define GD32_IRQ_TIMER5_DAC_UDR     (GD32_IRQ_EXINT + 54)  /* Vector  70: TIMER5 global, DAC0/1 underrun */
#define GD32_IRQ_TIMER6             (GD32_IRQ_EXINT + 55)  /* Vector  71: TIMER6 */
#define GD32_IRQ_DMA1_CHANNEL0      (GD32_IRQ_EXINT + 56)  /* Vector  72: DMA1 channel 0 */
#define GD32_IRQ_DMA1_CHANNEL1      (GD32_IRQ_EXINT + 57)  /* Vector  73: DMA1 channel 1 */
#define GD32_IRQ_DMA1_CHANNEL2      (GD32_IRQ_EXINT + 58)  /* Vector  74: DMA1 channel 2 */
#define GD32_IRQ_DMA1_CHANNEL3      (GD32_IRQ_EXINT + 59)  /* Vector  75: DMA1 channel 3 */
#define GD32_IRQ_DMA1_CHANNEL4      (GD32_IRQ_EXINT + 60)  /* Vector  76: DMA1 channel 4 */
#define GD32_IRQ_ENET0              (GD32_IRQ_EXINT + 61)  /* Vector  77: ENET0 */
#define GD32_IRQ_ENET0_WKUP         (GD32_IRQ_EXINT + 62)  /* Vector  78: ENET0 wakeup through EXTI line */
#define GD32_IRQ_DMA1_CHANNEL5      (GD32_IRQ_EXINT + 68)  /* Vector  84: DMA1 channel 5 */
#define GD32_IRQ_DMA1_CHANNEL6      (GD32_IRQ_EXINT + 69)  /* Vector  85: DMA1 channel 6 */
#define GD32_IRQ_DMA1_CHANNEL7      (GD32_IRQ_EXINT + 70)  /* Vector  86: DMA1 channel 7 */
#define GD32_IRQ_USART5             (GD32_IRQ_EXINT + 71)  /* Vector  87: USART5 global and wakeup */
#define GD32_IRQ_I2C2_EV            (GD32_IRQ_EXINT + 72)  /* Vector  88: I2C2 event */
#define GD32_IRQ_I2C2_ER            (GD32_IRQ_EXINT + 73)  /* Vector  89: I2C2 error */
#define GD32_IRQ_USBHS0_EP1_OUT     (GD32_IRQ_EXINT + 74)  /* Vector  90: USBHS0 endpoint 1 out */
#define GD32_IRQ_USBHS0_EP1_IN      (GD32_IRQ_EXINT + 75)  /* Vector  91: USBHS0 endpoint 1 in */
#define GD32_IRQ_USBHS0_WKUP        (GD32_IRQ_EXINT + 76)  /* Vector  92: USBHS0 wakeup through EXTI line */
#define GD32_IRQ_USBHS0             (GD32_IRQ_EXINT + 77)  /* Vector  93: USBHS0 */
#define GD32_IRQ_DCI                (GD32_IRQ_EXINT + 78)  /* Vector  94: DCI */
#define GD32_IRQ_CAU                (GD32_IRQ_EXINT + 79)  /* Vector  95: CAU */
#define GD32_IRQ_HAU_TRNG           (GD32_IRQ_EXINT + 80)  /* Vector  96: HAU and TRNG */
#define GD32_IRQ_FPU                (GD32_IRQ_EXINT + 81)  /* Vector  97: FPU */
#define GD32_IRQ_UART6              (GD32_IRQ_EXINT + 82)  /* Vector  98: UART6 */
#define GD32_IRQ_UART7              (GD32_IRQ_EXINT + 83)  /* Vector  99: UART7 */
#define GD32_IRQ_SPI3               (GD32_IRQ_EXINT + 84)  /* Vector 100: SPI3 */
#define GD32_IRQ_SPI4               (GD32_IRQ_EXINT + 85)  /* Vector 101: SPI4 */
#define GD32_IRQ_SPI5               (GD32_IRQ_EXINT + 86)  /* Vector 102: SPI5 */
#define GD32_IRQ_SAI0               (GD32_IRQ_EXINT + 87)  /* Vector 103: SAI0 */
#define GD32_IRQ_TLI                (GD32_IRQ_EXINT + 88)  /* Vector 104: TLI */
#define GD32_IRQ_TLI_ER             (GD32_IRQ_EXINT + 89)  /* Vector 105: TLI error */
#define GD32_IRQ_IPA                (GD32_IRQ_EXINT + 90)  /* Vector 106: IPA */
#define GD32_IRQ_SAI1               (GD32_IRQ_EXINT + 91)  /* Vector 107: SAI1 */
#define GD32_IRQ_OSPI0              (GD32_IRQ_EXINT + 92)  /* Vector 108: OSPI0 global */
#define GD32_IRQ_I2C3_EV            (GD32_IRQ_EXINT + 95)  /* Vector 111: I2C3 event */
#define GD32_IRQ_I2C3_ER            (GD32_IRQ_EXINT + 96)  /* Vector 112: I2C3 error */
#define GD32_IRQ_RSPDIF             (GD32_IRQ_EXINT + 97)  /* Vector 113: RSPDIF global */
#define GD32_IRQ_DMAMUX_OVR         (GD32_IRQ_EXINT + 102) /* Vector 118: DMAMUX overrun */
#define GD32_IRQ_HPDF_INT0          (GD32_IRQ_EXINT + 110) /* Vector 126: HPDF filter 0 */
#define GD32_IRQ_HPDF_INT1          (GD32_IRQ_EXINT + 111) /* Vector 127: HPDF filter 1 */
#define GD32_IRQ_HPDF_INT2          (GD32_IRQ_EXINT + 112) /* Vector 128: HPDF filter 2 */
#define GD32_IRQ_HPDF_INT3          (GD32_IRQ_EXINT + 113) /* Vector 129: HPDF filter 3 */
#define GD32_IRQ_SAI2               (GD32_IRQ_EXINT + 114) /* Vector 130: SAI2 */
#define GD32_IRQ_TIMER14            (GD32_IRQ_EXINT + 116) /* Vector 132: TIMER14 */
#define GD32_IRQ_TIMER15            (GD32_IRQ_EXINT + 117) /* Vector 133: TIMER15 */
#define GD32_IRQ_TIMER16            (GD32_IRQ_EXINT + 118) /* Vector 134: TIMER16 */
#define GD32_IRQ_MDIO               (GD32_IRQ_EXINT + 120) /* Vector 136: MDIO */
#define GD32_IRQ_MDMA               (GD32_IRQ_EXINT + 122) /* Vector 138: MDMA */
#define GD32_IRQ_SDIO1              (GD32_IRQ_EXINT + 124) /* Vector 140: SDIO1 */
#define GD32_IRQ_HWSEM              (GD32_IRQ_EXINT + 125) /* Vector 141: HWSEM */
#define GD32_IRQ_ADC2               (GD32_IRQ_EXINT + 127) /* Vector 143: ADC2 */
#define GD32_IRQ_CMP0_1             (GD32_IRQ_EXINT + 137) /* Vector 153: CMP0 and CMP1 */
#define GD32_IRQ_CTC                (GD32_IRQ_EXINT + 144) /* Vector 160: CTC */
#define GD32_IRQ_RAMECCMU           (GD32_IRQ_EXINT + 145) /* Vector 161: RAMECCMU */
#define GD32_IRQ_OSPI1              (GD32_IRQ_EXINT + 150) /* Vector 166: OSPI1 global */
#define GD32_IRQ_RTDEC0             (GD32_IRQ_EXINT + 151) /* Vector 167: RTDEC0 */
#define GD32_IRQ_RTDEC1             (GD32_IRQ_EXINT + 152) /* Vector 168: RTDEC1 */
#define GD32_IRQ_FAC                (GD32_IRQ_EXINT + 153) /* Vector 169: FAC */
#define GD32_IRQ_TMU                (GD32_IRQ_EXINT + 154) /* Vector 170: TMU */
#define GD32_IRQ_TIMER22            (GD32_IRQ_EXINT + 161) /* Vector 177: TIMER22 */
#define GD32_IRQ_TIMER23            (GD32_IRQ_EXINT + 162) /* Vector 178: TIMER23 */
#define GD32_IRQ_TIMER30            (GD32_IRQ_EXINT + 163) /* Vector 179: TIMER30 */
#define GD32_IRQ_TIMER31            (GD32_IRQ_EXINT + 164) /* Vector 180: TIMER31 */
#define GD32_IRQ_TIMER40            (GD32_IRQ_EXINT + 165) /* Vector 181: TIMER40 */
#define GD32_IRQ_TIMER41            (GD32_IRQ_EXINT + 166) /* Vector 182: TIMER41 */
#define GD32_IRQ_TIMER42            (GD32_IRQ_EXINT + 167) /* Vector 183: TIMER42 */
#define GD32_IRQ_TIMER43            (GD32_IRQ_EXINT + 168) /* Vector 184: TIMER43 */
#define GD32_IRQ_TIMER44            (GD32_IRQ_EXINT + 169) /* Vector 185: TIMER44 */
#define GD32_IRQ_TIMER50            (GD32_IRQ_EXINT + 170) /* Vector 186: TIMER50 */
#define GD32_IRQ_TIMER51            (GD32_IRQ_EXINT + 171) /* Vector 187: TIMER51 */
#define GD32_IRQ_USBHS1_EP1_OUT     (GD32_IRQ_EXINT + 172) /* Vector 188: USBHS1 endpoint 1 out */
#define GD32_IRQ_USBHS1_EP1_IN      (GD32_IRQ_EXINT + 173) /* Vector 189: USBHS1 endpoint 1 in */
#define GD32_IRQ_USBHS1_WKUP        (GD32_IRQ_EXINT + 174) /* Vector 190: USBHS1 wakeup through EXTI line */
#define GD32_IRQ_USBHS1             (GD32_IRQ_EXINT + 175) /* Vector 191: USBHS1 */
#define GD32_IRQ_ENET1              (GD32_IRQ_EXINT + 176) /* Vector 192: ENET1 */
#define GD32_IRQ_ENET1_WKUP         (GD32_IRQ_EXINT + 177) /* Vector 193: ENET1 wakeup through EXTI line */
#define GD32_IRQ_CAN0_WKUP          (GD32_IRQ_EXINT + 179) /* Vector 195: CAN0 wakeup through EXTI line */
#define GD32_IRQ_CAN0_MESSAGE       (GD32_IRQ_EXINT + 180) /* Vector 196: CAN0 message buffer */
#define GD32_IRQ_CAN0_BUSOFF        (GD32_IRQ_EXINT + 181) /* Vector 197: CAN0 bus off / bus off done */
#define GD32_IRQ_CAN0_ERROR         (GD32_IRQ_EXINT + 182) /* Vector 198: CAN0 error */
#define GD32_IRQ_CAN0_FASTERROR     (GD32_IRQ_EXINT + 183) /* Vector 199: CAN0 error in fast transmission */
#define GD32_IRQ_CAN0_TEC           (GD32_IRQ_EXINT + 184) /* Vector 200: CAN0 transmit warning */
#define GD32_IRQ_CAN0_REC           (GD32_IRQ_EXINT + 185) /* Vector 201: CAN0 receive warning */
#define GD32_IRQ_CAN1_WKUP          (GD32_IRQ_EXINT + 186) /* Vector 202: CAN1 wakeup through EXTI line */
#define GD32_IRQ_CAN1_MESSAGE       (GD32_IRQ_EXINT + 187) /* Vector 203: CAN1 message buffer */
#define GD32_IRQ_CAN1_BUSOFF        (GD32_IRQ_EXINT + 188) /* Vector 204: CAN1 bus off / bus off done */
#define GD32_IRQ_CAN1_ERROR         (GD32_IRQ_EXINT + 189) /* Vector 205: CAN1 error */
#define GD32_IRQ_CAN1_FASTERROR     (GD32_IRQ_EXINT + 190) /* Vector 206: CAN1 error in fast transmission */
#define GD32_IRQ_CAN1_TEC           (GD32_IRQ_EXINT + 191) /* Vector 207: CAN1 transmit warning */
#define GD32_IRQ_CAN1_REC           (GD32_IRQ_EXINT + 192) /* Vector 208: CAN1 receive warning */
#define GD32_IRQ_CAN2_WKUP          (GD32_IRQ_EXINT + 193) /* Vector 209: CAN2 wakeup through EXTI line */
#define GD32_IRQ_CAN2_MESSAGE       (GD32_IRQ_EXINT + 194) /* Vector 210: CAN2 message buffer */
#define GD32_IRQ_CAN2_BUSOFF        (GD32_IRQ_EXINT + 195) /* Vector 211: CAN2 bus off / bus off done */
#define GD32_IRQ_CAN2_ERROR         (GD32_IRQ_EXINT + 196) /* Vector 212: CAN2 error */
#define GD32_IRQ_CAN2_FASTERROR     (GD32_IRQ_EXINT + 197) /* Vector 213: CAN2 error in fast transmission */
#define GD32_IRQ_CAN2_TEC           (GD32_IRQ_EXINT + 198) /* Vector 214: CAN2 transmit warning */
#define GD32_IRQ_CAN2_REC           (GD32_IRQ_EXINT + 199) /* Vector 215: CAN2 receive warning */
#define GD32_IRQ_EFUSE              (GD32_IRQ_EXINT + 200) /* Vector 216: EFUSE */
#define GD32_IRQ_I2C0_WKUP          (GD32_IRQ_EXINT + 201) /* Vector 217: I2C0 wakeup through EXTI line */
#define GD32_IRQ_I2C1_WKUP          (GD32_IRQ_EXINT + 202) /* Vector 218: I2C1 wakeup through EXTI line */
#define GD32_IRQ_I2C2_WKUP          (GD32_IRQ_EXINT + 203) /* Vector 219: I2C2 wakeup through EXTI line */
#define GD32_IRQ_I2C3_WKUP          (GD32_IRQ_EXINT + 204) /* Vector 220: I2C3 wakeup through EXTI line */
#define GD32_IRQ_LPDTS              (GD32_IRQ_EXINT + 205) /* Vector 221: LPDTS */
#define GD32_IRQ_LPDTS_WKUP         (GD32_IRQ_EXINT + 206) /* Vector 222: LPDTS wakeup through EXTI line */
#define GD32_IRQ_TIMER0_DEC         (GD32_IRQ_EXINT + 207) /* Vector 223: TIMER0 DEC */
#define GD32_IRQ_TIMER7_DEC         (GD32_IRQ_EXINT + 208) /* Vector 224: TIMER7 DEC */
#define GD32_IRQ_TIMER1_DEC         (GD32_IRQ_EXINT + 209) /* Vector 225: TIMER1 DEC */
#define GD32_IRQ_TIMER2_DEC         (GD32_IRQ_EXINT + 210) /* Vector 226: TIMER2 DEC */
#define GD32_IRQ_TIMER3_DEC         (GD32_IRQ_EXINT + 211) /* Vector 227: TIMER3 DEC */
#define GD32_IRQ_TIMER4_DEC         (GD32_IRQ_EXINT + 212) /* Vector 228: TIMER4 DEC */
#define GD32_IRQ_TIMER22_DEC        (GD32_IRQ_EXINT + 213) /* Vector 229: TIMER22 DEC */
#define GD32_IRQ_TIMER23_DEC        (GD32_IRQ_EXINT + 214) /* Vector 230: TIMER23 DEC */
#define GD32_IRQ_TIMER30_DEC        (GD32_IRQ_EXINT + 215) /* Vector 231: TIMER30 DEC */
#define GD32_IRQ_TIMER31_DEC        (GD32_IRQ_EXINT + 216) /* Vector 232: TIMER31 DEC */

/* Total number of external interrupt vectors (the highest used vector + 1) */

#define GD32_IRQ_NEXTINT            (217)

/* Total number of IRQ numbers: 16 core exceptions + GD32_IRQ_NEXTINT
 * external interrupts.
 */

#define NR_IRQS                     (GD32_IRQ_EXINT + GD32_IRQ_NEXTINT)

#endif /* __ARCH_ARM_INCLUDE_GD32H7_GD32H7XX_IRQ_H */
