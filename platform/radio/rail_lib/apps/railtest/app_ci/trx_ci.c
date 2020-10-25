/***************************************************************************//**
 * @file
 * @brief This file implements the tx/rx commands for RAILtest applications.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <string.h>

#if !defined(__ICCARM__)
// IAR doesn't have strings.h and puts those declarations in string.h
#include <strings.h>
#endif

#include "response_print.h"

#include "rail.h"
#include "app_common.h"
#include "app_trx.h"

#if defined(SL_CATALOG_IOSTREAM_USART_PRESENT)
#include "sl_iostream_usart_vcom_config.h"
#endif

#if defined(SL_COMPONENT_CATALOG_PRESENT)
  #include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_IOSTREAM_USART_PRESENT) && defined(SL_CATALOG_CLI_PRESENT)
  #if defined(SL_CLI_USE_STDIO)
    #include "sl_iostream.h"
    #define consumeChar()                               \
  do {                                                  \
    char ch;                                            \
    (void) sl_iostream_getchar(SL_IOSTREAM_STDIN, &ch); \
  } while (0)
  #else
    #define consumeChar() ((void) getchar())
  #endif // #if defined(SL_CLI_USE_STDIO)
#else
  #define consumeChar()
#endif // #if defined(SL_CATALOG_IOSTREAM_USART_PRESENT) && defined(SL_CATALOG_CLI_PRESENT)

RAIL_PacketTimePosition_t txTimePosition = RAIL_PACKET_TIME_DEFAULT;
RAIL_PacketTimePosition_t rxTimePosition = RAIL_PACKET_TIME_DEFAULT;

void tx(sl_cli_command_arg_t *args)
{
  uint32_t newTxCount = sl_cli_get_argument_uint32(args, 0);
  radioTransmit(newTxCount, sl_cli_get_command_string(args, 0));
}

//deprecated
void txWithOptions(sl_cli_command_arg_t *args)
{
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  tx(args);
}

static const char *configuredTxAntenna(RAIL_TxOptions_t txOptions)
{
  switch (txOptions & (RAIL_TX_OPTION_ANTENNA0 | RAIL_TX_OPTION_ANTENNA1)) {
    case (RAIL_TX_OPTION_ANTENNA0 | RAIL_TX_OPTION_ANTENNA1): {
      return "Any";
      break;
    }
    case (RAIL_TX_OPTION_ANTENNA0): {
      return "Antenna0";
      break;
    }
    case (RAIL_TX_OPTION_ANTENNA1): {
      return "Antenna1";
      break;
    }
    default: {
      return "Any";
      break;
    }
  }
}

void configTxOptions(sl_cli_command_arg_t *args)
{
  if (sl_cli_get_argument_count(args) >= 1) {
    txOptions = sl_cli_get_argument_uint32(args, 0);
  }

  responsePrint(sl_cli_get_command_string(args, 0), "waitForAck:%s,removeCrc:%s,syncWordId:%d,"
                                                    "txAntenna:%s,altPreambleLen:%s,ccaPeakRssi:%s,"
                                                    "ccaOnly:%s",
                ((txOptions & RAIL_TX_OPTION_WAIT_FOR_ACK) ? "True" : "False"),
                ((txOptions & RAIL_TX_OPTION_REMOVE_CRC) ? "True" : "False"),
                ((txOptions & RAIL_TX_OPTION_SYNC_WORD_ID) >> RAIL_TX_OPTION_SYNC_WORD_ID_SHIFT),
                configuredTxAntenna(txOptions),
                ((txOptions & RAIL_TX_OPTION_ALT_PREAMBLE_LEN) ? "True" : "False"),
                ((txOptions & RAIL_TX_OPTION_CCA_PEAK_RSSI) ? "True" : "False"),
                ((txOptions & RAIL_TX_OPTION_CCA_ONLY) ? "True" : "False"));
}

void txAtTime(sl_cli_command_arg_t *args)
{
  // DEFAULTS: 0 ms, absolute time, postponse tx during rx
  RAIL_ScheduleTxConfig_t scheduledTxOptions = { 0 };
  scheduledTxOptions.when = sl_cli_get_argument_uint32(args, 0);

  // Attempt to parse the time mode if specified
  if (sl_cli_get_argument_count(args) >= 2) {
    if (!parseTimeModeFromString(sl_cli_get_argument_string(args, 1), &scheduledTxOptions.mode)) {
      responsePrintError(sl_cli_get_command_string(args, 0), 28, "Invalid time mode");
      return;
    }
  }

  scheduledTxOptions.txDuringRx =
    (sl_cli_get_argument_count(args) >= 3 && strcasecmp("abort", sl_cli_get_argument_string(args, 2)) == 0)
    ? RAIL_SCHEDULED_TX_DURING_RX_ABORT_TX
    : RAIL_SCHEDULED_TX_DURING_RX_POSTPONE_TX;

  setNextPacketTime(&scheduledTxOptions);
  setNextAppMode(TX_SCHEDULED, sl_cli_get_command_string(args, 0));
}

void txAfterRx(sl_cli_command_arg_t *args)
{
  uint32_t delay = sl_cli_get_argument_uint32(args, 0);
  txAfterRxDelay = delay;
  enableAppMode(SCHTX_AFTER_RX, (delay != 0), sl_cli_get_command_string(args, 0));
}

void getTxDelay(sl_cli_command_arg_t *args)
{
  responsePrint(sl_cli_get_command_string(args, 0), "txDelay:%d", continuousTransferPeriod);
}

void setTxDelay(sl_cli_command_arg_t *args)
{
  uint32_t delay = sl_cli_get_argument_uint32(args, 0);

  continuousTransferPeriod = delay;
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  getTxDelay(args);
}

void setTxHoldOff(sl_cli_command_arg_t *args)
{
  uint32_t holdOff = sl_cli_get_argument_uint32(args, 0);

  RAIL_EnableTxHoldOff(railHandle, holdOff);
  responsePrint(sl_cli_get_command_string(args, 0), "TxHoldOff:%s",
                (RAIL_IsTxHoldOffEnabled(railHandle) ? "Enabled" : "Disabled"));
}

void setTxAltPreambleLen(sl_cli_command_arg_t *args)
{
  uint16_t length = sl_cli_get_argument_uint16(args, 0);

  RAIL_Status_t status = RAIL_SetTxAltPreambleLength(railHandle, length);
  responsePrint(sl_cli_get_command_string(args, 0), "Result:%s",
                ((status == RAIL_STATUS_NO_ERROR) ? "Success" : "Failure"));
}

void stopInfinitePreambleTx(sl_cli_command_arg_t *args)
{
  RAIL_Status_t status = RAIL_StopInfinitePreambleTx(railHandle);
  responsePrint(sl_cli_get_command_string(args, 0), "Result:%s",
                ((status == RAIL_STATUS_NO_ERROR) ? "Success"
                 : (status == RAIL_STATUS_INVALID_CALL) ? "Invalid Call"
                 : (status == RAIL_STATUS_INVALID_STATE) ? "Invalid State"
                 : "Failure"
                ));
}

void getSyncWords(sl_cli_command_arg_t *args)
{
  RAIL_SyncWordConfig_t syncWordConfig = { 0, };
  RAIL_Status_t status = RAIL_GetSyncWords(railHandle, &syncWordConfig);
  responsePrint(sl_cli_get_command_string(args, 0),
                "Result:%s,bitlength:%u,syncWord1:%lu,syncWord2:%lu",
                status == RAIL_STATUS_NO_ERROR ? "Success" : "Failure",
                syncWordConfig.syncWordBits, syncWordConfig.syncWord1,
                syncWordConfig.syncWord2);
}

void configSyncWords(sl_cli_command_arg_t *args)
{
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }
  RAIL_SyncWordConfig_t syncWordConfig = { 0, };
  syncWordConfig.syncWordBits = sl_cli_get_argument_uint8(args, 0);
  syncWordConfig.syncWord1 = sl_cli_get_argument_uint32(args, 1);
  if (sl_cli_get_argument_count(args) >= 3) {
    syncWordConfig.syncWord2 = sl_cli_get_argument_uint32(args, 2);
  } else {
    syncWordConfig.syncWord2 = syncWordConfig.syncWord1;
  }
  RAIL_Status_t status = RAIL_ConfigSyncWords(railHandle, &syncWordConfig);
  responsePrint(sl_cli_get_command_string(args, 0), "Result:%s",
                ((status == RAIL_STATUS_NO_ERROR) ? "Success" : "Failure"));
}

void rx(sl_cli_command_arg_t *args)
{
  bool enable = !!sl_cli_get_argument_uint8(args, 0);
  RAIL_Status_t status = RAIL_STATUS_NO_ERROR;

  // Don't allow Rx enable calls when scheduled Rx has been started
  if ((currentAppMode() == RX_SCHEDULED) && enable) {
    responsePrintError(sl_cli_get_command_string(args, 0), 29, "Can't turn on receive when in ScheduledRx");
    return;
  }

  // Only allow calls to Rx if we're in direct mode, no mode, or this is a
  // scheduled Rx disable call
  if ((currentAppMode() != DIRECT)
      && (currentAppMode() != RX_SCHEDULED)
      && (!inAppMode(NONE, sl_cli_get_command_string(args, 0)))) {
    return;
  }

  // Do the minimum amount of work to get into the correct state
  RAIL_RadioState_t currentState = RAIL_GetRadioState(railHandle);
  if ((enable && (currentState & RAIL_RF_STATE_RX))
      || (!enable && (currentState <= RAIL_RF_STATE_IDLE))) {
    // Do nothing since we're already in the right state
  } else if (enable) {
    status = RAIL_StartRx(railHandle, channel, NULL);
  } else {
    RAIL_Idle(railHandle, RAIL_IDLE_ABORT, false);

    // Turn off ScheduledRx if we were in it
    if (currentAppMode() == RX_SCHEDULED) {
      (void) enableAppModeSync(RX_SCHEDULED, false, NULL);
    }
  }
  if (status == RAIL_STATUS_NO_ERROR) {
    receiveModeEnabled = enable;

    // Print out the current status of receive mode
    responsePrint(sl_cli_get_command_string(args, 0),
                  "Rx:%s,Idle:%s,Time:%u",
                  (enable ? "Enabled" : "Disabled"),
                  ((!enable) ? "Enabled" : "Disabled"),
                  RAIL_GetTime());
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0),
                       40,
                       "Could not change receive state '%d'",
                       status);
  }
}

void rxAt(sl_cli_command_arg_t *args)
{
  uint32_t startTime, endTime;
  RAIL_TimeMode_t startMode, endMode;
  bool rxTransitionEndSchedule = false, hardEnd = false;
  bool scheduledRxUpdate = false;
  RAIL_ScheduleRxConfig_t rxCfg;

  // Only allow this when app is idle or to reconfigure an active scheduled Rx
  if (!inAppMode(NONE, NULL) && !inAppMode(RX_SCHEDULED, NULL)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 30, "Cannot enter ScheduledRx when not in Idle.");
    return;
  }

  // Parse out the command line parameters
  startTime = sl_cli_get_argument_uint32(args, 0);
  endTime   = sl_cli_get_argument_uint32(args, 2);

  if (!parseTimeModeFromString(sl_cli_get_argument_string(args, 1), &startMode)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 25, "Invalid startMode specified");
    return;
  }
  if (!parseTimeModeFromString(sl_cli_get_argument_string(args, 3), &endMode)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 26, "Invalid endMode specified");
    return;
  }

  // Read the rxTransitionEndSchedule option if available
  if (sl_cli_get_argument_count(args) >= 5) {
    rxTransitionEndSchedule = !!sl_cli_get_argument_uint8(args, 4);
  }
  // Read the hardEnd option if available
  if (sl_cli_get_argument_count(args) >= 6) {
    hardEnd = !!sl_cli_get_argument_uint8(args, 5);
  }

  // Configure scheduled receive as requested
  rxCfg.start = startTime;
  rxCfg.startMode = startMode;
  rxCfg.end = endTime;
  rxCfg.endMode = endMode;
  rxCfg.rxTransitionEndSchedule = rxTransitionEndSchedule;
  rxCfg.hardWindowEnd = hardEnd;

  // Tell the app what we're doing so it can correctly track the end of the
  // scheduled receive state
  schRxStopOnRxEvent = rxTransitionEndSchedule;

  // Store whether this is an update or not
  if (inAppMode(RX_SCHEDULED, NULL)) {
    scheduledRxUpdate = true;
  }

  if (!scheduledRxUpdate) {
    // Attempt to put the app into scheduled receive mode
    if (!enableAppModeSync(RX_SCHEDULED, true, sl_cli_get_command_string(args, 0))) {
      return;
    }
  }

  // Enable scheduled receive mode
  uint8_t res = RAIL_ScheduleRx(railHandle, channel, &rxCfg, NULL);
  if (res != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 27, "Could not start scheduled receive %d", res);
    if (!scheduledRxUpdate) {
      (void) enableAppModeSync(RX_SCHEDULED, false, sl_cli_get_command_string(args, 0));
    }
    return;
  }
}

void setRxOptions(sl_cli_command_arg_t *args)
{
  // Only update the rxOptions if a parameter is given otherwise just print the
  // current settings
  if (sl_cli_get_argument_count(args) >= 1) {
    RAIL_RxOptions_t newRxOptions = sl_cli_get_argument_uint32(args, 0);
    RAIL_Status_t status = RAIL_ConfigRxOptions(railHandle,
                                                RAIL_RX_OPTIONS_ALL,
                                                newRxOptions);

    // Make sure there was no error setting the new options
    if (status != RAIL_STATUS_NO_ERROR) {
      responsePrintError(sl_cli_get_command_string(args, 0), 31, "RxOptions:Failed");
      return;
    }
    // Update the global rxOptions
    rxOptions = newRxOptions;
  }

  responsePrint(sl_cli_get_command_string(args, 0),
                "storeCrc:%s,ignoreCrcErrors:%s,enableDualSync:%s,"
                "trackAborted:%s,removeAppendedInfo:%s,rxAntenna:%s,"
                "frameDet:%s",
                (rxOptions & RAIL_RX_OPTION_STORE_CRC) ? "True" : "False",
                (rxOptions & RAIL_RX_OPTION_IGNORE_CRC_ERRORS) ? "True" : "False",
                (rxOptions & RAIL_RX_OPTION_ENABLE_DUALSYNC) ? "True" : "False",
                (rxOptions & RAIL_RX_OPTION_TRACK_ABORTED_FRAMES) ? "True" : "False",
                (rxOptions & RAIL_RX_OPTION_REMOVE_APPENDED_INFO) ? "True" : "False",
                configuredRxAntenna(rxOptions),
                (rxOptions & RAIL_RX_OPTION_DISABLE_FRAME_DETECTION) ? "Off" : "On");
}

void setTxTone(sl_cli_command_arg_t *args)
{
  uint8_t enable = sl_cli_get_argument_uint8(args, 0);
  streamMode = RAIL_STREAM_CARRIER_WAVE;
  antOptions = RAIL_TX_OPTIONS_DEFAULT;
  if (sl_cli_get_argument_count(args) >= 2) {
    uint8_t antenna = sl_cli_get_argument_uint32(args, 1);
    //Choose any antenna by default
    if (antenna == 1) {
      antOptions = RAIL_TX_OPTION_ANTENNA1;
    } else {
      antOptions = RAIL_TX_OPTION_ANTENNA0;
    }
  }
  enableAppMode(TX_STREAM, enable, sl_cli_get_command_string(args, 0));
}

void setTxStream(sl_cli_command_arg_t *args)
{
  uint8_t enable = sl_cli_get_argument_uint8(args, 0);
  RAIL_StreamMode_t stream = RAIL_STREAM_PN9_STREAM;
  antOptions = RAIL_TX_OPTIONS_DEFAULT;
  if (sl_cli_get_argument_count(args) >= 3) {
    uint8_t antenna = sl_cli_get_argument_uint32(args, 2);
    //Choose any antenna by default
    if (antenna == 1) {
      antOptions = RAIL_TX_OPTION_ANTENNA1;
    } else {
      antOptions = RAIL_TX_OPTION_ANTENNA0;
    }
  }
  if (sl_cli_get_argument_count(args) >= 2) {
    stream = (RAIL_StreamMode_t) sl_cli_get_argument_uint32(args, 1);
    if (stream >= RAIL_STREAM_MODES_COUNT) {
      responsePrintError(sl_cli_get_command_string(args, 0), 1, "Invalid stream mode");
      return;
    }
  }
  streamMode = stream;
  enableAppMode(TX_STREAM, enable, sl_cli_get_command_string(args, 0));
}

void setDirectMode(sl_cli_command_arg_t *args)
{
  uint8_t enable = sl_cli_get_argument_uint8(args, 0);
  enableAppMode(DIRECT, enable, sl_cli_get_command_string(args, 0));
}

void setDirectTx(sl_cli_command_arg_t *args)
{
  uint8_t enable = sl_cli_get_argument_uint8(args, 0);
  RAIL_Status_t status = RAIL_STATUS_NO_ERROR;

  // Make sure that direct mode is enabled to do a direct Tx
  if (currentAppMode() != DIRECT) {
    responsePrintError(sl_cli_get_command_string(args, 0), 7, "DirectMode not enabled");
    return;
  }

  // Stop whatever we were doing so we can go into Tx
  RAIL_Idle(railHandle, RAIL_IDLE_ABORT, false);

  // Either enable or disable the transmitter
  if (enable) {
    // Turn on Tx
    status = RAIL_StartTx(railHandle, channel, RAIL_TX_OPTIONS_DEFAULT, NULL);
  } else {
    if (receiveModeEnabled) {
      status = RAIL_StartRx(railHandle, channel, NULL);
    }
  }
  if (status == RAIL_STATUS_NO_ERROR) {
    responsePrint(sl_cli_get_command_string(args, 0), "DirectTx:%s", (enable ? "Enabled" : "Disabled"));
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 8, "DirectMode Rx/Tx not enabled '%d'", status);
  }
}

#include <stdio.h>
#include "em_gpio.h"
#include "em_emu.h"
#include "em_core.h"

#ifndef DEBUG_SLEEP_LOOP
#define DEBUG_SLEEP_LOOP 0
#endif//DEBUG_SLEEP_LOOP

static const char *rfBands[] = { "Off", "GHz", "MHz", "Any", };
static const char *rfSensitivity[] = { "High", "Low" };
static RAIL_RfSenseBand_t rfBand = RAIL_RFSENSE_OFF;
static uint32_t rfUs = 0;

static void RAILCb_SensedRf(void)
{
  counters.rfSensedEvent++;
  if (counters.rfSensedEvent == 0) { // Wrap it to 1 not 0
    counters.rfSensedEvent = 1;
  }
}

void sleep(sl_cli_command_arg_t *args)
{
  char* em4State = "";
  void (*em4Function)(void) = &EMU_EnterEM4;

  uint8_t emMode = sl_cli_get_argument_uint8(args, 0);
  uint8_t rfSenseSyncWordNumBytes = 0U;
  uint32_t rfSenseSyncWord = 0U;
  RailRfSenseMode_t mode = RAIL_RFSENSE_MODE_OFF;
  bool enableCb = (emMode & 0x80) ? false : true;
  emMode &= ~0x80;

  if (emMode <= 4) {
    if (emMode == 4) {
#ifdef _SILICON_LABS_32B_SERIES_1
      switch (sl_cli_get_argument_string(args, 0)[1]) {
        case 's': case 'S': // Configure EM4 Shutoff state
          em4Function = &EMU_EnterEM4S;
          em4State = "s";
          break;
        case 'h': case 'H': // Configure EM4 Hibernate state
          em4Function = &EMU_EnterEM4H;
          em4State = "h";
          break;
        default:
          // Use whatever prior EM4 state was specified
          em4State = (((EMU->EM4CTRL & EMU_EM4CTRL_EM4STATE)
                       == EMU_EM4CTRL_EM4STATE_EM4S) ? "s" : "h");
          break;
      }
#elif _SILICON_LABS_32B_SERIES_2_CONFIG >= 1
      em4State = "s"; // Only EM4 Shutoff supported
#endif
    }
    // Check for Selective(OOK) Mode
    if (sl_cli_get_argument_count(args) >= 4) {
#if _SILICON_LABS_32B_SERIES_2_CONFIG == 2
      rfSenseSyncWordNumBytes = (sl_cli_get_argument_uint8(args, 1) > 4)
                                ? 0 : sl_cli_get_argument_uint32(args, 1);
      rfSenseSyncWord = sl_cli_get_argument_uint32(args, 2);
      rfBand = ((RAIL_RfSenseBand_t) sl_cli_get_argument_uint32(args, 3)
                & RAIL_RFENSE_ANY_LOW_SENSITIVITY); // mask off illegal values
      mode = ((rfBand != RAIL_RFSENSE_OFF) && (rfSenseSyncWordNumBytes > 0))
             ? RAIL_RFSENSE_MODE_SELECTIVE_OOK : RAIL_RFSENSE_MODE_OFF;
#else
      responsePrintError(sl_cli_get_command_string(args, 0), 0x15, "RFSENSE Selective OOK Mode Unsupported");
      return;
#endif
    } else {
      // sleep is MODAL -- we'll block here in foreground.
      // If there are new RfSense parameters, grab 'em:
      if (sl_cli_get_argument_count(args) >= 2) {
#if _SILICON_LABS_32B_SERIES_2_CONFIG == 1
        responsePrintError(sl_cli_get_command_string(args, 0), 0x15, "RFSENSE Energy Detect Mode Unsupported");
        return;
#else
        rfUs = sl_cli_get_argument_uint32(args, 1);
        if (sl_cli_get_argument_count(args) >= 3) {
          rfBand = ((RAIL_RfSenseBand_t) sl_cli_get_argument_uint32(args, 2)
                    & RAIL_RFENSE_ANY_LOW_SENSITIVITY); // mask off illegal values
        }
        mode = ((rfBand != RAIL_RFSENSE_OFF) && (rfUs > 0))
               ? RAIL_RFSENSE_MODE_ENERGY_DETECTION : RAIL_RFSENSE_MODE_OFF;
#endif // _SILICON_LABS_32B_SERIES_2_CONFIG == 1
      }
    }
    // Only call if we plan to enable RFSENSE later.
    if (mode != RAIL_RFSENSE_MODE_OFF) {
      if (!enableAppModeSync(RF_SENSE, true, sl_cli_get_command_string(args, 0))) {
        return;
      }
    } else {
      rfBand = RAIL_RFSENSE_OFF; // rfUs == 0
    }

    // Shut down Peripherals for EM2+ sleep. If the radio is still enabled,
    // the chip will go into EM1P if supported
    if (emMode >= 2) {
      PeripheralDisable();
      GPIO_EM4SetPinRetention(true);
    }

    // We cannot configure UART RxD for EM4 wakeup on our EFR32's so the
    // *only* wakeup possible out of EM4 is RFsense (or reset).
    responsePrint(sl_cli_get_command_string(args, 0), "EM:%u%s,SerialWakeup:%s,RfSense:%s,RfSensitivity:%s",
                  emMode, em4State,
                  (emMode < 4) ? "On" : "Off",
                  rfBands[rfBand & RAIL_RFSENSE_ANY],
                  rfSensitivity[(rfBand & 0x20U) >> 5U]);
    serialWaitForTxIdle();

    // Disable interrupts heading into RAIL_StartRfSense() so we don't miss
    // the event occurring before we try to sleep.
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_CRITICAL();

#ifdef _SILICON_LABS_32B_SERIES_2
    // Sleep the USART Tx pin on series 2 devices to save energy
    if (emMode >= 2) {
      GPIO_PinModeSet(SL_IOSTREAM_USART_VCOM_TX_PORT,
                      SL_IOSTREAM_USART_VCOM_TX_PIN,
                      gpioModeDisabled, 1);
    }
#endif

    switch (mode) {
      case RAIL_RFSENSE_MODE_SELECTIVE_OOK:
      {
        RAIL_RfSenseSelectiveOokConfig_t config = {
          .band = rfBand,
          .syncWordNumBytes = rfSenseSyncWordNumBytes,
          .syncWord = rfSenseSyncWord,
          .cb = enableCb ? (&RAILCb_SensedRf) : NULL
        };
        (void) RAIL_StartSelectiveOokRfSense(railHandle, &config);
        break;
      }
      case RAIL_RFSENSE_MODE_ENERGY_DETECTION:
        rfUs = RAIL_StartRfSense(railHandle, rfBand,
                                 rfUs, enableCb ? (&RAILCb_SensedRf) : NULL);
        if (rfUs != 0) {
          break;
        }
      // Fall through
      case RAIL_RFSENSE_MODE_OFF:
        if (currentAppMode() == RF_SENSE) {
          enableAppMode(RF_SENSE, false, NULL);
        }
        break;
    }

    // Configure the USART Rx pin as a GPIO interrupt for sleep-wake purposes,
    // falling-edge only
    GPIO_IntConfig(SL_IOSTREAM_USART_VCOM_RX_PORT,
                   SL_IOSTREAM_USART_VCOM_RX_PIN,
                   false, true, true);

    serEvent = false;
    rxPacketEvent = false;

    bool rfSensed;
   #if     DEBUG_SLEEP_LOOP
    uint64_t sleeps = 0;
    typedef struct wakeReasons{
      uint32_t scbInts;
      uint32_t nvicInts[2];
      uint32_t gpioInts;
    } wakeReasons_t;
    // Maintain the most recent two wakeup events in 'circular' list
    wakeReasons_t wakeReasons[2] = { { 0, 0, }, { 0, 0, }, };
   #endif//DEBUG_SLEEP_LOOP

    do { // Loop modally here until either RfSense or Serial event occurs
         //@TODO Should we WDOG_Feed()??
      switch (emMode) {
        case 0:                      break;
        case 1:  EMU_EnterEM1(    ); break;
        case 2:  EMU_EnterEM2(true); break;
        case 3:  EMU_EnterEM3(true); break;
        case 4:  em4Function(    );  break; // NOTREACHED
        default:                     break;
      }
      // After waking, interrupts need to be re-enabled so ISRs can run
     #if     DEBUG_SLEEP_LOOP
      // But grab why we woke first (of course this is meaningless for EM0):
      sleeps++;
      wakeReasons[sleeps & 1].gpioInts = GPIO_IntGetEnabled();
      // No NVIC_ API for this
      wakeReasons[sleeps & 1].nvicInts[0] = NVIC->ISPR[0] /*& NVIC->ISER[0]*/;
      wakeReasons[sleeps & 1].nvicInts[1] = NVIC->ISPR[1] /*& NVIC->ISER[1]*/;
      wakeReasons[sleeps & 1].scbInts = SCB->ICSR;
     #endif//DEBUG_SLEEP_LOOP
      CORE_EXIT_CRITICAL(); // Briefly enable IRQs to let them run
      CORE_ENTER_CRITICAL(); // but shut back off in case we loop
      rfSensed = RAIL_IsRfSensed(railHandle);
    } while (!rfSensed && !serEvent && !rxPacketEvent);

    // Disable serial interrupt so it's not bothersome
    GPIO_IntDisable(1U << SL_IOSTREAM_USART_VCOM_RX_PIN);
    GPIO_IntClear(1U << SL_IOSTREAM_USART_VCOM_RX_PIN);

    CORE_EXIT_CRITICAL(); // Back on permanently

    // Here we've awoken for at least one of the desired events.
    // If we woke on serial, don't need RFSense active any more.
    if (currentAppMode() == RF_SENSE) {
      enableAppMode(RF_SENSE, false, NULL);
    }

#ifdef _SILICON_LABS_32B_SERIES_2
    // Wake the USART Tx pin back up
    if (emMode >= 2) {
      GPIO_PinModeSet(SL_IOSTREAM_USART_VCOM_TX_PORT,
                      SL_IOSTREAM_USART_VCOM_TX_PIN,
                      gpioModePushPull, 1);
    }
#endif

    // eliminate compiler warning (using multiple volatile variables below)
    bool serEventParam = serEvent;
    bool rxPacketEventParam = rxPacketEvent;

    responsePrint("sleepWoke",
                  "EM:%u%s,"
                  "SerialWakeup:%s,"
                  "RfSensed:%s,"
                  "PacketRx:%s,"
                  "RfUs:%u"
                 #if     DEBUG_SLEEP_LOOP
                  ",\nSleeps:%llu,"
                  "\nScb[-]:0x%08x,"
                  "Nvic[-]:0x%x.%08x,Gpios[-]:0x%08x,"
                  "\nScb[0]:0x%08x,"
                  "Nvic[0]:0x%x.%08x,Gpios[0]:0x%08x"
                 #endif//DEBUG_SLEEP_LOOP
                  , emMode,
                  em4State,
                  serEventParam ? "Yes" : "No",
                  rfSensed ? "Yes" : "No",
                  rxPacketEventParam ? "Yes" : "No",
                  rfUs
                 #if     DEBUG_SLEEP_LOOP
                  , sleeps,
                  wakeReasons[(sleeps - 1) & 1].scbInts,
                  wakeReasons[(sleeps - 1) & 1].nvicInts[1],
                  wakeReasons[(sleeps - 1) & 1].nvicInts[0],
                  wakeReasons[(sleeps - 1) & 1].gpioInts,
                  wakeReasons[(sleeps) & 1].scbInts,
                  wakeReasons[(sleeps) & 1].nvicInts[1],
                  wakeReasons[(sleeps) & 1].nvicInts[0],
                  wakeReasons[(sleeps) & 1].gpioInts
                 #endif//DEBUG_SLEEP_LOOP
                  );

    if (serEvent) { // Consume the character entered
      if (emMode >= 2) {
        // If UART was shut down, delay and gobble likely junk
        usDelay(250000); // Pause for 250 ms
        consumeChar();
        consumeChar();
        consumeChar();
        consumeChar();
      }
    }

    // Restart Rx if we're in Rx mode
    if (receiveModeEnabled) {
      if (RAIL_StartRx(railHandle, channel, NULL) != RAIL_STATUS_NO_ERROR) {
        responsePrintError(sl_cli_get_command_string(args, 0), 1, "Could not start receive after sleep");
      }
    }
    // Restart peripherals if they were active before sleeping
    if ((emMode >= 2) && (logLevel & PERIPHERAL_ENABLE)) {
      PeripheralEnable();
    }
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 1, "Invalid EM mode %u (valid 0-4)", emMode);
  }
}

void rfSense(sl_cli_command_arg_t *args)
{
#if _SILICON_LABS_32B_SERIES_2_CONFIG == 1
  responsePrintError(sl_cli_get_command_string(args, 0), 0x15, "RFSENSE Unsupported");
  return;
#else
  RailRfSenseMode_t mode = RAIL_RFSENSE_MODE_OFF;
  uint8_t rfSenseSyncWordNumBytes = 0U;
  uint32_t rfSenseSyncWord = 0U;
  // Check for Selective(OOK) Mode
  if (sl_cli_get_argument_count(args) >= 3) {
#if _SILICON_LABS_32B_SERIES_2_CONFIG == 2
    rfSenseSyncWordNumBytes = (sl_cli_get_argument_uint8(args, 0) > 4)
                              ? 0 : sl_cli_get_argument_uint32(args, 0);
    rfSenseSyncWord = sl_cli_get_argument_uint32(args, 1);
    rfBand = ((RAIL_RfSenseBand_t) sl_cli_get_argument_uint32(args, 2)
              & RAIL_RFENSE_ANY_LOW_SENSITIVITY); // mask off illegal values
    mode = ((rfBand != RAIL_RFSENSE_OFF) && (rfSenseSyncWordNumBytes > 0))
           ? RAIL_RFSENSE_MODE_SELECTIVE_OOK : RAIL_RFSENSE_MODE_OFF;
#else
    responsePrintError(sl_cli_get_command_string(args, 0), 0x15, "RFSENSE Selective OOK Mode Unsupported");
    return;
#endif
  } else {
    // If there are RfSense parameters, grab 'em; otherwise use prior settings
    if (sl_cli_get_argument_count(args) >= 1) {
      rfUs = sl_cli_get_argument_uint32(args, 0);
      if (sl_cli_get_argument_count(args) >= 2) {
        rfBand = ((RAIL_RfSenseBand_t) sl_cli_get_argument_uint32(args, 1)
                  & RAIL_RFENSE_ANY_LOW_SENSITIVITY); // mask off illegal values
      }
      mode = ((rfBand != RAIL_RFSENSE_OFF) && (rfUs > 0))
             ? RAIL_RFSENSE_MODE_ENERGY_DETECTION : RAIL_RFSENSE_MODE_OFF;
    }
  }

  if (!enableAppModeSync(RF_SENSE, mode != RAIL_RFSENSE_MODE_OFF, sl_cli_get_command_string(args, 0))) {
    return;
  }

  switch (mode) {
    case RAIL_RFSENSE_MODE_SELECTIVE_OOK:
    {
      RAIL_RfSenseSelectiveOokConfig_t config = {
        .band = rfBand,
        .syncWordNumBytes = rfSenseSyncWordNumBytes,
        .syncWord = rfSenseSyncWord,
        .cb = &RAILCb_SensedRf
      };
      (void) RAIL_StartSelectiveOokRfSense(railHandle, &config);
      break;
    }
    case RAIL_RFSENSE_MODE_ENERGY_DETECTION:
      rfUs = RAIL_StartRfSense(railHandle, rfBand, rfUs, &RAILCb_SensedRf);
      if (rfUs != 0) {
        break;
      }
    // Fall through
    case RAIL_RFSENSE_MODE_OFF:
      rfBand = RAIL_RFSENSE_OFF;
      if (currentAppMode() == RF_SENSE) {
        enableAppMode(RF_SENSE, false, NULL);
      }
      break;
  }

  responsePrint(sl_cli_get_command_string(args, 0), "RfBand:%s,RfUs:%u,RfSensitivity:%s",
                rfBands[rfBand & RAIL_RFSENSE_ANY],
                rfUs,
                rfSensitivity[(rfBand & 0x20U) >> 5U]);
#endif // _SILICON_LABS_32B_SERIES_2_CONFIG == 1
}

void rfSensedCheck(void)
{
  // If in RfSensing background mode, see if Rf got sensed
  if ((currentAppMode() == RF_SENSE) && RAIL_IsRfSensed(railHandle)) {
    // Yes, it did!  Terminate RfSensing background mode
    enableAppMode(RF_SENSE, false, NULL);
    responsePrint("rfSensedCheck", "RfSensed:%s,RfUs:%u",
                  rfBands[rfBand & RAIL_RFSENSE_ANY], rfUs);
  }
}

/**
 * Routine to parse a RAIL timer mode from a string.
 *
 * @param str The string to parse. Must be one of the following:
 * - 'rel': RAIL_TIME_DELAY
 * - 'abs': RAIL_TIME_ABSOLUTE
 * - 'dis': RAIL_TIME_DISABLED
 * @param mode The RAIL_TimeMode_t variable to fill in with the result of this
 * conversion.
 * @return Returns true if we were able to successfully convert the string into
 * a time mode and false if we could not.
 */
bool parseTimeModeFromString(char *str, RAIL_TimeMode_t *mode)
{
  if (strcasecmp("dis", str) == 0) {
    *mode = RAIL_TIME_DISABLED;
  } else if (strcasecmp("rel", str) == 0) {
    *mode = RAIL_TIME_DELAY;
  } else if (strcasecmp("abs", str) == 0) {
    *mode = RAIL_TIME_ABSOLUTE;
  } else {
    *mode = RAIL_TIME_DISABLED;
    return false;
  }
  return true;
}

// Must match RAIL_TimePosition_t enum values as indices:
static const char *timePosStrings[RAIL_PACKET_TIME_COUNT] = {
  "INVALID",
  "DEFAULT",
  "PREAMBLE_START",
  "PREAMBLE_START_UT",
  "SYNC_END",
  "SYNC_END_UT",
  "PACKET_END",
  "PACKET_END_UT",
};

void setTxTimePos(sl_cli_command_arg_t *args)
{
  uint32_t pos = sl_cli_get_argument_uint32(args, 0);
  if (pos < RAIL_PACKET_TIME_COUNT) {
    txTimePosition = pos;
    responsePrint(sl_cli_get_command_string(args, 0), "txTimePos:%s", timePosStrings[pos]);
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 1, "Invalid TimePosition %u (valid 0-%u)",
                       pos, RAIL_PACKET_TIME_COUNT - 1);
  }
}

void setRxTimePos(sl_cli_command_arg_t *args)
{
  uint32_t pos = sl_cli_get_argument_uint32(args, 0);
  if (pos < RAIL_PACKET_TIME_COUNT) {
    rxTimePosition = pos;
    responsePrint(sl_cli_get_command_string(args, 0), "rxTimePos:%s", timePosStrings[pos]);
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 1, "Invalid TimePosition %u (valid 0-%u)",
                       pos, RAIL_PACKET_TIME_COUNT - 1);
  }
}

void holdRx(sl_cli_command_arg_t *args)
{
  rxHeld = !!sl_cli_get_argument_uint8(args, 0);
  rxProcessHeld = !rxHeld;
  responsePrint(sl_cli_get_command_string(args, 0), "HoldRx:%s",
                rxHeld ? "Enabled" : "Disabled");
}
