/****************************************************************************
 * arch/arm/src/gd32h7/hardware/gd32h7xx_usart.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_USART_H
#define __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_USART_H

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

/* USART instances --------------------------------------------------------
 *
 * USART0 and USART5 are clocked by CK_PCLK2 (APB2).
 * USART1, USART2, UART3, UART4, UART6 and UART7 are clocked by CK_PCLK1
 * (APB1) unless a different CK_USART source is selected in RCU_CFG1.
 */

#define GD32H7_USART0                 GD32H7_USART0_BASE
#define GD32H7_USART1                 GD32H7_USART1_BASE
#define GD32H7_USART2                 GD32H7_USART2_BASE
#define GD32H7_UART3                  GD32H7_UART3_BASE
#define GD32H7_UART4                  GD32H7_UART4_BASE
#define GD32H7_USART5                 GD32H7_USART5_BASE
#define GD32H7_UART6                  GD32H7_UART6_BASE
#define GD32H7_UART7                  GD32H7_UART7_BASE

/* Register offsets ------------------------------------------------------ */

#define GD32H7_USART_CTL0_OFFSET      0x0000  /* USART control register 0 */
#define GD32H7_USART_CTL1_OFFSET      0x0004  /* USART control register 1 */
#define GD32H7_USART_CTL2_OFFSET      0x0008  /* USART control register 2 */
#define GD32H7_USART_BAUD_OFFSET      0x000c  /* USART baud rate generator */
#define GD32H7_USART_GP_OFFSET        0x0010  /* USART prescaler and guard time */
#define GD32H7_USART_RT_OFFSET        0x0014  /* USART receiver timeout */
#define GD32H7_USART_CMD_OFFSET       0x0018  /* USART command register */
#define GD32H7_USART_STAT_OFFSET      0x001c  /* USART status register */
#define GD32H7_USART_INTC_OFFSET      0x0020  /* USART interrupt status clear */
#define GD32H7_USART_RDATA_OFFSET     0x0024  /* USART receive data register */
#define GD32H7_USART_TDATA_OFFSET     0x0028  /* USART transmit data register */
#define GD32H7_USART_CHC_OFFSET       0x00c0  /* USART coherence control register */
#define GD32H7_USART_FCS_OFFSET       0x00d0  /* USART FIFO control and status */

/* Data mask (TDATA/RDATA are 10 bits wide on the GD32H7xx) */

#define GD32H7_USART_DATA_MASK        0x01ff

/* USART_CTL0 bits -------------------------------------------------------- */

#define USART_CTL0_UEN                (1 << 0)     /* USART enable */
#define USART_CTL0_UESM               (1 << 1)     /* USART enable in deep-sleep */
#define USART_CTL0_REN                (1 << 2)     /* Receive enable */
#define USART_CTL0_TEN                (1 << 3)     /* Transmit enable */
#define USART_CTL0_IDLEIE             (1 << 4)     /* Idle line detected interrupt enable */
#define USART_CTL0_RBNEIE             (1 << 5)     /* Read data buffer not empty interrupt enable */
#define USART_CTL0_TCIE               (1 << 6)     /* Transmission complete interrupt enable */
#define USART_CTL0_TBEIE              (1 << 7)     /* Transmitter register empty interrupt enable */
#define USART_CTL0_PERRIE             (1 << 8)     /* Parity error interrupt enable */
#define USART_CTL0_PM                 (1 << 9)     /* Parity mode: 0=even, 1=odd */
#define USART_CTL0_PCEN               (1 << 10)    /* Parity control enable */
#define USART_CTL0_WM                 (1 << 11)    /* Wakeup method in mute mode */
#define USART_CTL0_WL0                (1 << 12)    /* Word length 0 */
#define USART_CTL0_MEN                (1 << 13)    /* Mute mode enable */
#define USART_CTL0_AMIE0              (1 << 14)    /* Address 0 character match interrupt */
#define USART_CTL0_OVSMOD             (1 << 15)    /* Oversample mode: 0=x16, 1=x8 */
#define USART_CTL0_DED                (0x1f << 16) /* Driver deassertion time */
#define USART_CTL0_DEA                (0x1f << 21) /* Driver assertion time */
#define USART_CTL0_RTIE               (1 << 26)    /* Receiver timeout interrupt enable */
#define USART_CTL0_EBIE               (1 << 27)    /* End of block interrupt enable */
#define USART_CTL0_WL1                (1 << 28)    /* Word length 1 */

#define USART_CTL0_WL_8BIT            (0)
#define USART_CTL0_WL_9BIT            (USART_CTL0_WL0)
#define USART_CTL0_WL_7BIT            (USART_CTL0_WL1)
#define USART_CTL0_WL_10BIT           (USART_CTL0_WL0 | USART_CTL0_WL1)

#define USART_CTL0_PM_NONE            (0)
#define USART_CTL0_PM_EVEN            (0)
#define USART_CTL0_PM_ODD             (USART_CTL0_PM)

/* USART_CTL1 bits -------------------------------------------------------- */

#define USART_CTL1_AMEN0              (1 << 0)     /* Address 0 match mode enable */
#define USART_CTL1_ADDM0              (1 << 4)     /* Address 0 detection mode */
#define USART_CTL1_LBLEN              (1 << 5)     /* LIN break frame length */
#define USART_CTL1_LBDIE              (1 << 6)     /* LIN break detection interrupt enable */
#define USART_CTL1_CLEN               (1 << 8)     /* Last bit clock pulse */
#define USART_CTL1_CPH                (1 << 9)     /* Clock phase */
#define USART_CTL1_CPL                (1 << 10)    /* Clock polarity */
#define USART_CTL1_CKEN               (1 << 11)    /* CK pin enable */
#define USART_CTL1_STB                (0x3 << 12)  /* Stop bits length */
#define USART_CTL1_LMEN               (1 << 14)    /* LIN mode enable */
#define USART_CTL1_STRP               (1 << 15)    /* Swap TX/RX pins */
#define USART_CTL1_RINV               (1 << 16)    /* RX pin level inversion */
#define USART_CTL1_TINV               (1 << 17)    /* TX pin level inversion */
#define USART_CTL1_DINV               (1 << 18)    /* Data bit level inversion */
#define USART_CTL1_MSBF               (1 << 19)    /* MSB first */
#define USART_CTL1_RTEN               (1 << 23)    /* Receiver timeout enable */
#define USART_CTL1_ADDR0              (0xff << 24) /* Address 0 of the USART terminal */

#define USART_CTL1_STB_1BIT           (0 << 12)
#define USART_CTL1_STB_0_5BIT         (1 << 12)
#define USART_CTL1_STB_2BIT           (2 << 12)
#define USART_CTL1_STB_1_5BIT         (3 << 12)

/* USART_CTL2 bits -------------------------------------------------------- */

#define USART_CTL2_ERRIE              (1 << 0)     /* Error interrupt enable */
#define USART_CTL2_IREN               (1 << 1)     /* IrDA mode enable */
#define USART_CTL2_IRLP               (1 << 2)     /* IrDA low-power */
#define USART_CTL2_HDEN               (1 << 3)     /* Half-duplex enable */
#define USART_CTL2_NKEN               (1 << 4)     /* NACK enable in smartcard mode */
#define USART_CTL2_SCEN               (1 << 5)     /* Smartcard mode enable */
#define USART_CTL2_DENR               (1 << 6)     /* DMA enable for reception */
#define USART_CTL2_DENT               (1 << 7)     /* DMA enable for transmission */
#define USART_CTL2_RTSEN              (1 << 8)     /* RTS enable */
#define USART_CTL2_CTSEN              (1 << 9)     /* CTS enable */
#define USART_CTL2_CTSIE              (1 << 10)    /* CTS interrupt enable */
#define USART_CTL2_OSB                (1 << 11)    /* One sample bit method */
#define USART_CTL2_OVRD               (1 << 12)    /* Overrun disable */
#define USART_CTL2_DDRE               (1 << 13)    /* Disable DMA on reception error */
#define USART_CTL2_DEM                (1 << 14)    /* Driver mode enable */
#define USART_CTL2_DEP                (1 << 15)    /* Driver polarity mode */
#define USART_CTL2_AMEN1              (1 << 16)    /* Address 1 match mode enable */
#define USART_CTL2_SCRTNUM            (0x7 << 17)  /* Smartcard auto-retry number */
#define USART_CTL2_WUM                (0x3 << 20)  /* Wakeup mode from deep-sleep */
#define USART_CTL2_WUIE               (1 << 22)    /* Wakeup from deep-sleep interrupt */
#define USART_CTL2_ADDM1              (1 << 23)    /* Address 1 detection mode */
#define USART_CTL2_ADDR1              (0xff << 24) /* Address 1 of the USART terminal */

/* USART_BAUD bits --------------------------------------------------------- */

#define USART_BAUD_FRADIV             (0xf << 0)   /* Bits 0-3: fraction of baud-rate divider */
#define USART_BAUD_FRADIV_MASK        (0xf << 0)
#define USART_BAUD_INTDIV             (0xfff << 4) /* Bits 4-15: integer of baud-rate divider */
#define USART_BAUD_INTDIV_MASK        (0xfff << 4)

/* USART_CMD bits ---------------------------------------------------------- */

#define USART_CMD_SBKCMD              (1 << 1)     /* Send break command */
#define USART_CMD_MMCMD               (1 << 2)     /* Mute mode command */
#define USART_CMD_RXFCMD              (1 << 3)     /* Receive data flush command */
#define USART_CMD_TXFCMD              (1 << 4)     /* Transmit data flush command */

/* USART_STAT bits --------------------------------------------------------- */

#define USART_STAT_PERR               (1 << 0)     /* Parity error flag */
#define USART_STAT_FERR               (1 << 1)     /* Frame error flag */
#define USART_STAT_NERR               (1 << 2)     /* Noise error flag */
#define USART_STAT_ORERR              (1 << 3)     /* Overrun error flag */
#define USART_STAT_IDLEF              (1 << 4)     /* Idle line detected flag */
#define USART_STAT_RBNE               (1 << 5)     /* Read data buffer not empty */
#define USART_STAT_TC                 (1 << 6)     /* Transmission complete */
#define USART_STAT_TBE                (1 << 7)     /* Transmit data register empty */
#define USART_STAT_LBDF               (1 << 8)     /* LIN break detected flag */
#define USART_STAT_CTSF               (1 << 9)     /* CTS change flag */
#define USART_STAT_CTS                (1 << 10)    /* CTS level */
#define USART_STAT_RTF                (1 << 11)    /* Receiver timeout flag */
#define USART_STAT_EBF                (1 << 12)    /* End of block flag */
#define USART_STAT_AMF1               (1 << 13)    /* Address 1 character match flag */
#define USART_STAT_BSY                (1 << 16)    /* Busy flag */
#define USART_STAT_AMF0               (1 << 17)    /* Address 0 character match flag */
#define USART_STAT_SBF                (1 << 18)    /* Send break flag */
#define USART_STAT_RWU                (1 << 19)    /* Receiver wakeup from mute mode */
#define USART_STAT_WUF                (1 << 20)    /* Wakeup from deep-sleep flag */
#define USART_STAT_TEA                (1 << 21)    /* Transmit enable acknowledge */
#define USART_STAT_REA                (1 << 22)    /* Receive enable acknowledge */

/* USART_INTC bits (error flags are cleared via INTC, not by writing STAT) -- */

#define USART_INTC_PEC                (1 << 0)     /* Clear parity error */
#define USART_INTC_FEC                (1 << 1)     /* Clear frame error */
#define USART_INTC_NEC                (1 << 2)     /* Clear noise detected */
#define USART_INTC_OREC               (1 << 3)     /* Clear overrun error */
#define USART_INTC_IDLEC              (1 << 4)     /* Clear idle line detected */
#define USART_INTC_TCC                (1 << 6)     /* Clear transmission complete */
#define USART_INTC_LBDC               (1 << 8)     /* Clear LIN break detected */
#define USART_INTC_CTSC               (1 << 9)     /* Clear CTS change */
#define USART_INTC_RTC                (1 << 11)    /* Clear receiver timeout */
#define USART_INTC_EBC                (1 << 12)    /* Clear end of timeout */

/* USART_STAT error mask: all error flags that are reported to the serial
 * layer and cleared through the INTC register.
 */

#define USART_STAT_EFLAG_MASK \
  (USART_STAT_PERR | USART_STAT_FERR | USART_STAT_NERR | USART_STAT_ORERR)

/* USART_FCS bits ---------------------------------------------------------- */

#define USART_FCS_RFT                 (1 << 4)     /* Receive FIFO threshold flag */
#define USART_FCS_TFT                 (1 << 5)     /* Transmit FIFO threshold flag */
#define USART_FCS_TFE                 (1 << 6)     /* Transmit FIFO empty flag */
#define USART_FCS_TFF                 (1 << 7)     /* Transmit FIFO full flag */
#define USART_FCS_FEN                 (1 << 8)     /* FIFO enable */
#define USART_FCS_RFFIE               (1 << 9)     /* Receive FIFO full interrupt enable */
#define USART_FCS_RFE                 (1 << 10)    /* Receive FIFO empty flag */
#define USART_FCS_RFF                 (1 << 11)    /* Receive FIFO full flag */
#define USART_FCS_RFCNT               (0x1f << 12) /* Receive FIFO counter RFCNT[4:0] */
#define USART_FCS_TFTCFG              (0x7 << 19)  /* Transmit FIFO threshold configuration */
#define USART_FCS_RFTCFG              (0x7 << 16)  /* Receive FIFO threshold configuration */

#endif /* __ARCH_ARM_SRC_GD32H7_HARDWARE_GD32H7XX_USART_H */
