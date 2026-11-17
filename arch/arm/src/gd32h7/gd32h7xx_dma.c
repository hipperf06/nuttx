/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_dma.c
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

#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/semaphore.h>

#include "chip.h"
#include "gd32h7xx_dma.h"
#include "gd32h7xx.h"

#ifdef CONFIG_GD32H7_DMA

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define DMA_NCHANNELS_PER     (8)

#if defined(CONFIG_GD32H7_DMA0) && defined(CONFIG_GD32H7_DMA1)
#  define DMA_NCONTROLLERS    (2)
#elif defined(CONFIG_GD32H7_DMA0) || defined(CONFIG_GD32H7_DMA1)
#  define DMA_NCONTROLLERS    (1)
#else
#  error "CONFIG_GD32H7_DMA requires CONFIG_GD32H7_DMA0 or CONFIG_GD32H7_DMA1"
#endif

#define DMA_NCHANNELS         (DMA_NCHANNELS_PER * DMA_NCONTROLLERS)

/* Status bits reported to the DMA callback (uint16_t encoding) */

#define DMASTATUS_ERROR       (DMA_STATUS_ERROR)
#define DMASTATUS_SUCCESS     (DMA_STATUS_SUCCESS)

/****************************************************************************
 * Private Types
 ****************************************************************************/

/* This structure describes one DMA channel */

struct gd32_dma_channel_s
{
  uint8_t        chan_num;         /* DMA channel number (0-7) */
  uint8_t        irq;              /* DMA channel IRQ number */
  uint32_t       request;          /* DMAMUX request number */
  uint32_t       dmabase;          /* DMA base address */
  bool           allocated;        /* Channel allocation state */
  sem_t          chsem;            /* Wait for channel to become available */
  dma_callback_t callback;         /* Callback invoked when DMA completes */
  void           *arg;             /* Argument passed to callback */
};

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct gd32_dma_channel_s g_dmachan[DMA_NCHANNELS] =
{
#ifdef CONFIG_GD32H7_DMA0
  {
    .chan_num  = 0,
    .irq       = GD32_IRQ_DMA0_CHANNEL0,
    .dmabase   = GD32H7_DMA0,
    .chsem     = SEM_INITIALIZER(1),
  },
  {
    .chan_num  = 1,
    .irq       = GD32_IRQ_DMA0_CHANNEL1,
    .dmabase   = GD32H7_DMA0,
    .chsem     = SEM_INITIALIZER(1),
  },
  {
    .chan_num  = 2,
    .irq       = GD32_IRQ_DMA0_CHANNEL2,
    .dmabase   = GD32H7_DMA0,
    .chsem     = SEM_INITIALIZER(1),
  },
  {
    .chan_num  = 3,
    .irq       = GD32_IRQ_DMA0_CHANNEL3,
    .dmabase   = GD32H7_DMA0,
    .chsem     = SEM_INITIALIZER(1),
  },
  {
    .chan_num  = 4,
    .irq       = GD32_IRQ_DMA0_CHANNEL4,
    .dmabase   = GD32H7_DMA0,
    .chsem     = SEM_INITIALIZER(1),
  },
  {
    .chan_num  = 5,
    .irq       = GD32_IRQ_DMA0_CHANNEL5,
    .dmabase   = GD32H7_DMA0,
    .chsem     = SEM_INITIALIZER(1),
  },
  {
    .chan_num  = 6,
    .irq       = GD32_IRQ_DMA0_CHANNEL6,
    .dmabase   = GD32H7_DMA0,
    .chsem     = SEM_INITIALIZER(1),
  },
  {
    .chan_num  = 7,
    .irq       = GD32_IRQ_DMA0_CHANNEL7,
    .dmabase   = GD32H7_DMA0,
    .chsem     = SEM_INITIALIZER(1),
  },
#endif

#ifdef CONFIG_GD32H7_DMA1
  {
    .chan_num  = 0,
    .irq       = GD32_IRQ_DMA1_CHANNEL0,
    .dmabase   = GD32H7_DMA1,
    .chsem     = SEM_INITIALIZER(1),
  },
  {
    .chan_num  = 1,
    .irq       = GD32_IRQ_DMA1_CHANNEL1,
    .dmabase   = GD32H7_DMA1,
    .chsem     = SEM_INITIALIZER(1),
  },
  {
    .chan_num  = 2,
    .irq       = GD32_IRQ_DMA1_CHANNEL2,
    .dmabase   = GD32H7_DMA1,
    .chsem     = SEM_INITIALIZER(1),
  },
  {
    .chan_num  = 3,
    .irq       = GD32_IRQ_DMA1_CHANNEL3,
    .dmabase   = GD32H7_DMA1,
    .chsem     = SEM_INITIALIZER(1),
  },
  {
    .chan_num  = 4,
    .irq       = GD32_IRQ_DMA1_CHANNEL4,
    .dmabase   = GD32H7_DMA1,
    .chsem     = SEM_INITIALIZER(1),
  },
  {
    .chan_num  = 5,
    .irq       = GD32_IRQ_DMA1_CHANNEL5,
    .dmabase   = GD32H7_DMA1,
    .chsem     = SEM_INITIALIZER(1),
  },
  {
    .chan_num  = 6,
    .irq       = GD32_IRQ_DMA1_CHANNEL6,
    .dmabase   = GD32H7_DMA1,
    .chsem     = SEM_INITIALIZER(1),
  },
  {
    .chan_num  = 7,
    .irq       = GD32_IRQ_DMA1_CHANNEL7,
    .dmabase   = GD32H7_DMA1,
    .chsem     = SEM_INITIALIZER(1),
  },
#endif
};

/* The DMAMUX request multiplexer is a single block serving both DMA
 * controllers: DMA0 channels map to DMAMUX channels 0-7 and DMA1 channels
 * map to DMAMUX channels 8-15.
 */

#define DMAMUX_CH_INDEX(dmachan) \
  (((dmachan)->dmabase == GD32H7_DMA0 ? 0 : 8) + (dmachan)->chan_num)

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_dma_clock_enable
 *
 * Description:
 *   Enable the DMA controller clock.
 *
 ****************************************************************************/

static void gd32_dma_clock_enable(uint32_t dmabase)
{
  switch (dmabase)
    {
      default:
        return;

#ifdef CONFIG_GD32H7_DMA0
      case GD32H7_DMA0:
        gd32_rcu_periph_clock_enable(RCU_DMA0_EN);
        break;
#endif

#ifdef CONFIG_GD32H7_DMA1
      case GD32H7_DMA1:
        gd32_rcu_periph_clock_enable(RCU_DMA1_EN);
        break;
#endif
    }
}

/****************************************************************************
 * Name: gd32_dma_interrupt_flag_get
 *
 * Description:
 *   Get the packed interrupt status of one DMA channel.  Channels 0-4
 *   report their flags in INTF0, channels 5-7 in INTF1.
 *
 ****************************************************************************/

static uint8_t gd32_dma_interrupt_flag_get(uint32_t dmabase,
                                           uint8_t chan_num)
{
  uint32_t regval;
  uint32_t shift;

  if (chan_num < 5)
    {
      shift = GD32H7_DMA_FLAG_ADD(1, chan_num);
      regval = getreg32(GD32H7_DMA_INTF0(dmabase));
    }
  else
    {
      shift = GD32H7_DMA_FLAG_ADD(1, chan_num);
      regval = getreg32(GD32H7_DMA_INTF1(dmabase));
    }

  return (uint8_t)((regval >> shift) & DMA_INTF_MASK);
}

/****************************************************************************
 * Name: gd32_dma_interrupt_flag_clear
 *
 * Description:
 *   Clear the interrupt flags of one DMA channel.
 *
 ****************************************************************************/

static void gd32_dma_interrupt_flag_clear(uint32_t dmabase,
                                          uint8_t chan_num, uint8_t flags)
{
  uint32_t regval;

  if (flags == 0)
    {
      return;
    }

  if (chan_num < 5)
    {
      regval = GD32H7_DMA_FLAG_ADD(flags, chan_num);
      putreg32(regval, GD32H7_DMA_INTC0(dmabase));
    }
  else
    {
      regval = GD32H7_DMA_FLAG_ADD(flags, chan_num);
      putreg32(regval, GD32H7_DMA_INTC1(dmabase));
    }
}

/****************************************************************************
 * Name: gd32_dma_interrupt
 *
 * Description:
 *   DMA interrupt handler
 *
 ****************************************************************************/

static int gd32_dma_interrupt(int irq, void *context, void *arg)
{
  struct gd32_dma_channel_s *dmachan = (struct gd32_dma_channel_s *)arg;
  uint8_t status;

  DEBUGASSERT(dmachan != NULL);

  /* Get and clear the interrupt status for this channel */

  status = gd32_dma_interrupt_flag_get(dmachan->dmabase, dmachan->chan_num);
  if (status != 0)
    {
      gd32_dma_interrupt_flag_clear(dmachan->dmabase, dmachan->chan_num,
                                    status);

      /* Call the DMA callback */

      if (dmachan->callback)
        {
          dmachan->callback(dmachan, status, dmachan->arg);
        }
    }

  return OK;
}

/****************************************************************************
 * Name: gd32_channel_interrupt_disable
 *
 * Description:
 *   Disable the channel interrupts and clear pending flags.
 *
 ****************************************************************************/

static void gd32_channel_interrupt_disable(uint32_t dmabase,
                                           uint8_t chan_num)
{
  uint32_t regval;

  regval  = getreg32(GD32H7_DMA_CHCTL(dmabase, chan_num));
  regval &= ~(DMA_CHXCTL_SDEIE | DMA_CHXCTL_TAEIE |
              DMA_CHXCTL_HTFIE | DMA_CHXCTL_FTFIE);
  putreg32(regval, GD32H7_DMA_CHCTL(dmabase, chan_num));

  /* Also disable the FIFO exception interrupt in the FCTL register */

  regval  = getreg32(GD32H7_DMA_CHFCTL(dmabase, chan_num));
  regval &= ~DMA_CHXFCTL_FEEIE;
  putreg32(regval, GD32H7_DMA_CHFCTL(dmabase, chan_num));

  /* Clear pending flags */

  gd32_dma_interrupt_flag_clear(dmabase, chan_num, DMA_INTF_ALLFLAGS);
}

/****************************************************************************
 * Name: gd32_channel_enable / gd32_channel_disable
 *
 ****************************************************************************/

static void gd32_channel_enable(uint32_t dmabase, uint8_t chan_num)
{
  uint32_t regaddr = GD32H7_DMA_CHCTL(dmabase, chan_num);
  uint32_t regval  = getreg32(regaddr);

  regval |= DMA_CHXCTL_CHEN;
  putreg32(regval, regaddr);
}

static void gd32_channel_disable(uint32_t dmabase, uint8_t chan_num)
{
  uint32_t regaddr = GD32H7_DMA_CHCTL(dmabase, chan_num);
  uint32_t regval  = getreg32(regaddr);

  regval &= ~DMA_CHXCTL_CHEN;
  putreg32(regval, regaddr);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: arm_dma_initialize
 *
 * Description:
 *   Initialize the DMA subsystem
 *
 ****************************************************************************/

#ifdef CONFIG_ARCH_DMA
void weak_function arm_dma_initialize(void)
{
  struct gd32_dma_channel_s *dmachan;
  int i;

  dmainfo("Initialize DMA\n");

  for (i = 0; i < DMA_NCHANNELS; i++)
    {
      dmachan = &g_dmachan[i];

      /* Attach DMA interrupt vectors */

      irq_attach(dmachan->irq, gd32_dma_interrupt, dmachan);

      /* Disable the DMA channel and channel interrupt */

      gd32_channel_interrupt_disable(dmachan->dmabase, dmachan->chan_num);
      gd32_channel_disable(dmachan->dmabase, dmachan->chan_num);

      /* Enable the channel interrupts at the NVIC (still disabled at the
       * DMA controller).
       */

      up_enable_irq(dmachan->irq);
    }
}
#endif

/****************************************************************************
 * Name: gd32_dma_channel_alloc
 *
 * Description:
 *   Allocate a DMA channel.
 *
 ****************************************************************************/

DMA_HANDLE gd32_dma_channel_alloc(uint8_t periph_req)
{
  struct gd32_dma_channel_s *dmachan;
  uint8_t index;
  uint8_t subperiph;
  int ret;

  subperiph = (periph_req >> DMA_SUBPERI_SHIFT) & DMA_SUBPERI_MASK;
  index     = (periph_req >> DMA_CHANNEL_SHIFT) & DMA_CHANNEL_MASK;

  DEBUGASSERT(subperiph < DMA_NCONTROLLERS);
  DEBUGASSERT(index < DMA_NCHANNELS_PER);

#ifdef CONFIG_GD32H7_DMA0
  if (subperiph == 0)
    {
      dmachan = &g_dmachan[index];
    }
  else
#endif
#ifdef CONFIG_GD32H7_DMA1
  if (subperiph == 1)
    {
      dmachan = &g_dmachan[DMA_NCHANNELS_PER + index];
    }
  else
#endif
    {
      return NULL;
    }

  /* Get exclusive access to the DMA channel */

  ret = nxsem_wait_uninterruptible(&dmachan->chsem);
  if (ret < 0)
    {
      return NULL;
    }

  dmachan->allocated = true;
  return (DMA_HANDLE)dmachan;
}

/****************************************************************************
 * Name: gd32_dma_channel_free
 *
 * Description:
 *   Release a DMA channel.
 *
 ****************************************************************************/

void gd32_dma_channel_free(DMA_HANDLE handle)
{
  struct gd32_dma_channel_s *dmachan = (struct gd32_dma_channel_s *)handle;

  DEBUGASSERT(dmachan != NULL && dmachan->allocated);

  /* Disable the DMA channel and channel interrupt */

  gd32_channel_interrupt_disable(dmachan->dmabase, dmachan->chan_num);
  gd32_channel_disable(dmachan->dmabase, dmachan->chan_num);

  dmachan->allocated = false;

  /* Release the channel */

  nxsem_post(&dmachan->chsem);
}

/****************************************************************************
 * Name: gd32_dma_channel_request
 *
 * Description:
 *   Route a DMA channel to a peripheral request line through the DMAMUX.
 *
 ****************************************************************************/

void gd32_dma_channel_request(DMA_HANDLE handle, uint32_t request)
{
  struct gd32_dma_channel_s *dmachan = (struct gd32_dma_channel_s *)handle;
  uint32_t regaddr;
  uint32_t regval;

  DEBUGASSERT(dmachan != NULL);

  dmachan->request = request;

  /* Enable the DMAMUX clock */

  gd32_rcu_periph_clock_enable(RCU_DMAMUX_EN);

  regaddr = GD32H7_DMAMUX_RM_CHXCFG(DMAMUX_CH_INDEX(dmachan));

  regval  = getreg32(regaddr);
  regval &= ~DMAMUX_RM_CHXCFG_MUXID_MASK;
  regval |= DMAMUX_RM_CHXCFG_MUXID(request);
  putreg32(regval, regaddr);
}

/****************************************************************************
 * Name: gd32_dma_setup
 *
 * Description:
 *   Configure a DMA channel before starting a transfer.
 *
 ****************************************************************************/

void gd32_dma_setup(DMA_HANDLE handle,
                    FAR struct gd32_dma_param_s *param)
{
  struct gd32_dma_channel_s *dmachan = (struct gd32_dma_channel_s *)handle;
  uint32_t regaddr;
  uint32_t regval;

  DEBUGASSERT(dmachan != NULL && param != NULL);

  /* Enable the DMA controller clock */

  gd32_dma_clock_enable(dmachan->dmabase);

  /* Clear CHEN bit in the channel control register */

  regaddr = GD32H7_DMA_CHCTL(dmachan->dmabase, dmachan->chan_num);
  regval  = getreg32(regaddr);
  regval &= ~DMA_CHXCTL_CHEN;
  putreg32(regval, regaddr);

  /* Clear the channel interrupt flags */

  gd32_dma_interrupt_flag_clear(dmachan->dmabase, dmachan->chan_num,
                                DMA_INTF_ALLFLAGS);

  /* Select single data mode (FIFO disabled) */

  regaddr = GD32H7_DMA_CHFCTL(dmachan->dmabase, dmachan->chan_num);
  putreg32(0, regaddr);

  /* Configure peripheral base address */

  putreg32(param->periph_addr,
           GD32H7_DMA_CHPADDR(dmachan->dmabase, dmachan->chan_num));

  /* Configure memory base address */

  putreg32(param->memory0_addr,
           GD32H7_DMA_CHM0ADDR(dmachan->dmabase, dmachan->chan_num));

  /* Configure the number of data to be transferred */

  putreg32(param->number,
           GD32H7_DMA_CHCNT(dmachan->dmabase, dmachan->chan_num));

  /* Configure the channel control register */

  regaddr = GD32H7_DMA_CHCTL(dmachan->dmabase, dmachan->chan_num);
  regval  = getreg32(regaddr);
  regval &= ~(DMA_CHXCTL_TM_MASK | DMA_CHXCTL_PWIDTH_MASK |
              DMA_CHXCTL_MWIDTH_MASK | DMA_CHXCTL_PRIO_MASK |
              DMA_CHXCTL_PNAGA | DMA_CHXCTL_MNAGA |
              DMA_CHXCTL_PAIF | DMA_CHXCTL_CMEN);

  /* Transfer direction */

  switch (param->direction)
    {
      default:
      case DMA_PERIPH_TO_MEMORY:
      case DMA_MEMORY_TO_PERIPH:
      case DMA_MEMORY_TO_MEMORY:
        regval |= param->direction;
        break;
    }

  /* Peripheral and memory transfer width */

  switch (param->periph_memory_width)
    {
      case DMA_WIDTH_32BITS_SELECT:
        regval |= DMA_MEMORY_WIDTH_32BIT | DMA_PERIPH_WIDTH_32BIT;
        break;

      case DMA_WIDTH_16BITS_SELECT:
        regval |= DMA_MEMORY_WIDTH_16BIT | DMA_PERIPH_WIDTH_16BIT;
        break;

      default:
        regval |= DMA_MEMORY_WIDTH_8BIT | DMA_PERIPH_WIDTH_8BIT;
        break;
    }

  /* Channel priority */

  switch (param->priority)
    {
      case DMA_PRIO_LOW_SELECT:
        regval |= DMA_PRIORITY_LOW;
        break;

      case DMA_PRIO_HIGH_SELECT:
        regval |= DMA_PRIORITY_HIGH;
        break;

      case DMA_PRIO_ULTRA_HIGHSELECT:
        regval |= DMA_PRIORITY_ULTRA_HIGH;
        break;

      default:
        regval |= DMA_PRIORITY_MEDIUM;
        break;
    }

  /* Peripheral address generation: fixed (no increment) or increasing */

  if (param->periph_inc == 0)
    {
      regval &= ~DMA_CHXCTL_PNAGA;
    }
  else
    {
      regval |= DMA_CHXCTL_PNAGA;
    }

  /* Memory address generation */

  if (param->memory_inc == 0)
    {
      regval &= ~DMA_CHXCTL_MNAGA;
    }
  else
    {
      regval |= DMA_CHXCTL_MNAGA;
    }

  /* Circular mode */

  if (param->circular_mode == 1)
    {
      regval |= DMA_CHXCTL_CMEN;
    }
  else
    {
      regval &= ~DMA_CHXCTL_CMEN;
    }

  putreg32(regval, regaddr);
}

/****************************************************************************
 * Name: gd32_dma_start
 *
 * Description:
 *   Start the DMA transfer.
 *
 ****************************************************************************/

void gd32_dma_start(DMA_HANDLE handle, dma_callback_t callback, void *arg,
                    uint32_t interrupt)
{
  struct gd32_dma_channel_s *dmachan = (struct gd32_dma_channel_s *)handle;
  uint32_t regaddr;
  uint32_t regval;

  DEBUGASSERT(dmachan != NULL);

  /* Save the callback info.  This will be invoked when the DMA completes */

  dmachan->callback = callback;
  dmachan->arg      = arg;

  /* Enable the selected channel interrupts */

  regaddr = GD32H7_DMA_CHCTL(dmachan->dmabase, dmachan->chan_num);

  regval = getreg32(regaddr);
  regval &= ~(DMA_CHXCTL_SDEIE | DMA_CHXCTL_TAEIE |
              DMA_CHXCTL_HTFIE | DMA_CHXCTL_FTFIE);

  if (interrupt & DMA_INT_MASK)
    {
      regval |= (interrupt & DMA_INT_MASK);
    }
  else
    {
      /* Default: interrupt on full transfer finish */

      regval |= DMA_CHXCTL_FTFIE;
    }

  putreg32(regval, regaddr);

  /* Enable the DMA channel */

  gd32_channel_enable(dmachan->dmabase, dmachan->chan_num);
}

/****************************************************************************
 * Name: gd32_dma_stop
 *
 * Description:
 *   Cancel the DMA.
 *
 ****************************************************************************/

void gd32_dma_stop(DMA_HANDLE handle)
{
  struct gd32_dma_channel_s *dmachan = (struct gd32_dma_channel_s *)handle;

  DEBUGASSERT(dmachan != NULL);

  gd32_channel_interrupt_disable(dmachan->dmabase, dmachan->chan_num);
  gd32_channel_disable(dmachan->dmabase, dmachan->chan_num);

  dmachan->callback = NULL;
  dmachan->arg      = NULL;
}

/****************************************************************************
 * Name: gd32_dma_residuate / gd32_dma_txnum
 *
 * Description:
 *   Get the number of remaining data to be transferred by the DMA.
 *
 ****************************************************************************/

size_t gd32_dma_residuate(DMA_HANDLE handle)
{
  struct gd32_dma_channel_s *dmachan = (struct gd32_dma_channel_s *)handle;

  DEBUGASSERT(dmachan != NULL);

  return (size_t)getreg32(GD32H7_DMA_CHCNT(dmachan->dmabase,
                                           dmachan->chan_num)) &
         DMA_CHXCNT_CNT_MASK;
}

size_t gd32_dma_txnum(DMA_HANDLE handle)
{
  return gd32_dma_residuate(handle);
}

#endif /* CONFIG_GD32H7_DMA */
