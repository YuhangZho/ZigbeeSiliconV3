/**************************************************************************/ /**
 * @file
 * @brief SE Manager configuration options
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

#ifndef SL_SE_MANAGER_CONFIG_H
#define SL_SE_MANAGER_CONFIG_H

/// This file include the configuration options of the SE Manager.
/// For the time being the user should not change the default settings
/// of the configuration options in this file.

#if defined (SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT) || defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
// Threading support (as opposed to API calls only from a single thread)
// is currently required in RTOS mode.
  #define SL_SE_MANAGER_THREADING

  #if !defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION) && !defined(CRYPTOACC_PRESENT)
// Enable yield support. Configure sl_se_command_context_t to yield CPU while waiting for SE commands.
// This is not supported on EFR32xG22.
    #define SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION
  #endif
#endif

// Check consistency of configuration options.
// Always include se_manager_check_config.h in order to assert that the
// configuration options dependencies and restrictions are aok.
#include "sl_se_manager_check_config.h"

#endif // SL_SE_MANAGER_CONFIG_H
