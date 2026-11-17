/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_rcu.c
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
#include <assert.h>

#include <arch/board/board.h>

#include "arm_internal.h"

#include "chip.h"
#include "gd32h7xx.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Oscillator frequencies.  The board.h file must provide:
 *
 *   GD32_HXTAL_FREQUENCY  - Frequency of the high speed external oscillator
 *                           (HXTAL) in Hz.
 *   GD32_SYSCLK_FREQUENCY - Desired system clock frequency in Hz.
 *   GD32_AHB_FREQUENCY    - Desired AHB clock frequency in Hz.
 *   GD32_PCLK1_FREQUENCY  - Desired APB1 clock frequency in Hz.
 *   GD32_PCLK2_FREQUENCY  - Desired APB2 clock frequency in Hz.
 *   GD32_PCLK3_FREQUENCY  - Desired APB3 clock frequency in Hz.
 *   GD32_PCLK4_FREQUENCY  - Desired APB4 clock frequency in Hz.
 */

#ifndef GD32_HXTAL_FREQUENCY
#  error "GD32_HXTAL_FREQUENCY must be defined in board.h"
#endif

#ifndef GD32_SYSCLK_FREQUENCY
#  error "GD32_SYSCLK_FREQUENCY must be defined in board.h"
#endif

#ifndef GD32_AHB_FREQUENCY
#  error "GD32_AHB_FREQUENCY must be defined in board.h"
#endif

#ifndef GD32_PCLK1_FREQUENCY
#  error "GD32_PCLK1_FREQUENCY must be defined in board.h"
#endif

#ifndef GD32_PCLK2_FREQUENCY
#  error "GD32_PCLK2_FREQUENCY must be defined in board.h"
#endif

#ifndef GD32_PCLK3_FREQUENCY
#  error "GD32_PCLK3_FREQUENCY must be defined in board.h"
#endif

#ifndef GD32_PCLK4_FREQUENCY
#  error "GD32_PCLK4_FREQUENCY must be defined in board.h"
#endif

/* Internal oscillator frequencies (fixed by silicon) */

#define IRC64M_FREQUENCY        64000000
#define LPIRC4M_FREQUENCY       4000000

/* Select the system clock source:
 *
 *   CONFIG_GD32H7_USE_IRC64M  - Run the system clock from the internal
 *                               64MHz RC oscillator (no PLL).
 *   otherwise                 - Run the system clock from PLL0 which is
 *                               fed by HXTAL.  The PLL configuration must
 *                               be provided in board.h:
 *
 *   GD32_PLL0PSC              - PLL0 input prescaler (1-63)
 *   GD32_PLL0N                - PLL0 multiplication factor (9-512)
 *   GD32_PLL0P                - PLL0P output divider (1-128)
 *   GD32_PLL0Q                - PLL0Q output divider (1-128)
 *   GD32_PLL0R                - PLL0R output divider (1-128)
 */

#ifdef CONFIG_GD32H7_USE_IRC64M

#  define GD32_PLL0PSC          0
#  define GD32_PLL0N            0
#  define GD32_PLL0P            0
#  define GD32_PLL0Q            0
#  define GD32_PLL0R            0

#else

#  ifndef GD32_PLL0PSC
#    error "GD32_PLL0PSC must be defined in board.h"
#  endif
#  ifndef GD32_PLL0N
#    error "GD32_PLL0N must be defined in board.h"
#  endif
#  ifndef GD32_PLL0P
#    error "GD32_PLL0P must be defined in board.h"
#  endif
#  ifndef GD32_PLL0Q
#    error "GD32_PLL0Q must be defined in board.h"
#  endif
#  ifndef GD32_PLL0R
#    error "GD32_PLL0R must be defined in board.h"
#  endif

/* Validate the requested frequencies */

#  if (GD32_HXTAL_FREQUENCY / GD32_PLL0PSC) * GD32_PLL0N / GD32_PLL0P != GD32_SYSCLK_FREQUENCY
#    error "GD32_SYSCLK_FREQUENCY does not match HXTAL / PLL0PSC * PLL0N / PLL0P"
#  endif

#endif

/* Compute the prescaler register encodings from the requested bus
 * frequencies.
 */

#define AHBDIV(sys, ahb) \
  ((sys) == (ahb)          ? (0U << 4) : \
   (sys) == 2 * (ahb)      ? (8U << 4) : \
   (sys) == 4 * (ahb)      ? (9U << 4) : \
   (sys) == 8 * (ahb)      ? (10U << 4) : \
   (sys) == 16 * (ahb)     ? (11U << 4) : \
   (sys) == 64 * (ahb)     ? (12U << 4) : \
   (sys) == 128 * (ahb)    ? (13U << 4) : \
   (sys) == 256 * (ahb)    ? (14U << 4) : \
   (sys) == 512 * (ahb)    ? (15U << 4) : 0xffff)

/* The APB prescalers are 3-bit fields in RCU_CFG0: APB1 at bits 10-12,
 * APB2 at bits 13-15, APB3 at bits 27-29 and APB4 at bits 24-26.  The
 * register encodings are the same for all four fields (DIV1=0, DIV2=4,
 * DIV4=5, DIV8=6, DIV16=7); only the shift differs.
 */

#define APBDIV(ahb, apb, shift) \
  ((ahb) == (apb)          ? (0U << (shift)) : \
   (ahb) == 2 * (apb)      ? (4U << (shift)) : \
   (ahb) == 4 * (apb)      ? (5U << (shift)) : \
   (ahb) == 8 * (apb)      ? (6U << (shift)) : \
   (ahb) == 16 * (apb)     ? (7U << (shift)) : 0xffff)

#define GD32_AHB_PSC            AHBDIV(GD32_SYSCLK_FREQUENCY, GD32_AHB_FREQUENCY)
#define GD32_APB1_PSC           APBDIV(GD32_AHB_FREQUENCY, GD32_PCLK1_FREQUENCY, 10)
#define GD32_APB2_PSC           APBDIV(GD32_AHB_FREQUENCY, GD32_PCLK2_FREQUENCY, 13)
#define GD32_APB3_PSC           APBDIV(GD32_AHB_FREQUENCY, GD32_PCLK3_FREQUENCY, 27)
#define GD32_APB4_PSC           APBDIV(GD32_AHB_FREQUENCY, GD32_PCLK4_FREQUENCY, 24)

#if GD32_AHB_PSC == 0xffff
#  error "Unsupported GD32_AHB_FREQUENCY divider"
#endif
#if GD32_APB1_PSC == 0xffff
#  error "Unsupported GD32_PCLK1_FREQUENCY divider"
#endif
#if GD32_APB2_PSC == 0xffff
#  error "Unsupported GD32_PCLK2_FREQUENCY divider"
#endif
#if GD32_APB3_PSC == 0xffff
#  error "Unsupported GD32_PCLK3_FREQUENCY divider"
#endif
#if GD32_APB4_PSC == 0xffff
#  error "Unsupported GD32_PCLK4_FREQUENCY divider"
#endif

/* At CK_SYS frequencies above 200MHz the TCM requires a wait state
 * (see the official system_gd32h7xx.c).  The APB4 (and therefore the
 * SYSCFG) must be configured with a divider of at least 2 in that case.
 * Note that DIV1 encodes as 0.
 */

#if (GD32_SYSCLK_FREQUENCY > 200000000) && (GD32_APB4_PSC == 0)
#  warning "TCM wait state is required at high clock rates; use APB4 >= AHB/2"
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

#ifndef CONFIG_GD32H7_USE_IRC64M

/****************************************************************************
 * Name: gd32_pll_input_range
 *
 * Description:
 *   Return the PLLALL input range encoding for the given PLL input clock.
 *
 ****************************************************************************/

static inline uint32_t gd32_pll_input_range(uint32_t freq)
{
  if (freq <= 2000000)
    {
      return RCU_PLL0RNG_1M_2M;
    }
  else if (freq <= 4000000)
    {
      return RCU_PLL0RNG_2M_4M;
    }
  else if (freq <= 8000000)
    {
      return RCU_PLL0RNG_4M_8M;
    }
  else /* if (freq <= 16000000) */
    {
      return RCU_PLL0RNG_8M_16M;
    }
}

#endif /* !CONFIG_GD32H7_USE_IRC64M */

#ifdef CONFIG_GD32H7_USE_IRC64M

/****************************************************************************
 * Name: gd32_system_clock_irc64m
 *
 * Description:
 *   Configure the system clock to IRC64MDIV (64MHz) with no PLL.
 *
 ****************************************************************************/

static void gd32_system_clock_irc64m(void)
{
  uint32_t regval;

  /* IRC64M is already enabled and stable at this point */

  /* AHB = SYSCLK / 1, APB1/2/3/4 = AHB / 1 */

  regval  = getreg32(GD32H7_RCU_CFG0);
  regval &= ~(RCU_CFG0_AHBPSC | RCU_CFG0_APB1PSC | RCU_CFG0_APB2PSC |
              RCU_CFG0_APB3PSC | RCU_CFG0_APB4PSC);
  regval |= (RCU_CFG0_AHBPSC_DIV1 | RCU_CFG0_APB1PSC_DIV1 |
             RCU_CFG0_APB2PSC_DIV1 | RCU_CFG0_APB3PSC_DIV1 |
             RCU_CFG0_APB4PSC_DIV1);
  putreg32(regval, GD32H7_RCU_CFG0);

  /* Configure IRC64MDIV = IRC64M / 1 */

  regval  = getreg32(GD32H7_RCU_ADDCTL1);
  regval &= ~RCU_ADDCTL1_IRC64MDIV;
  regval |= RCU_IRC64M_DIV1;
  putreg32(regval, GD32H7_RCU_ADDCTL1);

  /* Select IRC64MDIV as the system clock */

  regval  = getreg32(GD32H7_RCU_CFG0);
  regval &= ~RCU_CFG0_SCS;
  regval |= RCU_CFG0_SCS_IRC64MDIV;
  putreg32(regval, GD32H7_RCU_CFG0);

  /* Wait until IRC64MDIV is selected as the system clock */

  while ((getreg32(GD32H7_RCU_CFG0) & RCU_CFG0_SCSS) != RCU_CFG0_SCSS_IRC64MDIV)
    {
    }
}

#endif /* CONFIG_GD32H7_USE_IRC64M */

#ifndef CONFIG_GD32H7_USE_IRC64M

/****************************************************************************
 * Name: gd32_system_clock_pll_hxtal
 *
 * Description:
 *   Configure the system clock from PLL0 which is fed by HXTAL.  The PLL
 *   settings are provided by board.h (GD32_PLL0PSC/N/P/Q/R).
 *
 ****************************************************************************/

static void gd32_system_clock_pll_hxtal(void)
{
  uint32_t regval;
  uint32_t pllin;

  /* Enable HXTAL */

  regval = getreg32(GD32H7_RCU_CTL);
  regval |= RCU_CTL_HXTALEN;
  putreg32(regval, GD32H7_RCU_CTL);

  /* Wait until HXTAL is stable */

  while ((getreg32(GD32H7_RCU_CTL) & RCU_CTL_HXTALSTB) == 0)
    {
    }

  /* Enable the SYSCFG clock (APB4) and insert a TCM wait state when the
   * system clock is higher than 200MHz.
   */

  modifyreg32(GD32H7_RCU_APB4EN, 0, RCU_APB4EN_SYSCFGEN);

#if (GD32_SYSCLK_FREQUENCY > 200000000)
  modifyreg32(GD32H7_SYSCFG_SRAMCFG1, 0, SYSCFG_SRAMCFG1_TCM_WAITSTATE);
#else
  modifyreg32(GD32H7_SYSCFG_SRAMCFG1, SYSCFG_SRAMCFG1_TCM_WAITSTATE, 0);
#endif

  /* Configure the bus prescalers */

  regval  = getreg32(GD32H7_RCU_CFG0);
  regval &= ~(RCU_CFG0_AHBPSC | RCU_CFG0_APB1PSC | RCU_CFG0_APB2PSC |
              RCU_CFG0_APB3PSC | RCU_CFG0_APB4PSC);
  regval |= (GD32_AHB_PSC | GD32_APB1_PSC | GD32_APB2_PSC |
             GD32_APB3_PSC | GD32_APB4_PSC);
  putreg32(regval, GD32H7_RCU_CFG0);

  /* Select HXTAL as the PLLs clock source and configure the PLL0 input
   * clock range.
   */

  pllin = GD32_HXTAL_FREQUENCY / GD32_PLL0PSC;

  regval  = getreg32(GD32H7_RCU_PLLALL);
  regval &= ~(RCU_PLLALL_PLLSEL | RCU_PLLALL_PLL0VCOSEL | RCU_PLLALL_PLL0RNG);
  regval |= (RCU_PLLALL_PLLSRC_HXTAL | gd32_pll_input_range(pllin));
  putreg32(regval, GD32H7_RCU_PLLALL);

  /* Configure PLL0:
   *
   *   PLL0P = HXTAL / PLL0PSC * PLL0N / PLL0P
   *   The register fields hold: PSC (direct), N-1, P-1, R-1 and Q-1.
   */

  regval  = getreg32(GD32H7_RCU_PLL0);
  regval &= ~(RCU_PLL0_PLL0N | RCU_PLL0_PLL0PSC |
              RCU_PLL0_PLL0P | RCU_PLL0_PLL0R | RCU_PLL0_PLLSTBSRC);
  regval |= (((GD32_PLL0N - 1) << 6) | (GD32_PLL0PSC << 0) |
             ((GD32_PLL0P - 1) << 16) | ((GD32_PLL0R - 1) << 24));
  putreg32(regval, GD32H7_RCU_PLL0);

  regval  = getreg32(GD32H7_RCU_PLLADDCTL);
  regval &= ~RCU_PLLADDCTL_PLL0Q;
  regval |= ((GD32_PLL0Q - 1) << 0);
  putreg32(regval, GD32H7_RCU_PLLADDCTL);

  /* Enable the PLL0P, PLL0Q and PLL0R divider outputs */

  modifyreg32(GD32H7_RCU_PLLADDCTL, 0,
              RCU_PLLADDCTL_PLL0PEN | RCU_PLLADDCTL_PLL0QEN |
              RCU_PLLADDCTL_PLL0REN);

  /* Enable PLL0 and wait until it is stable */

  modifyreg32(GD32H7_RCU_CTL, 0, RCU_CTL_PLL0EN);

  while ((getreg32(GD32H7_RCU_CTL) & RCU_CTL_PLL0STB) == 0)
    {
    }

  /* Select PLL0P as the system clock */

  regval  = getreg32(GD32H7_RCU_CFG0);
  regval &= ~RCU_CFG0_SCS;
  regval |= RCU_CFG0_SCS_PLL0P;
  putreg32(regval, GD32H7_RCU_CFG0);

  /* Wait until PLL0P is selected as the system clock */

  while ((getreg32(GD32H7_RCU_CFG0) & RCU_CFG0_SCSS) != RCU_CFG0_SCSS_PLL0P)
    {
    }
}

#endif /* !CONFIG_GD32H7_USE_IRC64M */

/****************************************************************************
 * Name: gd32_system_clock_config
 *
 * Description:
 *   Configure the system clock according to the settings in board.h.
 *
 ****************************************************************************/

static void gd32_system_clock_config(void)
{
#ifdef CONFIG_GD32H7_USE_IRC64M
  /* Select IRC64M which is already enabled as SYSCLK */

  gd32_system_clock_irc64m();
#else
  /* Select PLL which source is HXTAL as SYSCLK based on board.h setting */

  gd32_system_clock_pll_hxtal();
#endif
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_clockconfig
 *
 * Description:
 *   Called to initialize the GD32H7.  This does whatever setup is needed to
 *   put the MCU in a usable state.  This includes the initialization of
 *   clocking using the settings in board.h.
 *
 ****************************************************************************/

void gd32_clockconfig(void)
{
  uint32_t regval;

  /* Enable IRC64M and wait until it is stable */

  modifyreg32(GD32H7_RCU_CTL, 0, RCU_CTL_IRC64MEN);

  while ((getreg32(GD32H7_RCU_CTL) & RCU_CTL_IRC64MSTB) == 0)
    {
    }

  /* Select IRC64MDIV as the system clock source */

  regval  = getreg32(GD32H7_RCU_CFG0);
  regval &= ~RCU_CFG0_SCS;
  putreg32(regval, GD32H7_RCU_CFG0);

  /* Reset HXTALEN, CKMEN, PLL0EN, PLL1EN, PLL2EN and HXTALBPS bits */

  regval  = getreg32(GD32H7_RCU_CTL);
  regval &= ~(RCU_CTL_HXTALEN | RCU_CTL_CKMEN | RCU_CTL_PLL0EN |
              RCU_CTL_PLL1EN | RCU_CTL_PLL2EN | RCU_CTL_HXTALBPS);
  putreg32(regval, GD32H7_RCU_CTL);

  /* Reset PLLUSBHS0/1 and LPIRC4M enable bits */

  regval  = getreg32(GD32H7_RCU_ADDCTL1);
  regval &= ~(RCU_ADDCTL1_PLLUSBHS0EN | RCU_ADDCTL1_PLLUSBHS1EN |
              RCU_ADDCTL1_LPIRC4MEN);
  putreg32(regval, GD32H7_RCU_ADDCTL1);

  /* Reset CFG0 register */

  regval  = getreg32(GD32H7_RCU_CFG0);
  regval &= ~(RCU_CFG0_APB1PSC | RCU_CFG0_APB2PSC | RCU_CFG0_APB3PSC |
              RCU_CFG0_APB4PSC | RCU_CFG0_AHBPSC | RCU_CFG0_I2C0SEL |
              RCU_CFG0_SCS | RCU_CFG0_RTCDIV);
  putreg32(regval, GD32H7_RCU_CFG0);

  /* Reset CFG1, CFG2, CFG3, CFG4 and CFG5 registers */

  putreg32(0x00000000, GD32H7_RCU_CFG1);
  putreg32(0x00000000, GD32H7_RCU_CFG2);
  putreg32(0x00000000, GD32H7_RCU_CFG3);
  putreg32(0x00000000, GD32H7_RCU_CFG4);
  putreg32(0x00000000, GD32H7_RCU_CFG5);

  /* Wait until IRC64MDIV is selected as the system clock */

  while ((getreg32(GD32H7_RCU_CFG0) & RCU_CFG0_SCSS) != RCU_CFG0_SCSS_IRC64MDIV)
    {
    }

  /* Disable all clock stabilization interrupts */

  putreg32(0x14ff0000, GD32H7_RCU_INT);
  putreg32(0x00700000, GD32H7_RCU_ADDINT);

  /* Reset all PLL0 parameters */

  putreg32(0x01002020, GD32H7_RCU_PLL0);
  putreg32(0x01012020, GD32H7_RCU_PLL1);
  putreg32(0x01012020, GD32H7_RCU_PLL2);
  putreg32(0x00000000, GD32H7_RCU_PLLALL);
  putreg32(0x00010101, GD32H7_RCU_PLLADDCTL);
  putreg32(0x00000000, GD32H7_RCU_PLLUSBCFG);
  putreg32(0x00000000, GD32H7_RCU_PLL0FRA);
  putreg32(0x00000000, GD32H7_RCU_PLL1FRA);
  putreg32(0x00000000, GD32H7_RCU_PLL2FRA);

#if defined(CONFIG_ARCH_BOARD_GD32H7_CUSTOM_CLOCKCONFIG)

  /* Invoke board custom clock configuration */

  gd32_board_clockconfig();

#else

  /* Invoke standard, fixed clock configuration based on definitions in
   * board.h
   */

  gd32_system_clock_config();

#endif
}

/****************************************************************************
 * Name: gd32_rcu_periph_clock_enable
 *
 * Description:
 *   Enable the peripheral clock.  The peripheral selector is the encoded
 *   (register offset, bit position) value built with the RCU_PERI() macro.
 *
 ****************************************************************************/

void gd32_rcu_periph_clock_enable(uint32_t periph)
{
  uint32_t regaddr;
  uint32_t regval;

  regaddr = GD32H7_RCU + (periph >> RCU_PERI_REG_SHIFT);
  regval  = getreg32(regaddr);
  regval |= (1 << (periph & 0x1f));
  putreg32(regval, regaddr);
}

/****************************************************************************
 * Name: gd32_rcu_periph_clock_disable
 *
 * Description:
 *   Disable the peripheral clock.
 *
 ****************************************************************************/

void gd32_rcu_periph_clock_disable(uint32_t periph)
{
  uint32_t regaddr;
  uint32_t regval;

  regaddr = GD32H7_RCU + (periph >> RCU_PERI_REG_SHIFT);
  regval  = getreg32(regaddr);
  regval &= ~(1 << (periph & 0x1f));
  putreg32(regval, regaddr);
}

/****************************************************************************
 * Name: gd32_rcu_periph_reset_enable / disable
 *
 * Description:
 *   Assert / deassert the peripheral reset.  The selector uses the same
 *   RCU_PERI(offset, bit) encoding as the clock helpers, but the offset
 *   must be one of the RCU_xxxRST registers.
 *
 ****************************************************************************/

void gd32_rcu_periph_reset_enable(uint32_t periph)
{
  uint32_t regaddr;
  uint32_t regval;

  regaddr = GD32H7_RCU + (periph >> RCU_PERI_REG_SHIFT);
  regval  = getreg32(regaddr);
  regval |= (1 << (periph & 0x1f));
  putreg32(regval, regaddr);
}

void gd32_rcu_periph_reset_disable(uint32_t periph)
{
  uint32_t regaddr;
  uint32_t regval;

  regaddr = GD32H7_RCU + (periph >> RCU_PERI_REG_SHIFT);
  regval  = getreg32(regaddr);
  regval &= ~(1 << (periph & 0x1f));
  putreg32(regval, regaddr);
}

/****************************************************************************
 * Name: gd32_rcu_periph_reset
 *
 * Description:
 *   Pulse the peripheral reset (assert then release).
 *
 ****************************************************************************/

void gd32_rcu_periph_reset(uint32_t periph)
{
  gd32_rcu_periph_reset_enable(periph);
  gd32_rcu_periph_reset_disable(periph);
}

/****************************************************************************
 * Name: gd32_rcu_clock_freq_get
 *
 * Description:
 *   Return the frequency of one of the clock domains in Hz.
 *
 ****************************************************************************/

uint32_t gd32_rcu_clock_freq_get(enum gd32_rcu_clock_e domain)
{
  switch (domain)
    {
      case GD32_RCU_CKAHB:
        return GD32_AHB_FREQUENCY;

      case GD32_RCU_CKAPB1:
        return GD32_PCLK1_FREQUENCY;

      case GD32_RCU_CKAPB2:
        return GD32_PCLK2_FREQUENCY;

      case GD32_RCU_CKAPB3:
        return GD32_PCLK3_FREQUENCY;

      case GD32_RCU_CKAPB4:
        return GD32_PCLK4_FREQUENCY;

      case GD32_RCU_CKSYS:
      default:
        return GD32_SYSCLK_FREQUENCY;
    }
}

/****************************************************************************
 * Name: gd32_rcu_lxtal_config
 *
 * Description:
 *   Enable the 32.768kHz LXTAL oscillator and wait for it to stabilize.
 *
 * Returned Value:
 *   Zero (OK) on success; -ETIMEDOUT if the oscillator does not start.
 *
 ****************************************************************************/

int gd32_rcu_lxtal_config(void)
{
  volatile uint32_t timeout = 0x000fffff;

  modifyreg32(GD32H7_RCU_BDCTL, 0, RCU_BDCTL_LXTALEN);

  while (timeout-- > 0)
    {
      if (getreg32(GD32H7_RCU_BDCTL) & RCU_BDCTL_LXTALSTB)
        {
          return OK;
        }
    }

  return -ETIMEDOUT;
}

/****************************************************************************
 * Name: gd32_rcu_rtc_clock_config
 *
 * Description:
 *   Select the RTC kernel clock (LXTAL) and enable the RTC clock in the
 *   backup domain.
 *
 ****************************************************************************/

void gd32_rcu_rtc_clock_config(void)
{
  uint32_t regval;

  regval = getreg32(GD32H7_RCU_BDCTL);

  /* RTCSRC = 00: LXTAL is the RTC clock source */

  regval &= ~RCU_BDCTL_RTCSRC;
  regval |= RCU_BDCTL_RTCEN;
  putreg32(regval, GD32H7_RCU_BDCTL);
}

/****************************************************************************
 * Name: gd32_rcu_clock_monitor_enable
 *
 * Description:
 *   Enable/disable the HXTAL clock monitor (CKM).  On failure the hardware
 *   switches the system clock to IRC64M and the NMI handler is invoked.
 *
 ****************************************************************************/

void gd32_rcu_clock_monitor_enable(bool enable)
{
  uint32_t regval = getreg32(GD32H7_RCU_CTL);

  if (enable)
    {
      regval |= RCU_CTL_CKMEN;
    }
  else
    {
      regval &= ~RCU_CTL_CKMEN;
    }

  putreg32(regval, GD32H7_RCU_CTL);
}
