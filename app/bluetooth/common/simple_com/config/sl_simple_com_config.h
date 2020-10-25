/***************************************************************************//**
 * @file
 * @brief Simple Communication Interface (UART) Configuration
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

#ifndef SL_SIMPLE_COM_CONFIG_H
#define SL_SIMPLE_COM_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <o SL_SIMPLE_COM_UARTDRV_INSTANCE> UARTDRV instance selector
//   <SL_SIMPLE_COM_UARTDRV_INSTANCE_VCOM=> VCOM
//   <SL_SIMPLE_COM_UARTDRV_INSTANCE_EXP=> EXP
// <i> Select UARTDRV instance to be used
// <i> Default: SL_SIMPLE_COM_UARTDRV_INSTANCE_VCOM
#define SL_SIMPLE_COM_UARTDRV_INSTANCE   SL_SIMPLE_COM_UARTDRV_INSTANCE_VCOM

// <o SL_SIMPLE_COM_RX_BUF_SIZE> Receive buffer size (bytes) <360-1024>
// <i> Default: 512
// <i> Define the size of the receive buffer in bytes.
#define SL_SIMPLE_COM_RX_BUF_SIZE        (512)

// <o SL_SIMPLE_COM_TX_BUF_SIZE> Transmit buffer size (bytes) <512-4096>
// <i> Default: 1024
// <i> Define the size of the transmit buffer in bytes.
#define SL_SIMPLE_COM_TX_BUF_SIZE        (1024)

// <<< end of configuration section >>>

#endif // SL_SIMPLE_COM_CONFIG_H
