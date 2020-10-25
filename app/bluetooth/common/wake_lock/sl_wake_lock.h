/***************************************************************************//**
 * @file
 * @brief Wake and sleep functionality
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

#ifndef SL_WAKE_LOCK_H
#define SL_WAKE_LOCK_H

#include "sl_wake_lock_config.h"

/**************************************************************************//**
 * Wake and sleep (lock) initialization function
 *****************************************************************************/
void sl_wake_lock_init(void);

/**************************************************************************//**
 * Signal wake-up to remote controller.
 *****************************************************************************/
void sl_wake_lock_set_remote_req(void);

/**************************************************************************//**
 * Signal go to sleep (lock) to remote controller.
 *****************************************************************************/
void sl_wake_lock_clear_remote_req(void);

/**************************************************************************//**
 * Wake-up signal arrived from remote controller.
 *****************************************************************************/
void sl_wake_lock_set_req_rx_cb(void);

/**************************************************************************//**
 * Go to sleep (lock) signal arrived from remote controller.
 *****************************************************************************/
void sl_wake_lock_clear_req_rx_cb(void);

/**************************************************************************//**
 * Enable sleep mode locally.
 *****************************************************************************/
void sl_wake_lock_set_local(void);

/**************************************************************************//**
 * Disable sleep mode locally.
 *****************************************************************************/
void sl_wake_lock_clear_local(void);

#endif // SL_WAKE_LOCK_H
