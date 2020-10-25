#ifdef EMBER_AF_NCP
#include "app/framework/include/af-ncp.h"
#else // !EMBER_AF_NCP
#include "app/framework/include/af.h"
#endif // EMBER_AF_NCP
#include <em_common.h>
#include "sl_bluetooth.h"
#include "sl_bt_stack_init.h"
#include "sl_bt_api.h"
#include "ble-interface.h"

static sl_bt_configuration_t config = SL_BT_CONFIG_DEFAULT;

void sl_bt_init()
{
  emberAfPluginBleGetConfig(&config);
  sl_bt_init_stack(&config);
  sl_bt_class_dfu_init();
  sl_bt_class_system_init();
  sl_bt_class_advertiser_init();
  sl_bt_class_gap_init();
  sl_bt_class_scanner_init();
  sl_bt_class_connection_init();
  sl_bt_class_gatt_init();
  sl_bt_class_gatt_server_init();
  sl_bt_class_nvm_init();
  sl_bt_class_sm_init();
  sl_bt_init_multiprotocol();
}

SL_WEAK void sl_bt_on_event(sl_bt_msg_t* evt)
{
  (void)(evt);
}
void sl_bt_process_event(sl_bt_msg_t *evt)
{
  sl_bt_on_event(evt);
}
void sl_bt_step()
{
  sl_bt_msg_t evt;
  /* Pop (non-blocking) a Bluetooth stack event from event queue. */
  sl_status_t status = sl_bt_pop_event(&evt);
  if (status != SL_STATUS_OK) {
    return;
  }
  sl_bt_process_event(&evt);
}
