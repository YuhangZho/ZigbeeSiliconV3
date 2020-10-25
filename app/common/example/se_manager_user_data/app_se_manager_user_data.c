/***************************************************************************//**
 * @file app_se_manager_user_data.c
 * @brief SE manager user data functions.
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
#include "app_se_manager_user_data.h"

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
/// Command context
static sl_se_command_context_t cmd_ctx;

/// SE or OTP firmware version
static uint32_t version;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/***************************************************************************//**
 * Get version data.
 ******************************************************************************/
uint32_t get_version(void)
{
  return(version);
}

/***************************************************************************//**
 * Initialize the SE Manager.
 ******************************************************************************/
sl_status_t init_se_manager(void)
{
  print_error_cycle(sl_se_init(), NULL);
}

/***************************************************************************//**
 * Denitialize the SE Manager.
 ******************************************************************************/
sl_status_t deinit_se_manager(void)
{
  print_error_cycle(sl_se_deinit(), NULL);
}

/***************************************************************************//**
 * Get the SE firmware version.
 ******************************************************************************/
sl_status_t get_se_version(void)
{
  print_error_cycle(sl_se_get_se_version(&cmd_ctx, &version), &cmd_ctx);
}

/***************************************************************************//**
 * Get the OTP firmware version of the SE.
 ******************************************************************************/
sl_status_t get_otp_version(void)
{
  print_error_cycle(sl_se_get_otp_version(&cmd_ctx, &version), &cmd_ctx);
}

/***************************************************************************//**
 * Erase user data section in SE.
 ******************************************************************************/
sl_status_t erase_user_data(void)
{
  print_error_cycle(sl_se_erase_user_data(&cmd_ctx), &cmd_ctx);
}

/***************************************************************************//**
 * Write data to user data section in SE.
 ******************************************************************************/
sl_status_t write_user_data(uint32_t offset)
{
  print_error_cycle(sl_se_write_user_data(&cmd_ctx,
                                          offset,
                                          &version,
                                          sizeof(version)), &cmd_ctx);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
