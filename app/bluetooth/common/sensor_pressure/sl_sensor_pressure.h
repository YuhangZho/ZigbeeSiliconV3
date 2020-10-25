/***************************************************************************//**
 * @file
 * @brief Air pressure sensor header
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

#ifndef SL_SENSOR_PRESSURE_H
#define SL_SENSOR_PRESSURE_H

#include "sl_status.h"

/**************************************************************************//**
 * Initialize pressure sensor.
 *****************************************************************************/
void sl_sensor_pressure_init(void);

/**************************************************************************//**
 * Deinitialize pressure sensor.
 *****************************************************************************/
void sl_pressure_deinit(void);

/**************************************************************************//**
 * Getter for pressure sensor measurement data.
 * @param[out] pressure Barometric pressure (in millibars).
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t sl_pressure_get(float *pressure);

#endif // SL_SENSOR_PRESSURE_H
