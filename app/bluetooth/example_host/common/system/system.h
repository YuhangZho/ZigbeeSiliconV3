#ifndef SYSTEM_H
#define SYSTEM_H

#include "sl_bt_api.h"

/** @brief A macro for defining a weak symbol. */
#define SL_WEAK __attribute__ ((weak))

// TODO: remove if system takes care of generating this under autogen directory
void sl_system_init(void);
void sl_system_process_action(void);

void sl_bt_on_event(sl_bt_msg_t* evt);

#endif // SYSTEM_H
