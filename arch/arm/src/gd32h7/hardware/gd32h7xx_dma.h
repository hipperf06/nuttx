/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h7xx_dma.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_DMA_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_DMA_H

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

/* The DMA controllers */

#define GD32H7_DMA0                   GD32H7_DMA0_BASE
#define GD32H7_DMA1                   GD32H7_DMA1_BASE
#define GD32H7_DMAMUX                 GD32H7_DMAMUX_BASE

/* Register offsets ------------------------------------------------------ */

#define GD32H7_DMA_INTF0_OFFSET       0x0000  /* Interrupt flag register 0 */
#define GD32H7_DMA_INTF1_OFFSET       0x0004  /* Interrupt flag register 1 */
#define GD32H7_DMA_INTC0_OFFSET       0x0008  /* Interrupt flag clear 0 */
#define GD32H7_DMA_INTC1_OFFSET       0x000c  /* Interrupt flag clear 1 */

/* Each DMA channel occupies 0x18 bytes of registers, starting at 0x0010:
 * CTL, CNT, PADDR, M0ADDR, M1ADDR, FCTL.
 */

#define GD32H7_DMA_CH_REGSTRIDE       0x0018

#define GD32H7_DMA_CHCTL_OFFSET(ch)   (0x0010 + (ch) * GD32H7_DMA_CH_REGSTRIDE)
#define GD32H7_DMA_CHCNT_OFFSET(ch)   (0x0014 + (ch) * GD32H7_DMA_CH_REGSTRIDE)
#define GD32H7_DMA_CHPADDR_OFFSET(ch) (0x0018 + (ch) * GD32H7_DMA_CH_REGSTRIDE)
#define GD32H7_DMA_CHM0ADDR_OFFSET(ch) \
                                      (0x001c + (ch) * GD32H7_DMA_CH_REGSTRIDE)
#define GD32H7_DMA_CHM1ADDR_OFFSET(ch) \
                                      (0x0020 + (ch) * GD32H7_DMA_CH_REGSTRIDE)
#define GD32H7_DMA_CHFCTL_OFFSET(ch)  (0x0024 + (ch) * GD32H7_DMA_CH_REGSTRIDE)

/* Register addresses ---------------------------------------------------- */

#define GD32H7_DMA_INTF0(dmax)        ((dmax) + GD32H7_DMA_INTF0_OFFSET)
#define GD32H7_DMA_INTF1(dmax)        ((dmax) + GD32H7_DMA_INTF1_OFFSET)
#define GD32H7_DMA_INTC0(dmax)        ((dmax) + GD32H7_DMA_INTC0_OFFSET)
#define GD32H7_DMA_INTC1(dmax)        ((dmax) + GD32H7_DMA_INTC1_OFFSET)

#define GD32H7_DMA_CHCTL(dmax, ch) \
  ((dmax) + GD32H7_DMA_CHCTL_OFFSET(ch))
#define GD32H7_DMA_CHCNT(dmax, ch) \
  ((dmax) + GD32H7_DMA_CHCNT_OFFSET(ch))
#define GD32H7_DMA_CHPADDR(dmax, ch) \
  ((dmax) + GD32H7_DMA_CHPADDR_OFFSET(ch))
#define GD32H7_DMA_CHM0ADDR(dmax, ch) \
  ((dmax) + GD32H7_DMA_CHM0ADDR_OFFSET(ch))
#define GD32H7_DMA_CHM1ADDR(dmax, ch) \
  ((dmax) + GD32H7_DMA_CHM1ADDR_OFFSET(ch))
#define GD32H7_DMA_CHFCTL(dmax, ch) \
  ((dmax) + GD32H7_DMA_CHFCTL_OFFSET(ch))

/* DMAMUX request multiplexer channel registers (0x04 per channel) */

#define GD32H7_DMAMUX_RM_CHXCFG(ch)   (GD32H7_DMAMUX + 0x0004 * (ch))

/* Interrupt flags ---------------------------------------------------------
 *
 * The channel interrupt flags are packed into INTF0/INTF1.  The flag layout
 * for each channel is (bit offsets from the DMA_FLAG_ADD() formula):
 *
 *   bit 0: FEEIF  FIFO error and exception
 *   bit 2: SDEIF  single data mode exception
 *   bit 3: TAEIF  transfer access error
 *   bit 4: HTFIF  half transfer finish
 *   bit 5: FTFIF  full transfer finish
 *
 * Channels 0-4 place their flags in INTF0, channels 5-7 in INTF1.
 */

#define DMA_INTF_FEEIF                (1 << 0)  /* FIFO error and exception flag */
#define DMA_INTF_SDEIF                (1 << 2)  /* Single data mode exception flag */
#define DMA_INTF_TAEIF                (1 << 3)  /* Transfer access error flag */
#define DMA_INTF_HTFIF                (1 << 4)  /* Half transfer finish flag */
#define DMA_INTF_FTFIF                (1 << 5)  /* Full transfer finish flag */

#define DMA_INTC_FEEIFC               (1 << 0)
#define DMA_INTC_SDEIFC               (1 << 2)
#define DMA_INTC_TAEIFC               (1 << 3)
#define DMA_INTC_HTFIFC               (1 << 4)
#define DMA_INTC_FTFIFC               (1 << 5)

#define DMA_INTF_ALLFLAGS \
  (DMA_INTF_FEEIF | DMA_INTF_SDEIF | DMA_INTF_TAEIF | \
   DMA_INTF_HTFIF | DMA_INTF_FTFIF)

/* The flag field for one channel spans 6 bits with a 4-bit gap inserted
 * after every second channel (channels 2-3 and 6-7):
 *
 *   ch0: bits  0- 4        ch4: bits 24-28
 *   ch1: bits  6-10        ch5: bits 30-34 (INTF1)
 *   ch2: bits 12-16        ch6: bits 40-44 (INTF1)
 *   ch3: bits 22-26        ch7: bits 46-50 (INTF1)
 */

#define GD32H7_DMA_FLAG_ADD(flag, ch) \
  ((uint32_t)(flag) << ((((uint32_t)(ch)) * 6U) + \
   (((uint32_t)(ch)) >> 1U & 0x01U) * 4U))

/* Channel control register bits (DMA_CHxCTL) ------------------------------ */

#define DMA_CHXCTL_CHEN               (1 << 0)   /* Channel enable */
#define DMA_CHXCTL_SDEIE              (1 << 1)   /* Single data exception int enable */
#define DMA_CHXCTL_TAEIE              (1 << 2)   /* Transfer access error int enable */
#define DMA_CHXCTL_HTFIE              (1 << 3)   /* Half transfer finish int enable */
#define DMA_CHXCTL_FTFIE              (1 << 4)   /* Full transfer finish int enable */

#define DMA_CHXCTL_TM_SHIFT           (6)        /* Bits 6-7: transfer mode */
#define DMA_CHXCTL_TM_MASK            (3 << DMA_CHXCTL_TM_SHIFT)
#define DMA_CHXCTL_TM(n)              ((n) << DMA_CHXCTL_TM_SHIFT)
#  define DMA_PERIPH_TO_MEMORY        DMA_CHXCTL_TM(0)
#  define DMA_MEMORY_TO_PERIPH        DMA_CHXCTL_TM(1)
#  define DMA_MEMORY_TO_MEMORY        DMA_CHXCTL_TM(2)

#define DMA_CHXCTL_CMEN               (1 << 8)   /* Circulation mode enable */
#define DMA_CHXCTL_PNAGA              (1 << 9)   /* Peripheral next address gen algo */
#define DMA_CHXCTL_MNAGA              (1 << 10)  /* Memory next address gen algo */

#define DMA_CHXCTL_PWIDTH_SHIFT       (11)       /* Bits 11-12: peripheral width */
#define DMA_CHXCTL_PWIDTH_MASK        (3 << DMA_CHXCTL_PWIDTH_SHIFT)
#define DMA_CHXCTL_PWIDTH(n)          ((n) << DMA_CHXCTL_PWIDTH_SHIFT)
#  define DMA_PERIPH_WIDTH_8BIT       DMA_CHXCTL_PWIDTH(0)
#  define DMA_PERIPH_WIDTH_16BIT      DMA_CHXCTL_PWIDTH(1)
#  define DMA_PERIPH_WIDTH_32BIT      DMA_CHXCTL_PWIDTH(2)

#define DMA_CHXCTL_MWIDTH_SHIFT       (13)       /* Bits 13-14: memory width */
#define DMA_CHXCTL_MWIDTH_MASK        (3 << DMA_CHXCTL_MWIDTH_SHIFT)
#define DMA_CHXCTL_MWIDTH(n)          ((n) << DMA_CHXCTL_MWIDTH_SHIFT)
#  define DMA_MEMORY_WIDTH_8BIT       DMA_CHXCTL_MWIDTH(0)
#  define DMA_MEMORY_WIDTH_16BIT      DMA_CHXCTL_MWIDTH(1)
#  define DMA_MEMORY_WIDTH_32BIT      DMA_CHXCTL_MWIDTH(2)

#define DMA_CHXCTL_PAIF               (1 << 15)  /* Peripheral address increment fixed */

#define DMA_CHXCTL_PRIO_SHIFT         (16)       /* Bits 16-17: priority level */
#define DMA_CHXCTL_PRIO_MASK          (3 << DMA_CHXCTL_PRIO_SHIFT)
#define DMA_CHXCTL_PRIO(n)            ((n) << DMA_CHXCTL_PRIO_SHIFT)
#  define DMA_PRIORITY_LOW            DMA_CHXCTL_PRIO(0)
#  define DMA_PRIORITY_MEDIUM         DMA_CHXCTL_PRIO(1)
#  define DMA_PRIORITY_HIGH           DMA_CHXCTL_PRIO(2)
#  define DMA_PRIORITY_ULTRA_HIGH     DMA_CHXCTL_PRIO(3)

#define DMA_CHXCTL_SBMEN              (1 << 18)  /* Switch-buffer mode enable */
#define DMA_CHXCTL_MBS                (1 << 19)  /* Memory buffer select */

#define DMA_CHXCTL_PBURST_SHIFT       (21)       /* Bits 21-22: peripheral burst */
#define DMA_CHXCTL_PBURST_MASK        (3 << DMA_CHXCTL_PBURST_SHIFT)
#define DMA_CHXCTL_PBURST(n)          ((n) << DMA_CHXCTL_PBURST_SHIFT)
#  define DMA_PERIPH_BURST_SINGLE     DMA_CHXCTL_PBURST(0)
#  define DMA_PERIPH_BURST_4_BEAT     DMA_CHXCTL_PBURST(1)
#  define DMA_PERIPH_BURST_8_BEAT     DMA_CHXCTL_PBURST(2)
#  define DMA_PERIPH_BURST_16_BEAT    DMA_CHXCTL_PBURST(3)

#define DMA_CHXCTL_MBURST_SHIFT       (23)       /* Bits 23-24: memory burst */
#define DMA_CHXCTL_MBURST_MASK        (3 << DMA_CHXCTL_MBURST_SHIFT)
#define DMA_CHXCTL_MBURST(n)          ((n) << DMA_CHXCTL_MBURST_SHIFT)
#  define DMA_MEMORY_BURST_SINGLE     DMA_CHXCTL_MBURST(0)
#  define DMA_MEMORY_BURST_4_BEAT     DMA_CHXCTL_MBURST(1)
#  define DMA_MEMORY_BURST_8_BEAT     DMA_CHXCTL_MBURST(2)
#  define DMA_MEMORY_BURST_16_BEAT    DMA_CHXCTL_MBURST(3)

/* Channel FIFO control register bits (DMA_CHxFCTL) ------------------------ */

#define DMA_CHXFCTL_FCCV_SHIFT        (0)        /* Bits 0-1: FIFO critical value */
#define DMA_CHXFCTL_FCCV_MASK         (3 << DMA_CHXFCTL_FCCV_SHIFT)
#define DMA_CHXFCTL_FCCV(n)           ((n) << DMA_CHXFCTL_FCCV_SHIFT)
#  define DMA_FIFO_1_WORD             DMA_CHXFCTL_FCCV(0)
#  define DMA_FIFO_2_WORD             DMA_CHXFCTL_FCCV(1)
#  define DMA_FIFO_3_WORD             DMA_CHXFCTL_FCCV(2)
#  define DMA_FIFO_4_WORD             DMA_CHXFCTL_FCCV(3)

#define DMA_CHXFCTL_MDMEN             (1 << 2)   /* Multi-data mode enable */
#define DMA_CHXFCTL_FCNT_SHIFT        (3)        /* Bits 3-5: FIFO counter */
#define DMA_CHXFCTL_FCNT_MASK         (7 << DMA_CHXFCTL_FCNT_SHIFT)
#define DMA_CHXFCTL_FEEIE             (1 << 7)   /* FIFO exception int enable */

/* DMA_CHxCNT: bits 0-15 transfer counter */

#define DMA_CHXCNT_CNT_MASK           (0xffff << 0)

/* DMAMUX request multiplexer channel configuration bits ------------------- */

#define DMAMUX_RM_CHXCFG_MUXID_SHIFT  (0)        /* Bits 0-7: request mux input id */
#define DMAMUX_RM_CHXCFG_MUXID_MASK   (0xff << DMAMUX_RM_CHXCFG_MUXID_SHIFT)
#define DMAMUX_RM_CHXCFG_MUXID(n)     ((n) << DMAMUX_RM_CHXCFG_MUXID_SHIFT)
#define DMAMUX_RM_CHXCFG_SOIE         (1 << 8)   /* Sync overrun interrupt enable */
#define DMAMUX_RM_CHXCFG_SYNCEN       (1 << 16)  /* Synchronization enable */

/* Status groupings ------------------------------------------------------- */

#define DMA_STATUS_ERROR \
  (DMA_INTF_FEEIF | DMA_INTF_SDEIF | DMA_INTF_TAEIF)
#define DMA_STATUS_SUCCESS \
  (DMA_INTF_HTFIF | DMA_INTF_FTFIF)

#define DMA_INTF_MASK                 (0x3f)
#define DMA_INT_MASK \
  (DMA_CHXCTL_SDEIE | DMA_CHXCTL_TAEIE | DMA_CHXCTL_HTFIE | DMA_CHXCTL_FTFIE)

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_DMA_H */
