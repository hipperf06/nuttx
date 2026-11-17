/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_lowputc.c
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>

#include <arch/board/board.h>

#include "arm_internal.h"

#include "chip.h"

#include "gd32h7xx.h"
#include "gd32h7xx_rcu.h"
#include "gd32h7xx_gpio.h"
#include "gd32h7xx_usart.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Is there a serial console? */

#if defined(CONFIG_USART0_SERIAL_CONSOLE) || defined(CONFIG_USART1_SERIAL_CONSOLE) || \
    defined(CONFIG_USART2_SERIAL_CONSOLE) || defined(CONFIG_UART3_SERIAL_CONSOLE) || \
    defined(CONFIG_UART4_SERIAL_CONSOLE) || defined(CONFIG_USART5_SERIAL_CONSOLE) || \
    defined(CONFIG_UART6_SERIAL_CONSOLE) || defined(CONFIG_UART7_SERIAL_CONSOLE)
#  define HAVE_CONSOLE 1
#else
#  undef HAVE_CONSOLE
#endif

/* Is there a serial driver? */

#if defined(CONFIG_GD32H7_USART0_SERIALDRIVER) || defined(CONFIG_GD32H7_USART1_SERIALDRIVER) || \
    defined(CONFIG_GD32H7_USART2_SERIALDRIVER) || defined(CONFIG_GD32H7_UART3_SERIALDRIVER) || \
    defined(CONFIG_GD32H7_UART4_SERIALDRIVER) || defined(CONFIG_GD32H7_USART5_SERIALDRIVER) || \
    defined(CONFIG_GD32H7_UART6_SERIALDRIVER) || defined(CONFIG_GD32H7_UART7_SERIALDRIVER)
#  define HAVE_SERIALDRIVER 1
#else
#  undef HAVE_SERIALDRIVER
#endif

/* Select USART parameters for the selected console */

#ifdef HAVE_CONSOLE
#  if defined(CONFIG_USART0_SERIAL_CONSOLE)
#    define GD32_CONSOLE_BASE          GD32H7_USART0
#    define GD32_CONSOLE_APBEN         RCU_USART0_EN
#    define GD32_CONSOLE_CLOCK         GD32_PCLK2_FREQUENCY
#    define GD32_CONSOLE_BAUD          CONFIG_USART0_BAUD
#    define GD32_CONSOLE_PARITY        CONFIG_USART0_PARITY
#    define GD32_CONSOLE_NBITS         CONFIG_USART0_BITS
#    define GD32_CONSOLE_2STOP         CONFIG_USART0_2STOP
#    define GD32_CONSOLE_TX            GPIO_USART0_TX
#    define GD32_CONSOLE_RX            GPIO_USART0_RX
#  elif defined(CONFIG_USART1_SERIAL_CONSOLE)
#    define GD32_CONSOLE_BASE          GD32H7_USART1
#    define GD32_CONSOLE_APBEN         RCU_USART1_EN
#    define GD32_CONSOLE_CLOCK         GD32_PCLK1_FREQUENCY
#    define GD32_CONSOLE_BAUD          CONFIG_USART1_BAUD
#    define GD32_CONSOLE_PARITY        CONFIG_USART1_PARITY
#    define GD32_CONSOLE_NBITS         CONFIG_USART1_BITS
#    define GD32_CONSOLE_2STOP         CONFIG_USART1_2STOP
#    define GD32_CONSOLE_TX            GPIO_USART1_TX
#    define GD32_CONSOLE_RX            GPIO_USART1_RX
#  elif defined(CONFIG_USART2_SERIAL_CONSOLE)
#    define GD32_CONSOLE_BASE          GD32H7_USART2
#    define GD32_CONSOLE_APBEN         RCU_USART2_EN
#    define GD32_CONSOLE_CLOCK         GD32_PCLK1_FREQUENCY
#    define GD32_CONSOLE_BAUD          CONFIG_USART2_BAUD
#    define GD32_CONSOLE_PARITY        CONFIG_USART2_PARITY
#    define GD32_CONSOLE_NBITS         CONFIG_USART2_BITS
#    define GD32_CONSOLE_2STOP         CONFIG_USART2_2STOP
#    define GD32_CONSOLE_TX            GPIO_USART2_TX
#    define GD32_CONSOLE_RX            GPIO_USART2_RX
#  elif defined(CONFIG_UART3_SERIAL_CONSOLE)
#    define GD32_CONSOLE_BASE          GD32H7_UART3
#    define GD32_CONSOLE_APBEN         RCU_UART3_EN
#    define GD32_CONSOLE_CLOCK         GD32_PCLK1_FREQUENCY
#    define GD32_CONSOLE_BAUD          CONFIG_UART3_BAUD
#    define GD32_CONSOLE_PARITY        CONFIG_UART3_PARITY
#    define GD32_CONSOLE_NBITS         CONFIG_UART3_BITS
#    define GD32_CONSOLE_2STOP         CONFIG_UART3_2STOP
#    define GD32_CONSOLE_TX            GPIO_UART3_TX
#    define GD32_CONSOLE_RX            GPIO_UART3_RX
#  elif defined(CONFIG_UART4_SERIAL_CONSOLE)
#    define GD32_CONSOLE_BASE          GD32H7_UART4
#    define GD32_CONSOLE_APBEN         RCU_UART4_EN
#    define GD32_CONSOLE_CLOCK         GD32_PCLK1_FREQUENCY
#    define GD32_CONSOLE_BAUD          CONFIG_UART4_BAUD
#    define GD32_CONSOLE_PARITY        CONFIG_UART4_PARITY
#    define GD32_CONSOLE_NBITS         CONFIG_UART4_BITS
#    define GD32_CONSOLE_2STOP         CONFIG_UART4_2STOP
#    define GD32_CONSOLE_TX            GPIO_UART4_TX
#    define GD32_CONSOLE_RX            GPIO_UART4_RX
#  elif defined(CONFIG_USART5_SERIAL_CONSOLE)
#    define GD32_CONSOLE_BASE          GD32H7_USART5
#    define GD32_CONSOLE_APBEN         RCU_USART5_EN
#    define GD32_CONSOLE_CLOCK         GD32_PCLK2_FREQUENCY
#    define GD32_CONSOLE_BAUD          CONFIG_USART5_BAUD
#    define GD32_CONSOLE_PARITY        CONFIG_USART5_PARITY
#    define GD32_CONSOLE_NBITS         CONFIG_USART5_BITS
#    define GD32_CONSOLE_2STOP         CONFIG_USART5_2STOP
#    define GD32_CONSOLE_TX            GPIO_USART5_TX
#    define GD32_CONSOLE_RX            GPIO_USART5_RX
#  elif defined(CONFIG_UART6_SERIAL_CONSOLE)
#    define GD32_CONSOLE_BASE          GD32H7_UART6
#    define GD32_CONSOLE_APBEN         RCU_UART6_EN
#    define GD32_CONSOLE_CLOCK         GD32_PCLK1_FREQUENCY
#    define GD32_CONSOLE_BAUD          CONFIG_UART6_BAUD
#    define GD32_CONSOLE_PARITY        CONFIG_UART6_PARITY
#    define GD32_CONSOLE_NBITS         CONFIG_UART6_BITS
#    define GD32_CONSOLE_2STOP         CONFIG_UART6_2STOP
#    define GD32_CONSOLE_TX            GPIO_UART6_TX
#    define GD32_CONSOLE_RX            GPIO_UART6_RX
#  elif defined(CONFIG_UART7_SERIAL_CONSOLE)
#    define GD32_CONSOLE_BASE          GD32H7_UART7
#    define GD32_CONSOLE_APBEN         RCU_UART7_EN
#    define GD32_CONSOLE_CLOCK         GD32_PCLK1_FREQUENCY
#    define GD32_CONSOLE_BAUD          CONFIG_UART7_BAUD
#    define GD32_CONSOLE_PARITY        CONFIG_UART7_PARITY
#    define GD32_CONSOLE_NBITS         CONFIG_UART7_BITS
#    define GD32_CONSOLE_2STOP         CONFIG_UART7_2STOP
#    define GD32_CONSOLE_TX            GPIO_UART7_TX
#    define GD32_CONSOLE_RX            GPIO_UART7_RX
#  endif

/* CTL0 settings */

#  if GD32_CONSOLE_NBITS == 9
#    define USART_CTL0_WL_VALUE      USART_CTL0_WL_9BIT
#  elif GD32_CONSOLE_NBITS == 7
#    define USART_CTL0_WL_VALUE      USART_CTL0_WL_7BIT
#  else
#    define USART_CTL0_WL_VALUE      USART_CTL0_WL_8BIT
#  endif

#  if GD32_CONSOLE_PARITY == 1
#    define USART_CTL0_PARITY_VALUE  USART_CTL0_PM_ODD
#  elif GD32_CONSOLE_PARITY == 2
#    define USART_CTL0_PARITY_VALUE  USART_CTL0_PM_EVEN
#  else
#    define USART_CTL0_PARITY_VALUE  USART_CTL0_PM_NONE
#  endif

/* CTL1 settings */

#  if GD32_CONSOLE_2STOP != 0
#    define USART_CTL1_STB_VALUE     USART_CTL1_STB_2BIT
#  else
#    define USART_CTL1_STB_VALUE     USART_CTL1_STB_1BIT
#  endif

#endif /* HAVE_CONSOLE */

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: arm_lowputc
 *
 * Description:
 *   Output one byte on the serial console
 *
 ****************************************************************************/

void arm_lowputc(char ch)
{
#ifdef HAVE_CONSOLE
  /* Wait until the transmit data register is empty */

  while ((getreg32(GD32_CONSOLE_BASE + GD32H7_USART_STAT_OFFSET) &
          USART_STAT_TBE) == 0);

  /* Then send the character */

  putreg32((uint32_t)(ch & GD32H7_USART_DATA_MASK),
           GD32_CONSOLE_BASE + GD32H7_USART_TDATA_OFFSET);
#endif
}

/****************************************************************************
 * Name: gd32_lowsetup
 *
 * Description:
 *   This performs basic initialization of the USART used for the serial
 *   console.  Its purpose is to get the console output available as soon
 *   as possible.
 *
 ****************************************************************************/

void gd32_lowsetup(void)
{
#if defined(HAVE_SERIALDRIVER)
#if defined(HAVE_CONSOLE) && !defined(CONFIG_SUPPRESS_UART_CONFIG)
  uint32_t regval;
  uint32_t udiv;
  uint32_t intdiv;
  uint32_t fradiv;

  /* Enable USART APB1/2 clock */

  gd32_rcu_periph_clock_enable(GD32_CONSOLE_APBEN);
#endif

#if defined(HAVE_CONSOLE)

  /* Enable and configure the console TX, RX pins. */

#ifdef GD32_CONSOLE_TX
  gd32_configgpio(GD32_CONSOLE_TX);
#endif
#ifdef GD32_CONSOLE_RX
  gd32_configgpio(GD32_CONSOLE_RX);
#endif

#if !defined(CONFIG_SUPPRESS_UART_CONFIG)

  /* Reset USART */

  gd32_usart_reset(GD32_CONSOLE_BASE);

  /* Enable USART clock */

  gd32_usart_clock_enable(GD32_CONSOLE_BASE);

  /* Disable the USART before to configure it. */

  regval  = getreg32(GD32_CONSOLE_BASE + GD32H7_USART_CTL0_OFFSET);
  regval &= ~USART_CTL0_UEN;
  putreg32(regval, GD32_CONSOLE_BASE + GD32H7_USART_CTL0_OFFSET);

  /* Configure CTL0 */

  regval  = getreg32(GD32_CONSOLE_BASE + GD32H7_USART_CTL0_OFFSET);
  regval |= (USART_CTL0_WL_VALUE | USART_CTL0_PARITY_VALUE);
  putreg32(regval, GD32_CONSOLE_BASE + GD32H7_USART_CTL0_OFFSET);

  /* Configure CTL1 */

  regval  = getreg32(GD32_CONSOLE_BASE + GD32H7_USART_CTL1_OFFSET);
  regval |= USART_CTL1_STB_VALUE;
  putreg32(regval, GD32_CONSOLE_BASE + GD32H7_USART_CTL1_OFFSET);

  /* Configure USART baud rate value */

  regval = getreg32(GD32_CONSOLE_BASE + GD32H7_USART_CTL0_OFFSET);

  if ((regval & USART_CTL0_OVSMOD) == USART_CTL0_OVSMOD)
    {
      /* When oversampling by 8, configure the value of USART_BAUD */

      udiv = ((GD32_CONSOLE_CLOCK * 2) + GD32_CONSOLE_BAUD / 2) /
              GD32_CONSOLE_BAUD;
      intdiv = udiv & 0xfff0;
      fradiv = (udiv >> 1) & 0x7;
    }
  else
    {
      /* When oversampling by 16, configure the value of USART_BAUD */

      udiv = (GD32_CONSOLE_CLOCK + GD32_CONSOLE_BAUD / 2) /
              GD32_CONSOLE_BAUD;
      intdiv = udiv & 0xfff0;
      fradiv = udiv & 0xf;
    }

  regval = ((USART_BAUD_FRADIV_MASK | USART_BAUD_INTDIV_MASK) &
            (intdiv | fradiv));
  putreg32(regval, GD32_CONSOLE_BASE + GD32H7_USART_BAUD_OFFSET);

  /* Enable Rx, Tx, and the USART */

  regval = getreg32(GD32_CONSOLE_BASE + GD32H7_USART_CTL0_OFFSET);
  regval |= (USART_CTL0_UEN | USART_CTL0_TEN | USART_CTL0_REN);
  putreg32(regval, GD32_CONSOLE_BASE + GD32H7_USART_CTL0_OFFSET);

#endif /* !CONFIG_SUPPRESS_UART_CONFIG */
#endif /* HAVE_CONSOLE */
#endif /* HAVE_SERIALDRIVER */
}

/****************************************************************************
 * Name: gd32_usart_reset
 *
 * Description:
 *   Reset the USART.
 *
 ****************************************************************************/

void gd32_usart_reset(uint32_t usartbase)
{
  uint32_t rcu_rst;

  /* Determine which USART to configure */

  switch (usartbase)
    {
      default:
        return;
#ifdef CONFIG_GD32H7_USART0_SERIALDRIVER
      case GD32H7_USART0:
        rcu_rst = RCU_USART0_RST;
        break;
#endif
#ifdef CONFIG_GD32H7_USART1_SERIALDRIVER
      case GD32H7_USART1:
        rcu_rst = RCU_USART1_RST;
        break;
#endif
#ifdef CONFIG_GD32H7_USART2_SERIALDRIVER
      case GD32H7_USART2:
        rcu_rst = RCU_USART2_RST;
        break;
#endif
#ifdef CONFIG_GD32H7_UART3_SERIALDRIVER
      case GD32H7_UART3:
        rcu_rst = RCU_UART3_RST;
        break;
#endif
#ifdef CONFIG_GD32H7_UART4_SERIALDRIVER
      case GD32H7_UART4:
        rcu_rst = RCU_UART4_RST;
        break;
#endif
#ifdef CONFIG_GD32H7_USART5_SERIALDRIVER
      case GD32H7_USART5:
        rcu_rst = RCU_USART5_RST;
        break;
#endif
#ifdef CONFIG_GD32H7_UART6_SERIALDRIVER
      case GD32H7_UART6:
        rcu_rst = RCU_UART6_RST;
        break;
#endif
#ifdef CONFIG_GD32H7_UART7_SERIALDRIVER
      case GD32H7_UART7:
        rcu_rst = RCU_UART7_RST;
        break;
#endif
    }

  /* Enable APB1/2 reset for USART */

  gd32_rcu_periph_clock_enable(rcu_rst);

  /* Disable APB1/2 reset for USART */

  gd32_rcu_periph_clock_disable(rcu_rst);
}

/****************************************************************************
 * Name: gd32_usart_clock_enable
 *
 * Description:
 *   Enable USART clock
 *
 ****************************************************************************/

void gd32_usart_clock_enable(uint32_t usartbase)
{
  uint32_t rcu_en;

  /* Determine which USART to configure */

  switch (usartbase)
    {
      default:
        return;
#ifdef CONFIG_GD32H7_USART0_SERIALDRIVER
      case GD32H7_USART0:
        rcu_en = RCU_USART0_EN;
        break;
#endif
#ifdef CONFIG_GD32H7_USART1_SERIALDRIVER
      case GD32H7_USART1:
        rcu_en = RCU_USART1_EN;
        break;
#endif
#ifdef CONFIG_GD32H7_USART2_SERIALDRIVER
      case GD32H7_USART2:
        rcu_en = RCU_USART2_EN;
        break;
#endif
#ifdef CONFIG_GD32H7_UART3_SERIALDRIVER
      case GD32H7_UART3:
        rcu_en = RCU_UART3_EN;
        break;
#endif
#ifdef CONFIG_GD32H7_UART4_SERIALDRIVER
      case GD32H7_UART4:
        rcu_en = RCU_UART4_EN;
        break;
#endif
#ifdef CONFIG_GD32H7_USART5_SERIALDRIVER
      case GD32H7_USART5:
        rcu_en = RCU_USART5_EN;
        break;
#endif
#ifdef CONFIG_GD32H7_UART6_SERIALDRIVER
      case GD32H7_UART6:
        rcu_en = RCU_UART6_EN;
        break;
#endif
#ifdef CONFIG_GD32H7_UART7_SERIALDRIVER
      case GD32H7_UART7:
        rcu_en = RCU_UART7_EN;
        break;
#endif
    }

  /* Enable APB1/2 clock for USART */

  gd32_rcu_periph_clock_enable(rcu_en);
}

/****************************************************************************
 * Name: gd32_usart_clock_disable
 *
 * Description:
 *   Disable USART clock
 *
 ****************************************************************************/

void gd32_usart_clock_disable(uint32_t usartbase)
{
  uint32_t rcu_en;

  /* Determine which USART to configure */

  switch (usartbase)
    {
      default:
        return;
#ifdef CONFIG_GD32H7_USART0_SERIALDRIVER
      case GD32H7_USART0:
        rcu_en = RCU_USART0_EN;
        break;
#endif
#ifdef CONFIG_GD32H7_USART1_SERIALDRIVER
      case GD32H7_USART1:
        rcu_en = RCU_USART1_EN;
        break;
#endif
#ifdef CONFIG_GD32H7_USART2_SERIALDRIVER
      case GD32H7_USART2:
        rcu_en = RCU_USART2_EN;
        break;
#endif
#ifdef CONFIG_GD32H7_UART3_SERIALDRIVER
      case GD32H7_UART3:
        rcu_en = RCU_UART3_EN;
        break;
#endif
#ifdef CONFIG_GD32H7_UART4_SERIALDRIVER
      case GD32H7_UART4:
        rcu_en = RCU_UART4_EN;
        break;
#endif
#ifdef CONFIG_GD32H7_USART5_SERIALDRIVER
      case GD32H7_USART5:
        rcu_en = RCU_USART5_EN;
        break;
#endif
#ifdef CONFIG_GD32H7_UART6_SERIALDRIVER
      case GD32H7_UART6:
        rcu_en = RCU_UART6_EN;
        break;
#endif
#ifdef CONFIG_GD32H7_UART7_SERIALDRIVER
      case GD32H7_UART7:
        rcu_en = RCU_UART7_EN;
        break;
#endif
    }

  /* Disable APB1/2 clock for USART */

  gd32_rcu_periph_clock_disable(rcu_en);
}
