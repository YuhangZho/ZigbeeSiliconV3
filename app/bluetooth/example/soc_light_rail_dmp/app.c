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
#include "gatt_db.h"
#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT
#ifdef SL_CATALOG_CLI_PRESENT
#include "sl_cli.h"
#endif // SL_CATALOG_CLI_PRESENT
#include "app.h"
#include "app_bluetooth.h"
#include "app_proprietary.h"

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
  init_demo_app();
  init_prop_app();
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
}

#ifdef SL_CATALOG_CLI_PRESENT
void hello(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  bd_addr address;
  uint8_t address_type;
  sl_status_t sc = sl_bt_system_get_identity_address(&address, &address_type);
  sl_app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to get Bluetooth address\n",
                (int)sc);
  sl_app_log("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X\n",
             address_type ? "static random" : "public device",
             address.addr[5],
             address.addr[4],
             address.addr[3],
             address.addr[2],
             address.addr[1],
             address.addr[0]);
}
#endif // SL_CATALOG_CLI_PRESENT
