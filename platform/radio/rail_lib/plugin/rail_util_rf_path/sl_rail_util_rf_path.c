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

#include "rail.h"
#include "sl_rail_util_rf_path.h"
#include "sl_rail_util_rf_path_config.h"

void sl_rail_util_rf_path_init_rx_options(RAIL_RxOptions_t *rxOptions)
{
  if (NULL == rxOptions) {
    return;
  }
#if (SL_RAIL_UTIL_RF_PATH_EXT_RX_ANT_DIV_MODE \
     != SL_RAIL_UTIL_RF_PATH_ANT_DIV_DISABLED)
  *rxOptions = (*rxOptions
                & ~(RAIL_RX_OPTION_ANTENNA0 | RAIL_RX_OPTION_ANTENNA1))
               | ((((RAIL_RxOptions_t)SL_RAIL_UTIL_RF_PATH_EXT_RX_ANT_DIV_MODE)
                   << RAIL_RX_OPTION_ANTENNA0_SHIFT)
                  & (RAIL_RX_OPTION_ANTENNA0 | RAIL_RX_OPTION_ANTENNA1));
#endif
}

void sl_rail_util_rf_path_init_tx_options(RAIL_TxOptions_t *txOptions)
{
  if (NULL == txOptions) {
    return;
  }
#if (SL_RAIL_UTIL_RF_PATH_EXT_TX_ANT_DIV_MODE \
     != SL_RAIL_UTIL_RF_PATH_ANT_DIV_DISABLED)
  *txOptions = (*txOptions
                & ~(RAIL_TX_OPTION_ANTENNA0 | RAIL_TX_OPTION_ANTENNA1))
               | ((((RAIL_TxOptions_t)SL_RAIL_UTIL_RF_PATH_EXT_TX_ANT_DIV_MODE)
                   << RAIL_TX_OPTION_ANTENNA0_SHIFT)
                  & (RAIL_TX_OPTION_ANTENNA0 | RAIL_TX_OPTION_ANTENNA1));
#endif
}

void sl_rail_util_rf_path_get_antenna_config(RAIL_AntennaConfig_t *antennaConfig)
{
  if (NULL == antennaConfig) {
    return;
  }
#if (defined(SL_RAIL_UTIL_RF_PATH_ANT_DIV_ANT0_PORT) \
  && defined(SL_RAIL_UTIL_RF_PATH_ANT_DIV_ANT0_PIN))
  antennaConfig->ant0PinEn = true;
  antennaConfig->ant0Port = (uint8_t)SL_RAIL_UTIL_RF_PATH_ANT_DIV_ANT0_PORT;
  antennaConfig->ant0Pin  = SL_RAIL_UTIL_RF_PATH_ANT_DIV_ANT0_PIN;
#if defined(SL_RAIL_UTIL_RF_PATH_ANT_DIV_ANT0_LOC) // efr32xg1x only
  antennaConfig->ant0Loc  = SL_RAIL_UTIL_RF_PATH_ANT_DIV_ANT0_LOC;
#endif // ant0 loc
#endif // ant0 port & pin

#if (defined(SL_RAIL_UTIL_RF_PATH_ANT_DIV_ANT1_PORT) \
  && defined(SL_RAIL_UTIL_RF_PATH_ANT_DIV_ANT1_PIN))
  antennaConfig->ant1PinEn = true;
  antennaConfig->ant1Port = (uint8_t)SL_RAIL_UTIL_RF_PATH_ANT_DIV_ANT1_PORT;
  antennaConfig->ant1Pin  = SL_RAIL_UTIL_RF_PATH_ANT_DIV_ANT1_PIN;
#if defined(SL_RAIL_UTIL_RF_PATH_ANT_DIV_ANT1_LOC) // efr32xg1x only
  antennaConfig->ant1Loc  = SL_RAIL_UTIL_RF_PATH_ANT_DIV_ANT1_LOC;
#endif // ant1 loc
#endif // ant1 port & pin

#if defined(SL_RAIL_UTIL_RF_PATH_INT_RF_PATH_MODE) // efr32xg2x chip-specific
  antennaConfig->defaultPath = SL_RAIL_UTIL_RF_PATH_INT_RF_PATH_MODE;
#endif // internal rf path
}

void sl_rail_util_rf_path_init(void)
{
  RAIL_AntennaConfig_t antennaConfig = { 0 };
  sl_rail_util_rf_path_get_antenna_config(&antennaConfig);
  (void) RAIL_ConfigAntenna(RAIL_EFR32_HANDLE, &antennaConfig);
}
