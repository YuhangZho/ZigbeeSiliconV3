/***************************************************************************//**
 * @file
 * @brief Air Quality GATT Service header
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

#ifndef SL_GATT_SERVICE_GAS_H
#define SL_GATT_SERVICE_GAS_H

#include "sl_bt_api.h"

/**************************************************************************//**
 * Bluetooth stack event handler.
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_gatt_service_gas_on_event(sl_bt_msg_t *evt);

/**************************************************************************//**
 * Getter for ECO2 and TVOC characteristic values.
 * @param[out] eco2 Equivalent CO2 level (in ppm).
 * @param[out] tvoc Total Volatile Organic Compounds level (in ppb).
 * @return Status of the operation.
 * @note To be implemented in user code.
 *****************************************************************************/
sl_status_t sl_gatt_service_gas_get(uint16_t *eco2, uint16_t *tvoc);

#endif // SL_GATT_SERVICE_GAS_H
