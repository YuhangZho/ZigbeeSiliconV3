/***************************************************************************//**
 * @file
 * @brief Backbone of CLI framework
 * @version x.y.z
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "sl_system_kernel.h"
#include <common/include/rtos_err.h>
#include <kernel/include/os.h>
#else
#if defined(SL_CATALOG_CLI_DELAY_PRESENT)
#include "sl_cli_delay.h"
#endif
#if defined(SL_CATALOG_CLI_STORAGE_RAM_PRESENT)
#include "sl_cli_storage_ram.h"
#endif
#endif
#include "sl_cli.h"
#include "sli_cli_io.h"
#include "sl_cli_input.h"
#include "sli_cli_input.h"
#include <string.h>

#if defined(SL_CATALOG_CLI_STORAGE_NVM3_PRESENT)
#include "sl_cli_storage_nvm3.h"
#endif

/*******************************************************************************
 ****************************   STATIC VARIABLES   *****************************
 ******************************************************************************/

/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

sl_cli_handle_t sl_cli_default_handle;
sl_cli_command_group_t *sl_cli_default_command_group;

//****************************************************************************
// Local functions

/***************************************************************************//**
 * @brief
 *  Get a string that explain the status code in ascii text.
 *
 * @param[in] status
 *   A status code.
 *
 * @return
 *   A pointer to the string.
 ******************************************************************************/
static const char *status_to_string(sl_status_t status)
{
  char *string;

  switch (status) {
    case SL_STATUS_OK:
      string = "";
      break;
    case SL_STATUS_INVALID_PARAMETER:
      string = "The argument is not formatted correctly";
      break;
    case SL_STATUS_NOT_FOUND:
      string = "No command found";
      break;
    case SL_STATUS_HAS_OVERFLOWED:
      string = "Maximum number of arguments exceeded";
      break;
    case SL_STATUS_INVALID_TYPE:
      string = "Incorrect argument type";
      break;
    case SL_STATUS_INVALID_COUNT:
      string = "Incorrect number of arguments";
      break;
    default:
      string = "Unknown error";
      break;
  }

  return string;
}

/***************************************************************************//**
 * @brief
 *   Handle handle_input_and_history.
 *   Updates the history buffer, executes the function and clears the input
 *   string.
 ******************************************************************************/
static void handle_input_and_history(sl_cli_handle_t handle)
{
  if (strlen(handle->input_buffer) > 0) {
    sli_cli_input_update_history(handle);
    sl_cli_handle_input(handle, handle->input_buffer);
    sl_cli_input_clear(handle);
  }
}

/***************************************************************************//**
 * @brief
 *   Common tick function.
 *   Checks for new input, and acts according to the possible input.
 ******************************************************************************/
static bool tick(sl_cli_handle_t handle)
{
  int c;
  bool newline = false;
  bool no_valid_input = false;

  if (handle->tick_in_progress) {
    return no_valid_input;
  }
  handle->tick_in_progress = true;

  if (handle->req_prompt) {
    handle->req_prompt = false;
    sli_cli_io_printf("%s", handle->prompt_string);
  }
#if defined(SL_CLI_ACTIVE_FLAG_EN)
  handle->active = false;
#endif

  do {
#if !defined(SL_CATALOG_KERNEL_PRESENT)
    if (handle->input_char != EOF) {
      c = handle->input_char;
      handle->input_char = EOF;
    } else
#endif
    {
      c = sli_cli_io_getchar();
    }
    if (c != EOF) {
      newline = sl_cli_input_char(handle, (char)c);
    } else {
      no_valid_input = true;
    }
  } while ((c != EOF) && (!newline));

  if (newline) {
    handle_input_and_history(handle);
#if defined(SL_CLI_ACTIVE_FLAG_EN)
    handle->req_prompt = true;
    handle->active = true;
#else
    sli_cli_io_printf("%s", handle->prompt_string);
#endif
  }
  handle->tick_in_progress = false;

  return no_valid_input;
}

#if defined(SL_CATALOG_KERNEL_PRESENT)
/***************************************************************************//**
 * @brief
 *   The kernel tick function.
 ******************************************************************************/
static void tick_task(void *arg)
{
  RTOS_ERR err;
  sl_cli_handle_t handle = (sl_cli_handle_t)arg;

  sl_iostream_set_default(handle->iostream_handle);

  OSTimeDlyHMSM(0, 0, 0, handle->start_delay_ms, OS_OPT_TIME_HMSM_NON_STRICT, &err);

#if defined(SL_CATALOG_CLI_STORAGE_NVM3_PRESENT)
  sl_cli_storage_nvm3_tick(handle);
#endif

  while (1) {
    bool no_valid_input;

    no_valid_input = tick(handle);
    if (no_valid_input) {
      OSTimeDlyHMSM(0, 0, 0, handle->loop_delay_ms, OS_OPT_TIME_HMSM_NON_STRICT, &err);
      EFM_ASSERT(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);
    }
  }
}

/***************************************************************************//**
 * @brief
 *   The kernel create task function.
 ******************************************************************************/
static sl_status_t create_task(sl_cli_handle_t handle,
                               sl_cli_instance_parameters_t *parameters)
{
  RTOS_ERR err;

  OSTaskCreate(parameters->tcb,
               parameters->task_name,
               &tick_task,
               handle,
               parameters->prio,
               parameters->stack, (parameters->stack_size / 10u), parameters->stack_size,
               0u, 0u, DEF_NULL,
               (OS_OPT_TASK_STK_CLR), &err);

  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));

  return SL_STATUS_OK;
}
#endif

/*******************************************************************************
 ****************************   GLOBAL FUNCTIONS   *****************************
 ******************************************************************************/
uint8_t *sl_cli_get_argument_hex(sl_cli_command_arg_t *a, size_t n, size_t *l)
{
  // Byte 0 and 1 contains the length, while 2..n contains the data.
  uint8_t *ptr;
  uint8_t lo, hi;

  ptr = (uint8_t *)(a->argv[a->arg_ofs + n]);
  lo = *ptr++;
  hi = *ptr++;
  *l = (size_t)lo + ((size_t)hi << 8);
  return ptr;
}

void sl_cli_clear(sl_cli_handle_t handle)
{
  // Clear and initialize handle
  memset(handle, 0, sizeof(sl_cli_t));
  handle->input_size = SL_CLI_INPUT_BUFFER_SIZE;
  handle->input_pos = 0;
  handle->input_len = 0;
  handle->last_input_type = SL_CLI_INPUT_ORDINARY;
  handle->prompt_string = SL_CLI_PROMPT_STRING;
  handle->req_prompt = true;
#if SL_CLI_NUM_HISTORY_BYTES
  handle->history_pos = 0;
#endif
  sl_slist_init(&handle->command_group);
}

void sl_cli_redirect_command(sl_cli_handle_t handle, sl_cli_command_function_t command_function, const char *prompt, void *user)
{
  if ((handle->command_function == NULL) != (command_function == NULL)) {
    handle->command_function = command_function;
    handle->aux_argument = user;
    handle->prompt_string = (prompt == NULL) ? SL_CLI_PROMPT_STRING : (char *)prompt;
  }
}

sl_status_t sl_cli_handle_input(sl_cli_handle_t handle, char *string)
{
  sl_status_t status = SL_STATUS_OK;

  if (handle->command_function == NULL) {
    status = sl_cli_command_execute(handle, string);
    if (status != SL_STATUS_OK) {
      sli_cli_io_printf("%s\n", status_to_string(status));
    }
  } else {
    handle->command_function(string, handle->aux_argument);
  }

  return status;
}

sl_status_t sl_cli_instance_init(sl_cli_handle_t handle,
                                 sl_cli_instance_parameters_t *parameters)
{
  sl_status_t status = SL_STATUS_OK;

  sl_cli_clear(handle);
  handle->iostream_handle = parameters->iostream_handle;
  sl_cli_command_add_command_group(handle, parameters->default_command_group);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  handle->start_delay_ms = parameters->start_delay_ms;
  handle->loop_delay_ms = parameters->loop_delay_ms;
  status = create_task(handle, parameters);
#else
  handle->input_char = EOF;
#endif

#if defined(SL_CLI_ACTIVE_FLAG_EN)
  handle->active = true;
#endif

  return status;
}

#if !defined(SL_CATALOG_KERNEL_PRESENT)
bool sl_cli_is_ok_to_sleep(sl_cli_handle_t handle)
{
  if (handle->input_char == EOF) {
    handle->input_char = sli_cli_io_getchar();
  }
  if (handle->input_char != EOF) {
    return false;
  }

#if defined(SL_CLI_ACTIVE_FLAG_EN)
  return !handle->active;
#else
  return true;
#endif
}

void sl_cli_tick_instance(sl_cli_handle_t handle)
{
#if defined(SL_CATALOG_CLI_DELAY_PRESENT)
  if (sl_cli_delay_tick(handle)) {
    return;
  }
#endif
#if defined(SL_CATALOG_CLI_STORAGE_NVM3_PRESENT)
  if (sl_cli_storage_nvm3_tick(handle)) {
    return;
  }
#endif
#if defined(SL_CATALOG_CLI_STORAGE_RAM_PRESENT)
  if (sl_cli_storage_ram_tick(handle)) {
    return;
  }
#endif
  tick(handle);
}
#endif
