/**************************************************************************/ /**
 * @file
 * @brief OS abstraction layer primitives for SE manager on FreeRTOS
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

#ifndef SE_MANAGER_OSAL_FREERTOS_H
#define SE_MANAGER_OSAL_FREERTOS_H

#include "FreeRTOS.h"
#include "semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Defines

/// In order to wait forever in blocking functions the user can pass the
/// following value.
#define SE_MANAGER_OSAL_WAIT_FOREVER  ((int)portMAX_DELAY)
/// In order to return immediately in blocking functions the user can pass the
/// following value.
#define SE_MANAGER_OSAL_NON_BLOCKING  (0)

/// Priority to use for SEMBRX IRQ
#if defined(SE_MANAGER_USER_SEMBRX_IRQ_PRIORITY)
  #define SE_MANAGER_SEMBRX_IRQ_PRIORITY SE_MANAGER_USER_SEMBRX_IRQ_PRIORITY
#else
  #define SE_MANAGER_SEMBRX_IRQ_PRIORITY (configMAX_SYSCALL_INTERRUPT_PRIORITY >> (8U - __NVIC_PRIO_BITS) )
#endif

#if  (SE_MANAGER_SEMBRX_IRQ_PRIORITY < (configMAX_SYSCALL_INTERRUPT_PRIORITY >> (8U - __NVIC_PRIO_BITS) ) )
  #error SEMBRX IRQ priority should be numerically higher than or equal to the syscall interrupt.
#endif

/// Determine if executing at interrupt level on ARM Cortex-M.
#define RUNNING_AT_INTERRUPT_LEVEL (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)

// -----------------------------------------------------------------------------
// Typedefs

/// Completion object used to wait for and signal end of an operation.
typedef SemaphoreHandle_t se_manager_osal_completion_t;
/// SE manager mutex definition for FreeRTOS.
typedef SemaphoreHandle_t se_manager_osal_mutex_t;

// -----------------------------------------------------------------------------
// Functions

/// Initialize a mutex object.
__STATIC_INLINE
sl_status_t se_manager_osal_init_mutex(se_manager_osal_mutex_t *mutex)
{
  *mutex = (SemaphoreHandle_t) xSemaphoreCreateMutex();
  return (*mutex == NULL ? SL_STATUS_FAIL : SL_STATUS_OK);
}

/// Free a mutex object.
__STATIC_INLINE
sl_status_t se_manager_osal_free_mutex(se_manager_osal_mutex_t *mutex)
{
  /*
     Removed call to
     vSemaphoreDelete( (SemaphoreHandle_t) *p_comp );

     Mutex semaphores can not be deleted with default heap management of the
     current port. The following comment is from the vPortFree function of
     FreeRTOS which is called from vSemaphoreDelete (and subsequently
     vQueueDelete).
   */
  /* Memory cannot be freed using this scheme.  See heap_2.c, heap_3.c and
     heap_4.c for alternative implementations, and the memory management pages
     of http://www.FreeRTOS.org for more information. */
  return (*mutex == NULL ? SL_STATUS_FAIL : SL_STATUS_OK);
}

/// Acquire ownership of a mutex. If busy, wait until available.
__STATIC_INLINE
sl_status_t se_manager_osal_take_mutex(se_manager_osal_mutex_t *mutex)
{
  BaseType_t status;

  if ( (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0U ) {
    // Illegal call from ISR
    return SL_STATUS_ISR;
  }

  status = xSemaphoreTake(*mutex, (TickType_t) portMAX_DELAY);
  return (status == pdTRUE ? SL_STATUS_OK : SL_STATUS_FAIL);
}

/// Try to acquire ownership of a mutex without waiting.
__STATIC_INLINE
sl_status_t se_manager_osal_take_mutex_non_blocking(se_manager_osal_mutex_t *mutex)
{
  BaseType_t status;

  if ( (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0U ) {
    // Illegal call from ISR
    return SL_STATUS_ISR;
  }

  status = xSemaphoreTake(*mutex, (TickType_t) 0);
  return (status == pdTRUE ? SL_STATUS_OK : SL_STATUS_FAIL);
}

/// Release ownership of a mutex.
__STATIC_INLINE
sl_status_t se_manager_osal_give_mutex(se_manager_osal_mutex_t *mutex)
{
  BaseType_t status = xSemaphoreGive(*mutex);
  return (status == pdTRUE ? SL_STATUS_OK : SL_STATUS_FAIL);
}

/// Initialize a completion object.
__STATIC_INLINE sl_status_t
se_manager_osal_init_completion(se_manager_osal_completion_t *p_comp)
{
  *p_comp = (se_manager_osal_completion_t) xSemaphoreCreateBinary();
  EFM_ASSERT(*p_comp != NULL);
  return (*p_comp == NULL ? SL_STATUS_FAIL : SL_STATUS_OK);
}

/// Free a completion object.
__STATIC_INLINE sl_status_t
se_manager_osal_free_completion(se_manager_osal_completion_t *p_comp)
{
  (void) p_comp;
  /*
     Removed call to
     vSemaphoreDelete( (SemaphoreHandle_t) *p_comp );

     Semaphores can not be deleted with default heap management of the current
     port. The following comment is from the vPortFree function of FreeRTOS
     which is called from vSemaphoreDelete (and subsequently vQueueDelete).
   */
  /* Memory cannot be freed using this scheme.  See heap_2.c, heap_3.c and
     heap_4.c for alternative implementations, and the memory management pages
     of http://www.FreeRTOS.org for more information. */

  return (SL_STATUS_OK);
}

// Wait for a completion object to be completed.
__STATIC_INLINE sl_status_t
se_manager_osal_wait_completion(se_manager_osal_completion_t *p_comp, int ticks)
{
  return xSemaphoreTake( (SemaphoreHandle_t) *p_comp, (TickType_t) ticks)
         == pdTRUE ? SL_STATUS_OK : SL_STATUS_TIMEOUT;
}

// Complete a completion object.
__STATIC_INLINE sl_status_t
se_manager_osal_complete(se_manager_osal_completion_t* p_comp)
{
  BaseType_t status;
  if (RUNNING_AT_INTERRUPT_LEVEL) {
    BaseType_t higher_priority_task_woken;
    status = xSemaphoreGiveFromISR( (SemaphoreHandle_t) *p_comp,
                                    &higher_priority_task_woken);
  } else {
    status = xSemaphoreGive( (SemaphoreHandle_t) *p_comp);
  }
  EFM_ASSERT(status == pdTRUE);
  return (status == pdTRUE ? SL_STATUS_OK : SL_STATUS_FAIL);
}

#ifdef __cplusplus
}
#endif

#endif // SE_MANAGER_OSAL_FREERTOS_H
