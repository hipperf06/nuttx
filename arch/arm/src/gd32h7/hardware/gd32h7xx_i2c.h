/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h7xx_i2c.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_I2C_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_I2C_H

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

/* I2C instances (new-generation I2C with TIMING register) */

#define GD32H7_I2C0                   GD32H7_I2C0_BASE
#define GD32H7_I2C1                   GD32H7_I2C1_BASE
#define GD32H7_I2C2                   GD32H7_I2C2_BASE
#define GD32H7_I2C3                   GD32H7_I2C3_BASE

/* Register offsets ------------------------------------------------------ */

#define GD32H7_I2C_CTL0_OFFSET        0x0000  /* Control register 0 */
#define GD32H7_I2C_CTL1_OFFSET        0x0004  /* Control register 1 */
#define GD32H7_I2C_SADDR0_OFFSET      0x0008  /* Slave address register 0 */
#define GD32H7_I2C_SADDR1_OFFSET      0x000c  /* Slave address register 1 */
#define GD32H7_I2C_TIMING_OFFSET      0x0010  /* Timing register */
#define GD32H7_I2C_TIMEOUT_OFFSET     0x0014  /* Timeout register */
#define GD32H7_I2C_STAT_OFFSET        0x0018  /* Status register */
#define GD32H7_I2C_STATC_OFFSET       0x001c  /* Status clear register */
#define GD32H7_I2C_PEC_OFFSET         0x0020  /* PEC register */
#define GD32H7_I2C_RDATA_OFFSET       0x0024  /* Receive data register */
#define GD32H7_I2C_TDATA_OFFSET       0x0028  /* Transmit data register */

/* Register addresses ---------------------------------------------------- */

#define GD32H7_I2C_CTL0(i2cx)         ((i2cx) + GD32H7_I2C_CTL0_OFFSET)
#define GD32H7_I2C_CTL1(i2cx)         ((i2cx) + GD32H7_I2C_CTL1_OFFSET)
#define GD32H7_I2C_TIMING(i2cx)       ((i2cx) + GD32H7_I2C_TIMING_OFFSET)
#define GD32H7_I2C_STAT(i2cx)         ((i2cx) + GD32H7_I2C_STAT_OFFSET)
#define GD32H7_I2C_STATC(i2cx)        ((i2cx) + GD32H7_I2C_STATC_OFFSET)
#define GD32H7_I2C_RDATA(i2cx)        ((i2cx) + GD32H7_I2C_RDATA_OFFSET)
#define GD32H7_I2C_TDATA(i2cx)        ((i2cx) + GD32H7_I2C_TDATA_OFFSET)

/* I2C_CTL0 bits ---------------------------------------------------------- */

#define I2C_CTL0_I2CEN                (1 << 0)   /* I2C peripheral enable */
#define I2C_CTL0_TIE                  (1 << 1)   /* Transmit interrupt enable */
#define I2C_CTL0_RBNEIE               (1 << 2)   /* Receive interrupt enable */
#define I2C_CTL0_ADDMIE               (1 << 3)   /* Address match interrupt enable */
#define I2C_CTL0_NACKIE               (1 << 4)   /* NACK received interrupt enable */
#define I2C_CTL0_STPDETIE             (1 << 5)   /* Stop detection interrupt enable */
#define I2C_CTL0_TCIE                 (1 << 6)   /* Transfer complete interrupt enable */
#define I2C_CTL0_ERRIE                (1 << 7)   /* Error interrupt enable */
#define I2C_CTL0_DNF_SHIFT            (8)        /* Bits 8-11: digital noise filter */
#define I2C_CTL0_DNF_MASK             (0xf << I2C_CTL0_DNF_SHIFT)
#define I2C_CTL0_DNF(n)               ((n) << I2C_CTL0_DNF_SHIFT)
#define I2C_CTL0_ANOFF                (1 << 12)  /* Analog noise filter off */
#define I2C_CTL0_DENT                 (1 << 14)  /* DMA enable for transmission */
#define I2C_CTL0_DENR                 (1 << 15)  /* DMA enable for reception */
#define I2C_CTL0_GCEN                 (1 << 19)  /* General call response enable */

/* I2C_CTL1 bits ---------------------------------------------------------- */

#define I2C_CTL1_SADDRESS_SHIFT       (0)        /* Bits 0-9: slave address to send */
#define I2C_CTL1_SADDRESS_MASK        (0x3ff << I2C_CTL1_SADDRESS_SHIFT)
#define I2C_CTL1_SADDRESS(n)          ((n) << I2C_CTL1_SADDRESS_SHIFT)
#define I2C_CTL1_TRDIR                (1 << 10)  /* Transfer direction in master mode */
#define I2C_CTL1_ADD10EN              (1 << 11)  /* 10-bit addressing mode enable */
#define I2C_CTL1_HEAD10R              (1 << 12)  /* 10-bit address header read only */
#define I2C_CTL1_START                (1 << 13)  /* Generate a START condition */
#define I2C_CTL1_STOP                 (1 << 14)  /* Generate a STOP condition */
#define I2C_CTL1_NACKEN               (1 << 15)  /* Generate NACK in slave mode */
#define I2C_CTL1_BYTENUM_SHIFT        (16)       /* Bits 16-23: number of bytes */
#define I2C_CTL1_BYTENUM_MASK         (0xff << I2C_CTL1_BYTENUM_SHIFT)
#define I2C_CTL1_BYTENUM(n)           ((n) << I2C_CTL1_BYTENUM_SHIFT)
#define I2C_CTL1_RELOAD               (1 << 24)  /* Reload mode enable */
#define I2C_CTL1_AUTOEND              (1 << 25)  /* Automatic end mode */

/* I2C_TIMING bits --------------------------------------------------------- */

#define I2C_TIMING_SCLL_SHIFT         (0)        /* Bits 0-7: SCL low period */
#define I2C_TIMING_SCLL(n)            ((n) << I2C_TIMING_SCLL_SHIFT)
#define I2C_TIMING_SCLH_SHIFT         (8)        /* Bits 8-15: SCL high period */
#define I2C_TIMING_SCLH(n)            ((n) << I2C_TIMING_SCLH_SHIFT)
#define I2C_TIMING_SDADELY_SHIFT      (16)       /* Bits 16-19: data hold time */
#define I2C_TIMING_SDADELY(n)         ((n) << I2C_TIMING_SDADELY_SHIFT)
#define I2C_TIMING_SCLDELY_SHIFT      (20)       /* Bits 20-23: data setup time */
#define I2C_TIMING_SCLDELY(n)         ((n) << I2C_TIMING_SCLDELY_SHIFT)
#define I2C_TIMING_PSC_SHIFT          (28)       /* Bits 28-31: timing prescaler */
#define I2C_TIMING_PSC(n)             ((n) << I2C_TIMING_PSC_SHIFT)

/* I2C_STAT bits ----------------------------------------------------------- */

#define I2C_STAT_TBE                  (1 << 0)   /* TDATA empty during transmit */
#define I2C_STAT_TI                   (1 << 1)   /* Transmit interrupt */
#define I2C_STAT_RBNE                 (1 << 2)   /* RDATA not empty during receive */
#define I2C_STAT_ADDSEND              (1 << 3)   /* Address received matches (slave) */
#define I2C_STAT_NACK                 (1 << 4)   /* Not acknowledge flag */
#define I2C_STAT_STPDET               (1 << 5)   /* STOP condition detected (slave) */
#define I2C_STAT_TC                   (1 << 6)   /* Transfer complete (master) */
#define I2C_STAT_TCR                  (1 << 7)   /* Transfer complete reload */
#define I2C_STAT_BERR                 (1 << 8)   /* Bus error */
#define I2C_STAT_LOSTARB              (1 << 9)   /* Arbitration lost */
#define I2C_STAT_OUERR                (1 << 10)  /* Overrun/underrun error (slave) */
#define I2C_STAT_PECERR               (1 << 11)  /* PEC error */
#define I2C_STAT_TIMEOUT              (1 << 12)  /* Timeout flag */
#define I2C_STAT_SMBALT               (1 << 13)  /* SMBus alert */
#define I2C_STAT_I2CBSY               (1 << 15)  /* Busy flag */

/* I2C_STATC bits (write 1 to clear) --------------------------------------- */

#define I2C_STATC_ADDSENDC            (1 << 3)
#define I2C_STATC_NACKC               (1 << 4)
#define I2C_STATC_STPDETC             (1 << 5)
#define I2C_STATC_BERRC               (1 << 8)
#define I2C_STATC_LOSTARBC            (1 << 9)
#define I2C_STATC_OUERRC              (1 << 10)
#define I2C_STATC_PECERRC             (1 << 11)
#define I2C_STATC_TIMEOUTC            (1 << 12)
#define I2C_STATC_SMBALTC             (1 << 13)

/* I2C error status mask */

#define I2C_STAT_ERR_MASK \
  (I2C_STAT_BERR | I2C_STAT_LOSTARB | I2C_STAT_OUERR | \
   I2C_STAT_PECERR | I2C_STAT_TIMEOUT)

/* I2C_RDATA / I2C_TDATA: bits 0-7 data value */

#define I2C_RDATA_MASK                0xff
#define I2C_TDATA_MASK                0xff

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_I2C_H */
