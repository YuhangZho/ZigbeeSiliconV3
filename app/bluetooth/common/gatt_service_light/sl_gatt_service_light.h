/***************************************************************************//**
 * @file
 * @brief Ambient Light and UV Index GATT Service header
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

#ifndef SL_GATT_SERVICE_LIGHT_H
#define SL_GATT_SERVICE_LIGHT_H

#include "sl_bt_api.h"

/**************************************************************************//**
 * Bluetooth stack event handler.
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_gatt_service_light_on_event(sl_bt_msg_t *evt);

/**************************************************************************//**
 * Getter for Ambient Light and UV Index characteristic values.
 * @param[out] lux Ambient light illuminance (in lux).
 * @param[out] uvi UV index.
 * @return Status of the operation.
 * @note To be implemented in user code.
 *****************************************************************************/
sl_status_t sl_gatt_service_light_get(float *lux, float *uvi);

#endif // SL_GATT_SERVICE_LIGHT_H
