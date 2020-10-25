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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <em_device.h>
#include <kernel/include/os.h>
#include  <common/include/rtos_utils.h>

#include "sl_bluetooth.h"
#include "sl_bt_stack_config.h"
#include "sl_bt_rtos_adaptation.h"

#ifdef CONFIGURATION_HEADER
#include CONFIGURATION_HEADER
#endif // CONFIGURATION_HEADER

#define  SL_BT_RTOS_LL_PRIORITY                        3u
#define  SL_BT_RTOS_STACK_PRIORITY                     4u
#define  SL_BT_RTOS_EVENT_HANDLER_PRIORITY             5u

void sli_bt_cmd_handler_delegate(uint32_t header, sl_bt_handler, const void*);

void BluetoothUpdate();
static volatile  sl_bt_msg_t bluetooth_evt_instance;
sl_bt_msg_t*  bluetooth_evt = (sl_bt_msg_t*) &bluetooth_evt_instance;
OS_MUTEX           BluetoothMutex;

static  volatile  uint32_t command_header;
static  volatile  void*    command_data;
static  volatile  sl_bt_handler command_handler_func = NULL;
//Bluetooth Event task
#ifndef BLUETOOTH_EVENT_STACK_SIZE
#define BLUETOOTH_EVENT_STACK_SIZE (1000 / sizeof(CPU_STK))
#endif
#ifndef SL_BT_DISABLE_EVENT_TASK
static  void  sl_bt_event_handler_task (void  *p_arg);
static  OS_TCB            BluetoothEventTaskTCB;
static  CPU_STK           BluetoothEventTaskStk[BLUETOOTH_EVENT_STACK_SIZE];
#endif
//Bluetooth task
#ifndef BLUETOOTH_STACK_SIZE
#define BLUETOOTH_STACK_SIZE (2000 / sizeof(CPU_STK))
#endif
static  void  BluetoothTask (void  *p_arg);
static  OS_TCB            BluetoothTaskTCB;
static  CPU_STK           BluetoothTaskStk[BLUETOOTH_STACK_SIZE];

//Linklayer task
#ifndef LINKLAYER_STACK_SIZE
#define LINKLAYER_STACK_SIZE (1000 / sizeof(CPU_STK))
#endif
static  void  LinklayerTask (void  *p_arg);
static  OS_TCB            LinklayerTaskTCB;
static  CPU_STK           LinklayerTaskStk[LINKLAYER_STACK_SIZE];

OS_FLAG_GRP bluetooth_event_flags;

void sl_bt_rtos_init()
{
  RTOS_ERR os_err;

  OSFlagCreate(&bluetooth_event_flags,
               "Bluetooth Flags",
               (OS_FLAGS)0,
               &os_err);
  OSMutexCreate(&BluetoothMutex,
                "Bluetooth Mutex",
                &os_err);
  //create task for Bluetooth host stack
  OSTaskCreate(&BluetoothTaskTCB,
               "Bluetooth Task",
               BluetoothTask,
               0u,
               SL_BT_RTOS_STACK_PRIORITY,
               &BluetoothTaskStk[0u],
               BLUETOOTH_STACK_SIZE / 10u,
               BLUETOOTH_STACK_SIZE,
               0u,
               0u,
               0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &os_err);
}
static void sl_bt_rtos_create_tasks()
{
  RTOS_ERR os_err;

  //create tasks for Linklayer

  OSTaskCreate(&LinklayerTaskTCB,
               "Linklayer Task",
               LinklayerTask,
               0u,
               SL_BT_RTOS_LL_PRIORITY,
               &LinklayerTaskStk[0u],
               LINKLAYER_STACK_SIZE / 10u,
               LINKLAYER_STACK_SIZE,
               0u,
               0u,
               0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &os_err);

  //create tasks for Bluetooth event handler
#ifndef SL_BT_DISABLE_EVENT_TASK
  OSTaskCreate(&BluetoothEventTaskTCB,
               "Bluetooth Event Task",
               sl_bt_event_handler_task,
               0u,
               SL_BT_RTOS_EVENT_HANDLER_PRIORITY,
               &BluetoothEventTaskStk[0u],
               BLUETOOTH_EVENT_STACK_SIZE / 10u,
               BLUETOOTH_EVENT_STACK_SIZE,
               0u,
               0u,
               0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &os_err);
#endif
}

//This callback is called from interrupt context (Kernel Aware)
//sets flag to trigger Link Layer Task
void sli_bt_rtos_ll_callback()
{
  RTOS_ERR os_err;
  OSFlagPost(&bluetooth_event_flags, (OS_FLAGS)SL_BT_RTOS_EVENT_FLAG_LL, OS_OPT_POST_FLAG_SET, &os_err);
}
//This callback is called from Bluetooth stack
//Called from kernel aware interrupt context (RTCC interrupt) and from Bluetooth task
//sets flag to trigger running Bluetooth stack
void sli_bt_rtos_stack_callback()
{
  RTOS_ERR os_err;
  OSFlagPost(&bluetooth_event_flags, (OS_FLAGS)SL_BT_RTOS_EVENT_FLAG_STACK, OS_OPT_POST_FLAG_SET, &os_err);
}
//Bluetooth task, it waits for events from bluetooth and handles them
void BluetoothTask(void *p_arg)
{
  (void)p_arg;

  RTOS_ERR      os_err;
  OS_FLAGS      flags = SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED;

  sl_bt_init();

  sli_bt_set_cmd_handler_delegate(sli_bt_cmd_handler_rtos_delegate);
  sl_bt_rtos_create_tasks();
  while (DEF_TRUE) {
    //Command needs to be sent to Bluetooth stack
    if (flags & SL_BT_RTOS_EVENT_FLAG_CMD_WAITING) {
      uint32_t header = command_header;
      sl_bt_handler cmd_handler = command_handler_func;
      sli_bt_cmd_handler_delegate(header, cmd_handler, (void*)command_data);
      command_handler_func = NULL;
      flags &= ~SL_BT_RTOS_EVENT_FLAG_CMD_WAITING;
      OSFlagPost(&bluetooth_event_flags, (OS_FLAGS)SL_BT_RTOS_EVENT_FLAG_RSP_WAITING, OS_OPT_POST_FLAG_SET, &os_err);
    }

    //Bluetooth stack needs updating, and evt can be used
    if (sl_bt_event_pending() && (flags & SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED)) {  //update bluetooth & read event
      sl_status_t status = sl_bt_pop_event(bluetooth_evt);
      APP_RTOS_ASSERT_DBG(status != SL_STATUS_INVALID_PARAMETER, 1);
      OSFlagPost(&bluetooth_event_flags, (OS_FLAGS)SL_BT_RTOS_EVENT_FLAG_EVT_WAITING, OS_OPT_POST_FLAG_SET, &os_err);
      flags &= ~SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED;
    }

    uint32_t timeout = sli_bt_can_sleep_ticks();
    if (timeout == 0 && (flags & SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED)) {
      continue;
    }
    flags |= OSFlagPend(&bluetooth_event_flags,
                        (OS_FLAGS)SL_BT_RTOS_EVENT_FLAG_STACK + SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED + SL_BT_RTOS_EVENT_FLAG_CMD_WAITING,
                        0,
                        OS_OPT_PEND_BLOCKING + OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME,
                        NULL,
                        &os_err);
    // flag_stack is used to wakeup from pend and then sl_bt_event_pending() is used to check if event is queued
    // even if event stays in queue and task again sleeps, it is woke up by evt_handled and then it can be processed.
  }
}

static  void  LinklayerTask(void *p_arg)
{
  RTOS_ERR      os_err;
  (void)p_arg;

  while (DEF_TRUE) {
    OSFlagPend(&bluetooth_event_flags, (OS_FLAGS)SL_BT_RTOS_EVENT_FLAG_LL,
               0,
               OS_OPT_PEND_BLOCKING + OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME,
               NULL,
               &os_err);

    sl_bt_priority_handle();
  }
}
// Event task, this calls the application code
#ifndef SL_BT_DISABLE_EVENT_TASK
void  sl_bt_event_handler_task(void  *p_arg)
{
  RTOS_ERR      os_err;
  (void)p_arg;

  while (DEF_TRUE) {
    OSFlagPend(&bluetooth_event_flags, (OS_FLAGS)SL_BT_RTOS_EVENT_FLAG_EVT_WAITING,
               0,
               OS_OPT_PEND_BLOCKING + OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME,
               NULL,
               &os_err);
    sl_bt_process_event((sl_bt_msg_t*)bluetooth_evt);

    OSFlagPost(&bluetooth_event_flags, (OS_FLAGS)SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED, OS_OPT_POST_FLAG_SET, &os_err);
  }
}
#endif
//hooks for API
//called from tasks using BGAPI
void sli_bt_cmd_handler_rtos_delegate(uint32_t header, sl_bt_handler handler, const void* payload)
{
  RTOS_ERR os_err;

  command_header = header;
  command_handler_func = handler;
  command_data = (void*)payload;
  //Command structure is filled, notify the stack
  OSFlagPost(&bluetooth_event_flags, (OS_FLAGS)SL_BT_RTOS_EVENT_FLAG_CMD_WAITING, OS_OPT_POST_FLAG_SET, &os_err);
  //wait for response
  OSFlagPend(&bluetooth_event_flags, (OS_FLAGS)SL_BT_RTOS_EVENT_FLAG_RSP_WAITING,
             0,
             OS_OPT_PEND_BLOCKING + OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME,
             NULL,
             &os_err);
}

void BluetoothPend(RTOS_ERR *err)
{
  OSMutexPend((OS_MUTEX *)&BluetoothMutex,
              (OS_TICK   )0,
              (OS_OPT    )OS_OPT_PEND_BLOCKING,
              (CPU_TS   *)NULL,
              (RTOS_ERR *)err);
}
void BluetoothPost(RTOS_ERR *err)
{
  OSMutexPost((OS_MUTEX *)&BluetoothMutex,
              (OS_OPT    )OS_OPT_POST_NONE,
              (RTOS_ERR *)err);
}
