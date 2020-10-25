/***************************************************************************//**
 * @file
 * @brief Core application logic.
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
#include <stdbool.h>
#include "em_common.h"
#include "sl_app_log.h"
#include "sl_app_assert.h"
#include "sl_bluetooth.h"
#include "app.h"
#include "dtm.h"
#include "sl_iostream_init_usart_instances.h"

enum signal{
  signal_testmode_command_ready = 1,
};

void app_write_response(uint8_t data)
{
  sl_iostream_putchar(sl_iostream_exp_handle, data);
}

static const testmode_config_t testmode_config = {
  .write_response_byte = app_write_response,
  .get_ticks = sl_sleeptimer_get_tick_count,
  .ticks_per_second = 32768,
  .command_ready_signal = signal_testmode_command_ready,
};

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  sl_app_log("soc_dtm initialised\n");
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////

  testmode_init(&testmode_config);
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////

  uint8_t data = 0;

  if (SL_STATUS_OK == sl_iostream_read(sl_iostream_exp_handle, &data, 1, NULL)) {
    testmode_process_command_byte(data);
  }
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  testmode_handle_gecko_event(evt);
}
