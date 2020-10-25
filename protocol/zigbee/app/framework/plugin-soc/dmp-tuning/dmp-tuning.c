/***************************************************************************//**
 * @file
 * @brief Plugin code for better DMP Tuning and Testing.
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

#include "app/framework/include/af.h"
#include "dmp-tuning-profiles.h"

uint32_t railScheduledEventCntr = 0;
uint32_t railUnscheduledEventCntr = 0;

void emberAfPluginBleGetConfigCallback(sl_bt_configuration_t *config)
{
  // Make sure BLE scheduler priorities are set up according to the plugin options.
  config->bluetooth.linklayer_priorities->rail_mapping_offset =
    EMBER_AF_PLUGIN_DMP_TUNING_BLE_RAIL_MAPPING_OFFSET;
  config->bluetooth.linklayer_priorities->rail_mapping_range =
    EMBER_AF_PLUGIN_DMP_TUNING_BLE_RAIL_MAPPING_RANGE;
  config->bluetooth.linklayer_priorities->scan_min =
    EMBER_AF_PLUGIN_DMP_TUNING_BLE_SCAN_PRIORITY_MIN;
  config->bluetooth.linklayer_priorities->scan_max =
    EMBER_AF_PLUGIN_DMP_TUNING_BLE_SCAN_PRIORITY_MAX;
  config->bluetooth.linklayer_priorities->adv_min =
    EMBER_AF_PLUGIN_DMP_TUNING_BLE_ADVERTISEMENT_PRIORITY_MIN;
  config->bluetooth.linklayer_priorities->adv_max =
    EMBER_AF_PLUGIN_DMP_TUNING_BLE_ADVERTISEMENT_PRIORITY_MAX;
  config->bluetooth.linklayer_priorities->init_min =
    EMBER_AF_PLUGIN_DMP_TUNING_BLE_CONNECTION_INIT_PRIORITY_MIN;
  config->bluetooth.linklayer_priorities->init_max =
    EMBER_AF_PLUGIN_DMP_TUNING_BLE_CONNECTION_INIT_PRIORITY_MAX;
  config->bluetooth.linklayer_priorities->conn_min =
    EMBER_AF_PLUGIN_DMP_TUNING_BLE_CONNECTION_PRIORITY_MIN;
  config->bluetooth.linklayer_priorities->conn_max =
    EMBER_AF_PLUGIN_DMP_TUNING_BLE_CONNECTION_PRIORITY_MAX;

  emberAfCorePrintln("BLE scheduler priorities initialized.");
}

void emberAfPluginDmpTuningInitCallback(void)
{
  // Configure the zigbee TX and RX priorities
  EmberMultiprotocolPriorities pri;
  pri.backgroundRx = EMBER_AF_PLUGIN_DMP_TUNING_ZB_BACKGROUND_RX_PRIORIY;
  pri.activeRx = EMBER_AF_PLUGIN_DMP_TUNING_ZB_ACTIVE_RX_PRIORITY;
  pri.tx = EMBER_AF_PLUGIN_DMP_TUNING_ZB_ACTIVE_TX_PRIORITY;
  if (emberRadioSetSchedulerPriorities(&pri) == EMBER_SUCCESS) {
    emberAfCorePrintln("Zigbee TX/RX priorities initialized.");
  } else {
    emberAfCorePrintln("Warning: zigbee TX/RX priorities cannot be initialized.");
  }
}

void emRadioConfigScheduledCallback(bool scheduled)
{
  if (scheduled) {
    railScheduledEventCntr++;
  } else {
    railUnscheduledEventCntr++;
  }
}
