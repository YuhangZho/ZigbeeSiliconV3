/***************************************************************************//**
 * @file
 * @brief ADPCM encoder
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef ADPCM_H_
#define ADPCM_H_

#include <stdint.h>

/***************************************************************************//**
 * @addtogroup ADPCM
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @defgroup ADPCM_Config_Settings ADPCM coder Configuration Settings
 * @{
 * @brief ADPCM coder configuration setting macro definitions
 ******************************************************************************/

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/
typedef struct {
  int8_t step;                         /**< Index into adpcmStepTable */
  int16_t predictedSample;             /**< Last predicted sample */
} adpcm_state_t;

typedef struct {
  adpcm_state_t comprStateBegin;       /**< Compression begin state */
  adpcm_state_t comprStateCurrent;     /**< Compression current state */
} adpcm_t;

typedef struct {
  uint8_t ch_count;
  adpcm_t *adpcm_list;
} adpcm_context_t;

/***************************************************************************//**
 * @defgroup ADPCM_Functions ADPCM coder Functions
 * @{
 * @brief ADPCM coder API functions
 ******************************************************************************/

void ADPCM_init(adpcm_context_t *ctx);
void ADPCM_encode(adpcm_context_t *ctx, int16_t *in, uint8_t* out, uint32_t n_frames);

/** @} {end defgroup ADPCM_Functions}*/

/** @} {end addtogroup ADPCM} */

#endif /* ADPCM_H_ */
