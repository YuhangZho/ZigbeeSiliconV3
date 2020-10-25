/***************************************************************************//**
 * @file
 * @brief Battery measurement GATT service header
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

#ifndef SL_GATT_SERVICE_BATTERY_H
#define SL_GATT_SERVICE_BATTERY_H

#include "sl_bt_api.h"

/**************************************************************************//**
 * Bluetooth stack event handler.
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_gatt_service_battery_on_event(sl_bt_msg_t *evt);

/**************************************************************************//**
 * Getter for Battery Level characteristic value.
 * @return Battery charge level percent (0..100).
 * @note To be implemented in user code.
 *****************************************************************************/
uint8_t sl_gatt_service_battery_get_level(void);

/**************************************************************************//**
 * Getter for Power Source characteristic value.
 * @return Power source type represented as an integer.
 * @note To be implemented in user code.
 *****************************************************************************/
uint8_t sl_gatt_service_battery_get_type(void);

#endif // SL_GATT_SERVICE_BATTERY_H
