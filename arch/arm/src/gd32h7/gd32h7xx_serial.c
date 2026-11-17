/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_serial.c
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

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <debug.h>
#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/fs/ioctl.h>
#include <nuttx/serial/serial.h>
#include <nuttx/spinlock.h>

#ifdef CONFIG_SERIAL_TERMIOS
#  include <termios.h>
#endif

#include <arch/board/board.h>

#include "chip.h"
#include "gd32h7xx.h"
#include "gd32h7xx_usart.h"
#include "gd32h7xx_rcu.h"
#include "arm_internal.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Interrupt bits used by the driver.
 *
 * On the GD32H7xx, the receive/transmit/parity interrupt enables are in
 * the CTL0 register and the error interrupt enable is in the CTL2
 * register.
 */

#define USART_CTL0_USED_INTS \
  (USART_CTL0_IDLEIE | USART_CTL0_RBNEIE | USART_CTL0_TCIE | \
   USART_CTL0_TBEIE | USART_CTL0_PERRIE)

#define USART_CTL2_USED_INTS (USART_CTL2_ERRIE)

/* If we are not using the serial driver for the console, then we still must
 * provide some minimal implementation of up_putc.
 */

#ifdef USE_SERIALDRIVER

#ifdef HAVE_SERIALDRIVER

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct up_dev_s
{
  bool            initialized;   /* Indicate that if has been initialized and HW is setup. */

#ifdef CONFIG_SERIAL_TERMIOS

#  ifdef CONFIG_SERIAL_IFLOWCONTROL
  bool            iflow;         /* input flow control (RTS) enabled */
#  endif
#  ifdef CONFIG_SERIAL_OFLOWCONTROL
  bool            oflow;         /* output flow control (CTS) enabled */
#  endif

  uint16_t        oversamp;      /* USART oversample mode */
  uintptr_t       usartbase;     /* Base address of UART registers */
  uint32_t        baud;          /* Configured baud */
  uint32_t        clock;         /* Frequency of the UART */
  uint8_t         irq;           /* IRQ associated with this USART */
  uint8_t         parity;        /* 0=none, 1=odd, 2=even */
  uint8_t         bits;          /* Number of bits (7, 8 or 9) */
  uint8_t         stop_2bits;    /* True: Configure with 2 stop bits instead of 1 */
  uint32_t        tx_gpio;       /* USART TX GPIO pin configuration */
  uint32_t        rx_gpio;       /* USART RX GPIO pin configuration */

#else /* CONFIG_SERIAL_TERMIOS */

  const uint16_t  oversamp;      /* USART oversample mode */
  const uintptr_t usartbase;     /* Base address of UART registers */
  const uint32_t  baud;          /* Configured baud */
  const uint32_t  clock;         /* Frequency of the UART */
  const uint8_t   irq;           /* IRQ associated with this USART */
  const uint8_t   parity;        /* 0=none, 1=odd, 2=even */
  const uint8_t   bits;          /* Number of bits (7, 8 or 9) */
  const uint8_t   stop_2bits;    /* True: Configure with 2 stop bits instead of 1 */
  const uint32_t  tx_gpio;       /* USART TX GPIO pin configuration */
  const uint32_t  rx_gpio;       /* USART RX GPIO pin configuration */

#endif /* CONFIG_SERIAL_TERMIOS */

  uint32_t        ie;            /* USART enabled interrupts */
  uint32_t        sr;            /* Save USART status */
  spinlock_t      lock;          /* Spinlock */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static inline uint32_t up_serialin(struct up_dev_s *priv, int offset);
static inline void up_serialout(struct up_dev_s *priv, int offset,
                                uint32_t value);
static void up_setusartint(struct up_dev_s *priv, uint32_t ie);
static void up_disableusartint(struct up_dev_s *priv, uint32_t *ie);
static void up_restoreusartint(struct up_dev_s *priv, uint32_t ie);

static void gd32_usart_configure(struct uart_dev_s *dev);

static int  up_setup(struct uart_dev_s *dev);
static void up_shutdown(struct uart_dev_s *dev);
static int  up_attach(struct uart_dev_s *dev);
static void up_detach(struct uart_dev_s *dev);
static int  up_interrupt(int irq, void *context, void *arg);
static int  up_ioctl(struct file *filep, int cmd, unsigned long arg);

static int  up_receive(struct uart_dev_s *dev, unsigned int *status);
static void up_rxint(struct uart_dev_s *dev, bool enable);
static bool up_rxavailable(struct uart_dev_s *dev);
#ifdef CONFIG_SERIAL_IFLOWCONTROL
static bool up_rxflowcontrol(struct uart_dev_s *dev, unsigned int nbuffered,
                             bool upper);
#endif
static void up_send(struct uart_dev_s *dev, int ch);
static void up_txint(struct uart_dev_s *dev, bool enable);
static bool up_txready(struct uart_dev_s *dev);
static bool up_txempty(struct uart_dev_s *dev);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct uart_ops_s g_uart_ops =
{
  .setup          = up_setup,
  .shutdown       = up_shutdown,
  .attach         = up_attach,
  .detach         = up_detach,
  .ioctl          = up_ioctl,
  .receive        = up_receive,
  .rxint          = up_rxint,
  .rxavailable    = up_rxavailable,
#ifdef CONFIG_SERIAL_IFLOWCONTROL
  .rxflowcontrol  = up_rxflowcontrol,
#endif
  .send           = up_send,
  .txint          = up_txint,
  .txready        = up_txready,
  .txempty        = up_txempty,
};

/* Receive / Transmit buffers */

#ifdef CONFIG_GD32H7_USART0_SERIALDRIVER
static char g_usart0rxbuffer[CONFIG_USART0_RXBUFSIZE];
static char g_usart0txbuffer[CONFIG_USART0_TXBUFSIZE];
#endif

#ifdef CONFIG_GD32H7_USART1_SERIALDRIVER
static char g_usart1rxbuffer[CONFIG_USART1_RXBUFSIZE];
static char g_usart1txbuffer[CONFIG_USART1_TXBUFSIZE];
#endif

#ifdef CONFIG_GD32H7_USART2_SERIALDRIVER
static char g_usart2rxbuffer[CONFIG_USART2_RXBUFSIZE];
static char g_usart2txbuffer[CONFIG_USART2_TXBUFSIZE];
#endif

#ifdef CONFIG_GD32H7_UART3_SERIALDRIVER
static char g_uart3rxbuffer[CONFIG_UART3_RXBUFSIZE];
static char g_uart3txbuffer[CONFIG_UART3_TXBUFSIZE];
#endif

#ifdef CONFIG_GD32H7_UART4_SERIALDRIVER
static char g_uart4rxbuffer[CONFIG_UART4_RXBUFSIZE];
static char g_uart4txbuffer[CONFIG_UART4_TXBUFSIZE];
#endif

#ifdef CONFIG_GD32H7_USART5_SERIALDRIVER
static char g_usart5rxbuffer[CONFIG_USART5_RXBUFSIZE];
static char g_usart5txbuffer[CONFIG_USART5_TXBUFSIZE];
#endif

#ifdef CONFIG_GD32H7_UART6_SERIALDRIVER
static char g_uart6rxbuffer[CONFIG_UART6_RXBUFSIZE];
static char g_uart6txbuffer[CONFIG_UART6_TXBUFSIZE];
#endif

#ifdef CONFIG_GD32H7_UART7_SERIALDRIVER
static char g_uart7rxbuffer[CONFIG_UART7_RXBUFSIZE];
static char g_uart7txbuffer[CONFIG_UART7_TXBUFSIZE];
#endif

/* This describes the state of the GD32H7 USART0 port. */

#ifdef CONFIG_GD32H7_USART0_SERIALDRIVER

static struct up_dev_s g_usart0priv =
{
  .oversamp      = 0,
  .usartbase     = GD32H7_USART0,
  .baud          = CONFIG_USART0_BAUD,
  .clock         = GD32_PCLK2_FREQUENCY,
  .irq           = GD32_IRQ_USART0,
  .parity        = CONFIG_USART0_PARITY,
  .bits          = CONFIG_USART0_BITS,
  .stop_2bits    = CONFIG_USART0_2STOP,

  .tx_gpio       = GPIO_USART0_TX,
  .rx_gpio       = GPIO_USART0_RX,
  .lock          = SP_UNLOCKED,
};

static struct uart_dev_s g_usart0port =
{
#if defined(CONSOLE_UART) && CONSOLE_UART == 0
  .isconsole = true,
#endif
  .recv      =
  {
    .size    = CONFIG_USART0_RXBUFSIZE,
    .buffer  = g_usart0rxbuffer,
  },
  .xmit      =
  {
    .size    = CONFIG_USART0_TXBUFSIZE,
    .buffer  = g_usart0txbuffer,
  },
  .ops       = &g_uart_ops,
  .priv      = &g_usart0priv,
};

#endif

/* This describes the state of the GD32H7 USART1 port. */

#ifdef CONFIG_GD32H7_USART1_SERIALDRIVER

static struct up_dev_s g_usart1priv =
{
  .oversamp      = 0,
  .usartbase     = GD32H7_USART1,
  .baud          = CONFIG_USART1_BAUD,
  .clock         = GD32_PCLK1_FREQUENCY,
  .irq           = GD32_IRQ_USART1,
  .parity        = CONFIG_USART1_PARITY,
  .bits          = CONFIG_USART1_BITS,
  .stop_2bits    = CONFIG_USART1_2STOP,

  .tx_gpio       = GPIO_USART1_TX,
  .rx_gpio       = GPIO_USART1_RX,
  .lock          = SP_UNLOCKED,
};

static struct uart_dev_s g_usart1port =
{
#if defined(CONSOLE_UART) && CONSOLE_UART == 1
  .isconsole = true,
#endif
  .recv      =
  {
    .size    = CONFIG_USART1_RXBUFSIZE,
    .buffer  = g_usart1rxbuffer,
  },
  .xmit      =
  {
    .size    = CONFIG_USART1_TXBUFSIZE,
    .buffer  = g_usart1txbuffer,
  },
  .ops       = &g_uart_ops,
  .priv      = &g_usart1priv,
};

#endif

/* This describes the state of the GD32H7 USART2 port. */

#ifdef CONFIG_GD32H7_USART2_SERIALDRIVER

static struct up_dev_s g_usart2priv =
{
  .oversamp      = 0,
  .usartbase     = GD32H7_USART2,
  .baud          = CONFIG_USART2_BAUD,
  .clock         = GD32_PCLK1_FREQUENCY,
  .irq           = GD32_IRQ_USART2,
  .parity        = CONFIG_USART2_PARITY,
  .bits          = CONFIG_USART2_BITS,
  .stop_2bits    = CONFIG_USART2_2STOP,

  .tx_gpio       = GPIO_USART2_TX,
  .rx_gpio       = GPIO_USART2_RX,
  .lock          = SP_UNLOCKED,
};

static struct uart_dev_s g_usart2port =
{
#if defined(CONSOLE_UART) && CONSOLE_UART == 2
  .isconsole = true,
#endif
  .recv      =
  {
    .size    = CONFIG_USART2_RXBUFSIZE,
    .buffer  = g_usart2rxbuffer,
  },
  .xmit      =
  {
    .size    = CONFIG_USART2_TXBUFSIZE,
    .buffer  = g_usart2txbuffer,
  },
  .ops       = &g_uart_ops,
  .priv      = &g_usart2priv,
};

#endif

/* This describes the state of the GD32H7 USART5 port. */

#ifdef CONFIG_GD32H7_USART5_SERIALDRIVER

static struct up_dev_s g_usart5priv =
{
  .oversamp      = 0,
  .usartbase     = GD32H7_USART5,
  .baud          = CONFIG_USART5_BAUD,
  .clock         = GD32_PCLK2_FREQUENCY,
  .irq           = GD32_IRQ_USART5,
  .parity        = CONFIG_USART5_PARITY,
  .bits          = CONFIG_USART5_BITS,
  .stop_2bits    = CONFIG_USART5_2STOP,

  .tx_gpio       = GPIO_USART5_TX,
  .rx_gpio       = GPIO_USART5_RX,
  .lock          = SP_UNLOCKED,
};

static struct uart_dev_s g_usart5port =
{
#if defined(CONSOLE_UART) && CONSOLE_UART == 5
  .isconsole = true,
#endif
  .recv      =
  {
    .size    = CONFIG_USART5_RXBUFSIZE,
    .buffer  = g_usart5rxbuffer,
  },
  .xmit      =
  {
    .size    = CONFIG_USART5_TXBUFSIZE,
    .buffer  = g_usart5txbuffer,
  },
  .ops       = &g_uart_ops,
  .priv      = &g_usart5priv,
};

#endif

/* This describes the state of the GD32H7 UART3 port. */

#ifdef CONFIG_GD32H7_UART3_SERIALDRIVER

static struct up_dev_s g_uart3priv =
{
  .oversamp      = 0,
  .usartbase     = GD32H7_UART3,
  .baud          = CONFIG_UART3_BAUD,
  .clock         = GD32_PCLK1_FREQUENCY,
  .irq           = GD32_IRQ_UART3,
  .parity        = CONFIG_UART3_PARITY,
  .bits          = CONFIG_UART3_BITS,
  .stop_2bits    = CONFIG_UART3_2STOP,

  .tx_gpio       = GPIO_UART3_TX,
  .rx_gpio       = GPIO_UART3_RX,
  .lock          = SP_UNLOCKED,
};

static struct uart_dev_s g_uart3port =
{
#if defined(CONSOLE_UART) && CONSOLE_UART == 3
  .isconsole = true,
#endif
  .recv      =
  {
    .size    = CONFIG_UART3_RXBUFSIZE,
    .buffer  = g_uart3rxbuffer,
  },
  .xmit      =
  {
    .size    = CONFIG_UART3_TXBUFSIZE,
    .buffer  = g_uart3txbuffer,
  },
  .ops       = &g_uart_ops,
  .priv      = &g_uart3priv,
};

#endif

/* This describes the state of the GD32H7 UART4 port. */

#ifdef CONFIG_GD32H7_UART4_SERIALDRIVER

static struct up_dev_s g_uart4priv =
{
  .oversamp      = 0,
  .usartbase     = GD32H7_UART4,
  .baud          = CONFIG_UART4_BAUD,
  .clock         = GD32_PCLK1_FREQUENCY,
  .irq           = GD32_IRQ_UART4,
  .parity        = CONFIG_UART4_PARITY,
  .bits          = CONFIG_UART4_BITS,
  .stop_2bits    = CONFIG_UART4_2STOP,

  .tx_gpio       = GPIO_UART4_TX,
  .rx_gpio       = GPIO_UART4_RX,
  .lock          = SP_UNLOCKED,
};

static struct uart_dev_s g_uart4port =
{
#if defined(CONSOLE_UART) && CONSOLE_UART == 4
  .isconsole = true,
#endif
  .recv      =
  {
    .size    = CONFIG_UART4_RXBUFSIZE,
    .buffer  = g_uart4rxbuffer,
  },
  .xmit      =
  {
    .size    = CONFIG_UART4_TXBUFSIZE,
    .buffer  = g_uart4txbuffer,
  },
  .ops       = &g_uart_ops,
  .priv      = &g_uart4priv,
};

#endif

/* This describes the state of the GD32H7 UART6 port. */

#ifdef CONFIG_GD32H7_UART6_SERIALDRIVER

static struct up_dev_s g_uart6priv =
{
  .oversamp      = 0,
  .usartbase     = GD32H7_UART6,
  .baud          = CONFIG_UART6_BAUD,
  .clock         = GD32_PCLK1_FREQUENCY,
  .irq           = GD32_IRQ_UART6,
  .parity        = CONFIG_UART6_PARITY,
  .bits          = CONFIG_UART6_BITS,
  .stop_2bits    = CONFIG_UART6_2STOP,

  .tx_gpio       = GPIO_UART6_TX,
  .rx_gpio       = GPIO_UART6_RX,
  .lock          = SP_UNLOCKED,
};

static struct uart_dev_s g_uart6port =
{
#if defined(CONSOLE_UART) && CONSOLE_UART == 6
  .isconsole = true,
#endif
  .recv      =
  {
    .size    = CONFIG_UART6_RXBUFSIZE,
    .buffer  = g_uart6rxbuffer,
  },
  .xmit      =
  {
    .size    = CONFIG_UART6_TXBUFSIZE,
    .buffer  = g_uart6txbuffer,
  },
  .ops       = &g_uart_ops,
  .priv      = &g_uart6priv,
};

#endif

/* This describes the state of the GD32H7 UART7 port. */

#ifdef CONFIG_GD32H7_UART7_SERIALDRIVER

static struct up_dev_s g_uart7priv =
{
  .oversamp      = 0,
  .usartbase     = GD32H7_UART7,
  .baud          = CONFIG_UART7_BAUD,
  .clock         = GD32_PCLK1_FREQUENCY,
  .irq           = GD32_IRQ_UART7,
  .parity        = CONFIG_UART7_PARITY,
  .bits          = CONFIG_UART7_BITS,
  .stop_2bits    = CONFIG_UART7_2STOP,

  .tx_gpio       = GPIO_UART7_TX,
  .rx_gpio       = GPIO_UART7_RX,
  .lock          = SP_UNLOCKED,
};

static struct uart_dev_s g_uart7port =
{
#if defined(CONSOLE_UART) && CONSOLE_UART == 7
  .isconsole = true,
#endif
  .recv      =
  {
    .size    = CONFIG_UART7_RXBUFSIZE,
    .buffer  = g_uart7rxbuffer,
  },
  .xmit      =
  {
    .size    = CONFIG_UART7_TXBUFSIZE,
    .buffer  = g_uart7txbuffer,
  },
  .ops       = &g_uart_ops,
  .priv      = &g_uart7priv,
};

#endif

/* This table lets us iterate over the configured USARTs */

static struct uart_dev_s * const g_uart_devs[GD32_NUSART] =
{
#ifdef CONFIG_GD32H7_USART0_SERIALDRIVER
  [0] = &g_usart0port,
#endif
#ifdef CONFIG_GD32H7_USART1_SERIALDRIVER
  [1] = &g_usart1port,
#endif
#ifdef CONFIG_GD32H7_USART2_SERIALDRIVER
  [2] = &g_usart2port,
#endif
#ifdef CONFIG_GD32H7_UART3_SERIALDRIVER
  [3] = &g_uart3port,
#endif
#ifdef CONFIG_GD32H7_UART4_SERIALDRIVER
  [4] = &g_uart4port,
#endif
#ifdef CONFIG_GD32H7_USART5_SERIALDRIVER
  [5] = &g_usart5port,
#endif
#ifdef CONFIG_GD32H7_UART6_SERIALDRIVER
  [6] = &g_uart6port,
#endif
#ifdef CONFIG_GD32H7_UART7_SERIALDRIVER
  [7] = &g_uart7port,
#endif
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_serialin
 ****************************************************************************/

static inline uint32_t up_serialin(struct up_dev_s *priv, int offset)
{
  return getreg32(priv->usartbase + offset);
}

/****************************************************************************
 * Name: up_serialout
 ****************************************************************************/

static inline void up_serialout(struct up_dev_s *priv, int offset,
                                uint32_t value)
{
  putreg32(value, priv->usartbase + offset);
}

/****************************************************************************
 * Name: up_setusartint
 ****************************************************************************/

static void up_setusartint(struct up_dev_s *priv, uint32_t ie)
{
  uint32_t regval;

  /* Save the USART interrupts */

  priv->ie = ie;

  /* Set interrupts in the CTL0 register */

  regval  = up_serialin(priv, GD32H7_USART_CTL0_OFFSET);
  regval &= ~USART_CTL0_USED_INTS;
  regval |= (ie & USART_CTL0_USED_INTS);
  up_serialout(priv, GD32H7_USART_CTL0_OFFSET, regval);

  /* Set interrupts in the CTL2 register */

  regval  = up_serialin(priv, GD32H7_USART_CTL2_OFFSET);
  regval &= ~USART_CTL2_USED_INTS;
  regval |= (ie & USART_CTL2_USED_INTS);
  up_serialout(priv, GD32H7_USART_CTL2_OFFSET, regval);
}

/****************************************************************************
 * Name: up_disableusartint
 ****************************************************************************/

static void up_disableusartint(struct up_dev_s *priv, uint32_t *ie)
{
  irqstate_t flags;

  flags = spin_lock_irqsave(&priv->lock);

  if (ie)
    {
      /* Save the current interrupt enables */

      *ie = priv->ie;
    }

  /* Disable all interrupts */

  up_setusartint(priv, 0);

  spin_unlock_irqrestore(&priv->lock, flags);
}

/****************************************************************************
 * Name: up_restoreusartint
 ****************************************************************************/

static void up_restoreusartint(struct up_dev_s *priv, uint32_t ie)
{
  irqstate_t flags;

  flags = spin_lock_irqsave(&priv->lock);

  up_setusartint(priv, ie);

  spin_unlock_irqrestore(&priv->lock, flags);
}

/****************************************************************************
 * Name: gd32_usart_configure
 *
 * Description:
 *   Configure the USART.
 *
 ****************************************************************************/

#ifndef CONFIG_SUPPRESS_UART_CONFIG
static void gd32_usart_configure(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;

  uint32_t udiv;
  uint32_t intdiv;
  uint32_t fradiv;
  uint32_t regval;

  /* Configure the USART oversample mode (16x by default). */

  regval = up_serialin(priv, GD32H7_USART_CTL0_OFFSET);
  regval &= ~USART_CTL0_OVSMOD;
  regval |= ((uint32_t)priv->oversamp << 15);
  up_serialout(priv, GD32H7_USART_CTL0_OFFSET, regval);

  /* Configure USART baud rate value */

  if ((priv->oversamp & 1) != 0)
    {
      /* When oversampling by 8, configure the value of USART_BAUD */

      udiv = ((priv->clock * 2) + priv->baud / 2) / priv->baud;
      intdiv = udiv & 0xfff0;
      fradiv = (udiv >> 1) & 0x7;
    }
  else
    {
      /* When oversampling by 16, configure the value of USART_BAUD */

      udiv = (priv->clock + priv->baud / 2) / priv->baud;
      intdiv = udiv & 0xfff0;
      fradiv = udiv & 0xf;
    }

  regval = ((USART_BAUD_FRADIV_MASK | USART_BAUD_INTDIV_MASK) &
            (intdiv | fradiv));
  up_serialout(priv, GD32H7_USART_BAUD_OFFSET, regval);

  /* Configure parity mode */

  regval = up_serialin(priv, GD32H7_USART_CTL0_OFFSET);
  regval &= ~(USART_CTL0_PM | USART_CTL0_PCEN | USART_CTL0_WL0 | USART_CTL0_WL1);

  if (priv->parity == 1)
    {
      /* Configure as odd parity */

      regval |= (USART_CTL0_PM_ODD | USART_CTL0_PCEN);
    }
  else if (priv->parity == 2)
    {
      /* Configure as even parity */

      regval |= (USART_CTL0_PM_EVEN | USART_CTL0_PCEN);
    }

  /* Configure USART word length:
   * With parity enabled the effective data length must include the parity
   * bit, so 8 data bits + parity are configured as 9-bit word length.
   */

  if (priv->bits == 9 || (priv->bits == 8 && priv->parity != 0))
    {
      regval |= USART_CTL0_WL_9BIT;
    }
  else if (priv->bits == 7)
    {
      regval |= USART_CTL0_WL_7BIT;
    }
  else
    {
      regval |= USART_CTL0_WL_8BIT;
    }

  up_serialout(priv, GD32H7_USART_CTL0_OFFSET, regval);

  /* Configure STOP bits */

  regval = up_serialin(priv, GD32H7_USART_CTL1_OFFSET);
  regval &= ~(USART_CTL1_STB);

  if (priv->stop_2bits)
    {
      regval |= USART_CTL1_STB_2BIT;
    }

  up_serialout(priv, GD32H7_USART_CTL1_OFFSET, regval);
}
#endif /* CONFIG_SUPPRESS_UART_CONFIG */

/****************************************************************************
 * Name: up_setup
 *
 * Description:
 *   Configure the USART baud, bits, parity, etc. This method is called the
 *   first time that the serial port is opened.
 *
 ****************************************************************************/

static int up_setup(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;

#ifndef CONFIG_SUPPRESS_UART_CONFIG
  uint32_t regval;

  /* Enable USART clock */

  gd32_usart_clock_enable(priv->usartbase);

  /* Configure pins for USART use */

  gd32_configgpio(priv->tx_gpio);
  gd32_configgpio(priv->rx_gpio);

  /* Reset the USART */

  gd32_usart_reset(priv->usartbase);

  /* Disable the USART before to configure it. */

  regval = up_serialin(priv, GD32H7_USART_CTL0_OFFSET);
  regval &= ~USART_CTL0_UEN;
  up_serialout(priv, GD32H7_USART_CTL0_OFFSET, regval);

  /* Configure the USART. */

  gd32_usart_configure(dev);

  /* Enable USART receiver and transmitter */

  regval = up_serialin(priv, GD32H7_USART_CTL0_OFFSET);
  regval |= (USART_CTL0_UEN | USART_CTL0_REN | USART_CTL0_TEN);
  up_serialout(priv, GD32H7_USART_CTL0_OFFSET, regval);

#endif /* CONFIG_SUPPRESS_UART_CONFIG */

  /* Set up the cached interrupt enables value */

  priv->ie = 0;

  /* Set initialize state. */

  priv->initialized = true;

  return OK;
}

/****************************************************************************
 * Name: up_shutdown
 *
 * Description:
 *   Disable the USART.  This method is called when the serial
 *   port is closed
 *
 ****************************************************************************/

static void up_shutdown(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;

  /* Reset initialize state. */

  priv->initialized = false;

  /* Reset hardware and disable Rx and Tx */

  gd32_usart_reset(priv->usartbase);

  /* Disable USART clock */

  gd32_usart_clock_disable(priv->usartbase);

  /* Unconfigure RX, TX GPIO */

  gd32_unconfiggpio(priv->tx_gpio);
  gd32_unconfiggpio(priv->rx_gpio);
}

/****************************************************************************
 * Name: up_attach
 *
 * Description:
 *   Configure the USART to operation in interrupt driven mode.  This method
 *   is called when the serial port is opened.  Normally, this is just after
 *   the setup() method is called, however, the serial console may operate
 *   in a non-interrupt driven mode during the boot phase.
 *
 *   RX and TX interrupts are not enabled when by the attach method (unless
 *   the hardware supports multiple levels of interrupt enabling).  The RX
 *   and TX interrupts are not enabled until the txint() and rxint() methods
 *   are called.
 *
 ****************************************************************************/

static int up_attach(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;
  int ret;

  /* Attach and enable the IRQ */

  ret = irq_attach(priv->irq, up_interrupt, dev);
  if (ret == OK)
    {
      /* Enable the interrupt (RX and TX interrupts are still disabled
       * in the USART
       */

      up_enable_irq(priv->irq);
    }

  return ret;
}

/****************************************************************************
 * Name: up_detach
 *
 * Description:
 *   Detach USART interrupts.  This method is called when the serial port is
 *   closed normally just before the shutdown method is called.  The
 *   exception is the serial console which is never shutdown.
 *
 ****************************************************************************/

static void up_detach(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;
  up_disable_irq(priv->irq);
  irq_detach(priv->irq);
}

/****************************************************************************
 * Name: up_interrupt
 *
 * Description:
 *   This is the UART interrupt handler.  It will be invoked when an
 *   interrupt is received on the 'irq'.  It should call uart_xmitchars or
 *   uart_recvchars to perform the appropriate data transfers.  The
 *   interrupt handling logic must be able to map the 'arg' to the
 *   appropriate uart_dev_s structure in order to call these functions.
 *
 ****************************************************************************/

static int up_interrupt(int irq, void *context, void *arg)
{
  struct uart_dev_s *dev = (struct uart_dev_s *)arg;
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;
  int passes;
  bool handled;

  DEBUGASSERT(dev != NULL && priv != NULL);

  /* Loop until there are no characters to be transferred or,
   * until we have been looping for a long time.
   */

  handled = true;
  for (passes = 0; passes < 256 && handled; passes++)
    {
      handled = false;

      /* Get the current USART status. */

      priv->sr = up_serialin(priv, GD32H7_USART_STAT_OFFSET);

      /* Handle incoming, receive bytes. */

      if (((priv->sr & USART_STAT_RBNE) != 0) &&
          ((priv->ie & USART_CTL0_RBNEIE) == USART_CTL0_RBNEIE))
        {
          uart_recvchars(dev);
          handled = true;
        }

      /* Clear USART error flags.
       *
       * NOTE: On the GD32H7xx the error flags are cleared through the
       * INTC register, not by writing to the STAT register.  Reading the
       * RDATA register alone is not sufficient to clear the overrun
       * flag.
       */

      if ((priv->sr & USART_STAT_EFLAG_MASK) != 0)
        {
          uint32_t intc = 0;

          if ((priv->sr & USART_STAT_ORERR) != 0)
            {
              intc |= USART_INTC_OREC;
            }

          if ((priv->sr & USART_STAT_NERR) != 0)
            {
              intc |= USART_INTC_NEC;
            }

          if ((priv->sr & USART_STAT_FERR) != 0)
            {
              intc |= USART_INTC_FEC;
            }

          if ((priv->sr & USART_STAT_PERR) != 0)
            {
              intc |= USART_INTC_PEC;
            }

          up_serialout(priv, GD32H7_USART_INTC_OFFSET, intc);
          handled = true;
        }

      /* Handle outgoing, transmit bytes */

      if (((priv->sr & USART_STAT_TBE) != 0) &&
          ((priv->ie & USART_CTL0_TBEIE) == USART_CTL0_TBEIE))
        {
          uart_xmitchars(dev);
          handled = true;
        }
    }

  return OK;
}

/****************************************************************************
 * Name: up_ioctl
 *
 * Description:
 *   All ioctl calls will be routed through this method
 *
 ****************************************************************************/

static int up_ioctl(struct file *filep, int cmd, unsigned long arg)
{
#if defined(CONFIG_SERIAL_TERMIOS) || defined(CONFIG_SERIAL_TIOCSERGSTRUCT)
  struct inode      *inode = filep->f_inode;
  struct uart_dev_s *dev   = inode->i_private;
#endif
#ifdef CONFIG_SERIAL_TERMIOS
  struct up_dev_s   *priv  = (struct up_dev_s *)dev->priv;
#endif
  int ret = OK;

  switch (cmd)
    {
#ifdef CONFIG_SERIAL_TIOCSERGSTRUCT
    case TIOCSERGSTRUCT:
      {
        struct up_dev_s *user = (struct up_dev_s *)arg;
        if (!user)
          {
            ret = -EINVAL;
          }
        else
          {
            memcpy(user, dev, sizeof(struct up_dev_s));
          }
      }
      break;
#endif

#ifdef CONFIG_SERIAL_TERMIOS
    case TCGETS:
      {
        struct termios *termiosp = (struct termios *)arg;

        if (!termiosp)
          {
            ret = -EINVAL;
            break;
          }

        /* The driver only supports 8-bit mode and therefore there is
         * no way to report 9-bit mode, we always claim 8.
         */

        termiosp->c_cflag = CS8;

        if (priv->parity != 0)
          {
            termiosp->c_cflag |= PARENB;
          }

        if (priv->parity == 1)
          {
            termiosp->c_cflag |= PARODD;
          }

        if (priv->stop_2bits)
          {
            termiosp->c_cflag |= CSTOPB;
          }

        cfsetispeed(termiosp, priv->baud);
      }
      break;

    case TCSETS:
      {
        struct termios *termiosp = (struct termios *)arg;

        if (!termiosp)
          {
            ret = -EINVAL;
            break;
          }

        /* Perform some sanity checks before accepting any changes */

        if ((termiosp->c_cflag & CSIZE) != CS8)
          {
            ret = -EINVAL;
            break;
          }

        if (termiosp->c_cflag & PARENB)
          {
            priv->parity = (termiosp->c_cflag & PARODD) ? 1 : 2;
          }
        else
          {
            priv->parity = 0;
          }

        priv->stop_2bits = (termiosp->c_cflag & CSTOPB) != 0;

        /* Note that only cfgetispeed is used because we have knowledge
         * that only one speed is supported.
         */

        priv->baud = cfgetispeed(termiosp);

        /* Effect the changes immediately - note that we do not implement
         * TCSADRAIN / TCSAFLUSH
         */

        gd32_usart_configure(dev);
      }
      break;
#endif /* CONFIG_SERIAL_TERMIOS */

    default:
      ret = -ENOTTY;
      break;
    }

  return ret;
}

/****************************************************************************
 * Name: up_receive
 *
 * Description:
 *   Called (usually) from the interrupt level to receive one
 *   character from the USART.  Error bits associated with the
 *   receipt are provided in the return 'status'.
 *
 ****************************************************************************/

static int up_receive(struct uart_dev_s *dev, unsigned int *status)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;
  uint32_t data;

  /* Get the receive data */

  data = up_serialin(priv, GD32H7_USART_RDATA_OFFSET);

  /* Return status information */

  if (status)
    {
      *status  = priv->sr;
      priv->sr = 0;
    }

  /* Then return the receive data */

  return data & 0xff;
}

/****************************************************************************
 * Name: up_rxint
 *
 * Description:
 *   Call to enable or disable RX interrupts
 *
 ****************************************************************************/

static void up_rxint(struct uart_dev_s *dev, bool enable)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;
  irqstate_t flags;
  uint32_t ie;

  flags = enter_critical_section();
  ie = priv->ie;
  if (enable)
    {
      /* Receive an interrupt when there is anything in the Rx data register
       * (or an Rx timeout occurs).
       */

#ifndef CONFIG_SUPPRESS_SERIAL_INTS
#  ifdef CONFIG_USART_ERRINTS
      ie |= (USART_CTL0_RBNEIE | USART_CTL0_PERRIE | USART_CTL2_ERRIE);
#  else
      ie |= USART_CTL0_RBNEIE;
#  endif
#endif
    }
  else
    {
      ie &= ~(USART_CTL0_RBNEIE | USART_CTL0_PERRIE | USART_CTL2_ERRIE);
    }

  /* Then set the new interrupt state */

  up_restoreusartint(priv, ie);
  leave_critical_section(flags);
}

/****************************************************************************
 * Name: up_rxavailable
 *
 * Description:
 *   Return true if the receive register is not empty
 *
 ****************************************************************************/

static bool up_rxavailable(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;

  /* return true if read data buffer not empty */

  return ((up_serialin(priv, GD32H7_USART_STAT_OFFSET) &
           USART_STAT_RBNE) != 0);
}

/****************************************************************************
 * Name: up_rxflowcontrol
 *
 * Description:
 *   Called when the Rx buffer is full (or exceeds configured watermark
 *   levels).  Hardware flow control is not yet implemented in this driver;
 *   always return false so that incoming data continues to be processed.
 *
 ****************************************************************************/

#ifdef CONFIG_SERIAL_IFLOWCONTROL
static bool up_rxflowcontrol(struct uart_dev_s *dev,
                             unsigned int nbuffered, bool upper)
{
  (void)dev;
  (void)nbuffered;
  (void)upper;
  return false;
}
#endif

/****************************************************************************
 * Name: up_send
 *
 * Description:
 *   This method will send one byte on the USART
 *
 ****************************************************************************/

static void up_send(struct uart_dev_s *dev, int ch)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;

  up_serialout(priv, GD32H7_USART_TDATA_OFFSET,
               (uint32_t)(ch & GD32H7_USART_DATA_MASK));
}

/****************************************************************************
 * Name: up_txint
 *
 * Description:
 *   Call to enable or disable TX interrupts
 *
 ****************************************************************************/

static void up_txint(struct uart_dev_s *dev, bool enable)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;
  irqstate_t flags;

  flags = enter_critical_section();
  if (enable)
    {
      /* Enable the TX interrupt */

#ifndef CONFIG_SUPPRESS_SERIAL_INTS
      up_setusartint(priv, priv->ie | USART_CTL0_TBEIE);
#else
      /* Fake a TX interrupt here by just calling uart_xmitchars() with
       * interrupts disabled (note this may recurse).
       */

      uart_xmitchars(dev);
#endif
    }
  else
    {
      /* Disable the TX interrupt */

      up_setusartint(priv, priv->ie & ~USART_CTL0_TBEIE);
    }

  leave_critical_section(flags);
}

/****************************************************************************
 * Name: up_txready
 *
 * Description:
 *   Return true if the transmit data register is empty
 *
 ****************************************************************************/

static bool up_txready(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;
  return ((up_serialin(priv, GD32H7_USART_STAT_OFFSET) &
           USART_STAT_TBE) != 0);
}

/****************************************************************************
 * Name: up_txempty
 *
 * Description:
 *   Return true if the transmit data register is empty
 *
 ****************************************************************************/

static bool up_txempty(struct uart_dev_s *dev)
{
  struct up_dev_s *priv = (struct up_dev_s *)dev->priv;

  /* Return true if transmit data buffer empty */

  return ((up_serialin(priv, GD32H7_USART_STAT_OFFSET) &
           USART_STAT_TC) != 0);
}

#endif /* HAVE_SERIALDRIVER */
#endif /* USE_SERIALDRIVER */

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#ifdef USE_SERIALDRIVER

/****************************************************************************
 * Name: arm_earlyserialinit
 *
 * Description:
 *   Performs the low level USART initialization early in debug so that the
 *   serial console will be available during boot up.  This must be called
 *   before arm_serialinit.
 *
 ****************************************************************************/

#ifdef USE_EARLYSERIALINIT
void arm_earlyserialinit(void)
{
#ifdef HAVE_SERIALDRIVER
  unsigned i;

  /* Disable all USART interrupts */

  for (i = 0; i < GD32_NUSART; i++)
    {
      if (g_uart_devs[i] && g_uart_devs[i]->priv)
        {
          up_disableusartint((struct up_dev_s *)g_uart_devs[i]->priv, 0);
        }
    }

  /* Configure whichever one is the console */

#  ifdef CONSOLE_UART
  up_setup(g_uart_devs[CONSOLE_UART]);
#  endif
#endif /* HAVE SERIALDRIVER */
}
#endif

/****************************************************************************
 * Name: arm_serialinit
 *
 * Description:
 *   Register serial console and serial ports.  This assumes
 *   that arm_earlyserialinit was called previously.
 *
 ****************************************************************************/

void arm_serialinit(void)
{
#ifdef HAVE_SERIALDRIVER
  char devname[16];
  unsigned i;
  unsigned start = 0;

  /* Register the console */

#ifdef CONSOLE_UART
  uart_register("/dev/console", g_uart_devs[CONSOLE_UART]);

  /* If not disabled, register the console UART to ttyS0 and exclude
   * it from initializing it further down
   */

  uart_register("/dev/ttyS0", g_uart_devs[CONSOLE_UART]);
  start = 1;
#endif /* CONSOLE_UART */

  /* Register all remaining USARTs */

  strlcpy(devname, "/dev/ttySx", sizeof(devname));

  for (i = 0; i < GD32_NUSART; i++)
    {
      /* Don't create a device for non-configured ports. */

      if (g_uart_devs[i] == 0)
        {
          continue;
        }

      /* Don't create a device for the console - we did that above */

      if (g_uart_devs[i]->isconsole)
        {
          continue;
        }

      /* Register USARTs as devices in increasing order */

      devname[9] = '0' + start++;
      uart_register(devname, g_uart_devs[i]);
    }
#endif /* HAVE SERIALDRIVER */
}

/****************************************************************************
 * Name: up_putc
 *
 * Description:
 *   Provide priority, low-level access to support DEBUG output.  It must
 *   do this without interrupt disabling and without using any additional
 *   support re-entrancy.
 *
 ****************************************************************************/

void up_putc(int ch)
{
#ifdef HAVE_CONSOLE
  arm_lowputc(ch);
#endif
}

#else /* USE_SERIALDRIVER */

/****************************************************************************
 * Name: up_putc
 *
 * Description:
 *   Provide priority, low-level access to support DEBUG output.
 *
 ****************************************************************************/

void up_putc(int ch)
{
#ifdef HAVE_CONSOLE
  arm_lowputc(ch);
#endif
}

#endif /* USE_SERIALDRIVER */
