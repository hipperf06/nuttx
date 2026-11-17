/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_fmc.c
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
#include <errno.h>
#include <string.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>

#include "arm_internal.h"
#include "nvic.h"

#include "hardware/gd32h7xx_fmc.h"
#include "gd32h7xx_fmc.h"

#ifdef CONFIG_GD32H7_FMC

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define FMC_TIMEOUT       0x00100000

#define FLASH_BASE        GD32H7_FLASH_BASE
#define FLASH_SIZE        GD32H7_FLASH_SIZE
#define FLASH_END         (FLASH_BASE + FLASH_SIZE)
#define SECTOR_SIZE       GD32H7_FLASH_SECTOR_SIZE
#define NSECTORS          (FLASH_SIZE / SECTOR_SIZE)

/* The erased state of the flash (read-back value of erased bytes) */

#define ERASED_WORD       0xffffffff

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_fmc_ready_wait
 ****************************************************************************/

static int gd32_fmc_ready_wait(void)
{
  volatile uint32_t timeout = FMC_TIMEOUT;
  uint32_t stat;

  while (timeout-- > 0)
    {
      stat = getreg32(GD32H7_FMC_STAT);

      if ((stat & FMC_STAT_BUSY) == 0)
        {
          if (stat & FMC_STAT_ERRORS)
            {
              /* Clear the error flags */

              putreg32(FMC_STAT_ERRORS, GD32H7_FMC_STAT);
              return -EIO;
            }

          return OK;
        }
    }

  return -ETIMEDOUT;
}

/****************************************************************************
 * Name: gd32_fmc_unlock / gd32_fmc_lock
 ****************************************************************************/

static int gd32_fmc_unlock(void)
{
  uint32_t regval;

  regval = getreg32(GD32H7_FMC_CTL);
  if ((regval & FMC_CTL_LK) == 0)
    {
      return OK;
    }

  putreg32(FMC_KEY1, GD32H7_FMC_KEY);
  putreg32(FMC_KEY2, GD32H7_FMC_KEY);

  regval = getreg32(GD32H7_FMC_CTL);
  if (regval & FMC_CTL_LK)
    {
      return -EACCES;
    }

  return OK;
}

static void gd32_fmc_lock(void)
{
  modifyreg32(GD32H7_FMC_CTL, 0, FMC_CTL_LK);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_progmem_neraseblocks
 ****************************************************************************/

size_t up_progmem_neraseblocks(void)
{
  return NSECTORS;
}

/****************************************************************************
 * Name: up_progmem_isuniform
 ****************************************************************************/

bool up_progmem_isuniform(void)
{
  return true;
}

/****************************************************************************
 * Name: up_progmem_pagesize
 ****************************************************************************/

size_t up_progmem_pagesize(size_t page)
{
  return SECTOR_SIZE;
}

/****************************************************************************
 * Name: up_progmem_erasesize
 ****************************************************************************/

size_t up_progmem_erasesize(size_t block)
{
  return SECTOR_SIZE;
}

/****************************************************************************
 * Name: up_progmem_getpage
 ****************************************************************************/

ssize_t up_progmem_getpage(size_t addr)
{
  if (addr < FLASH_BASE || addr >= FLASH_END)
    {
      return -EFAULT;
    }

  return (addr - FLASH_BASE) / SECTOR_SIZE;
}

/****************************************************************************
 * Name: up_progmem_getaddress
 ****************************************************************************/

size_t up_progmem_getaddress(size_t page)
{
  if (page >= NSECTORS)
    {
      return SIZE_MAX;
    }

  return FLASH_BASE + page * SECTOR_SIZE;
}

/****************************************************************************
 * Name: up_progmem_eraseblock
 *
 * Description:
 *   Erase one flash sector.
 *
 ****************************************************************************/

ssize_t up_progmem_eraseblock(size_t block)
{
  int ret;

  if (block >= NSECTORS)
    {
      return -EFAULT;
    }

  ret = gd32_fmc_unlock();
  if (ret < 0)
    {
      return ret;
    }

  /* Program the sector erase operation */

  modifyreg32(GD32H7_FMC_CTL, 0, FMC_CTL_SER);
  putreg32(FLASH_BASE + block * SECTOR_SIZE, GD32H7_FMC_ADDR);
  modifyreg32(GD32H7_FMC_CTL, 0, FMC_CTL_START);

  ret = gd32_fmc_ready_wait();
  modifyreg32(GD32H7_FMC_CTL, FMC_CTL_SER | FMC_CTL_START, 0);

  gd32_fmc_lock();

  if (ret < 0)
    {
      return ret;
    }

  return SECTOR_SIZE;
}

/****************************************************************************
 * Name: up_progmem_ispageerased
 ****************************************************************************/

ssize_t up_progmem_ispageerased(size_t page)
{
  FAR const uint32_t *ptr;
  size_t count;
  size_t bwritten = 0;

  if (page >= NSECTORS)
    {
      return -EFAULT;
    }

  ptr   = (FAR const uint32_t *)(FLASH_BASE + page * SECTOR_SIZE);
  count = SECTOR_SIZE / sizeof(uint32_t);

  for (; count > 0; count--)
    {
      uint32_t word = *ptr++;

      if (word != ERASED_WORD)
        {
          if (word & 0x0000ffff)  bwritten += 2;
          if (word & 0xffff0000)  bwritten += 2;
          if ((word & 0x0000ffff) != 0x0000ffff) bwritten--;
          if ((word & 0xffff0000) != 0xffff0000) bwritten--;
        }
    }

  return (ssize_t)bwritten;
}

/****************************************************************************
 * Name: up_progmem_write
 *
 * Description:
 *   Write a buffer into flash.  The destination area must be erased.  The
 *   flash is programmed word by word; the address and length are padded to
 *   4-byte alignment.
 *
 ****************************************************************************/

ssize_t up_progmem_write(size_t addr, FAR const void *buf, size_t count)
{
  FAR const uint8_t *src = (FAR const uint8_t *)buf;
  size_t written = 0;
  uint32_t word;
  int ret;
  int i;

  if (addr < FLASH_BASE || addr + count > FLASH_END)
    {
      return -EFAULT;
    }

  ret = gd32_fmc_unlock();
  if (ret < 0)
    {
      return ret;
    }

  /* Handle an unaligned leading fragment by read-modify-write */

  while (((addr + written) & 3) != 0 && written < count)
    {
      size_t a = (addr + written) & ~3u;
      uint32_t *dst = (uint32_t *)a;
      uint32_t old = *dst;
      uint32_t shift = (addr + written) & 3;

      word = old;
      word &= ~(0xffu << (8 * shift));
      word |= (uint32_t)src[written] << (8 * shift);

      modifyreg32(GD32H7_FMC_CTL, 0, FMC_CTL_PG);
      UP_DMB();
      UP_DSB();
      *dst = word;
      UP_DMB();
      UP_DSB();
      ret = gd32_fmc_ready_wait();
      modifyreg32(GD32H7_FMC_CTL, FMC_CTL_PG, 0);

      if (ret < 0)
        {
          goto errout;
        }

      written++;
    }

  /* Program full words */

  while (count - written >= 4)
    {
      word = (uint32_t)src[written] |
             ((uint32_t)src[written + 1] << 8) |
             ((uint32_t)src[written + 2] << 16) |
             ((uint32_t)src[written + 3] << 24);

      modifyreg32(GD32H7_FMC_CTL, 0, FMC_CTL_PG);
      UP_DMB();
      UP_DSB();
      *(uint32_t *)(addr + written) = word;
      UP_DMB();
      UP_DSB();
      ret = gd32_fmc_ready_wait();
      modifyreg32(GD32H7_FMC_CTL, FMC_CTL_PG, 0);

      if (ret < 0)
        {
          goto errout;
        }

      written += 4;
    }

  /* Trailing fragment */

  if (written < count)
    {
      size_t a = addr + written;
      uint32_t *dst = (uint32_t *)(a & ~3u);
      uint32_t old = *dst;

      word = old;
      for (i = 0; written < count; i++, written++)
        {
          word &= ~(0xffu << (8 * i));
          word |= (uint32_t)src[written] << (8 * i);
        }

      modifyreg32(GD32H7_FMC_CTL, 0, FMC_CTL_PG);
      UP_DMB();
      UP_DSB();
      *dst = word;
      UP_DMB();
      UP_DSB();
      ret = gd32_fmc_ready_wait();
      modifyreg32(GD32H7_FMC_CTL, FMC_CTL_PG, 0);

      if (ret < 0)
        {
          goto errout;
        }
    }

errout:
  gd32_fmc_lock();
  return ret < 0 ? ret : (ssize_t)written;
}

/****************************************************************************
 * Name: up_progmem_read
 ****************************************************************************/

ssize_t up_progmem_read(size_t addr, FAR void *buf, size_t count)
{
  if (addr < FLASH_BASE || addr + count > FLASH_END)
    {
      return -EFAULT;
    }

  memcpy(buf, (FAR const void *)addr, count);
  return (ssize_t)count;
}

/****************************************************************************
 * Name: up_progmem_erasestate
 ****************************************************************************/

uint8_t up_progmem_erasestate(void)
{
  return 0xff;
}

#endif /* CONFIG_GD32H7_FMC */
