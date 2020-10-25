#include "openthread-system.h"
#include "sl_ot_freertos_adaptation.h"
#include "sl_bt_rtos_adaptation.h"
#include "openthread/cli.h"
#include "bluetooth_cli.h"
#include "sl_button.h"
#include "sl_simple_button.h"
#include "uartdrv.h"
#include "sl_power_manager.h"

static bool sButtonPressed = false;
static bool sStayAwake = true;

void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    sButtonPressed = true;
    otSysEventSignalPending();
  }
}

void sl_ot_rtos_application_init(void)
{
  otCliUartInit(otrGetInstance());
  bluetoothCliInit();
  // Start out awake so the CLI works.
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
}

void sl_ot_rtos_application_tick(void)
{
  if (sButtonPressed) {
    sButtonPressed = false;
    sStayAwake = !sStayAwake;
    if (sStayAwake) {
      sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
    } else {
      sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
    }
  }
  
#ifdef SL_BT_DISABLE_EVENT_TASK
  sl_bt_event_handler_tick();
#endif
}

int main(int argc, char *argv[])
{
  // This calls sl_system_init() which also initializes bluetooth.
  otrInit(argc, argv);
  vTaskStartScheduler();
  return 0;
}
