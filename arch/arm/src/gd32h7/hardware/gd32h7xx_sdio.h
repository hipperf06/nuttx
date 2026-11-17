/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h7xx_sdio.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_SDIO_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_SDIO_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include "chip.h"
#include "hardware/gd32h73x_memorymap.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define GD32H7_SDIO0                  GD32H7_SDIO0_BASE

/* Register offsets ------------------------------------------------------ */

#define GD32H7_SDIO_PWRCTL_OFFSET     0x0000
#define GD32H7_SDIO_CLKCTL_OFFSET     0x0004
#define GD32H7_SDIO_CMDAGMT_OFFSET    0x0008
#define GD32H7_SDIO_CMDCTL_OFFSET     0x000c
#define GD32H7_SDIO_RSPCMDIDX_OFFSET  0x0010
#define GD32H7_SDIO_RESP0_OFFSET      0x0014
#define GD32H7_SDIO_RESP1_OFFSET      0x0018
#define GD32H7_SDIO_RESP2_OFFSET      0x001c
#define GD32H7_SDIO_RESP3_OFFSET      0x0020
#define GD32H7_SDIO_DATATO_OFFSET     0x0024
#define GD32H7_SDIO_DATALEN_OFFSET    0x0028
#define GD32H7_SDIO_DATACTL_OFFSET    0x002c
#define GD32H7_SDIO_DATACNT_OFFSET    0x0030
#define GD32H7_SDIO_STAT_OFFSET       0x0034
#define GD32H7_SDIO_INTC_OFFSET       0x0038
#define GD32H7_SDIO_INTEN_OFFSET      0x003c
#define GD32H7_SDIO_FIFO_OFFSET       0x0080

/* Register addresses ---------------------------------------------------- */

#define GD32H7_SDIO_PWRCTL(s)         ((s) + GD32H7_SDIO_PWRCTL_OFFSET)
#define GD32H7_SDIO_CLKCTL(s)         ((s) + GD32H7_SDIO_CLKCTL_OFFSET)
#define GD32H7_SDIO_CMDAGMT(s)        ((s) + GD32H7_SDIO_CMDAGMT_OFFSET)
#define GD32H7_SDIO_CMDCTL(s)         ((s) + GD32H7_SDIO_CMDCTL_OFFSET)
#define GD32H7_SDIO_RSPCMDIDX(s)      ((s) + GD32H7_SDIO_RSPCMDIDX_OFFSET)
#define GD32H7_SDIO_RESP0(s)          ((s) + GD32H7_SDIO_RESP0_OFFSET)
#define GD32H7_SDIO_RESP1(s)          ((s) + GD32H7_SDIO_RESP1_OFFSET)
#define GD32H7_SDIO_RESP2(s)          ((s) + GD32H7_SDIO_RESP2_OFFSET)
#define GD32H7_SDIO_RESP3(s)          ((s) + GD32H7_SDIO_RESP3_OFFSET)
#define GD32H7_SDIO_DATATO(s)         ((s) + GD32H7_SDIO_DATATO_OFFSET)
#define GD32H7_SDIO_DATALEN(s)        ((s) + GD32H7_SDIO_DATALEN_OFFSET)
#define GD32H7_SDIO_DATACTL(s)        ((s) + GD32H7_SDIO_DATACTL_OFFSET)
#define GD32H7_SDIO_DATACNT(s)        ((s) + GD32H7_SDIO_DATACNT_OFFSET)
#define GD32H7_SDIO_STAT(s)           ((s) + GD32H7_SDIO_STAT_OFFSET)
#define GD32H7_SDIO_INTC(s)           ((s) + GD32H7_SDIO_INTC_OFFSET)
#define GD32H7_SDIO_INTEN(s)          ((s) + GD32H7_SDIO_INTEN_OFFSET)
#define GD32H7_SDIO_FIFO(s)           ((s) + GD32H7_SDIO_FIFO_OFFSET)

/* PWRCTL */

#define SDIO_PWRCTL_PWRVAL            (3 << 0)   /* Power on */

/* CLKCTL */

#define SDIO_CLKCTL_DIV_SHIFT         (0)        /* Divisor (value - 2) */
#define SDIO_CLKCTL_DIV_MASK          (0x3ff << SDIO_CLKCTL_DIV_SHIFT)
#define SDIO_CLKCTL_CLKEN             (1 << 10)  /* Clock enable */
#define SDIO_CLKCTL_CLKPWRSAV         (1 << 12)  /* Power saving */
#define SDIO_CLKCTL_BUSMODE_SHIFT     (14)       /* Bus width */
#define SDIO_CLKCTL_BUSMODE_MASK      (3 << SDIO_CLKCTL_BUSMODE_SHIFT)
#  define SDIO_BUSMODE_1BIT           (0 << SDIO_CLKCTL_BUSMODE_SHIFT)
#  define SDIO_BUSMODE_4BIT           (1 << SDIO_CLKCTL_BUSMODE_SHIFT)
#  define SDIO_BUSMODE_8BIT           (2 << SDIO_CLKCTL_BUSMODE_SHIFT)
#define SDIO_CLKCTL_CLKEDGE           (1 << 16)  /* Clock edge (0=rising) */
#define SDIO_CLKCTL_HWEN              (1 << 17)  /* HW flow control */

/* CMDCTL */

#define SDIO_CMDCTL_CMDIDX_SHIFT      (0)
#define SDIO_CMDCTL_CMDIDX_MASK       (0x3f << SDIO_CMDCTL_CMDIDX_SHIFT)
#define SDIO_CMDCTL_TREN              (1 << 6)   /* Data transfer enabled */
#define SDIO_CMDCTL_TRDIR             (1 << 7)   /* 0=host->card, 1=card->host */
#define SDIO_CMDCTL_CMDRESP_SHIFT     (8)        /* Response type */
#define SDIO_CMDCTL_CMDRESP_MASK      (3 << SDIO_CMDCTL_CMDRESP_SHIFT)
#  define SDIO_RESP_NONE              (0 << SDIO_CMDCTL_CMDRESP_SHIFT)
#  define SDIO_RESP_SHORT             (1 << SDIO_CMDCTL_CMDRESP_SHIFT)
#  define SDIO_RESP_LONG              (3 << SDIO_CMDCTL_CMDRESP_SHIFT)
#define SDIO_CMDCTL_INTWAIT           (1 << 10)
#define SDIO_CMDCTL_WAITDEND          (1 << 11)  /* Wait for data end */
#define SDIO_CMDCTL_CSMEN             (1 << 12)  /* Command state machine enable */
#define SDIO_CMDCTL_HOLD              (1 << 13)

/* DATACTL */

#define SDIO_DATACTL_DATAEN           (1 << 0)
#define SDIO_DATACTL_DATADIR          (1 << 1)   /* 0=host->card, 1=card->host */
#define SDIO_DATACTL_TRANSMOD_BLOCK   (0 << 2)
#define SDIO_DATACTL_BLKSZ_SHIFT      (4)        /* log2(block size) */
#define SDIO_DATACTL_BLKSZ_MASK       (0xf << SDIO_DATACTL_BLKSZ_SHIFT)
#define SDIO_DATACTL_DMAEN            (1 << 3)
#define SDIO_DATACTL_FIFOREST         (1 << 13)

/* STAT */

#define SDIO_STAT_CCRCERR             (1 << 0)
#define SDIO_STAT_DTCRCERR            (1 << 1)
#define SDIO_STAT_CMDTMOUT            (1 << 2)
#define SDIO_STAT_DTTMOUT             (1 << 3)
#define SDIO_STAT_TXURE               (1 << 4)
#define SDIO_STAT_RXORE               (1 << 5)
#define SDIO_STAT_CMDRECV             (1 << 6)   /* Response received */
#define SDIO_STAT_CMDSEND             (1 << 7)   /* Command sent */
#define SDIO_STAT_DTEND               (1 << 8)
#define SDIO_STAT_DTBLKEND            (1 << 10)
#define SDIO_STAT_TFH                 (1 << 14)  /* TX FIFO half empty */
#define SDIO_STAT_RFH                 (1 << 15)  /* RX FIFO half full */
#define SDIO_STAT_TFF                 (1 << 16)  /* TX FIFO full */
#define SDIO_STAT_RFF                 (1 << 17)  /* RX FIFO full */
#define SDIO_STAT_TFE                 (1 << 18)  /* TX FIFO empty */
#define SDIO_STAT_RFE                 (1 << 19)  /* RX FIFO empty */
#define SDIO_STAT_DAT0BSY             (1 << 20)

#define SDIO_STAT_CMD_ERRS            (SDIO_STAT_CCRCERR | \
                                       SDIO_STAT_CMDTMOUT)
#define SDIO_STAT_DATA_ERRS           (SDIO_STAT_DTCRCERR | \
                                       SDIO_STAT_DTTMOUT | \
                                       SDIO_STAT_TXURE | SDIO_STAT_RXORE)

/* INTC - write 1 to clear; the clear bits mirror the STAT bits */

#define SDIO_INTC_ALL                 0x01ffffff

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_SDIO_H */
