/***************************************************************************//**
 * @file
 * @brief This file provides a definition for main() for non-RTOS applications.
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

#include PLATFORM_HEADER
#include "app/framework/include/af.h"

#if defined(EMBER_AF_PLUGIN_MBEDTLS) && defined(CORTEXM3_EFR32_MICRO)
#include "sl_mbedtls.h"
#endif // EMBER_AF_PLUGIN_MBEDTLS && CORTEXM3_EFR32_MICRO

// Our entry point is typically main(), except during testing.
#ifdef EMBER_TEST
  #define MAIN nodeMain
#else
  #define MAIN main
#endif

int MAIN(MAIN_FUNCTION_PARAMETERS)
{
  halInit();
  emberAfMainInit();
  #if defined(EMBER_AF_PLUGIN_MBEDTLS) && defined(CORTEXM3_EFR32_MICRO)
  // This function must be called by an application before using any mbedTLS
  // functions. This function will make sure that the platform hooks in mbedTLS
  // are configured to ensure correct runtime behavior.
  sl_mbedtls_init();
  #endif // EMBER_AF_PLUGIN_MBEDTLS && CORTEXM3_EFR32_MICRO
  return emberAfMain(MAIN_FUNCTION_ARGUMENTS);
}
