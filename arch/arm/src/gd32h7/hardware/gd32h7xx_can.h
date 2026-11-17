/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h7xx_can.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_CAN_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_CAN_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "chip.h"
#include "hardware/gd32h73x_memorymap.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* CAN instances */

#define GD32H7_CAN0                   GD32H7_CAN0_BASE
#define GD32H7_CAN1                   GD32H7_CAN1_BASE
#define GD32H7_CAN2                   GD32H7_CAN2_BASE

/* Register offsets ------------------------------------------------------ */

#define GD32H7_CAN_CTL0_OFFSET        0x0000  /* Control register 0 */
#define GD32H7_CAN_CTL1_OFFSET        0x0004  /* Control register 1 */
#define GD32H7_CAN_TIMER_OFFSET       0x0008  /* Free running timer */
#define GD32H7_CAN_RMPUBF_OFFSET      0x0010  /* Rx mailbox public filter */
#define GD32H7_CAN_ERR0_OFFSET        0x001c  /* Error register 0 */
#define GD32H7_CAN_ERR1_OFFSET        0x0020  /* Error register 1 */
#define GD32H7_CAN_INTEN_OFFSET       0x0028  /* Interrupt enable */
#define GD32H7_CAN_STAT_OFFSET        0x0030  /* Status register */
#define GD32H7_CAN_CTL2_OFFSET        0x0034  /* Control register 2 */
#define GD32H7_CAN_BT_OFFSET          0x0050  /* Bit timing register */

/* Register addresses ---------------------------------------------------- */

#define GD32H7_CAN_CTL0(c)            ((c) + GD32H7_CAN_CTL0_OFFSET)
#define GD32H7_CAN_CTL1(c)            ((c) + GD32H7_CAN_CTL1_OFFSET)
#define GD32H7_CAN_TIMER(c)           ((c) + GD32H7_CAN_TIMER_OFFSET)
#define GD32H7_CAN_RMPUBF(c)          ((c) + GD32H7_CAN_RMPUBF_OFFSET)
#define GD32H7_CAN_ERR0(c)            ((c) + GD32H7_CAN_ERR0_OFFSET)
#define GD32H7_CAN_ERR1(c)            ((c) + GD32H7_CAN_ERR1_OFFSET)
#define GD32H7_CAN_INTEN(c)           ((c) + GD32H7_CAN_INTEN_OFFSET)
#define GD32H7_CAN_STAT(c)            ((c) + GD32H7_CAN_STAT_OFFSET)
#define GD32H7_CAN_CTL2(c)            ((c) + GD32H7_CAN_CTL2_OFFSET)
#define GD32H7_CAN_BT(c)              ((c) + GD32H7_CAN_BT_OFFSET)

/* Mailbox RAM: a mailbox slot is (payload + 2 descriptor words) of 4-byte
 * words starting at CAN base + 0x80.  In classical mode the payload is 8
 * bytes (2 words), so one slot is 4 words (16 bytes).  Slot 5 doubles as
 * the Rx FIFO.
 */

#define GD32H7_CAN_RAM(c)             ((c) + 0x0080)
#define GD32H7_CAN_MB_WORDS           4
#define GD32H7_CAN_FIFO_SLOT          5

/* Private filter registers */

#define GD32H7_CAN_RFIFOMPF(c, n)     ((c) + 0x0880 + (n) * 4)

/* CTL0 */

#define CAN_CTL0_MSZ_SHIFT            (0)      /* Mailbox RAM size (0..31) */
#define CAN_CTL0_MSZ_MASK             (0x1f << CAN_CTL0_MSZ_SHIFT)
#define CAN_CTL0_MST                  (1 << 12)
#define CAN_CTL0_SRDIS                (1 << 17) /* Self reception disable */
#define CAN_CTL0_INAS                 (1 << 24) /* Inactive acknowledge */
#define CAN_CTL0_SWRST                (1 << 25) /* Software reset */
#define CAN_CTL0_NRDY                 (1 << 27) /* Module not ready */
#define CAN_CTL0_HALT                 (1 << 28) /* Halt request */
#define CAN_CTL0_RFEN                 (1 << 29) /* Rx FIFO enable */
#define CAN_CTL0_INAMOD               (1 << 30) /* Inactive mode request */
#define CAN_CTL0_CANDIS               (1 << 31) /* CAN disable */

/* CTL1 */

#define CAN_CTL1_MMOD                 (1 << 3)  /* Monitor mode */
#define CAN_CTL1_LSCMOD               (1 << 12) /* Loopback/silent mode */

/* STAT - mailbox status bits (write 1 to clear) */

#define CAN_STAT_MS(n)                (1u << (n))
#define CAN_STAT_MS5_RFNE             CAN_STAT_MS(5)  /* FIFO not empty */

/* BT - bit timing */

#define CAN_BT_PBS2_SHIFT             (0)      /* Phase segment 2 (value-1) */
#define CAN_BT_PBS1_SHIFT             (5)      /* Phase segment 1 (value-1) */
#define CAN_BT_PTS_SHIFT              (10)     /* Propagation seg (value-1) */
#define CAN_BT_SJW_SHIFT              (16)     /* Resync jump width-1 */
#define CAN_BT_BAUDPSC_SHIFT          (21)     /* Baud prescaler (value-1) */

#define CAN_BT(psc, sjw, pts, pbs1, pbs2) \
  ((((psc) - 1) << CAN_BT_BAUDPSC_SHIFT) | \
   (((sjw) - 1) << CAN_BT_SJW_SHIFT) | \
   (((pts) - 1) << CAN_BT_PTS_SHIFT) | \
   (((pbs1) - 1) << CAN_BT_PBS1_SHIFT) | \
   (((pbs2) - 1) << CAN_BT_PBS2_SHIFT))

/* Mailbox descriptor words (in the mailbox RAM) */

#define CAN_MDES0_TIMESTAMP_SHIFT     (0)
#define CAN_MDES0_DLC_SHIFT           (16)
#define CAN_MDES0_RTR                 (1 << 20)
#define CAN_MDES0_IDE                 (1 << 21)
#define CAN_MDES0_SRR                 (1 << 22)
#define CAN_MDES0_CODE_SHIFT          (24)
#define CAN_MDES0_FDF                 (1 << 31)

#define CAN_MDES1_ID_STD_SHIFT        (18)
#define CAN_MDES1_ID_EXD_MASK         0x1fffffff

/* Mailbox codes */

#define CAN_MB_RX_INACTIVE            0
#define CAN_MB_RX_FULL                2
#define CAN_MB_RX_EMPTY               4
#define CAN_MB_RX_OVERRUN             6
#define CAN_MB_TX_INACTIVE            8
#define CAN_MB_TX_ABORT               9
#define CAN_MB_TX_DATA                12

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_CAN_H */
