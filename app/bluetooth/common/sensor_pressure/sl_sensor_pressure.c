/***************************************************************************//**
 * @file
 * @brief Air pressure sensor
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

#include <stddef.h>
#include "sl_board_control.h"
#include "sl_bmp280.h"
#include "sl_sensor_select.h"
#include "sl_app_assert.h"
#include "sl_sensor_pressure.h"

// -----------------------------------------------------------------------------
// Public function definitions

void sl_sensor_pressure_init(void)
{
  sl_status_t sc;
  sl_i2cspm_t *pressure_sensor = sl_sensor_select(SL_BOARD_SENSOR_PRESSURE);
  sc = sl_board_enable_sensor(SL_BOARD_SENSOR_RHT);
  sl_app_assert((SL_STATUS_OK == sc) && (NULL != pressure_sensor),
                "[E: %#04x] Pressure sensor not available\n", sc);
  sc = sl_bmp280_init(pressure_sensor);
  sl_app_assert(SL_STATUS_OK == sc, "[E: %#04x] Pressure sensor init failed\n", sc);
}

void sl_pressure_deinit(void)
{
  (void)sl_board_disable_sensor(SL_BOARD_SENSOR_PRESSURE);
}

sl_status_t sl_pressure_get(float *pressure)
{
  sl_status_t sc;
  sl_i2cspm_t *pressure_sensor = sl_sensor_select(SL_BOARD_SENSOR_PRESSURE);
  sc = sl_bmp280_measure_pressure(pressure_sensor, pressure);
  return sc;
}
