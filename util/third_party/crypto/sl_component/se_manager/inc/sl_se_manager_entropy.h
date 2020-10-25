/***************************************************************************//**
 * @file
 * @brief Silicon Labs Secure Element Manager API.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: APACHE-2.0
 *
 * This software is subject to an open source license and is distributed by
 * Silicon Laboratories Inc. pursuant to the terms of the Apache License,
 * Version 2.0 available at https://www.apache.org/licenses/LICENSE-2.0.
 * Such terms and conditions may be further supplemented by the Silicon Labs
 * Master Software License Agreement (MSLA) available at www.silabs.com and its
 * sections applicable to open source software.
 *
 ******************************************************************************/
#ifndef SL_SE_MANAGER_ENTROPY_H
#define SL_SE_MANAGER_ENTROPY_H

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT) || defined(DOXYGEN)

/// @addtogroup sl_se_manager
/// @{

/***************************************************************************//**
 * @addtogroup sl_se_manager_entropy Entropy
 *
 * @brief
 *   Random number generators
 *
 * @details
 *   API for getting randomness from the Secure Element True Random Number
 *   Generator (TRNG).
 *
 * @{
 ******************************************************************************/

#include "sl_se_manager_key_handling.h"
#include "sl_se_manager_types.h"
#include "em_se.h"
#include "sl_status.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * @brief
 *   Get random data from hardware TRNG.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[out] data
 *   Random data from TRNG.
 *
 * @param[in] num_bytes
 *   Length of data request.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_get_random(sl_se_command_context_t *cmd_ctx,
                             void *data,
                             uint32_t num_bytes);

#ifdef __cplusplus
}
#endif

/// @} (end addtogroup sl_se_manager_entropy)
/// @} (end addtogroup sl_se)

#endif // defined(SEMAILBOX_PRESENT)

#endif // SL_SE_MANAGER_ENTROPY_H
