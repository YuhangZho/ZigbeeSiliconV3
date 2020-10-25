/***************************************************************************//**
 * @file
 * @brief Inertial Measurement Unit sensor
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

#include "sl_board_control.h"
#include "sl_imu.h"
#include "sl_app_assert.h"
#include "sl_sensor_imu.h"

// -----------------------------------------------------------------------------
// Configuration

#define IMU_SAMPLE_RATE    5.0f /* Hz */

// -----------------------------------------------------------------------------
// Public function definitions

void sl_sensor_imu_init(void)
{
  (void)sl_board_enable_sensor(SL_BOARD_SENSOR_IMU);
}

void sl_sensor_imu_deinit(void)
{
  (void)sl_board_disable_sensor(SL_BOARD_SENSOR_IMU);
}

void sl_sensor_imu_enable(bool enable)
{
  sl_status_t sc;
  uint8_t state = sl_imu_get_state();
  if (enable && (IMU_STATE_DISABLED == state)) {
    sc = sl_imu_init();
    sl_app_assert(sc == SL_STATUS_OK,
                  "[E: 0x%04x] IMU init failed\n",
                  (int)sc);
    sl_imu_configure(IMU_SAMPLE_RATE);
  } else if (!enable && (IMU_STATE_READY == state)) {
    sl_imu_deinit();
  }
}

sl_status_t sl_sensor_imu_get(int16_t ovec[3], int16_t avec[3])
{
  sl_status_t sc = SL_STATUS_NOT_READY;
  if (sl_imu_is_data_ready()) {
    sl_imu_update();
    sl_imu_get_orientation(ovec);
    sl_imu_get_acceleration(avec);
    sc = SL_STATUS_OK;
  }
  return sc;
}

sl_status_t sl_sensor_imu_calibrate(void)
{
  return sl_imu_calibrate_gyro();
}
