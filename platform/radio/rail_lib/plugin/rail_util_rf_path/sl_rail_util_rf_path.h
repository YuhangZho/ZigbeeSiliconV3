/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_RAIL_UTIL_RF_PATH_H
#define SL_RAIL_UTIL_RF_PATH_H

#include "rail.h"

/**
 * Initialize the RAIL RF Path Utility.
 *
 * @note: This function should be called during application initialization.
 */
void sl_rail_util_rf_path_init(void);

/**
 * Get the antenna configuration.
 *
 * @param[out] antennaConfig The antenna configuration being used.
 */
void sl_rail_util_rf_path_get_antenna_config(RAIL_AntennaConfig_t *antennaConfig);

/**
 * Initialize the RX antenna options.
 *
 * @param[out] rxOptions Update the RX antenna options in the provided
 * RX option feature mask.
 *
 * @note: Only the RX antenna option bits are altered by this function - other
 * options are left untouched.
 */
void sl_rail_util_rf_path_init_rx_options(RAIL_RxOptions_t *rxOptions);

/**
 * Initialize the TX antenna options.
 *
 * @param[out] txOptions Update the TX antenna options in the provided
 * TX option feature mask.
 *
 * @note: Only the TX antenna option bits are altered by this function - other
 * options are left untouched.
 */
void sl_rail_util_rf_path_init_tx_options(RAIL_TxOptions_t *txOptions);

#endif // SL_RAIL_UTIL_RF_PATH_H
