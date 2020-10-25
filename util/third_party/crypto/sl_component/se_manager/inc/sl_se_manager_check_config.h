/**************************************************************************/ /**
 * @file
 * @brief Consistency checks for SE Manager configuration options
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

#ifndef SL_SE_MANAGER_CHECK_CONFIG_H
#define SL_SE_MANAGER_CHECK_CONFIG_H

#if defined (SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_SE_MANAGER_THREADING)                                    \
  && !defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION) \
  && !defined(CRYPTOACC_PRESENT)
#error "Yield when waiting for SE commands to finish is currently required in RTOS mode."
#endif
#if defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION) \
  && !defined(SL_SE_MANAGER_THREADING)
#error "Yield when waiting for SE commands to finish currently requires RTOS mode. I.e. yield support is not available in bare metal mode."
#endif

#if (defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT) || defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)) \
  && !defined(SL_SE_MANAGER_THREADING)
#error "RTOS requires threading mode."
#endif

#if (defined(CRYPTOACC_PRESENT) && defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION))
#error "Yield support is not available on EFR32xG22 devices"
#endif

#endif // SL_SE_MANAGER_CHECK_CONFIG_H
