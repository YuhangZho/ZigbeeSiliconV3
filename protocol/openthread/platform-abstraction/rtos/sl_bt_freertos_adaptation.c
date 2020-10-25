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

#include <stdint.h>
#include <string.h>
#include <em_device.h>

#include "sl_bluetooth.h"
#include "sl_bt_stack_config.h"
#include "sl_bt_rtos_adaptation.h"

#ifdef CONFIGURATION_HEADER
#include CONFIGURATION_HEADER
#endif // CONFIGURATION_HEADER

#define  SL_BT_RTOS_LL_PRIORITY                        5u
#define  SL_BT_RTOS_STACK_PRIORITY                     4u
#define  SL_BT_RTOS_EVENT_HANDLER_PRIORITY             3u

EventBits_t vRaiseEventFlagBasedOnContext(EventGroupHandle_t xEventGroup,
                                          EventBits_t        uxBitsToWaitFor,
                                          BaseType_t *       pxHigherPriorityTaskWoken);

void sli_bt_cmd_handler_delegate(uint32_t header, sl_bt_handler, const void*);

void BluetoothUpdate();
static volatile  sl_bt_msg_t bluetooth_evt_instance;
sl_bt_msg_t*  bluetooth_evt = (sl_bt_msg_t*) &bluetooth_evt_instance;
SemaphoreHandle_t BluetoothMutex = NULL;

static volatile uint32_t            command_header;
static volatile void *              command_data;
static volatile sl_bt_handler       command_handler_func = NULL;
//Bluetooth Event task
#ifndef BLUETOOTH_EVENT_STACK_SIZE
#define BLUETOOTH_EVENT_STACK_SIZE (1000)
#endif
#ifndef SL_BT_DISABLE_EVENT_TASK
static  void  sl_bt_event_handler_task (void  *p_arg);
static  TaskHandle_t BluetoothEventTaskHandle = NULL;
#endif
// Bluetooth task
#ifndef BLUETOOTH_STACK_SIZE
#define BLUETOOTH_STACK_SIZE (2000)
#endif
static void         BluetoothTask(void *p_arg);
static TaskHandle_t BluetoothTaskHandle = NULL;

// Linklayer task
#ifndef LINKLAYER_STACK_SIZE
#define LINKLAYER_STACK_SIZE (1000)
#endif
static void         LinklayerTask(void *p_arg);
static TaskHandle_t LinklayerTaskHandle = NULL;
//
#define RTOS_TICK_HZ 1024
#define BLUETOOTH_TICK_HZ 32768
#define BLUETOOTH_TO_RTOS_TICK (BLUETOOTH_TICK_HZ / RTOS_TICK_HZ)

EventGroupHandle_t bluetooth_event_flags;

void sl_bt_rtos_init()
{
    bluetooth_event_flags = xEventGroupCreate();
    configASSERT(bluetooth_event_flags);

    BluetoothMutex = xSemaphoreCreateMutex();
    // create tasks for Bluetooth host stack
    xTaskCreate(BluetoothTask,                              /* Function that implements the task. */
                "Bluetooth Task",                           /* Text name for the task. */
                BLUETOOTH_STACK_SIZE / sizeof(StackType_t), /* Number of indexes in the xStack array. */
                NULL,                                       /* Parameter passed into the task. */
                SL_BT_RTOS_STACK_PRIORITY,                  /* Priority at which the task is created. */
                &BluetoothTaskHandle);                      /* Variable to hold the task's data structure. */
}

static void sl_bt_rtos_create_tasks()
{
    // create tasks for Linklayer
    xTaskCreate(LinklayerTask,                              /* Function that implements the task. */
                "Linklayer Task",                           /* Text name for the task. */
                LINKLAYER_STACK_SIZE / sizeof(StackType_t), /* Number of indexes in the xStack array. */
                NULL,                                       /* Parameter passed into the task. */
                SL_BT_RTOS_LL_PRIORITY,                     /* Priority at which the task is created. */
                &LinklayerTaskHandle);                      /* Variable to hold the task's data structure. */

    //create tasks for Bluetooth event handler
#ifndef SL_BT_DISABLE_EVENT_TASK    
    xTaskCreate(sl_bt_event_handler_task,
                "Bluetooth Event Task",
                BLUETOOTH_EVENT_STACK_SIZE / sizeof(StackType_t),
                NULL,
                SL_BT_RTOS_EVENT_HANDLER_PRIORITY,
                &BluetoothEventTaskHandle);
#endif
}

// This callback is called from interrupt context (Kernel Aware)
// sets flag to trigger Link Layer Task
void sli_bt_rtos_ll_callback()
{
    EventBits_t eventBits;
    BaseType_t  pxHigherPriorityTaskWoken = pdFALSE;
    eventBits =
        vRaiseEventFlagBasedOnContext(bluetooth_event_flags, SL_BT_RTOS_EVENT_FLAG_LL, &pxHigherPriorityTaskWoken);
    (void)eventBits;
}
// This callback is called from Bluetooth stack
// Called from kernel aware interrupt context (RTCC interrupt) and from Bluetooth task
// sets flag to trigger running Bluetooth stack
void sli_bt_rtos_stack_callback()
{
    EventBits_t eventBits;
    BaseType_t  pxHigherPriorityTaskWoken = pdFALSE;
    eventBits =
        vRaiseEventFlagBasedOnContext(bluetooth_event_flags, SL_BT_RTOS_EVENT_FLAG_STACK, &pxHigherPriorityTaskWoken);
    (void)eventBits;
}
// Bluetooth task, it waits for events from bluetooth and handles them
void BluetoothTask(void *p_arg)
{
    (void)p_arg;
    
    EventBits_t flags = SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED;
    EventBits_t eventBits;

    sl_bt_init();

    sli_bt_set_cmd_handler_delegate(sli_bt_cmd_handler_rtos_delegate);
    sl_bt_rtos_create_tasks();
    while (pdTRUE) {
        // Command needs to be sent to Bluetooth stack
        if (flags & SL_BT_RTOS_EVENT_FLAG_CMD_WAITING) {
            uint32_t header      = command_header;
            sl_bt_handler cmd_handler = command_handler_func;
            sli_bt_cmd_handler_delegate(header, cmd_handler, (void *)command_data);
            command_handler_func = NULL;
            flags &= ~SL_BT_RTOS_EVENT_FLAG_CMD_WAITING;
            eventBits = vRaiseEventFlagBasedOnContext(bluetooth_event_flags, SL_BT_RTOS_EVENT_FLAG_RSP_WAITING, NULL);
            (void)eventBits;
        }

        // Bluetooth stack needs updating, and evt can be used
        if (sl_bt_event_pending() && (flags & SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED)) {  // update bluetooth & read event
            sl_status_t status = sl_bt_pop_event(bluetooth_evt);
            configASSERT(status != SL_STATUS_INVALID_PARAMETER);
		        eventBits = vRaiseEventFlagBasedOnContext(bluetooth_event_flags, SL_BT_RTOS_EVENT_FLAG_EVT_WAITING, NULL);
            (void)eventBits;
            flags &= ~(SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED);
        }

        flags |=
            xEventGroupWaitBits(bluetooth_event_flags,
                                SL_BT_RTOS_EVENT_FLAG_STACK + SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED + SL_BT_RTOS_EVENT_FLAG_CMD_WAITING,
                                pdTRUE,        /* flags should be cleared before returning. */
                                pdFALSE,       /* Wait for all the bits to be set, not needed for single bit. */
                                portMAX_DELAY); /* Wait for maximum duration for bit to be set. With 1 ms tick,
                                                  portMAX_DELAY will result in wait of 50 days*/

        if (((flags & SL_BT_RTOS_EVENT_FLAG_STACK) == 0) && ((flags & SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED) == 0) &&
            ((flags & SL_BT_RTOS_EVENT_FLAG_CMD_WAITING) == 0)) {
            // timeout occurred, set the flag to update the Bluetooth stack
            flags |= SL_BT_RTOS_EVENT_FLAG_STACK;
        }
    }
}

static void LinklayerTask(void *p_arg)
{
    (void)p_arg;

    while (pdTRUE)
    {
        EventBits_t uxBits;

        uxBits = xEventGroupWaitBits(bluetooth_event_flags,   /* The event group being tested. */
                                     SL_BT_RTOS_EVENT_FLAG_LL, /* The bits within the event group to wait for. */
                                     pdTRUE,         /* SL_BT_RTOS_EVENT_FLAG_LL should be cleared before returning. */
                                     pdTRUE,         /* Wait for all the bits to be set, not needed for single bit. */
                                     portMAX_DELAY); /* Wait for maximum duration for bit to be set. With 1 ms tick,
                                                        portMAX_DELAY will result in wait of 50 days*/

        if (uxBits & SL_BT_RTOS_EVENT_FLAG_LL)
        {
            sl_bt_priority_handle();
        }
    }
}

void sl_bt_event_handler_tick(void)
{
        EventBits_t flags = 0;
        flags |= xEventGroupWaitBits(bluetooth_event_flags,
                                     SL_BT_RTOS_EVENT_FLAG_EVT_WAITING,
                                     pdFALSE,  /* Dont clear flags before returning */
                                     pdFALSE,  /* Any flag */
#ifdef SL_BT_DISABLE_EVENT_TASK
                                     0);       /* Don't wait */
#else
                                     portMAX_DELAY);  /* 50 days */
#endif

        if (flags & SL_BT_RTOS_EVENT_FLAG_EVT_WAITING) {
            xEventGroupClearBits(bluetooth_event_flags, SL_BT_RTOS_EVENT_FLAG_EVT_WAITING);
            sl_bt_process_event((sl_bt_msg_t*)bluetooth_evt);
            flags = vRaiseEventFlagBasedOnContext(bluetooth_event_flags, SL_BT_RTOS_EVENT_FLAG_EVT_HANDLED, NULL);
        }
}

// Event task, this calls the application code
#ifndef SL_BT_DISABLE_EVENT_TASK
void  sl_bt_event_handler_task(void  *p_arg)
{
    (void)p_arg;

    while (pdTRUE) {
      sl_bt_event_handler_tick();
    }
}
#endif
// hooks for API
// called from tasks using BGAPI
void sli_bt_cmd_handler_rtos_delegate(uint32_t header, sl_bt_handler handler, const void *payload)
{
    EventBits_t uxBits;

    command_header       = header;
    command_handler_func = handler;
    command_data         = (void *)payload;
    // Command structure is filled, notify the stack
    uxBits = vRaiseEventFlagBasedOnContext(bluetooth_event_flags, SL_BT_RTOS_EVENT_FLAG_CMD_WAITING, NULL);

    // wait for response
    uxBits = xEventGroupWaitBits(bluetooth_event_flags,            /* The event group being tested. */
                                 SL_BT_RTOS_EVENT_FLAG_RSP_WAITING, /* The bits within the event group to wait for. */
                                 pdTRUE,         /* SL_BT_RTOS_EVENT_FLAG_LL should be cleared before returning. */
                                 pdTRUE,         /* Wait for all the bits to be set, not needed for single bit. */
                                 portMAX_DELAY); /* Wait for maximum duration for bit to be set. With 1 ms tick,
                                                    portMAX_DELAY will result in wait of 50 days*/
    (void)uxBits;
}

void BluetoothPend(void)
{
    xSemaphoreTake(BluetoothMutex, portMAX_DELAY);
}
void BluetoothPost(void)
{
    xSemaphoreGive(BluetoothMutex);
}

void vApplicationMallocFailedHook(void)
{
    /* Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

    /* Force an assert. */
    configASSERT((volatile void *)NULL);
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    (void)pcTaskName;
    (void)pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */

    /* Force an assert. */
    configASSERT((volatile void *)NULL);
}

void vApplicationTickHook(void)
{
}

/*-----------------------------------------------------------*/

EventBits_t vRaiseEventFlagBasedOnContext(EventGroupHandle_t xEventGroup,
                                          EventBits_t        uxBitsToWaitFor,
                                          BaseType_t *       pxHigherPriorityTaskWoken)
{
    EventBits_t eventBits;
    BaseType_t  higherPrioTaskWoken = pdFALSE;

    if (xPortIsInsideInterrupt())
    {
        eventBits = xEventGroupSetBitsFromISR(xEventGroup, uxBitsToWaitFor, &higherPrioTaskWoken);
    }
    else
    {
        eventBits = xEventGroupSetBits(xEventGroup, uxBitsToWaitFor);
    }

    if (pxHigherPriorityTaskWoken != NULL)
    {
        *pxHigherPriorityTaskWoken = higherPrioTaskWoken;
    }
    return eventBits;
}
