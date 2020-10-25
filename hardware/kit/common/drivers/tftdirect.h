/***************************************************************************//**
 * @file
 * @brief SSD2119 TFT initialization and setup for Direct Drive mode
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef __TFTDIRECT_H
#define __TFTDIRECT_H

#ifndef SL_SUPRESS_DEPRECATION_WARNINGS_SDK_3_0
#warning "The SSD119 TFT direct mode init is deprecated and marked for removal in a later release."
#endif

#include <stdbool.h>
#include "em_device.h"
#include "em_ebi.h"

/***************************************************************************//**
 * @addtogroup kitdrv
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup Tft
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void TFT_DirectGPIOConfig(void);
bool TFT_DirectInit(const EBI_TFTInit_TypeDef *tftInit);

#ifdef __cplusplus
}
#endif

/** @} (end group Tft) */
/** @} (end group kitdrv) */

#endif
