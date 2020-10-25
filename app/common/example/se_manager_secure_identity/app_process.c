/***************************************************************************//**
 * @file app_process.c
 * @brief Top level application functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_process.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// State machine state variable
static state_t app_state = SE_MANAGER_INIT;

/// String for example
static uint8_t example_string[] = "SE Manager Secure Identity Example";

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Application state machine, called infinitely.
 ******************************************************************************/
void app_process_action(void)
{
  switch (app_state) {
    case SE_MANAGER_INIT:
      printf("\n%s - Core running at %" PRIu32 " kHz.\n", example_string,
             CMU_ClockFreqGet(cmuClock_CORE) / 1000);
      printf("  . SE manager initialization... ");
      if (init_se_manager() == SL_STATUS_OK) {
        app_state = RD_CERT_SIZE;
      } else {
        app_state = SE_MANAGER_IDLE;
      }
      break;

    case RD_CERT_SIZE:
      printf("\n  . Secure Vault device:\n");
      printf("  + Read size of on-chip certificates... ");
      if (read_cert_size() == SL_STATUS_OK) {
        app_state = RD_DEVICE_CERT;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case RD_DEVICE_CERT:
      printf("  + Read on-chip device certificate... ");
      if (read_cert_data(SL_SE_CERT_DEVICE_HOST) == SL_STATUS_OK) {
        app_state = PARSE_DEVICE_CERT;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case PARSE_DEVICE_CERT:
      printf("  + Parse the device certificate (DER format)... ");
      if (parse_device_cert() == SL_STATUS_OK) {
        printf("  + Get the public device key in device certificate... ");
        if (get_pub_device_key() == SL_STATUS_OK) {
          printf("OK\n");
          app_state = RD_BATCH_CERT;
        } else {
          printf("Failed\n");
          app_state = SE_MANAGER_EXIT;
        }
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case RD_BATCH_CERT:
      printf("  + Read on-chip batch certificate... ");
      if (read_cert_data(SL_SE_CERT_BATCH) == SL_STATUS_OK) {
        app_state = PARSE_BATCH_CERT;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case PARSE_BATCH_CERT:
      printf("  + Parse the batch certificate (DER format)... ");
      if (parse_batch_cert() == SL_STATUS_OK) {
        app_state = PARSE_FACTORY_CERT;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case PARSE_FACTORY_CERT:
      printf("\n  . Remote device:\n");
      printf("  + Parse the factory certificate (PEM format)... ");
      if (parse_factory_cert() == SL_STATUS_OK) {
        app_state = PARSE_ROOT_CERT;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case PARSE_ROOT_CERT:
      printf("  + Parse the root certificate (PEM format)... ");
      if (parse_root_cert() == SL_STATUS_OK) {
        app_state = VERIFY_CERT_CHAIN;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case VERIFY_CERT_CHAIN:
#if (SE_MANAGER_PRINT_CERT == 0)
      printf("  + Verify the certificate chain with root certificate... ");
#endif
      if (verify_cert_chain() == SL_STATUS_OK) {
        app_state = CREATE_CHALLENGE;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case CREATE_CHALLENGE:
      printf("\n  . Remote authentication:\n");
      printf("  + Create %d bytes challenge (random number) in remote device "
             "for signing... ", CHALLENGE_SIZE);
      if (generate_random_number(get_challenge_buf_ptr(), CHALLENGE_SIZE)
          == SL_STATUS_OK) {
        app_state = SIGN_CHALLENGE;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case SIGN_CHALLENGE:
      printf("  + Sign challenge with private device key in Secure Vault "
             "device... ");
      if (sign_challenge() == SL_STATUS_OK) {
        app_state = VERIFY_SIGNATURE;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case VERIFY_SIGNATURE:
      printf("  + Verify signature with public device key in remote "
             "device... ");
      verify_signature();
      free_cert_ctx();
      app_state = SE_MANAGER_EXIT;
      break;

    case SE_MANAGER_EXIT:
      printf("\n  . SE manager deinitialization... ");
      deinit_se_manager();
      app_state = SE_MANAGER_IDLE;
      break;

    case SE_MANAGER_IDLE:
    default:
      break;
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
