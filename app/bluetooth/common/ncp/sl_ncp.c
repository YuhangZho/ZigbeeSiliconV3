/***************************************************************************//**
 * @file
 * @brief Bluetooth Network Co-Processor (NCP) Interface
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
#include "em_common.h"
#include "em_core.h"
#include "sl_bluetooth.h"
#include "sl_status.h"
#include "sl_app_assert.h"
#include "sl_simple_com.h"
#include "sl_ncp.h"
#include "sl_component_catalog.h"
#include "sl_simple_timer.h"
#if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
#include "sl_wake_lock.h"
#endif // SL_CATALOG_WAKE_LOCK_PRESENT

// Command buffer
typedef struct {
  uint16_t len;
  uint8_t buf[SL_NCP_CMD_BUF_SIZE];
  bool available;
} cmd_t;

// Event buffer
typedef struct {
  uint16_t len;
  uint8_t buf[SL_NCP_EVT_BUF_SIZE];
  bool available;
} evt_t;

// Timer states
typedef enum {
  CMD_IDLE = 0,
  CMD_WAITING,
  CMD_RECEIVED
} timer_state_t;

static cmd_t cmd = { 0 };
static evt_t evt = { 0 };
static bool busy = false;
#if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
// semaphore to disable sleep prematurely
static uint8_t sleep_semaphore = 0;
// upper bits are flags to store signal value and wait until first cmd is received
#define SLEEP_SIGNAL_PRESENT   0x80
#define SLEEP_WAITING_FIRST_RX 0x40
#define SLEEP_SIGNAL_MASK      SLEEP_WAITING_FIRST_RX | SLEEP_SIGNAL_PRESENT
#endif // SL_CATALOG_WAKE_LOCK_PRESENT

// Command and event buffer helper functions
static void cmd_enqueue(uint16_t len, uint8_t *data);
static void cmd_dequeue(void);
static void evt_enqueue(uint16_t len, uint8_t *data);
static void evt_dequeue(void);

// Command and event helper functions
static inline bool cmd_is_available(void);
static inline void cmd_set_available(void);
static inline void cmd_clr_available(void);
static inline bool cmd_is_user_type(void);
static inline bool evt_is_available(void);
static inline void evt_set_available(void);
static inline void evt_clr_available(void);
static inline uint16_t msg_get_len(sl_bt_msg_t *msg);

// Timer handle and callback for command timeout.
static sl_simple_timer_t cmd_timer;
static void cmd_timer_cb(sl_simple_timer_t *timer, void *data);

#if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
static inline bool sleep_signal_mask_is_set(void);
static inline void sleep_signal_mask_set(void);
static inline void sleep_signal_flag_clr(void);
static inline void sleep_signal_wait_clr(void);
#endif // SL_CATALOG_WAKE_LOCK_PRESENT

// -----------------------------------------------------------------------------
// Public functions (API implementation)

/**************************************************************************//**
 * Ncp Initialization function.
 *****************************************************************************/
void sl_ncp_init(void)
{
  // Clear all buffers
  cmd_dequeue();
  evt_dequeue();
  // Init communication interface
  sl_simple_com_init();
  // Start reception
  sl_simple_com_receive();
  busy = false;
  #if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
  sleep_semaphore = 0;
  #endif // SL_CATALOG_WAKE_LOCK_PRESENT
}

/**************************************************************************//**
 * Ncp process action function.
 *
 * @note Called through sl_system_process_action
 *****************************************************************************/
void sl_ncp_step(void)
{
  // -------------------------------
  // Command available and NCP not busy
  if (cmd_is_available() && !busy) {
    cmd_clr_available();
    // User command
    if (cmd_is_user_type()) {
      // Call user command handler
      sl_ncp_user_command_cb((void *)&cmd.buf[SL_BT_MSG_HEADER_LEN]);
    }
    // Bluetooth stack command
    else {
      // Call Bluetooth API binary command handler
      sl_bt_handle_command(*(uint32_t *)cmd.buf,
                           (void*)&cmd.buf[SL_BT_MSG_HEADER_LEN]);
    }
    // Clear command buffer
    cmd_dequeue();
    busy = true;
    #if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
    // Wake up other controller
    sl_wake_lock_set_remote_req();
    #endif // SL_CATALOG_WAKE_LOCK_PRESENT
    // Transmit command response
    sl_simple_com_transmit((uint32_t)(msg_get_len(sl_bt_get_command_response())),
                           (uint8_t*)sl_bt_get_command_response());
  }

  // -------------------------------
  // Event available and NCP not busy
  if (evt_is_available() && !busy) {
    evt_clr_available();
    busy = true;
    #if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
    // Wake up other controller
    sl_wake_lock_set_remote_req();
    #endif // SL_CATALOG_WAKE_LOCK_PRESENT
    // Transmit command response
    sl_simple_com_transmit((uint32_t)evt.len, evt.buf);
    // Clear event buffer
    evt_dequeue();
  }
}

/**************************************************************************//**
 * User command handler callback.
 *
 * Handles user defined commands received from NCP.
 *
 * @param[in] data Data received from NCP through uart.
 *
 * @note Weak implementation.
 *****************************************************************************/
SL_WEAK void sl_ncp_user_command_cb(void *data)
{
  (void)data;
}

/**************************************************************************//**
 * Send NCP user command response
 *
 * Sends command response to user defined commands to NCP.
 *
 * @param[out] result Result of the response to the command received.
 * @param[out] len Message length.
 * @param[out] data Data to send to NCP.
 *****************************************************************************/
void sl_ncp_user_command_rsp(sl_status_t result, uint8_t len, uint8_t *data)
{
  sl_bt_send_rsp_user_message_to_target(result, len, data);
}

/**************************************************************************//**
 * Send NCP user event response
 *
 * Sends event response to user defined commands to NCP.
 *
 * @param[out] len Message length.
 * @param[out] data Data to send to NCP.
 *****************************************************************************/
void sl_ncp_user_evt(uint8_t len, uint8_t *data)
{
  sl_bt_send_evt_user_message_to_host(len, data);
}

// -----------------------------------------------------------------------------
// Functions used for communication between stack and Uart

/**************************************************************************//**
 * Bluetooth stack event handler.
 *
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  // Enqueue event
  evt_enqueue(msg_get_len(evt),
              (uint8_t *)evt);
}

/**************************************************************************//**
 * Signal if bluetooth stack can process a new received event.
 *
 * Called before putting event into event buffer.
 *
 * @param[in] len Message length
 *
 * @return True if message fits into buffer, otherwise false.
 *****************************************************************************/
bool sl_bt_can_process_event(uint32_t len)
{
  bool ret = false;
  // event fits into event buffer; otherwise don't pop it from queue
  if (len <= (uint32_t)(sizeof(evt.buf) - evt.len)) {
    ret = true;
  }
  return ret;
}

/**************************************************************************//**
 * Uart transmit completed callback
 *
 * Called after Uart transmit is finished.
 *
 * @param[in] status Status of the transmission
 *****************************************************************************/
void sl_simple_com_transmit_cb(sl_status_t status)
{
  (void)status;

  #if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
  // Signal other controller that it can go to sleep
  sl_wake_lock_clear_remote_req();

  // Command response completed, go to sleep if possible
  if (sleep_signal_mask_is_set()) {
    // Only decrease semaphore if a command was received before
    // i.e. not an event was sent out
    sleep_semaphore--;
    if (!sleep_semaphore) {
      // Go to sleep if signal is down and command answered
      sl_wake_lock_set_local();
    }
  }
  #endif // SL_CATALOG_WAKE_LOCK_PRESENT
  busy = false;
}

/**************************************************************************//**
 * Uart receive completed callback
 *
 * Called after Uart receive is finished.
 *
 * @param[in] status Status of the reception
 * @param[in] len Received message length
 * @param[in] data Data received
 *****************************************************************************/
void sl_simple_com_receive_cb(sl_status_t status, uint32_t len, uint8_t *data)
{
  (void)status;
  // Stop on communication error
  sl_app_assert(SL_STATUS_OK == status,
                "Receive error. Received %d bytes.\n",
                (int)len);
  // Copy command into buffer
  cmd_enqueue((uint16_t)len, data);
}

#if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
/**************************************************************************//**
 * Wake-up signal arrived from host.
 * This overrides the dummy weak implementation.
 *****************************************************************************/
void sl_wake_lock_set_req_rx_cb(void)
{
  // Other end set wake-up pin, so set signaling flag. Also signal that at least
  // one command needs to be received and processed before sleep.
  sleep_signal_mask_set();
  sl_bt_send_system_awake();
}

/**************************************************************************//**
 * Go to sleep signal arrived from host.
 * This overrides the dummy weak implementation.
 *****************************************************************************/
void sl_wake_lock_clear_req_rx_cb(void)
{
  // Other end released pin, remove signaling flag
  sleep_signal_flag_clr();

  if (!sleep_semaphore) {
    // Go to sleep if signal is down and command answered
    sl_wake_lock_set_local();
  }
}
#endif // SL_CATALOG_WAKE_LOCK_PRESENT

// -----------------------------------------------------------------------------
// Command and event buffer handling functions

/**************************************************************************//**
 * Put command to command buffer
 *
 * @param[in] len Command length
 * @param[in] data Command data
 *****************************************************************************/
static void cmd_enqueue(uint16_t len, uint8_t *data)
{
  sl_status_t sc;
  timer_state_t state = CMD_IDLE;

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  // Command fits into command buffer; otherwise discard it.
  if (len <= (sizeof(cmd.buf) - cmd.len)) {
    memcpy((void *)&cmd.buf[cmd.len], (void *)data, (size_t)len);
    cmd.len += len;
    // Part of the command received, start timer to not get stuck.
    state = CMD_WAITING;
    // Command header has been received.
    if (cmd.len >= SL_BT_MSG_HEADER_LEN) {
      // 4-byte header + len bytes payload
      uint32_t len = SL_BT_MSG_LEN(*(uint32_t *)cmd.buf) + SL_BT_MSG_HEADER_LEN;
      if (cmd.len >= len) {
        // Command has been received entirely.
        cmd_set_available();
        state = CMD_RECEIVED;
  #if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
        // Command received fully, increase semaphore.
        sleep_semaphore++;
        // Also signal that a command was received, system can go to sleep after
        sleep_signal_wait_clr();
  #endif // SL_CATALOG_WAKE_LOCK_PRESENT
      }
    }
  }
  CORE_EXIT_ATOMIC();

  if (state == CMD_RECEIVED) {
    // Stop timer as the whole command was received. No problem if timer was
    // not started before, simple_timer is prepared for that.
    sc = sl_simple_timer_stop(&cmd_timer);

    sl_app_assert(sc == SL_STATUS_OK,
                  "[E: 0x%04x] Failed to stop timer\n",
                  (int)sc);
  } else if (state == CMD_WAITING) {
    // Start timer used for max waiting time of fragmented packets.
    sc = sl_simple_timer_start(&cmd_timer,
                               SL_NCP_CMD_TIMEOUT_MS,
                               cmd_timer_cb,
                               NULL,
                               false);
    sl_app_assert(sc == SL_STATUS_OK,
                  "[E: 0x%04x] Failed to start timer\n",
                  (int)sc);
  }
}

/**************************************************************************//**
 * Clear command buffer
 *****************************************************************************/
static void cmd_dequeue(void)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  memset((void *)cmd.buf, 0, (size_t)cmd.len);
  cmd.len = 0;
  cmd_clr_available();
  CORE_EXIT_ATOMIC();
}

/**************************************************************************//**
 * Put event to event buffer
 *
 * @param[in] len Event length
 * @param[in] data Event data
 *****************************************************************************/
static void evt_enqueue(uint16_t len, uint8_t *data)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  // event fits into event buffer; otherwise discard it
  if (len <= (sizeof(evt.buf) - evt.len)) {
    memcpy((void *)&evt.buf[evt.len], (void *)data, len);
    evt.len += len;
    evt_set_available();
  }
  CORE_EXIT_ATOMIC();
}

/**************************************************************************//**
 * Put event to event buffer
 *****************************************************************************/
static void evt_dequeue(void)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  memset((void *)evt.buf, 0, sizeof(evt.buf));
  evt.len = 0;
  evt_clr_available();
  CORE_EXIT_ATOMIC();
}

// -----------------------------------------------------------------------------
// Timer callback function

static void cmd_timer_cb(sl_simple_timer_t *timer, void *data)
{
  (void)data;
  (void)timer;

  // Signal the other end that the command was received only partially
  // then clean buffer. The timer is already stopped.
  sl_bt_send_system_error(SL_STATUS_COMMAND_INCOMPLETE, 0, NULL);
  cmd_dequeue();
}

// -----------------------------------------------------------------------------
// Helper functions

/**************************************************************************//**
 * Get command availability in command buffer
 *****************************************************************************/
static inline bool cmd_is_available(void)
{
  return cmd.available;
}

/**************************************************************************//**
 * Set command available in command buffer
 *****************************************************************************/
static inline void cmd_set_available(void)
{
  cmd.available = true;
}

/**************************************************************************//**
 * Clear command available in command buffer
 *****************************************************************************/
static inline void cmd_clr_available(void)
{
  cmd.available = false;
}

/**************************************************************************//**
 * Check user type command in command buffer
 *****************************************************************************/
static inline bool cmd_is_user_type(void)
{
  return (sl_bt_cmd_user_message_to_target_id
          == SL_BT_MSG_ID(*(uint32_t *)cmd.buf));
}

/**************************************************************************//**
 * Get event availability in command buffer
 *****************************************************************************/
static inline bool evt_is_available(void)
{
  return evt.available;
}

/**************************************************************************//**
 * Set event available in command buffer
 *****************************************************************************/
static inline void evt_set_available(void)
{
  evt.available = true;
}

/**************************************************************************//**
 * Clear event available in command buffer
 *****************************************************************************/
static inline void evt_clr_available(void)
{
  evt.available = false;
}

/**************************************************************************//**
 * Get message length from header
 *****************************************************************************/
static inline uint16_t msg_get_len(sl_bt_msg_t *msg)
{
  return (uint16_t)(SL_BT_MSG_LEN(msg->header) + SL_BT_MSG_HEADER_LEN);
}

#if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
/**************************************************************************//**
 * Get sleep signal flag and wait for rx cmd flag state
 *****************************************************************************/
static inline bool sleep_signal_mask_is_set(void)
{
  return (sleep_semaphore & ~(SLEEP_SIGNAL_MASK));
}

/**************************************************************************//**
 * Set sleep signal flag and wait for rx cmd flag state
 *****************************************************************************/
static inline void sleep_signal_mask_set(void)
{
  sleep_semaphore |= (SLEEP_SIGNAL_PRESENT | SLEEP_WAITING_FIRST_RX);
}

/**************************************************************************//**
 * Clear sleep signal flag state
 *****************************************************************************/
static inline void sleep_signal_flag_clr(void)
{
  sleep_semaphore &= ~(SLEEP_SIGNAL_PRESENT);
}

/**************************************************************************//**
 * Clear wait for rx cmd flag state
 *****************************************************************************/
static inline void sleep_signal_wait_clr(void)
{
  sleep_semaphore &= ~(SLEEP_WAITING_FIRST_RX);
}
#endif // SL_CATALOG_WAKE_LOCK_PRESENT
