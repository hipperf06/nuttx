/****************************************************************************
 * boards/arm/gd32h7/gd32h757zm-start/src/gd32h757zm_start.h
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

#ifndef __BOARDS_ARM_GD32H7_GD32H757ZM_START_SRC_GD32H757ZM_START_H
#define __BOARDS_ARM_GD32H7_GD32H757ZM_START_SRC_GD32H757ZM_START_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/compiler.h>

#include <stdint.h>

#include "gd32h7xx_gpio.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* procfs File System *******************************************************/

#ifdef CONFIG_FS_PROCFS
#  ifdef CONFIG_NSH_PROC_MOUNTPOINT
#    define GD32_PROCFS_MOUNTPOINT CONFIG_NSH_PROC_MOUNTPOINT
#  else
#    define GD32_PROCFS_MOUNTPOINT "/proc"
#  endif
#endif

/* GD32H757ZM GPIO Pin Definitions ******************************************/

/* LEDs
 *
 * FMUv6C wiring: red on PD10, blue on PD11, both active low (anode to 3.3V),
 * so the pins start driven high (LED off).
 */

#define GPIO_LED_RED \
  (GPIO_CFG_MODE_OUTPUT | GPIO_CFG_OUTPUT_SET | GPIO_CFG_PUPD_NONE | \
   GPIO_CFG_SPEED_12MHZ | GPIO_CFG_PORT_D | GPIO_CFG_PIN_10)

#define GPIO_LED_BLUE \
  (GPIO_CFG_MODE_OUTPUT | GPIO_CFG_OUTPUT_SET | GPIO_CFG_PUPD_NONE | \
   GPIO_CFG_SPEED_12MHZ | GPIO_CFG_PORT_D | GPIO_CFG_PIN_11)

#define GPIO_LED GPIO_LED_RED

#define LED_DRIVER_PATH "/dev/userleds"

/****************************************************************************
 * Public Types
 ****************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: gd32_bringup
 *
 * Description:
 *   Perform board-specific device driver initialization.  Called either
 *   from board_late_initialize() (if CONFIG_BOARD_LATE_INITIALIZE is set)
 *   or from the application start-up code.
 *
 ****************************************************************************/

int gd32_bringup(void);

#endif /* __ASSEMBLY__ */
#endif /* __BOARDS_ARM_GD32H7_GD32H757ZM_START_SRC_GD32H757ZM_START_H */
