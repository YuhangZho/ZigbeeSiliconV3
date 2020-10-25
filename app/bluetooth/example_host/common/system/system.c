#include "sl_bt_api.h"
#include "sl_bt_ncp_host.h"
#include "system.h"

static void sl_bt_init(void);
static void sl_bt_step(void);

// TODO: remove if system takes care of generating this under autogen directory
void sl_system_init(void)
{
  sl_bt_init();
}

// TODO: remove if system takes care of generating this under autogen directory
void sl_system_process_action(void)
{
  sl_bt_step();
}

// Bluetooth stack event handler
SL_WEAK void sl_bt_on_event(sl_bt_msg_t *evt)
{
  (void)(evt);
}

// Initialize Bluetooth stack
static void sl_bt_init(void)
{
  // nothing to do
}

// Poll Bluetooth stack for an event and call event handler
static void sl_bt_step(void)
{
  sl_bt_msg_t evt;

  // Pop (non-blocking) a Bluetooth stack event from event queue.
  sl_status_t status = sl_bt_pop_event(&evt);
  if (status != SL_STATUS_OK) {
    return;
  }
  sl_bt_on_event(&evt);
}
