/***************************************************************************//**
 * @file
 * @brief Relative Humidity and Temperature sensor
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
#include "sl_si70xx.h"
#include "sl_sensor_select.h"
#include "sl_app_assert.h"
#include "sl_sensor_rht.h"

// -----------------------------------------------------------------------------
// Configuration

#define RHT_ADDRESS  SI7021_ADDR

// -----------------------------------------------------------------------------
// Public function definitions

void sl_sensor_rht_init(void)
{
  sl_status_t sc;
  sl_i2cspm_t *rht_sensor = sl_sensor_select(SL_BOARD_SENSOR_RHT);
  (void)sl_board_enable_sensor(SL_BOARD_SENSOR_RHT);
  sl_app_assert(NULL != rht_sensor, "Si70xx sensor not available\n");
  sc = sl_si70xx_init(rht_sensor, RHT_ADDRESS);
  sl_app_assert(SL_STATUS_OK == sc, "[E: %#04x] Si70xx sensor init failed\n", sc);
}

void sl_sensor_rht_deinit(void)
{
  (void)sl_board_disable_sensor(SL_BOARD_SENSOR_RHT);
}

sl_status_t sl_sensor_rht_get(uint32_t *rh, int32_t *t)
{
  sl_status_t sc;
  sl_i2cspm_t *rht_sensor = sl_sensor_select(SL_BOARD_SENSOR_RHT);
  sc = sl_si70xx_measure_rh_and_temp(rht_sensor, RHT_ADDRESS, rh, t);
  return sc;
}
