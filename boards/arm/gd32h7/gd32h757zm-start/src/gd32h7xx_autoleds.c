/****************************************************************************
 * boards/arm/gd32h7/gd32h757zm-start/src/gd32h7xx_autoleds.c
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

/* The GD32H757ZM-START has a single LED so a "blinking LED" convention is
 * used to indicate the system state:
 *
 *   SYMBOL                Meaning                   LED1 (PE5)
 *   --------------------- ------------------------  -----------
 *   LED_STARTED           NuttX has been started    OFF
 *   LED_HEAPALLOCATE      Heap has been allocated   ON
 *   LED_IRQSENABLED       Interrupts enabled        OFF
 *   LED_STACKCREATED      Idle stack created        ON
 *   LED_INIRQ             In an interrupt           No change
 *   LED_SIGNAL            In a signal handler       No change
 *   LED_ASSERTION         An assertion failed       No change
 *   LED_PANIC             The system has crashed    FLASH
 *   LED_IDLE              MCU is in sleep mode      Not used
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdbool.h>
#include <debug.h>

#include <sys/param.h>

#include <nuttx/board.h>
#include <arch/board/board.h>

#include "gd32h7xx_gpio.h"
#include "gd32h757zm_start.h"

#ifdef CONFIG_ARCH_LEDS

/****************************************************************************
 * Private Data
 ****************************************************************************/

static bool g_initialized;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/* Turn on the LED */

static void gd32_start_led_on(void)
{
  gd32_gpiowrite(GPIO_LED, true);
}

/* Turn off the LED */

static void gd32_start_led_off(void)
{
  gd32_gpiowrite(GPIO_LED, false);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_autoled_initialize
 ****************************************************************************/

void board_autoled_initialize(void)
{
  /* Configure the LED GPIO for output. Initial state is OFF */

  gd32_configgpio(GPIO_LED);
}

/****************************************************************************
 * Name: board_autoled_on
 ****************************************************************************/

void board_autoled_on(int led)
{
  switch (led)
    {
      default:
        break;

      case LED_HEAPALLOCATE:
      case LED_STACKCREATED:
        gd32_start_led_on();
        g_initialized = true;
        break;

      case LED_PANIC:
        gd32_start_led_on();
        break;
    }
}

/****************************************************************************
 * Name: board_autoled_off
 ****************************************************************************/

void board_autoled_off(int led)
{
  switch (led)
    {
      default:
        break;

      case LED_PANIC:
        /* Blink: turn the LED off.  The PANIC handler periodically toggles
         * the LED between on and off to make it flash.
         */

        gd32_start_led_off();
        break;

      case LED_HEAPALLOCATE:
      case LED_STACKCREATED:
        gd32_start_led_off();
        break;
    }
}

#endif /* CONFIG_ARCH_LEDS */
