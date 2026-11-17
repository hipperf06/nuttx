/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_exti.c
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
#include "nvic.h"

#include "chip.h"
#include "gd32h7xx.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* The GPIO EXTI lines 0-15 are grouped into three NVIC vectors:
 *
 *   EXTI0:     lines 0
 *   EXTI1:     lines 1
 *   EXTI2:     lines 2
 *   EXTI3:     lines 3
 *   EXTI4:     lines 4
 *   EXTI5_9:   lines 5-9
 *   EXTI10_15: lines 10-15
 */

#define EXTI_NLINES  16

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* State of one GPIO EXTI line */

struct gd32_exti_s
{
  xcpt_t handler;             /* User interrupt handler */
  void *arg;                  /* Handler argument */
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* Registered handlers, one per GPIO EXTI line */

static struct gd32_exti_s g_exti_handlers[EXTI_NLINES];

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_exti_line_of_pin
 *
 * Description:
 *   Return the EXTI line encoding for a GPIO pin (0-15).
 *
 ****************************************************************************/

static inline uint32_t gd32_exti_line_of_pin(uint8_t pin)
{
  return GD32H7_EXTI_LINE(GD32H7_EXTI_GROUP0_OFFSET, pin);
}

/****************************************************************************
 * Name: gd32_exti_irq_of_pin
 *
 * Description:
 *   Return the IRQ number that services a GPIO EXTI line.
 *
 ****************************************************************************/

static inline int gd32_exti_irq_of_pin(uint8_t pin)
{
  switch (pin)
    {
      case 0:
        return GD32_IRQ_EXTI0;
      case 1:
        return GD32_IRQ_EXTI1;
      case 2:
        return GD32_IRQ_EXTI2;
      case 3:
        return GD32_IRQ_EXTI3;
      case 4:
        return GD32_IRQ_EXTI4;
      default:
        if (pin <= 9)
          {
            return GD32_IRQ_EXTI5_9;
          }
        else
          {
            return GD32_IRQ_EXTI10_15;
          }
    }
}

/****************************************************************************
 * Name: gd32_exti_dispatch
 *
 * Description:
 *   Dispatch all pending GPIO EXTI lines that share a NVIC vector.
 *
 ****************************************************************************/

static int gd32_exti_dispatch(int irq, void *context, void *arg)
{
  uint32_t min;
  uint32_t max;
  uint32_t line;
  uint32_t regval;

  switch (irq)
    {
      case GD32_IRQ_EXTI0:
        min = 0;
        max = 0;
        break;
      case GD32_IRQ_EXTI1:
        min = 1;
        max = 1;
        break;
      case GD32_IRQ_EXTI2:
        min = 2;
        max = 2;
        break;
      case GD32_IRQ_EXTI3:
        min = 3;
        max = 3;
        break;
      case GD32_IRQ_EXTI4:
        min = 4;
        max = 4;
        break;
      case GD32_IRQ_EXTI5_9:
        min = 5;
        max = 9;
        break;
      default: /* GD32_IRQ_EXTI10_15 */
        min = 10;
        max = 15;
        break;
    }

  for (line = min; line <= max; line++)
    {
      if ((getreg32(GD32H7_EXTI_PD(GD32H7_EXTI_GROUP0_OFFSET)) &
           (1 << line)) != 0)
        {
          struct gd32_exti_s *exti = &g_exti_handlers[line];

          xcpt_t handler = exti->handler;

          /* Clear the pending flag first */

          regval = getreg32(GD32H7_EXTI_PD(GD32H7_EXTI_GROUP0_OFFSET));
          regval |= (1 << line);
          putreg32(regval, GD32H7_EXTI_PD(GD32H7_EXTI_GROUP0_OFFSET));

          /* Then dispatch the handler */

          if (handler != NULL)
            {
              handler(irq, context, exti->arg);
            }
        }
    }

  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_exti_init
 *
 * Description:
 *   Initialize one EXTI line (interrupt or event, with trigger mode).
 *
 ****************************************************************************/

void gd32_exti_init(uint32_t linex, uint8_t exti_mode, uint8_t trig_type)
{
  uint32_t group = GD32H7_EXTI_GROUP(linex);
  uint32_t bit   = 1U << (linex & 0x1f);
  uint32_t regval;

  /* Reset the trigger configuration of the line */

  modifyreg32(GD32H7_EXTI_RTEN(group), bit, 0);
  modifyreg32(GD32H7_EXTI_FTEN(group), bit, 0);

  /* Configure the trigger mode */

  if (trig_type == GD32H7_EXTI_TRIG_RISING ||
      trig_type == GD32H7_EXTI_TRIG_BOTH)
    {
      modifyreg32(GD32H7_EXTI_RTEN(group), 0, bit);
    }

  if (trig_type == GD32H7_EXTI_TRIG_FALLING ||
      trig_type == GD32H7_EXTI_TRIG_BOTH)
    {
      modifyreg32(GD32H7_EXTI_FTEN(group), 0, bit);
    }

  /* Configure the interrupt/event mode */

  if (exti_mode == GD32H7_EXTI_INTERRUPT)
    {
      modifyreg32(GD32H7_EXTI_EVEN(group), bit, 0);
      modifyreg32(GD32H7_EXTI_INTEN(group), 0, bit);
    }
  else /* GD32H7_EXTI_EVENT */
    {
      modifyreg32(GD32H7_EXTI_INTEN(group), bit, 0);
      modifyreg32(GD32H7_EXTI_EVEN(group), 0, bit);
    }

  /* Clear any pending flag of the line */

  regval = getreg32(GD32H7_EXTI_PD(group));
  regval |= bit;
  putreg32(regval, GD32H7_EXTI_PD(group));
}

/****************************************************************************
 * Name: gd32_exti_interrupt_enable / gd32_exti_interrupt_disable
 *
 ****************************************************************************/

void gd32_exti_interrupt_enable(uint32_t linex)
{
  modifyreg32(GD32H7_EXTI_INTEN(GD32H7_EXTI_GROUP(linex)), 0,
              GD32H7_EXTI_BIT(linex));
}

void gd32_exti_interrupt_disable(uint32_t linex)
{
  modifyreg32(GD32H7_EXTI_INTEN(GD32H7_EXTI_GROUP(linex)),
              GD32H7_EXTI_BIT(linex), 0);
}

/****************************************************************************
 * Name: gd32_exti_event_enable / gd32_exti_event_disable
 *
 ****************************************************************************/

void gd32_exti_event_enable(uint32_t linex)
{
  modifyreg32(GD32H7_EXTI_EVEN(GD32H7_EXTI_GROUP(linex)), 0,
              GD32H7_EXTI_BIT(linex));
}

void gd32_exti_event_disable(uint32_t linex)
{
  modifyreg32(GD32H7_EXTI_EVEN(GD32H7_EXTI_GROUP(linex)),
              GD32H7_EXTI_BIT(linex), 0);
}

/****************************************************************************
 * Name: gd32_exti_software_interrupt_enable / _disable
 *
 ****************************************************************************/

void gd32_exti_software_interrupt_enable(uint32_t linex)
{
  modifyreg32(GD32H7_EXTI_SWIEV(GD32H7_EXTI_GROUP(linex)), 0,
              GD32H7_EXTI_BIT(linex));
}

void gd32_exti_software_interrupt_disable(uint32_t linex)
{
  modifyreg32(GD32H7_EXTI_SWIEV(GD32H7_EXTI_GROUP(linex)),
              GD32H7_EXTI_BIT(linex), 0);
}

/****************************************************************************
 * Name: gd32_exti_flag_get / gd32_exti_flag_clear
 *
 ****************************************************************************/

bool gd32_exti_flag_get(uint32_t linex)
{
  return (getreg32(GD32H7_EXTI_PD(GD32H7_EXTI_GROUP(linex))) &
          GD32H7_EXTI_BIT(linex)) != 0;
}

void gd32_exti_flag_clear(uint32_t linex)
{
  uint32_t regaddr = GD32H7_EXTI_PD(GD32H7_EXTI_GROUP(linex));
  uint32_t regval  = getreg32(regaddr);

  regval |= GD32H7_EXTI_BIT(linex);
  putreg32(regval, regaddr);
}

/****************************************************************************
 * Name: gd32_gpioexti_attach
 *
 * Description:
 *   Attach an interrupt handler to a GPIO EXTI interrupt.  The cfgset
 *   argument is a bit-encoded GPIO configuration; the pin must be marked
 *   with GPIO_CFG_EXTI and configured as input.
 *
 ****************************************************************************/

int gd32_gpioexti_attach(uint32_t cfgset, xcpt_t irqhandler, void *arg)
{
  uint8_t pin;
  uint8_t port;
  int irq;
  int ret;

  pin  = (cfgset & GPIO_CFG_PIN_MASK) >> GPIO_CFG_PIN_SHIFT;
  port = (cfgset & GPIO_CFG_PORT_MASK) >> GPIO_CFG_PORT_SHIFT;

  if (pin >= EXTI_NLINES || port >= GD32_NGPIO_PORTS)
    {
      return -EINVAL;
    }

  /* Select the GPIO port as the EXTI line source */

  switch (port)
    {
      case 0:
        gd32_syscfg_exti_line_config(SYSCFG_EXTI_SOURCE_GPIOA, pin);
        break;
      case 1:
        gd32_syscfg_exti_line_config(SYSCFG_EXTI_SOURCE_GPIOB, pin);
        break;
      case 2:
        gd32_syscfg_exti_line_config(SYSCFG_EXTI_SOURCE_GPIOC, pin);
        break;
      case 3:
        gd32_syscfg_exti_line_config(SYSCFG_EXTI_SOURCE_GPIOD, pin);
        break;
      case 4:
        gd32_syscfg_exti_line_config(SYSCFG_EXTI_SOURCE_GPIOE, pin);
        break;
      case 5:
        gd32_syscfg_exti_line_config(SYSCFG_EXTI_SOURCE_GPIOF, pin);
        break;
      case 6:
        gd32_syscfg_exti_line_config(SYSCFG_EXTI_SOURCE_GPIOG, pin);
        break;
      case 7:
        gd32_syscfg_exti_line_config(SYSCFG_EXTI_SOURCE_GPIOH, pin);
        break;
      case 8:
        gd32_syscfg_exti_line_config(SYSCFG_EXTI_SOURCE_GPIOJ, pin);
        break;
      case 9:
        gd32_syscfg_exti_line_config(SYSCFG_EXTI_SOURCE_GPIOK, pin);
        break;
      default:
        return -EINVAL;
    }

  /* Register the handler */

  g_exti_handlers[pin].handler = irqhandler;
  g_exti_handlers[pin].arg     = arg;

  /* Attach the shared dispatch handler to the NVIC vector */

  irq = gd32_exti_irq_of_pin(pin);

  ret = irq_attach(irq, gd32_exti_dispatch, NULL);
  if (ret < 0)
    {
      return ret;
    }

  up_enable_irq(irq);
  return OK;
}

/****************************************************************************
 * Name: gd32_gpioexti_detach
 *
 * Description:
 *   Detach the interrupt handler from a GPIO EXTI interrupt and disable
 *   the EXTI line.
 *
 ****************************************************************************/

void gd32_gpioexti_detach(uint32_t cfgset)
{
  uint8_t pin;
  uint8_t port;

  pin  = (cfgset & GPIO_CFG_PIN_MASK) >> GPIO_CFG_PIN_SHIFT;
  port = (cfgset & GPIO_CFG_PORT_MASK) >> GPIO_CFG_PORT_SHIFT;

  if (pin >= EXTI_NLINES || port >= GD32_NGPIO_PORTS)
    {
      return;
    }

  gd32_exti_interrupt_disable(gd32_exti_line_of_pin(pin));

  g_exti_handlers[pin].handler = NULL;
  g_exti_handlers[pin].arg     = NULL;
}
