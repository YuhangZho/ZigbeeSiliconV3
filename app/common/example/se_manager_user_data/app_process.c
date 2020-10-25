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
static uint8_t example_string[] = "SE Manager User Data Example";

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Application state machine, called infinitely.
 *****************************************************************************/
void app_process_action(void)
{
  uint32_t i;

  switch (app_state) {
    case SE_MANAGER_INIT:
      printf("\n%s - Core running at %" PRIu32 " kHz.\n", example_string,
             CMU_ClockFreqGet(cmuClock_CORE) / 1000);
      printf("  . SE manager initialization... ");
      if (init_se_manager() == SL_STATUS_OK) {
        app_state = ERASE_USER_DATA;
      } else {
        app_state = SE_MANAGER_IDLE;
      }
      break;

    case ERASE_USER_DATA:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Erase user data section... ");
      if (erase_user_data() == SL_STATUS_OK) {
        printf("  + Check all data in user data section are erased... ");
        for (i = 0; i < (USERDATA_SIZE / 4); i++) {
          if (*((uint32_t*)USERDATA_BASE + i) != 0xffffffff) {
            printf("Failed\n");
            break;
          }
        }
        if (i == (USERDATA_SIZE / 4)) {
          printf("OK\n");
          app_state = READ_SE_VERSION;
        }
      }
      break;

    case READ_SE_VERSION:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Get the SE firmware version... ");
      if (get_se_version() == SL_STATUS_OK) {
        printf("  + The SE firmware version (MSB..LSB): %08lX\n",
               get_version());
        app_state = WRITE_SE_VERSION;
      }
      break;

    case WRITE_SE_VERSION:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Write the SE firmware version to user data... ");
      if (write_user_data(0) == SL_STATUS_OK) {
        printf("  + Read back the SE firmware version from user data to "
               "compare... ");
        if (*(uint32_t*)USERDATA_BASE != get_version()) {
          printf("Failed\n");
          break;
        }
        printf("OK\n");
        app_state = READ_OTP_VERSION;
      }
      break;

    case READ_OTP_VERSION:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Get the OTP firmware version of the SE... ");
      if (get_otp_version() == SL_STATUS_OK) {
        printf("  + The OTP firmware version of the SE (MSB..LSB): %08lX\n",
               get_version());
        app_state = WRITE_OTP_VERSION;
      }
      break;

    case WRITE_OTP_VERSION:
      printf("\n  . Write the OTP firmware version of the SE to user data... ");
      if (write_user_data(4) == SL_STATUS_OK) {
        printf("  + Read back the OTP firmware version of the SE from user "
               "data to compare... ");
        if (*((uint32_t*)USERDATA_BASE + 1) != get_version()) {
          printf("Failed\n");
        } else {
          printf("OK\n");
        }
      }
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
