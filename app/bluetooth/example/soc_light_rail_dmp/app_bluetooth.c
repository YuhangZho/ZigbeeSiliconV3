/***************************************************************************//**
 * @file
 * @brief Bluetooth application logic.
 *
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
#include "sl_app_assert.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT
#ifdef SL_CATALOG_CLI_PRESENT
#include "sl_cli.h"
#endif // SL_CATALOG_CLI_PRESENT
#include "demo-ui.h"
#include "sl_bt_api.h"
#include "sl_bt_rtos_adaptation.h"
#include "printf.h"
#include "rtos_err.h"
#include "os.h"
#include "infrastructure.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#include "app_bluetooth.h"
#include "app_proprietary.h"
#include "string.h"

// Timer Frequency used.
#define TIMER_CLK_FREQ ((uint32_t)32768)
// Convert msec to timer ticks.
#define TIMER_MS_2_TIMERTICK(ms) ((TIMER_CLK_FREQ * ms) / 1000)
#define TIMER_S_2_TIMERTICK(s) (TIMER_CLK_FREQ * s)

#define BLUETOOTH_EVENT_FLAG_APP_INDICATE_START   ((OS_FLAGS)64)   //notify clients
#define BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS ((OS_FLAGS)128)  //notified clients
#define BLUETOOTH_EVENT_FLAG_APP_INDICATE_TIMEOUT ((OS_FLAGS)256)  //notify timeout
#define BLUETOOTH_EVENT_FLAG_APP_MASK             (BLUETOOTH_EVENT_FLAG_APP_INDICATE_START     \
                                                   + BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS \
                                                   + BLUETOOTH_EVENT_FLAG_APP_INDICATE_TIMEOUT)

#define LIGHT_STATE_GATTDB     gattdb_light_state_rail
#define TRIGGER_SOURCE_GATTDB  gattdb_trigger_source_rail
#define SOURCE_ADDRESS_GATTDB  gattdb_source_address_rail
/* Write response codes*/
#define ES_WRITE_OK                         0
#define DEV_ID_STR_LEN                    9u

// Demo Application task
#define DEMO_APP_TASK_PRIO                7u
#define DEMO_APP_TASK_STK_SIZE            512u

// BT Application task
#define BT_APP_TASK_PRIO                  6u
#define BT_APP_TASK_STACK_SIZE            (2000 / sizeof(CPU_STK))

// We need to put the device name into a scan response packet,
// since it isn't included in the 'standard' beacons -
// I've included the flags, since certain apps seem to expect them
#define DEVNAME "DMP%02X%02X"
#define DEVNAME_LEN 8  // incl term null
#define UUID_LEN 16 // 128-bit UUID
#define IBEACON_MAJOR_NUM 0x0200 // 16-bit major number

typedef enum {
  BLE_STATE_READY                   = 0,
  BLE_STATE_INDICATE_LIGHT          = 1,
  BLE_STATE_INDICATE_TRIGGER_SOURCE = 2,
  BLE_STATE_INDICATE_SOURCE_ADDRESS = 3
} ble_state_t;

typedef struct {
  uint8_t handle;
  bd_addr address;
  bool in_use;
  uint8_t indicated;
} ble_conn_t;

typedef enum {
  demo_tmr_indicate   = 0x02
} demo_tmr_t;

// iBeacon structure and data
typedef struct {
  uint8_t flags_len;      /* Length of the Flags field.             */
  uint8_t flags_type;     /* Type of the Flags field.               */
  uint8_t flags;          /* Flags field.                           */
  uint8_t mandata_len;    /* Length of the Manufacturer Data field. */
  uint8_t mandata_type;   /* Type of the Manufacturer Data field.   */
  uint8_t comp_id[2];     /* Company ID field.                      */
  uint8_t beac_type[2];   /* Beacon Type field.                     */
  uint8_t uuid[16];       /* 128-bit Universally Unique Identifier (UUID). The UUID is an identifier for the company using the beacon.        */
  uint8_t maj_num[2];     /* Beacon major number. Used to group related beacons.                                                              */
  uint8_t min_num[2];     /* Beacon minor number. Used to specify individual beacons within a group.                                          */
  uint8_t tx_power;       /* The Beacon's measured RSSI at 1 meter distance in dBm. See the iBeacon specification for measurement guidelines. */
} i_beacon_data_t;

static i_beacon_data_t i_beacon_data = {
  /* Flag bits - See Bluetooth 4.0 Core Specification , Volume 3, Appendix C, 18.1 for more details on flags. */
  2,  /* length  */
  0x01, /* type */
  0x04 | 0x02, /* Flags: LE General Discoverable Mode, BR/EDR is disabled. */

  /* Manufacturer specific data */
  26,  /* length of field*/
  0xFF, /* type of field */

  /* The first two data octets shall contain a company identifier code from
   * the Assigned Numbers - Company Identifiers document */
  { UINT16_TO_BYTES(0x004C) },

  /* Beacon type */
  /* 0x0215 is iBeacon */
  { UINT16_TO_BYTE1(0x0215), UINT16_TO_BYTE0(0x0215) },

  /* 128 bit / 16 byte UUID - generated specially for the DMP Demo */
  { 0x00, 0x47, 0xe7, 0x0a, 0x5d, 0xc1, 0x47, 0x25, 0x87, 0x99, 0x83, 0x05, 0x44, 0xae, 0x04, 0xf6 },

  /* Beacon major number */
  { UINT16_TO_BYTE1(IBEACON_MAJOR_NUM), UINT16_TO_BYTE0(IBEACON_MAJOR_NUM) },

  /* Beacon minor number  - not used for this application*/
  { UINT16_TO_BYTE1(0), UINT16_TO_BYTE0(0) },

  /* The Beacon's measured RSSI at 1 meter distance in dBm */
  /* 0xD8 is -40dBm */
  // TBD: check?
  0xD8
};

typedef struct {
  uint8_t flags_len;          /* Length of the Flags field.                             */
  uint8_t flags_type;         /* Type of the Flags field.                               */
  uint8_t flags;              /* Flags field.                                           */
  uint8_t ser_len;            /* Length of Complete list of 16-bit Service UUIDs.       */
  uint8_t ser_type;           /* Complete list of 16-bit Service UUIDs.                 */
  uint8_t service_list[2];    /* Complete list of 16-bit Service UUIDs.                 */
  uint8_t ser_data_length;    /* Length of Service Data.                                */
  uint8_t ser_data_type;      /* Type of Service Data.                                  */
  uint8_t uuid[2];            /* 16-bit Eddystone UUID.                                 */
  uint8_t frame_type;         /* Frame type.                                            */
  uint8_t tx_power;           /* The Beacon's measured RSSI at 0 meter distance in dBm. */
  uint8_t url_prefix;         /* URL prefix type.                                       */
  uint8_t url[10];            /* URL.                                                   */
} eddystone_data_t;

static eddystone_data_t eddystone_data = {
  /* Flag bits - See Bluetooth 4.0 Core Specification , Volume 3, Appendix C, 18.1 for more details on flags. */
  2,                /* length  */
  0x01,             /* type */
  0x04 | 0x02,      /* Flags: LE General Discoverable Mode, BR/EDR is disabled. */
  /* Service field length */
  0x03,
  /* Service field type */
  0x03,
  /* 16-bit Eddystone UUID */
  { UINT16_TO_BYTES(0xFEAA) },
  /* Eddystone-TLM Frame length */
  0x10,
  /* Service Data data type value */
  0x16,
  /* 16-bit Eddystone UUID */
  { UINT16_TO_BYTES(0xFEAA) },
  /* Eddystone-URL Frame type */
  0x10,
  /* Tx power */
  0x01,
  /* URL prefix - standard */
  0x00,
  /* URL */
  { 's', 'i', 'l', 'a', 'b', 's', '.', 'c', 'o', 'm' }
};

OS_FLAG_GRP bluetooth_app_flags;

static OS_Q    demo_queue;
static ble_conn_t ble_conn[SL_BT_CONFIG_MAX_CONNECTIONS];
static OS_TMR  demo_timer;
static void demo_timer_cb(void *p_tmr, void *p_arg);
static OS_TMR  demo_timer_direction;
static void demo_timer_direction_cb(void *p_tmr, void *p_arg);

static CPU_STK app_task_demo_stk[DEMO_APP_TASK_STK_SIZE];
static CPU_STK bt_event_task_demo_stk[BT_APP_TASK_STACK_SIZE];
static OS_TCB  demo_app_task_TCB;
static OS_TCB  bt_app_task_TCB;
static void    demo_app_task(void *p_arg);
static void    bt_event_handler_task(void *p_arg);

// The advertising set handle allocated from Bluetooth stack
static uint8_t advertising_set_handle = 0xff;
static uint8_t advertising_set_handle1 = 0xff;
static uint8_t advertising_set_handle2 = 0xff;

typedef struct {
  uint8_t flags_len;                /* Length of the Flags field.       */
  uint8_t flags_type;               /* Type of the Flags field.         */
  uint8_t flags;                    /* Flags field.                     */
  uint8_t short_name_len;           /* Length of Shortened Local Name.  */
  uint8_t short_name_type;          /* Shortened Local Name.            */
  uint8_t short_name[DEVNAME_LEN];  /* Shortened Local Name.            */
  uint8_t uuid_length;              /* Length of UUID.                  */
  uint8_t uuid_type;                /* Type of UUID.                    */
  uint8_t uuid[UUID_LEN];           /* 128-bit UUID.                    */
} response_data_t;

static response_data_t response_data = {
  2,  /* length (incl type) */
  0x01, /* type */
  0x04 | 0x02, /* Flags: LE General Discoverable Mode, BR/EDR is disabled. */
  DEVNAME_LEN + 1,        // length of local name (incl type)
  0x08,               // shortened local name
  { 'D', 'M', '0', '0', ':', '0', '0' },
  UUID_LEN + 1,           // length of UUID data (incl type)
  0x06,               // incomplete list of service UUID's
  // custom service UUID for silabs light in little-endian format
  { 0x13, 0x87, 0x37, 0x25, 0x42, 0xb0, 0xc3, 0xbf, 0x78, 0x40, 0x83, 0xb5, 0xe4, 0x96, 0xf5, 0x63 }
};

demo_t demo = {
  .state = DEMO_STATE_INIT,
  .light = demo_light_off,
  .light_ind = gatt_disable,
  .direction = demo_light_direction_button,
  .direction_ind = gatt_disable,
  .src_addr = { { 0, 0, 0, 0, 0, 0, 0, 0 } },
  .src_addr_ind = gatt_disable,
  .conn_bluetooth_in_use = 0,
  .conn_proprietary_in_use = 0,
  .indication_ongoing = false,
  .indication_pending = false,
  .own_addr = { { 0, 0, 0, 0, 0, 0, 0, 0 } }
};

void light_pend(RTOS_ERR *err);
void light_post(RTOS_ERR *err);
void demo_queue_post(demo_msg_t msg, RTOS_ERR *err);
void init_demo_app(void);

// Light Mutex
static OS_MUTEX light_mutex;
static void bluetooth_flag_set(OS_FLAGS flag, RTOS_ERR *err);
static void bluetooth_flag_clr(OS_FLAGS flag, RTOS_ERR *err);
static demo_msg_t demo_queue_pend(RTOS_ERR *err);

// single timer handle
static sl_sleeptimer_timer_handle_t app_single_timer;
// single timer callback
static void app_single_timer_cb(sl_sleeptimer_timer_handle_t *handle,
                                void *data);

// single shot timer callback
static void app_single_timer_cb(sl_sleeptimer_timer_handle_t *handle,
                                void *data)
{
  (void)data;
  (void)handle;
  RTOS_ERR err;

  bluetooth_flag_set(BLUETOOTH_EVENT_FLAG_APP_INDICATE_TIMEOUT, &err);
}

void init_demo_app(void)
{
  RTOS_ERR err;

  RTOS_ERR os_err;

  OSFlagCreate(&bluetooth_app_flags,
               "Bluetooth App Flags",
               (OS_FLAGS)0,
               &os_err);

  OSMutexCreate(&light_mutex,
                "Light Mutex",
                &err);

  OSQCreate((OS_Q     *)&demo_queue,
            (CPU_CHAR *)"Demo Queue",
            (OS_MSG_QTY) 32,
            (RTOS_ERR *)&err);

  // Create the Demo task.
  OSTaskCreate((OS_TCB     *)&demo_app_task_TCB,
               (CPU_CHAR   *)"Demo Task",
               (OS_TASK_PTR ) demo_app_task,
               (void       *) 0,
               (OS_PRIO     ) DEMO_APP_TASK_PRIO,
               (CPU_STK    *)&app_task_demo_stk[0],
               (CPU_STK     )(DEMO_APP_TASK_STK_SIZE / 10u),
               (CPU_STK_SIZE) DEMO_APP_TASK_STK_SIZE,
               (OS_MSG_QTY  ) 0,
               (OS_TICK     ) 0,
               (void       *) 0,
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (RTOS_ERR     *)&err);

  // Create BT event handler Task
  OSTaskCreate((OS_TCB     *)&bt_app_task_TCB,
               (CPU_CHAR   *)"BT event handler Task",
               (OS_TASK_PTR ) bt_event_handler_task,
               (void       *) 0,
               (OS_PRIO     ) BT_APP_TASK_PRIO,
               (CPU_STK    *)&bt_event_task_demo_stk[0],
               (CPU_STK     )(BT_APP_TASK_STACK_SIZE / 10u),
               (CPU_STK_SIZE) BT_APP_TASK_STACK_SIZE,
               (OS_MSG_QTY  ) 0,
               (OS_TICK     ) 0,
               (void       *) 0,
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               (RTOS_ERR     *)&err);
}

static void bluetooth_flag_set(OS_FLAGS flag, RTOS_ERR* err)
{
  OSFlagPost((OS_FLAG_GRP *)&bluetooth_app_flags,
             (OS_FLAGS) flag,
             (OS_OPT) OS_OPT_POST_FLAG_SET,
             (RTOS_ERR *) err);

  sl_app_assert(err->Code == RTOS_ERR_NONE,
                "[E: 0x%04x] Failed to set ble flag\n",
                (int)err->Code);
}

static void bluetooth_flag_clr(OS_FLAGS flag, RTOS_ERR *err)
{
  OSFlagPost((OS_FLAG_GRP *)&bluetooth_app_flags,
             (OS_FLAGS) flag,
             (OS_OPT) OS_OPT_POST_FLAG_CLR,
             (RTOS_ERR *) err);

  sl_app_assert(err->Code == RTOS_ERR_NONE,
                "[E: 0x%04x] Failed to clear ble flag\n",
                (int)err->Code);
}

void light_pend(RTOS_ERR *err)
{
  OSMutexPend((OS_MUTEX *)&light_mutex,
              (OS_TICK) 0,
              (OS_OPT) OS_OPT_PEND_BLOCKING,
              (CPU_TS *) DEF_NULL,
              (RTOS_ERR *) err);

  sl_app_assert(err->Code == RTOS_ERR_NONE,
                "[E: 0x%04x] Failed to pend light_mutex\n",
                (int)err->Code);
}

void light_post(RTOS_ERR *err)
{
  OSMutexPost((OS_MUTEX *)&light_mutex,
              (OS_OPT) OS_OPT_POST_NONE,
              (RTOS_ERR *) err);

  sl_app_assert(err->Code == RTOS_ERR_NONE,
                "[E: 0x%04x] Failed to post light_mutex\n",
                (int)err->Code);
}

static bd_addr* bleGetAddr(uint8_t handle)
{
  for (uint8_t i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    if (ble_conn[i].handle == handle) {
      return &(ble_conn[i].address);
    }
  }
  return (bd_addr *)DEF_NULL;
}

void appUiLedOff(void)
{
  sl_led_led0.turn_off(sl_led_led0.context);
  sl_led_led1.turn_off(sl_led_led1.context);
}

void appUiLedOn(void)
{
  sl_led_led0.turn_on(sl_led_led0.context);
  sl_led_led1.turn_on(sl_led_led1.context);
}

static demo_msg_t demo_queue_pend(RTOS_ERR *err)
{
  demo_msg_t demo_msg;
  OS_MSG_SIZE demo_msg_size;
  demo_msg = (demo_msg_t)OSQPend((OS_Q *)&demo_queue,
                                 (OS_TICK) 0,
                                 (OS_OPT) OS_OPT_PEND_BLOCKING,
                                 (OS_MSG_SIZE *)&demo_msg_size,
                                 (CPU_TS *) DEF_NULL,
                                 (RTOS_ERR *) err);

  sl_app_assert(err->Code == RTOS_ERR_NONE,
                "[E: 0x%04x] OS error demo_queue_pend\n",
                (int)err->Code);

  return demo_msg;
}

void demo_queue_post(demo_msg_t msg, RTOS_ERR *err)
{
  OSQPost((OS_Q *)&demo_queue,
          (void *) msg,
          (OS_MSG_SIZE) sizeof(void*),
          (OS_OPT) OS_OPT_POST_FIFO + OS_OPT_POST_ALL,
          (RTOS_ERR *) err);

  sl_app_assert(err->Code == RTOS_ERR_NONE,
                "[E: 0x%04x] OS error demo_queue_post\n",
                (int)err->Code);
}

/**************************************************************************//**
 * Timer callback.
 *
 * @param p_tmr pointer to the user-allocated timer.
 * @param p_arg argument passed when creating the timer.
 *
 *****************************************************************************/
static void demo_timer_cb(void *p_tmr, void *p_arg)
{
  PP_UNUSED_PARAM(p_tmr);
  PP_UNUSED_PARAM(p_arg);
  RTOS_ERR err;
  demo_queue_post(DEMO_EVT_INDICATION, &err);
}

static void demo_timer_direction_cb(void *p_tmr, void *p_arg)
{
  PP_UNUSED_PARAM(p_tmr);
  PP_UNUSED_PARAM(p_arg);
  RTOS_ERR err;
  demo_queue_post(DEMO_EVT_CLEAR_DIRECTION, &err);
}

// return false on error
static bool ble_add_conn(uint8_t handle, bd_addr *address)
{
  for (uint8_t i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    if (ble_conn[i].in_use == false) {
      ble_conn[i].handle = handle;
      Mem_Copy((void*)&ble_conn[i].address,
               (void*)address,
               sizeof(ble_conn[i].address));
      ble_conn[i].in_use = true;
      return true;
    }
  }
  return false;
}

static bool ble_remove_conn(uint8_t handle)
{
  for (uint8_t i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
    if (ble_conn[i].handle == handle) {
      ble_conn[i].handle = 0;
      Mem_Set((void*)&ble_conn[i].address.addr, 0, sizeof(ble_conn[i].address.addr));
      ble_conn[i].in_use = false;
      return true;
    }
  }
  return false;
}

/**************************************************************************//**
 * Simple Button
 * Button state changed callback
 * @param[in] handle Button enevnt handle
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  RTOS_ERR err;

  // button pressed
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (&sl_button_btn0 == handle) {
      demo_queue_post(DEMO_EVT_BUTTON0_PRESSED, &err);
    } else if (&sl_button_btn1 == handle) {
      demo_queue_post(DEMO_EVT_BUTTON1_PRESSED, &err);
    }
  }
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  RTOS_ERR err;
  sl_status_t sc = SL_STATUS_OK;
  bd_addr address;
  uint8_t address_type;
  uint8_t system_id[8];
  uint8_t addr[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  uint8_t *addr_poi = addr;
  uint16_t sent_len = 0;

  // -------------------------------
  // Bluetooth stack event handling.
  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!       //
    case sl_bt_evt_system_boot_id:
      light_pend(&err);
      // Extract unique ID from BT Address
      sc = sl_bt_system_get_identity_address(&address, &address_type);
      sl_app_assert(sc == SL_STATUS_OK,
                    "[E: 0x%04x] Failed to get Bluetooth address\n",
                    (int)sc);

      // Pad and reverse unique ID to get System ID
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
                    "[E: 0x%04x] Failed to get Bluetooth address\n",
                    (int)sc);

      // own address: Bluetooth device address
      Mem_Set((void *)demo.own_addr.addr, 0, sizeof(demo.own_addr.addr));
      Mem_Copy((void *)demo.own_addr.addr,
               (void *)&address,
               sizeof(address));
      demo.indication_ongoing = false;
      demo_queue_post(DEMO_EVT_BOOTED, &err);
      light_post(&err);
      break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      sc = ble_add_conn(evt->data.evt_connection_opened.connection,
                        (bd_addr *)&evt->data.evt_connection_opened.address);
      sl_app_assert(sc == true,
                    "[E: 0x%04x] Failed to add conn\n",
                    (int)sc);

      demo_queue_post(DEMO_EVT_BLUETOOTH_CONNECTED, &err);
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      sc = ble_remove_conn(evt->data.evt_connection_closed.connection);
      sl_app_assert(sc == true,
                    "[E: 0x%04x] Failed to remove conn\n",
                    (int)sc);

      demo_queue_post(DEMO_EVT_BLUETOOTH_DISCONNECTED, &err);
      break;

    // -------------------------------
    // This event indicates that a remote GATT client is attempting to write
    // a value of a user type attribute in to the local GATT database.
    case sl_bt_evt_gatt_server_user_write_request_id:
      // light state write
      if (LIGHT_STATE_GATTDB == evt->data.evt_gatt_server_user_write_request.characteristic) {
        addr_poi = (uint8_t*)bleGetAddr(evt->data.evt_gatt_server_user_write_request.connection);
        sl_app_assert(addr_poi != NULL,
                      "[E: 0x%04x] Failed to get ble address\n",
                      (int)sc);

        light_pend(&err);

        demo.light = (demo_light_t)evt->data.evt_gatt_server_user_write_request.value.data[0];
        demo.direction = demo_light_direction_bluetooth;
        Mem_Copy(demo.src_addr.addr, (void const*)addr_poi, sizeof(bd_addr));

        light_post(&err);
        /* Send response to write request */
        sl_bt_gatt_server_send_user_write_response(evt->data.evt_gatt_server_user_write_request.connection,
                                                   LIGHT_STATE_GATTDB,
                                                   ES_WRITE_OK);

        sl_app_assert(sc == SL_STATUS_OK,
                      "[E: 0x%04x] Failed to send_user_write_response LIGHT_STATE_GATTDB\n",
                      (int)sc);

        demo_queue_post(DEMO_EVT_LIGHT_CHANGED_BLUETOOTH, &err);
      }
      break;

    // -------------------------------
    // This event indicates that a remote GATT client is attempting to read a value of an
    // attribute from the local GATT database, where the attribute was defined in the GATT
    // XML firmware configuration file to have type="user".
    case sl_bt_evt_gatt_server_user_read_request_id:
      // light state read
      if (LIGHT_STATE_GATTDB == evt->data.evt_gatt_server_user_read_request.characteristic) {
        /* Send response to read request */
        light_pend(&err);
        sc = sl_bt_gatt_server_send_user_read_response(evt->data.evt_gatt_server_user_read_request.connection,
                                                       LIGHT_STATE_GATTDB,
                                                       0,
                                                       sizeof(demo.light),
                                                       (uint8_t*)&demo.light,
                                                       &sent_len
                                                       );

        sl_app_assert(sc == SL_STATUS_OK,
                      "[E: 0x%04x] Failed to send user read response LIGHT_STATE_GATTDB\n",
                      (int)sc);

        light_post(&err);

        // trigger source read
      } else if (TRIGGER_SOURCE_GATTDB == evt->data.evt_gatt_server_user_read_request.characteristic) {
        /* Send response to read request */
        light_pend(&err);
        sc = sl_bt_gatt_server_send_user_read_response(evt->data.evt_gatt_server_user_read_request.connection,
                                                       TRIGGER_SOURCE_GATTDB,
                                                       0,
                                                       sizeof(demo.direction),
                                                       (uint8_t *)&demo.direction,
                                                       &sent_len
                                                       );

        sl_app_assert(sc == SL_STATUS_OK,
                      "[E: 0x%04x] Failed to send user read response TRIGGER_SOURCE_GATTDB\n",
                      (int)sc);

        light_post(&err);
        // source address read
      } else if (SOURCE_ADDRESS_GATTDB == evt->data.evt_gatt_server_user_read_request.characteristic) {
        /* Send response to read request */
        light_pend(&err);
        sc = sl_bt_gatt_server_send_user_read_response(evt->data.evt_gatt_server_user_read_request.connection,
                                                       SOURCE_ADDRESS_GATTDB,
                                                       0,
                                                       sizeof(demo.src_addr.addr),
                                                       (uint8_t*)demo.src_addr.addr,
                                                       &sent_len
                                                       );

        sl_app_assert(sc == SL_STATUS_OK,
                      "[E: 0x%04x] Failed to send user read response SOURCE_ADDRESS_GATTDB\n",
                      (int)sc);

        light_post(&err);
        // unhandled read
      }
      break;

    // -------------------------------
    // This event indicates either that a local Client Characteristic Configuration descriptor
    // has been changed by the remote GATT client, or that a confirmation from the remote GATT
    // client was received upon a successful reception of the indication.
    case sl_bt_evt_gatt_server_characteristic_status_id:
      if (LIGHT_STATE_GATTDB == evt->data.evt_gatt_server_characteristic_status.characteristic) {
        // confirmation of indication received from remote GATT client
        if (gatt_server_confirmation == (gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
          // stop light indication confirmation timer
          bool running = false;
          sc = sl_sleeptimer_is_timer_running(&app_single_timer, &running);
          sl_app_assert(sc == SL_STATUS_OK,
                        "[E: 0x%04x] Failed to read timer\n",
                        (int)sc);
          if (running) {
            sc = sl_sleeptimer_stop_timer(&app_single_timer);
            sl_app_assert(sc == SL_STATUS_OK,
                          "[E: 0x%04x] Failed to stop timer\n",
                          (int)sc);
          }
          bluetooth_flag_set(BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS, &err);
          // client characteristic configuration changed by remote GATT client
        } else if (gatt_server_client_config == (gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
          light_pend(&err);
          demo.light_ind = (gatt_client_config_flag_t)evt->data.evt_gatt_server_characteristic_status.client_config_flags;
          light_post(&err);
          // unhandled event
        } else {
        }
      } else if (TRIGGER_SOURCE_GATTDB == evt->data.evt_gatt_server_characteristic_status.characteristic) {
        // confirmation of indication received from GATT client
        if (gatt_server_confirmation == (gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
          // stop direction indication confirmation timer
          bool running = false;
          sc = sl_sleeptimer_is_timer_running(&app_single_timer, &running);
          sl_app_assert(sc == SL_STATUS_OK,
                        "[E: 0x%04x] Failed to read timer\n",
                        (int)sc);
          if (running) {
            sc = sl_sleeptimer_stop_timer(&app_single_timer);
            sl_app_assert(sc == SL_STATUS_OK,
                          "[E: 0x%04x] Failed to stop timer\n",
                          (int)sc);
          }

          bluetooth_flag_set(BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS, &err);
          // client characteristic configuration changed by remote GATT client
        } else if (gatt_server_client_config == (gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
          light_pend(&err);
          demo.direction_ind = (gatt_client_config_flag_t)evt->data.evt_gatt_server_characteristic_status.client_config_flags;
          light_post(&err);
          // unhandled event
        } else {
        }
      } else if (SOURCE_ADDRESS_GATTDB == evt->data.evt_gatt_server_characteristic_status.characteristic) {
        // confirmation of indication received from GATT client
        if (gatt_server_confirmation == (gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
          // stop direction indication confirmation timer
          bool running = false;
          sc = sl_sleeptimer_is_timer_running(&app_single_timer, &running);
          sl_app_assert(sc == SL_STATUS_OK,
                        "[E: 0x%04x] Failed to read timer\n",
                        (int)sc);
          if (running) {
            sc = sl_sleeptimer_stop_timer(&app_single_timer);
            sl_app_assert(sc == SL_STATUS_OK,
                          "[E: 0x%04x] Failed to stop timer\n",
                          (int)sc);
          }

          bluetooth_flag_set(BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS, &err);
          // client characteristic configuration changed by remote GATT client
        } else if (gatt_server_client_config == (gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags) {
          light_pend(&err);
          demo.src_addr_ind = (gatt_client_config_flag_t)evt->data.evt_gatt_server_characteristic_status.client_config_flags;
          light_post(&err);
          // unhandled event
        } else {
        }
      } else {
      }
      break;

    // -------------------------------
    // Software Timer event
    case sl_bt_evt_system_soft_timer_id:
      if (demo_tmr_indicate == evt->data.evt_system_soft_timer.handle) {
        bluetooth_flag_set(BLUETOOTH_EVENT_FLAG_APP_INDICATE_TIMEOUT, &err);
      } else {
      }
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
 * Bluetooth event task.
 * @param[in] p_arg the argument passed by 'OSTaskCreate()'.
 *****************************************************************************/
static void bt_event_handler_task(void *p_arg)
{
  PP_UNUSED_PARAM(p_arg);
  RTOS_ERR err;
  OS_FLAGS flags = 0;
  ble_state_t ble_state = BLE_STATE_READY;
  uint16_t len = 0;
  sl_status_t sc;

  while (DEF_TRUE) {
    // wait for Bluetooth events; do not consume set flag
    flags |= OSFlagPend(&bluetooth_app_flags,
                        (OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_INDICATE_START
                        + (OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS
                        + (OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_INDICATE_TIMEOUT,
                        (OS_TICK)0,
                        OS_OPT_PEND_BLOCKING + OS_OPT_PEND_FLAG_SET_ANY,
                        NULL,
                        &err);
    // -------------------------------
    // Application event handling.
    if (flags & (OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_MASK) {
      switch (ble_state) {
        case BLE_STATE_READY:
          // send out indication
          if (flags & (OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_INDICATE_START) {
            flags &= ~(OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_INDICATE_START;
            bluetooth_flag_clr(BLUETOOTH_EVENT_FLAG_APP_INDICATE_START, &err);

            if (demo.light_ind == gatt_indication) {
              // start timer for light indication confirmation
              sc = sl_sleeptimer_start_timer_ms(&app_single_timer,
                                                1000,
                                                app_single_timer_cb,
                                                (void *)NULL,
                                                0,
                                                0);
              sl_app_assert(sc == SL_STATUS_OK,
                            "[E: 0x%04x] Failed to start timer\n",
                            (int)sc);

              light_pend(&err);
              /* Send notification/indication data */
              sl_bt_gatt_server_send_characteristic_notification(ble_conn[0].handle,
                                                                 LIGHT_STATE_GATTDB,
                                                                 (uint8_t)sizeof(demo.light),
                                                                 (uint8_t*)&demo.light,
                                                                 &len);
              sl_app_assert(sc == SL_STATUS_OK,
                            "[E: 0x%04x] Failed to send_characteristic_notification\n",
                            (int)sc);

              light_post(&err);
            } else {
              bluetooth_flag_set(BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS, &err);
            }
            ble_state = BLE_STATE_INDICATE_LIGHT;
          }
          break;

        case BLE_STATE_INDICATE_LIGHT:
          if (flags & (OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS) {
            flags &= ~(OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS;
            bluetooth_flag_clr(BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS, &err);

            if (demo.direction_ind == gatt_indication) {
              // start timer for trigger source indication confirmation
              sc = sl_sleeptimer_start_timer_ms(&app_single_timer,
                                                1000,
                                                app_single_timer_cb,
                                                (void *)NULL,
                                                0,
                                                0);
              sl_app_assert(sc == SL_STATUS_OK,
                            "[E: 0x%04x] Failed to start timer\n",
                            (int)sc);

              light_pend(&err);
              /* Send notification/indication data */
              sl_bt_gatt_server_send_characteristic_notification(ble_conn[0].handle,
                                                                 TRIGGER_SOURCE_GATTDB,
                                                                 (uint8_t)sizeof(demo.direction),
                                                                 (uint8_t*)&demo.direction,
                                                                 &len);
              sl_app_assert(sc == SL_STATUS_OK,
                            "[E: 0x%04x] Failed to send_characteristic_notification\n",
                            (int)sc);

              light_post(&err);
            } else {
              bluetooth_flag_set(BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS, &err);
            }
            ble_state = BLE_STATE_INDICATE_TRIGGER_SOURCE;
          } else if (flags & (OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_INDICATE_TIMEOUT) {
            flags &= ~(OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_INDICATE_TIMEOUT;
            bluetooth_flag_clr(BLUETOOTH_EVENT_FLAG_APP_INDICATE_TIMEOUT, &err);
            demo_queue_post(DEMO_EVT_INDICATION_FAILED, &err);
            ble_state = BLE_STATE_READY;
          }
          break;

        case BLE_STATE_INDICATE_TRIGGER_SOURCE:
          if (flags & (OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS) {
            flags &= ~(OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS;
            bluetooth_flag_clr(BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS, &err);

            if (demo.src_addr_ind == gatt_indication) {
              // start timer for source address indication confirmation
              sc = sl_sleeptimer_start_timer_ms(&app_single_timer,
                                                1000,
                                                app_single_timer_cb,
                                                (void *)NULL,
                                                0,
                                                0);
              sl_app_assert(sc == SL_STATUS_OK,
                            "[E: 0x%04x] Failed to start timer\n",
                            (int)sc);

              light_pend(&err);
              /* Send notification/indication data */
              sl_bt_gatt_server_send_characteristic_notification(ble_conn[0].handle,
                                                                 SOURCE_ADDRESS_GATTDB,
                                                                 (uint8_t)sizeof(demo.src_addr.addr),
                                                                 (uint8_t*)demo.src_addr.addr,
                                                                 &len);

              sl_app_assert(sc == SL_STATUS_OK,
                            "[E: 0x%04x] Failed to send_characteristic_notification\n",
                            (int)sc);

              light_post(&err);
            } else {
              bluetooth_flag_set(BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS, &err);
            }
            ble_state = BLE_STATE_INDICATE_SOURCE_ADDRESS;
          } else if (flags & (OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_INDICATE_TIMEOUT) {
            flags &= ~(OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_INDICATE_TIMEOUT;
            bluetooth_flag_clr(BLUETOOTH_EVENT_FLAG_APP_INDICATE_TIMEOUT, &err);
            demo_queue_post(DEMO_EVT_INDICATION_FAILED, &err);
            ble_state = BLE_STATE_READY;
          }
          break;

        case BLE_STATE_INDICATE_SOURCE_ADDRESS:
          if (flags & (OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS) {
            flags &= ~(OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS;
            bluetooth_flag_clr(BLUETOOTH_EVENT_FLAG_APP_INDICATE_SUCCESS, &err);
            ble_state = BLE_STATE_READY;
            demo_queue_post(DEMO_EVT_INDICATION_SUCCESSFUL, &err);
          } else if (flags & (OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_INDICATE_TIMEOUT) {
            flags &= ~(OS_FLAGS)BLUETOOTH_EVENT_FLAG_APP_INDICATE_TIMEOUT;
            bluetooth_flag_clr(BLUETOOTH_EVENT_FLAG_APP_INDICATE_TIMEOUT, &err);
            demo_queue_post(DEMO_EVT_INDICATION_FAILED, &err);
            ble_state = BLE_STATE_READY;
          }
          break;

        default:
          break;
      }
    }
  }
}

static void beacon_advertisements(void)
{
  static uint8_t *adv_data;
  static uint8_t adv_data_len;
  sl_status_t sc;
  int16_t ret_power;

  /* Set 0 dBm Transmit Power */
  sl_bt_system_set_max_tx_power(0, &ret_power);

  adv_data = (uint8_t *)&i_beacon_data;
  adv_data_len = sizeof(i_beacon_data);

  // Delete previous set
  sl_bt_advertiser_delete_set(advertising_set_handle1);
  advertising_set_handle1 = 0xFF;

  // Create an advertising set.
  sc = sl_bt_advertiser_create_set(&advertising_set_handle1);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to create advertising set\n",
                (int)sc);

  // Set the response data
  //sl_bt_system_set_ota_advertising_data(1, adv_data_len, (uint8_t*)&adv_data);
  sl_bt_advertiser_set_data(advertising_set_handle1, 0, adv_data_len, (uint8_t *)&adv_data);

  // Set nominal 100ms advertising interval, so we just get
  // a single beacon of each type
  sc = sl_bt_advertiser_set_timing(
    advertising_set_handle1, // advertising set handle
    160, // min. adv. interval (milliseconds * 1.6)
    160, // max. adv. interval (milliseconds * 1.6)
    0,   // adv. duration
    0);  // max. num. adv. events
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to set advertising timing\n",
                (int)sc);

  sl_bt_advertiser_set_configuration(advertising_set_handle1, 1);

  sc = sl_bt_advertiser_start(advertising_set_handle1,
                              advertiser_user_data, // discoverable mode
                              advertiser_non_connectable); // connectable mode
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to start advertising\n",
                (int)sc);

  //-------------------------------------------------
  adv_data = (uint8_t *)&eddystone_data;
  adv_data_len = sizeof(eddystone_data);

  // Delete previous set
  sl_bt_advertiser_delete_set(advertising_set_handle2);
  advertising_set_handle2 = 0xFF;

  // Create an advertising set.
  sc = sl_bt_advertiser_create_set(&advertising_set_handle2);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to create advertising set\n",
                (int)sc);

  // Set the response data
  //sl_bt_system_set_ota_advertising_data(1, adv_data_len, (uint8_t*)&adv_data);
  sl_bt_advertiser_set_data(advertising_set_handle2, 0, adv_data_len, (uint8_t *)&adv_data);

  // Set nominal 100ms advertising interval, so we just get
  // a single beacon of each type
  sc = sl_bt_advertiser_set_timing(
    advertising_set_handle2, // advertising set handle
    160, // min. adv. interval (milliseconds * 1.6)
    160, // max. adv. interval (milliseconds * 1.6)
    0,   // adv. duration
    0);  // max. num. adv. events
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to set advertising timing\n",
                (int)sc);

  sl_bt_advertiser_set_configuration(advertising_set_handle2, 1);

  sc = sl_bt_advertiser_start(advertising_set_handle2,
                              advertiser_user_data, // discoverable mode
                              advertiser_non_connectable); // connectable mode
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to start advertising\n",
                (int)sc);
}

static void enable_ble_advertisements(void)
{
  sl_status_t sc;
  // Create the device name based on the 16-bit device ID.
  uint16_t dev_id;
  static char dev_name[DEVNAME_LEN];
  char adv_name[13] = { 0 };
  dev_id = *((uint16_t *)demo.own_addr.addr);

  // Copy to the local GATT database - this will be used by the BLE stack
  // to put the local device name into the advertisements, but only if we are
  // using default advertisements
  snprintf(dev_name, DEVNAME_LEN, DEVNAME, dev_id >> 8, dev_id & 0xff);

  strcat(adv_name, dev_name);
  strcat(adv_name, " Light");

  sl_bt_gatt_server_write_attribute_value(gattdb_device_name,
                                          0,
                                          strlen(adv_name),
                                          (uint8_t *)adv_name);
  // Copy the shortened device name to the response data, overwriting
  // the default device name which is set at compile time
  Mem_Copy(((uint8_t *)&response_data) + 5, dev_name, 8);

  // Delete previous advertistment
  sl_bt_advertiser_delete_set(advertising_set_handle);
  advertising_set_handle = 0xFF;

  // Create an advertising set
  sc = sl_bt_advertiser_create_set(&advertising_set_handle);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to create advertising set\n",
                (int)sc);
  // Set the response data
  //sl_bt_system_set_ota_advertising_data(0, sizeof(response_data), (uint8_t*)&response_data);
  sl_bt_advertiser_set_data(advertising_set_handle, 0, sizeof(response_data), (uint8_t *)&response_data);

  // Set nominal 100ms advertising interval, so we just get
  // a single beacon of each type
  sc = sl_bt_advertiser_set_timing(
    advertising_set_handle, // advertising set handle
    160, // min. adv. interval (milliseconds * 1.6)
    160, // max. adv. interval (milliseconds * 1.6)
    0,   // adv. duration
    0);  // max. num. adv. events
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to set advertising timing\n",
                (int)sc);
  //gecko_cmd_le_gap_set_advertise_report_scan_request(0, 1);
  sl_bt_advertiser_set_report_scan_request(advertising_set_handle, 1);

  /* Start advertising in user mode and enable connections, if we are
   * not already connected */
  if (demo.conn_bluetooth_in_use) {
    // Start general advertising and enable connections.
    sc = sl_bt_advertiser_start(advertising_set_handle,
                                advertiser_user_data, // discoverable mode
                                advertiser_non_connectable); // connectable mode
    sl_app_assert(sc == SL_STATUS_OK,
                  "[E: 0x%04x] Failed to start advertising\n",
                  (int)sc);
  } else {
    // Start general advertising and enable connections.
    sc = sl_bt_advertiser_start(advertising_set_handle,
                                advertiser_user_data, // discoverable mode
                                advertiser_connectable_scannable); // connectable mode

    sl_app_assert(sc == SL_STATUS_OK,
                  "[E: 0x%04x] Failed to start advertising\n",
                  (int)sc);
  }
  beacon_advertisements();
}

/**************************************************************************//**
 * Demo task.
 * @param[in] p_arg the argument passed by 'OSTaskCreate()'.
 *****************************************************************************/
static void demo_app_task(void *p_arg)
{
  PP_UNUSED_PARAM(p_arg);
  RTOS_ERR err;
  OS_STATE demo_tmr_state;
  demo_msg_t demo_msg;
  bool bluetooth_ind = false;
  char *demo_mode_str_adv = "ADVERT";
  char *demo_mode_str_rep = "READY";
  char dev_id_str[DEV_ID_STR_LEN];
  snprintf(dev_id_str, DEV_ID_STR_LEN, "DMP:%04X", *((uint16_t *)demo.own_addr.addr));

  // create timer for periodic indications
  OSTmrCreate(&demo_timer,            /*   Pointer to user-allocated timer.     */
              "Demo Timer",          /*   Name used for debugging.             */
              0,                  /*     0 initial delay.                   */
              10,                  /*   100 Timer Ticks period.              */
              OS_OPT_TMR_PERIODIC,  /*   Timer is periodic.                   */
              &demo_timer_cb,    /*   Called when timer expires.           */
              DEF_NULL,             /*   No arguments to callback.            */
              &err);

  sl_app_assert(err.Code == RTOS_ERR_NONE,
                "[E: 0x%04x] OS Timer error\n",
                (int)err.Code);

  // create one-shot timer for direction array
  OSTmrCreate(&demo_timer_direction,    /*   Pointer to user-allocated timer.   */
              "Demo Timer Direction", /*   Name used for debugging.           */
              5,                      /*   50 Timer Ticks timeout.            */
              0,                      /*   Unused                             */
              OS_OPT_TMR_ONE_SHOT,    /*   Timer is one-shot.                 */
              &demo_timer_direction_cb,  /*   Called when timer expires.         */
              DEF_NULL,               /*   No arguments to callback.          */
              &err);

  sl_app_assert(err.Code == RTOS_ERR_NONE,
                "[E: 0x%04x] OS Timer error\n",
                (int)err.Code);

  demo.state = DEMO_STATE_INIT;

  demoUIInit();
  demoUIClearMainScreen((uint8_t *)"Light", true, true);
  demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_adv);
  demoUIDisplayId(DEMO_UI_PROTOCOL2, (uint8_t *)dev_id_str);

  while (DEF_TRUE) {
    // pending on demo message queue
    demo_msg = demo_queue_pend(&err);
    switch (demo.state) {
      case DEMO_STATE_INIT:
        switch (demo_msg) {
          // bluetooth booted
          case DEMO_EVT_BOOTED:
            // initialise demo variables:
            // make it look like the last trigger source of the light was a button press
            light_pend(&err);
            demo.conn_bluetooth_in_use = 0;
            demo.conn_proprietary_in_use = 0;
            demo.indication_ongoing = false;
            demo.direction = demo_light_direction_button;
            Mem_Copy((void*)demo.src_addr.addr,
                     (void*)demo.own_addr.addr,
                     sizeof(demo.src_addr.addr));
            light_post(&err);
            enable_ble_advertisements();
            // bluetooth client connected or proprietary link is used; start demo timer
            if (demo.conn_bluetooth_in_use || demo.conn_proprietary_in_use) {
              OSTmrStart(&demo_timer, &err);
              sl_app_assert(err.Code == RTOS_ERR_NONE,
                            "[E: 0x%04x] OS Timer error\n",
                            (int)err.Code);
            }
            demo.state = DEMO_STATE_READY;
            break;

          default:
            break;
        }
        break;

      case DEMO_STATE_READY:
        switch (demo_msg) {
          case DEMO_EVT_BLUETOOTH_CONNECTED:
            demo.indication_ongoing = false;
            if (!demo.conn_bluetooth_in_use) {
              demo.conn_bluetooth_in_use = 1;
            }
            // Bluetooth connected: start periodic indications if not already ongoing
            demo_tmr_state = OSTmrStateGet(&demo_timer, &err);

            if (OS_TMR_STATE_RUNNING != demo_tmr_state) {
              OSTmrStart(&demo_timer, &err);
              sl_app_assert(err.Code == RTOS_ERR_NONE,
                            "[E: 0x%04x] OS Timer error\n",
                            (int)err.Code);
            }
            demoUIDisplayProtocol(DEMO_UI_PROTOCOL2, true);
            break;

          case DEMO_EVT_BLUETOOTH_DISCONNECTED:
            // Flush pending and ongoing notifications
            demo.indication_pending = false;
            demo.indication_ongoing = false;
            // Clear indication flags
            demo.light_ind = gatt_disable;
            demo.direction_ind = gatt_disable;
            demo.src_addr_ind = gatt_disable;
            if (demo.conn_bluetooth_in_use) {
              demo.conn_bluetooth_in_use = 0;
            }
            demoUIClearDirection(DEMO_UI_DIRECTION_PROT2);
            demoUIDisplayProtocol(DEMO_UI_PROTOCOL2, false);
            // restart connectable advertising
            enable_ble_advertisements();
            // No Bluetooth nor RAIL client connected; stop periodic indications
            if (!demo.conn_bluetooth_in_use && !demo.conn_proprietary_in_use) {
              OSTmrStop(&demo_timer, OS_OPT_TMR_NONE, DEF_NULL, &err);
              sl_app_assert(err.Code == RTOS_ERR_NONE,
                            "[E: 0x%04x] OS Timer error\n",
                            (int)err.Code);
            }
            break;

          case DEMO_EVT_RAIL_READY:
            demo.conn_proprietary_in_use = 1;
            // RAIL linked: start periodic indications if not already ongoing
            demo_tmr_state = OSTmrStateGet(&demo_timer, &err);
            if (OS_TMR_STATE_RUNNING != demo_tmr_state) {
              OSTmrStart(&demo_timer, &err);
              sl_app_assert(err.Code == RTOS_ERR_NONE,
                            "[E: 0x%04x] OS Timer error\n",
                            (int)err.Code);
            }
            demoUIClearMainScreen((uint8_t *)"Light", true, true);
            demoUIClearDirection(DEMO_UI_DIRECTION_PROT1);
            demoUIDisplayLight(demo.light);
            demoUIDisplayProtocol(DEMO_UI_PROTOCOL1, true);
            demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_rep);
            demoUIDisplayId(DEMO_UI_PROTOCOL2, (uint8_t *)dev_id_str);
            break;

          case DEMO_EVT_RAIL_ADVERTISE:
            demo.conn_proprietary_in_use = 0;
            // No Bluetooth nor RAIL client connected; stop periodic indications
            if (!demo.conn_bluetooth_in_use && !demo.conn_proprietary_in_use) {
              OSTmrStop(&demo_timer, OS_OPT_TMR_NONE, DEF_NULL, &err);
              sl_app_assert(err.Code == RTOS_ERR_NONE,
                            "[E: 0x%04x] OS Timer error\n",
                            (int)err.Code);
            }

            demoUIClearMainScreen((uint8_t*)"Light", true, true);
            demoUIClearDirection(DEMO_UI_DIRECTION_PROT1);
            demoUIDisplayLight(demo.light);
            demoUIDisplayProtocol(DEMO_UI_PROTOCOL1, false);
            demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_adv);
            demoUIDisplayId(DEMO_UI_PROTOCOL2, (uint8_t *)dev_id_str);
            break;

          case DEMO_EVT_LIGHT_CHANGED_BLUETOOTH:

            if (demo_light_off == demo.light) {
              appUiLedOff();
            } else {
              appUiLedOn();
            }

            demoUIDisplayLight(demo.light);

            if (demo.conn_proprietary_in_use) {
              demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_rep);
            } else {
              demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_adv);
            }

            demoUIDisplayId(DEMO_UI_PROTOCOL2, (uint8_t *)dev_id_str);
            demoUIDisplayDirection(DEMO_UI_DIRECTION_PROT2);

            OSTmrStart(&demo_timer_direction, &err);
            sl_app_assert(err.Code == RTOS_ERR_NONE,
                          "[E: 0x%04x] OS Timer error\n",
                          (int)err.Code);

            demo_queue_post(DEMO_EVT_INDICATION, &err);
            break;

          case DEMO_EVT_LIGHT_CHANGED_RAIL:
            light_pend(&err);
            demo.direction = demo_light_direction_proprietary;

            if (demo_light_off == demo.light) {
              appUiLedOff();
            } else {
              appUiLedOn();
            }

            light_post(&err);
            demoUIDisplayLight(demo.light);
            if (demo.conn_proprietary_in_use) {
              demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_rep);
            } else {
              demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_adv);
            }
            demoUIDisplayId(DEMO_UI_PROTOCOL2, (uint8_t *)dev_id_str);
            demoUIDisplayDirection(DEMO_UI_DIRECTION_PROT1);

            OSTmrStart(&demo_timer_direction, &err);
            sl_app_assert(err.Code == RTOS_ERR_NONE,
                          "[E: 0x%04x] OS Timer error\n",
                          (int)err.Code);

            demo_queue_post(DEMO_EVT_INDICATION, &err);
            break;

          case DEMO_EVT_BUTTON0_PRESSED:
            light_pend(&err);

            if (demo_light_off == demo.light) {
              demo.light = demo_light_on;
              appUiLedOn();
            } else {
              demo.light = demo_light_off;
              appUiLedOff();
            }
            demo.direction = demo_light_direction_button;
            Mem_Copy((void *)demo.src_addr.addr,
                     (void *)demo.own_addr.addr,
                     sizeof(demo.src_addr.addr));
            light_post(&err);
            demoUIDisplayLight(demo.light);
            if (demo.conn_proprietary_in_use) {
              demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_rep);
            } else {
              demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t *)demo_mode_str_adv);
            }
            demoUIDisplayId(DEMO_UI_PROTOCOL2, (uint8_t *)dev_id_str);
            demo_queue_post(DEMO_EVT_INDICATION, &err);
            break;

          case DEMO_EVT_BUTTON1_PRESSED:
            proprietary_queue_post(PROP_TOGGLE_MODE, &err);
            break;

          case DEMO_EVT_CLEAR_DIRECTION:
            if (demo_light_direction_proprietary == demo.direction) {
              demoUIClearDirection(DEMO_UI_DIRECTION_PROT1);
            } else {
              demoUIClearDirection(DEMO_UI_DIRECTION_PROT2);
            }
            break;

          case DEMO_EVT_INDICATION:
            light_pend(&err);
            bluetooth_ind = demo.conn_bluetooth_in_use
                            && (demo.light_ind == gatt_indication
                                || demo.direction_ind == gatt_indication
                                || demo.src_addr_ind == gatt_indication);

            // no ongoing indication, free to start sending one out
            if (!demo.indication_ongoing) {
              if (bluetooth_ind || demo.conn_proprietary_in_use) {
                demo.indication_ongoing = true;
              }
              // send indication on BLE side
              if (bluetooth_ind) {
                bluetooth_flag_set(BLUETOOTH_EVENT_FLAG_APP_INDICATE_START, &err);
              }
              // send indication on proprietary side
              // there is no protocol on proprietary side; BLE side transmission is the slower
              // send out proprietary packets at maximum the same rate as BLE indications
              if (demo.conn_proprietary_in_use) {
                proprietary_queue_post(PROP_STATUS_SEND, &err);
                if (!bluetooth_ind) {
                  demo_queue_post(DEMO_EVT_INDICATION_SUCCESSFUL, &err);
                }
              }
              // ongoing indication; set pending flag
            } else {
              demo.indication_pending = true;
            }
            light_post(&err);
            break;

          case DEMO_EVT_INDICATION_SUCCESSFUL:
            demo.indication_ongoing = false;
            if (demo.indication_pending) {
              demo.indication_pending = false;
              demo_queue_post(DEMO_EVT_INDICATION, &err);
            }
            break;
          case DEMO_EVT_INDICATION_FAILED:
            break;
          default:
            break;
        }
        break;

      // error
      default:
        break;
    }
  }
}
