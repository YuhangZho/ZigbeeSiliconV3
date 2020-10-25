/***************************************************************************//**
 * @file
 * @brief RGB LED GATT Service header
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

#ifndef SL_GATT_SERVICE_RGB_H
#define SL_GATT_SERVICE_RGB_H

#include "sl_bt_api.h"

/**************************************************************************//**
 * Bluetooth stack event handler.
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_gatt_service_rgb_on_event(sl_bt_msg_t* evt);

/**************************************************************************//**
 * Setter for RGB Leds characteristic value.
 * @param[in] m LED bitmask.
 * @param[in] r Red intensity.
 * @param[in] g Green intensity.
 * @param[in] b Blue intensity.
 * @note To be implemented in user code.
 *****************************************************************************/
void sl_gatt_service_rgb_set_led(uint8_t m, uint8_t r, uint8_t g, uint8_t b);

#endif // SL_GATT_SERVICE_RGB_H
