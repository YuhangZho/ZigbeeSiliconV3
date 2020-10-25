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
/// Push button 0 status
static bool pb0_press;

/// Push button 1 status
static bool pb1_press;

/// State machine state variable
static state_t app_state = SE_MANAGER_INIT;

/// String for example
static uint8_t example_string[] = "SE Manager "S2 " Firmware Upgrade Example";

#if defined(CRYPTOACC_PRESENT)
/// Response buffer of executed command
static uint8_t resp_buf[RESP_BUF_SIZE];
#endif

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
      printf("\n\n%s - Core running at %" PRIu32 " kHz.\n", example_string,
             CMU_ClockFreqGet(cmuClock_CORE) / 1000);
      printf("  . SE manager initialization... ");
      if (init_se_manager() == SL_STATUS_OK) {
        if ((get_se_firmware_addr() == (const uint8_t *)SE_FIRMWARE_ADDR)
            && (get_se_firmware_size() == (uint32_t)SE_FIRMWARE_SIZE)) {
          printf("\n  . The start address and size of "S2 " firmware image are "
                                                          "0x%08lX and %lu bytes.\n", (uint32_t)SE_FIRMWARE_ADDR,
                 SE_FIRMWARE_SIZE);
          printf("  + The "S2 " firmware image version (MSB..LSB): %08lX\n",
                 *((uint32_t *)get_se_firmware_addr() + 3));
          app_state = READ_SE_VERSION;
        } else {
          printf("\n  . Incorrect "S2 " firmware image start address or/and "
                                      "size.\n");
          app_state = SE_MANAGER_EXIT;
        }
      } else {
        app_state = SE_MANAGER_IDLE;
      }
      break;

    case READ_SE_VERSION:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Get current "S2 " firmware version... ");
      if (get_se_version() == SL_STATUS_OK) {
        printf("  + Current "S2 " firmware version (MSB..LSB): %08lX\n",
               get_version());
#if defined(CRYPTOACC_PRESENT)
        app_state = CHECK_EXECUTED_COMMAND;
#else
        app_state = GET_SE_UPGRADE_STATUS;
#endif
      }
      break;

#if defined(CRYPTOACC_PRESENT)
    case CHECK_EXECUTED_COMMAND:
      printf("\n  . Check executed command... ");
      if (check_executed_command() != SL_STATUS_OK) {
        app_state = GET_SE_UPGRADE_STATUS;
      } else {
        switch (get_executed_command()) {
          case SLI_SE_COMMAND_STATUS_SE_IMAGE:
            app_state = POST_SE_UPGRADE_STATUS;
            break;

          case SLI_SE_COMMAND_CHECK_SE_IMAGE:
            app_state = POST_VALIDATE_SE_IMAGE;
            break;

          case SLI_SE_COMMAND_APPLY_SE_IMAGE:
            app_state = POST_UPGRADE_SE_IMAGE;
            break;

          default:
            app_state = SE_MANAGER_EXIT;
            break;
        }
      }
      break;
#endif

    case GET_SE_UPGRADE_STATUS:
#if defined(CRYPTOACC_PRESENT)
      printf("\n  . Reset to get previous VSE firmware upgrade status...  ");
      get_se_upgrade_status();
      app_state = SE_MANAGER_EXIT;
#else
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Get previous SE firmware upgrade status... ");
      if (get_se_upgrade_status() == SL_STATUS_OK) {
        printf("  + The status of the last SE upgrade attempt (MSB..LSB): "
               "0x%08lX\n", get_status());
        printf("  + Previous SE firmware upgrade version (MSB..LSB): %08lX\n",
               get_version());
        app_state = VALIDATE_SE_IMAGE;
      }
#endif
      break;

#if defined(CRYPTOACC_PRESENT)
    case POST_SE_UPGRADE_STATUS:
      printf("\n  . Get previous VSE firmware upgrade status response after "
             "reset... ");
      if (get_executed_command_response(resp_buf, sizeof(resp_buf)) == SL_STATUS_OK) {
        printf("  + The status of the last VSE upgrade attempt (MSB..LSB): "
               "0x%08lX\n", *((uint32_t *)resp_buf));
        printf("  + Previous VSE firmware upgrade version (MSB..LSB): %08lX\n",
               *((uint32_t *)resp_buf + 1));
        app_state = VALIDATE_SE_IMAGE;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;
#endif

    case VALIDATE_SE_IMAGE:
#if defined(CRYPTOACC_PRESENT)
      printf("\n  . Reset to validate the VSE firmware image...  ");
      validate_se_image();
      app_state = SE_MANAGER_EXIT;
#else
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Validate the SE firmware image... ");
      if (validate_se_image() == SL_STATUS_OK) {
        printf("  + Press PB0 to upgrade the SE firmware or press PB1 to "
               "exit.\n");
        app_state = UPGRADE_SE_IMAGE;
      }
#endif
      break;

#if defined(CRYPTOACC_PRESENT)
    case POST_VALIDATE_SE_IMAGE:
      printf("\n  . Get the VSE firmware image validation response after "
             "reset... ");
      if (get_executed_command_response(resp_buf, sizeof(resp_buf)) == SL_STATUS_OK) {
        printf("  + Press PB0 to upgrade the VSE firmware or press PB1 to "
               "exit.\n");
        app_state = UPGRADE_SE_IMAGE;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;
#endif

    case UPGRADE_SE_IMAGE:
      if (pb0_press) {
        pb0_press = false;
#if defined(CRYPTOACC_PRESENT)
        printf("\n  . Reset to upgrade the VSE firmware image...  ");
#else
        printf("\n  . Device will reset if SE firmware upgrade succeeds.\n");
        printf("  + Upgrade the SE firmware image... ");
#endif
        upgrade_se_image();
        // Cannot reach here if (V)SE firmware was successfully upgraded.
        app_state = SE_MANAGER_EXIT;
      }
      if (pb1_press) {
        pb1_press = false;
        app_state = SE_MANAGER_EXIT;
      }
      break;

#if defined(CRYPTOACC_PRESENT)
    case POST_UPGRADE_SE_IMAGE:
      printf("\n  . Get the VSE firmware image upgrade response after "
             "reset... ");
      get_executed_command_response(resp_buf, sizeof(resp_buf));
      app_state = SE_MANAGER_EXIT;
      break;
#endif

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

/***************************************************************************//**
 * Button callback, called if any button is pressed or released.
 ******************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  // Check if any button was pressed
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (&sl_button_btn0 == handle) {
      pb0_press = true;
    }
    if (&sl_button_btn1 == handle) {
      pb1_press = true;
    }
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
