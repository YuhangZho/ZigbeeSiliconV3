/***************************************************************************//**
 * @file
 * @brief CLI for the DMO Tuning and Testing plugin.
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
#include "app/framework/util/af-main.h"
#include "app/util/serial/command-interpreter2.h"

#include "ble-interface.h"
#include "sl_bt_ll_config.h"
#include "platform/radio/mac/lower-mac.h"
#include "dmp-tuning.h"
#include "dmp-tuning-profiles.h"

void emRadioEnableConfigScheduledCallback(bool enable);
bool emRadioConfigScheduledCallbackEnabled(void);
void emRadioConfigScheduledCallback(bool scheduled);

// Zigbee related CLI commands
// Zigbee shceduler priorities
void emberAfPluginDmpTuningSetZigbeeSchedulerPrioritiesCommand(void)
{
  EmberMultiprotocolPriorities pri;
  pri.backgroundRx = emberUnsignedCommandArgument(0);
  pri.activeRx = emberUnsignedCommandArgument(1);
  pri.tx = emberUnsignedCommandArgument(2);
  if (emberRadioSetSchedulerPriorities(&pri) == EMBER_SUCCESS) {
    sl_mac_lower_mac_radio_sleep();
    sl_mac_lower_mac_radio_wakeup();
    emberAfCorePrintln("DmpTuning - SetZigbeeSchedulerPrioritiesCommand: success");
  } else {
    emberAfCorePrintln("invalid call");
  }
}

void emberAfPluginDmpTuningGetZigbeeSchedulerPrioritiesCommand(void)
{
  EmberMultiprotocolPriorities pri;
  emberRadioGetSchedulerPriorities(&pri);
  emberAfCorePrintln("DmpTuning - GetZigbeeSchedulerPrioritiesCommand: backgroundRx:%d activeRx:%d tx:%d",
                     pri.backgroundRx,
                     pri.activeRx,
                     pri.tx);
}

// Zigbee Slip Time
void emberAfPluginDmpTuningSetZigbeeSlipTimeCommand(void)
{
  EmberStatus status = emberRadioSetSchedulerSliptime((uint32_t)emberUnsignedCommandArgument(0));
  if (status == EMBER_SUCCESS) {
    emberAfCorePrintln("DmpTuning - SetZigbeeSlipTime: success");
  } else {
    emberAfCorePrintln("DmpTuning - SetZigbeeSlipTime should only be used in multiprotocol");
  }
}

void emberAfPluginDmpTuningGetZigbeeSlipTimeCommand(void)
{
  uint32_t slipTime;
  EmberStatus status = emberRadioGetSchedulerSliptime(&slipTime);
  if (status == EMBER_SUCCESS) {
    emberAfCorePrintln("DmpTuning - GetZigbeeSlipTime: slip_time:%d", slipTime);
  } else {
    emberAfCorePrintln("DmpTuning - GetZigbeeSlipTime should only be used in multiprotocol");
  }
}

// BLE related CLI commands
// BLE Scan Priorities
void emberAfPluginDmpTuningSetBleScanPrioritiesCommand(void)
{
  sl_bt_bluetooth_ll_priorities *ll_priorities = emberAfPluginBleGetLlPrioritiesConfig();

  ll_priorities->scan_min = emberUnsignedCommandArgument(0);
  ll_priorities->scan_max = emberUnsignedCommandArgument(1);

  emberAfCorePrintln("DmpTuning - SetBleScanPriorities: success");
}

void emberAfPluginDmpTuningGetBleScanPrioritiesCommand(void)
{
  sl_bt_bluetooth_ll_priorities *ll_priorities = emberAfPluginBleGetLlPrioritiesConfig();

  emberAfCorePrintln("DmpTuning - GetBleScanPriorities: scan_min:%d scan_max:%d",
                     ll_priorities->scan_min,
                     ll_priorities->scan_max);
}

// BLE Advertisement Priorities
void emberAfPluginDmpTuningSetBleAdvertisementPrioritiesCommand(void)
{
  sl_bt_bluetooth_ll_priorities *ll_priorities = emberAfPluginBleGetLlPrioritiesConfig();

  ll_priorities->adv_min = emberUnsignedCommandArgument(0);
  ll_priorities->adv_max = emberUnsignedCommandArgument(1);

  emberAfCorePrintln("DmpTuning - SetBleAdvertisementPriorities: success");
}

void emberAfPluginDmpTuningGetBleAdvertisementPrioritiesCommand(void)
{
  sl_bt_bluetooth_ll_priorities *ll_priorities = emberAfPluginBleGetLlPrioritiesConfig();

  emberAfCorePrintln("DmpTuning - GetBleAdvertisementPriorities: adv_min:%d adv_max:%d",
                     ll_priorities->adv_min,
                     ll_priorities->adv_max);
}

// BLE Connection Init Priorities
void emberAfPluginDmpTuningSetBleConnectionInitPrioritiesCommand(void)
{
  sl_bt_bluetooth_ll_priorities *ll_priorities = emberAfPluginBleGetLlPrioritiesConfig();

  ll_priorities->init_min = emberUnsignedCommandArgument(0);
  ll_priorities->init_max = emberUnsignedCommandArgument(1);

  emberAfCorePrintln("DmpTuning - SetBleConnectionInitPriorities: success");
}

void emberAfPluginDmpTuningGetBleConnectionInitPrioritiesCommand(void)
{
  sl_bt_bluetooth_ll_priorities *ll_priorities = emberAfPluginBleGetLlPrioritiesConfig();

  emberAfCorePrintln("DmpTuning - GetBleConnectionInitPriorities: init_min:%d init_max:%d",
                     ll_priorities->init_min,
                     ll_priorities->init_max);
}

// BLE Connection Priorities
void emberAfPluginDmpTuningSetBleConnectionPrioritiesCommand(void)
{
  sl_bt_bluetooth_ll_priorities *ll_priorities = emberAfPluginBleGetLlPrioritiesConfig();

  ll_priorities->conn_min = emberUnsignedCommandArgument(0);
  ll_priorities->conn_max = emberUnsignedCommandArgument(1);

  emberAfCorePrintln("DmpTuning - SetBleConnectionPriorities: success");
}

void emberAfPluginDmpTuningGetBleConnectionPrioritiesCommand(void)
{
  sl_bt_bluetooth_ll_priorities *ll_priorities = emberAfPluginBleGetLlPrioritiesConfig();

  emberAfCorePrintln("DmpTuning - GetBleConnectionPriorities: conn_min:%d conn_max:%d",
                     ll_priorities->conn_min,
                     ll_priorities->conn_max);
}

// BLE RAIL Mapping
void emberAfPluginDmpTuningSetBleRailMappingCommand(void)
{
  sl_bt_bluetooth_ll_priorities *ll_priorities = emberAfPluginBleGetLlPrioritiesConfig();

  ll_priorities->rail_mapping_offset = emberUnsignedCommandArgument(0);
  ll_priorities->rail_mapping_range = emberUnsignedCommandArgument(1);

  emberAfCorePrintln("DmpTuning - SetBleRailMapping: success");
}

void emberAfPluginDmpTuningGetBleRailMappingCommand(void)
{
  sl_bt_bluetooth_ll_priorities *ll_priorities = emberAfPluginBleGetLlPrioritiesConfig();

  emberAfCorePrintln("DmpTuning - GetBleRailMapping: rail_mapping_offset:%d rail_mapping_range:%d",
                     ll_priorities->rail_mapping_offset,
                     ll_priorities->rail_mapping_range);
}

// CLI for protocol switch related RAIL events.
void emberAfPluginDmpTuningGetRailScheduledEventCounters(void)
{
  emberAfCorePrintln("Scheduled event counter:%d Unscheduled event counter:%d",
                     railScheduledEventCntr,
                     railUnscheduledEventCntr);
}

void emberAfPluginDmpTuningEnableRailConfigSchedulerEvents(void)
{
  emRadioEnableConfigScheduledCallback(true);
  emberAfCorePrintln("DmpTuning - EnableRailConfigSchedulerEvents: success");
}

void emberAfPluginDmpTuningDisableRailConfigSchedulerEvents(void)
{
  emRadioEnableConfigScheduledCallback(false);
  emberAfCorePrintln("DmpTuning - DisableRailConfigSchedulerEvents: success");
}

void emberAfPluginDmpTuningGetRailConfigSchedulerEventsEnabled(void)
{
  emberAfCorePrintln("RAIL config scheduler event enabled: %d",
                     emRadioConfigScheduledCallbackEnabled());
}
