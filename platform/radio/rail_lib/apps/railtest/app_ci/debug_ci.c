/***************************************************************************//**
 * @file
 * @brief This file implements the debug commands for RAILtest applications.
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

#include <stdio.h>
#include <string.h>

#if !defined(__ICCARM__)
// IAR doesn't have strings.h and puts those declarations in string.h
#include <strings.h>
#endif

#include "response_print.h"
#include "rail_types.h"

#include "rail.h"
#include "app_common.h"
#include "app_trx.h"
#include "hal_common.h"

#include "em_cmu.h"
#include "em_gpio.h"
#include "rail_features.h"

uint32_t rxOverflowDelay = 10 * 1000000; // 10 seconds

/*
 * setFrequency
 *
 * Allows the user to set an arbitrary frequency if
 * the frequency override debug mode is enabled.
 */
void setFrequency(sl_cli_command_arg_t *args)
{
  uint32_t newFrequency = sl_cli_get_argument_uint32(args, 0);

  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }

  if ((RAIL_GetDebugMode(railHandle) & RAIL_DEBUG_MODE_FREQ_OVERRIDE) == RAIL_DEBUG_MODE_FREQ_OVERRIDE) {
    if (!RAIL_OverrideDebugFrequency(railHandle, newFrequency)) {
      responsePrint(sl_cli_get_command_string(args, 0), "NewFrequency:%u", newFrequency);
    } else {
      // This won't take effect until we parse divider ranges.
      responsePrintError(sl_cli_get_command_string(args, 0), 0x14, "%u Hz is out of range and cannot be "
                                                                   "set as the frequency", newFrequency);
    }
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x13, "Not currently in frequency override debug mode.");
  }
}

char * lookupDebugModeString(uint32_t debugMode)
{
  if (debugMode == 0) {
    return "Disabled";
  }
  if (debugMode & RAIL_DEBUG_MODE_FREQ_OVERRIDE) {
    return "Frequency Override";
  }

  return "NULL";
}
/*
 * setDebugMode()
 *
 * Configure RAIL into a debug mode.
 *
 * Current debug modes:
 * RAIL_DEBUG_MODE_FREQ_OVERRIDE - Disable RAIL's channel scheme and
 * uses a specific frequency defined by the user.
 */
void setDebugMode(sl_cli_command_arg_t *args)
{
  uint32_t debugMode;

  debugMode = sl_cli_get_argument_uint32(args, 0);
  if (!RAIL_SetDebugMode(railHandle, debugMode)) {
    responsePrint(sl_cli_get_command_string(args, 0), "DebugMode:%s", lookupDebugModeString(debugMode));
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x15, "%d is an invalid debug mode!", debugMode);
  }
}

void getMemWord(sl_cli_command_arg_t *args)
{
  uint32_t *address = (uint32_t*)sl_cli_get_argument_uint32(args, 0);
  uint32_t count = 1;
  uint32_t value;

  // If there was a length given then read it out
  if (sl_cli_get_argument_count(args) >= 2) {
    count = sl_cli_get_argument_uint32(args, 1);
  }

  // Check for alignment
  if (((uint32_t)address % 4) != 0) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Address 0x%.8X is not 32bit aligned", address);
    return;
  }

  responsePrintHeader(sl_cli_get_command_string(args, 0), "address:0x%.8x,value:0x%.8x");
  for (uint32_t i = 0; i < count; i++) {
    value = address[i];
    responsePrintMulti("address:0x%.8x,value:0x%.8x", address + i, value);
  }
}

void setMemWord(sl_cli_command_arg_t *args)
{
  uint32_t *address = (uint32_t*)sl_cli_get_argument_uint32(args, 0);
  int count = 0;
  char lengthStr[12];

  // Check for alignment
  if (((uint32_t)address % 4) != 0) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Address 0x%.8X is not 32bit aligned", address);
    return;
  }

  // Write each word given sequentially
  for (int i = 2; i < (sl_cli_get_argument_count(args) + 1); i++) {
    address[count] = sl_cli_get_argument_uint32(args, i - 1);
    count++;
  }

  // Make getMemWord print out everything we just wrote to
  sprintf(lengthStr, "%d", count);
  args->argc = 3;
  args->argv[2] = lengthStr;
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  getMemWord(args);
}

void setTxUnderflow(sl_cli_command_arg_t *args)
{
  bool enable = !!sl_cli_get_argument_uint8(args, 0);
  enableAppMode(TX_UNDERFLOW, enable, sl_cli_get_command_string(args, 0));
}

void setRxOverflow(sl_cli_command_arg_t *args)
{
  bool enable = !!sl_cli_get_argument_uint8(args, 0);
  if (sl_cli_get_argument_count(args) >= 2) {
    rxOverflowDelay = sl_cli_get_argument_uint32(args, 1);
  } else {
    // 10 seconds should be enough to trigger an overflow
    rxOverflowDelay = 10 * 1000000;
  }
  enableAppMode(RX_OVERFLOW, enable, sl_cli_get_command_string(args, 0));
}

void setCalibrations(sl_cli_command_arg_t *args)
{
  bool enable = !!sl_cli_get_argument_uint8(args, 0);
  skipCalibrations = !enable;
  responsePrint(sl_cli_get_command_string(args, 0), "Calibrations:%s", enable ? "Enabled" : "Disabled");
}

void txCancel(sl_cli_command_arg_t *args)
{
  int32_t delay = sl_cli_get_argument_uint32(args, 0);
  txCancelDelay = delay;
  txCancelMode = RAIL_STOP_MODES_NONE; // Default to using RAIL_Idle()
  if (sl_cli_get_argument_count(args) >= 2) {
    txCancelMode = sl_cli_get_argument_uint8(args, 1);
  }

  enableAppMode(TX_CANCEL, delay >= 0, sl_cli_get_command_string(args, 0)); // Pends transmit to cancel
}

void startThermistorMeasurement(sl_cli_command_arg_t *args)
{
#if RAIL_FEAT_EXTERNAL_THERMISTOR
  RAIL_Status_t status = RAIL_StartThermistorMeasurement(railHandle);
  if (status == RAIL_STATUS_NO_ERROR) {
    responsePrint(sl_cli_get_command_string(args, 0), "Thermistor measurement:Started.");
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Ongoing thermistor measurement or unconfigured modem.");
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif
  return;
}

void getThermistorImpedance(sl_cli_command_arg_t *args)
{
#if RAIL_FEAT_EXTERNAL_THERMISTOR
  RAIL_Status_t status;
  uint32_t thermistorResistance;
  status = RAIL_GetThermistorImpedance(railHandle, &thermistorResistance);
  if (status == RAIL_STATUS_NO_ERROR) {
    responsePrint(sl_cli_get_command_string(args, 0), "Thermistor Measurement: %u Ohms", thermistorResistance);
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Thermistor mesurement not done yet.");
  }
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0xFF, "Feature not supported in this target.");
#endif
  return;
}

void getLogLevels(sl_cli_command_arg_t *args)
{
  responsePrint(sl_cli_get_command_string(args, 0), "Peripherals:%s,Notifications:%s",
                ((logLevel & PERIPHERAL_ENABLE) ? "Enabled" : "Disabled"),
                ((logLevel & ASYNC_RESPONSE) ? "Enabled" : "Disabled"));
}

void setPeripheralEnable(sl_cli_command_arg_t *args)
{
  bool enable = !!sl_cli_get_argument_uint8(args, 0);
  logLevel = enable ? (logLevel | PERIPHERAL_ENABLE)
             : (logLevel & ~(PERIPHERAL_ENABLE));
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  getLogLevels(args);

  // Actually turn on/off the peripherals as requested to save power
  if (enable) {
    PeripheralEnable();
  } else {
    PeripheralDisable();
  }
}

void setNotifications(sl_cli_command_arg_t *args)
{
  bool enable = !!sl_cli_get_argument_uint8(args, 0);
  logLevel = enable ? (logLevel | ASYNC_RESPONSE)
             : (logLevel & ~(ASYNC_RESPONSE));
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  getLogLevels(args);
}

void resetChip(sl_cli_command_arg_t *args)
{
  (void)args;
  // Wait for any serial traffic to be done before resetting so we don't
  // output garbage characters
  serialWaitForTxIdle();

  // Use the NVIC to reset the chip
  NVIC_SystemReset();
}

void printDataRates(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  responsePrint(sl_cli_get_command_string(args, 0),
                "Symbolrate:%d,Bitrate:%d",
                RAIL_GetSymbolRate(railHandle),
                RAIL_GetBitRate(railHandle));
}

#define MAX_RANDOM_BYTES (128)
static char randomPrintBuffer[MAX_RANDOM_BYTES];
static uint8_t randomDataBuffer[MAX_RANDOM_BYTES];

void getRandom(sl_cli_command_arg_t *args)
{
  uint16_t length = sl_cli_get_argument_uint16(args, 0);
  bool hidden = false;
  uint16_t result, offset = 0;
  int i;

  // Read out the hidden option if specified
  if (sl_cli_get_argument_count(args) >= 2) {
    hidden = !!sl_cli_get_argument_uint8(args, 1);
  }

  if (length > MAX_RANDOM_BYTES) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x10,
                       "Cannot collect more than %d random bytes.",
                       MAX_RANDOM_BYTES);
    return;
  }

  // Collect the random data
  result = RAIL_GetRadioEntropy(railHandle, randomDataBuffer, length);
  if (result != length) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x11, "Error collecting random data.");
  }

  if (!hidden) {
    for (i = 0; i < length; i++) {
      int n = snprintf(randomPrintBuffer + offset,
                       sizeof(randomPrintBuffer) - offset,
                       "%.2x",
                       randomDataBuffer[i]);
      if (n < 0) {
        responsePrintError(sl_cli_get_command_string(args, 0), 0x12, "Error printing random data.");
        return;
      }
      offset += n;

      // Make sure we don't try to print too much data
      if (offset >= sizeof(randomPrintBuffer)) {
        break;
      }
    }

    responsePrint(sl_cli_get_command_string(args, 0), "Length:%u,Data:%s", result, randomPrintBuffer);
  } else {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "Length:%u,DataPtr:0x%.8x",
                  result,
                  &randomDataBuffer);
  }
}

static void printDebugSignalHelp(char *cmdName,
                                 const debugPin_t *pins,
                                 const debugSignal_t *signals,
                                 uint32_t numPins,
                                 uint32_t numSignals)
{
  uint32_t i;

  RAILTEST_PRINTF("%s [pin] [signal] [options]\n", cmdName);
  RAILTEST_PRINTF("Pins: ");
  for (i = 0; i < numPins; i++) {
    if (i != 0) {
      RAILTEST_PRINTF(", ");
    }
    RAILTEST_PRINTF("%s", pins[i].name);
  }

  // Print information about the supported debug signals
  RAILTEST_PRINTF("\nSignals: \n");
  RAILTEST_PRINTF("  OFF\n  CUSTOM_PRS <source> <signal>\n  CUSTOM_LIB <event>\n");
  for (i = 0; i < numSignals; i++) {
    RAILTEST_PRINTF("  %s\n", signals[i].name);
  }
}

void setDebugSignal(sl_cli_command_arg_t *args)
{
  const debugPin_t *pin = NULL, *pinList;
  const debugSignal_t *signal = NULL, *signalList;
  debugSignal_t customSignal;
  uint32_t i;
  uint32_t numSignals, numPins;
  bool disablePin = false;

  // Get the debug signals and pins for this chip
  signalList = halGetDebugSignals(&numSignals);
  pinList = halGetDebugPins(&numPins);

  // Provide information about the pins and signals supported by this chip if
  // the help command is given. @TODO: It would be nice if this ignored the next
  // parameter somehow...
  if ((sl_cli_get_argument_count(args) < 1) || (strcasecmp(sl_cli_get_argument_string(args, 0), "help") == 0)) {
    printDebugSignalHelp(sl_cli_get_command_string(args, 0), pinList, signalList, numPins, numSignals);
    return;
  }

  // Make sure the pin they're trying to use is valid for this chip
  for (i = 0; i < numPins; i++) {
    if (strcasecmp(pinList[i].name, sl_cli_get_argument_string(args, 0)) == 0) {
      pin = &pinList[i];
    }
  }
  if (pin == NULL) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x50, "%s is not a valid pin name", sl_cli_get_argument_string(args, 0));
    return;
  }

  // Make sure the signal they're trying to use is valid for this chip
  if (strcasecmp("CUSTOM_LIB", sl_cli_get_argument_string(args, 1)) == 0) {
    // Make sure the correct parameters were given for this command
    if (!(sl_cli_get_argument_count(args) == 3)) {
      responsePrintError(sl_cli_get_command_string(args, 0),
                         0x51,
                         "Invalid parameters passed to CUSTOM_LIB");
      return;
    }
    customSignal.name = "CUSTOM_LIB";
    customSignal.isPrs = false;
    customSignal.loc.debugEventNum = sl_cli_get_argument_uint16(args, 2);
    signal = &customSignal;
  } else if (strcasecmp("CUSTOM_PRS", sl_cli_get_argument_string(args, 1)) == 0) {
    // Make sure that the right arguments were given for this command
    if (!(sl_cli_get_argument_count(args) == 4)) {
      responsePrintError(sl_cli_get_command_string(args, 0),
                         0x53,
                         "Invalid parameters passed to CUSTOM_PRS");
      return;
    }
    customSignal.name = "CUSTOM_PRS";
    customSignal.isPrs = true;
    customSignal.loc.prs.source = sl_cli_get_argument_uint8(args, 2);
    customSignal.loc.prs.signal = sl_cli_get_argument_uint8(args, 3);
    signal = &customSignal;
  } else if (strcasecmp("OFF", sl_cli_get_argument_string(args, 1)) == 0) {
    disablePin = true;
  } else {
    // Search through the list of known signals for the requested one
    for (i = 0; i < numSignals; i++) {
      if (strcasecmp(signalList[i].name, sl_cli_get_argument_string(args, 1)) == 0) {
        signal = &signalList[i];
      }
    }
    if (signal == NULL) {
      responsePrintError(sl_cli_get_command_string(args, 0), 0x54, "%s is not a valid signal name", sl_cli_get_argument_string(args, 1));
      return;
    }
  }

  // Turn on the GPIO clock since we're going to need that
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Disable the GPIO while configuring it
  GPIO_PinModeSet(pin->gpioPort, pin->gpioPin, gpioModeDisabled, 0);

  // If this is a disable command then just turn everything off for this debug
  // pin to stop it from outputting.
  if (disablePin) {
    // Turn off the PRS output on this pin's channel
    halDisablePrs(pin->prsChannel);
    // @TODO: Turn off the RAIL debug event for this pin
    responsePrint(sl_cli_get_command_string(args, 0), "Pin:%s,Signal:OFF", pin->name);
    return;
  }

  if (signal == NULL) {
    return;
  }
  // Configure the PRS or library signal as needed
  if (signal->isPrs) {
    // Enable this PRS signal
    halEnablePrs(pin->prsChannel,
                 pin->prsLocation,
                 pin->gpioPort,
                 pin->gpioPin,
                 signal->loc.prs.source,
                 signal->loc.prs.signal);
  } else {
    // Turn on the RAIL debug event for this signal
  }

  responsePrint(sl_cli_get_command_string(args, 0), "Pin:%s,Signal:%s", pin->name, signal->name);
}

void forceAssert(sl_cli_command_arg_t *args)
{
  uint32_t errorCode = sl_cli_get_argument_uint32(args, 0);

  responsePrint(sl_cli_get_command_string(args, 0), "code:%d", errorCode);
  RAILCb_AssertFailed(railHandle, errorCode);
}

void configPrintEvents(sl_cli_command_arg_t *args)
{
  if (sl_cli_get_argument_count(args) == 0) {
    responsePrintHeader(sl_cli_get_command_string(args, 0), "name:%s,shift:%u,mask:%x%08x,printEnabled:%s");
    for (uint32_t i = 0; i < numRailEvents; i++) {
      uint64_t mask = 1ULL << i;
      responsePrintMulti("name:RAIL_EVENT_%s,shift:%u,mask:0x%x%08x,printEnabled:%s",
                         eventNames[i],
                         i,
                         (uint32_t)(mask >> 32),
                         (uint32_t)(mask),
                         (mask & enablePrintEvents) ? "True" : "False");
    }
    return;
  }

  RAIL_Events_t printEvents = sl_cli_get_argument_uint32(args, 0);
  RAIL_Events_t printEventsMask = RAIL_EVENTS_ALL;

  if (sl_cli_get_argument_count(args) >= 2) {
    printEvents |= (((RAIL_Events_t)sl_cli_get_argument_uint32(args, 1)) << 32);
  }
  // Read out the optional mask bits
  if (sl_cli_get_argument_count(args) == 3) {
    printEventsMask = 0xFFFFFFFF00000000ULL | sl_cli_get_argument_uint32(args, 2);
  } else if (sl_cli_get_argument_count(args) >= 4) {
    printEventsMask = ((RAIL_Events_t)sl_cli_get_argument_uint32(args, 3)) << 32;
    printEventsMask |= (RAIL_Events_t)sl_cli_get_argument_uint32(args, 2);
  }
  // Modify only the requested events
  enablePrintEvents = (enablePrintEvents & ~printEventsMask)
                      | (printEvents & printEventsMask);

  responsePrint(sl_cli_get_command_string(args, 0), "enablePrintEvents:0x%x%08x",
                (uint32_t)(enablePrintEvents >> 32),
                (uint32_t)(enablePrintEvents));
}

void printTxAcks(sl_cli_command_arg_t *args)
{
  printTxAck = !!sl_cli_get_argument_uint8(args, 0);

  responsePrint(sl_cli_get_command_string(args, 0), "printTxAcks:%s",
                printTxAck ? "True" : "False");
}

void printRxErrors(sl_cli_command_arg_t *args)
{
  printRxErrorPackets = !!sl_cli_get_argument_uint8(args, 0);

  responsePrint(sl_cli_get_command_string(args, 0), "printRxErrors:%s",
                printRxErrorPackets ? "True" : "False");
}

void printRxFreqOffsets(sl_cli_command_arg_t *args)
{
  printRxFreqOffsetData = !!sl_cli_get_argument_uint8(args, 0);

  if (printRxFreqOffsetData) {
    // Enable the first sync word and second sync word events
    RAIL_ConfigEvents(railHandle,
                      (RAIL_EVENT_RX_SYNC1_DETECT | RAIL_EVENT_RX_SYNC2_DETECT),
                      (RAIL_EVENT_RX_SYNC1_DETECT | RAIL_EVENT_RX_SYNC2_DETECT));
  } else {
    rxFreqOffset = RAIL_FREQUENCY_OFFSET_INVALID;
  }

  responsePrint(sl_cli_get_command_string(args, 0), "printRxFreqOffsets:%s",
                printRxFreqOffsetData ? "True" : "False");
}

void setPrintingEnable(sl_cli_command_arg_t *args)
{
  printingEnabled = !!sl_cli_get_argument_uint8(args, 0);
  responsePrintEnable(printingEnabled);
  responsePrint(sl_cli_get_command_string(args, 0), "printingEnabled:%s",
                printingEnabled ? "True" : "False");
}

void getAppMode(sl_cli_command_arg_t *args)
{
  responsePrint(sl_cli_get_command_string(args, 0), "appMode:%s", appModeNames(currentAppMode()));
}

void getRadioState(sl_cli_command_arg_t *args)
{
  responsePrint(sl_cli_get_command_string(args, 0), "radioState:%s",
                getRfStateName(RAIL_GetRadioState(railHandle)));
}

static bool verifyFirstTime = true;
static bool verifyUseOverride = false;
static bool verifyUseCallback = false;
static uint32_t verifyCbCounter = 0; // Number of times the callback is called.

static bool RAILCb_VerificationApproval(uint32_t address,
                                        uint32_t expectedValue,
                                        uint32_t actualValue)
{
  // true = change approved (Do this to see a list of all registers that are
  //   different for the current radio state.)
  // false = change unapproved (This is the default behavior when no approval
  //   callback is provided.)
  bool approveDifference = true;
  // Print out all addresses that contain differences.
  responsePrint("verifyRadioCb",
                "encodedAddress:0x%08x,"
                "expectedValue:0x%08x,"
                "actualValue:0x%08x,"
                "differenceApproved:%s",
                address,
                expectedValue,
                actualValue,
                (approveDifference ? "true" : "false"));
  verifyCbCounter++;
  return approveDifference;
}

void verifyRadio(sl_cli_command_arg_t *args)
{
  char *answer;
  uint32_t durationUs = sl_cli_get_argument_uint32(args, 0);
  bool restart = !!sl_cli_get_argument_uint8(args, 1);
  bool useOverride = !!sl_cli_get_argument_uint8(args, 2);
  bool useCallback = !!sl_cli_get_argument_uint8(args, 3);
  uint32_t *radioConfig;
  RAIL_VerifyCallbackPtr_t cb;
  uint32_t timeBefore;
  uint32_t timeAfter;
  RAIL_Status_t retVal;

  // Only run RAIL_ConfigVerification when we have to.
  if (verifyFirstTime
      || (useOverride != verifyUseOverride)
      || (useCallback != verifyUseCallback)) {
    verifyFirstTime = false;
    verifyUseOverride = useOverride;
    verifyUseCallback = useCallback;

    if (useOverride) {
#if SL_RAIL_TEST_RADIO_CONFIG_EXTERNAL_SUPPORT_ENABLED
      // Provide a custom radio config.
      radioConfig = (uint32_t *)(channelConfigs[configIndex]->phyConfigBase);
#else // !SL_RAIL_TEST_RADIO_CONFIG_EXTERNAL_SUPPORT_ENABLED
      // Restore variable to default value so this error always occurs.
      verifyFirstTime = true;
      responsePrintError(sl_cli_get_command_string(args, 0), 0x22, "External radio config support not enabled");
      return;
#endif // SL_RAIL_TEST_RADIO_CONFIG_EXTERNAL_SUPPORT_ENABLED
    } else {
      radioConfig = NULL;
    }
    if (useCallback) {
      // Provide an approval callback to the application.
      cb = RAILCb_VerificationApproval;
    } else {
      cb = NULL;
    }

    RAIL_ConfigVerification(railHandle, &configVerify, radioConfig, cb);
  }

  // Clear the callback counter when restarting verification.
  if (restart) {
    verifyCbCounter = 0;
  }

  timeBefore = RAIL_GetTime();
  retVal = RAIL_Verify(&configVerify, durationUs, restart);
  timeAfter = RAIL_GetTime();
  switch (retVal) {
    case RAIL_STATUS_NO_ERROR:
    {
      answer = "success, done";
      break;
    }
    case RAIL_STATUS_SUSPENDED:
    {
      answer = "success, operation suspended";
      break;
    }
    case RAIL_STATUS_INVALID_PARAMETER:
    {
      answer = "invalid input parameter";
      break;
    }
    case RAIL_STATUS_INVALID_STATE:
    default:
    {
      answer = "data corruption";
    }
  }
  if (useCallback) {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "verification:%s,testDurationUs:%d,callbackCounter:%d",
                  answer,
                  timeAfter - timeBefore,
                  verifyCbCounter);
  } else {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "verification:%s,testDurationUs:%d",
                  answer,
                  timeAfter - timeBefore);
  }
  // Clear the callback counter on success after we output the value.
  if (RAIL_STATUS_NO_ERROR == retVal) {
    verifyCbCounter = 0;
  }
}

void setVerifyConfig(sl_cli_command_arg_t *args)
{
  verifyConfigEnabled = !!sl_cli_get_argument_uint8(args, 0);
  responsePrint(sl_cli_get_command_string(args, 0),
                "verify config enabled:%d,"
                "Status:Success",
                verifyConfigEnabled);
}

void getVerifyConfig(sl_cli_command_arg_t *args)
{
  responsePrint(sl_cli_get_command_string(args, 0),
                "verify config enabled:%d",
                verifyConfigEnabled);
}
