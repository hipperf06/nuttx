/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_spi.c
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
#include <assert.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/irq.h>
#include <nuttx/arch.h>
#include <nuttx/mutex.h>
#include <nuttx/spi/spi.h>

#include <arch/board/board.h>

#include "chip.h"
#include "gd32h7xx_spi.h"
#include "gd32h7xx_rcu.h"
#include "gd32h7xx.h"

#ifdef CONFIG_GD32H7_SPI

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* SPI clock prescaler values as (divisor): PSC field 0-7 maps to
 * 2/4/8/16/32/64/128/256.
 */

/* Configuration ************************************************************/

/* Is there at least one SPI enabled? */

#undef HAVE_SPI

#if defined(CONFIG_GD32H7_SPI0) || defined(CONFIG_GD32H7_SPI1) || \
    defined(CONFIG_GD32H7_SPI2) || defined(CONFIG_GD32H7_SPI3) || \
    defined(CONFIG_GD32H7_SPI4) || defined(CONFIG_GD32H7_SPI5)
#  define HAVE_SPI 1
#endif

/* Chip select management: the board must provide the chip select GPIO
 * definitions.  This driver deasserts all chip selects in the board
 * provided gd32_spidev_initialize().
 */

#ifdef CONFIG_SPI_CALLBACK
#  define SPI_MAX_CS 4
#endif

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct gd32_spidev_s
{
  struct spi_dev_s spidev;       /* Externally visible part of the SPI interface */
  uint32_t         spibase;      /* SPIn base address */
  uint32_t         spiclock;     /* Clocking for the SPI module */
  uint32_t         frequency;    /* Requested clock frequency */
  uint32_t         actual;       /* Actual clock frequency */
  mutex_t          lock;         /* Held while chip is selected */
  uint8_t          nbits;        /* Width of word in bits (8 to 32) */
  uint8_t          mode;         /* Mode 0,1,2,3 */
  bool             initialized;  /* Has SPI interface been initialized */
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void gd32_spi_reset(uint32_t spibase);
static void gd32_spi_clock_enable(uint32_t spibase);
static void gd32_spi_clock_disable(uint32_t spibase);

static int  spi_lock(FAR struct spi_dev_s *dev, bool lock);
static void spi_select(FAR struct spi_dev_s *dev, uint32_t devid,
                       bool selected);
static uint32_t spi_setfrequency(FAR struct spi_dev_s *dev,
                                 uint32_t frequency);
static void spi_setmode(FAR struct spi_dev_s *dev, enum spi_mode_e mode);
static void spi_setbits(FAR struct spi_dev_s *dev, int nbits);
static uint32_t spi_send(FAR struct spi_dev_s *dev, uint32_t wd);
static void spi_exchange(FAR struct spi_dev_s *dev, FAR const void *txbuffer,
                         FAR void *rxbuffer, size_t nwords);
#ifndef CONFIG_SPI_EXCHANGE
static void spi_sndblock(FAR struct spi_dev_s *dev,
                         FAR const void *buffer, size_t nwords);
static void spi_recvblock(FAR struct spi_dev_s *dev, FAR void *buffer,
                          size_t nwords);
static void spi_exchange_nodma(FAR struct spi_dev_s *dev,
                               FAR const void *txbuffer,
                               FAR void *rxbuffer, size_t nwords);
#endif
static int  spi_bus_initialize(FAR struct spi_dev_s *dev);
static int  spi_bus_uninitialize(FAR struct spi_dev_s *dev);

/****************************************************************************
 * Private Data
 ****************************************************************************/

#ifdef CONFIG_GD32H7_SPI0
static const struct spi_ops_s g_spi0ops =
{
  .lock              = spi_lock,
  .select            = spi_select,
  .setfrequency      = spi_setfrequency,
  .setmode           = spi_setmode,
  .setbits           = spi_setbits,
  .status            = 0,
  .send              = spi_send,
#ifdef CONFIG_SPI_EXCHANGE
  .exchange          = spi_exchange,
#else
  .sndblock          = spi_sndblock,
  .recvblock         = spi_recvblock,
#endif
  .registercallback  = 0,                   /* Not implemented */
};
#endif

#ifdef CONFIG_GD32H7_SPI1
static const struct spi_ops_s g_spi1ops =
{
  .lock              = spi_lock,
  .select            = spi_select,
  .setfrequency      = spi_setfrequency,
  .setmode           = spi_setmode,
  .setbits           = spi_setbits,
  .status            = 0,
  .send              = spi_send,
#ifdef CONFIG_SPI_EXCHANGE
  .exchange          = spi_exchange,
#else
  .sndblock          = spi_sndblock,
  .recvblock         = spi_recvblock,
#endif
  .registercallback  = 0,
};
#endif

#ifdef CONFIG_GD32H7_SPI2
static const struct spi_ops_s g_spi2ops =
{
  .lock              = spi_lock,
  .select            = spi_select,
  .setfrequency      = spi_setfrequency,
  .setmode           = spi_setmode,
  .setbits           = spi_setbits,
  .status            = 0,
  .send              = spi_send,
#ifdef CONFIG_SPI_EXCHANGE
  .exchange          = spi_exchange,
#else
  .sndblock          = spi_sndblock,
  .recvblock         = spi_recvblock,
#endif
  .registercallback  = 0,
};
#endif

#ifdef CONFIG_GD32H7_SPI3
static const struct spi_ops_s g_spi3ops =
{
  .lock              = spi_lock,
  .select            = spi_select,
  .setfrequency      = spi_setfrequency,
  .setmode           = spi_setmode,
  .setbits           = spi_setbits,
  .status            = 0,
  .send              = spi_send,
#ifdef CONFIG_SPI_EXCHANGE
  .exchange          = spi_exchange,
#else
  .sndblock          = spi_sndblock,
  .recvblock         = spi_recvblock,
#endif
  .registercallback  = 0,
};
#endif

#ifdef CONFIG_GD32H7_SPI4
static const struct spi_ops_s g_spi4ops =
{
  .lock              = spi_lock,
  .select            = spi_select,
  .setfrequency      = spi_setfrequency,
  .setmode           = spi_setmode,
  .setbits           = spi_setbits,
  .status            = 0,
  .send              = spi_send,
#ifdef CONFIG_SPI_EXCHANGE
  .exchange          = spi_exchange,
#else
  .sndblock          = spi_sndblock,
  .recvblock         = spi_recvblock,
#endif
  .registercallback  = 0,
};
#endif

#ifdef CONFIG_GD32H7_SPI5
static const struct spi_ops_s g_spi5ops =
{
  .lock              = spi_lock,
  .select            = spi_select,
  .setfrequency      = spi_setfrequency,
  .setmode           = spi_setmode,
  .setbits           = spi_setbits,
  .status            = 0,
  .send              = spi_send,
#ifdef CONFIG_SPI_EXCHANGE
  .exchange          = spi_exchange,
#else
  .sndblock          = spi_sndblock,
  .recvblock         = spi_recvblock,
#endif
  .registercallback  = 0,
};
#endif

#ifdef CONFIG_GD32H7_SPI0
static struct gd32_spidev_s g_spi0dev =
{
  .spidev     =
  {
    .ops      = &g_spi0ops,
  },
  .spibase    = GD32H7_SPI0,
  .spiclock   = GD32_PCLK2_FREQUENCY,
  .lock       = NXMUTEX_INITIALIZER,
};
#endif

#ifdef CONFIG_GD32H7_SPI1
static struct gd32_spidev_s g_spi1dev =
{
  .spidev     =
  {
    .ops      = &g_spi1ops,
  },
  .spibase    = GD32H7_SPI1,
  .spiclock   = GD32_PCLK1_FREQUENCY,
  .lock       = NXMUTEX_INITIALIZER,
};
#endif

#ifdef CONFIG_GD32H7_SPI2
static struct gd32_spidev_s g_spi2dev =
{
  .spidev     =
  {
    .ops      = &g_spi2ops,
  },
  .spibase    = GD32H7_SPI2,
  .spiclock   = GD32_PCLK1_FREQUENCY,
  .lock       = NXMUTEX_INITIALIZER,
};
#endif

#ifdef CONFIG_GD32H7_SPI3
static struct gd32_spidev_s g_spi3dev =
{
  .spidev     =
  {
    .ops      = &g_spi3ops,
  },
  .spibase    = GD32H7_SPI3,
  .spiclock   = GD32_PCLK2_FREQUENCY,
  .lock       = NXMUTEX_INITIALIZER,
};
#endif

#ifdef CONFIG_GD32H7_SPI4
static struct gd32_spidev_s g_spi4dev =
{
  .spidev     =
  {
    .ops      = &g_spi4ops,
  },
  .spibase    = GD32H7_SPI4,
  .spiclock   = GD32_PCLK2_FREQUENCY,
  .lock       = NXMUTEX_INITIALIZER,
};
#endif

#ifdef CONFIG_GD32H7_SPI5
static struct gd32_spidev_s g_spi5dev =
{
  .spidev     =
  {
    .ops      = &g_spi5ops,
  },
  .spibase    = GD32H7_SPI5,
  .spiclock   = GD32_PCLK2_FREQUENCY,
  .lock       = NXMUTEX_INITIALIZER,
};
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_spi_clock_enable / disable
 *
 * Description:
 *   Enable or disable the SPI clock.
 *
 ****************************************************************************/

static void gd32_spi_clock_enable(uint32_t spibase)
{
  switch (spibase)
    {
      default:
        return;
#ifdef CONFIG_GD32H7_SPI0
      case GD32H7_SPI0:
        gd32_rcu_periph_clock_enable(RCU_SPI0_EN);
        break;
#endif
#ifdef CONFIG_GD32H7_SPI1
      case GD32H7_SPI1:
        gd32_rcu_periph_clock_enable(RCU_SPI1_EN);
        break;
#endif
#ifdef CONFIG_GD32H7_SPI2
      case GD32H7_SPI2:
        gd32_rcu_periph_clock_enable(RCU_SPI2_EN);
        break;
#endif
#ifdef CONFIG_GD32H7_SPI3
      case GD32H7_SPI3:
        gd32_rcu_periph_clock_enable(RCU_SPI3_EN);
        break;
#endif
#ifdef CONFIG_GD32H7_SPI4
      case GD32H7_SPI4:
        gd32_rcu_periph_clock_enable(RCU_SPI4_EN);
        break;
#endif
#ifdef CONFIG_GD32H7_SPI5
      case GD32H7_SPI5:
        gd32_rcu_periph_clock_enable(RCU_SPI5_EN);
        break;
#endif
    }
}

static void gd32_spi_clock_disable(uint32_t spibase)
{
  switch (spibase)
    {
      default:
        return;
#ifdef CONFIG_GD32H7_SPI0
      case GD32H7_SPI0:
        gd32_rcu_periph_clock_disable(RCU_SPI0_EN);
        break;
#endif
#ifdef CONFIG_GD32H7_SPI1
      case GD32H7_SPI1:
        gd32_rcu_periph_clock_disable(RCU_SPI1_EN);
        break;
#endif
#ifdef CONFIG_GD32H7_SPI2
      case GD32H7_SPI2:
        gd32_rcu_periph_clock_disable(RCU_SPI2_EN);
        break;
#endif
#ifdef CONFIG_GD32H7_SPI3
      case GD32H7_SPI3:
        gd32_rcu_periph_clock_disable(RCU_SPI3_EN);
        break;
#endif
#ifdef CONFIG_GD32H7_SPI4
      case GD32H7_SPI4:
        gd32_rcu_periph_clock_disable(RCU_SPI4_EN);
        break;
#endif
#ifdef CONFIG_GD32H7_SPI5
      case GD32H7_SPI5:
        gd32_rcu_periph_clock_disable(RCU_SPI5_EN);
        break;
#endif
    }
}

/****************************************************************************
 * Name: gd32_spi_reset
 *
 * Description:
 *   Reset the SPI via the RCU reset register.
 *
 ****************************************************************************/

static void gd32_spi_reset(uint32_t spibase)
{
  uint32_t rcu_rst;

  switch (spibase)
    {
      default:
        return;
#ifdef CONFIG_GD32H7_SPI0
      case GD32H7_SPI0:
        rcu_rst = RCU_SPI0_RST;
        break;
#endif
#ifdef CONFIG_GD32H7_SPI1
      case GD32H7_SPI1:
        rcu_rst = RCU_SPI1_RST;
        break;
#endif
#ifdef CONFIG_GD32H7_SPI2
      case GD32H7_SPI2:
        rcu_rst = RCU_SPI2_RST;
        break;
#endif
#ifdef CONFIG_GD32H7_SPI3
      case GD32H7_SPI3:
        rcu_rst = RCU_SPI3_RST;
        break;
#endif
#ifdef CONFIG_GD32H7_SPI4
      case GD32H7_SPI4:
        rcu_rst = RCU_SPI4_RST;
        break;
#endif
#ifdef CONFIG_GD32H7_SPI5
      case GD32H7_SPI5:
        rcu_rst = RCU_SPI5_RST;
        break;
#endif
    }

  gd32_rcu_periph_reset(rcu_rst);
}

/****************************************************************************
 * Name: spi_lock
 *
 * Description:
 *   On SPI buses where there are multiple devices, it will be necessary to
 *   lock SPI to have exclusive access to the buses for a sequence of
 *   transfers.  The bus should be locked before the chip is selected. After
 *   locking the SPI bus, the caller should then also call the setfrequency,
 *   setbits, and setmode methods to make sure that the SPI is properly
 *   configured for the device.  If the SPI bus is being used for both
 *   master and slave, it probably is necessary to lock this device if
 *   starting negotiations with a different device.
 *
 ****************************************************************************/

static int spi_lock(FAR struct spi_dev_s *dev, bool lock)
{
  FAR struct gd32_spidev_s *priv = (FAR struct gd32_spidev_s *)dev;
  int ret;

  if (lock)
    {
      /* Take the semaphore (perhaps waiting) */

      ret = nxmutex_lock(&priv->lock);
      if (ret < 0)
        {
          return ret;
        }
    }
  else
    {
      ret = nxmutex_unlock(&priv->lock);
    }

  return ret;
}

/****************************************************************************
 * Name: spi_select
 *
 * Description:
 *   This function is called when a device is selected.  The chip select
 *   GPIO is managed by the board logic through the board provided
 *   gd32_spidev_select() function (weak symbol).
 *
 ****************************************************************************/

void weak_function gd32_spidev_select(FAR struct spi_dev_s *dev,
                                      uint32_t devid, bool selected)
{
  spiinfo("WARNING: no chip select callback provided\n");
}

static void spi_select(FAR struct spi_dev_s *dev, uint32_t devid,
                       bool selected)
{
  gd32_spidev_select(dev, devid, selected);
}

/****************************************************************************
 * Name: spi_setfrequency
 *
 * Description:
 *   Set the SPI frequency.
 *
 * Input Parameters:
 *   frequency - The SPI frequency requested
 *
 * Returned Value:
 *   Returns the actual frequency selected
 *
 ****************************************************************************/

static uint32_t spi_setfrequency(FAR struct spi_dev_s *dev,
                                 uint32_t frequency)
{
  FAR struct gd32_spidev_s *priv = (FAR struct gd32_spidev_s *)dev;
  uint32_t spi_clock;
  uint32_t regval;
  uint32_t psc;
  uint32_t actual;

  spiinfo("frequency=%lu\n", (unsigned long)frequency);

  DEBUGASSERT(priv->spiclock > 0);

  spi_clock = priv->spiclock;

  /* Find the smallest prescaler that produces a frequency at or below the
   * requested frequency: f_spi = f_pclk / (2^(psc+1)).
   */

  psc = 0;
  actual = spi_clock / 2;

  while (actual > frequency && psc < 7)
    {
      psc++;
      actual >>= 1;
    }

  priv->frequency = frequency;
  priv->actual    = actual;

  /* Disable SPI before changing the configuration */

  regval = getreg32(GD32H7_SPI_CTL0(priv->spibase));
  regval &= ~SPI_CTL0_SPIEN;
  putreg32(regval, GD32H7_SPI_CTL0(priv->spibase));

  /* Set the prescaler in CFG0 */

  regval = getreg32(GD32H7_SPI_CFG0(priv->spibase));
  regval &= ~SPI_CFG0_PSC_MASK;
  regval |= SPI_CFG0_PSC(psc);
  putreg32(regval, GD32H7_SPI_CFG0(priv->spibase));

  spiinfo("frequency=%lu, actual=%lu\n",
          (unsigned long)frequency, (unsigned long)actual);

  return actual;
}

/****************************************************************************
 * Name: spi_setmode
 *
 * Description:
 *   Set the SPI mode.  Options:
 *     SPIDEV_MODE0 - CPOL=0, CPHA=0
 *     SPIDEV_MODE1 - CPOL=0, CPHA=1
 *     SPIDEV_MODE2 - CPOL=1, CPHA=0
 *     SPIDEV_MODE3 - CPOL=1, CPHA=1
 *
 ****************************************************************************/

static void spi_setmode(FAR struct spi_dev_s *dev, enum spi_mode_e mode)
{
  FAR struct gd32_spidev_s *priv = (FAR struct gd32_spidev_s *)dev;
  uint32_t setbits;
  uint32_t clrbits;
  uint32_t regval;

  spiinfo("mode=%d\n", mode);

  /* Has the mode changed? */

  if (mode != priv->mode)
    {
      /* Disable SPI before changing the configuration */

      regval = getreg32(GD32H7_SPI_CTL0(priv->spibase));
      regval &= ~SPI_CTL0_SPIEN;
      putreg32(regval, GD32H7_SPI_CTL0(priv->spibase));

      switch (mode)
        {
          case SPIDEV_MODE0: /* CPOL=0, CPHA=0 */
            clrbits = SPI_CFG1_CKPL | SPI_CFG1_CKPH;
            break;

          case SPIDEV_MODE1: /* CPOL=0, CPHA=1 */
            clrbits = SPI_CFG1_CKPL;
            setbits = SPI_CFG1_CKPH;
            break;

          case SPIDEV_MODE2: /* CPOL=1, CPHA=0 */
            clrbits = SPI_CFG1_CKPH;
            setbits = SPI_CFG1_CKPL;
            break;

          case SPIDEV_MODE3: /* CPOL=1, CPHA=1 */
            setbits = SPI_CFG1_CKPL | SPI_CFG1_CKPH;
            break;

          default:
            spiinfo("Unsupported mode: %d\n", mode);
            DEBUGASSERT(false);
            return;
        }

      /* CKPL/CKPH live in CFG1.  Handle the four combinations explicitly to
       * avoid undefined setbits in the switch above.
       */

      regval = getreg32(GD32H7_SPI_CFG1(priv->spibase));
      regval &= ~(SPI_CFG1_CKPL | SPI_CFG1_CKPH);

      if (mode == SPIDEV_MODE1 || mode == SPIDEV_MODE3)
        {
          regval |= SPI_CFG1_CKPH;
        }

      if (mode == SPIDEV_MODE2 || mode == SPIDEV_MODE3)
        {
          regval |= SPI_CFG1_CKPL;
        }

      putreg32(regval, GD32H7_SPI_CFG1(priv->spibase));

      UNUSED(clrbits);
      UNUSED(setbits);

      /* Save the mode */

      priv->mode = mode;
    }
}

/****************************************************************************
 * Name: spi_setbits
 *
 * Description:
 *   Set the number of bits per word.
 *
 ****************************************************************************/

static void spi_setbits(FAR struct spi_dev_s *dev, int nbits)
{
  FAR struct gd32_spidev_s *priv = (FAR struct gd32_spidev_s *)dev;
  uint32_t regval;

  spiinfo("nbits=%d\n", nbits);

  DEBUGASSERT(nbits > 0 && nbits <= 32);

  /* Has the number of bits changed? */

  if (nbits != priv->nbits)
    {
      /* Disable SPI before changing the configuration */

      regval = getreg32(GD32H7_SPI_CTL0(priv->spibase));
      regval &= ~SPI_CTL0_SPIEN;
      putreg32(regval, GD32H7_SPI_CTL0(priv->spibase));

      /* Data size: CFG0.DZ holds (nbits - 1) */

      regval = getreg32(GD32H7_SPI_CFG0(priv->spibase));
      regval &= ~SPI_CFG0_DZ_MASK;
      regval |= SPI_CFG0_DZ(nbits - 1);
      putreg32(regval, GD32H7_SPI_CFG0(priv->spibase));

      priv->nbits = (uint8_t)nbits;
    }
}

/****************************************************************************
 * Name: spi_send
 *
 * Description:
 *   Exchange one word on SPI.
 *
 * Input Parameters:
 *   wd - The word to send.  the size of the data is determined by the
 *        number of bits selected by the spi_setbits.
 *
 * Returned Value:
 *   response
 *
 ****************************************************************************/

static uint32_t spi_send(FAR struct spi_dev_s *dev, uint32_t wd)
{
  FAR struct gd32_spidev_s *priv = (FAR struct gd32_spidev_s *)dev;
  uint32_t data;

  /* Wait for TxFIFO packet space available */

  while ((getreg32(GD32H7_SPI_STAT(priv->spibase)) & SPI_STAT_TP) == 0);

  /* Write the word to the TDATA register */

  putreg32(wd, GD32H7_SPI_TDATA(priv->spibase));

  /* Wait for RxFIFO data */

  while ((getreg32(GD32H7_SPI_STAT(priv->spibase)) & SPI_STAT_RP) == 0);

  /* Read the received word */

  data = getreg32(GD32H7_SPI_RDATA(priv->spibase));

  return data;
}

/****************************************************************************
 * Name: spi_exchange
 *
 * Description:
 *   Exchange a block of data from SPI.
 *
 ****************************************************************************/

#ifdef CONFIG_SPI_EXCHANGE
static void spi_exchange(FAR struct spi_dev_s *dev,
                         FAR const void *txbuffer, FAR void *rxbuffer,
                         size_t nwords)
#else
static void spi_sndblock(FAR struct spi_dev_s *dev,
                         FAR const void *buffer, size_t nwords)
{
  spi_exchange_nodma(dev, buffer, NULL, nwords);
}

static void spi_recvblock(FAR struct spi_dev_s *dev, FAR void *buffer,
                          size_t nwords)
{
  spi_exchange_nodma(dev, NULL, buffer, nwords);
}

static void spi_exchange_nodma(FAR struct spi_dev_s *dev,
                               FAR const void *txbuffer,
                               FAR void *rxbuffer, size_t nwords)
#endif
{
  FAR struct gd32_spidev_s *priv = (FAR struct gd32_spidev_s *)dev;
  FAR uint8_t *brxptr = (FAR uint8_t *)rxbuffer;
  FAR const uint8_t *btxptr = (FAR const uint8_t *)txbuffer;

  spiinfo("txbuffer=%p rxbuffer=%p nwords=%d\n", txbuffer, rxbuffer, nwords);

  /* 8-bit mode */

  DEBUGASSERT(priv->nbits <= 8);

  while (nwords-- > 0)
    {
      uint32_t word;

      /* Get the next word to write.  Is there a source buffer? */

      if (btxptr)
        {
          word = *btxptr++;
        }
      else
        {
          word = 0xff;
        }

      /* Exchange one word */

      word = spi_send(dev, word);

      /* Is there a buffer to receive the return value? */

      if (brxptr)
        {
          *brxptr++ = (uint8_t)word;
        }
    }
}

/****************************************************************************
 * Name: spi_bus_initialize
 *
 * Description:
 *   Configure the SPI bus.
 *
 ****************************************************************************/

static int spi_bus_initialize(FAR struct spi_dev_s *dev)
{
  FAR struct gd32_spidev_s *priv = (FAR struct gd32_spidev_s *)dev;
  uint32_t regval;

  if (!priv->initialized)
    {
      spiinfo("initialize spibase=%" PRIx32 "\n", priv->spibase);

      /* Reset and enable the SPI */

      gd32_spi_reset(priv->spibase);
      gd32_spi_clock_enable(priv->spibase);

      /* Disable SPI while configuring */

      regval = getreg32(GD32H7_SPI_CTL0(priv->spibase));
      regval &= ~SPI_CTL0_SPIEN;
      putreg32(regval, GD32H7_SPI_CTL0(priv->spibase));

      /* Master mode, software NSS management, 8-bit frames, MSB first,
       * default mode 0.
       */

      regval = getreg32(GD32H7_SPI_CFG1(priv->spibase));
      regval |= (SPI_CFG1_MSTMOD | SPI_CFG1_NSSIM);
      regval &= ~(SPI_CFG1_CKPL | SPI_CFG1_CKPH | SPI_CFG1_LF |
                  SPI_CFG1_RO | SPI_CFG1_BDEN | SPI_CFG1_BDOEN);
      putreg32(regval, GD32H7_SPI_CFG1(priv->spibase));

      /* Data size 8-bit, medium priority prescaler default (PSC set by
       * setfrequency()).
       */

      regval = getreg32(GD32H7_SPI_CFG0(priv->spibase));
      regval &= ~(SPI_CFG0_DZ_MASK | SPI_CFG0_PSC_MASK);
      regval |= SPI_DATASIZE_8BIT;
      putreg32(regval, GD32H7_SPI_CFG0(priv->spibase));

      /* Clear any pending error flags */

      putreg32(SPI_STATC_ETC | SPI_STATC_TXFC | SPI_STATC_TXURERRC |
               SPI_STATC_RXORERRC | SPI_STATC_CRCERRC | SPI_STATC_FERRC |
               SPI_STATC_CONFERRC,
               GD32H7_SPI_STATC(priv->spibase));

      /* Set the default frequency: PCLK / 256 to start conservatively */

      priv->frequency = 0;
      priv->nbits     = 8;
      priv->mode      = SPIDEV_MODE0;
      spi_setfrequency(dev, 1000000);

      /* Enable SPI */

      regval = getreg32(GD32H7_SPI_CTL0(priv->spibase));
      regval |= SPI_CTL0_SPIEN;
      putreg32(regval, GD32H7_SPI_CTL0(priv->spibase));

      priv->initialized = true;
    }

  return OK;
}

/****************************************************************************
 * Name: spi_bus_uninitialize
 *
 * Description:
 *   De-initialize the SPI bus.
 *
 ****************************************************************************/

static int spi_bus_uninitialize(FAR struct spi_dev_s *dev)
{
  FAR struct gd32_spidev_s *priv = (FAR struct gd32_spidev_s *)dev;
  uint32_t regval;

  if (priv->initialized)
    {
      /* Disable SPI */

      regval = getreg32(GD32H7_SPI_CTL0(priv->spibase));
      regval &= ~SPI_CTL0_SPIEN;
      putreg32(regval, GD32H7_SPI_CTL0(priv->spibase));

      /* Disable the SPI clock */

      gd32_spi_clock_disable(priv->spibase);

      priv->initialized = false;
    }

  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_spibus_initialize
 *
 * Description:
 *   Initialize the selected SPI bus.
 *
 ****************************************************************************/

FAR struct spi_dev_s *gd32_spibus_initialize(int port)
{
  FAR struct gd32_spidev_s *priv = NULL;

  irqstate_t flags = enter_critical_section();
  int ret;

#ifdef CONFIG_GD32H7_SPI0
  if (port == 0)
    {
      priv = &g_spi0dev;
    }
  else
#endif
#ifdef CONFIG_GD32H7_SPI1
  if (port == 1)
    {
      priv = &g_spi1dev;
    }
  else
#endif
#ifdef CONFIG_GD32H7_SPI2
  if (port == 2)
    {
      priv = &g_spi2dev;
    }
  else
#endif
#ifdef CONFIG_GD32H7_SPI3
  if (port == 3)
    {
      priv = &g_spi3dev;
    }
  else
#endif
#ifdef CONFIG_GD32H7_SPI4
  if (port == 4)
    {
      priv = &g_spi4dev;
    }
  else
#endif
#ifdef CONFIG_GD32H7_SPI5
  if (port == 5)
    {
      priv = &g_spi5dev;
    }
  else
#endif
    {
      spiinfo("ERROR: Unsupported SPI port: %d\n", port);
      leave_critical_section(flags);
      return NULL;
    }

  ret = spi_bus_initialize(&priv->spidev);
  if (ret < 0)
    {
      leave_critical_section(flags);
      return NULL;
    }

  leave_critical_section(flags);
  return &priv->spidev;
}

/****************************************************************************
 * Name: gd32_spibus_uninitialize
 *
 * Description:
 *   De-initialize the selected SPI bus.
 *
 ****************************************************************************/

int gd32_spibus_uninitialize(FAR struct spi_dev_s *dev)
{
  return spi_bus_uninitialize(dev);
}

#endif /* CONFIG_GD32H7_SPI */
