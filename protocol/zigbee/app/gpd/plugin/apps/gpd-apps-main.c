/***************************************************************************//**
 * @file
 * @brief Application main entry and initialisations.
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
#include "gpd-components-common.h"
#include "gpd-callbacks.h"

#if defined(CORTEXM3_EFR32_MICRO)
#include "sl_crypto/include/sl_mbedtls.h"
#endif

// Main entry to application
int main(void)
{
  // Initialise the system clocks and other HAL components
  halInit();

  #if defined(CORTEXM3_EFR32_MICRO)
  // This function must be called by an application before using any mbedTLS
  // functions. This function will make sure that the platform hooks in mbedTLS
  // are configured to ensure correct runtime behavior.
  sl_mbedtls_init();
  #endif

  // Initialise NV
  emberGpdNvInit();

  // Initialise timer for rxOffset timing during rxAfterTx
  emberGpdLeTimerInit();

  // Initialise Radio
  emberGpdRadioInit();

  //Initialise the Gpd
  EmberGpd_t * gpd = emberGpdInit();

  // Call user to implement rest of the thing
  emberGpdAfPluginMainCallback(gpd);
}
