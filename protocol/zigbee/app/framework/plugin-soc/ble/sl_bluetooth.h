#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#define SL_BT_COMPONENT_ADVERTISERS 0
#define SL_BT_DISABLE_EVENT_TASK 1

#include <stdbool.h>
#include "sl_power_manager.h"
#include "sl_bluetooth_config.h"
#include "sl_bt_api.h"

// initialize Bluetooth core functionality
void sl_bt_init();
//Polls bluetooth stack for an event and processes it
void sl_bt_step();

//processes a single bluetooth event
void sl_bt_process_event(sl_bt_msg_t *evt);

void sl_bt_on_event(sl_bt_msg_t* evt);

//power manager related functions
bool sli_bt_is_ok_to_sleep(void);
sl_power_manager_on_isr_exit_t sli_bt_sleep_on_isr_exit(void);

#endif
