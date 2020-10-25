/***************************************************************************//**
 * @file
 * @brief CLI for the BLE plugin.
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
#include "app/framework/plugin/drlc/demand-response-load-control.h"
#include "app/framework/plugin/drlc/load-control-event-table.h"

#include "sl_bt_rtos_adaptation.h"

#define LE_GAP_MAX_DISCOVERY_MODE       scanner_discover_observation
#define MAX_CHAR_SIZE                   252
static uint8_t adv_handle = 0xFF;

void emberAfPluginBleHelloCommand(void)
{
  sl_status_t status = sl_bt_system_hello();

  emberAfCorePrintln("BLE hello: %s",
                     (status == SL_STATUS_OK) ? "success" : "error");
}

void emberAfPluginBleGetAddressCommand(void)
{
  bd_addr ble_address;
  uint8_t type;

  sl_status_t status = sl_bt_system_get_identity_address(&ble_address, &type);

  emberAfCorePrintln("BLE address: [%x %x %x %x %x %x]",
                     ble_address.addr[5], ble_address.addr[4],
                     ble_address.addr[3], ble_address.addr[2],
                     ble_address.addr[1], ble_address.addr[0]);
}

void emberAfPluginBleSetAdvertisementParamsCommand(void)
{
  uint16_t min_interval = emberUnsignedCommandArgument(0);
  uint16_t max_interval = emberUnsignedCommandArgument(1);
  uint8_t channel_map = emberUnsignedCommandArgument(2);

  sl_status_t status = sl_bt_advertiser_set_channel_map(0, channel_map);

  if (status != SL_STATUS_OK) {
    return;
  }

  status = sl_bt_advertiser_set_timing(0,  // handle
                                       min_interval,
                                       max_interval,
                                       0,  // duration : continue advertisement until stopped
                                       0); // max_events :continue advertisement until stopped

  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("success");
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}

void emberAfPluginBleSetGapModeCommand(void)
{
  uint8_t discoverable_mode = emberUnsignedCommandArgument(0);
  uint8_t connectable_mode = emberUnsignedCommandArgument(1);

  if (adv_handle == 0xFF) {
    sl_bt_advertiser_create_set(&adv_handle);
  }

  if (discoverable_mode > advertiser_user_data
      || connectable_mode > advertiser_connectable_non_scannable) {
    emberAfCorePrintln("Invalid params");
    return;
  }
  sl_status_t status = sl_bt_advertiser_start(adv_handle,
                                              discoverable_mode,
                                              connectable_mode);

  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("success");
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}

void emberAfPluginBleSetBt5GapModeCommand(void)
{
  uint8_t discoverable_mode = emberUnsignedCommandArgument(1);
  uint8_t connectable_mode = emberUnsignedCommandArgument(2);
  uint16_t max_events = emberUnsignedCommandArgument(3);
  uint8_t address_type = emberUnsignedCommandArgument(4); //This is not used?
  if (adv_handle == 0xFF) {
    sl_bt_advertiser_create_set(&adv_handle);
  }

  if (discoverable_mode > advertiser_user_data
      || connectable_mode > advertiser_connectable_non_scannable) {
    emberAfCorePrintln("Invalid params");
    return;
  }
  sl_status_t status = sl_bt_advertiser_set_timing(adv_handle,
                                                   (100 / 0.625), //100ms min adv interval in terms of 0.625ms
                                                   (100 / 0.625), //100ms max adv interval in terms of 0.625ms
                                                   0, // duration : continue advertisement until stopped
                                                   max_events);
  if (status == SL_STATUS_OK) {
    status = sl_bt_advertiser_start(adv_handle,
                                    discoverable_mode,
                                    connectable_mode);
  }

  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("success");
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}

void emberAfPluginBleGapDiscoverCommand(void)
{
  uint8_t discovery_mode = emberUnsignedCommandArgument(0);
  if (discovery_mode > scanner_discover_observation) {
    emberAfCorePrintln("Invalid params");
    return;
  }
  //preferred phy 1: 1M phy, 2: 2M phy, 4: 125k coded phy, 8: 500k coded phy
  sl_status_t status = sl_bt_scanner_start(gap_1m_phy, discovery_mode);

  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("success");
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}

void emberAfPluginBleGapConnectionOpenCommand(void)
{
  uint8_t address_type = emberUnsignedCommandArgument(1);
  bd_addr address;
  uint8_t connection_handle;
  sl_status_t status;
  emberAfCopyBigEndianEui64Argument(0, address.addr);

  if (address_type > gap_random_nonresolvable_address) {
    emberAfCorePrintln("Invalid params");
    return;
  }

  status = sl_bt_connection_open(address, address_type, gap_1m_phy, &connection_handle);

  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("success");
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}

void emberAfPluginBleGapSetConnectionParamsCommand(void)
{
  uint16_t min_interval = emberUnsignedCommandArgument(0);
  uint16_t max_interval = emberUnsignedCommandArgument(1);
  uint16_t slave_latency = emberUnsignedCommandArgument(2);
  uint16_t supervision_timeout = emberUnsignedCommandArgument(3);

  sl_status_t status = sl_bt_connection_set_default_parameters(min_interval,
                                                               max_interval,
                                                               slave_latency,
                                                               supervision_timeout,
                                                               0, // min_ce_length
                                                               0xFFFF); // max_ce_length

  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("success");
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}

void emberAfPluginBleConnectionSetParamsCommand(void)
{
  uint16_t connection_handle = emberUnsignedCommandArgument(0);
  uint16_t min_interval = emberUnsignedCommandArgument(1);
  uint16_t max_interval = emberUnsignedCommandArgument(2);
  uint16_t slave_latency = emberUnsignedCommandArgument(3);
  uint16_t supervision_timeout = emberUnsignedCommandArgument(4);

  sl_status_t status =  sl_bt_connection_set_parameters(connection_handle,
                                                        min_interval,
                                                        max_interval,
                                                        slave_latency,
                                                        supervision_timeout,
                                                        0, // min_ce_length
                                                        0xFFFF); // max_ce_length

  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("success");
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}

void emberAfPluginBleSecurityManagerIncreaseSecurityCommand(void)
{
  uint16_t connection_handle = emberUnsignedCommandArgument(0);

  sl_status_t status = sl_bt_sm_increase_security(connection_handle);

  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("success");
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}

void emberAfPluginBleSecurityManagerListAllBondingsCommand(void)
{
  sl_status_t status = sl_bt_sm_list_all_bondings();

  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("success");
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}

void emberAfPluginBleSecurityManagerDeleteBondingCommand(void)
{
  uint8_t bonding_handle = emberUnsignedCommandArgument(0);

  sl_status_t status = sl_bt_sm_delete_bonding(bonding_handle);

  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("success");
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}

void emberAfPluginBleSecurityManagerDeleteAllBondingsCommand(void)
{
  sl_status_t status = sl_bt_sm_delete_bondings();

  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("success");
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}

void emberAfPluginBleSecurityManagerSetBondableModeCommand(void)
{
  uint8_t bondable_mode = emberUnsignedCommandArgument(0);

  if (bondable_mode > 1) {
    emberAfCorePrintln("Invalid params");
    return;
  }

  sl_status_t status = sl_bt_sm_set_bondable_mode(bondable_mode);

  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("success");
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}

void emberAfPluginBleGattDiscoverPrimaryServicesCommand(void)
{
  uint8_t connection_handle = emberUnsignedCommandArgument(0);

  sl_status_t status = sl_bt_gatt_discover_primary_services(connection_handle);

  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("success");
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}

void emberAfPluginBleGattDiscoverCharacteristicsCommand(void)
{
  uint8_t connection_handle = emberUnsignedCommandArgument(0);
  uint32_t service_handle = emberUnsignedCommandArgument(1);

  sl_status_t status = sl_bt_gatt_discover_characteristics(connection_handle, service_handle);

  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("success");
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}

void emberAfPluginBleGattSetCharacteristicNotificationCommand(void)
{
  uint8_t connection_handle = emberUnsignedCommandArgument(0);
  uint16_t char_handle = emberUnsignedCommandArgument(1);
  uint8_t flags = emberUnsignedCommandArgument(2);

  sl_status_t status = sl_bt_gatt_set_characteristic_notification(connection_handle,
                                                                  char_handle,
                                                                  flags);
  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("success");
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}

void emberAfPluginBleGattWriteCharacteristicValueCommand(void)
{
  uint8_t connection_handle = emberUnsignedCommandArgument(0);
  uint16_t characteristic = emberUnsignedCommandArgument(1);
  uint8_t value_len = emberUnsignedCommandArgument(2);
  uint8_t value_data[MAX_CHAR_SIZE];
  emberCopyStringArgument(3, value_data, MAX_CHAR_SIZE, FALSE);

  sl_status_t status = sl_bt_gatt_write_characteristic_value(connection_handle,
                                                             characteristic,
                                                             value_len,
                                                             value_data);

  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("success");
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}

void emberAfPluginBleConnectionCloseCommand(void)
{
  uint8_t connection_handle = emberUnsignedCommandArgument(0);

  sl_status_t status = sl_bt_connection_close(connection_handle);

  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("success");
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}

void emberAfPluginBleSetTxPowerCommand(void)
{
  int16_t tx_power_set_value = (int16_t)emberSignedCommandArgument(0);
  int16_t tx_power_get_value = 0;

  sl_status_t status = sl_bt_system_set_max_tx_power(tx_power_set_value, &tx_power_get_value);

  if (status == SL_STATUS_OK) {
    emberAfCorePrintln("tx power is set to: %d.%ddBm", (tx_power_get_value / 10), (tx_power_get_value % 10));
  } else {
    emberAfCorePrintln("error: 0x%2x", status);
  }
}
