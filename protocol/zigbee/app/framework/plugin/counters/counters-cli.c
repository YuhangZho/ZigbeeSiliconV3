/***************************************************************************//**
 * @file
 * @brief Used for testing the counters library via a command line interface.
 * For documentation on the counters library see counters.h.
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

#include "app/framework/include/af.h"
#include "app/util/serial/command-interpreter2.h"
#include "app/util/common/common.h"
#include "counters.h"
#include "counters-ota.h"
#include "counters-cli.h"

const char * titleStrings[] = {
  EMBER_COUNTER_STRINGS
};
const char * unknownCounter = "???";
static void emberAfPluginCountersPrint(void);

void emberAfPluginCountersPrintCommand(void)
{
#if defined(EZSP_HOST)
  ezspReadAndClearCounters(emberCounters);
#endif

  emberAfPluginCountersPrint();

#if !defined(EZSP_HOST)
  emberAfPluginCountersClear();
#endif
}

void emberAfPluginCounterPrintCounterTypeCommand(void)
{
#if !defined(EZSP_HOST)
  uint8_t counterType = (uint8_t)emberUnsignedCommandArgument(0);
  if (counterType < EMBER_COUNTER_TYPE_COUNT) {
    emberAfCorePrintln("%u) %p: %u",
                       counterType,
                       (titleStrings[counterType] == NULL
                        ? unknownCounter
                        : titleStrings[counterType]),
                       emberCounters[counterType]);
  }
#endif
}

void emberAfPluginCountersSimplePrintCommand(void)
{
#if defined(EZSP_HOST)
  ezspReadCounters(emberCounters);
#endif

  emberAfPluginCountersPrint();
}

void emberAfPluginCountersSendRequestCommand(void)
{
#if defined(EMBER_AF_PLUGIN_COUNTERS_OTA)
  emberAfPluginCountersSendRequest(emberUnsignedCommandArgument(0),
                                   emberUnsignedCommandArgument(1));
#endif
}

void emberAfPluginCountersSetThresholdCommand(void)
{
  EmberCounterType type = (uint8_t)emberUnsignedCommandArgument(0);
  uint16_t threshold = (uint16_t)emberUnsignedCommandArgument(1);
  emberAfCorePrintln("Setting Threshold command");
  emberAfPluginCountersSetThreshold(type, threshold);
}

void emberAfPluginCountersPrintThresholdsCommand(void)
{
  uint8_t i;
  for (i = 0; i < EMBER_COUNTER_TYPE_COUNT; i++) {
    emberAfCorePrintln("%u) %p: %u",
                       i,
                       (titleStrings[i] == NULL
                        ? unknownCounter
                        : titleStrings[i]),
                       emberCountersThresholds[i]);
  }
}

#if !defined(EZSP_HOST)

void emberAfPluginCounterPrintCountersResponse(EmberMessageBuffer message)
{
  uint8_t i;
  uint8_t length = emberMessageBufferLength(message);
  for (i = 0; i < length; i += 3) {
    emberAfCorePrintln("%d: %d",
                       emberGetLinkedBuffersByte(message, i),
                       emberGetLinkedBuffersLowHighInt16u(message, i + 1));
  }
}

#endif

static void emberAfPluginCountersPrint(void)
{
  uint8_t i;
  for (i = 0; i < EMBER_COUNTER_TYPE_COUNT; i++) {
    emberAfCorePrintln("%u) %p: %u",
                       i,
                       (titleStrings[i] == NULL
                        ? unknownCounter
                        : titleStrings[i]),
                       emberCounters[i]);
  }
}
