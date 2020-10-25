/**************************************************************************/ /**
 * @file
 * @brief OS abstraction primitives for the SE Manager for bare metal apps
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

#ifndef SE_MANAGER_OSAL_BAREMETAL_H
#define SE_MANAGER_OSAL_BAREMETAL_H

#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Defines

/// In order to wait forever in blocking functions the user can pass the
/// following value.
#define SE_MANAGER_OSAL_WAIT_FOREVER  (-1)
/// In order to return immediately in blocking functions the user can pass the
/// following value.
#define SE_MANAGER_OSAL_NON_BLOCKING  (0)

/// Priority to use for SEMBRX IRQ
#if defined(SE_MANAGER_USER_SEMBRX_IRQ_PRIORITY)
  #define SE_MANAGER_SEMBRX_IRQ_PRIORITY SE_MANAGER_USER_SEMBRX_IRQ_PRIORITY
#else
  #define SE_MANAGER_SEMBRX_IRQ_PRIORITY (0)
#endif

// -----------------------------------------------------------------------------
// Typedefs

/// Completion type used to wait for and signal end of operation.
typedef volatile bool se_manager_osal_completion_t;

/// SE manager mutex definition for Baremetal.
typedef volatile unsigned int se_manager_osal_mutex_t;


// -----------------------------------------------------------------------------
// Globals

#if defined(SE_MANAGER_OSAL_TEST)
  /// Global variable to keep track of ticks in bare metal test apps.
  extern unsigned int sli_se_manager_test_ticks;
#endif

// -----------------------------------------------------------------------------
// Functions

/// Initialize a completion object.
__STATIC_INLINE
sl_status_t se_manager_osal_init_completion(se_manager_osal_completion_t *p_comp)
{
  *p_comp = false;
  return (0);
}

/// Free a completion object.
__STATIC_INLINE
sl_status_t se_manager_osal_free_completion(se_manager_osal_completion_t *p_comp)
{
  *p_comp = false;
  return (0);
}

/// Wait for completion event.
__STATIC_INLINE sl_status_t
se_manager_osal_wait_completion(se_manager_osal_completion_t *p_comp, int ticks)
{
  int ret;
  if (ticks == SE_MANAGER_OSAL_WAIT_FOREVER) {
    while ( *p_comp == false ) {
#if defined(SE_MANAGER_OSAL_TEST)
      sli_se_manager_test_ticks++;
#endif
    }
    *p_comp = false;
    ret = 0;
  } else {
    while ((*p_comp == false) && (ticks > 0)) {
      ticks--;
#if defined(SE_MANAGER_OSAL_TEST)
      sli_se_manager_test_ticks++;
#endif
    }
    if (*p_comp == true) {
      *p_comp = false;
      ret = 0;
    } else {
      ret = SL_STATUS_TIMEOUT;
    }
  }

  return(ret);
}

/// Signal completion event.
__STATIC_INLINE
sl_status_t se_manager_osal_complete(se_manager_osal_completion_t* p_comp)
{
  *p_comp = true;
  return (0);
}

#ifdef __cplusplus
}
#endif

#endif // SE_MANAGER_OSAL_BAREMETAL_H
