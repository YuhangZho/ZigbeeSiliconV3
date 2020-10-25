/***************************************************************************//**
 * @brief Adaptation for running Bluetooth in RTOS
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

#ifndef RTOS_BLUETOOTH_H
#define RTOS_BLUETOOTH_H

#include <kernel/include/os.h>
#include "sl_bt_types.h"
#include "sl_bt_api.h"

//Bluetooth event flag group
extern OS_FLAG_GRP bluetooth_event_flags;
//Bluetooth event flag definitions
#define SL_BT_RTOS_EVENT_FLAG_STACK       ((OS_FLAGS)1)    //Bluetooth task needs an update
#define SL_BT_RTOS_EVENT_FLAG_LL          ((OS_FLAGS)2)    //Linklayer task needs an update
#define SL_BT_RTOS_EVENT_FLAG_CMD_WAITING ((OS_FLAGS)4)    //BGAPI command is waiting to be processed
#define SL_BT_RTOS_EVENT_FLAG_RSP_WAITING ((OS_FLAGS)8)    //BGAPI response is waiting to be processed
#define SL_BT_RTOS_EVENT_FLAG_EVT_WAITING ((OS_FLAGS)16)   //BGAPI event is waiting to be processed
#define SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED ((OS_FLAGS)32)   //BGAPI event is handled

//hooks for API
//called from tasks using BGAPI
void sli_bt_cmd_handler_rtos_delegate(uint32_t header, sl_bt_handler handler, const void* payload);

//Bluetooth event data pointer
extern sl_bt_msg_t*  bluetooth_evt;

//Mutex functions for using Bluetooth from multiple tasks
void BluetoothPend(RTOS_ERR *err);
void BluetoothPost(RTOS_ERR *err);

// Initialize Bluetooth for running in RTOS.
void sl_bt_rtos_init();

/**
 * Internal stack function to get how long the Bluetooth stack can sleep.
 *
 * @return 0 if the stack cannot sleep;
 * Maximum value of uint32_t if the stack has no task scheduled to process;
 * The ticks (in sleeptimer frequency) the stack needs to wake up to process a task
 */
uint32_t sli_bt_can_sleep_ticks();

#endif //RTOS_BLUETOOTH_H
