/***************************************************************************//**
 * @file
 * @brief Relative Humidity and Temperature sensor header
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

#ifndef SL_SENSOR_RHT_H
#define SL_SENSOR_RHT_H

#include <stdint.h>
#include "sl_status.h"

/**************************************************************************//**
 * Initialize Relative Humidity and Temperature sensor.
 *****************************************************************************/
void sl_sensor_rht_init(void);

/**************************************************************************//**
 * Deinitialize Relative Humidity and Temperature sensor.
 *****************************************************************************/
void sl_sensor_rht_deinit(void);

/**************************************************************************//**
 * Getter for Relative Humidity and Temperature sensor measurement data.
 * @param[out] rh Relative humidity (in 0.001 percent).
 * @param[out] t Temperature (in 0.001 Celsius).
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t sl_sensor_rht_get(uint32_t *rh, int32_t *t);

#endif // SL_SENSOR_RHT_H
