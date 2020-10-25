/***************************************************************************//**
 * @file
 * @brief Bluetooth Network Co-Processor (NCP) Host Communication Interface
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
#ifndef SL_NCP_HOST_COM_H
#define SL_NCP_HOST_COM_H

#include "sl_ncp_host_com_config.h"

/**************************************************************************//**
 * NCP host communication initialization.
 *****************************************************************************/
void sl_ncp_host_com_init(void);

/**************************************************************************//**
 * Transmit function
 *****************************************************************************/
void sl_ncp_host_com_write(uint32_t len, uint8_t *data);

/**************************************************************************//**
 * Receive function
 *****************************************************************************/
int32_t sl_ncp_host_com_read(uint32_t len, uint8_t *data);

/**************************************************************************//**
 * Gives back already received message length.
 *****************************************************************************/
int32_t sl_ncp_host_com_peek(void);

bool sl_ncp_host_is_ok_to_sleep(void);
#endif // SL_NCP_HOST_COM_H
