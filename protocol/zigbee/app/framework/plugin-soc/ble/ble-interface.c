/***************************************************************************//**
 * @file
 * @brief Routines to interfact with the BLE stack.
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
#ifdef EMBER_AF_NCP
#include "app/framework/include/af-ncp.h"
#else // !EMBER_AF_NCP
#include "app/framework/include/af.h"
#endif // EMBER_AF_NCP
#include "sl_bt_rtos_adaptation.h"
#include "sl_bt_ll_config.h"
#include "sl_bt_types.h"
#include "sl_bluetooth_config.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "hal-config.h"

#include <kernel/include/os.h>

#if defined(EMBER_AF_PLUGIN_BLE_PSSTORE_LIBRARY) || defined (EMBER_AF_PLUGIN_SIM_EEPROM2_TO_NVM3_UPGRADE_RESERVE_PSSTORE)
// This is needed in order to properly place the PSStore space at the top of
// flash (PSStore is not relocatable, so it needs to be at the top of flash).
VAR_AT_SEGMENT(NO_STRIPPING uint8_t psStore[FLASH_PAGE_SIZE * 2], __PSSTORE__);
#endif

void BluetoothLLCallback();
void BluetoothUpdate();

void emberAfPluginBleGetConfig(sl_bt_configuration_t* config)
{
  emberAfPluginBleGetConfigCallback(config);

  // Re-assigning the heap-related fields, just in case.
  config->bluetooth.max_connections = SL_BT_CONFIG_MAX_CONNECTIONS;
  config->bluetooth.max_advertisers = SL_BT_CONFIG_MAX_ADVERTISERS;
  config->bluetooth.max_periodic_sync = SL_BT_CONFIG_MAX_PERIODIC_ADVERTISING_SYNC;
}

sl_bt_bluetooth_ll_priorities *emberAfPluginBleGetLlPrioritiesConfig(void)
{
  return &ll_priorities;
}

bool emberAfPluginBleHasEventPending(void)
{
  RTOS_ERR os_err;

  OSFlagPend(&bluetooth_event_flags, (OS_FLAGS)SL_BT_RTOS_EVENT_FLAG_EVT_WAITING,
             0,
             OS_OPT_PEND_NON_BLOCKING
             + OS_OPT_PEND_FLAG_SET_ANY,
             NULL,
             &os_err);

  return (RTOS_ERR_CODE_GET(os_err) == RTOS_ERR_NONE);
}

void emberAfPluginBleInitCallback(void)
{
  // TODO
}

// This is run as part of the Zigbee task main loop.
void emberAfPluginBleTickCallback(void)
{
  RTOS_ERR os_err;

  OSFlagPend(&bluetooth_event_flags, (OS_FLAGS)SL_BT_RTOS_EVENT_FLAG_EVT_WAITING,
             0,
             OS_OPT_PEND_NON_BLOCKING
             + OS_OPT_PEND_FLAG_SET_ANY
             + OS_OPT_PEND_FLAG_CONSUME,
             NULL,
             &os_err);

  if (RTOS_ERR_CODE_GET(os_err) == RTOS_ERR_NONE) {
    emberAfPluginBleEventCallback((sl_bt_msg_t*)bluetooth_evt);

    OSFlagPost(&bluetooth_event_flags,
               (OS_FLAGS)SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED,
               OS_OPT_POST_FLAG_SET,
               &os_err);
  }
}
