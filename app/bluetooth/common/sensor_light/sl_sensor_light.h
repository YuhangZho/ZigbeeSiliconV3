/***************************************************************************//**
 * @file
 * @brief Ambient light and UV index sensor header
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

#ifndef SL_SENSOR_LIGHT_H
#define SL_SENSOR_LIGHT_H

#include "sl_status.h"

/**************************************************************************//**
 * Initialize ambient light and UV index sensor.
 *****************************************************************************/
void sl_sensor_light_init(void);

/**************************************************************************//**
 * Deinitialize ambient light and UV index sensor.
 *****************************************************************************/
void sl_sensor_light_deinit(void);

/**************************************************************************//**
 * Getter for ambient light and UV index sensor measurement data.
 * @param[out] lux Ambient light illuminance (in lux).
 * @param[out] uvi UV index.
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t sl_sensor_light_get(float *lux, float *uvi);

#endif // SL_SENSOR_LIGHT_H
