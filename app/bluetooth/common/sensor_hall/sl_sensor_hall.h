/***************************************************************************//**
 * @file
 * @brief Hall sensor header
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

#ifndef SL_SENSOR_HALL_H
#define SL_SENSOR_HALL_H

#include <stdbool.h>
#include "sl_status.h"

/**************************************************************************//**
 * Initialize hall sensor.
 *****************************************************************************/
void sl_sensor_hall_init(void);

/**************************************************************************//**
 * Deinitialize hall sensor.
 *****************************************************************************/
void sl_sensor_hall_deinit(void);

/**************************************************************************//**
 * Getter for hall sensor measurement data.
 * @param[out] field_strength Field strength level (in mT).
 * @param[out] alert Field strength has reached the alert level.
 * @param[out] tamper Field strength has reached the tamper level.
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t sl_sensor_hall_get(float *field_strength, bool *alert, bool *tamper);

#endif // SL_SENSOR_HALL_H
