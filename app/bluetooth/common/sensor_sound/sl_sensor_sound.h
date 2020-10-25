/***************************************************************************//**
 * @file
 * @brief Sound level sensor header
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

#ifndef SL_SENSOR_SOUND_H
#define SL_SENSOR_SOUND_H

#include "sl_status.h"

/**************************************************************************//**
 * Initialize sound level sensor.
 *****************************************************************************/
void sl_sensor_sound_init(void);

/**************************************************************************//**
 * Deinitialize sound level sensor.
 *****************************************************************************/
void sl_sensor_sound_deinit(void);

/**************************************************************************//**
 * Getter for sound level sensor measurement data.
 * @param[out] sl Sound pressure level (in dB).
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t sl_sensor_sound_get(float *sl);

/**************************************************************************//**
 * Sound level sensor event handler.
 *****************************************************************************/
void sl_sensor_sound_step(void);

#endif // SL_SENSOR_SOUND_H
