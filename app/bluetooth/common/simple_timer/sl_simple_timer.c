/***************************************************************************//**
 * @file
 * @brief Simple timer service
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
#include "sl_simple_timer.h"
#include "em_core.h"

// -----------------------------------------------------------------------------
// Private variables

/// Number of the triggered timers.
static uint32_t trigger_count = 0;

/// Start of the linked list which contains the active timers.
static sl_simple_timer_t *simple_timer_head = NULL;

// -----------------------------------------------------------------------------
// Private function declarations

/***************************************************************************//**
 * Common callback for the sleeptimers.
 *
 * @param[in] handle Pointer to the sleeptimer handle.
 * @param[in] data Pointer to the sleeptimer's parent simple timer.
 *
 * @note This function runs in interrupt context.
 ******************************************************************************/
static void simple_timer_callback(sl_sleeptimer_timer_handle_t *handle,
                                  void *data);

/***************************************************************************//**
 * Append a timer to the end of the linked list.
 *
 * @param[in] timer Pointer to the timer handle.
 *
 * @pre Assumes that the timer is not present in the list.
 ******************************************************************************/
static void append_simple_timer(sl_simple_timer_t *timer);

/***************************************************************************//**
 * Remove a timer from the linked list.
 *
 * @param[in] timer Pointer to the timer handle.
 *
 * @return Presence of the timer in the linked list.
 * @retval true  Timer was in the list.
 * @retval false Timer was not found in the list.
 ******************************************************************************/
static bool remove_simple_timer(sl_simple_timer_t *timer);

// -----------------------------------------------------------------------------
// Public function definitions

sl_status_t sl_simple_timer_start(sl_simple_timer_t *timer,
                                  uint32_t timeout_ms,
                                  sl_simple_timer_callback_t callback,
                                  void *callback_data,
                                  bool is_periodic)
{
  sl_status_t sc;

  // Check input parameters.
  if ((timeout_ms == 0) && is_periodic) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // Make sure that timer is stopped, also check for NULL.
  sc = sl_simple_timer_stop(timer);
  if (SL_STATUS_OK != sc) {
    return sc;
  }
  timer->triggered = false;

  // Start sleeptimer.
  if (is_periodic) {
    sc = sl_sleeptimer_start_periodic_timer_ms(
      &timer->sleeptimer_handle,
      timeout_ms,
      simple_timer_callback,
      (void*)timer,
      0,
      0);
  } else {
    sc = sl_sleeptimer_start_timer_ms(
      &timer->sleeptimer_handle,
      timeout_ms,
      simple_timer_callback,
      (void*)timer,
      0,
      0);
  }

  if (SL_STATUS_OK == sc) {
    timer->callback = callback;
    timer->callback_data = callback_data;
    timer->periodic = is_periodic;
    append_simple_timer(timer);
  }
  return sc;
}

sl_status_t sl_simple_timer_stop(sl_simple_timer_t *timer)
{
  bool timer_present;

  if (timer == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  // Stop sleeptimer, ignore error code if was not running.
  (void)sl_sleeptimer_stop_timer(&timer->sleeptimer_handle);

  timer_present = remove_simple_timer(timer);
  if (timer_present && timer->triggered) {
    // Timer has been triggered but not served yet.
    CORE_ATOMIC_SECTION(--trigger_count; )
  }
  return SL_STATUS_OK;
}

void sli_simple_timer_step(void)
{
  if (trigger_count > 0) {
    sl_simple_timer_t *timer = simple_timer_head;
    // Find triggered timers in list and call their callbacks.
    while (timer != NULL) {
      if (timer->triggered) {
        CORE_ATOMIC_SECTION(
          timer->triggered = false;
          --trigger_count;
          )
        if (!timer->periodic) {
          (void)remove_simple_timer(timer);
        }
        timer->callback(timer, timer->callback_data);
      }
      timer = timer->next;
    }
  }
}

// -----------------------------------------------------------------------------
// Private function definitions

static void simple_timer_callback(sl_sleeptimer_timer_handle_t *handle,
                                  void *data)
{
  (void)handle;
  if (((sl_simple_timer_t*)data)->triggered == false) {
    ((sl_simple_timer_t*)data)->triggered = true;
    ++trigger_count;
  }
}

static void append_simple_timer(sl_simple_timer_t *timer)
{
  if (simple_timer_head != NULL) {
    sl_simple_timer_t *current = simple_timer_head;
    // Find end of list.
    while (current->next != NULL) {
      current = current->next;
    }
    current->next = timer;
  } else {
    simple_timer_head = timer;
  }
  timer->next = NULL;
}

static bool remove_simple_timer(sl_simple_timer_t *timer)
{
  sl_simple_timer_t *prev = NULL;
  sl_simple_timer_t *current = simple_timer_head;

  // Find timer in list.
  while (current != NULL && current != timer) {
    prev = current;
    current = current->next;
  }

  if (current != timer) {
    // Not found.
    return false;
  }

  if (prev != NULL) {
    prev->next = timer->next;
  } else {
    simple_timer_head = timer->next;
  }
  return true;
}
