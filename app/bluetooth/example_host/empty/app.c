/***************************************************************************//**
 * @file
 * @brief Empty NCP-host Example Project.
 *
 * Reference implementation of an NCP (Network Co-Processor) host, which is
 * typically run on a central MCU without radio. It can connect to an NCP via
 * VCOM to access the Bluetooth stack of the NCP and to control it using BGAPI.
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

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include "system.h"
#include "sl_bt_api.h"
#include "sl_bt_ncp_host.h"
#include "log.h"
#include "app_assert.h"
#include "uart.h"
#include "app.h"

#define USAGE "Usage: %s <serial port> <baud rate> [flow control: 1(on, default) or 0(off)]\n\n"
#define DEFAULT_UART_PORT             NULL
#define DEFAULT_UART_BAUD_RATE        115200
#define DEFAULT_UART_FLOW_CONTROL     1

// This characteristic handle value has to match the value in gatt_db.h of
// NCP empty example running on the connected WSTK.
#define GATTDB_SYSTEM_ID 18

SL_BT_API_DEFINE();

// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;

static int serial_port_init(int argc, char *argv[], int32_t timeout);
static void uart_tx_wrapper(uint32_t len, uint8_t *data);

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
void app_init(int argc, char *argv[])
{
  SL_BT_API_INITIALIZE_NONBLOCK(uart_tx_wrapper, uartRx, uartRxPeek);
  // Initialise serial communication as non-blocking.
  if (serial_port_init(argc, argv, 100) < 0) {
    app_log("Non-blocking serial port init failure\n");
    exit(EXIT_FAILURE);
  }
  // Flush std output
  fflush(stdout);
  app_log("Empty NCP-host initialised\n");
  app_log("Resetting NCP...\n");
  // Reset NCP to ensure it gets into a defined state.
  // Once the chip successfully boots, boot event should be received.
  sl_bt_system_reset(0);

  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  bd_addr address;
  uint8_t address_type;
  uint8_t system_id[8];

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      // Print boot message.
      app_log("Bluetooth stack booted: v%d.%d.%d-b%d\n",
              evt->data.evt_system_boot.major,
              evt->data.evt_system_boot.minor,
              evt->data.evt_system_boot.patch,
              evt->data.evt_system_boot.build);
      // Extract unique ID from BT Address.
      sc = sl_bt_system_get_identity_address(&address, &address_type);
      app_assert(sc == SL_STATUS_OK,
                 "[E: 0x%04x] Failed to get Bluetooth address\n",
                 (int)sc);
      app_log("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X\n",
              address_type ? "static random" : "public device",
              address.addr[5],
              address.addr[4],
              address.addr[3],
              address.addr[2],
              address.addr[1],
              address.addr[0]);
      // Pad and reverse unique ID to get System ID.
      system_id[0] = address.addr[5];
      system_id[1] = address.addr[4];
      system_id[2] = address.addr[3];
      system_id[3] = 0xFF;
      system_id[4] = 0xFE;
      system_id[5] = address.addr[2];
      system_id[6] = address.addr[1];
      system_id[7] = address.addr[0];
      // Write System ID attribute
      sc = sl_bt_gatt_server_write_attribute_value(GATTDB_SYSTEM_ID,
                                                   0,
                                                   sizeof(system_id),
                                                   system_id);
      app_assert(sc == SL_STATUS_OK,
                 "[E: 0x%04x] Failed to write attribute\n",
                 (int)sc);
      // Create an advertising set.
      sc = sl_bt_advertiser_create_set(&advertising_set_handle);
      app_assert(sc == SL_STATUS_OK,
                 "[E: 0x%04x] Failed to create advertising set\n",
                 (int)sc);
      // Set advertising interval to 100ms.
      sc = sl_bt_advertiser_set_timing(
        advertising_set_handle,
        160, // min. adv. interval (milliseconds * 1.6)
        160, // max. adv. interval (milliseconds * 1.6)
        0,   // adv. duration
        0);  // max. num. adv. events
      app_assert(sc == SL_STATUS_OK,
                 "[E: 0x%04x] Failed to set advertising timing\n",
                 (int)sc);
      // Start general advertising and enable connections.
      sc = sl_bt_advertiser_start(
        advertising_set_handle,
        advertiser_general_discoverable,
        advertiser_connectable_scannable);
      app_assert(sc == SL_STATUS_OK,
                 "[E: 0x%04x] Failed to start advertising\n",
                 (int)sc);
      app_log("Started advertising\n");
      break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      app_log("Connection opened\n");
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      app_log("Connection closed\n");
      // Restart advertising after client has disconnected.
      sc = sl_bt_advertiser_start(
        advertising_set_handle,
        advertiser_general_discoverable,
        advertiser_connectable_scannable);
      app_assert(sc == SL_STATUS_OK,
                 "[E: 0x%04x] Failed to start advertising\n",
                 (int)sc);
      app_log("Started advertising\n");
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////

    // -------------------------------
    // Default event handler.
    default:
      break;
  }
}

/**************************************************************************//**
 * UART TX Wrapper.
 *****************************************************************************/
static void uart_tx_wrapper(uint32_t len, uint8_t *data)
{
  if (0 > uartTx(len, data)) {
    app_log("Failed to write to serial port\n");
    exit(EXIT_FAILURE);
  }
}

/**************************************************************************//**
 * Initialise serial port.
 *****************************************************************************/
static int serial_port_init(int argc, char *argv[], int32_t timeout)
{
  char *uartPort = DEFAULT_UART_PORT;
  uint32_t uartBaudRate = DEFAULT_UART_BAUD_RATE;
  uint32_t uartFlowControl = DEFAULT_UART_FLOW_CONTROL;
  // Handle the command-line arguments.
  switch (argc) {
    case 4:
      uartFlowControl = atoi(argv[3]);
    // Falls through on purpose.
    case 3:
      uartBaudRate = atoi(argv[2]);
      uartPort = argv[1];
    // Falls through on purpose.
    default:
      break;
  }
  if (!uartPort || !uartBaudRate || (uartFlowControl > 1)) {
    app_log(USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }
  // Initialise the serial port with RTS/CTS enabled.
  return uartOpen((int8_t*)uartPort, uartBaudRate, uartFlowControl, timeout);
}
