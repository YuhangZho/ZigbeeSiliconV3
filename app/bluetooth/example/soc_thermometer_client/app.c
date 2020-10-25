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
#include <math.h>
#include "em_common.h"
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
#include "app.h"

// connection parameters
#define CONN_INTERVAL_MIN             80   //100ms
#define CONN_INTERVAL_MAX             80   //100ms
#define CONN_SLAVE_LATENCY            0    //no latency
#define CONN_TIMEOUT                  100  //1000ms
#define CONN_MIN_CE_LENGTH            0
#define CONN_MAX_CE_LENGTH            0xffff

#define SCAN_INTERVAL                 16   //10ms
#define SCAN_WINDOW                   16   //10ms
#define SCAN_PASSIVE                  0

#define TEMP_INVALID                  NAN
#define UNIT_INVALID                  ('?')
#define UNIT_CELSIUS                  ('C')
#define UNIT_FAHRENHEIT               ('F')
#define RSSI_INVALID                  ((int8_t)0x7F)
#define CONNECTION_HANDLE_INVALID     ((uint8_t)0xFFu)
#define SERVICE_HANDLE_INVALID        ((uint32_t)0xFFFFFFFFu)
#define CHARACTERISTIC_HANDLE_INVALID ((uint16_t)0xFFFFu)
#define TABLE_INDEX_INVALID           ((uint8_t)0xFFu)

#if SL_BT_CONFIG_MAX_CONNECTIONS < 1
  #error At least 1 connection has to be enabled!
#endif

// Macro to translate the Flags to Celsius (C) or Fahrenheit (F). Flags is the first byte of the
// Temperature Measurement characteristic value according to the Bluetooth SIG
#define translate_flags_to_temperature_unit(flags) (((flags) & 1) ? UNIT_FAHRENHEIT : UNIT_CELSIUS)

typedef enum {
  scanning,
  opening,
  discover_services,
  discover_characteristics,
  enable_indication,
  running
} conn_state_t;

typedef struct {
  uint8_t  connection_handle;
  int8_t   rssi;
  uint16_t server_address;
  uint32_t thermometer_service_handle;
  uint16_t thermometer_characteristic_handle;
  float    temperature;
  char     unit;
} conn_properties_t;

typedef struct {
  uint8_t mantissa_l;
  uint8_t mantissa_m;
  int8_t mantissa_h;
  int8_t exponent;
} IEEE_11073_float;

// Array for holding properties of multiple (parallel) connections
static conn_properties_t conn_properties[SL_BT_CONFIG_MAX_CONNECTIONS];
// Counter of active connections
static uint8_t active_connections_num;
// State of the connection under establishment
static conn_state_t conn_state;
// Health Thermometer service UUID defined by Bluetooth SIG
static const uint8_t thermo_service[2] = { 0x09, 0x18 };
// Temperature Measurement characteristic UUID defined by Bluetooth SIG
static const uint8_t thermo_char[2] = { 0x1c, 0x2a };

static void init_properties(void);
static uint8_t find_service_in_advertisement(uint8_t *data, uint8_t len);
static uint8_t find_index_by_connection_handle(uint8_t connection);
static void add_connection(uint8_t connection, uint16_t address);
// Remove a connection from the connection_properties array
static void remove_connection(uint8_t connection);
static float translate_IEEE_11073_temperature_to_float(IEEE_11073_float const *IEEE_11073_value);
static bd_addr *read_and_cache_bluetooth_address(uint8_t *address_type_out);
static void print_bluetooth_address(void);
static void set_attribute_system_id(void);
// Print RSSI and temperature values
static void print_values(conn_properties_t *conn_properties);

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  // Initialize connection properties
  init_properties();
  sl_app_log("soc_thermometer_client initialised\n");
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t* evt)
{
  sl_status_t sc;
  uint8_t *char_value;
  uint16_t addr_value;
  uint8_t table_index;

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
      // Print bluetooth address.
      print_bluetooth_address();
      // Set GATT Attribute System ID
      set_attribute_system_id();

      // Set passive scanning on 1Mb PHY
      sc = sl_bt_scanner_set_mode(gap_1m_phy, SCAN_PASSIVE);
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to set discovery type\n",
                    (int)sc);
      // Set scan interval and scan window
      sc = sl_bt_scanner_set_timing(gap_1m_phy, SCAN_INTERVAL, SCAN_WINDOW);
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to set discovery timing\n",
                    (int)sc);
      // Set the default connection parameters for subsequent connections
      sc = sl_bt_connection_set_default_parameters(CONN_INTERVAL_MIN,
                                                   CONN_INTERVAL_MAX,
                                                   CONN_SLAVE_LATENCY,
                                                   CONN_TIMEOUT,
                                                   CONN_MIN_CE_LENGTH,
                                                   CONN_MAX_CE_LENGTH);
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to set connection timing\n",
                    (int)sc);
      // Start scanning - looking for thermometer devices
      sc = sl_bt_scanner_start(gap_1m_phy, scanner_discover_generic);
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to start discovery #1\n",
                    (int)sc);
      conn_state = scanning;
      break;

    // -------------------------------
    // This event is generated when an advertisement packet or a scan response
    // is received from a slave
    case sl_bt_evt_scanner_scan_report_id:
      // Parse advertisement packets
      if (evt->data.evt_scanner_scan_report.packet_type == 0) {
        // If a thermometer advertisement is found...
        if (find_service_in_advertisement(&(evt->data.evt_scanner_scan_report.data.data[0]),
                                          evt->data.evt_scanner_scan_report.data.len) != 0) {
          // then stop scanning for a while
          sc = sl_bt_scanner_stop();
          sl_app_assert(sc == SL_STATUS_OK,
                        "[E: 0x%04x] Failed to stop discovery\n",
                        (int)sc);
          // and connect to that device
          if (active_connections_num < SL_BT_CONFIG_MAX_CONNECTIONS) {
            sc = sl_bt_connection_open(evt->data.evt_scanner_scan_report.address,
                                       evt->data.evt_scanner_scan_report.address_type,
                                       gap_1m_phy,
                                       NULL);
            sl_app_assert(sc == SL_STATUS_OK,
                          "[E: 0x%04x] Failed to connect\n",
                          (int)sc);
            conn_state = opening;
          }
        }
      }
      break;

    // -------------------------------
    // This event is generated when a new connection is established
    case sl_bt_evt_connection_opened_id:
      // Get last two bytes of sender address
      addr_value = (uint16_t)(evt->data.evt_connection_opened.address.addr[1] << 8) + evt->data.evt_connection_opened.address.addr[0];
      // Add connection to the connection_properties array
      add_connection(evt->data.evt_connection_opened.connection, addr_value);
      // Discover Health Thermometer service on the slave device
      sc = sl_bt_gatt_discover_primary_services_by_uuid(evt->data.evt_connection_opened.connection,
                                                        sizeof(thermo_service),
                                                        (const uint8_t*)thermo_service);
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to discover primary services\n",
                    (int)sc);
      conn_state = discover_services;
      break;

    // -------------------------------
    // This event is generated when a new service is discovered
    case sl_bt_evt_gatt_service_id:
      table_index = find_index_by_connection_handle(evt->data.evt_gatt_service.connection);
      if (table_index != TABLE_INDEX_INVALID) {
        // Save service handle for future reference
        conn_properties[table_index].thermometer_service_handle = evt->data.evt_gatt_service.service;
      }
      break;

    // -------------------------------
    // This event is generated when a new characteristic is discovered
    case sl_bt_evt_gatt_characteristic_id:
      table_index = find_index_by_connection_handle(evt->data.evt_gatt_characteristic.connection);
      if (table_index != TABLE_INDEX_INVALID) {
        // Save characteristic handle for future reference
        conn_properties[table_index].thermometer_characteristic_handle = evt->data.evt_gatt_characteristic.characteristic;
      }
      break;

    // -------------------------------
    // This event is generated for various procedure completions, e.g. when a
    // write procedure is completed, or service discovery is completed
    case sl_bt_evt_gatt_procedure_completed_id:
      table_index = find_index_by_connection_handle(evt->data.evt_gatt_procedure_completed.connection);
      if (table_index == TABLE_INDEX_INVALID) {
        break;
      }
      // If service discovery finished
      if (conn_state == discover_services && conn_properties[table_index].thermometer_service_handle != SERVICE_HANDLE_INVALID) {
        // Discover thermometer characteristic on the slave device
        sc = sl_bt_gatt_discover_characteristics_by_uuid(evt->data.evt_gatt_procedure_completed.connection,
                                                         conn_properties[table_index].thermometer_service_handle,
                                                         sizeof(thermo_char),
                                                         (const uint8_t*)thermo_char);
        sl_app_assert(sc == SL_STATUS_OK,
                      "[E: 0x%04x] Failed to discover characteristics\n",
                      (int)sc);
        conn_state = discover_characteristics;
        break;
      }
      // If characteristic discovery finished
      if (conn_state == discover_characteristics && conn_properties[table_index].thermometer_characteristic_handle != CHARACTERISTIC_HANDLE_INVALID) {
        // stop discovering
        sl_bt_scanner_stop();
        // enable indications
        sc = sl_bt_gatt_set_characteristic_notification(evt->data.evt_gatt_procedure_completed.connection,
                                                        conn_properties[table_index].thermometer_characteristic_handle,
                                                        gatt_indication);
        sl_app_assert(sc == SL_STATUS_OK,
                      "[E: 0x%04x] Failed to set characteristic notification\n",
                      (int)sc);
        conn_state = enable_indication;
        break;
      }
      // If indication enable process finished
      if (conn_state == enable_indication) {
        // and we can connect to more devices
        if (active_connections_num < SL_BT_CONFIG_MAX_CONNECTIONS) {
          // start scanning again to find new devices
          sc = sl_bt_scanner_start(gap_1m_phy, scanner_discover_generic);
          sl_app_assert(sc == SL_STATUS_OK,
                        "[E: 0x%04x] Failed to start discovery #2\n",
                        (int)sc);
          conn_state = scanning;
        } else {
          conn_state = running;
        }
        break;
      }
      break;

    // -------------------------------
    // This event is generated when a connection is dropped
    case sl_bt_evt_connection_closed_id:
      // remove connection from active connections
      remove_connection(evt->data.evt_connection_closed.connection);
      if (conn_state != scanning) {
        // start scanning again to find new devices
        sc = sl_bt_scanner_start(gap_1m_phy, scanner_discover_generic);
        sl_app_assert(sc == SL_STATUS_OK,
                      "[E: 0x%04x] Failed to start discovery #3\n",
                      (int)sc);
        conn_state = scanning;
      }
      break;

    // -------------------------------
    // This event is generated when a characteristic value was received e.g. an indication
    case sl_bt_evt_gatt_characteristic_value_id:
      if (evt->data.evt_gatt_characteristic_value.value.len >= 5) {
        char_value = &(evt->data.evt_gatt_characteristic_value.value.data[0]);
        table_index = find_index_by_connection_handle(evt->data.evt_gatt_characteristic_value.connection);
        if (table_index != TABLE_INDEX_INVALID) {
          conn_properties[table_index].temperature = translate_IEEE_11073_temperature_to_float((IEEE_11073_float *)(char_value + 1));
          conn_properties[table_index].unit = translate_flags_to_temperature_unit(char_value[0]);
        }
      } else {
        sl_app_log("Characteristic value too short: %d\n", evt->data.evt_gatt_characteristic_value.value.len);
      }
      // Send confirmation for the indication
      sc = sl_bt_gatt_send_characteristic_confirmation(evt->data.evt_gatt_characteristic_value.connection);
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to send characteristic confirmation\n",
                    (int)sc);
      // Trigger RSSI measurement on the connection
      sc = sl_bt_connection_get_rssi(evt->data.evt_gatt_characteristic_value.connection);
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to get RSSI\n",
                    (int)sc);
      break;

    // -------------------------------
    // This event is generated when RSSI value was measured
    case sl_bt_evt_connection_rssi_id:
      table_index = find_index_by_connection_handle(evt->data.evt_connection_rssi.connection);
      if (table_index != TABLE_INDEX_INVALID) {
        conn_properties[table_index].rssi = evt->data.evt_connection_rssi.rssi;
      }
      // Print the values
      print_values(conn_properties);
      break;

    default:
      break;
  }
}

// Init connection properties
static void init_properties(void)
{
  uint8_t i;
  active_connections_num = 0;

  for (i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    conn_properties[i].connection_handle = CONNECTION_HANDLE_INVALID;
    conn_properties[i].thermometer_service_handle = SERVICE_HANDLE_INVALID;
    conn_properties[i].thermometer_characteristic_handle = CHARACTERISTIC_HANDLE_INVALID;
    conn_properties[i].temperature = TEMP_INVALID;
    conn_properties[i].unit = UNIT_INVALID;
    conn_properties[i].rssi = RSSI_INVALID;
  }
}

// Parse advertisements looking for advertised Health Thermometer service
static uint8_t find_service_in_advertisement(uint8_t *data, uint8_t len)
{
  uint8_t ad_field_length;
  uint8_t ad_field_type;
  uint8_t i = 0;
  // Parse advertisement packet
  while (i < len) {
    ad_field_length = data[i];
    ad_field_type = data[i + 1];
    // Partial ($02) or complete ($03) list of 16-bit UUIDs
    if (ad_field_type == 0x02 || ad_field_type == 0x03) {
      // compare UUID to Health Thermometer service UUID
      if (memcmp(&data[i + 2], thermo_service, 2) == 0) {
        return 1;
      }
    }
    // advance to the next AD struct
    i = i + ad_field_length + 1;
  }
  return 0;
}

// Find the index of a given connection in the connection_properties array
static uint8_t find_index_by_connection_handle(uint8_t connection)
{
  for (uint8_t i = 0; i < active_connections_num; i++) {
    if (conn_properties[i].connection_handle == connection) {
      return i;
    }
  }
  return TABLE_INDEX_INVALID;
}

// Add a new connection to the connection_properties array
static void add_connection(uint8_t connection, uint16_t address)
{
  conn_properties[active_connections_num].connection_handle = connection;
  conn_properties[active_connections_num].server_address    = address;
  active_connections_num++;
}

// Remove a connection from the connection_properties array
static void remove_connection(uint8_t connection)
{
  uint8_t i;
  uint8_t table_index = find_index_by_connection_handle(connection);

  if (active_connections_num > 0) {
    active_connections_num--;
  }
  // Shift entries after the removed connection toward 0 index
  for (i = table_index; i < active_connections_num; i++) {
    conn_properties[i] = conn_properties[i + 1];
  }
  // Clear the slots we've just removed so no junk values appear
  for (i = active_connections_num; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    conn_properties[i].connection_handle = CONNECTION_HANDLE_INVALID;
    conn_properties[i].thermometer_service_handle = SERVICE_HANDLE_INVALID;
    conn_properties[i].thermometer_characteristic_handle = CHARACTERISTIC_HANDLE_INVALID;
    conn_properties[i].temperature = TEMP_INVALID;
    conn_properties[i].unit = UNIT_INVALID;
    conn_properties[i].rssi = RSSI_INVALID;
  }
}

// Translate a IEEE-11073 Temperature Value to a float Value
static float translate_IEEE_11073_temperature_to_float(IEEE_11073_float const *IEEE_11073_value)
{
  int32_t mantissa = 0;
  uint8_t mantissa_l;
  uint8_t mantissa_m;
  int8_t mantissa_h;
  int8_t exponent;

  // Wrong Argument: NULL pointer is passed
  if ( !IEEE_11073_value ) {
    return NAN;
  }

  // Caching Fields
  mantissa_l = IEEE_11073_value->mantissa_l;
  mantissa_m = IEEE_11073_value->mantissa_m;
  mantissa_h = IEEE_11073_value->mantissa_h;
  exponent =  IEEE_11073_value->exponent;

  // IEEE-11073 Standard NaN Value Passed
  if ((mantissa_l == 0xFF) && (mantissa_m == 0xFF) && (mantissa_h == 0x7F) && (exponent == 0x00)) {
    return NAN;
  }

  // Converting a 24bit Signed Value to a 32bit Signed Value
  mantissa |= mantissa_h;
  mantissa <<= 8;
  mantissa |= mantissa_m;
  mantissa <<= 8;
  mantissa |= mantissa_l;
  mantissa <<= 8;
  mantissa >>= 8;

  return ((float)mantissa) * pow(10.0f, (float)exponent);
}

/**************************************************************************//**
 * @brief
 *   Function to Read and Cache Bluetooth Address.
 * @param address_type_out [out]
 *   A pointer to the outgoing address_type. This pointer can be NULL.
 * @return
 *   Pointer to the cached Bluetooth Address
 *****************************************************************************/
static bd_addr *read_and_cache_bluetooth_address(uint8_t *address_type_out)
{
  static bd_addr address;
  static uint8_t address_type;
  static bool cached = false;

  if (!cached) {
    sl_status_t sc = sl_bt_system_get_identity_address(&address, &address_type);
    sl_app_assert(sc == SL_STATUS_OK,
                  "[E: 0x%04x] Failed to get Bluetooth address\n",
                  (int)sc);
    cached = true;
  }

  if (address_type_out) {
    *address_type_out = address_type;
  }

  return &address;
}

/**************************************************************************//**
 * @brief
 *   Function to Print Bluetooth Address.
 * @return
 *   None
 *****************************************************************************/
static void print_bluetooth_address(void)
{
  uint8_t address_type;
  bd_addr *address = read_and_cache_bluetooth_address(&address_type);

  sl_app_log("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X\n",
             address_type ? "static random" : "public device",
             address->addr[5],
             address->addr[4],
             address->addr[3],
             address->addr[2],
             address->addr[1],
             address->addr[0]);
}

/**************************************************************************//**
 * @brief
 *   Function to Set GATT Attribute System ID.
 * @return
 *   None
 *****************************************************************************/
static void set_attribute_system_id(void)
{
  sl_status_t sc;
  uint8_t system_id[8];
  bd_addr *address = read_and_cache_bluetooth_address(NULL);

  // Pad and reverse unique ID to get System ID
  system_id[0] = address->addr[5];
  system_id[1] = address->addr[4];
  system_id[2] = address->addr[3];
  system_id[3] = 0xFF;
  system_id[4] = 0xFE;
  system_id[5] = address->addr[2];
  system_id[6] = address->addr[1];
  system_id[7] = address->addr[0];

  sc = sl_bt_gatt_server_write_attribute_value(gattdb_system_id,
                                               0,
                                               sizeof(system_id),
                                               system_id);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to write attribute System ID\n",
                (int)sc);
}

static void print_values(conn_properties_t *conn_properties)
{
  static bool print_header = true;
  uint8_t i;

  if (true == print_header) {
    print_header = false;
    for (i = 0u; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
      sl_app_log("ADDR   TEMP   RSSI |");
    }
    sl_app_log("\n");
  }
  for (i = 0u; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    if ((TEMP_INVALID != conn_properties[i].temperature) && (RSSI_INVALID != conn_properties[i].rssi) ) {
      sl_app_log("%04x ", conn_properties[i].server_address);
      sl_app_log("%6.2f", conn_properties[i].temperature);
      sl_app_log("%c ", conn_properties[i].unit);
      sl_app_log("% 3d", conn_properties[i].rssi);
      sl_app_log("dBm|");
    } else {
      sl_app_log("---- ------- ------|");
    }
  }
  sl_app_log("\r");
}

#ifdef SL_CATALOG_CLI_PRESENT
void hello(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  print_bluetooth_address();
}
#endif // SL_CATALOG_CLI_PRESENT
