/***************************************************************************//**
 * @file
 * @brief Health Thermometer GATT service
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

#ifndef SL_HEALTH_THERMOMETER_H
#define SL_HEALTH_THERMOMETER_H

#include <stddef.h>
#include <stdint.h>
#include "sl_health_thermometer_config.h"

/// Possible values of the Temperature Type characteristic (UUID: 2A1D)
enum sl_bt_ht_temperature_type {
  SL_BT_HT_TEMPERATURE_TYPE_ARMPIT = 1,
  SL_BT_HT_TEMPERATURE_TYPE_BODY,
  SL_BT_HT_TEMPERATURE_TYPE_EAR,
  SL_BT_HT_TEMPERATURE_TYPE_FINGER,
  SL_BT_HT_TEMPERATURE_TYPE_GASTRO_INTESTINAL_TRACT,
  SL_BT_HT_TEMPERATURE_TYPE_MOUTH,
  SL_BT_HT_TEMPERATURE_TYPE_RECTUM,
  SL_BT_HT_TEMPERATURE_TYPE_TOE,
  SL_BT_HT_TEMPERATURE_TYPE_TYMPANUM,
};

/**************************************************************************//**
 * Callback to handle connection closed event.
 * @param[in] reason Result code.
 * @param[in] connection Handle of the closed connection
 * @note To be implemented in user code.
 *****************************************************************************/
void sl_bt_connection_closed_cb(uint16_t reason, uint8_t connection);

/**************************************************************************//**
 * Send Temperature Measurement characteristic indication to the client.
 * @param[in] connection Connection handle of the client.
 * @param[in] value Temperature value in millidegree.
 * @param[in] fahrenheit Value is given in Fahrenheit (true) or Celsius (false).
 *****************************************************************************/
sl_status_t sl_bt_ht_temperature_measurement_indicate(uint8_t connection,
                                                      int32_t value,
                                                      bool fahrenheit);

/**************************************************************************//**
 * Temperature Measurement characteristic's CCCD has changed.
 * @param[in] connection Connection handle of the client.
 * @param[in] client_config Characteristic Client Configuration Flag.
 * @note To be implemented in user code.
 *****************************************************************************/
void sl_bt_ht_temperature_measurement_indication_changed_cb(uint8_t connection,
                                                            gatt_client_config_flag_t client_config);

/**************************************************************************//**
 * Temperature Measurement characteristic indication confirmed.
 * @param[in] connection Connection handle of the client.
 * @note To be implemented in user code.
 *****************************************************************************/
void sl_bt_ht_temperature_measurement_indication_confirmed_cb(uint8_t connection);

/**************************************************************************//**
 * Bluetooth stack event handler.
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_ht_on_event(sl_bt_msg_t *evt);

#endif // SL_HEALTH_THERMOMETER_H
