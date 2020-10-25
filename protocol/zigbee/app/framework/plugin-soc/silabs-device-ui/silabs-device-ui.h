/***************************************************************************//**
 * @file
 * @brief Definitions for the Silabs Device UI plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SILABS_SILABS_DEVICE_UI_H
#define SILABS_SILABS_DEVICE_UI_H

//------------------------------------------------------------------------------
// Plugin public function declarations.

/** @brief Sets the EZ-Mode commissioning clusters.
 *
 * If EZ-Mode commissioning is used with the application and the device will be
 * the initiator, this function can be used to set the clusters for which
 * binding table entries will be created during the commissioning process. If
 * the EZ-Mode commissioning plugin is not included, this function will have no
 * effect.
 *
 * @param clusterIds The array of clusters to create binding table entries for.
 * This array will be copied by the plugin, and does not need to be stored by
 * invoker once this function returns.
 * @param clusterIdsLength The number of elements in the clusterIds array. This
 * is bound by the EMBER_AF_PLUGIN_SILABS_DEVICE_UI_EZ_MODE_MAX_CLUSTERS macro,
 * and if a value greater than that macro is passed in, only the first
 * EMBER_AF_PLUGIN_SILABS_DEVICE_UI_EZ_MODE_MAX_CLUSTERS number of entries will
 * be used during EZMODE commissioning, and a value of EMBER_TABLE_FULL will be
 * returned.
 *
 * @return The success or failure of the operation.
 */
EmberStatus emberAfPluginSilabsDeviceUiSetEzModeClusters(
  const EmberAfClusterId *clusterIds,
  uint8_t clusterIdsLength);

/** @brief Blink the Network Found LED pattern.
 */
void emberAfPluginSilabsDeviceUiLedNetworkFoundBlink(void);

/** @brief Blink the Network Lost LED pattern.
 */
void emberAfPluginSilabsDeviceUiLedNetworkLostBlink(void);

/** @brief Blink the Network Searching LED pattern.
 */
void emberAfPluginSilabsDeviceUiLedNetworkSearchingBlink(void);

/** @brief Blink the Network Identify LED pattern.
 */
void emberAfPluginSilabsDeviceUiLedIdentifyBlink(void);

/** @brief Blink the Network Proactive Rejoin LED pattern.
 */
void emberAfPluginSilabsDeviceUiLedProactiveRejoinBlink(void);

#endif //__SILABS_DEVICE_UI_H__
