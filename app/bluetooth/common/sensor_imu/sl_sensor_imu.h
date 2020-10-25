/***************************************************************************//**
 * @file
 * @brief Inertial Measurement Unit sensor header
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

#ifndef SL_SENSOR_IMU_H
#define SL_SENSOR_IMU_H

#include <stdbool.h>
#include "sl_status.h"

/**************************************************************************//**
 * Initialize IMU sensor.
 *****************************************************************************/
void sl_sensor_imu_init(void);

/**************************************************************************//**
 * Deinitialize IMU sensor.
 *****************************************************************************/
void sl_sensor_imu_deinit(void);

/**************************************************************************//**
 * Enable/disable IMU sensor.
 * @param[in] Enable (true) or disable (false).
 *****************************************************************************/
void sl_sensor_imu_enable(bool enable);

/**************************************************************************//**
 * Getter for orientation and acceleration sensor measurement data.
 * @param[out] ovec Three dimensional orientation vector (in 0.01 degree).
 * @param[out] avec Three dimensional acceleration vector.
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t sl_sensor_imu_get(int16_t ovec[3], int16_t avec[3]);

/**************************************************************************//**
 * Perform IMU sensor calibration.
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t sl_sensor_imu_calibrate(void);

#endif // SL_SENSOR_IMU_H
