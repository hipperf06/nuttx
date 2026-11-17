/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_GD32H7XX_H
#define __ARCH_ARM_SRC_GD32H7_GD32H7XX_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>

#include "arm_internal.h"

/* Peripherals **************************************************************/

#include "chip.h"
#include "gd32h7xx_irq.h"
#include "gd32h7xx_rcu.h"
#include "gd32h7xx_gpio.h"
#include "gd32h7xx_syscfg.h"
#include "gd32h7xx_exti.h"
#include "gd32h7xx_usart.h"
#include "gd32h7xx_dma.h"
#include "gd32h7xx_spi.h"
#include "gd32h7xx_i2c.h"
#include "gd32h7xx_tim.h"
#include "gd32h7xx_pwm.h"
#include "gd32h7xx_crc.h"
#include "gd32h7xx_wdg.h"
#include "gd32h7xx_pmu.h"
#include "gd32h7xx_adc.h"
#include "gd32h7xx_dac.h"
#include "gd32h7xx_rtc.h"
#include "gd32h7xx_can.h"
#include "gd32h7xx_fmc.h"
#include "gd32h7xx_sdio.h"
#include "gd32h7xx_lowputc.h"

#endif /* __ARCH_ARM_SRC_GD32H7_GD32H7XX_H */
