/***************************************************************************//**
 * @file
 * @brief Ambient light sensor header
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

#ifndef SL_SENSOR_LUX_H
#define SL_SENSOR_LUX_H

#include "sl_status.h"

/**************************************************************************//**
 * Initialize ambient light sensor.
 *****************************************************************************/
void sl_sensor_lux_init(void);

/**************************************************************************//**
 * Deinitialize ambient light sensor.
 *****************************************************************************/
void sl_sensor_lux_deinit(void);

/**************************************************************************//**
 * Getter for ambient light sensor measurement data.
 * @param[out] lux Ambient light illuminance (in lux).
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t sl_sensor_lux_get(float *lux);

#endif // SL_SENSOR_LUX_H
