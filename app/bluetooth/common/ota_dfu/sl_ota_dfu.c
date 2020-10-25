/***************************************************************************//**
 * @file
 * @brief Over-the-Air Device Firmware Update
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "sl_app_assert.h"
#include "sl_ota_dfu.h"

// Flag for indicating DFU reset must be performed.
static bool boot_to_dfu = false;

/**************************************************************************//**
 * Bluetooth stack event handler.
 *****************************************************************************/
void sl_bt_ota_dfu_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;

  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates that a remote GATT client is attempting to write
    // a value of a user type attribute in to the local GATT database.
    case sl_bt_evt_gatt_server_user_write_request_id:
      // If user-type OTA Control Characteristic was written, boot the device
      // into Device Firmware Upgrade (DFU) mode.
      if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control) {
        // Set flag to enter OTA mode.
        boot_to_dfu = true;
        // Send response to user write request.
        sc = sl_bt_gatt_server_send_user_write_response(
          evt->data.evt_gatt_server_user_write_request.connection,
          gattdb_ota_control,
          SL_STATUS_OK);
        sl_app_assert(sc == SL_STATUS_OK,
                      "[E: 0x%04x] Failed to send response to user write request\n",
                      (int)sc);
        // Close connection to enter to DFU OTA mode
        sc = sl_bt_connection_close(
          evt->data.evt_gatt_server_user_write_request.connection);
        sl_app_assert(sc == SL_STATUS_OK,
                      "[E: 0x%04x] Failed to close connection to enter to DFU OTA mode\n",
                      (int)sc);
      }
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      // Check if need to boot to OTA DFU mode.
      if (boot_to_dfu) {
        // Reset MCU and enter OTA DFU mode.
        sl_bt_system_reset(2);
      }
      break;

    default:
      break;
  }
}
