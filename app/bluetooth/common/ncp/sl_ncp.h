/***************************************************************************//**
 * @file
 * @brief Bluetooth Network Co-Processor (NCP) Interface
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

#ifndef SL_NCP_H
#define SL_NCP_H

#include <stdint.h>
#include "sl_status.h"
#include "sl_ncp_config.h"

/**************************************************************************//**
 * Ncp Initialization function.
 *****************************************************************************/
void sl_ncp_init(void);

/**************************************************************************//**
 * Ncp process action function.
 *****************************************************************************/
void sl_ncp_step(void);

/**************************************************************************//**
 * User command handler callback.
 *****************************************************************************/
void sl_ncp_user_command_cb(void *data);

/**************************************************************************//**
 * Send NCP user command response
 *****************************************************************************/
void sl_ncp_user_command_rsp(sl_status_t result, uint8_t len, uint8_t *data);

/**************************************************************************//**
 * Send NCP user event response
 *****************************************************************************/
void sl_ncp_user_evt(uint8_t len, uint8_t *data);

#endif // SL_NCP_H
