/***************************************************************************//**
 * @file app.c
 * @brief Application
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

/**
 * This an example application that demonstrates Bluetooth Device Firmware Update (DFU)
 * over UART interface.
 *
 * To use this application you must have a WSTK configured into NCP mode connected to your
 * PC.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "sl_bt_api.h"
#include "sl_bt_ncp_host.h"
#include "uart.h"

SL_BT_API_DEFINE();
static void on_message_send(uint32_t msg_len, uint8_t* msg_data);

/**
 * Configurable parameters that can be modified to match the test setup.
 */

/** The serial port to use for BGAPI communication. */
static char* uart_port = NULL;
/** The baud rate to use. */
static uint32_t baud_rate = 0;

/** Usage string */
#define USAGE "Usage: %s [serial port] [baud rate] [dfu file] \n\n"

/** dfu file to upload*/
FILE *dfu_file = NULL;

#define MAX_DFU_PACKET 48
uint8_t dfu_data[MAX_DFU_PACKET];
size_t dfu_toload = 0;
size_t dfu_total = 0;
size_t current_pos = 0;

static int dfu_read_size()
{
  if (fseek(dfu_file, 0L, SEEK_END)) {
    return 1;
  }
  dfu_total = dfu_toload = ftell(dfu_file);
  if (fseek(dfu_file, 0L, SEEK_SET)) {
    return 1;
  }
  printf("Bytes to send:%d\n", (int)dfu_toload); fflush(stdout);
  return 0;
}

/**
 * Function called when a message needs to be written to the serial port.
 * @param msg_len Length of the message.
 * @param msg_data Message data, including the header.
 * @param data_len Optional variable data length.
 * @param data Optional variable data.
 */
static void on_message_send(uint32_t msg_len, uint8_t* msg_data)
{
  /** Variable for storing function return values. */
  int ret;

#if DEBUG
  printf("on_message_send()\n");
#endif /* DEBUG */

  ret = uartTx(msg_len, msg_data);
  if (ret < 0) {
    printf("on_message_send() - failed to write to serial port %s, ret: %d, errno: %d\n", uart_port, ret, errno);
    exit(EXIT_FAILURE);
  }
}

static int hw_init(int argc, char* argv[])
{
  if (argc < 4) {
    printf(USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }
  /**
   * Handle the command-line arguments.
   */

  //filename
  dfu_file = fopen(argv[3], "rb");
  if (dfu_file == NULL) {
    printf("cannot open file %s\n", argv[3]);
    exit(-1);
  }
  baud_rate = atoi(argv[2]);
  uart_port = argv[1];

  if (!uart_port || !baud_rate || !dfu_file) {
    printf(USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }

  /**
   * Initialise the serial port.
   */
  return uartOpen((int8_t*)uart_port, baud_rate, 1, 100);
}

static void sync_dfu_boot()
{
  sl_bt_msg_t evt = { 0 };
  uint8_t ret;

  printf("Syncing");
  fflush(stdout);

  do {
    printf(".");
    fflush(stdout);

    ret = sl_bt_pop_event(&evt);

    if (0 == ret) {
      switch (SL_BT_MSG_ID(evt.header)) {
        case sl_bt_evt_dfu_boot_id:
          printf("DFU OK\nBootloader version: %u (0x%x)\n", evt.data.evt_dfu_boot.version, evt.data.evt_dfu_boot.version);
          fflush(stdout);
          return;
        default:
          printf("ID:%08x\n", SL_BT_MSG_ID(evt.header));
          break;
      }
    } else {
      fflush(stdout);
      sl_bt_system_reset(1);
      sleep(1);
    }
  } while (1);
}

void upload_dfu_file()
{
  uint16_t result;
  size_t dfu_size;
  current_pos = 0;

  /** get dfu file size*/
  if (dfu_read_size()) {
    printf("Error, DFU file read failed\n");
    exit(EXIT_FAILURE);
  }

  /** move target to dfu mode*/
  sync_dfu_boot();

  /** update firmware*/
  sl_bt_dfu_flash_set_address(0);

  printf("DFU packet size:%d\n", (int)dfu_toload);
  while (dfu_toload > 0) {
    dfu_size = dfu_toload > sizeof(dfu_data) ? sizeof(dfu_data) : dfu_toload;
    if (fread(dfu_data, 1, dfu_size, dfu_file) != dfu_size) {
      printf("%d:%d:%d\n", (int)current_pos, (int)dfu_size, dfu_total);
      printf("File read failure\n");
      exit(EXIT_FAILURE);
    }

    printf("\r%d%%", (int)(100 * current_pos / dfu_total));
    fflush(stdout);

    result = sl_bt_dfu_flash_upload(dfu_size, dfu_data);
    if (result) {//error
      printf("\nError in upload 0x%.4x\n", result);
      exit(EXIT_FAILURE);
    }
    current_pos += dfu_size;
    dfu_toload -= dfu_size;
  }
  result = sl_bt_dfu_flash_upload_finish();
  if (result) {
    printf("\nError in dfu 0x%.4x", result); fflush(stdout);
  } else {
    printf("\nfinish\n"); fflush(stdout);
  }

  fclose(dfu_file);
  sl_bt_dfu_reset(0);
}
/***************************************************************************//**
 * Application initialisation.
 ******************************************************************************/
void app_init(int argc, char* argv[])
{
  SL_BT_API_INITIALIZE_NONBLOCK(on_message_send, uartRx, uartRxPeek);

  //////////////////////////////////////////
  // Add your application init code here! //
  //////////////////////////////////////////

  if (hw_init(argc, argv) < 0) {
    printf("HW init failure\n");
    exit(EXIT_FAILURE);
  }
}
/***************************************************************************//**
 * Application process actions.
 ******************************************************************************/
void app_process_action(void)
{
  //////////////////////////////////////////
  // Add your application tick code here! //
  //////////////////////////////////////////
}
