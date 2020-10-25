/***************************************************************************//**
 * @file
 * @brief Configuration header file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "log.h"
#include "sl_bt_types.h"

/***************************************************************************************************
 * Type Definitions
 **************************************************************************************************/

/** Supported sample rates */
typedef enum {
  sr_8k = 8,
  sr_16k = 16,
}adc_sample_rate_t;

/** Configuration structure */
typedef struct {
  uint32_t baud_rate;                 /**< UART baud rate */
  char *uart_port;                    /**< UART port */
  char *out_file_name;                /**< Output file name */
  bool output_to_stdout;              /**< Audio stream to stdout instead of file */
  char *transfer_status_file_name;    /**< Transfer status file name */
  bool audio_data_notification;       /**< Enable/Disable audio data notification*/
  adc_sample_rate_t adc_sample_rate;  /**< Sample rate*/
  bool filter_enabled;                /**< Enable/Disable filtering*/
  bool encoding_enabled;              /**< Enable/Disable encoding*/
  bool transfer_status;               /**< Enable/Disable transfet status flag*/
  bool remote_address_set;            /**< Remote Device address given as application parameter */
  bd_addr remote_address;             /**< Remote Device address */
}configuration_t;

#define DEFAULT_UART_BAUD_RATE   115200
#define DEFAULT_OUTPUT_FILE_NAME "-"
#define IMA_FILE_EXTENSION ".ima"
#define S8_FILE_EXTENSION ".s8"
#define S16_FILE_EXTENSION ".s16"
#define DEFAULT_TRANSFER_STATUS_FILE_NAME "transfer_status.log"

#define SERVICE_VOICE_OVER_BLE_UUID { 0x10, 0x0ad, 0xb3, 0x9e, 0x42, 0xdf, 0xd3, 0x93, 0x62, 0x43, 0x2e, 0xdc, 0x93, 0x11, 0xef, 0xb7 }

/*  UUIDs of the characteristics*/
#define CHAR_AUDIO_DATA_UUID      { 0xf2, 0xc5, 0xfd, 0x27, 0xec, 0x81, 0x3e, 0x94, 0x3d, 0x47, 0x08, 0xec, 0x72, 0x7a, 0xce, 0x00 }
#define CHAR_SAMPLE_RATE_UUID     { 0x01, 0xc6, 0xfd, 0x27, 0xec, 0x81, 0x3e, 0x94, 0x3d, 0x47, 0x08, 0xec, 0x72, 0x7a, 0xce, 0x00 }
#define CHAR_FILTER_ENABLE_UUID   { 0x02, 0xc6, 0xfd, 0x27, 0xec, 0x81, 0x3e, 0x94, 0x3d, 0x47, 0x08, 0xec, 0x72, 0x7a, 0xce, 0x00 }
#define CHAR_ENCODING_ENABLE_UUID { 0x03, 0xc6, 0xfd, 0x27, 0xec, 0x81, 0x3e, 0x94, 0x3d, 0x47, 0x08, 0xec, 0x72, 0x7a, 0xce, 0x00 }
#define CHAR_TRANSFER_STATUS_UUID { 0x04, 0xc6, 0xfd, 0x27, 0xec, 0x81, 0x3e, 0x94, 0x3d, 0x47, 0x08, 0xec, 0x72, 0x7a, 0xce, 0x00 }

/**  Default configuration*/
#define DEFAULT_CONFIGURATION                                                        \
  {                                                                                  \
    DEFAULT_UART_BAUD_RATE,            /** The default baud rate to use. */          \
    NULL,                              /** No default port */                        \
    DEFAULT_OUTPUT_FILE_NAME,          /** The default output file name. */          \
    false,                             /** Audio streaming to file. */               \
    DEFAULT_TRANSFER_STATUS_FILE_NAME, /** The default transfer status file name. */ \
    true,                              /** Audio Data notification enabled */        \
    sr_16k,                            /** The default ADC sample rate */            \
    false,                             /** Enable filtering */                       \
    true,                              /** Enable encoding */                        \
    true,                              /** Enable transfer status */                 \
  }

/** SW Timer events definition */
#define EVT_CONNECT         0x1
#define EVT_SCANNING        0x2
#define EVT_SCAN_TIMEOUT    0x3

/** SW timer intervals */
#define SW_TIMER_10_MS   (328)
#define SW_TIMER_20_MS   (SW_TIMER_10_MS * 2)
#define SW_TIMER_50_MS   (SW_TIMER_10_MS * 5)

#define DEFAULT_SCAN_TIMEOUT 10 * 20 /** 10 seconds */

/** Error macro */
#define ERROR_EXIT(...) do { app_log(__VA_ARGS__); exit(EXIT_FAILURE); } while (0)

/** Debug macros */
#define DEBUG_INFO(...)     do { app_log("[Inf] "); app_log(__VA_ARGS__); fflush(stdout); } while (0)
#define DEBUG_WARNING(...)  do { app_log("[War] "); app_log(__VA_ARGS__); fflush(stdout); } while (0)
#define DEBUG_ERROR(...)    do { app_log("[Err] "); app_log(__VA_ARGS__); fflush(stdout); } while (0)
#define DEBUG_SUCCESS(...)  do { app_log("[Ok ] "); app_log(__VA_ARGS__); app_log("\n"); fflush(stdout); } while (0)

configuration_t *CONF_get(void);

#endif /* CONFIG_H_ */
