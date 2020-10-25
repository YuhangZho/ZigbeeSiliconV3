#ifndef __SL_BLUETOOTH_CONFIG_H__
#define __SL_BLUETOOTH_CONFIG_H__

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Bluetooth Stack Configuration

// <o SL_BT_CONFIG_MAX_CONNECTIONS> Max number of connections <0-8>
// <i> Default: 4
// <i> Define the number of connections the application needs.
#define SL_BT_CONFIG_MAX_CONNECTIONS     EMBER_AF_PLUGIN_BLE_MAX_CONNECTIONS

// <o SL_BT_CONFIG_USER_ADVERTISERS> Max number of advertisers reserved for user <0-8>
// <i> Default: 1
// <i> Define the number of advertisers the application needs.
#define SL_BT_CONFIG_USER_ADVERTISERS     EMBER_AF_PLUGIN_BLE_MAX_USER_ADVERTISERS

#define SL_BT_CONFIG_MAX_ADVERTISERS     (SL_BT_CONFIG_USER_ADVERTISERS + SL_BT_COMPONENT_ADVERTISERS)

// <o SL_BT_CONFIG_MAX_SOFTWARE_TIMERS> Max number of software timers <0-16>
// <i> Default: 4
// <i> Define the number of software timers the application needs.  Each timer needs resources from the stack to be implemented. Increasing amount of soft timers may cause degraded performance in some use cases.
#define SL_BT_CONFIG_MAX_SOFTWARE_TIMERS     (4)

// <o SL_BT_CONFIG_MAX_PERIODIC_ADVERTISING_SYNC> Max number of periodic advertising synchronizations <0-8>
// <i> Default: 0
// <i> The maximum number of periodic advertising synchronizations the Bluetooth stack needs to support.
#define SL_BT_CONFIG_MAX_PERIODIC_ADVERTISING_SYNC     EMBER_AF_PLUGIN_BLE_MAX_PERIODIC_ADVERTISING_SYNC

// <o SL_BT_CONFIG_BUFFER_SIZE> Buffer memory size for Bluetooth stack
// <i> Default: 5308
// <i> Define buffer memory size for running Bluetooth stack and buffering data over Bluetooth connections,
// <i> advertising and scanning. The default value is an estimation for achieving adequate throughtput
// <i> and supporting multiple simultaneous connections. Consider increasing this value for
// <i> higher data throughput over connections, advertising or scanning long adveristment data.
#define SL_BT_CONFIG_BUFFER_SIZE    (5308)

// </h> End Bluetooth Stack Configuration

// <h> RF Path

// <o SL_BT_CONFIG_RF_PATH_GAIN_TX> RF TX Path gain (.1dBm)
// <i> Default: 0
// <i> The Bluetooth stack takes TX RF path gain into account when adjusting transmitter power. Power radiated from the antenna then matches the application request. For example, if maximum power requested by the application is at +10 dBm and path loss is -1 dBm, then actual power at the pin is +11 dBm.
#define SL_BT_CONFIG_RF_PATH_GAIN_TX     (0)
// <o SL_BT_CONFIG_RF_PATH_GAIN_RX> RF RX Path gain (.1dBm)
// <i> Default: 0
// <i> RX RF path gain is used to compensate the RSSI reports from the Bluetooth Stack.
#define SL_BT_CONFIG_RF_PATH_GAIN_RX     (0)

// </h> End RF Path

// <<< end of configuration section >>>
#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

void sli_bt_rtos_ll_callback();
void sli_bt_rtos_stack_callback();
#define SL_BT_CONFIG_FLAGS            SL_BT_CONFIG_FLAG_RTOS
#define SL_BT_CONFIG_LL_CALLBACK      sli_bt_rtos_ll_callback
#define SL_BT_CONFIG_STACK_CALLBACK   sli_bt_rtos_stack_callback

extern const struct bg_gattdb_def bg_gattdb_data;

#include "sl_bt_stack_config.h"

static sl_bt_bluetooth_ll_priorities ll_priorities = SL_BT_BLUETOOTH_PRIORITIES_DEFAULT;

#define SL_BT_CONFIG_DEFAULT                                                   \
  {                                                                            \
    .config_flags = SL_BT_CONFIG_FLAGS,                                        \
    .bluetooth.max_connections = SL_BT_CONFIG_MAX_CONNECTIONS,                 \
    .bluetooth.max_advertisers = SL_BT_CONFIG_MAX_ADVERTISERS,                 \
    .bluetooth.max_periodic_sync = SL_BT_CONFIG_MAX_PERIODIC_ADVERTISING_SYNC, \
    .bluetooth.linklayer_priorities = &ll_priorities,                          \
    .bluetooth.max_buffer_memory = SL_BT_CONFIG_BUFFER_SIZE,                   \
    .scheduler_callback = SL_BT_CONFIG_LL_CALLBACK,                            \
    .stack_schedule_callback = SL_BT_CONFIG_STACK_CALLBACK,                    \
    .gattdb = &bg_gattdb_data,                                                 \
    .max_timers = SL_BT_CONFIG_MAX_SOFTWARE_TIMERS,                            \
    .rf.tx_gain = SL_BT_CONFIG_RF_PATH_GAIN_TX,                                \
    .rf.rx_gain = SL_BT_CONFIG_RF_PATH_GAIN_RX,                                \
  }

#endif //__SL_BLUETOOTH_CONFIG_H__
