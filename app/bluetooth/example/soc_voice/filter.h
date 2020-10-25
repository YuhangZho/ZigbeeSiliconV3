/***************************************************************************//**
 * @file
 * @brief Biquadratic filters interface.
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

#ifndef FILTER_H
#define FILTER_H

#include <stdint.h>
#include "sl_status.h"

// -----------------------------------------------------------------------------
// Public macros

/** Default filter parameters */
#define DEFAULT_FILTER                        \
  {                                           \
    HPF,  /** Default filter type */          \
    0,    /** Default gain */                 \
    100,  /** Default frequency */            \
    8000, /** Default sampling rate */        \
    2,    /** Default bandwidth in octaves */ \
  }

// -----------------------------------------------------------------------------
// Public type definitions

/** Sample type */
typedef double sample_t;

/** This structure holds the data required to update samples through a filter */
typedef struct {
  sample_t a0, a1, a2, a3, a4;
  sample_t x1, x2, y1, y2;
} biquad_t;

/** Filter context for multi-channel operation */
typedef struct {
  uint8_t ch_count;
  biquad_t *biquad_list;
} filter_context_t;

/** Filter types */
typedef enum {
  LPF,     /**< Low Pass Filter        */
  HPF,     /**< High Pass Filter       */
  BPF,     /**< Band Pass Filter       */
  NOTCH,   /**< Notch Filter           */
  PEQ,     /**< Peaking Band EQ Filter */
  LSH,     /**< Low Shelf Filter       */
  HSH      /**< High Shelf Filter      */
} filter_type_t;

/** Filter parameters */
typedef struct {
  filter_type_t type;           /**< Filter type */
  sample_t      db_gain;        /**< Gain of filter */
  sample_t      freq;           /**< Center frequency */
  sample_t      srate;          /**< Sampling rate */
  sample_t      bandwidth;      /**< Bandwidth in octaves */
} filter_parameters_t;

// -----------------------------------------------------------------------------
// Public function declarations

/***************************************************************************//**
 * Filter initialization.
 *
 * @param[in] ctx Biquadratic filter list to be initialized.
 * @param[in] fp Filter parameters.
 *
 * @return Status of the operation.
 ******************************************************************************/
sl_status_t fil_init(filter_context_t *ctx, filter_parameters_t *fp);

/***************************************************************************//**
 * Filter audio data.
 *
 * @param[in] ctx Biquadratic filters to be applied on audio data.
 * @param[in] in Audio samples to be filtered.
 * @param[out] out Filtered audio samples (the input buffer can be reused).
 * @param[in] n_frames Number of samples to process per channel.
 *
 * @return Status of the operation.
 ******************************************************************************/
sl_status_t fil_filter(filter_context_t *ctx, int16_t *in, int16_t *out, uint32_t n_frames);

#endif // FILTER_H
