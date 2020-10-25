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
/***************************************************************************//**
 * Print SE status.
 ******************************************************************************/
static void print_se_status(void);

/***************************************************************************//**
 * Check SE status.
 ******************************************************************************/
static void check_se_status(void);

/***************************************************************************//**
 * Print buffer data in ASCII hex.

 * @param buf Pointer to the binary buffer.
 * @param len Number of bytes to print.
 ******************************************************************************/
static void print_buf(uint8_t *buf, size_t len);

/***************************************************************************//**
 * Issue a secure debug unlock to the device.
 ******************************************************************************/
static void issue_secure_debug_unlock(void);

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
static uint8_t example_string[] = "SE Manager Secure Debug Example";

/// Secure debug is active
static bool secure_debug_active;

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
        app_state = GET_SE_STATUS;
      } else {
        app_state = SE_MANAGER_IDLE;
      }
      break;

    case GET_SE_STATUS:
      printf("\n  . Get SE status... ");
      if (get_se_status() == SL_STATUS_OK) {
        print_se_status();
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case CHECK_SE_STATUS:
      check_se_status();
      break;

    case STANDARD_DEBUG_UNLOCK:
      if (pb0_press) {
        pb0_press = false;
        app_state = SE_MANAGER_EXIT;
        printf("  + Performing a device mass erase and debug unlock... ");
        disable_debug_lock();
        // Cannot reach here if debug unlock was successfully executed.
      }
      if (pb1_press) {
        pb1_press = false;
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case NORMAL_SECURE_DEBUG_ENABLE:
      app_state = SE_MANAGER_EXIT;
      printf("  + Reading the public command key from SE OTP... ");
      if (read_public_command_key() != SL_STATUS_OK) {
        break;
      }

      printf("  + Exporting a public command key from a hard-coded private "
             "command key... ");
      if (export_public_command_key() != SL_STATUS_OK) {
        break;
      }

      printf("  + Comparing exported public command key with public command "
             "key in SE OTP... ");
      if (compare_public_command_key() != 0) {
        printf("Failed\n");
        break;
      }
      printf("OK\n");
      printf("  + Press PB0 to lock the device or press PB1 to exit.\n");
      app_state = ENABLE_DEBUG_LOCK;
      break;

    case NORMAL_SECURE_DEBUG_DISABLE:
      app_state = SE_MANAGER_EXIT;
      printf("  + Exporting a public command key from a hard-coded private "
             "command key... ");
      if (export_public_command_key() != SL_STATUS_OK) {
        break;
      }

      printf("  + Reading the public command key from SE OTP... ");
      if (read_public_command_key() != SL_STATUS_OK) {
        printf("  + Press PB0 to program public command key in SE OTP or press "
               "PB1 to abort.\n");
        app_state = PROGRAM_COMMAND_KEY;
        break;
      } else {
        printf("  + Comparing exported public command key with SE OTP public "
               "command key... ");
        if (compare_public_command_key() != 0) {
          printf("Failed\n");
          break;
        } else {
          printf("OK\n");
        }
      }
      printf("  + Press PB0 to enable secure debug or press PB1 to exit.\n");
      app_state = ENABLE_SECURE_DEBUG_LOCK;
      break;

    case SECURE_DEBUG_LOCK:
      if (pb0_press) {
        pb0_press = false;
        app_state = ISSUE_SECURE_DEBUG_UNLOCK;
      }
      if (pb1_press) {
        pb1_press = false;
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case SECURE_DEBUG_UNLOCK:
      if (pb0_press) {
        pb0_press = false;
        app_state = SE_MANAGER_EXIT;
        printf("  + Request current challenge from the SE... ");
        if (request_challenge() != SL_STATUS_OK) {
          break;
        }
        printf("  + The current challenge (16 bytes): ");
        print_buf(get_challenge_buf_ptr(), 16);

        printf("  + Rolling the challenge... ");
        if (roll_challenge() == SL_STATUS_OK) {
          printf("  + Request rolled challenge from the SE... ");
          if (request_challenge() != SL_STATUS_OK) {
            break;
          }
          printf("  + The rolled challenge (16 bytes): ");
          print_buf(get_challenge_buf_ptr(), 16);
          printf("  + Issue a power-on or pin reset to activate the rolled "
                 "challenge.\n");
        }
      }
      if (pb1_press) {
        pb1_press = false;
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case PROGRAM_COMMAND_KEY:
      if (pb0_press) {
        pb0_press = false;
        // Option to provision public command key
        printf("  + Warning: The public command key in SE OTP cannot be "
               "changed once written!\n");
        printf("  + Press PB0 to confirm or press PB1 to skip if you are not "
               "sure.\n");
        app_state = CONFIRM_PROGRAM_COMMAND_KEY;
      }
      if (pb1_press) {
        pb1_press = false;
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case CONFIRM_PROGRAM_COMMAND_KEY:
      if (pb0_press) {
        pb0_press = false;
        printf("  + Programming a public command key to SE OTP... ");
        app_state = SE_MANAGER_EXIT;
        if (program_public_command_key() == SL_STATUS_OK) {
          printf("  + Press PB0 to enable secure debug or press PB1 to "
                 "exit.\n");
          app_state = ENABLE_SECURE_DEBUG_LOCK;
        }
      }
      if (pb1_press) {
        pb1_press = false;
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case ENABLE_SECURE_DEBUG_LOCK:
      if (pb0_press) {
        pb0_press = false;
        app_state = SE_MANAGER_EXIT;
        printf("  + Enable the secure debug... ");
        if (enable_secure_debug() != SL_STATUS_OK) {
          break;
        }
        printf("  + Press PB0 to lock the device or press PB1 to disable the "
               "secure debug and exit.\n");
        secure_debug_active = true;
        app_state = ENABLE_DEBUG_LOCK;
      }
      if (pb1_press) {
        pb1_press = false;
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case ENABLE_DEBUG_LOCK:
      if (pb0_press) {
        pb0_press = false;
        app_state = SE_MANAGER_EXIT;
        printf("  + Locking the device... ");
        if (enable_debug_lock() == SL_STATUS_OK) {
          if (get_se_status_buf_ptr()->debug_status.device_erase_enabled) {
            printf("  + Device erase is enabled, press PB0 to disable device "
                   "erase (optional) or press PB1 to skip.\n");
            app_state = DISABLE_DEVICE_ERASE;
          } else {
            // Device erase had already disabled
            app_state = GET_SE_STATUS;
          }
        }
      }
      if (pb1_press) {
        pb1_press = false;
        app_state = SE_MANAGER_EXIT;
        if (secure_debug_active) {
          secure_debug_active = false;
          printf("  + Disable the secure debug... ");
          disable_secure_debug();
        }
      }
      break;

    case DISABLE_DEVICE_ERASE:
      if (pb0_press) {
        pb0_press = false;
        // Option to disable device erase
        printf("  + Warning: This is a ONE-TIME command which PERMANETLY "
               "disables device erase!\n");
        printf("  + Press PB0 to confirm or press PB1 to skip if you are not "
               "sure.\n");
        app_state = CONFIRM_DISABLE_DEVICE_ERASE;
      }
      if (pb1_press) {
        pb1_press = false;
        app_state = GET_SE_STATUS;
      }
      break;

    case CONFIRM_DISABLE_DEVICE_ERASE:
      if (pb0_press) {
        pb0_press = false;
        printf("  + Disable device erase... ");
        if (disable_device_erase() == SL_STATUS_OK) {
          app_state = GET_SE_STATUS;
        } else {
          app_state = SE_MANAGER_EXIT;
          break;
        }
      }
      if (pb1_press) {
        pb1_press = false;
        app_state = GET_SE_STATUS;
      }
      break;

    case ISSUE_SECURE_DEBUG_UNLOCK:
      issue_secure_debug_unlock();
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
/***************************************************************************//**
 * Print SE status.
 ******************************************************************************/
static void print_se_status(void)
{
  printf("  + The SE firmware version (MSB..LSB): ");
  printf("%08lX\n", get_se_status_buf_ptr()->se_fw_version);
  printf("  + Debug lock: ");
  if (get_se_status_buf_ptr()->debug_status.debug_port_lock_applied) {
    printf("Enabled\n");
  } else {
    printf("Disabled\n");
  }
  printf("  + Debug lock state: ");
  if (get_se_status_buf_ptr()->debug_status.debug_port_lock_state) {
    printf("True\n");
  } else {
    printf("False\n");
  }
  printf("  + Device Erase: ");
  if (get_se_status_buf_ptr()->debug_status.device_erase_enabled) {
    printf("Enabled\n");
  } else {
    printf("Disabled\n");
  }
  printf("  + Secure debug: ");
  if (get_se_status_buf_ptr()->debug_status.secure_debug_enabled) {
    printf("Enabled\n");
  } else {
    printf("Disabled\n");
  }
  printf("  + Secure boot: ");
  if (get_se_status_buf_ptr()->secure_boot_enabled) {
    printf("Enabled\n");
  } else {
    printf("Disabled\n");
  }
  app_state = CHECK_SE_STATUS;
}

/***************************************************************************//**
 * Check SE status.
 ******************************************************************************/
static void check_se_status(void)
{
  // Standard debug lock
  if (get_se_status_buf_ptr()->debug_status.device_erase_enabled
      && get_se_status_buf_ptr()->debug_status.debug_port_lock_applied
      && get_se_status_buf_ptr()->debug_status.debug_port_lock_state
      && !get_se_status_buf_ptr()->debug_status.secure_debug_enabled) {
    printf("\n  . The device is in standard debug lock state, the secure "
           "debug unlock cannot apply to this part.\n");
    printf("  + Press PB0 to unlock the device or press PB1 to exit.\n");
    app_state = STANDARD_DEBUG_UNLOCK;
    return;
  }

  // Permanent debug lock
  if (!get_se_status_buf_ptr()->debug_status.device_erase_enabled
      && get_se_status_buf_ptr()->debug_status.debug_port_lock_applied
      && get_se_status_buf_ptr()->debug_status.debug_port_lock_state
      && !get_se_status_buf_ptr()->debug_status.secure_debug_enabled) {
    printf("\n  . The device is in permanent debug lock state, this part "
           "cannot be unlocked.\n");
    app_state = SE_MANAGER_EXIT;
    return;
  }

  // Normal state with secure debug enable
  if (!get_se_status_buf_ptr()->debug_status.debug_port_lock_applied
      && !get_se_status_buf_ptr()->debug_status.debug_port_lock_state
      && get_se_status_buf_ptr()->debug_status.secure_debug_enabled) {
    printf("\n  . The device is in normal state and secure debug is "
           "enabled.\n");
    app_state = NORMAL_SECURE_DEBUG_ENABLE;
    return;
  }

  // Normal state with secure debug disable
  if (!get_se_status_buf_ptr()->debug_status.debug_port_lock_applied
      && !get_se_status_buf_ptr()->debug_status.debug_port_lock_state
      && !get_se_status_buf_ptr()->debug_status.secure_debug_enabled) {
    printf("\n  . The device is in normal state and secure debug is "
           "disabled.\n");
    app_state = NORMAL_SECURE_DEBUG_DISABLE;
    return;
  }

  // Secure debug lock
  if (get_se_status_buf_ptr()->debug_status.debug_port_lock_applied
      && get_se_status_buf_ptr()->debug_status.debug_port_lock_state
      && get_se_status_buf_ptr()->debug_status.secure_debug_enabled) {
    printf("\n  . The device is in secure debug lock state.\n");
    printf("  + Press PB0 to issue a secure debug unlock or press PB1 to "
           "exit.\n");
    app_state = SECURE_DEBUG_LOCK;
    return;
  }

  // Secure debug unlock
  if (get_se_status_buf_ptr()->debug_status.debug_port_lock_applied
      && !get_se_status_buf_ptr()->debug_status.debug_port_lock_state
      && get_se_status_buf_ptr()->debug_status.secure_debug_enabled) {
    printf("\n  . The device is in secure debug unlock state.\n");
    printf("  + Issue a power-on or pin reset to re-enable the secure debug "
           "lock.\n");
    printf("  + Press PB0 to roll the challenge to invalidate the current "
           "unlock token or press PB1 to exit.\n");
    printf("  + The challenge cannot be rolled before it has been used at "
           "least once (secure debug unlock was issued).\n");
    app_state = SECURE_DEBUG_UNLOCK;
    return;
  }
}

/***************************************************************************//**
 * Print buffer data in ASCII hex.
 ******************************************************************************/
void print_buf(uint8_t *buf, size_t len)
{
  uint32_t i;
  uint8_t hex_array[16] = "0123456789ABCDEF";

  for (i = 0; i < len; i++) {
    printf("%c", hex_array[(buf[i] >> 4) & 0x0f]);
    printf("%c ", hex_array[buf[i] & 0x0f]);
  }
  printf("\n");
}

/***************************************************************************//**
 * Issue a secure debug unlock to the device.
 ******************************************************************************/
static void issue_secure_debug_unlock(void)
{
  app_state = SE_MANAGER_EXIT;

  printf("  + Creating a private certificate key in a buffer... ");
  if (create_private_certificate_key() != SL_STATUS_OK) {
    return;
  }

  printf("  + Exporting a public certificate key from a private certificate "
         "key... ");
  if (export_public_certificate_key() != SL_STATUS_OK) {
    return;
  }

  printf("  + Read the serial number of the SE and save it to access "
         "certificate... ");
  if (read_serial_number() != SL_STATUS_OK) {
    return;
  }

  printf("  + Signing the access certificate with private command key... ");
  if (sign_access_certificate() != SL_STATUS_OK) {
    return;
  }

  printf("  + Request challenge from the SE and save it to challenge "
         "response... ");
  if (request_challenge() != SL_STATUS_OK) {
    return;
  }

  printf("  + Signing the challenge response with private certificate key... ");
  if (sign_challenge_response() != SL_STATUS_OK) {
    return;
  }

  printf("  + Setting the debug options... ");
  if (set_debug_option() != SL_STATUS_OK) {
    return;
  }

  printf("  + Creating an unlock token to unlock the device... ");
  if (create_unlock_token() != SL_STATUS_OK) {
    return;
  }

  printf("  + Get debug status to verify the device is unlocked... ");
  if (get_debug_lock_status() != SL_STATUS_OK) {
    return;
  }

  if (!get_debug_lock_status_buf_ptr()->debug_port_lock_state) {
    printf("  + Success to unlock the device!\n");
    app_state = GET_SE_STATUS;
  } else {
    printf("  + Fail to unlock the device!\n");
  }
}
