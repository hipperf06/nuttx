/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_i2c.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_GD32H7XX_I2C_H
#define __ARCH_ARM_SRC_GD32H7_GD32H7XX_I2C_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <nuttx/i2c/i2c_master.h>

#include "chip.h"
#include "hardware/gd32h7xx_i2c.h"

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifndef __ASSEMBLY__

#if defined(__cplusplus)
extern "C"
{
#endif

/****************************************************************************
 * Name: gd32_i2cbus_initialize
 *
 * Description:
 *   Initialize one I2C bus.
 *
 * Input Parameters:
 *   port - The I2C bus to initialize (0-3)
 *
 * Returned Value:
 *   Valid I2C device structure reference on success; NULL on failure.
 *
 ****************************************************************************/

FAR struct i2c_master_s *gd32_i2cbus_initialize(int port);

/****************************************************************************
 * Name: gd32_i2cbus_uninitialize
 *
 * Description:
 *   Un-initialize one I2C bus.
 *
 ****************************************************************************/

int gd32_i2cbus_uninitialize(FAR struct i2c_master_s *dev);

#if defined(__cplusplus)
}
#endif
#endif /* __ASSEMBLY__ */
#endif /* __ARCH_ARM_SRC_GD32H7_GD32H7XX_I2C_H */
