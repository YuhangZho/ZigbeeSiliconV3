/***************************************************************************//**
 * @file
 * @brief Inertial Measurement Unit GATT Service
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
#include "sl_app_log.h"
#include "sl_gatt_service_imu.h"

// -----------------------------------------------------------------------------
// Private macros

#define IMU_CP_RESPONSE                     0x10
#define IMU_CP_OPCODE_CALIBRATE             0x01
#define IMU_CP_OPCODE_ORIRESET              0x02
#define IMU_CP_OPCODE_CALRESET              0x64
#define IMU_CP_RESP_SUCCESS                 0x01
#define IMU_CP_RESP_ERROR                   0x02
// Client Characteristic Configuration descriptor improperly configured
#define IMU_CP_ERR_CCCD_CONF                0x81

// -----------------------------------------------------------------------------
// Private variables

static uint8_t imu_connection = 0;
static bool imu_state = false; /* disabled / enabled */
static bool imu_acceleration_notification = false;
static bool imu_orientation_notification = false;
static bool imu_control_pont_indication = false;
static int16_t imu_avec[3] = { 0, 0, 0 };
static int16_t imu_ovec[3] = { 0, 0, 0 };
static uint8_t imu_cp_opcode;
static uint8_t imu_cp_status;
static enum {
  IMU_CP_IND_IDLE,
  IMU_CP_IND_PENDING,
  IMU_CP_IND_WAITING
} imu_cp_indication_status = IMU_CP_IND_IDLE;

// -----------------------------------------------------------------------------
// Private function declarations

static void imu_update_state(void);
static void imu_acceleration_notify(void);
static void imu_orientation_notify(void);
static void imu_control_point_indicate(void);
static void imu_connection_closed_cb(sl_bt_evt_connection_closed_t *data);
static void imu_char_config_changed_cb(sl_bt_evt_gatt_server_characteristic_status_t *data);
static void imu_char_write_cb(sl_bt_evt_gatt_server_user_write_request_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

static void imu_update_state(void)
{
  bool imu_state_old = imu_state;
  imu_state = imu_acceleration_notification || imu_orientation_notification;
  if (imu_state_old != imu_state) {
    sl_gatt_service_imu_enable(imu_state);
  }
}

static void imu_acceleration_notify(void)
{
  sl_status_t sc;
  sc = sl_bt_gatt_server_send_characteristic_notification(
    imu_connection,
    gattdb_imu_acceleration,
    sizeof(imu_avec),
    (uint8_t*)imu_avec,
    NULL);
  if (sc != SL_STATUS_OK) {
    sl_app_log("[E: 0x%04x] Failed to send characteristic notification\n", (int)sc);
  }
}

static void imu_orientation_notify(void)
{
  sl_status_t sc;
  sc = sl_bt_gatt_server_send_characteristic_notification(
    imu_connection,
    gattdb_imu_orientation,
    sizeof(imu_ovec),
    (uint8_t*)imu_ovec,
    NULL);
  if (sc != SL_STATUS_OK) {
    sl_app_log("[E: 0x%04x] Failed to send characteristic notification\n", (int)sc);
  }
}

static void imu_control_point_indicate(void)
{
  sl_status_t sc;
  uint8_t response[3] = { IMU_CP_RESPONSE, imu_cp_opcode, imu_cp_status };

  switch (imu_cp_indication_status) {
    case IMU_CP_IND_IDLE:
      sc = sl_bt_gatt_server_send_characteristic_notification(
        imu_connection,
        gattdb_imu_control_point,
        sizeof(response),
        response,
        NULL);
      if (sc != SL_STATUS_OK) {
        sl_app_log("[E: 0x%04x] Failed to send characteristic indication\n", (int)sc);
      } else {
        imu_cp_indication_status = IMU_CP_IND_PENDING;
      }
      break;
    case IMU_CP_IND_PENDING:
      // an indication is already pending, send it later
      imu_cp_indication_status = IMU_CP_IND_WAITING;
      break;
    case IMU_CP_IND_WAITING:
      sl_app_log("Unable to queue characteristic indication\n");
      break;
  }
}

static void imu_connection_closed_cb(sl_bt_evt_connection_closed_t *data)
{
  (void)data;
  imu_acceleration_notification = false;
  imu_orientation_notification = false;
  imu_control_pont_indication = false;
  imu_cp_indication_status = IMU_CP_IND_IDLE;
  imu_update_state();
}

static void imu_char_config_changed_cb(sl_bt_evt_gatt_server_characteristic_status_t *data)
{
  bool enable = gatt_disable != data->client_config_flags;
  imu_connection = data->connection;

  // update notification status
  switch (data->characteristic) {
    case gattdb_imu_acceleration:
      imu_acceleration_notification = enable;
      break;
    case gattdb_imu_orientation:
      imu_orientation_notification = enable;
      break;
    case gattdb_imu_control_point:
      imu_control_pont_indication = enable;
      break;
    default:
      sl_app_assert(false, "Unexpected characteristic\n");
      break;
  }
  imu_update_state();
}

static void imu_char_write_cb(sl_bt_evt_gatt_server_user_write_request_t *data)
{
  sl_status_t sc;
  uint8_t att_errorcode = 0;

  if (data->value.len == 1) {
    if (imu_control_pont_indication) {
      imu_cp_opcode = data->value.data[0];
      imu_cp_status = IMU_CP_RESP_ERROR;

      switch (imu_cp_opcode) {
        case IMU_CP_OPCODE_CALIBRATE:
          if (SL_STATUS_OK == sl_gatt_service_imu_calibrate()) {
            imu_cp_status = IMU_CP_RESP_SUCCESS;
          }
          break;
        case IMU_CP_OPCODE_ORIRESET:
          SL_FALLTHROUGH
        case IMU_CP_OPCODE_CALRESET:
          /* Do nothing, keep for compatibility reasons */
          imu_cp_status = IMU_CP_RESP_SUCCESS;
          break;
      }
    } else {
      att_errorcode = IMU_CP_ERR_CCCD_CONF;
    }
  } else {
    att_errorcode = 0x0D; // Invalid Attribute Value Length
  }

  sc = sl_bt_gatt_server_send_user_write_response(data->connection,
                                                  data->characteristic,
                                                  att_errorcode);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to send user write response\n",
                (int)sc);

  if (0 == att_errorcode) {
    imu_control_point_indicate();
  }
}

// -----------------------------------------------------------------------------
// Public function definitions

void sl_gatt_service_imu_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_connection_closed_id:
      imu_connection_closed_cb(&evt->data.evt_connection_closed);
      break;

    case sl_bt_evt_gatt_server_characteristic_status_id:
      if ((gatt_server_client_config == (gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags)
          && ((gattdb_imu_acceleration == evt->data.evt_gatt_server_user_read_request.characteristic)
              || (gattdb_imu_orientation == evt->data.evt_gatt_server_user_read_request.characteristic)
              || (gattdb_imu_control_point == evt->data.evt_gatt_server_user_read_request.characteristic))) {
        // client characteristic configuration changed by remote GATT client
        imu_char_config_changed_cb(&evt->data.evt_gatt_server_characteristic_status);
      } else if ((gatt_server_confirmation == (gatt_server_characteristic_status_flag_t)evt->data.evt_gatt_server_characteristic_status.status_flags)
                 && (gattdb_imu_control_point == evt->data.evt_gatt_server_user_read_request.characteristic)) {
        // confirmation of indication received from remove GATT client
        if (IMU_CP_IND_WAITING == imu_cp_indication_status) {
          imu_cp_indication_status = IMU_CP_IND_IDLE;
          imu_control_point_indicate();
        }
        // reset indication status
        imu_cp_indication_status = IMU_CP_IND_IDLE;
      }
      break;

    case sl_bt_evt_gatt_server_user_write_request_id:
      if (gattdb_imu_control_point == evt->data.evt_gatt_server_user_write_request.characteristic) {
        imu_char_write_cb(&evt->data.evt_gatt_server_user_write_request);
      }
      break;
  }
}

void sl_gatt_service_imu_step(void)
{
  if (imu_state) {
    if (SL_STATUS_OK == sl_gatt_service_imu_get(imu_ovec, imu_avec)) {
      if (imu_acceleration_notification) {
        imu_acceleration_notify();
      }
      if (imu_orientation_notification) {
        imu_orientation_notify();
      }
    }
  }
}

SL_WEAK sl_status_t sl_gatt_service_imu_get(int16_t ovec[3], int16_t avec[3])
{
  (void)ovec;
  (void)avec;
  return SL_STATUS_FAIL;
}

SL_WEAK sl_status_t sl_gatt_service_imu_calibrate(void)
{
  return SL_STATUS_OK;
}

SL_WEAK void sl_gatt_service_imu_enable(bool enable)
{
  (void)enable;
}
