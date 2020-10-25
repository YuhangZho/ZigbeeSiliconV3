/***************************************************************************//**
 * @file
 * @brief Direct test mode interface.
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

#ifndef DTM_H
#define DTM_H

#include <stdint.h>

#include "sl_bt_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  /* Function for sending a single response byte to the Upper Tester */
  void (*write_response_byte)(uint8_t byte);

  /* Function for getting current clock ticks */
  uint32_t (*get_ticks)();

  /* The tick frequency in Hz returned by the get_ticks function */
  uint32_t ticks_per_second;

  /* A signal emitted by gecko_external_signal when a command is ready */
  uint32_t command_ready_signal;
} testmode_config_t;

/**************************************************************************//**
 * Initialize testmode library.
 * @param[in] config Configuration structure
 *****************************************************************************/
void testmode_init(const testmode_config_t *config);

/**************************************************************************//**
 * Process a single byte of a command received from the Upper Tester.
 * @param[in] byte The command byte to process
 *****************************************************************************/
void testmode_process_command_byte(uint8_t byte);

/**************************************************************************//**
 * Handle a gecko event. This function can be called for all events received
 * from the Bluetooth stack.
 * @param[in] evt Event received from the Bluetooth stack
 *****************************************************************************/
void testmode_handle_gecko_event(sl_bt_msg_t *evt);

#ifdef __cplusplus
};
#endif

#endif // DTM_H
