/***************************************************************************//**
 * @file
 * @brief Ambient Light and UV Index GATT Service
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
#include "sl_gatt_service_light.h"

// -----------------------------------------------------------------------------
// Private variables

// default ambient light: 0 lux
static uint32_t light_lux = 0;
// default UV index: 0
static uint8_t light_uvi = 0;

// -----------------------------------------------------------------------------
// Private function declarations

static void light_update(void);
static void light_ambient_light_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);
static void light_uvindex_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

static void light_update(void)
{
  float lux;
  float uvi;
  // keep previous data if measurement fails
  if (SL_STATUS_OK == sl_gatt_service_light_get(&lux, &uvi)) {
    light_lux = (uint32_t)(lux * 100);
    light_uvi = (uint8_t)uvi;
  }
}

static void light_ambient_light_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;

  // update measurement data
  light_update();

  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(light_lux),
    (uint8_t*)&light_lux,
    NULL);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to send user read response\n",
                (int)sc);
}

static void light_uvindex_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;

  // update measurement data
  light_update();

  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(light_uvi),
    (uint8_t*)&light_uvi,
    NULL);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to send user read response\n",
                (int)sc);
}

// -----------------------------------------------------------------------------
// Public function definitions

void sl_gatt_service_light_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  if (sl_bt_evt_gatt_server_user_read_request_id == SL_BT_MSG_ID(evt->header)) {
    if (gattdb_es_ambient_light == evt->data.evt_gatt_server_user_read_request.characteristic) {
      light_ambient_light_read_cb(&evt->data.evt_gatt_server_user_read_request);
    } else if (gattdb_es_uvindex == evt->data.evt_gatt_server_user_read_request.characteristic) {
      light_uvindex_read_cb(&evt->data.evt_gatt_server_user_read_request);
    }
  }
}

SL_WEAK sl_status_t sl_gatt_service_light_get(float* lux, float* uvi)
{
  (void)lux;
  (void)uvi;
  // keep default values
  return SL_STATUS_FAIL;
}
