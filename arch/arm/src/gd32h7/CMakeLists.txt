# ##############################################################################
# arch/arm/src/gd32h7/CMakeLists.txt
#
# SPDX-License-Identifier: Apache-2.0
#
# Licensed to the Apache Software Foundation (ASF) under one or more contributor
# license agreements.  See the NOTICE file distributed with this work for
# additional information regarding copyright ownership.  The ASF licenses this
# file to you under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#
# ##############################################################################

list(
  APPEND
  SRCS
  gd32h7xx_allocateheap.c
  gd32h7xx_start.c
  gd32h7xx_rcu.c
  gd32h7xx_serial.c
  gd32h7xx_lowputc.c
  gd32h7xx_gpio.c
  gd32h7xx_syscfg.c
  gd32h7xx_irq.c
  gd32h7xx_exti.c)

if(CONFIG_GD32H7_DMA)
  list(APPEND SRCS gd32h7xx_dma.c)
endif()

if(CONFIG_GD32H7_SPI)
  list(APPEND SRCS gd32h7xx_spi.c)
endif()

if(CONFIG_GD32H7_I2C)
  list(APPEND SRCS gd32h7xx_i2c.c)
endif()

if(CONFIG_GD32H7_TIMER)
  list(APPEND SRCS gd32h7xx_tim.c)
endif()

if(CONFIG_GD32H7_PWM)
  list(APPEND SRCS gd32h7xx_pwm.c)
endif()

if(CONFIG_GD32H7_FWDGT)
  list(APPEND SRCS gd32h7xx_wdg.c)
endif()

if(CONFIG_GD32H7_CRC)
  list(APPEND SRCS gd32h7xx_crc.c)
endif()

if(CONFIG_GD32H7_TRNG)
  list(APPEND SRCS gd32h7xx_trng.c)
endif()

if(CONFIG_GD32H7_PMU)
  list(APPEND SRCS gd32h7xx_pmu.c)
endif()

if(CONFIG_GD32H7_ADC)
  list(APPEND SRCS gd32h7xx_adc.c)
endif()

if(CONFIG_GD32H7_DAC)
  list(APPEND SRCS gd32h7xx_dac.c)
endif()

if(CONFIG_GD32H7_RTC)
  list(APPEND SRCS gd32h7xx_rtc.c)
endif()

if(CONFIG_GD32H7_CAN)
  list(APPEND SRCS gd32h7xx_can.c)
endif()

if(CONFIG_GD32H7_FMC)
  list(APPEND SRCS gd32h7xx_fmc.c)
endif()

if(CONFIG_GD32H7_SDIO)
  list(APPEND SRCS gd32h7xx_sdio.c)
endif()

if(NOT CONFIG_SCHED_TICKLESS)
  list(APPEND SRCS gd32h7xx_timerisr.c)
endif()

if(NOT CONFIG_ARCH_IDLE_CUSTOM)
  list(APPEND SRCS gd32h7xx_idle.c)
endif()

target_sources(arch PRIVATE ${SRCS})
