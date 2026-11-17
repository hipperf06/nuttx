/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_gpio.c
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
#include <stdbool.h>
#include <errno.h>

#include <nuttx/irq.h>

#include "arm_internal.h"

#include "chip.h"
#include "gd32h7xx.h"
#include "gd32h7xx_exti.h"

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* Description of one GPIO pin */

struct gd32_gpio_s
{
  uint32_t gpio_periph;       /* Base address of the GPIO port */
  uint32_t pin;               /* Pin number (0-15) */
};

/* Number of pins per GPIO port */

#define GD32_GPIO_NPINS  16

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* Lookup table that maps the GPIO_CFG_PORT_x encoding to the base address
 * of the GPIO port.
 */

static const uint32_t g_gpioport[GD32_NGPIO_PORTS] =
{
  GD32H7_GPIOA,  /* 0 */
  GD32H7_GPIOB,  /* 1 */
  GD32H7_GPIOC,  /* 2 */
  GD32H7_GPIOD,  /* 3 */
  GD32H7_GPIOE,  /* 4 */
  GD32H7_GPIOF,  /* 5 */
  GD32H7_GPIOG,  /* 6 */
  GD32H7_GPIOH,  /* 7 */
  GD32H7_GPIOJ,  /* 8 */
  GD32H7_GPIOK   /* 9 */
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_decodegpio
 *
 * Description:
 *   Decode a bit-encoded GPIO configuration and return the base address of
 *   the GPIO port and the pin number.
 *
 ****************************************************************************/

static int gd32_decodegpio(uint32_t cfgset, struct gd32_gpio_s *gpio)
{
  uint32_t port;

  port = (cfgset & GPIO_CFG_PORT_MASK) >> GPIO_CFG_PORT_SHIFT;
  if (port >= GD32_NGPIO_PORTS)
    {
      return -EINVAL;
    }

  gpio->gpio_periph = g_gpioport[port];
  gpio->pin         = (cfgset & GPIO_CFG_PIN_MASK) >> GPIO_CFG_PIN_SHIFT;
  return OK;
}

/****************************************************************************
 * Name: gd32_rcu_periph_clock
 *
 * Description:
 *   Return the RCU clock selector of the GPIO port identified by its base
 *   address.
 *
 ****************************************************************************/

static uint32_t gd32_rcu_periph_clock(uint32_t gpio_periph)
{
  switch (gpio_periph)
    {
      case GD32H7_GPIOA:
        return RCU_GPIOA_EN;
      case GD32H7_GPIOB:
        return RCU_GPIOB_EN;
      case GD32H7_GPIOC:
        return RCU_GPIOC_EN;
      case GD32H7_GPIOD:
        return RCU_GPIOD_EN;
      case GD32H7_GPIOE:
        return RCU_GPIOE_EN;
      case GD32H7_GPIOF:
        return RCU_GPIOF_EN;
      case GD32H7_GPIOG:
        return RCU_GPIOG_EN;
      case GD32H7_GPIOH:
        return RCU_GPIOH_EN;
      case GD32H7_GPIOJ:
        return RCU_GPIOJ_EN;
      case GD32H7_GPIOK:
        return RCU_GPIOK_EN;
      default:
        return 0;
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_configgpio
 *
 * Description:
 *   Configure a GPIO pin based on bit-encoded description of the pin.
 *   NOTE: GPIO_CFG_EXTI input configurations are handled by the caller
 *   through gd32_exti_init() + gd32_gpioexti_attach(); this function only
 *   configures the pin itself.
 *
 ****************************************************************************/

int gd32_configgpio(uint32_t cfgset)
{
  struct gd32_gpio_s gpio;
  uint32_t regval;
  uint32_t mode;
  uint32_t pupd;
  uint32_t speed;
  uint32_t af;
  uint32_t pin;
  int ret;

  ret = gd32_decodegpio(cfgset, &gpio);
  if (ret < 0)
    {
      return ret;
    }

  /* Enable the clock of the GPIO port */

  gd32_rcu_periph_clock_enable(gd32_rcu_periph_clock(gpio.gpio_periph));

  mode   = (cfgset & GPIO_CFG_MODE_MASK) >> GPIO_CFG_MODE_SHIFT;
  pupd   = (cfgset & GPIO_CFG_PUPD_MASK) >> GPIO_CFG_PUPD_SHIFT;
  speed  = (cfgset & GPIO_CFG_SPEED_MASK) >> GPIO_CFG_SPEED_SHIFT;
  af     = (cfgset & GPIO_CFG_AF_MASK) >> GPIO_CFG_AF_SHIFT;
  pin    = gpio.pin;

  /* Configure the port mode (CTL register, 2 bits per pin) */

  regval = getreg32(gpio.gpio_periph + GD32H7_GPIO_CTL_OFFSET);
  regval &= ~GD32H7_GPIO_MODE_MASK(pin);
  regval |= (mode << (pin << 1));
  putreg32(regval, gpio.gpio_periph + GD32H7_GPIO_CTL_OFFSET);

  /* Configure the pull-up/pull-down (PUD register, 2 bits per pin) */

  regval = getreg32(gpio.gpio_periph + GD32H7_GPIO_PUD_OFFSET);
  regval &= ~GD32H7_GPIO_PUPD_MASK(pin);
  regval |= (pupd << (pin << 1));
  putreg32(regval, gpio.gpio_periph + GD32H7_GPIO_PUD_OFFSET);

  if (mode == 1) /* GPIO_CFG_MODE_OUTPUT */
    {
      /* Configure the output type (OMODE register, 1 bit per pin):
       * bit set = open drain, bit clear = push pull.
       */

      regval = getreg32(gpio.gpio_periph + GD32H7_GPIO_OMODE_OFFSET);
      if ((cfgset & GPIO_CFG_ODPP_MASK) != 0)
        {
          regval |= GD32H7_GPIO_OMODE_BIT(pin);
        }
      else
        {
          regval &= ~GD32H7_GPIO_OMODE_BIT(pin);
        }

      putreg32(regval, gpio.gpio_periph + GD32H7_GPIO_OMODE_OFFSET);
    }

  /* Configure the output speed (OSPD register, 2 bits per pin) */

  regval = getreg32(gpio.gpio_periph + GD32H7_GPIO_OSPD_OFFSET);
  regval &= ~GD32H7_GPIO_OSPD_MASK(pin);
  regval |= (speed << (pin << 1));
  putreg32(regval, gpio.gpio_periph + GD32H7_GPIO_OSPD_OFFSET);

  if (mode == 2) /* GPIO_CFG_MODE_AF */
    {
      /* Configure the alternate function (AFSEL0 for pins 0-7, AFSEL1 for
       * pins 8-15, 4 bits per pin).
       */

      if (pin < 8)
        {
          regval = getreg32(gpio.gpio_periph + GD32H7_GPIO_AFSEL0_OFFSET);
          regval &= ~(0xf << (pin << 2));
          regval |= (af << (pin << 2));
          putreg32(regval, gpio.gpio_periph + GD32H7_GPIO_AFSEL0_OFFSET);
        }
      else
        {
          regval = getreg32(gpio.gpio_periph + GD32H7_GPIO_AFSEL1_OFFSET);
          regval &= ~(0xf << ((pin - 8) << 2));
          regval |= (af << ((pin - 8) << 2));
          putreg32(regval, gpio.gpio_periph + GD32H7_GPIO_AFSEL1_OFFSET);
        }
    }

  /* Set the initial output value for outputs */

  if (mode == 1) /* GPIO_CFG_MODE_OUTPUT */
    {
      if ((cfgset & GPIO_CFG_OUTPUT_MASK) != 0)
        {
          putreg32(1 << pin, gpio.gpio_periph + GD32H7_GPIO_BOP_OFFSET);
        }
      else
        {
          putreg32(1 << pin, gpio.gpio_periph + GD32H7_GPIO_BC_OFFSET);
        }
    }

  return OK;
}

/****************************************************************************
 * Name: gd32_unconfiggpio
 *
 * Description:
 *   Deconfigure a GPIO pin based on bit-encoded description of the pin, set
 *   it into default (analog input) state.
 *
 ****************************************************************************/

int gd32_unconfiggpio(uint32_t cfgset)
{
  /* Re-configure the pin as analog input */

  cfgset &= ~(GPIO_CFG_MODE_MASK | GPIO_CFG_PUPD_MASK);
  cfgset |= (GPIO_CFG_MODE_ANALOG | GPIO_CFG_PUPD_NONE);

  return gd32_configgpio(cfgset);
}

/****************************************************************************
 * Name: gd32_gpiowrite
 *
 * Description:
 *   Write or clear the value of a single GPIO bit.
 *
 ****************************************************************************/

void gd32_gpiowrite(uint32_t pinset, bool value)
{
  struct gd32_gpio_s gpio;
  uint32_t regaddr;

  if (gd32_decodegpio(pinset, &gpio) == OK)
    {
      /* Use the atomic BOP (bit set) or BC (bit clear) registers */

      regaddr = gpio.gpio_periph;
      regaddr += value ? GD32H7_GPIO_BOP_OFFSET : GD32H7_GPIO_BC_OFFSET;
      putreg32(1 << gpio.pin, regaddr);
    }
}

/****************************************************************************
 * Name: gd32_gpioread
 *
 * Description:
 *   Read the value of a single GPIO pin.
 *
 ****************************************************************************/

bool gd32_gpioread(uint32_t pinset)
{
  struct gd32_gpio_s gpio;
  uint32_t regaddr;
  uint32_t regval;

  if (gd32_decodegpio(pinset, &gpio) != OK)
    {
      return false;
    }

  if ((pinset & GPIO_CFG_MODE_MASK) == GPIO_CFG_MODE_OUTPUT)
    {
      regaddr = gpio.gpio_periph + GD32H7_GPIO_OCTL_OFFSET;
    }
  else
    {
      regaddr = gpio.gpio_periph + GD32H7_GPIO_ISTAT_OFFSET;
    }

  regval = getreg32(regaddr);
  return (regval & (1 << gpio.pin)) != 0;
}

/****************************************************************************
 * Name: gd32_gpiosetevent
 *
 * Description:
 *   Sets/clears GPIO based event and interrupt triggers.
 *
 * Input Parameters:
 *  - pinset:      GPIO pin configuration
 *  - risingedge:  Enables interrupt on rising edges
 *  - fallingedge: Enables interrupt on falling edges
 *  - event:       Generate event when set
 *  - func:        When non-NULL, generate interrupt
 *  - arg:         Argument passed to the interrupt callback
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure indicating the
 *   nature of the failure.
 *
 ****************************************************************************/

int gd32_gpiosetevent(uint32_t pinset, bool risingedge, bool fallingedge,
                      bool event, xcpt_t func, void *arg)
{
  uint8_t pin;
  uint8_t trig;
  int ret;

  pin = (pinset & GPIO_CFG_PIN_MASK) >> GPIO_CFG_PIN_SHIFT;
  if (pin >= GD32_GPIO_NPINS)
    {
      return -EINVAL;
    }

  /* Configure the pin as an input (with EXTI capability) */

  ret = gd32_configgpio(pinset | GPIO_CFG_EXTI);
  if (ret < 0)
    {
      return ret;
    }

  /* Select the trigger mode */

  if (risingedge && fallingedge)
    {
      trig = GD32H7_EXTI_TRIG_BOTH;
    }
  else if (risingedge)
    {
      trig = GD32H7_EXTI_TRIG_RISING;
    }
  else if (fallingedge)
    {
      trig = GD32H7_EXTI_TRIG_FALLING;
    }
  else
    {
      trig = GD32H7_EXTI_TRIG_NONE;
    }

  gd32_exti_init(pin, event ? GD32H7_EXTI_EVENT : GD32H7_EXTI_INTERRUPT, trig);

  /* Attach (or detach) the handler */

  if (func != NULL)
    {
      ret = gd32_gpioexti_attach(pinset, func, arg);
      if (ret < 0)
        {
          return ret;
        }
    }
  else
    {
      gd32_exti_interrupt_disable(pin);
      gd32_gpioexti_attach(pinset, NULL, NULL);
    }

  return OK;
}
