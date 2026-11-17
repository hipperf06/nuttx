/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_dma.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_GD32H7XX_DMA_H
#define __ARCH_ARM_SRC_GD32H7_GD32H7XX_DMA_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <nuttx/irq.h>

#include "chip.h"
#include "hardware/gd32h7xx_dma.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Channel/peripheral encoding for gd32_dma_channel_alloc():
 *
 *   periph_req = (dma_index << DMA_CHANNEL_SHIFT) | (channel << DMA_SUBPERI_SHIFT)
 *
 * where dma_index 0 = DMA0, 1 = DMA1 and channel selects one of the 8
 * channels of the DMA controller.
 */

#define DMA_CHANNEL_SHIFT             (0)
#define DMA_CHANNEL_MASK              (0xf)
#define DMA_SUBPERI_SHIFT             (4)
#define DMA_SUBPERI_MASK              (0x7)

/* Helper to build the periph_req encoding */

#define GD32_DMA_REQ(dma_index, chan) \
  (((dma_index) << DMA_SUBPERI_SHIFT) | ((chan) << DMA_CHANNEL_SHIFT))

/* Standard request encodings (DMA controller index, channel) */

#define DMA_REQ_DMA0_CH0              GD32_DMA_REQ(0, 0)
#define DMA_REQ_DMA0_CH1              GD32_DMA_REQ(0, 1)
#define DMA_REQ_DMA0_CH2              GD32_DMA_REQ(0, 2)
#define DMA_REQ_DMA0_CH3              GD32_DMA_REQ(0, 3)
#define DMA_REQ_DMA0_CH4              GD32_DMA_REQ(0, 4)
#define DMA_REQ_DMA0_CH5              GD32_DMA_REQ(0, 5)
#define DMA_REQ_DMA0_CH6              GD32_DMA_REQ(0, 6)
#define DMA_REQ_DMA0_CH7              GD32_DMA_REQ(0, 7)

#define DMA_REQ_DMA1_CH0              GD32_DMA_REQ(1, 0)
#define DMA_REQ_DMA1_CH1              GD32_DMA_REQ(1, 1)
#define DMA_REQ_DMA1_CH2              GD32_DMA_REQ(1, 2)
#define DMA_REQ_DMA1_CH3              GD32_DMA_REQ(1, 3)
#define DMA_REQ_DMA1_CH4              GD32_DMA_REQ(1, 4)
#define DMA_REQ_DMA1_CH5              GD32_DMA_REQ(1, 5)
#define DMA_REQ_DMA1_CH6              GD32_DMA_REQ(1, 6)
#define DMA_REQ_DMA1_CH7              GD32_DMA_REQ(1, 7)

/* DMAMUX request numbers (RM_CHXCFG_MUXID values) for the common
 * peripherals.  One DMA channel must be routed to the request line of the
 * peripheral that it services.
 */

#define DMA_REQUEST_M2M               0U
#define DMA_REQUEST_ADC0              9U
#define DMA_REQUEST_ADC1              10U
#define DMA_REQUEST_I2C0_RX           43U
#define DMA_REQUEST_I2C0_TX           44U
#define DMA_REQUEST_I2C1_RX           45U
#define DMA_REQUEST_I2C1_TX           46U
#define DMA_REQUEST_SPI0_RX           47U
#define DMA_REQUEST_SPI0_TX           48U
#define DMA_REQUEST_SPI1_RX           49U
#define DMA_REQUEST_SPI1_TX           50U
#define DMA_REQUEST_USART0_RX         51U
#define DMA_REQUEST_USART0_TX         52U
#define DMA_REQUEST_USART1_RX         53U

/* Timer requests (DMAMUX request IDs 11..27, 29..42, 57..84, 109..122).
 * Needed for the timer DMA burst used to emit DShot frames.
 */

#define DMA_REQUEST_TIMER0_CH0        11U
#define DMA_REQUEST_TIMER0_CH1        12U
#define DMA_REQUEST_TIMER0_CH2        13U
#define DMA_REQUEST_TIMER0_CH3        14U
#define DMA_REQUEST_TIMER0_MCH0       15U
#define DMA_REQUEST_TIMER0_MCH1       16U
#define DMA_REQUEST_TIMER0_MCH2       17U
#define DMA_REQUEST_TIMER0_MCH3       18U
#define DMA_REQUEST_TIMER0_UP         19U
#define DMA_REQUEST_TIMER0_TRG        20U
#define DMA_REQUEST_TIMER0_CMT        21U

#define DMA_REQUEST_TIMER1_CH0        22U
#define DMA_REQUEST_TIMER1_CH1        23U
#define DMA_REQUEST_TIMER1_CH2        24U
#define DMA_REQUEST_TIMER1_CH3        25U
#define DMA_REQUEST_TIMER1_UP         26U
#define DMA_REQUEST_TIMER1_TRG        27U

#define DMA_REQUEST_TIMER2_CH0        29U
#define DMA_REQUEST_TIMER2_CH1        30U
#define DMA_REQUEST_TIMER2_CH2        31U
#define DMA_REQUEST_TIMER2_CH3        32U
#define DMA_REQUEST_TIMER2_UP         33U
#define DMA_REQUEST_TIMER2_TRG        35U

#define DMA_REQUEST_TIMER3_CH0        36U
#define DMA_REQUEST_TIMER3_CH1        37U
#define DMA_REQUEST_TIMER3_CH2        38U
#define DMA_REQUEST_TIMER3_CH3        39U
#define DMA_REQUEST_TIMER3_UP         42U
#define DMA_REQUEST_TIMER3_TRG        41U

#define DMA_REQUEST_TIMER7_CH0        57U
#define DMA_REQUEST_TIMER7_CH1        58U
#define DMA_REQUEST_TIMER7_CH2        59U
#define DMA_REQUEST_TIMER7_CH3        60U
#define DMA_REQUEST_TIMER7_UP         65U
#define DMA_REQUEST_TIMER7_TRG        66U

#define DMA_REQUEST_TIMER4_CH0        68U
#define DMA_REQUEST_TIMER4_CH1        69U
#define DMA_REQUEST_TIMER4_CH2        70U
#define DMA_REQUEST_TIMER4_CH3        71U
#define DMA_REQUEST_TIMER4_UP         72U
#define DMA_REQUEST_TIMER4_TRG        74U

#define DMA_REQUEST_TIMER5_UP         83U
#define DMA_REQUEST_TIMER6_UP         84U

#define DMA_REQUEST_TIMER14_CH0       109U
#define DMA_REQUEST_TIMER14_CH1       110U
#define DMA_REQUEST_TIMER14_UP        112U

#define DMA_REQUEST_TIMER15_CH0       115U
#define DMA_REQUEST_TIMER15_UP        118U

#define DMA_REQUEST_TIMER16_CH0       119U
#define DMA_REQUEST_TIMER16_UP        122U
#define DMA_REQUEST_USART1_TX         54U
#define DMA_REQUEST_USART2_RX         55U
#define DMA_REQUEST_USART2_TX         56U
#define DMA_REQUEST_SPI2_RX           75U
#define DMA_REQUEST_SPI2_TX           76U
#define DMA_REQUEST_UART3_RX          77U
#define DMA_REQUEST_UART3_TX          78U
#define DMA_REQUEST_UART4_RX          79U
#define DMA_REQUEST_UART4_TX          80U
#define DMA_REQUEST_DAC_CH0           81U
#define DMA_REQUEST_DAC_CH1           82U
#define DMA_REQUEST_USART5_RX         85U
#define DMA_REQUEST_USART5_TX         86U
#define DMA_REQUEST_I2C2_RX           87U
#define DMA_REQUEST_I2C2_TX           88U
#define DMA_REQUEST_UART6_RX          93U
#define DMA_REQUEST_UART6_TX          94U
#define DMA_REQUEST_UART7_RX          95U
#define DMA_REQUEST_UART7_TX          96U
#define DMA_REQUEST_SPI3_RX           97U
#define DMA_REQUEST_SPI3_TX           98U
#define DMA_REQUEST_SPI4_RX           99U
#define DMA_REQUEST_SPI4_TX           100U
#define DMA_REQUEST_SPI5_RX           182U
#define DMA_REQUEST_SPI5_TX           183U
#define DMA_REQUEST_I2C3_RX           184U
#define DMA_REQUEST_I2C3_TX           185U
#define DMA_REQUEST_CAN0              186U
#define DMA_REQUEST_CAN1              187U
#define DMA_REQUEST_CAN2              188U

/* Direction / width / priority / mode selectors (consumed by
 * gd32_dma_setup() through the parameter structure)
 */

#define DMA_WIDTH_8BITS_SELECT        0
#define DMA_WIDTH_16BITS_SELECT       1
#define DMA_WIDTH_32BITS_SELECT       2

#define DMA_PRIO_LOW_SELECT           0
#define DMA_PRIO_MEDIUM_SELECT        1
#define DMA_PRIO_HIGH_SELECT          2
#define DMA_PRIO_ULTRA_HIGHSELECT     3

/* DMA_HANDLE provides an opaque reference that can be used to represent a
 * DMA channel.
 */

typedef void *DMA_HANDLE;

/* Description:
 *   This is the type of the callback that is used to inform the user of the
 *   completion of the DMA.
 *
 * Input Parameters:
 *   handle - Refers to the DMA channel
 *   status - A bit encoded value that provides the completion status
 *   arg    - A user-provided value that was provided when gd32_dma_start()
 *            was called.
 */

typedef void (*dma_callback_t)(DMA_HANDLE handle, uint16_t status,
                               void *arg);

/* DMA single data mode initialize struct */

struct gd32_dma_param_s
{
  uint32_t periph_addr;          /* Peripheral base address */
  uint32_t memory0_addr;         /* Memory 0 base address */
  uint16_t number;               /* Channel transfer number */
  uint8_t periph_inc;            /* Peripheral increasing mode */
  uint8_t memory_inc;            /* Memory increasing mode */
  uint8_t periph_memory_width;   /* Transfer data size of peripheral */
  uint8_t circular_mode;         /* DMA circular mode */
  uint8_t direction;             /* Channel data transfer direction */
  uint8_t priority;              /* Channel priority level */
};

#ifndef __ASSEMBLY__

#if defined(__cplusplus)
extern "C"
{
#endif

/****************************************************************************
 * Name: arm_dma_initialize
 *
 * Description:
 *   Initialize the DMA subsystem (called by the NuttX core when
 *   CONFIG_ARCH_DMA is enabled).
 *
 ****************************************************************************/

#ifdef CONFIG_ARCH_DMA
void weak_function arm_dma_initialize(void);
#endif

/****************************************************************************
 * Name: gd32_dma_channel_alloc
 *
 * Description:
 *   Allocate a DMA channel.  This function gives the caller mutually
 *   exclusive access to the DMA channel specified by the 'periph_req'
 *   argument.  A DMA channel only can be used by one peripheral at a time.
 *
 * Input Parameters:
 *   periph_req - Identifies which DMA controller and channel is requested
 *
 * Returned Value:
 *   If periph_req is valid, this function returns a non-NULL DMA channel
 *   handle; NULL is returned on failure.
 *
 ****************************************************************************/

DMA_HANDLE gd32_dma_channel_alloc(uint8_t periph_req);

/****************************************************************************
 * Name: gd32_dma_channel_free
 *
 * Description:
 *   Release a DMA channel.
 *
 ****************************************************************************/

void gd32_dma_channel_free(DMA_HANDLE handle);

/****************************************************************************
 * Name: gd32_dma_channel_request
 *
 * Description:
 *   Route a DMA channel to a peripheral request line through the DMAMUX.
 *
 * Input Parameters:
 *   handle  - DMA channel handle from gd32_dma_channel_alloc()
 *   request - DMAMUX request number (DMA_REQUEST_xxx)
 *
 ****************************************************************************/

void gd32_dma_channel_request(DMA_HANDLE handle, uint32_t request);

/****************************************************************************
 * Name: gd32_dma_setup
 *
 * Description:
 *   Configure a DMA channel before starting a transfer.
 *
 ****************************************************************************/

void gd32_dma_setup(DMA_HANDLE handle,
                    FAR struct gd32_dma_param_s *param);

/****************************************************************************
 * Name: gd32_dma_start
 *
 * Description:
 *   Start the DMA transfer.
 *
 ****************************************************************************/

void gd32_dma_start(DMA_HANDLE handle, dma_callback_t callback, void *arg,
                    uint32_t interrupt);

/****************************************************************************
 * Name: gd32_dma_stop
 *
 * Description:
 *   Cancel the DMA.  After gd32_dma_stop() is called, gd32_dma_setup()
 *   must be called before gd32_dma_start() can be called again.
 *
 ****************************************************************************/

void gd32_dma_stop(DMA_HANDLE handle);

/****************************************************************************
 * Name: gd32_dma_residuate / gd32_dma_txnum
 *
 * Description:
 *   Get the number of remaining data to be transferred by the DMA.
 *
 ****************************************************************************/

size_t gd32_dma_residuate(DMA_HANDLE handle);
size_t gd32_dma_txnum(DMA_HANDLE handle);

#if defined(__cplusplus)
}
#endif
#endif /* __ASSEMBLY__ */
#endif /* __ARCH_ARM_SRC_GD32H7_GD32H7XX_DMA_H */
