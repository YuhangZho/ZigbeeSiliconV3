/***************************************************************************//**
 * @brief Adaptation for running Bluetooth in RTOS
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

#ifndef FREERTOS_BLUETOOTH_H
#define FREERTOS_BLUETOOTH_H

#if __cplusplus
extern "C" {
#endif

#include "sl_bt_types.h"
#include "sl_bt_api.h"
  
#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"

// Bluetooth event flag group
extern EventGroupHandle_t bluetooth_event_flags;
// Bluetooth event flag definitions
#define SL_BT_RTOS_EVENT_FLAG_STACK (0x01)       // Bluetooth task needs an update
#define SL_BT_RTOS_EVENT_FLAG_LL (0x02)          // Linklayer task needs an update
#define SL_BT_RTOS_EVENT_FLAG_CMD_WAITING (0x04) // BGAPI command is waiting to be processed
#define SL_BT_RTOS_EVENT_FLAG_RSP_WAITING (0x08) // BGAPI response is waiting to be processed
#define SL_BT_RTOS_EVENT_FLAG_EVT_WAITING (0x10) // BGAPI event is waiting to be processed
#define SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED (0x20) // BGAPI event is handled

//hooks for API
//called from tasks using BGAPI
void sli_bt_cmd_handler_rtos_delegate(uint32_t header, sl_bt_handler handler, const void *payload);

//Bluetooth event data pointer
extern sl_bt_msg_t*  bluetooth_evt;
  
// Mutex functions for using Bluetooth from multiple tasks
void BluetoothPend(void);
void BluetoothPost(void);

void sl_bt_rtos_init();
void sl_bt_event_handler_tick(void);

EventBits_t vRaiseEventFlagBasedOnContext(EventGroupHandle_t xEventGroup,
                                          EventBits_t        uxBitsToWaitFor,
                                          BaseType_t *       pxHigherPriorityTaskWoken);
EventBits_t vSendToQueueBasedOnContext(QueueHandle_t xQueue,
                                       void *        xItemToQueue,
                                       TickType_t    xTicksToWait,
                                       BaseType_t *  pxHigherPriorityTaskWoken);

#if __cplusplus
}
#endif

#endif // FREERTOS_BLUETOOTH_H
