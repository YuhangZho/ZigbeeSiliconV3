/***************************************************************************//**
 * @file
 * @brief Automation IO GATT service
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

#include "stdbool.h"
#include "sl_status.h"
#include "gatt_db.h"
#include "sl_app_assert.h"
#include "sl_app_log.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#include "sl_gatt_service_aio.h"

// -----------------------------------------------------------------------------
// Private macros

// AIO digital states defined by the BLE standard
#define AIO_DIGITAL_STATE_INACTIVE  0
#define AIO_DIGITAL_STATE_ACTIVE    1
#define AIO_DIGITAL_STATE_TRISTATE  2
#define AIO_DIGITAL_STATE_UNKNOWN   3

#define AIO_DIGITAL_STATE_MASK      3
#define AIO_DIGITAL_STATE_SIZE      2
// AIO digital state is represented on 1 byte, thus 4 digitals are supported
#define AIO_DIGITAL_COUNT_MAX       4

#if (SL_SIMPLE_BUTTON_COUNT > AIO_DIGITAL_COUNT_MAX) \
  || (SL_SIMPLE_LED_COUNT > AIO_DIGITAL_COUNT_MAX)
#error Maximal AIO count exceeded.
#endif

// -----------------------------------------------------------------------------
// Private variables

static bool aio_notification_enabled = false;
static bool aio_button_changed = false;
static uint8_t aio_connection = 0;

static const sl_button_t *aoi_digital_in_list[SL_SIMPLE_BUTTON_COUNT] =
{
#if SL_SIMPLE_BUTTON_COUNT > 0
  &sl_button_btn0,
#if SL_SIMPLE_BUTTON_COUNT > 1
  &sl_button_btn1,
#if SL_SIMPLE_BUTTON_COUNT > 2
  &sl_button_btn2,
#if SL_SIMPLE_BUTTON_COUNT > 3
  &sl_button_btn3,
#endif
#endif
#endif
#endif
};

static const sl_led_t *aoi_digital_out_list[SL_SIMPLE_LED_COUNT] =
{
#if SL_SIMPLE_LED_COUNT > 0
  &sl_led_led0,
#if SL_SIMPLE_LED_COUNT > 1
  &sl_led_led1,
#if SL_SIMPLE_LED_COUNT > 2
  &sl_led_led2,
#if SL_SIMPLE_LED_COUNT > 3
  &sl_led_led3,
#endif
#endif
#endif
#endif
};

// -----------------------------------------------------------------------------
// Private function declarations

static uint8_t aio_digital_in_get_state(void);
static uint8_t aio_digital_out_get_state(void);
static void aio_digital_out_set_state(uint8_t state);

static void aio_digital_in_notify(void);
static void aio_system_boot_cb(void);
static void aio_connection_opened_cb(sl_bt_evt_connection_opened_t *data);
static void aio_connection_closed_cb(sl_bt_evt_connection_closed_t *data);
static void aio_digital_in_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);
static void aio_digital_out_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);
static void aio_digital_in_changed_cb(sl_bt_evt_gatt_server_characteristic_status_t *data);
static void aio_digital_out_write_cb(sl_bt_evt_gatt_server_user_write_request_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

static uint8_t aio_digital_in_get_state(void)
{
  uint8_t aio_state = 0;
  sl_button_state_t btn_state;
  uint8_t i;

  // read button states
  for (i = 0; i < SL_SIMPLE_BUTTON_COUNT; i++) {
    btn_state = sl_button_get_state(aoi_digital_in_list[i]);
    if (btn_state == SL_SIMPLE_BUTTON_PRESSED) {
      aio_state |= AIO_DIGITAL_STATE_ACTIVE << (i * AIO_DIGITAL_STATE_SIZE);
    }
  }
  sl_app_log("AIO in: %d\n", aio_state);
  return aio_state;
}

static uint8_t aio_digital_out_get_state(void)
{
  uint8_t aio_state = 0;
  sl_led_state_t led_state;
  uint8_t i;

  // read led states
  for (i = 0; i < SL_SIMPLE_LED_COUNT; i++) {
    led_state = sl_led_get_state(aoi_digital_out_list[i]);
    if (led_state == SL_LED_CURRENT_STATE_ON) {
      aio_state |= AIO_DIGITAL_STATE_ACTIVE << (i * AIO_DIGITAL_STATE_SIZE);
    }
  }
  return aio_state;
}

static void aio_digital_out_set_state(uint8_t state)
{
  uint8_t led_state, i;
  for (i = 0; i < SL_SIMPLE_LED_COUNT; i++) {
    led_state = (state >> (i * AIO_DIGITAL_STATE_SIZE)) & AIO_DIGITAL_STATE_MASK;
    if (led_state == AIO_DIGITAL_STATE_ACTIVE) {
      sl_led_turn_on(aoi_digital_out_list[i]);
    } else {
      sl_led_turn_off(aoi_digital_out_list[i]);
    }
  }
  sl_app_log("AIO out: %d\n", state);
}

static void aio_digital_in_notify(void)
{
  sl_status_t sc;
  uint8_t value = aio_digital_in_get_state();
  sc = sl_bt_gatt_server_send_characteristic_notification(
    aio_connection,
    gattdb_aio_digital_in,
    1,
    &value,
    NULL);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to send characteristic notification\n",
                (int)sc);
}

static void aio_system_boot_cb(void)
{
  sl_status_t sc;
  uint8_t value_in = SL_SIMPLE_BUTTON_COUNT;
  uint8_t value_out = SL_SIMPLE_LED_COUNT;
  sc = sl_bt_gatt_server_write_attribute_value(gattdb_aio_num_of_digitals_in, 0, 1, &value_in);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to write attribute value\n",
                (int)sc);
  sc = sl_bt_gatt_server_write_attribute_value(gattdb_aio_num_of_digitals_out, 0, 1, &value_out);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to write attribute value\n",
                (int)sc);
}

static void aio_connection_opened_cb(sl_bt_evt_connection_opened_t *data)
{
  (void)data;
  // Reset LED state
  aio_digital_out_set_state(0);
}

static void aio_connection_closed_cb(sl_bt_evt_connection_closed_t *data)
{
  (void)data;
  // Reset LED state
  aio_digital_out_set_state(0);
  // Disable notifications
  aio_notification_enabled = false;
}

static void aio_digital_in_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;
  uint8_t value = aio_digital_in_get_state();
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

static void aio_digital_out_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;
  uint8_t value = aio_digital_out_get_state();
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

static void aio_digital_in_changed_cb(sl_bt_evt_gatt_server_characteristic_status_t *data)
{
  aio_connection = data->connection;
  // indication or notification enabled
  if (gatt_disable != data->client_config_flags) {
    // enable notifications
    aio_notification_enabled = true;
    // send the first notification
    aio_digital_in_notify();
  }
  // indication and notification disabled
  else {
    aio_notification_enabled = false;
  }
}

static void aio_digital_out_write_cb(sl_bt_evt_gatt_server_user_write_request_t *data)
{
  sl_status_t sc;
  uint8_t att_errorcode = 0;

  if (data->value.len == 1) {
    aio_digital_out_set_state(data->value.data[0]);
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

void sl_gatt_service_aio_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_system_boot_id:
      aio_system_boot_cb();
      break;

    case sl_bt_evt_connection_opened_id:
      aio_connection_opened_cb(&evt->data.evt_connection_opened);
      break;

    case sl_bt_evt_connection_closed_id:
      aio_connection_closed_cb(&evt->data.evt_connection_closed);
      break;

    case sl_bt_evt_gatt_server_user_read_request_id:
      switch (evt->data.evt_gatt_server_user_read_request.characteristic) {
        case gattdb_aio_digital_in:
          aio_digital_in_read_cb(&evt->data.evt_gatt_server_user_read_request);
          break;
        case gattdb_aio_digital_out:
          aio_digital_out_read_cb(&evt->data.evt_gatt_server_user_read_request);
          break;
      }
      break;

    case sl_bt_evt_gatt_server_characteristic_status_id:
      if ((gattdb_aio_digital_in == evt->data.evt_gatt_server_characteristic_status.characteristic)
          && (gatt_server_client_config == (gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags)) {
        // client characteristic configuration changed by remote GATT client
        aio_digital_in_changed_cb(&evt->data.evt_gatt_server_characteristic_status);
      }
      break;

    case sl_bt_evt_gatt_server_user_write_request_id:
      if (gattdb_aio_digital_out == evt->data.evt_gatt_server_user_write_request.characteristic) {
        aio_digital_out_write_cb(&evt->data.evt_gatt_server_user_write_request);
      }
      break;
  }
}

void sl_gatt_service_aio_step(void)
{
  if (aio_button_changed) {
    aio_button_changed = false;
    aio_digital_in_notify();
  }
}

void sl_gatt_service_aio_on_change(void)
{
  if (aio_notification_enabled) {
    aio_button_changed = true;
  }
}
