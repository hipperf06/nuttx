/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_pmu.c
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

#include <nuttx/irq.h>
#include <nuttx/arch.h>

#include "arm_internal.h"
#include "nvic.h"

#include "hardware/gd32h7xx_pmu.h"
#include "gd32h7xx_pmu.h"
#include "gd32h7xx_rcu.h"

#ifdef CONFIG_GD32H7_PMU

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Cortex-M7 System Control Register (SCB->SCR) */

#define GD32H7_SCB_SCR               0xe000ed10

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_pmu_initialize
 *
 * Description:
 *   Enable the PMU peripheral clock and unlock the backup domain so that
 *   the RTC and the backup registers can be written.
 *
 ****************************************************************************/

void gd32_pmu_initialize(void)
{
  gd32_rcu_periph_clock_enable(RCU_PMU_EN);

  /* Unlock the backup domain */

  modifyreg32(GD32H7_PMU_CTL0, 0, PMU_CTL0_BKPWEN);
}

/****************************************************************************
 * Name: gd32_pmu_clearflags
 ****************************************************************************/

void gd32_pmu_clearflags(void)
{
  modifyreg32(GD32H7_PMU_CTL0, 0,
              PMU_CTL0_WURST | PMU_CTL0_STBRST);
}

/****************************************************************************
 * Name: gd32_pmu_wakeup_config
 *
 * Description:
 *   Enable the given wakeup pin(s) (WKUPs).  The argument is a mask of
 *   PMU_CS_WUPENx bits.
 *
 ****************************************************************************/

void gd32_pmu_wakeup_config(uint32_t wkups)
{
  modifyreg32(GD32H7_PMU_CS, 0, wkups);
}

/****************************************************************************
 * Name: gd32_pmu_setregulator
 *
 * Description:
 *   Select the LDO or the direct SMPS (bypass) supply for the core domain.
 *
 ****************************************************************************/

void gd32_pmu_setregulator(bool bypass)
{
  if (bypass)
    {
      modifyreg32(GD32H7_PMU_CTL2, 0, PMU_CTL2_BYPASS);
    }
  else
    {
      modifyreg32(GD32H7_PMU_CTL2, PMU_CTL2_BYPASS, 0);
    }
}

/****************************************************************************
 * Name: gd32_pmu_standby
 *
 * Description:
 *   Enter standby mode.  This function does not return; the MCU restarts
 *   when a wakeup event occurs.
 *
 ****************************************************************************/

void gd32_pmu_standby(void)
{
  irqstate_t flags;

  flags = enter_critical_section();

  /* Clear the wakeup and standby flags, then arm standby mode */

  modifyreg32(GD32H7_PMU_CTL0, 0, PMU_CTL0_WURST | PMU_CTL0_STBRST);
  modifyreg32(GD32H7_PMU_CTL0, 0, PMU_CTL0_STBMOD);

  /* Request deep sleep and wait for the event */

  putreg32(NVIC_SYSCON_SLEEPDEEP, GD32H7_SCB_SCR);
  __asm__ __volatile__ ("dsb\n\t"
                        "wfi\n\t"
                        "isb\n\t"
                        : : : "memory");

  /* The debugger or a spurious wakeup may return here */

  leave_critical_section(flags);
}

#endif /* CONFIG_GD32H7_PMU */
