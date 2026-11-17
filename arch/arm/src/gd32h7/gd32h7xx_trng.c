/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_trng.c
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
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/mutex.h>
#include <nuttx/fs/fs.h>
#include <nuttx/drivers/drivers.h>

#include "arm_internal.h"

#include "hardware/gd32h7xx_trng.h"
#include "gd32h7xx_rcu.h"

#ifdef CONFIG_GD32H7_TRNG

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define TRNG_TIMEOUT      1000000

/****************************************************************************
 * Private Data
 ****************************************************************************/

#if defined(CONFIG_DEV_RANDOM) || defined(CONFIG_DEV_URANDOM_ARCH)
static mutex_t g_trnglock = NXMUTEX_INITIALIZER;
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_trng_initialize
 ****************************************************************************/

void gd32_trng_initialize(void)
{
  uint32_t regval;

  gd32_rcu_periph_clock_enable(RCU_TRNG_EN);

  /* Disable clock error detection (the TRNG kernel clock is derived from
   * the AHB clock in this configuration) and enable the generator.
   */

  regval = getreg32(GD32H7_TRNG_CTL);
  regval &= ~TRNG_CTL_CED;
  regval |=  TRNG_CTL_TRNGEN;
  putreg32(regval, GD32H7_TRNG_CTL);
}

/****************************************************************************
 * Name: gd32_trng_read32
 *
 * Description:
 *   Block until one 32-bit random word is available.
 *
 ****************************************************************************/

static int gd32_trng_read32(FAR uint32_t *value)
{
  volatile uint32_t timeout = TRNG_TIMEOUT;
  uint32_t regval;

  while (timeout-- > 0)
    {
      regval = getreg32(GD32H7_TRNG_STAT);

      if ((regval & TRNG_STAT_SECS) != 0)
        {
          /* Seed error: re-initialize the generator */

          uint32_t ctl = getreg32(GD32H7_TRNG_CTL);
          putreg32(ctl & ~TRNG_CTL_TRNGEN, GD32H7_TRNG_CTL);
          putreg32(ctl, GD32H7_TRNG_CTL);
          return -EIO;
        }

      if ((regval & TRNG_STAT_DRDY) != 0)
        {
          *value = getreg32(GD32H7_TRNG_DATA);
          return OK;
        }
    }

  return -ETIMEDOUT;
}

/****************************************************************************
 * Name: gd32_trng_get32
 *
 * Description:
 *   Public helper returning one 32-bit random word.  Available even when
 *   the /dev/random character driver is not configured.
 *
 ****************************************************************************/

int gd32_trng_get32(FAR uint32_t *value)
{
  return gd32_trng_read32(value);
}

#if defined(CONFIG_DEV_RANDOM) || defined(CONFIG_DEV_URANDOM_ARCH)

/****************************************************************************
 * Name: gd32_trng_read
 ****************************************************************************/

static ssize_t gd32_trng_read(FAR struct file *filep, FAR char *buffer,
                              size_t buflen)
{
  size_t remaining = buflen;
  FAR uint8_t *ptr = (FAR uint8_t *)buffer;
  uint32_t word;
  int ret;

  ret = nxmutex_lock(&g_trnglock);
  if (ret < 0)
    {
      return ret;
    }

  while (remaining > 0)
    {
      size_t chunk;

      ret = gd32_trng_read32(&word);
      if (ret < 0)
        {
          nxmutex_unlock(&g_trnglock);
          return ret;
        }

      chunk = remaining < sizeof(word) ? remaining : sizeof(word);
      memcpy(ptr, &word, chunk);
      ptr       += chunk;
      remaining -= chunk;
    }

  nxmutex_unlock(&g_trnglock);
  return (ssize_t)buflen;
}

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct file_operations g_trngops =
{
  NULL,          /* open */
  NULL,          /* close */
  gd32_trng_read /* read */
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#ifdef CONFIG_DEV_RANDOM
void devrandom_register(void)
{
  gd32_trng_initialize();
  register_driver("/dev/random", &g_trngops, 0444, NULL);
}
#endif

#ifdef CONFIG_DEV_URANDOM_ARCH
void devurandom_register(void)
{
#ifndef CONFIG_DEV_RANDOM
  gd32_trng_initialize();
#endif
  register_driver("/dev/urandom", &g_trngops, 0444, NULL);
}
#endif

#endif /* CONFIG_DEV_RANDOM || CONFIG_DEV_URANDOM_ARCH */

#endif /* CONFIG_GD32H7_TRNG */
