/***************************************************************************//**
 * @file
 * @brief I2C sensor select utility for BRD4166A
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
#include "sl_sensor_select.h"
#include "sl_board_control_config.h"
#include "sl_i2cspm_instances.h"

#if defined(BRD4166A_SUPPORT)
// More than one I2C instance present, we need to switch between them.
#include "brd4166a_support.h"
#else
// Only one I2C instance present, let's use this dummy macro.
#define sl_thunderboard_require_i2c(x)
#endif

sl_i2cspm_t* sl_sensor_select(sl_board_sensor_t sensor)
{
  sl_i2cspm_t *i2cspm_sensor = sl_i2cspm_sensor;

  switch (sensor) {
    case SL_BOARD_SENSOR_RHT:
      sl_thunderboard_require_i2c(SL_THUNDERBOARD_I2C_BUS_SELECT_ENV);
      break;
    case SL_BOARD_SENSOR_LIGHT:
      sl_thunderboard_require_i2c(SL_THUNDERBOARD_I2C_BUS_SELECT_ENV);
      break;
    case SL_BOARD_SENSOR_PRESSURE:
      sl_thunderboard_require_i2c(SL_THUNDERBOARD_I2C_BUS_SELECT_ENV);
      break;
    case SL_BOARD_SENSOR_HALL:
      sl_thunderboard_require_i2c(SL_THUNDERBOARD_I2C_BUS_SELECT_HALL);
      break;
    case SL_BOARD_SENSOR_GAS:
      sl_thunderboard_require_i2c(SL_THUNDERBOARD_I2C_BUS_SELECT_GAS);
      break;
    default:
      i2cspm_sensor = NULL;
      break;
  }

  return i2cspm_sensor;
}
