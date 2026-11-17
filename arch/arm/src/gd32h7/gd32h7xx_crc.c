/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_crc.c
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

#include <nuttx/irq.h>
#include <nuttx/arch.h>

#include "arm_internal.h"

#include "gd32h7xx_crc.h"
#include "gd32h7xx_rcu.h"

#ifdef CONFIG_GD32H7_CRC

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_crc_initialize
 *
 * Description:
 *   Enable the CRC peripheral clock and reset the calculator to the
 *   default configuration (32-bit polynomial, no bit reflection).
 *
 ****************************************************************************/

void gd32_crc_initialize(void)
{
  gd32_rcu_periph_clock_enable(RCU_CRC_EN);

  putreg32(CRC_CTL_RST, GD32H7_CRC_CTL);
  putreg32(CRC_CTL_PS_32BIT | CRC_CTL_REV_I_NONE, GD32H7_CRC_CTL);
}

/****************************************************************************
 * Name: gd32_crc_reset
 ****************************************************************************/

void gd32_crc_reset(void)
{
  putreg32(CRC_CTL_RST, GD32H7_CRC_CTL);
}

/****************************************************************************
 * Name: gd32_crc_accumulate_word
 *
 * Description:
 *   Feed one 32-bit word into the CRC calculator and return the current
 *   result.
 *
 ****************************************************************************/

uint32_t gd32_crc_accumulate_word(uint32_t word)
{
  putreg32(word, GD32H7_CRC_DATA);
  return getreg32(GD32H7_CRC_DATA);
}

/****************************************************************************
 * Name: gd32_crc_accumulate_buffer
 *
 * Description:
 *   Feed a buffer of 32-bit words into the CRC calculator.  Any trailing
 *   bytes are packed little-endian into a final word.
 *
 ****************************************************************************/

uint32_t gd32_crc_accumulate_buffer(FAR const void *buffer, size_t nbytes)
{
  FAR const uint8_t *ptr = (FAR const uint8_t *)buffer;
  uint32_t word;
  size_t   rem;
  int      i;

  while (nbytes >= 4)
    {
      word = (uint32_t)ptr[0] |
             ((uint32_t)ptr[1] << 8) |
             ((uint32_t)ptr[2] << 16) |
             ((uint32_t)ptr[3] << 24);

      putreg32(word, GD32H7_CRC_DATA);
      ptr   += 4;
      nbytes -= 4;
    }

  rem = nbytes;
  if (rem > 0)
    {
      word = 0;
      for (i = 0; i < (int)rem; i++)
        {
          word |= (uint32_t)ptr[i] << (8 * i);
        }

      putreg32(word, GD32H7_CRC_DATA);
    }

  return getreg32(GD32H7_CRC_DATA);
}

#endif /* CONFIG_GD32H7_CRC */
