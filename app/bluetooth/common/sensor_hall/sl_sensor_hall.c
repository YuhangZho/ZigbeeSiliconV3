/***************************************************************************//**
 * @file
 * @brief Hall sensor
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

#include <math.h>
#include "sl_board_control.h"
#include "sl_si7210.h"
#include "sl_app_assert.h"
#include "sl_sensor_select.h"
#include "sl_sensor_hall.h"

// -----------------------------------------------------------------------------
// Configuration

#define HALL_THRESHOLD    3.0f  /* mT */
#define HALL_HYSTERESIS   0.5f  /* mT */
#define HALL_POLARITY     0x00  /* Omnipolar field polarity */
#define HALL_SCALE        20000 /* uT */

// -----------------------------------------------------------------------------
// Public function definitions

void sl_sensor_hall_init(void)
{
  sl_status_t sc;
  sl_si7210_configure_t hall_config = {
    .threshold = HALL_THRESHOLD,
    .hysteresis = HALL_HYSTERESIS,
    .polarity = HALL_POLARITY,
    .output_invert = false
  };
  sl_i2cspm_t *hall_sensor = sl_sensor_select(SL_BOARD_SENSOR_HALL);
  sc = sl_board_enable_sensor(SL_BOARD_SENSOR_HALL);
  sl_app_assert((SL_STATUS_OK == sc) && (NULL != hall_sensor),
                "[E: %#04x] Si7210 sensor not available\n", sc);
  sc = sl_si7210_init(hall_sensor);
  sl_app_assert(SL_STATUS_OK == sc, "[E: %#04x] Si7210 sensor init failed\n", sc);
  sc = sl_si7210_configure(hall_sensor, &hall_config);
  sl_app_assert(SL_STATUS_OK == sc, "[E: %#04x] Si7210 sensor config failed\n", sc);
}

void sl_sensor_hall_deinit(void)
{
  (void)sl_board_disable_sensor(SL_BOARD_SENSOR_HALL);
}

sl_status_t sl_sensor_hall_get(float *field_strength, bool *alert, bool *tamper)
{
  sl_status_t sc;
  float fs_abs;
  // store previous alert state to implement hysteresis
  static bool alert_local = false;
  // measure field strength
  sl_i2cspm_t *hall_sensor = sl_sensor_select(SL_BOARD_SENSOR_HALL);
  sc = sl_si7210_measure(hall_sensor, HALL_SCALE, field_strength);
  if (SL_STATUS_OK != sc) {
    // measurement failed
    return sc;
  }
  // get absolute value for threshold comparisons (because of omnipolar config)
  fs_abs = fabsf(*field_strength);
  // check alert threshold with hysteresis
  if (fs_abs < (HALL_THRESHOLD - HALL_HYSTERESIS)) {
    alert_local = false;
  } else if (fs_abs > (HALL_THRESHOLD + HALL_HYSTERESIS)) {
    alert_local = true;
  }
  *alert = alert_local;
  // check tamper threshold
  if (fs_abs > sl_si7210_get_tamper_threshold()) {
    *tamper = true;
  } else {
    *tamper = false;
  }
  return sc;
}
