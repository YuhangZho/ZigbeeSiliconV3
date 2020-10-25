/***************************************************************************//**
 * @file
 * @brief Ambient light and UV index sensor
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
#include "sl_si1133.h"
#include "sl_sensor_select.h"
#include "sl_app_assert.h"
#include "sl_sensor_light.h"

// -----------------------------------------------------------------------------
// Public function definitions

void sl_sensor_light_init(void)
{
  sl_status_t sc;
  sl_i2cspm_t *light_sensor = sl_sensor_select(SL_BOARD_SENSOR_LIGHT);
  sc = sl_board_enable_sensor(SL_BOARD_SENSOR_LIGHT);
  sl_app_assert((SL_STATUS_OK == sc) && (NULL != light_sensor),
                "[E: %#04x] Si1133 sensor not available\n", sc);
  sc = sl_si1133_init(light_sensor);
  sl_app_assert(SL_STATUS_OK == sc, "[E: %#04x] Si1133 sensor init failed\n", sc);
}

void sl_sensor_light_deinit(void)
{
  (void)sl_board_disable_sensor(SL_BOARD_SENSOR_LIGHT);
}

sl_status_t sl_sensor_light_get(float *lux, float *uvi)
{
  sl_status_t sc;
  sl_i2cspm_t *light_sensor = sl_sensor_select(SL_BOARD_SENSOR_LIGHT);
  sc = sl_si1133_measure_lux_uvi(light_sensor, lux, uvi);
  return sc;
}
