/***************************************************************************//**
 * @file
 * @brief Air quality sensor
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
#include "sl_ccs811.h"
#include "sl_sensor_select.h"
#include "sl_app_assert.h"
#include "sl_sensor_gas.h"

// -----------------------------------------------------------------------------
// Configuration

#define GAS_MEASURE_MODE  CCS811_MEASURE_MODE_DRIVE_MODE_10SEC

// -----------------------------------------------------------------------------
// Public function definitions

void sl_sensor_gas_init(void)
{
  sl_status_t sc;
  sl_i2cspm_t *gas_sensor = sl_sensor_select(SL_BOARD_SENSOR_GAS);
  sc = sl_board_enable_sensor(SL_BOARD_SENSOR_GAS);
  sl_app_assert((SL_STATUS_OK == sc) && (NULL != gas_sensor),
                "[E: %#04x] Gas sensor not available\n", sc);
  sc = sl_ccs811_init(gas_sensor);
  if ( sc == SL_STATUS_OK ) {
    sc = sl_ccs811_set_measure_mode(gas_sensor, GAS_MEASURE_MODE);
  }
  sl_app_assert(SL_STATUS_OK == sc, "[E: %#04x] Gas sensor init failed\n", sc);
}

void sl_sensor_gas_deinit(void)
{
  (void)sl_board_disable_sensor(SL_BOARD_SENSOR_GAS);
}

sl_status_t sl_sensor_gas_get(uint16_t *eco2, uint16_t *tvoc)
{
  sl_status_t sc = SL_STATUS_NOT_READY;
  sl_i2cspm_t *gas_sensor = sl_sensor_select(SL_BOARD_SENSOR_GAS);
  if ( sl_ccs811_is_data_available(gas_sensor) ) {
    sc = sl_ccs811_get_measurement(gas_sensor, eco2, tvoc);
  }
  return sc;
}
