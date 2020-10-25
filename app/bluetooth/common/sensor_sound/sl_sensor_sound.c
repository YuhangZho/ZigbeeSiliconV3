/***************************************************************************//**
 * @file
 * @brief Sound level sensor
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

#include <stdbool.h>
#include "sl_board_control.h"
#include "sl_mic.h"
#include "sl_sleeptimer.h"
#include "sl_app_assert.h"
#include "sl_sensor_sound.h"

// -----------------------------------------------------------------------------
// Private macros

#define MIC_SAMPLE_RATE            44100
#define MIC_SAMPLE_BUFFER_SIZE     1000
#define MIC_N_CHANNELS             1
#define MIC_IIR_WEIGHT             0.1f

// -----------------------------------------------------------------------------
// Private variables

static bool mic_enabled = false;
static float sound_level = 0;
static int16_t buffer[MIC_SAMPLE_BUFFER_SIZE * MIC_N_CHANNELS];

// -----------------------------------------------------------------------------
// Public function definitions

void sl_sensor_sound_init(void)
{
  sl_status_t sc;
  // Enable microphone and wait 50 ms power up time
  sl_board_enable_sensor(SL_BOARD_SENSOR_MICROPHONE);
  sl_sleeptimer_delay_millisecond(50);
  sc = sl_mic_init(MIC_SAMPLE_RATE, MIC_N_CHANNELS);
  sl_app_assert(SL_STATUS_OK == sc, "[E: %#04x] Microphone init failed\n", sc);
  // Start sampling
  sl_mic_get_n_samples(buffer, MIC_SAMPLE_BUFFER_SIZE);
  mic_enabled = true;
}

void sl_sensor_sound_deinit(void)
{
  mic_enabled = false;
  sl_mic_deinit();
  sl_board_disable_sensor(SL_BOARD_SENSOR_MICROPHONE);
}

sl_status_t sl_sensor_sound_get(float *sl)
{
  *sl = sound_level;
  return SL_STATUS_OK;
}

void sl_sensor_sound_step(void)
{
  float sl;
  if (mic_enabled) {
    if (sl_mic_sample_buffer_ready()) {
      if (SL_STATUS_OK == sl_mic_calculate_sound_level(&sl, buffer, MIC_SAMPLE_BUFFER_SIZE, 0)) {
        // Apply IIR filter on the measured value
        sound_level = (sl * MIC_IIR_WEIGHT) + (sound_level * (1 - MIC_IIR_WEIGHT));
      }
      sl_mic_get_n_samples(buffer, MIC_SAMPLE_BUFFER_SIZE);
    }
  }
}
