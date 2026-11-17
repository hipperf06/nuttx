/****************************************************************************
 * arch/arm/src/gd32h7/gd32h7xx_spi.h
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

#ifndef __ARCH_ARM_SRC_GD32H7_GD32H7XX_SPI_H
#define __ARCH_ARM_SRC_GD32H7_GD32H7XX_SPI_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <nuttx/spi/spi.h>

#include "chip.h"
#include "hardware/gd32h7xx_spi.h"

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifndef __ASSEMBLY__

#if defined(__cplusplus)
extern "C"
{
#endif

/****************************************************************************
 * Name: gd32_spibus_initialize
 *
 * Description:
 *   Initialize the selected SPI bus.  The SPI buses are enabled and
 *   configured either by the board logic or by this function.
 *
 * Input Parameters:
 *   port - The SPI bus to initialize (0-5)
 *
 * Returned Value:
 *   Valid SPI device structure reference on success; NULL on failure.
 *
 ****************************************************************************/

FAR struct spi_dev_s *gd32_spibus_initialize(int port);

/****************************************************************************
 * Name: gd32_spibus_uninitialize
 *
 * Description:
 *   Un-initialize the selected SPI bus.
 *
 ****************************************************************************/

int gd32_spibus_uninitialize(FAR struct spi_dev_s *dev);

#if defined(__cplusplus)
}
#endif
#endif /* __ASSEMBLY__ */
#endif /* __ARCH_ARM_SRC_GD32H7_GD32H7XX_SPI_H */
