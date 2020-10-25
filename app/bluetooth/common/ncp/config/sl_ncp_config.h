/***************************************************************************//**
 * @file
 * @brief Bluetooth Network Co-Processor (NCP) Interface Configuration
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

#ifndef SL_NCP_CONFIG_H
#define SL_NCP_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <o SL_NCP_CMD_BUF_SIZE> Command buffer size (bytes) <360-1024>
// <i> Default: 512
// <i> Define the size of Bluetooth NCP command buffer in bytes.
#define SL_NCP_CMD_BUF_SIZE     (512)

// <o SL_NCP_EVT_BUF_SIZE> Event buffer size (bytes) <512-4096>
// <i> Default: 1024
// <i> Define the size of Bluetooth NCP event buffer in bytes.
#define SL_NCP_EVT_BUF_SIZE     (1024)

// <o SL_NCP_CMD_TIMEOUT_MS> Command timeout (ms) <0-10000>
// <i> Default: 500
// <i> Allowed timeout in ms for command reception before triggering error.
#define SL_NCP_CMD_TIMEOUT_MS   (500)
// <<< end of configuration section >>>

#endif // SL_NCP_CONFIG_H
