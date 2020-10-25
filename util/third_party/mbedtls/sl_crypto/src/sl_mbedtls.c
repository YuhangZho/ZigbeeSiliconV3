/***************************************************************************//**
 * @file
 * @brief Initialize the Silicon Labs platform integration of mbedTLS.
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
#include "sl_mbedtls.h"
#include "mbedtls/threading.h"
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
#include "sl_se_manager.h"
#endif

/*
 * Initialize the Silicon Labs platform integration of mbedTLS.
 *
 * This function must be called by an application before using any mbedTLS
 * functions. This function will make sure that the platform hooks in mbedTLS
 * are configured to ensure correct runtime behavior.
 */
void sl_mbedtls_init(void)
{
#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)
  /* Initialize the SE Manager including the SE lock.
     No need for critical region here since sl_se_init implements one. */
  sl_status_t ret;
  ret = sl_se_init();
  EFM_ASSERT(ret == SL_STATUS_OK);
#endif

#if defined(MBEDTLS_THREADING_C)
  mbedtls_threading_set_alt(&THREADING_InitMutex,
                            &THREADING_FreeMutex,
                            &THREADING_TakeMutexBlocking,
                            &THREADING_GiveMutex);
#endif
}
