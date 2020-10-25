/***************************************************************************//**
 * @file
 * @brief Ambient Light GATT Service
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

#include "em_common.h"
#include "sl_status.h"
#include "gatt_db.h"
#include "sl_app_assert.h"
#include "sl_gatt_service_lux.h"

// -----------------------------------------------------------------------------
// Private variables

// default ambient light: 0 lux
static uint32_t lux_char_value = 0;

// -----------------------------------------------------------------------------
// Private function declarations

static void lux_ambient_light_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

static void lux_ambient_light_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;
  float lux;

  // update measurement data, keep previous data if measurement fails
  if (SL_STATUS_OK == sl_gatt_service_lux_get(&lux)) {
    lux_char_value = (uint32_t)(lux * 100);
  }

  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(lux_char_value),
    (uint8_t*)&lux_char_value,
    NULL);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to send user read response\n",
                (int)sc);
}

// -----------------------------------------------------------------------------
// Public function definitions

void sl_gatt_service_lux_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  if (sl_bt_evt_gatt_server_user_read_request_id == SL_BT_MSG_ID(evt->header)) {
    if (gattdb_es_ambient_light == evt->data.evt_gatt_server_user_read_request.characteristic) {
      lux_ambient_light_read_cb(&evt->data.evt_gatt_server_user_read_request);
    }
  }
}

SL_WEAK sl_status_t sl_gatt_service_lux_get(float* lux)
{
  (void)lux;
  // keep default values
  return SL_STATUS_FAIL;
}
