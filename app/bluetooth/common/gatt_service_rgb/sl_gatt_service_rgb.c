/***************************************************************************//**
 * @file
 * @brief RGB LED GATT Service
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

#include <string.h>
#include "em_core.h"
#include "sl_status.h"
#include "gatt_db.h"
#include "sl_app_assert.h"
#include "sl_app_log.h"
#include "sl_gatt_service_rgb.h"

// -----------------------------------------------------------------------------
// Private variables

static struct {
  uint8_t m; // mask
  uint8_t r; // red
  uint8_t g; // green
  uint8_t b; // blue
} rgb_led_state = { .m = 0, .r = 0, .g = 0x50, .b = 0x70 };

// -----------------------------------------------------------------------------
// Private function declarations

static void rgb_connection_opened_cb(sl_bt_evt_connection_opened_t *data);
static void rgb_connection_closed_cb(sl_bt_evt_connection_closed_t *data);
static void rgb_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);
static void rgb_write_cb(sl_bt_evt_gatt_server_user_write_request_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

static void rgb_connection_opened_cb(sl_bt_evt_connection_opened_t *data)
{
  (void)data;
}

static void rgb_connection_closed_cb(sl_bt_evt_connection_closed_t *data)
{
  (void)data;
  rgb_led_state.m = 0;
}

static void rgb_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;
  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(rgb_led_state),
    (uint8_t*)&rgb_led_state,
    NULL);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to send user read response\n",
                (int)sc);
}

static void rgb_write_cb(sl_bt_evt_gatt_server_user_write_request_t *data)
{
  sl_status_t sc;
  uint8_t att_errorcode = 0;

  if (data->value.len == sizeof(rgb_led_state)) {
    (void)memcpy(&rgb_led_state, data->value.data, sizeof(rgb_led_state));
    sl_gatt_service_rgb_set_led(rgb_led_state.m, rgb_led_state.r, rgb_led_state.g, rgb_led_state.b);
  } else {
    att_errorcode = 0x0D; // Invalid Attribute Value Length
  }

  sc = sl_bt_gatt_server_send_user_write_response(data->connection,
                                                  data->characteristic,
                                                  att_errorcode);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to send user write response\n",
                (int)sc);
}

// -----------------------------------------------------------------------------
// Public function definitions

void sl_gatt_service_rgb_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_connection_opened_id:
      rgb_connection_opened_cb(&evt->data.evt_connection_opened);
      break;

    case sl_bt_evt_connection_closed_id:
      rgb_connection_closed_cb(&evt->data.evt_connection_closed);
      break;

    case sl_bt_evt_gatt_server_user_read_request_id:
      if (gattdb_ui_rgbleds == evt->data.evt_gatt_server_user_read_request.characteristic) {
        rgb_read_cb(&evt->data.evt_gatt_server_user_read_request);
      }
      break;

    case sl_bt_evt_gatt_server_user_write_request_id:
      if (gattdb_ui_rgbleds == evt->data.evt_gatt_server_user_write_request.characteristic) {
        rgb_write_cb(&evt->data.evt_gatt_server_user_write_request);
      }
      break;
  }
}

SL_WEAK void sl_gatt_service_rgb_set_led(uint8_t m, uint8_t r, uint8_t g, uint8_t b)
{
  sl_app_log("RGB set: %d %d %d %d\n", m, r, g, b);
}
