/***************************************************************************//**
 * @file
 * @brief Air quality sensor header
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

#ifndef SL_SENSOR_GAS_H
#define SL_SENSOR_GAS_H

#include <stdint.h>
#include "sl_status.h"

/**************************************************************************//**
 * Initialize air quality sensor.
 *****************************************************************************/
void sl_sensor_gas_init(void);

/**************************************************************************//**
 * Deinitialize air quality sensor.
 *****************************************************************************/
void sl_sensor_gas_deinit(void);

/**************************************************************************//**
 * Getter for air quality sensor measurement data.
 * @param[out] eco2 Equivalent CO2 level (in ppm).
 * @param[out] tvoc Total Volatile Organic Compounds level (in ppb).
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t sl_sensor_gas_get(uint16_t *eco2, uint16_t *tvoc);

#endif // SL_SENSOR_GAS_H
