/***************************************************************************//**
 * @file
 * @brief Relative Humidity and Temperature GATT Service
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
#include "sl_gatt_service_rht.h"

// -----------------------------------------------------------------------------
// Private variables

// default relative humidity: 50 %
static uint16_t rht_humidity = 5000;
// default temperature: 25 C
static int16_t rht_temperature = 2500;

// -----------------------------------------------------------------------------
// Private function declarations

static void rht_update(void);
static void rht_humidity_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);
static void rht_temperature_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

static void rht_update(void)
{
  uint32_t humidity;
  int32_t temperature;
  // keep previous data if measurement fails
  if (SL_STATUS_OK == sl_gatt_service_rht_get(&humidity, &temperature)) {
    rht_humidity = humidity / 10;       // 0.01 %
    rht_temperature = temperature / 10; // 0.01 C
  }
}

static void rht_humidity_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;

  // update measurement data
  rht_update();

  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(rht_humidity),
    (uint8_t*)&rht_humidity,
    NULL);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to send user read response\n",
                (int)sc);
}

static void rht_temperature_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;

  // update measurement data
  rht_update();

  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(rht_temperature),
    (uint8_t*)&rht_temperature,
    NULL);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to send user read response\n",
                (int)sc);
}

// -----------------------------------------------------------------------------
// Public function definitions

void sl_gatt_service_rht_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  if (sl_bt_evt_gatt_server_user_read_request_id == SL_BT_MSG_ID(evt->header)) {
    if (gattdb_es_temperature == evt->data.evt_gatt_server_user_read_request.characteristic) {
      rht_temperature_read_cb(&evt->data.evt_gatt_server_user_read_request);
    } else if (gattdb_es_humidity == evt->data.evt_gatt_server_user_read_request.characteristic) {
      rht_humidity_read_cb(&evt->data.evt_gatt_server_user_read_request);
    }
  }
}

SL_WEAK sl_status_t sl_gatt_service_rht_get(uint32_t *rh, int32_t *t)
{
  (void)rh;
  (void)t;
  // keep default values
  return SL_STATUS_FAIL;
}
