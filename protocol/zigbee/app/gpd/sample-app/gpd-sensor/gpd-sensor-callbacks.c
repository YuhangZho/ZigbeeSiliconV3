/***************************************************************************//**
 * @file
 * @brief Application specific overrides of weak functions defined as part of
 * the test application.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "gpd-components-common.h"
#include "sl_sleeptimer.h"

// If serial plugin is enabled for debug print
#if defined(EMBER_AF_PLUGIN_SERIAL)
#include EMBER_AF_API_SERIAL
#endif

// If debug print is enabled
#if defined(EMBER_AF_PLUGIN_EMBER_MINIMAL_PRINTF)
#define gpdDebugPrintf(...) emberSerialGuaranteedPrintf(APP_SERIAL, __VA_ARGS__)
#else
#define gpdDebugPrintf(...)
#endif

//#define GPD_CLI
#if defined(EMBER_AF_PLUGIN_COMMAND_INTERPRETER2)
#include EMBER_AF_API_COMMAND_INTERPRETER2
  #define COMMAND_READER_INIT() emberCommandReaderInit()
#else
  #define COMMAND_READER_INIT()
#endif

// NV Storage
#if defined EMBER_AF_PLUGIN_NVM3
#include "nvm3.h"
#elif defined EMBER_AF_PLUGIN_PSSTORE
extern void store_init(void);
static uint8_t *p;
static uint8_t length;
static uint8_t flags;
#endif

#define EMBER_GPD_NV_DATA_TAG 0xA9A1

// LED Indication
#define ACTIVITY_LED BOARDLED1
#define COMMISSIONING_STATE_LED BOARDLED0

#if defined EMBER_AF_PLUGIN_LED
#define BOARD_LED_ON(led) halSetLed(led)
#define BOARD_LED_OFF(led) halClearLed(led)
#else
#define BOARD_LED_ON(led)
#define BOARD_LED_OFF(led)
#endif

// App button press event types
enum {
  APP_EVENT_ACTION_IDLE = 0,
  APP_EVENT_ACTION_SEND_COMMISSION = 0x01,
  APP_EVENT_ACTION_SEND_DECOMMISSION = 0x02,
  APP_EVENT_ACTION_SEND_TOGGLE = 0x03,
  APP_EVENT_ACTION_SEND_REPORT = 0x04,
};
typedef uint8_t GpdAppEventActionType;

#ifndef APP_SINGLE_EVENT_COMMISSIONING_LOOP_TIME_MS
#define APP_SINGLE_EVENT_COMMISSIONING_LOOP_TIME_MS 1000
#endif

#ifndef APP_GPD_REPORTING_TIME_MS
#define APP_GPD_REPORTING_TIME_MS 30000
#endif

// ----------- GPD application functional blocks ------------------------------
// This implements the following
// -- 1. NVM Storage (NVM3 or PSSTORE)for the application - NVM3 is used.
// -- 2. Bidirectional rx offset and window timing
//       Using the sl_sleeptimer driver.
// -- 3. Application main loop
//       Using the sl_sleeptimer driver for reporting
// -- 4. Application Button
//       To send (de)commissioning command and send a report out
// -- 5. Application main loop
// ----------- END :GPD application functional blocks -------------------------

// ----------------------------------------------------------------------------
// The following code is organised as per the above application functionality
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// ----------- 1. NVM usage in Application ------------------------------------
// ----------------------------------------------------------------------------

/** @brief This is called by framework to initialise the NVM system
 *
 */
void emberGpdAfPluginNvInitCallback(void)
{
  // Initialise the NV
#if defined EMBER_AF_PLUGIN_PSSTORE
  store_init();
#elif defined EMBER_AF_PLUGIN_NVM3
  // use default NVM space and handle
  Ecode_t status = nvm3_open(nvm3_defaultHandle, nvm3_defaultInit);
  if (status != ECODE_NVM3_OK) {
    // Handle error
  }

  // Do re-packing if needed
  if (nvm3_repackNeeded(nvm3_defaultHandle)) {
    status = nvm3_repack(nvm3_defaultHandle);
    if (status != ECODE_NVM3_OK) {
      // Handle error
    }
  }
#endif
}

/** @brief Called to the application to give a chance to load or store the GPD Context
 *.        in a non volatile context. Thsi can help the application to use any other
 *         non volatile storage.
 *
 * @param nvmData The pointer to the data that needs saving or retrieving to or from
 *                the non-volatile memory.
 * @param sizeOfNvmData The size of the data non-volatile data.
 * @param loadStore indication wether to load or store the context.
 * Ver.: always
 *
 * @return true if application handled it.
 */
bool emberGpdAfPluginNvSaveAndLoadCallback(EmberGpd_t * gpd,
                                           uint8_t * nvmData,
                                           uint8_t sizeOfNvmData,
                                           EmebrGpdNvLoadStore_t loadStore)
{
  if (loadStore == EMEBER_GPD_AF_CALLBACK_LOAD_GPD_FROM_NVM) {
  #if defined EMBER_AF_PLUGIN_PSSTORE
    if (0 != store_read(EMBER_GPD_NV_DATA_TAG, &flags, &length, &p)) {
      // Fresh chip , erase, create a storage with default setting.
      store_eraseall();
      // First write to the NVM shadow so that it updated with default ones
      emberGpdCopyToShadow(gpd);
      // Write the data to NVM
      store_write(EMBER_GPD_NV_DATA_TAG,
                  flags,
                  sizeOfNvmData,
                  (void *)nvmData);
    } else {
      memcpy(nvmData, p, sizeOfNvmData);
    }
  #elif defined EMBER_AF_PLUGIN_NVM3
    uint32_t objectType;
    size_t dataLen;
    if (0 == nvm3_countObjects(nvm3_defaultHandle)
        || (nvm3_getObjectInfo(nvm3_defaultHandle,
                               EMBER_GPD_NV_DATA_TAG,
                               &objectType,
                               &dataLen) == ECODE_NVM3_OK
            && objectType != NVM3_OBJECTTYPE_DATA)) {
      // Fresh chip , erase, create a storage with default setting.
      // Erase all objects and write initial data to NVM3
      nvm3_eraseAll(nvm3_defaultHandle);
      // First write to the NVM shadow so that it updated with default ones
      emberGpdCopyToShadow(gpd);
      // Write the data to NVM
      nvm3_writeData(nvm3_defaultHandle,
                     EMBER_GPD_NV_DATA_TAG,
                     nvmData,
                     sizeOfNvmData);
    } else {
      nvm3_readData(nvm3_defaultHandle,
                    EMBER_GPD_NV_DATA_TAG,
                    nvmData,
                    sizeOfNvmData);
    }
  #endif
  } else if (loadStore == EMEBER_GPD_AF_CALLBACK_STORE_GPD_TO_NVM) {
  #if defined EMBER_AF_PLUGIN_PSSTORE
    store_write(EMBER_GPD_NV_DATA_TAG,
                flags,
                sizeOfNvmData,
                (void *)nvmData);
  #elif defined EMBER_AF_PLUGIN_NVM3
    nvm3_writeData(nvm3_defaultHandle,
                   EMBER_GPD_NV_DATA_TAG,
                   nvmData,
                   sizeOfNvmData);
  #endif
  } else {
    // bad command
  }
  return false;
}
// ----------------------------------------------------------------------------
// ----------- END : NVM usage in Application ---------------------------------
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// 2. Bidirectional rxOffset and rxWindow timing
// ----------------------------------------------------------------------------
// Sleep timer.
static sl_sleeptimer_timer_handle_t le_timer_handle;

// Sleep timer callbacks
static void leTimeCallback(sl_sleeptimer_timer_handle_t *handle, void *contextData)
{
  sl_sleeptimer_stop_timer(handle);
}

bool emberGpdLeTimerRunning(void)
{
  bool running = false;
  sl_sleeptimer_is_timer_running(&le_timer_handle, &running);
  return running;
}

void emberGpdLeTimerInit(void)
{
  // For Low Energy Timing the sleep timer is used , which uses RTC/RTCC
  // Ensure the clock is enabled.

#if defined _SILICON_LABS_32B_SERIES_2
  // Setting RTCC clock source
  CMU_ClockSelectSet(cmuClock_RTCCCLK, cmuSelect_LFRCO);
#elif defined _SILICON_LABS_32B_SERIES_1
  // Turn on the clock for the RTCC
  CMU_ClockEnable(cmuClock_HFLE, true);
  CMU_ClockSelectSet(cmuClock_LFE, cmuSelect_LFRCO);
#endif
  CMU_ClockEnable(cmuClock_RTCC, true);
}

void emberGpdLoadLeTimer(uint32_t timeInUs)
{
  // in Hz => Time period in micro sec T = 1000000/f
  uint32_t f = sl_sleeptimer_get_timer_frequency();
  // ticks needed = (timeout needed in mico sec)/(T in micro sec) =  ((timeout needed in micro sec) * f)/1000000
  uint32_t tick = (timeInUs * f) / 1000000;
  sl_sleeptimer_restart_timer(&le_timer_handle,
                              tick,
                              leTimeCallback,
                              NULL,
                              0,
                              0);
}

void emberGpdAfPluginSleepCallback(void)
{
  EMU_EnterEM2(true);
}
// ----------------------------------------------------------------------------
// ----------- END : Bidirectional rxOffset and rxWindow timing ---------------
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// 3. Application main loop timing
// ----------------------------------------------------------------------------

// Low Power Mode with option to force EM4 mode.
static void gpdEnterLowPowerMode(bool forceEm4)
{
  if (forceEm4) {
    EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
    // EM4 Pin Mask = (Pin1|Pin0 << Base shift)
    GPIO_EM4EnablePinWakeup((3 << 16), 0);
    //em4Init.retainLfxo = true;
    em4Init.pinRetentionMode = emuPinRetentionEm4Exit;
    em4Init.em4State = emuEM4Shutoff;
    EMU_EM4Init(&em4Init);
    SLEEP_ForceSleepInEM4();
  } else {
    EMU_EnterEM2(true);
  }
}

// ----------------------------------------------------------------------------
// ----------- END : Application main loop timing -----------------------------
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// ------------Sending an operational command ---------------------------------
// ----------------------------------------------------------------------------
static void sendReport(EmberGpd_t * gpd)
{
  uint8_t command[] = {
    GP_CMD_ATTRIBUTE_REPORTING,
    0x06, 0x04, // Cluster Id
    0x00, 0x00, // Attribute Id
    0x10,       // Attribute Type
    0x01        // Attribute Value
  };
  gpd->rxAfterTx = false;
  emberAfGpdfSend(EMBER_GPD_NWK_FC_FRAME_TYPE_DATA,
                  gpd,
                  command,
                  sizeof(command),
                  EMBER_AF_PLUGIN_APPS_CMD_RESEND_NUMBER);
}
// ----------------------------------------------------------------------------
// ------------ END : Sending an operational command --------------------------
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// -------------- 4. Application Event ----------------------------------------
// ----------------------------------------------------------------------------
static GpdAppEventActionType appAction;
static sl_sleeptimer_timer_handle_t app_single_event_commission;
static void processAppEvent(EmberGpd_t * gpd, GpdAppEventActionType *appAction)
{
  if (*appAction != APP_EVENT_ACTION_IDLE) {
    BOARD_LED_ON(ACTIVITY_LED);
    if (*appAction == APP_EVENT_ACTION_SEND_DECOMMISSION) {
      emberGpdAfPluginDeCommission(gpd);
      emberGpdStoreSecDataToNV(gpd);
      gpdDebugPrintf("Decomm Cmd : ");
    } else if (*appAction == APP_EVENT_ACTION_SEND_COMMISSION) {
      emberGpdAfPluginCommission(gpd);
      emberGpdStoreSecDataToNV(gpd);
      gpdDebugPrintf("Comm. Cmd : ");
    } else if (*appAction == APP_EVENT_ACTION_SEND_REPORT) {
      sendReport(gpd);
      emberGpdStoreSecDataToNV(gpd);
      gpdDebugPrintf("Reporting : ");
    }
    gpdDebugPrintf("Comm. State :%d\n", gpd->gpdState);
    *appAction = APP_EVENT_ACTION_IDLE;
    BOARD_LED_OFF(ACTIVITY_LED);
  }
}
static void appSingleEventCommissionTimer(sl_sleeptimer_timer_handle_t *handle, void *contextData)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  if (gpd->gpdState < EMBER_GPD_APP_STATE_OPERATIONAL) {
    appAction = APP_EVENT_ACTION_SEND_COMMISSION;
  } else {
    sl_sleeptimer_stop_timer(handle);
  }
}
// Application Commissioning that completes all the statest of the commissioning
void emberGpdAppSingleEventCommission(void)
{
  sl_sleeptimer_restart_periodic_timer_ms(&app_single_event_commission,
                                          APP_SINGLE_EVENT_COMMISSIONING_LOOP_TIME_MS,
                                          appSingleEventCommissionTimer,
                                          NULL,
                                          0,
                                          0);
}

// ----------------------------------------------------------------------------
// ------------ END : Application events and actions --------------------------
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// -------------- 4. Application Button ---------------------------------------
// ----------------------------------------------------------------------------
#if defined EMBER_AF_PLUGIN_BUTTON
#if BSP_BUTTON_COUNT == 2
void halButtonIsr(uint8_t button, uint8_t state)
{
  uint8_t botton0State = halButtonState(BSP_BUTTON0_PIN);
  uint8_t botton1State = halButtonState(BSP_BUTTON1_PIN);
  if (botton0State == BUTTON_PRESSED && botton1State == BUTTON_PRESSED) {
    appAction = APP_EVENT_ACTION_SEND_DECOMMISSION;
  } else if (botton0State == BUTTON_PRESSED) {
    appAction = APP_EVENT_ACTION_SEND_COMMISSION;
  } else if (botton1State == BUTTON_PRESSED) {
    appAction = APP_EVENT_ACTION_SEND_REPORT;
  }
}
#elif BSP_BUTTON_COUNT == 1
static sl_sleeptimer_timer_handle_t button_release_timer_handle;
static void buttonReleaseTimeout(sl_sleeptimer_timer_handle_t *handle, void *contextData)
{
  sl_sleeptimer_stop_timer(handle);
  appAction = APP_EVENT_ACTION_SEND_DECOMMISSION;
}
void halButtonIsr(uint8_t button, uint8_t state)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  if (state == BUTTON_PRESSED) {
    if (gpd->gpdState < EMBER_GPD_APP_STATE_OPERATIONAL) {
      appAction = APP_EVENT_ACTION_SEND_COMMISSION;
    } else {
      appAction = APP_EVENT_ACTION_SEND_REPORT;
    }
    sl_sleeptimer_restart_timer_ms(&button_release_timer_handle,
                                   5000,
                                   buttonReleaseTimeout,
                                   NULL,
                                   0,
                                   0);
  } else {
    sl_sleeptimer_stop_timer(&button_release_timer_handle);
  }
}
#endif
#endif
// ----------------------------------------------------------------------------
// ----------- END : Application Button ---------------------------------------
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// ----------- Periodic Reporting  --------------------------------------------
// ----------------------------------------------------------------------------
static sl_sleeptimer_timer_handle_t report_timer_handle;
// Periodic Timer for reporting
static void reportTimeCallback(sl_sleeptimer_timer_handle_t *handle, void *contextData)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  if (gpd->gpdState >= EMBER_GPD_APP_STATE_OPERATIONAL) {
    appAction = APP_EVENT_ACTION_SEND_REPORT;
  } else {
    sl_sleeptimer_stop_timer(handle);
  }
}

static void reportData(uint32_t reportInterval_ms)
{
  bool running = false;
  sl_sleeptimer_is_timer_running(&report_timer_handle, &running);
  if (running == false) {
    sl_sleeptimer_restart_periodic_timer_ms(&report_timer_handle,
                                            reportInterval_ms,
                                            reportTimeCallback,
                                            NULL,
                                            0,
                                            0);
  }
}
// ----------------------------------------------------------------------------
// ----------- END : Periodic Reporting  --------------------------------------
// ----------------------------------------------------------------------------
#if defined(EMBER_AF_PLUGIN_COMMAND_INTERPRETER2)
static bool sleepy = false;
void emberGpdAfCliSensorReport(void)
{
  appAction = APP_EVENT_ACTION_SEND_REPORT;
}
void emberGpdAfCliSensorSleep(void)
{
  // enable sleep
  sleepy = true;
}
#else
static bool sleepy = true;
#endif
// ----------------------------------------------------------------------------
// 5. Application main state machine.
// ----------------------------------------------------------------------------
/** @brief Called by framework from the application main enry to inform the user
 * as the first call to the main.
 *
 */
void emberGpdAfPluginMainCallback(EmberGpd_t * gpd)
{
#if defined(EMBER_AF_PLUGIN_SERIAL)
  emberSerialInit(BSP_SERIAL_APP_PORT, HAL_SERIAL_APP_BAUD_RATE, 0, 1);
  emberSerialGuaranteedPrintf(BSP_SERIAL_APP_PORT, "Reset info: 0x%x (%p)\n",
                              halGetResetInfo(),
                              halGetResetString());
#if defined(EXTENDED_RESET_INFO)
  emberSerialGuaranteedPrintf(BSP_SERIAL_APP_PORT, "Extended Reset info: 0x%2X (%p)",
                              halGetExtendedResetInfo(),
                              halGetExtendedResetString());

  if (halResetWasCrash()) {
    halPrintCrashSummary(serialPort);
    halPrintCrashDetails(serialPort);
    halPrintCrashData(serialPort);
  }
#endif
#endif

  // Initialise Command Interpreter if enabled
  COMMAND_READER_INIT();

  // Initialise timer for application state machine with sleep consideration
  sl_status_t initStatus = sl_sleeptimer_init();
  // Trap if the sleep timer initialisation fails
  while (initStatus != SL_STATUS_OK) ;

  gpdDebugPrintf("GPD Main\n");
  while (true) {
    // Periodically reset the watch dog.
    halResetWatchdog();

    // GPD state LED indication if enabled.
    if (gpd->gpdState < EMBER_GPD_APP_STATE_OPERATIONAL) {
      BOARD_LED_ON(COMMISSIONING_STATE_LED);
    } else {
      BOARD_LED_OFF(COMMISSIONING_STATE_LED);
    }

    // Process CLI
#if defined(EMBER_AF_PLUGIN_COMMAND_INTERPRETER2)
    if (emberProcessCommandInput(APP_SERIAL)) {
      gpdDebugPrintf(EMBER_AF_DEVICE_NAME ">");
    }
#endif

    // Periodic reporting in the operational state
    if (gpd->gpdState >= EMBER_GPD_APP_STATE_OPERATIONAL) {
      reportData(APP_GPD_REPORTING_TIME_MS);
    }

    // Process application actions
    if (appAction != APP_EVENT_ACTION_IDLE) {
      processAppEvent(gpd, &appAction);
    }

    // Enter sleep in sleepy mode, the wake up is on button activation or
    // or when periodic timer expires.
    if (sleepy) {
      gpdEnterLowPowerMode(false);
    }
  }
}
// ----------------------------------------------------------------------------
// ------------ END : Application main loop -----------------------------------
// ----------------------------------------------------------------------------
/** @brief Called from the imcomming command handler context for all the incoming
 *         command before the command handler handles it. based on the return code
 *         of this callback the internal command handler skips the processing.
 *
 * @param gpdCommand CommandId.
 * @param length length of the command
 * @param commandPayload The pointer to the commissioning reply payload.
 * Ver.: always
 *
 * @return true if application handled it.
 */
bool emberGpdAfPluginIncomingCommandCallback(uint8_t gpdCommand,
                                             uint8_t length,
                                             uint8_t * commandPayload)
{
  gpdDebugPrintf("RX: cmd=%x len=%x payload[", gpdCommand, length);
  if (commandPayload != NULL
      && length > 0
      && length < 0xFE) {
    for (int i = 0; i < length; i++) {
      gpdDebugPrintf("%x", commandPayload[i]);
    }
  }
  gpdDebugPrintf("]\n");
  return false;
}

//Provide a stub
void emberDebugAssert(const char * filename, int linenumber)
{
  gpdDebugPrintf("Assert file: %s Line:%d\n", filename, linenumber);
}

// Provide a stub for Series 1
#if !defined (_SILICON_LABS_32B_SERIES_2)
void sl_mac_lower_mac_radio_sleep(void)
{
}
#endif // !defined (_SILICON_LABS_32B_SERIES_2) // emRadioSleep
