/***************************************************************************//**
 * @file
 * @brief Sound Level GATT Service header
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

#ifndef SL_GATT_SERVICE_SOUND_H
#define SL_GATT_SERVICE_SOUND_H

#include "sl_bt_api.h"

/**************************************************************************//**
 * Bluetooth stack event handler.
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_gatt_service_sound_on_event(sl_bt_msg_t *evt);

/**************************************************************************//**
 * Getter for Sound Level characteristic value.
 * @param[out] sound_level Sound pressure level (in dB).
 * @return Status of the operation.
 * @note To be implemented in user code.
 *****************************************************************************/
sl_status_t sl_gatt_service_sound_get(float *sound_level);

#endif // SL_GATT_SERVICE_SOUND_H
