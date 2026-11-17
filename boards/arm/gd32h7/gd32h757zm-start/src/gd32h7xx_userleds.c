/****************************************************************************
 * boards/arm/gd32h7/gd32h757zm-start/src/gd32h7xx_userleds.c
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

#include <stdbool.h>
#include <debug.h>

#include <sys/param.h>

#include <nuttx/board.h>
#include <arch/board/board.h>

#include "gd32h7xx_gpio.h"
#include "gd32h757zm_start.h"

#ifndef CONFIG_ARCH_LEDS

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const uint32_t g_led_map[BOARD_NLEDS] =
{
  GPIO_LED_RED,       /* LED1: red  (PD10) */
  GPIO_LED_BLUE       /* LED2: blue (PD11) */
};

static const uint32_t g_led_setmap[BOARD_NLEDS] =
{
  BOARD_LED1_BIT,
  BOARD_LED2_BIT
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_userled_initialize
 *
 * Description:
 *   If CONFIG_ARCH_LEDS is defined, then NuttX will control the on-board
 *   LEDs.  If CONFIG_ARCH_LEDS is not defined, then the
 *   board_userled_initialize() is available to initialize the LED from
 *   user application logic.
 *
 ****************************************************************************/

uint32_t board_userled_initialize(void)
{
  int i;

  /* Configure the LED GPIO for output. */

  for (i = 0; i < nitems(g_led_map); i++)
    {
      gd32_configgpio(g_led_map[i]);
    }

  return BOARD_NLEDS;
}

/****************************************************************************
 * Name: board_userled
 *
 * Description:
 *   If CONFIG_ARCH_LEDS is defined, then NuttX will control the on-board
 *   LEDs.  If CONFIG_ARCH_LEDS is not defined, then the board_userled()
 *   is available to control the LED from user application logic.
 *
 ****************************************************************************/

void board_userled(int led, bool ledon)
{
  if ((unsigned)led < nitems(g_led_map))
    {
      gd32_gpiowrite(g_led_map[led], ledon);
    }
}

/****************************************************************************
 * Name: board_userled_all
 *
 * Description:
 *   If CONFIG_ARCH_LEDS is defined, then NuttX will control the on-board
 *   LEDs.  If CONFIG_ARCH_LEDS is not defined, then the
 *   board_userled_all() is available to control the LED from user
 *   application logic.
 *
 ****************************************************************************/

void board_userled_all(uint32_t ledset)
{
  int i;

  /* Configure the LED GPIO for output */

  for (i = 0; i < nitems(g_led_map); i++)
    {
      gd32_gpiowrite(g_led_map[i], (ledset & g_led_setmap[i]) != 0);
    }
}

#endif /* !CONFIG_ARCH_LEDS */
