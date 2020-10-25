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

#include "heartbeat_led_config.h"
#include "sl_simple_led.h"
#include "sl_simple_led_instances.h"
#include "sleeptimer.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#define CONCAT_PASTER(first, second)        (first ##  second)
#define LED_INSTANCE(instance_name)        CONCAT_PASTER(sl_, instance_name)

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
sl_sleeptimer_timer_handle_t timer;
bool toggle_timeout = false;

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void on_timeout(sl_sleeptimer_timer_handle_t *handle,
                       void *data);

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize Heartbeat.
 ******************************************************************************/
void heartbeat_init(void)
{
  sl_sleeptimer_start_periodic_timer_ms(&timer,
                                        HEARTBEAT_TOOGLE_DELAY_MS,
                                        on_timeout, NULL,
                                        0,
                                        SL_SLEEPTIMER_NO_HIGH_PRECISION_HF_CLOCKS_REQUIRED_FLAG);
}

/***************************************************************************//**
 * Heartbeat ticking function.
 ******************************************************************************/
void heartbeat_process_action(void)
{
  if (toggle_timeout == true) {
    sl_led_toggle(&LED_INSTANCE(HEARTBEAT_LED_INSTANCE));
    toggle_timeout = false;
  }
}

/***************************************************************************//**
 * Sleeptimer timeout callback.
 ******************************************************************************/
static void on_timeout(sl_sleeptimer_timer_handle_t *handle,
                       void *data)
{
  (void)&handle;
  (void)&data;
  toggle_timeout = true;
}
