/***************************************************************************//**
 * @file
 * @brief Silicon Labs CRYPTO device management interface.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "se_management.h"

#if defined( SEMAILBOX_PRESENT )

#include "sli_se_manager_internal.h"
#include "mbedtls/threading.h"

/* Get ownership of the Secure Element (SE).
   This function blocks until the SE becomes available. */
int se_management_acquire( void )
{
#if defined( MBEDTLS_THREADING_C )
    sl_status_t ret;

    if ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0U) {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }

    // Take SE lock - wait/block if taken by another thread.
    ret = sli_se_lock_acquire();

    if (ret != SL_STATUS_OK) {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }
    else {
        return 0;
    }

#else //!MBEDTLS_THREADING_C

#if !defined(USE_SE_IN_IRQ) || USE_SE_IN_IRQ == 0
    /* If USE_SE_IN_IRQ is not explicilty defined, then calling SE-based
       functionality from IRQ context is not supported, since there's no way
       to tell whether the SEMAILBOX is currently processing a command or is
       idling. */
    if ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0U) {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }
#endif
    return 0;

#endif //!MBEDTLS_THREADING_C
}

/* Release ownership of the Secure Element. */
int se_management_release( void )
{
#if defined( MBEDTLS_THREADING_C )
    sl_status_t ret;

    ret = sli_se_lock_release();

    if (ret != SL_STATUS_OK) {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }
#endif
    return 0;
}

#endif /* SEMAILBOX_PRESENT */
