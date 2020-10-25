/***************************************************************************//**
 * @file
 * @brief Battery measurement GATT service
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
#include "sl_simple_timer.h"
#include "gatt_db.h"
#include "sl_app_assert.h"
#include "sl_gatt_service_battery.h"

// -----------------------------------------------------------------------------
// Configuration

#define BATT_MEASUREMENT_INTERVAL_MS 10000

// -----------------------------------------------------------------------------
// Private variables

static sl_simple_timer_t batt_timer;
static uint8_t batt_connection = 0;

// -----------------------------------------------------------------------------
// Private function declarations

static void batt_measurement_notify(void);
static void batt_timer_cb(sl_simple_timer_t *timer, void *data);
static void batt_connection_closed_cb(sl_bt_evt_connection_closed_t *data);
static void batt_measurement_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);
static void batt_type_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);
static void batt_measurement_changed_cb(sl_bt_evt_gatt_server_characteristic_status_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

static void batt_measurement_notify(void)
{
  sl_status_t sc;
  uint8_t value = sl_gatt_service_battery_get_level();
  sc = sl_bt_gatt_server_send_characteristic_notification(
    batt_connection,
    gattdb_batt_measurement,
    1,
    &value,
    NULL);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to send characteristic notification\n",
                (int)sc);
}

static void batt_timer_cb(sl_simple_timer_t *timer, void *data)
{
  (void)data;
  (void)timer;
  // send temperature measurement indication to connected client
  batt_measurement_notify();
}

static void batt_connection_closed_cb(sl_bt_evt_connection_closed_t *data)
{
  (void)data;
  sl_status_t sc;
  sc = sl_simple_timer_stop(&batt_timer);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to stop periodic timer\n",
                (int)sc);
}

static void batt_measurement_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;
  uint8_t value = sl_gatt_service_battery_get_level();
  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    1,
    &value,
    NULL);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to send user read response\n",
                (int)sc);
}

static void batt_type_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;
  uint8_t value = sl_gatt_service_battery_get_type();
  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    1,
    &value,
    NULL);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to send user read response\n",
                (int)sc);
}

static void batt_measurement_changed_cb(sl_bt_evt_gatt_server_characteristic_status_t *data)
{
  sl_status_t sc;
  batt_connection = data->connection;
  // indication or notification enabled
  if (gatt_disable != data->client_config_flags) {
    // start timer used for periodic notifications
    sc = sl_simple_timer_start(&batt_timer,
                               BATT_MEASUREMENT_INTERVAL_MS,
                               batt_timer_cb,
                               NULL,
                               true);
    sl_app_assert(sc == SL_STATUS_OK,
                  "[E: 0x%04x] Failed to start periodic timer\n",
                  (int)sc);
    // Send the first notification
    batt_measurement_notify();
  }
  // indication and notifications disabled
  else {
    // stop timer used for periodic notifications
    sc = sl_simple_timer_stop(&batt_timer);
    sl_app_assert(sc == SL_STATUS_OK,
                  "[E: 0x%04x] Failed to stop periodic timer\n",
                  (int)sc);
  }
}

// -----------------------------------------------------------------------------
// Public function definitions

void sl_gatt_service_battery_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_connection_closed_id:
      batt_connection_closed_cb(&evt->data.evt_connection_closed);
      break;

    case sl_bt_evt_gatt_server_user_read_request_id:
      switch (evt->data.evt_gatt_server_user_read_request.characteristic) {
        case gattdb_batt_measurement:
          batt_measurement_read_cb(&evt->data.evt_gatt_server_user_read_request);
          break;
        case gattdb_power_source_type:
          batt_type_read_cb(&evt->data.evt_gatt_server_user_read_request);
          break;
      }
      break;

    case sl_bt_evt_gatt_server_characteristic_status_id:
      if ((gattdb_batt_measurement == evt->data.evt_gatt_server_characteristic_status.characteristic)
          && (gatt_server_client_config == (gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags)) {
        // client characteristic configuration changed by remote GATT client
        batt_measurement_changed_cb(&evt->data.evt_gatt_server_characteristic_status);
      }
      break;

    default:
      break;
  }
}

SL_WEAK uint8_t sl_gatt_service_battery_get_level(void)
{
  return 0;
}

SL_WEAK uint8_t sl_gatt_service_battery_get_type(void)
{
  return 0;
}
