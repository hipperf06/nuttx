/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_allocateheap.c
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
#include <string.h>
#include <assert.h>
#include <debug.h>

#include <nuttx/arch.h>
#include <nuttx/board.h>
#include <nuttx/kmalloc.h>
#include <nuttx/userspace.h>

#include <arch/board/board.h>

#include "chip.h"

#include "mpu.h"
#include "arm_internal.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* The GD32H7xx contains:
 *
 *  - 512KiB AXI SRAM at 0x24000000 (used for .data/.bss and the primary
 *    heap),
 *  - 64KiB DTCM RAM at 0x20000000 (only accessible through the core D-bus,
 *    not usable for DMA), and
 *  - additional SRAM0-3 at 0x30000000.
 *
 * The primary RAM region must be provided in the configuration (the usual
 * NuttX convention, set in the defconfig):
 *
 *   CONFIG_RAM_START :  Start address of the primary RAM (0x24000000)
 *   CONFIG_RAM_SIZE  :  Size of the primary RAM in bytes
 *
 * The DTCM is different to SRAM, and it can be accessed only by the data
 * bus of the Cortex-M7 core, and can not be used for DMA.  When DMA is
 * used, then the following should be defined to exclude DTCM from the heap:
 *
 * CONFIG_GD32H7_DTCMEXCLUDE  : Exclude DTCM from the HEAP
 *
 * Otherwise DTCM can be added as a second heap region by defining:
 *
 * CONFIG_GD32H7_DTCMHEAP     : Add DTCM to the heap as a second region.
 *
 * Make sure that CONFIG_MM_REGIONS matches the number of regions used.
 */

#if defined(CONFIG_RAM_START) && defined(CONFIG_RAM_SIZE)
#  define PRIMARY_RAM_START  CONFIG_RAM_START
#  define PRIMARY_RAM_END    (CONFIG_RAM_START + CONFIG_RAM_SIZE)
#else
#  error "CONFIG_RAM_START and CONFIG_RAM_SIZE must be provided in the defconfig"
#  define PRIMARY_RAM_START  GD32H7_AXISRAM_BASE
#  define PRIMARY_RAM_END    (GD32H7_AXISRAM_BASE + GD32H7_AXISRAM_SIZE)
#endif

#ifdef CONFIG_GD32H7_DTCMHEAP
#  undef CONFIG_GD32H7_DTCMEXCLUDE
#endif

#if !defined(CONFIG_GD32H7_DTCMHEAP) && !defined(CONFIG_GD32H7_DTCMEXCLUDE)
#  define CONFIG_GD32H7_DTCMEXCLUDE 1
#endif

#if defined(CONFIG_GD32H7_DTCMHEAP) && defined(CONFIG_ARCH_DMA)
#  error "DTCM is included in the heap AND DMA is enabled"
#endif

#if defined(CONFIG_GD32H7_DTCMHEAP) && (CONFIG_MM_REGIONS < 2)
#  error "CONFIG_MM_REGIONS must be at least 2 when CONFIG_GD32H7_DTCMHEAP is set"
#endif

#ifndef CONFIG_MM_REGIONS
#  define CONFIG_MM_REGIONS 1
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_heap_color
 *
 * Description:
 *   Set heap memory to a known, non-zero state to checking heap usage.
 *
 ****************************************************************************/

#ifdef CONFIG_HEAP_COLORATION
static inline void up_heap_color(void *start, size_t size)
{
  memset(start, HEAP_COLOR, size);
}
#else
#  define up_heap_color(start, size)
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: up_allocate_heap
 *
 * Description:
 *   This function will be called to dynamically set aside the heap region.
 *
 *   For the case of the flat build, the "heap" is the system RAM that is
 *   not used by .data/.bss/stack (starting at g_idle_topstack) up to
 *   PRIMARY_RAM_END.
 *
 *   For the case of the kernel build, the "heap" is the system RAM that is
 *   not used by .data/.bss/stack (starting at g_idle_topstack) up to
 *   PRIMARY_RAM_END.
 *
 *   If a protected build is provided, the heap is the "user heap":  The
 *   system heap lies in a separate region.
 *
 ****************************************************************************/

void up_allocate_heap(FAR void **heap_start, size_t *heap_size)
{
#if defined(CONFIG_BUILD_PROTECTED) && defined(CONFIG_MM_KERNEL_HEAP)
  /* Get the (unaligned) addresses of the user heap region */

  uintptr_t uheap_start = (uintptr_t)_euserheap;
  uintptr_t uheap_end   = PRIMARY_RAM_END;
  size_t    uheap_size;

  /* Adjust the size of the user heap to account for alignment */

  uheap_start = (uheap_start + 7) & ~7;
  uheap_end   = uheap_end & ~7;
  uheap_size  = uheap_end - uheap_start;

  /* Return the user heap region */

  *heap_start = (FAR void *)uheap_start;
  *heap_size  = uheap_size;
#else
  /* Get the (unaligned) addresses of the heap region */

  uintptr_t heapbase = g_idle_topstack;
  uintptr_t heapend  = PRIMARY_RAM_END;
  size_t    heapsize;

  /* Adjust the size of the heap to account for alignment */

  heapbase = (heapbase + 7) & ~7;
  heapend  = heapend & ~7;
  heapsize = heapend - heapbase;

  up_heap_color((FAR void *)heapbase, heapsize);

  /* Return the heap region */

  *heap_start = (FAR void *)heapbase;
  *heap_size  = heapsize;
#endif
}

/****************************************************************************
 * Name: up_addregion
 *
 * Description:
 *   Add additional heap regions.
 *
 ****************************************************************************/

#if CONFIG_MM_REGIONS > 1
void up_addregion(void)
{
#ifdef CONFIG_GD32H7_DTCMHEAP
  /* Add the DTCM RAM region to the heap.
   *
   * NOTE: The DTCM can only be accessed through the Cortex-M7 D-bus, so
   * this region may not be used for DMA.
   */

  uintptr_t dtcm_start = (GD32H7_DTCM_BASE + 7) & ~7;
  uintptr_t dtcm_end   = (GD32H7_DTCM_BASE + GD32H7_DTCM_SIZE) & ~7;

  up_heap_color((FAR void *)dtcm_start, dtcm_end - dtcm_start);
  kumm_addregion((FAR void *)dtcm_start, dtcm_end - dtcm_start);
#endif
}
#endif
