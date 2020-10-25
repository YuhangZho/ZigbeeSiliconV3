/***************************************************************************//**
 * @file
 * @brief Simple Communication Interface (UART)
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

#ifndef SL_SIMPLE_COM_H
#define SL_SIMPLE_COM_H

#include <stdint.h>
#include "sl_status.h"
#include "sl_simple_com_config.h"

#define SL_SIMPLE_COM_UARTDRV_INSTANCE_VCOM    0
#define SL_SIMPLE_COM_UARTDRV_INSTANCE_EXP     1

/**************************************************************************//**
 * Simple Comm Init.
 *****************************************************************************/
void sl_simple_com_init(void);

/**************************************************************************//**
 * Uart transmit function
 *****************************************************************************/
void sl_simple_com_transmit(uint32_t len, uint8_t *data);

/**************************************************************************//**
 * Uart transmit completed callback
 *****************************************************************************/
void sl_simple_com_transmit_cb(sl_status_t status);

/**************************************************************************//**
 * Uart receive function
 *****************************************************************************/
void sl_simple_com_receive(void);

/**************************************************************************//**
 * Uart receive completed callback
 *****************************************************************************/
void sl_simple_com_receive_cb(sl_status_t status, uint32_t len, uint8_t *data);

#endif // SL_SIMPLE_COM_H
