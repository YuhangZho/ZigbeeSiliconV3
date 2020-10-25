/***************************************************************************//**
 * @file
 * @brief Silicon Labs CRYPTOACC device management interface.
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

#include "cryptoacc_management.h"

#if defined(CRYPTOACC_PRESENT)

#include "sx_trng.h"
#include "sx_errors.h"
#include "cryptolib_def.h"
#include "sli_se_manager_internal.h"
#include "mbedtls/threading.h"

#define DO_TRNG_COND_TEST  (1)

static bool trng_initialized = false;

/* Get ownership of an available CRYPTOACC device */
int cryptoacc_management_acquire( void )
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
#endif

    CMU->CLKEN1 |= CMU_CLKEN1_CRYPTOACC;
    CMU->CRYPTOACCCLKCTRL |= CMU_CRYPTOACCCLKCTRL_PKEN;
    CMU->CRYPTOACCCLKCTRL |= CMU_CRYPTOACCCLKCTRL_AESEN;

    return 0;
}

/* Release ownership of an available CRYPTOACC device */
int cryptoacc_management_release( void )
{
    CMU->CLKEN1 = (CMU->CLKEN1 & ~CMU_CLKEN1_CRYPTOACC);
    CMU->CRYPTOACCCLKCTRL = (CMU->CRYPTOACCCLKCTRL & ~CMU_CRYPTOACCCLKCTRL_PKEN);
    CMU->CRYPTOACCCLKCTRL = (CMU->CRYPTOACCCLKCTRL & ~CMU_CRYPTOACCCLKCTRL_AESEN);

#if defined( MBEDTLS_THREADING_C )
    if (sli_se_lock_release() != SL_STATUS_OK) {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }
#endif

    return 0;
}

int cryptoacc_trng_initialize( void )
{
    int status;
    if(!trng_initialized) {
        status = cryptoacc_management_acquire();
        if (status != 0) {
            return status;
        }
        status = sx_trng_init(DO_TRNG_COND_TEST);
        cryptoacc_management_release();
        if (status != CRYPTOLIB_SUCCESS) {
            return MBEDTLS_ERR_CRYPTOACC_TRNG_FAILED;
        }
        trng_initialized = true;
    }
    return 0;
}

#endif /* CRYPTOACC_PRESENT */
