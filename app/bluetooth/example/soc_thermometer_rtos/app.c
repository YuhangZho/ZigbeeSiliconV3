/***************************************************************************//**
 * @file
 * @brief Core application logic.
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
#include <stdbool.h>
#include "em_common.h"
#include "sl_status.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#include "os.h"
#include "sl_app_log.h"
#include "sl_app_assert.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT
#ifdef SL_CATALOG_CLI_PRESENT
#include "sl_cli.h"
#endif // SL_CATALOG_CLI_PRESENT
#include "sl_sensor_rht.h"
#include "sl_health_thermometer.h"
#include "app.h"

// Connection handle.
static uint8_t app_connection = 0;

// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;

// Button state.
static volatile bool app_btn0_pressed = false;

// Periodic timer handle.
static OS_TMR app_periodic_timer;

// Periodic timer callback.
static void app_periodic_timer_cb(void *handle, void *p_arg);

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  RTOS_ERR err;
  OS_RATE_HZ tick_rate;
  OS_TICK period;

  sl_app_log("health thermometer RTOS initialised\n");

  // Calculate timer period.
  tick_rate = OSTimeTickRateHzGet(&err);
  period = SL_BT_HT_MEASUREMENT_INTERVAL_SEC * tick_rate / OSTmrUpdateCnt;

  OSTmrCreate(&app_periodic_timer,
              "app_periodic_timer",
              period,
              period,
              OS_OPT_TMR_PERIODIC,
              &app_periodic_timer_cb,
              NULL,
              &err);
  sl_app_assert(err.Code == RTOS_ERR_NONE,
                "[E: 0x%04x] Failed to create periodic timer\n",
                (int)err.Code);

  // Init temperature sensor.
  sl_sensor_rht_init();
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  bd_addr address;
  uint8_t address_type;
  uint8_t system_id[8];

  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      // Print boot message.
      sl_app_log("Bluetooth stack booted: v%d.%d.%d-b%d\n",
                 evt->data.evt_system_boot.major,
                 evt->data.evt_system_boot.minor,
                 evt->data.evt_system_boot.patch,
                 evt->data.evt_system_boot.build);

      // Extract unique ID from BT Address.
      sc = sl_bt_system_get_identity_address(&address, &address_type);
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to get Bluetooth address\n",
                    (int)sc);

      // Pad and reverse unique ID to get System ID.
      system_id[0] = address.addr[5];
      system_id[1] = address.addr[4];
      system_id[2] = address.addr[3];
      system_id[3] = 0xFF;
      system_id[4] = 0xFE;
      system_id[5] = address.addr[2];
      system_id[6] = address.addr[1];
      system_id[7] = address.addr[0];

      sc = sl_bt_gatt_server_write_attribute_value(gattdb_system_id,
                                                   0,
                                                   sizeof(system_id),
                                                   system_id);
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to write attribute\n",
                    (int)sc);

      sl_app_log("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X\n",
                 address_type ? "static random" : "public device",
                 address.addr[5],
                 address.addr[4],
                 address.addr[3],
                 address.addr[2],
                 address.addr[1],
                 address.addr[0]);

      // Create an advertising set.
      sc = sl_bt_advertiser_create_set(&advertising_set_handle);
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to create advertising set\n",
                    (int)sc);

      // Set advertising interval to 100ms.
      sc = sl_bt_advertiser_set_timing(
        advertising_set_handle, // advertising set handle
        160, // min. adv. interval (milliseconds * 1.6)
        160, // max. adv. interval (milliseconds * 1.6)
        0,   // adv. duration
        0);  // max. num. adv. events
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to set advertising timing\n",
                    (int)sc);
      // Start general advertising and enable connections.
      sc = sl_bt_advertiser_start(
        advertising_set_handle,
        advertiser_general_discoverable,
        advertiser_connectable_scannable);
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to start advertising\n",
                    (int)sc);
      sl_app_log("Started advertising\n");
      break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      sl_app_log("Connection opened\n");
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      sl_app_log("Connection closed\n");
      // Restart advertising after client has disconnected.
      sc = sl_bt_advertiser_start(
        advertising_set_handle,
        advertiser_general_discoverable,
        advertiser_connectable_scannable);
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to start advertising\n",
                    (int)sc);
      sl_app_log("Started advertising\n");
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////

    // -------------------------------
    // Default event handler.
    default:
      break;
  }
}

/**************************************************************************//**
 * Callback function of connection close event.
 *
 * @param[in] reason Unused parameter required by the health_thermometer component
 * @param[in] connection Unused parameter required by the health_thermometer component
 *****************************************************************************/
void sl_bt_connection_closed_cb(uint16_t reason, uint8_t connection)
{
  (void)reason;
  (void)connection;
  RTOS_ERR err;
  OS_STATE state;

  // Check if periodic timer is running.
  state = OSTmrStateGet(&app_periodic_timer, &err);
  sl_app_assert(err.Code == RTOS_ERR_NONE,
                "[E: 0x%04x] Failed to check if periodic timer is running\n",
                (int)err.Code);

  // Stop timer if it is still running.
  if (OS_TMR_STATE_RUNNING == state) {
    OSTmrStop(&app_periodic_timer, OS_OPT_TMR_NONE, NULL, &err);
    sl_app_assert(err.Code == RTOS_ERR_NONE,
                  "[E: 0x%04x] Failed to stop periodic timer\n",
                  (int)err.Code);
  }
}

/**************************************************************************//**
 * Health Thermometer - Temperature Measurement
 * Indication changed callback
 *
 * Called when indication of temperature measurement is enabled/disabled by
 * the client.
 *****************************************************************************/
void sl_bt_ht_temperature_measurement_indication_changed_cb(uint8_t connection,
                                                            gatt_client_config_flag_t client_config)
{
  RTOS_ERR err;
  app_connection = connection;
  // Indication or notification enabled.
  if (gatt_disable != client_config) {
    // Start timer used for periodic indications.
    OSTmrStart(&app_periodic_timer, &err);
    sl_app_assert(err.Code == RTOS_ERR_NONE,
                  "[E: 0x%04x] Failed to start periodic timer\n",
                  (int)err.Code);
    // Send first indication.
    app_periodic_timer_cb(&app_periodic_timer, NULL);
  }
  // Indications disabled.
  else {
    // Stop timer used for periodic indications.
    OSTmrStop(&app_periodic_timer, OS_OPT_TMR_NONE, NULL, &err);
    sl_app_assert(err.Code == RTOS_ERR_NONE,
                  "[E: 0x%04x] Failed to stop periodic timer\n",
                  (int)err.Code);
  }
}

/**************************************************************************//**
 * Simple Button
 * Button state changed callback
 * @param[in] handle Button event handle
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  // Button pressed.
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (&sl_button_btn0 == handle) {
      sl_led_turn_on(&sl_led_led0);
      app_btn0_pressed = true;
    }
  }
  // Button released.
  else if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_RELEASED) {
    if (&sl_button_btn0 == handle) {
      sl_led_turn_off(&sl_led_led0);
      app_btn0_pressed = false;
    }
  }
}

/**************************************************************************//**
 * Timer callback
 * Called periodically to time periodic temperature measurements and indications.
 *****************************************************************************/
static void app_periodic_timer_cb(void *handle, void *p_arg)
{
  (void)p_arg;
  (void)handle;
  sl_status_t sc;
  int32_t temperature = 0;
  uint32_t humidity = 0;
  float tmp_c = 0.0;
  // float tmp_f = 0.0;

  // Measure temperature; units are % and milli-Celsius.
  sc = sl_sensor_rht_get(&humidity, &temperature);
  if (sc != SL_STATUS_OK) {
    sl_app_log("Warning! Invalid RHT reading: %lu %ld\n", humidity, temperature);
  }

  // button 0 pressed: overwrite temperature with -20C.
  if (app_btn0_pressed) {
    temperature = -20 * 1000;
  }

  tmp_c = (float)temperature / 1000;
  sl_app_log("Temperature: %5.2f C\n", tmp_c);
  // Send temperature measurement indication to connected client.
  sc = sl_bt_ht_temperature_measurement_indicate(app_connection,
                                                 temperature,
                                                 false);
  // Conversion to Fahrenheit: F = C * 1.8 + 32
  // tmp_f = (float)(temperature*18+320000)/10000;
  // sl_app_log("Temperature: %5.2f F\n", tmp_f);
  // Send temperature measurement indication to connected client.
  // sc = sl_bt_ht_temperature_measurement_indicate(app_connection,
  //                                                (temperature*18+320000)/10,
  //                                                true);
  if (sc) {
    sl_app_log("Warning! Failed to send temperature measurement indication\n");
  }
}

#ifdef SL_CATALOG_CLI_PRESENT
void hello(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  bd_addr address;
  uint8_t address_type;
  sl_status_t sc = sl_bt_system_get_identity_address(&address, &address_type);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to get Bluetooth address\n",
                (int)sc);
  sl_app_log("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X\n",
             address_type ? "static random" : "public device",
             address.addr[5],
             address.addr[4],
             address.addr[3],
             address.addr[2],
             address.addr[1],
             address.addr[0]);
}
#endif // SL_CATALOG_CLI_PRESENT
