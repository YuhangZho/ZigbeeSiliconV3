/***************************************************************************//**
 * @file
 * @brief Air Quality GATT Service
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
#include "sl_gatt_service_gas.h"

// -----------------------------------------------------------------------------
// Private variables

static uint16_t gas_eco2 = 0;
static uint16_t gas_tvoc = 0;

// -----------------------------------------------------------------------------
// Private function declarations

static void gas_update(void);
static void gas_eco2_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);
static void gas_tvoc_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

static void gas_update(void)
{
  uint16_t eco2;
  uint16_t tvoc;
  // keep previous data if measurement fails
  if (SL_STATUS_OK == sl_gatt_service_gas_get(&eco2, &tvoc)) {
    gas_eco2 = eco2;
    gas_tvoc = tvoc;
  }
}

static void gas_eco2_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;

  // update measurement data
  gas_update();

  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(gas_eco2),
    (uint8_t*)&gas_eco2,
    NULL);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to send user read response\n",
                (int)sc);
}

static void gas_tvoc_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;

  // update measurement data
  gas_update();

  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(gas_tvoc),
    (uint8_t*)&gas_tvoc,
    NULL);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to send user read response\n",
                (int)sc);
}

// -----------------------------------------------------------------------------
// Public function definitions

void sl_gatt_service_gas_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  if (sl_bt_evt_gatt_server_user_read_request_id == SL_BT_MSG_ID(evt->header)) {
    if (gattdb_iaq_eco2 == evt->data.evt_gatt_server_user_read_request.characteristic) {
      gas_eco2_read_cb(&evt->data.evt_gatt_server_user_read_request);
    } else if (gattdb_iaq_tvoc == evt->data.evt_gatt_server_user_read_request.characteristic) {
      gas_tvoc_read_cb(&evt->data.evt_gatt_server_user_read_request);
    }
  }
}

SL_WEAK sl_status_t sl_gatt_service_gas_get(uint16_t *eco2, uint16_t *tvoc)
{
  (void)eco2;
  (void)tvoc;
  // keep default values
  return SL_STATUS_FAIL;
}
