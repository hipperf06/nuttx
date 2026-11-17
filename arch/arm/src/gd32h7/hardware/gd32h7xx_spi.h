/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h7xx_spi.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_SPI_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_SPI_H

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

/* SPI instances (the new-generation SPI in the GD32H7xx) */

#define GD32H7_SPI0                   GD32H7_SPI0_BASE
#define GD32H7_SPI1                   GD32H7_SPI1_BASE
#define GD32H7_SPI2                   GD32H7_SPI2_BASE
#define GD32H7_SPI3                   GD32H7_SPI3_BASE
#define GD32H7_SPI4                   GD32H7_SPI4_BASE
#define GD32H7_SPI5                   GD32H7_SPI5_BASE

/* Register offsets ------------------------------------------------------ */

#define GD32H7_SPI_CTL0_OFFSET        0x0000  /* Control register 0 */
#define GD32H7_SPI_CTL1_OFFSET        0x0004  /* Control register 1 */
#define GD32H7_SPI_CFG0_OFFSET        0x0008  /* Configuration register 0 */
#define GD32H7_SPI_CFG1_OFFSET        0x000c  /* Configuration register 1 */
#define GD32H7_SPI_INT_OFFSET         0x0010  /* Interrupt register */
#define GD32H7_SPI_STAT_OFFSET        0x0014  /* Status register */
#define GD32H7_SPI_STATC_OFFSET       0x0018  /* Status clear register */
#define GD32H7_SPI_TDATA_OFFSET       0x0020  /* Data transfer register */
#define GD32H7_SPI_RDATA_OFFSET       0x0030  /* Data receive register */
#define GD32H7_SPI_CRCPOLY_OFFSET     0x0040  /* CRC polynomial register */
#define GD32H7_SPI_TCRC_OFFSET        0x0044  /* TX CRC register */
#define GD32H7_SPI_RCRC_OFFSET        0x0048  /* RX CRC register */
#define GD32H7_SPI_URDATA_OFFSET      0x004c  /* Underrun data register */
#define GD32H7_SPI_I2SCTL_OFFSET      0x0050  /* I2S control register */
#define GD32H7_SPI_QCTL_OFFSET        0x0080  /* Quad-SPI mode control */
#define GD32H7_SPI_RXDLYCK_OFFSET     0x00fc  /* Receive clock delay */

/* Register addresses ---------------------------------------------------- */

#define GD32H7_SPI_CTL0(spix)         ((spix) + GD32H7_SPI_CTL0_OFFSET)
#define GD32H7_SPI_CTL1(spix)         ((spix) + GD32H7_SPI_CTL1_OFFSET)
#define GD32H7_SPI_CFG0(spix)         ((spix) + GD32H7_SPI_CFG0_OFFSET)
#define GD32H7_SPI_CFG1(spix)         ((spix) + GD32H7_SPI_CFG1_OFFSET)
#define GD32H7_SPI_INT(spix)          ((spix) + GD32H7_SPI_INT_OFFSET)
#define GD32H7_SPI_STAT(spix)         ((spix) + GD32H7_SPI_STAT_OFFSET)
#define GD32H7_SPI_STATC(spix)        ((spix) + GD32H7_SPI_STATC_OFFSET)
#define GD32H7_SPI_TDATA(spix)        ((spix) + GD32H7_SPI_TDATA_OFFSET)
#define GD32H7_SPI_RDATA(spix)        ((spix) + GD32H7_SPI_RDATA_OFFSET)

/* SPI_CTL0 bits ---------------------------------------------------------- */

#define SPI_CTL0_SPIEN                (1 << 0)   /* SPI enable */
#define SPI_CTL0_MASP                 (1 << 8)   /* Auto suspend in receive mode */
#define SPI_CTL0_MSTART               (1 << 9)   /* Master start transfer */
#define SPI_CTL0_MSPDR                (1 << 10)  /* Suspend request in master mode */

/* SPI_CFG0 bits ---------------------------------------------------------- */

#define SPI_CFG0_DZ_SHIFT             (0)        /* Bits 0-4: data size */
#define SPI_CFG0_DZ_MASK              (0x1f << SPI_CFG0_DZ_SHIFT)
#define SPI_CFG0_DZ(n)                ((n) << SPI_CFG0_DZ_SHIFT)
#  define SPI_DATASIZE_8BIT           SPI_CFG0_DZ(7)
#  define SPI_DATASIZE_16BIT          SPI_CFG0_DZ(15)
#  define SPI_DATASIZE_32BIT          SPI_CFG0_DZ(31)

#define SPI_CFG0_FIFOLVL_SHIFT        (5)        /* Bits 5-8: FIFO threshold */
#define SPI_CFG0_FIFOLVL_MASK         (0xf << SPI_CFG0_FIFOLVL_SHIFT)
#define SPI_CFG0_FIFOLVL(n)           ((n) << SPI_CFG0_FIFOLVL_SHIFT)

#define SPI_CFG0_DMAREN               (1 << 14)  /* Receive buffer DMA enable */
#define SPI_CFG0_DMATEN               (1 << 15)  /* Transmit buffer DMA enable */
#define SPI_CFG0_CRCEN                (1 << 22)  /* CRC calculation enable */
#define SPI_CFG0_BYTEN                (1 << 23)  /* Byte access enable */
#define SPI_CFG0_WORDEN               (1 << 24)  /* Word access enable */

#define SPI_CFG0_PSC_SHIFT            (28)       /* Bits 28-30: master clock prescaler */
#define SPI_CFG0_PSC_MASK             (7 << SPI_CFG0_PSC_SHIFT)
#define SPI_CFG0_PSC(n)               ((n) << SPI_CFG0_PSC_SHIFT)
#  define SPI_PSC_2                   SPI_CFG0_PSC(0)
#  define SPI_PSC_4                   SPI_CFG0_PSC(1)
#  define SPI_PSC_8                   SPI_CFG0_PSC(2)
#  define SPI_PSC_16                  SPI_CFG0_PSC(3)
#  define SPI_PSC_32                  SPI_CFG0_PSC(4)
#  define SPI_PSC_64                  SPI_CFG0_PSC(5)
#  define SPI_PSC_128                 SPI_CFG0_PSC(6)
#  define SPI_PSC_256                 SPI_CFG0_PSC(7)

/* SPI_CFG1 bits ---------------------------------------------------------- */

#define SPI_CFG1_MSSD_SHIFT           (0)        /* Bits 0-3: NSS idleness delay */
#define SPI_CFG1_MDFD_SHIFT           (4)        /* Bits 4-7: master data frame delay */
#define SPI_CFG1_SWPMIO               (1 << 15)  /* MOSI/MISO pin swap */
#define SPI_CFG1_RO                   (1 << 16)  /* Receive only */
#define SPI_CFG1_BDOEN                (1 << 17)  /* Bidirectional transmit output enable */
#define SPI_CFG1_BDEN                 (1 << 18)  /* Bidirectional enable */
#define SPI_CFG1_TMOD                 (1 << 21)  /* TI mode enable */
#define SPI_CFG1_MSTMOD               (1 << 22)  /* Master mode enable */
#define SPI_CFG1_LF                   (1 << 23)  /* LSB first mode */
#define SPI_CFG1_CKPH                 (1 << 24)  /* Clock phase selection */
#define SPI_CFG1_CKPL                 (1 << 25)  /* Clock polarity selection */
#define SPI_CFG1_NSSIM                (1 << 26)  /* NSS input signal manage mode (software) */
#define SPI_CFG1_NSSIOPL              (1 << 28)  /* NSS pin polarity selection */
#define SPI_CFG1_NSSDRV               (1 << 29)  /* NSS pin output enable in master mode */
#define SPI_CFG1_NSSCTL               (1 << 30)  /* NSS pin output control */

/* SPI_INT bits ----------------------------------------------------------- */

#define SPI_INT_RPIE                  (1 << 0)   /* RP interrupt enable */
#define SPI_INT_TPIE                  (1 << 1)   /* TP interrupt enable */
#define SPI_INT_ESTCIE                (1 << 3)   /* End of transfer interrupt enable */
#define SPI_INT_TXUREIE               (1 << 5)   /* Underrun error interrupt enable */
#define SPI_INT_RXOREIE               (1 << 6)   /* Overrun error interrupt enable */
#define SPI_INT_CRCERIE               (1 << 7)   /* CRC error interrupt enable */
#define SPI_INT_CONFEIE               (1 << 9)   /* Configuration error interrupt enable */

/* SPI_STAT bits ---------------------------------------------------------- */

#define SPI_STAT_RP                   (1 << 0)   /* RxFIFO packet space available */
#define SPI_STAT_TP                   (1 << 1)   /* TxFIFO packet space available */
#define SPI_STAT_DP                   (1 << 2)   /* Duplex packet */
#define SPI_STAT_ET                   (1 << 3)   /* End of transmission/reception */
#define SPI_STAT_TXF                  (1 << 4)   /* TxFIFO transmission filled */
#define SPI_STAT_TXURERR              (1 << 5)   /* Transmission underrun error */
#define SPI_STAT_RXORERR              (1 << 6)   /* Reception overrun error */
#define SPI_STAT_CRCERR               (1 << 7)   /* CRC error */
#define SPI_STAT_FERR                 (1 << 8)   /* TI format error */
#define SPI_STAT_CONFERR              (1 << 9)   /* Configuration error */
#define SPI_STAT_TC                   (1 << 12)  /* TxFIFO transmission complete */

/* SPI_STATC bits (write 1 to clear) --------------------------------------- */

#define SPI_STATC_ETC                 (1 << 3)
#define SPI_STATC_TXFC                (1 << 4)
#define SPI_STATC_TXURERRC            (1 << 5)
#define SPI_STATC_RXORERRC            (1 << 6)
#define SPI_STATC_CRCERRC             (1 << 7)
#define SPI_STATC_FERRC               (1 << 8)
#define SPI_STATC_CONFERRC            (1 << 9)

/* SPI error status mask (all error flags) */

#define SPI_STAT_ERR_MASK \
  (SPI_STAT_TXURERR | SPI_STAT_RXORERR | SPI_STAT_CRCERR | \
   SPI_STAT_FERR | SPI_STAT_CONFERR)

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_SPI_H */
