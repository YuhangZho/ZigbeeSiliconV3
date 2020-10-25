/***************************************************************************//**
 * @file heartbeat.c
 * @brief heartbeat examples functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "heartbeat_task_config.h"
#include "heartbeat_led_config.h"
#include "sl_simple_led.h"
#include "sl_simple_led_instances.h"
#include "os.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#define CONCAT_PASTER(first, second)        (first ##  second)
#define LED_INSTANCE(instance_name)        CONCAT_PASTER(sl_, instance_name)

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static OS_TCB tcb;
static CPU_STK stack[HEARTBEAT_TASK_STACK_SIZE];

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/
static void heartbeat_task(void *arg);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize Heartbeat.
 ******************************************************************************/
void heartbeat_init(void)
{
  RTOS_ERR err;

  OSTaskCreate(&tcb,
               "heartbeat task",
               heartbeat_task,
               DEF_NULL,
               HEARTBEAT_TASK_PRIO,
               &stack[0],
               (HEARTBEAT_TASK_STACK_SIZE / 10u),
               HEARTBEAT_TASK_STACK_SIZE,
               0u,
               0u,
               DEF_NULL,
               (OS_OPT_TASK_STK_CLR),
               &err);
  EFM_ASSERT((RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE));
}

/***************************************************************************//**
 * Heartbeat task.
 ******************************************************************************/
static void heartbeat_task(void *arg)
{
  (void)&arg;
  do {
    OS_TICK os_ticks;
    RTOS_ERR err;

    os_ticks = (OSCfg_TickRate_Hz * HEARTBEAT_TOOGLE_DELAY_MS) / 1000;
    OSTimeDly(os_ticks, OS_OPT_TIME_DLY, &err);

    sl_led_toggle(&LED_INSTANCE(HEARTBEAT_LED_INSTANCE));
  } while (1);
}
