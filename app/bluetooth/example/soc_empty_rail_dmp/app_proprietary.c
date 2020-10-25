/***************************************************************************//**
 * @file
 * @brief Core proprietary application logic.
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
#include "os.h"
#include "rail.h"
#include "sl_rail_util_init.h"
#include "em_common.h"
#include "sl_app_assert.h"
#include "app_proprietary.h"

// OS task parameters
#define APP_PROPRIETARY_TASK_PRIO         6u
#define APP_PROPRIETARY_TASK_STACK_SIZE   (1024 / sizeof(CPU_STK))

// OS task variables
static CPU_STK app_proprietary_task_stack[APP_PROPRIETARY_TASK_STACK_SIZE];
static OS_TCB app_proprietary_task_tcb;

// OS event to prevent cyclic execution of the task main loop.
OS_FLAG_GRP app_proprietary_event_flags;

/**************************************************************************//**
 * Proprietary application task.
 *
 * @param[in] p_arg Unused parameter required by the OS API.
 *****************************************************************************/
static void app_proprietary_task(void *p_arg);

/**************************************************************************//**
 * Proprietary application init.
 *****************************************************************************/
void app_proprietary_init()
{
  RTOS_ERR err;

  // Create the Proprietary Application task.
  OSTaskCreate(&app_proprietary_task_tcb,
               "App Proprietary Task",
               app_proprietary_task,
               0u,
               APP_PROPRIETARY_TASK_PRIO,
               &app_proprietary_task_stack[0u],
               (APP_PROPRIETARY_TASK_STACK_SIZE / 10u),
               APP_PROPRIETARY_TASK_STACK_SIZE,
               0u,
               0u,
               0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &err);
  sl_app_assert(err.Code == RTOS_ERR_NONE,
                "[E: 0x%04x] Task creation failed\n",
                (int)err.Code);

  // Initialize the flag group for the proprietary task.
  OSFlagCreate(&app_proprietary_event_flags, "Prop. flags", (OS_FLAGS)0, &err);
  sl_app_assert(err.Code == RTOS_ERR_NONE,
                "[E: 0x%04x] Event flag creation failed\n",
                (int)err.Code);
}

static void app_proprietary_task(void *p_arg)
{
  PP_UNUSED_PARAM(p_arg);
  RAIL_Handle_t rail_handle;
  RAIL_Status_t status;
  RTOS_ERR err;

  rail_handle = sl_rail_util_get_handle();
  // Start reception.
  status = RAIL_StartRx(rail_handle, 0, NULL);
  sl_app_assert(status == RAIL_STATUS_NO_ERROR,
                "[E: 0x%04x] Failed to start RAIL reception\n",
                (int)status);

  // Start task main loop.
  while (DEF_TRUE) {
    // Wait for the event flag to be set.
    OSFlagPend(&app_proprietary_event_flags,
               APP_PROPRIETARY_EVENT_FLAG,
               (OS_TICK)0,
               OS_OPT_PEND_BLOCKING       \
               + OS_OPT_PEND_FLAG_SET_ANY \
               + OS_OPT_PEND_FLAG_CONSUME,
               NULL,
               &err);
    sl_app_assert(err.Code == RTOS_ERR_NONE,
                  "[E: 0x%04x] Prop event flag pend error\n",
                  (int)err.Code);

    ///////////////////////////////////////////////////////////////////////////
    // Put your additional application code here!                            //
    // This is called when the event flag is set with OSFlagPost.            //
    ///////////////////////////////////////////////////////////////////////////
  }
}

/**************************************************************************//**
 * This callback is called on registered RAIL events.
 * Overrides dummy weak implementation.
 *****************************************************************************/
void sl_rail_app_on_event(RAIL_Handle_t rail_handle,
                          RAIL_Events_t events)
{
  PP_UNUSED_PARAM(rail_handle);
  PP_UNUSED_PARAM(events);

  /////////////////////////////////////////////////////////////////////////////
  // Add event handlers here as your application requires!                   //
  /////////////////////////////////////////////////////////////////////////////
}
