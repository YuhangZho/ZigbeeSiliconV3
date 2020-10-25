/***************************************************************************//**
 * @file
 * @brief I2C sensor select utility for BRD4166
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

#ifndef SL_SENSOR_SELECT_H
#define SL_SENSOR_SELECT_H

#include "sl_i2cspm.h"
#include "sl_board_control.h"

/**************************************************************************//**
 * Select I2C peripheral for a given sensor.
 * @param[in] sensor Sensor to be selected.
 * @return Pointer to the I2C peripheral. NULL if sensor is not available.
 *****************************************************************************/
sl_i2cspm_t* sl_sensor_select(sl_board_sensor_t sensor);

#endif // SL_SENSOR_SELECT_H
