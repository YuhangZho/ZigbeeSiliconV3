/***************************************************************************//**
 * @file
 * @brief Sound Level GATT Service
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
#include "sl_gatt_service_sound.h"

// -----------------------------------------------------------------------------
// Private variables

static int16_t sound_level = 0;

// -----------------------------------------------------------------------------
// Private function declarations

static void sound_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data);

// -----------------------------------------------------------------------------
// Private function definitions

static void sound_read_cb(sl_bt_evt_gatt_server_user_read_request_t *data)
{
  sl_status_t sc;
  float sl;

  // keep previous data if measurement fails
  if (SL_STATUS_OK == sl_gatt_service_sound_get(&sl)) {
    /* Sound level from sensor represented in dB, characteristic represented in 0.01 dB */
    sound_level = (int16_t)(sl * 100.0f);
  }

  sc = sl_bt_gatt_server_send_user_read_response(
    data->connection,
    data->characteristic,
    0,
    sizeof(sound_level),
    (uint8_t*)&sound_level,
    NULL);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to send user read response\n",
                (int)sc);
}

// -----------------------------------------------------------------------------
// Public function definitions

void sl_gatt_service_sound_on_event(sl_bt_msg_t *evt)
{
  // Handle stack events
  if (sl_bt_evt_gatt_server_user_read_request_id == SL_BT_MSG_ID(evt->header)) {
    if (gattdb_es_sound_level == evt->data.evt_gatt_server_user_read_request.characteristic) {
      sound_read_cb(&evt->data.evt_gatt_server_user_read_request);
    }
  }
}

SL_WEAK sl_status_t sl_gatt_service_sound_get(float *sound_level)
{
  (void)sound_level;
  // keep default value
  return SL_STATUS_FAIL;
}
