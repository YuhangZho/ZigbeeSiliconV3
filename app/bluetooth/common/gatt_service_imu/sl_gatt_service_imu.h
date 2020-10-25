/***************************************************************************//**
 * @file
 * @brief Inertial Measurement Unit GATT Service header
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

#ifndef SL_GATT_SERVICE_IMU_H
#define SL_GATT_SERVICE_IMU_H

#include "sl_bt_api.h"

/**************************************************************************//**
 * Bluetooth stack event handler.
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_gatt_service_imu_on_event(sl_bt_msg_t *evt);

/**************************************************************************//**
 * IMU GATT service event handler.
 *****************************************************************************/
void sl_gatt_service_imu_step(void);

/**************************************************************************//**
 * Getter for Orientation and Acceleration characteristic values.
 * @param[out] ovec Three dimensional orientation vector (in 0.01 degree).
 * @param[out] avec Three dimensional acceleration vector.
 * @return Status of the operation.
 * @note To be implemented in user code.
 *****************************************************************************/
sl_status_t sl_gatt_service_imu_get(int16_t ovec[3], int16_t avec[3]);

/**************************************************************************//**
 * Called when a calibration was requested by the GATT client.
 * @return Status of the operation.
 * @note To be implemented in user code.
 *****************************************************************************/
sl_status_t sl_gatt_service_imu_calibrate(void);

/**************************************************************************//**
 * Enable/disable IMU sensor.
 * @param[in] Enable (true) or disable (false).
 * @note To be implemented in user code.
 *****************************************************************************/
void sl_gatt_service_imu_enable(bool enable);

#endif // SL_GATT_SERVICE_IMU_H
