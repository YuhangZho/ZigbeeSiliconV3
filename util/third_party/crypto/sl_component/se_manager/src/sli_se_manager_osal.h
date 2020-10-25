/**************************************************************************/ /**
 * @file
 * @brief OS abstraction layer primitives for the SE Manager
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

#ifndef SE_MANAGER_OSAL_H
#define SE_MANAGER_OSAL_H

#if !defined(SE_MANAGER_CONFIG_FILE)
  #include "sl_se_manager_config.h"
#else
  #include SE_MANAGER_CONFIG_FILE
#endif

#if defined (SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_MICRIUMOS_KERNEL_PRESENT)
// Include MicriumOS kernel abstraction layer:
  #include "sli_se_manager_osal_micriumos.h"
#elif defined(SL_CATALOG_FREERTOS_KERNEL_PRESENT)
// Include FreeRTOS kernel abstraction layer:
  #include "sli_se_manager_osal_freertos.h"
#else
// Include bare metal abstraction layer:
  #include "sli_se_manager_osal_baremetal.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SL_SE_MANAGER_THREADING) || defined(DOXYGEN)

/***************************************************************************//**
 * @brief          Initialize a given mutex
 *
 * @param mutex    Pointer to the mutex needing initialization
 *
 * @return         SL_STATUS_OK on success, error code otherwise.
 *****************************************************************************/
__STATIC_INLINE sl_status_t se_manager_osal_init_mutex(se_manager_osal_mutex_t *mutex);

/***************************************************************************//**
 * @brief          Free a given mutex
 *
 * @param mutex    Pointer to the mutex being freed
 *
 * @return         SL_STATUS_OK on success, error code otherwise.
 *****************************************************************************/
__STATIC_INLINE sl_status_t se_manager_osal_free_mutex(se_manager_osal_mutex_t *mutex);

/***************************************************************************//**
 * @brief          Pend on a mutex
 *
 * @param mutex    Pointer to the mutex being pended on
 *
 * @return         SL_STATUS_OK on success, error code otherwise.
 *****************************************************************************/
__STATIC_INLINE sl_status_t se_manager_osal_take_mutex(se_manager_osal_mutex_t *mutex);

/***************************************************************************//**
 * @brief          Try to own a mutex without waiting
 *
 * @param mutex    Pointer to the mutex being tested
 *
 * @return         SL_STATUS_OK on success (= mutex successfully owned), error code otherwise.
 *****************************************************************************/
__STATIC_INLINE sl_status_t se_manager_osal_take_mutex_non_blocking(se_manager_osal_mutex_t *mutex);

/***************************************************************************//**
 * @brief          Release a mutex
 *
 * @param mutex    Pointer to the mutex being released
 *
 * @return         SL_STATUS_OK on success, error code otherwise.
 *****************************************************************************/
__STATIC_INLINE sl_status_t se_manager_osal_give_mutex(se_manager_osal_mutex_t *mutex);

#endif // SL_SE_MANAGER_THREADING

/***************************************************************************//**
 * @brief          Initialize a completion object.
 *
 * @param p_comp   Pointer to an se_manager_osal_completion_t object allocated
 *                 by the user.
 *
 * @return         Status code, @ref sl_status.h.
 *****************************************************************************/
__STATIC_INLINE sl_status_t
se_manager_osal_init_completion(se_manager_osal_completion_t *p_comp);

/***************************************************************************//**
 * @brief          Free a completion object.
 *
 * @param p_comp   Pointer to an se_manager_osal_completion_t object.
 *
 * @return         Status code, @ref sl_status.h.
 *****************************************************************************/
__STATIC_INLINE sl_status_t
se_manager_osal_free_completion(se_manager_osal_completion_t *p_comp);

/***************************************************************************//**
 * @brief          Wait for completion event.
 *
 * @param p_comp   Pointer to completion object which must be initialized by
 *                 calling se_manager_osal_completion_init before calling this
 *                 function.
 *
 * @param ticks    Ticks to wait for the completion.
 *                 Pass a value of SE_MANAGER_OSAL_WAIT_FOREVER in order to
 *                 wait forever.
 *                 Pass a value of SE_MANAGER_OSAL_NON_BLOCKING in order to
 *                 return immediately.
 *
 * @return         Status code, @ref sl_status.h. Typcally SL_STATUS_OK if success,
 *                 or SL_STATUS_TIMEOUT if no completion within the given ticks.
 *****************************************************************************/
__STATIC_INLINE sl_status_t
se_manager_osal_wait_completion(se_manager_osal_completion_t *p_comp,
                                int ticks);

/***************************************************************************//**
 * @brief          Signal completion.
 *
 * @param p_comp   Pointer to completion object which must be initialized by
 *                 calling se_manager_osal_completion_init before calling this
 *                 function.
 *
 * @return         Status code, @ref sl_status.h.
 *****************************************************************************/
__STATIC_INLINE sl_status_t
se_manager_osal_complete(se_manager_osal_completion_t *p_comp);

#ifdef __cplusplus
}
#endif

#endif // SE_MANAGER_OSAL_H
