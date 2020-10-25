/***************************************************************************//**
 * @file
 * @brief Simple timer service header
 *
 * Simple timer service for applications with less strict timing requirements.
 * This module is based on the sleeptimer, but runs the timer callbacks in
 * non-interrupt context. This behavior gives more flexibility for the callback
 * implementation but causes a less precise timing.
 *
 * @note If your application requires precise timing, please use the sleeptimer
 *       directly.
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
#ifndef SIMPLE_TIMER_H
#define SIMPLE_TIMER_H

#include "sl_sleeptimer.h"

// Forward declaration
typedef struct sl_simple_timer sl_simple_timer_t;

/***************************************************************************//**
 * Expected prototype of the user's callback function which is called when a
 * timer expires.
 *
 * @param timer Pointer to the timer handle.
 * @param data An extra parameter for the user application.
 ******************************************************************************/
typedef void (*sl_simple_timer_callback_t)(sl_simple_timer_t *timer, void *data);

/// Timer structure
struct sl_simple_timer {
  sl_sleeptimer_timer_handle_t sleeptimer_handle;
  sl_simple_timer_callback_t callback;
  void *callback_data;
  sl_simple_timer_t *next;
  bool triggered;
  bool periodic;
};

/***************************************************************************//**
 * Start timer or restart if it is running already.
 *
 * @param[in] timer Pointer to the timer.
 * @param[in] timeout_ms Timer timeout, in milliseconds.
 * @param[in] callback Callback function that is called when timeout expires.
 * @param[in] callback_data Pointer to user data that will be passed to callback.
 * @param[in] is_periodic Reload timer when it expires if true.
 *
 * @return Status of the operation.
 ******************************************************************************/
sl_status_t sl_simple_timer_start(sl_simple_timer_t *timer,
                                  uint32_t timeout_ms,
                                  sl_simple_timer_callback_t callback,
                                  void *callback_data,
                                  bool is_periodic);

/***************************************************************************//**
 * Stop running timer.
 *
 * @param[in] timer Pointer to the timer.
 *
 * @return Status of the operation.
 ******************************************************************************/
sl_status_t sl_simple_timer_stop(sl_simple_timer_t *timer);

/***************************************************************************//**
 * Execute timer callback functions.
 ******************************************************************************/
void sli_simple_timer_step(void);

#endif // SIMPLE_TIMER_H
