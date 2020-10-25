/***************************************************************************//**
 * @file
 * @brief Voice transmission header
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
#ifndef VOICE_H
#define VOICE_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  sr_8k = 8,
  sr_16k = 16,
} sample_rate_t;

/***************************************************************************//**
 * Initialize internal variables.
 ******************************************************************************/
void voice_init(void);

/***************************************************************************//**
 * Start voice transmission.
 ******************************************************************************/
void voice_start(void);

/***************************************************************************//**
 * Stop voice transmission.
 ******************************************************************************/
void voice_stop(void);

/***************************************************************************//**
 * Transmit voice buffer.
 * @param[in] buffer Transmit buffer containing voice data.
 * @param[in] size Size of the transmit buffer.
 * @note To be implemented in user code.
 ******************************************************************************/
void voice_transmit(uint8_t *buffer, uint32_t size);

/***************************************************************************//**
 * Voice event handler.
 ******************************************************************************/
void voice_process_action(void);

/***************************************************************************//**
 * Setter for configuration setting sample rate.
 *
 * @param[in] sample_rate Sample rate to be used, see \ref sample_rate_t.
 ******************************************************************************/
void voice_set_sample_rate(sample_rate_t sample_rate);

/***************************************************************************//**
 * Setter for configuration setting channels.
 *
 * @param[in] channels Number of audio channels to use.
 ******************************************************************************/
void voice_set_channels(uint8_t channels);

/***************************************************************************//**
 * Setter for configuration setting filter status.
 *
 * @param[in] status Enable (true) or disable (false) the biquad filter.
 ******************************************************************************/
void voice_set_filter_enable(bool status);

/***************************************************************************//**
 * Setter for configuration setting encoding status.
 *
 * @param[in] status Enable (true) or disable (false) the ADPCM encoder.
 ******************************************************************************/
void voice_set_encoding_enable(bool status);

#endif // VOICE_H
