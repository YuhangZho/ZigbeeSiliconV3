/***************************************************************************//**
 * @file
 * @brief This file implements the parameter commands for RAILtest applications.
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
#include <stdio.h>

#include "response_print.h"

#include "rail.h"
#include "rail_types.h"
#include "app_common.h"
#include "em_core.h"
#include "pa_conversions_efr32.h"

static const char *powerModes[] = RAIL_TX_POWER_MODE_NAMES;

void getChannel(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (RAIL_GetDebugMode(railHandle) & RAIL_DEBUG_MODE_FREQ_OVERRIDE) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12, "Channels are not valid in Debug Mode");
  } else {
    responsePrint(sl_cli_get_command_string(args, 0), "channel:%d", channel);
  }
}

void setChannel(sl_cli_command_arg_t *args)
{
  uint16_t proposedChannel = sl_cli_get_argument_uint16(args, 0);
  bool success = false;

  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  // Make sure this is a valid channel
  if (RAIL_IsValidChannel(railHandle, proposedChannel)
      == RAIL_STATUS_NO_ERROR) {
    changeChannel(proposedChannel);
    success = true;
  }

  if (!success) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x11, "Invalid channel '%d'", proposedChannel);
    return;
  }

  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  getChannel(args);
}

void setFreqOffset(sl_cli_command_arg_t *args)
{
  static RAIL_FrequencyOffset_t currentFreqOffset = 0;
  if (sl_cli_get_argument_count(args) >= 1) {
    RAIL_FrequencyOffset_t freqOffset = sl_cli_get_argument_int32(args, 0);

    if ((freqOffset < RAIL_FREQUENCY_OFFSET_MIN)
        || (freqOffset > RAIL_FREQUENCY_OFFSET_MAX)) {
      responsePrintError(sl_cli_get_command_string(args, 0), RAIL_STATUS_INVALID_PARAMETER,
                         "Offset %d outside range of [%d, %d]",
                         freqOffset,
                         RAIL_FREQUENCY_OFFSET_MIN,
                         RAIL_FREQUENCY_OFFSET_MAX);
      return;
    }
    RAIL_Status_t status = RAIL_SetFreqOffset(railHandle, freqOffset);
    if (status != RAIL_STATUS_NO_ERROR) {
      responsePrintError(sl_cli_get_command_string(args, 0), status, "Could not set frequency offset");
      return;
    }
    currentFreqOffset = freqOffset;
  }
  responsePrint(sl_cli_get_command_string(args, 0), "freqOffset:%d", currentFreqOffset);
}

void getPowerConfig(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_TxPowerConfig_t config;
  RAIL_Status_t status = RAIL_GetTxPowerConfig(railHandle, &config);

  responsePrint(sl_cli_get_command_string(args, 0), "success:%s,mode:%s,voltage:%d,rampTime:%d",
                status == RAIL_STATUS_NO_ERROR ? "true" : "false",
                powerModes[config.mode], config.voltage, config.rampTime);
}

void setPowerConfig(sl_cli_command_arg_t *args)
{
  RAIL_TxPowerConfig_t *txPowerConfigPtr = sl_rail_util_pa_get_tx_power_config_2p4ghz();
  // Make a backup of the TX Power Config before it's changed.
  RAIL_TxPowerConfig_t txPowerConfigBackup = {
    .mode = txPowerConfigPtr->mode,
    .voltage = txPowerConfigPtr->voltage,
    .rampTime = txPowerConfigPtr->rampTime
  };
  RAIL_TxPowerMode_t mode = sl_cli_get_argument_uint8(args, 0);
  if (mode >= RAIL_TX_POWER_MODE_NONE) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x13, "Invalid PA enum value selected: %d", mode);
    return;
  }

  uint16_t voltage = sl_cli_get_argument_uint16(args, 1);
  uint16_t rampTime = sl_cli_get_argument_uint16(args, 2);

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  txPowerConfigPtr->mode = mode;
  txPowerConfigPtr->voltage = voltage;
  txPowerConfigPtr->rampTime = rampTime;
  CORE_EXIT_CRITICAL();

  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_Status_t status = RAIL_ConfigTxPower(railHandle, txPowerConfigPtr);
  // Restore the backup TX Power Config on error.
  if (status != RAIL_STATUS_NO_ERROR) {
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_CRITICAL();
    txPowerConfigPtr->mode = txPowerConfigBackup.mode;
    txPowerConfigPtr->voltage = txPowerConfigBackup.voltage;
    txPowerConfigPtr->rampTime = txPowerConfigBackup.rampTime;
    CORE_EXIT_CRITICAL();
  }

  if (status == RAIL_STATUS_NO_ERROR) {
    args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
    getPowerConfig(args);
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x26, "Could not set power config");
  }
}

void getPower(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  responsePrint(sl_cli_get_command_string(args, 0),
                "powerLevel:%d,power:%d",
                RAIL_GetTxPower(railHandle),
                RAIL_GetTxPowerDbm(railHandle));
}

void setPower(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }
  RAIL_TxPowerLevel_t powerLevel;
  RAIL_TxPower_t power;
  bool setPowerError = false;

  if (sl_cli_get_argument_count(args) >= 2 && strcmp(sl_cli_get_argument_string(args, 1), "raw") == 0) {
    RAIL_TxPowerLevel_t rawLevel = sl_cli_get_argument_uint8(args, 0);

    // Set the power and update the RAW level global
    if (RAIL_SetTxPower(railHandle, rawLevel) != RAIL_STATUS_NO_ERROR) {
      setPowerError = true;
    }
  } else {
    RAIL_TxPowerConfig_t tempCfg;
    RAIL_TxPower_t powerDbm = sl_cli_get_argument_int16(args, 0);

    // Set the power in dBm and figure out what RAW level to store based on what
    // was requested NOT what is actually applied to the hardware after limits.
    if ((RAIL_SetTxPowerDbm(railHandle, powerDbm)
         != RAIL_STATUS_NO_ERROR)
        || (RAIL_GetTxPowerConfig(railHandle, &tempCfg)
            != RAIL_STATUS_NO_ERROR)) {
      setPowerError = true;
    }
  }

  if (setPowerError) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x23, "Could not set power.");
  } else {
    // Get and print out the actual applied power and power level
    powerLevel = RAIL_GetTxPower(railHandle);
    power = RAIL_GetTxPowerDbm(railHandle);
    responsePrint(sl_cli_get_command_string(args, 0), "powerLevel:%d,power:%d", powerLevel, power);
  }
}

void sweepTxPower(sl_cli_command_arg_t *args)
{
  responsePrint(sl_cli_get_command_string(args, 0), "Sweeping:Started,Instructions:'q' to quit or 'enter' to continue.");
  RAIL_TxPowerConfig_t txPowerConfig;

  RAIL_GetTxPowerConfig(railHandle, &txPowerConfig);

  RAIL_TxPowerLevel_t start = 1;
  RAIL_TxPowerLevel_t end = 255;

  switch (txPowerConfig.mode) {
    case RAIL_TX_POWER_MODE_2P4_HP:
      end = RAIL_TX_POWER_LEVEL_HP_MAX;
      break;
    #if _SILICON_LABS_32B_SERIES_2_CONFIG == 1
    case RAIL_TX_POWER_MODE_2P4_MP:
      end = RAIL_TX_POWER_LEVEL_MP_MAX;
      break;
    #endif
    case RAIL_TX_POWER_MODE_2P4_LP:
      end = RAIL_TX_POWER_LEVEL_LP_MAX;
      break;
    #ifdef RAIL_TX_POWER_MODE_SUBGIG
    case RAIL_TX_POWER_MODE_SUBGIG:
      end = RAIL_TX_POWER_LEVEL_SUBGIG_MAX;
      break;
    #endif
    default:
      responsePrintError(sl_cli_get_command_string(args, 0), 0x21, "PA not configured.");
      return;
  }

  char input;
  RAIL_TxPowerLevel_t i;

  for (i = start; i <= end; i++) {
    responsePrint(sl_cli_get_command_string(args, 0), "PowerLevel:%u", i);
    RAIL_Idle(railHandle, RAIL_IDLE_FORCE_SHUTDOWN_CLEAR_FLAGS, true);
    RAIL_SetTxPower(railHandle, i);
    RAIL_StartTxStream(railHandle, channel, RAIL_STREAM_CARRIER_WAVE);

    input = getchar();

    while (1) {
      if (input == '\n' || input == '\r') {
        break;
      }
      if (input == 'q') {
        responsePrintError(sl_cli_get_command_string(args, 0), 0x20, "Sweep Aborted.");
        return;
      }
      input = getchar();
    }

    RAIL_Idle(railHandle, RAIL_IDLE_FORCE_SHUTDOWN_CLEAR_FLAGS, true);
  }

  responsePrint(sl_cli_get_command_string(args, 0), "Sweeping:Complete");
}

void getCtune(sl_cli_command_arg_t *args)
{
  uint32_t ctune = RAIL_GetTune(railHandle);

  responsePrint(sl_cli_get_command_string(args, 0), "CTUNE:0x%.3x", ctune);
}

void setCtune(sl_cli_command_arg_t *args)
{
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }

  RAIL_SetTune(railHandle, sl_cli_get_argument_uint32(args, 0));

  // Read out and print the current CTUNE value
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  getCtune(args);
}

void setPaCtune(sl_cli_command_arg_t *args)
{
  RAIL_Status_t status;
  uint8_t txVal = sl_cli_get_argument_uint8(args, 0);
  uint8_t rxVal = sl_cli_get_argument_uint8(args, 1);

  status = RAIL_SetPaCTune(railHandle, txVal, rxVal);

  if (status == RAIL_STATUS_NO_ERROR) {
    responsePrint(sl_cli_get_command_string(args, 0), "PACTUNETX:%d,PACTUNERX:%d", txVal, rxVal);
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), status, "Error");
  }
}

void enablePaCal(sl_cli_command_arg_t *args)
{
  uint8_t enable = sl_cli_get_argument_uint8(args, 0);
  RAIL_EnablePaCal(enable);
  responsePrint(sl_cli_get_command_string(args, 0), "paCal:%s", (enable ? "Enabled" : "Disabled"));
}

// Helper to convert a string to a RAIL RadioState
static int8_t stringToState(char *string, RAIL_RadioState_t *state)
{
  switch (string[0]) {
    case 'i': case 'I':
      *state =  RAIL_RF_STATE_IDLE;
      break;
    case 'r': case 'R':
      *state =  RAIL_RF_STATE_RX;
      break;
    case 't': case 'T':
      *state =  RAIL_RF_STATE_TX;
      break;
    default:
      return 1;
  }
  return 0;
}

void setTxTransitions(sl_cli_command_arg_t *args)
{
  RAIL_RadioState_t states[2];
  if (stringToState(sl_cli_get_argument_string(args, 0), &states[0])
      || stringToState(sl_cli_get_argument_string(args, 1), &states[1])) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x16, "Invalid states");
    return;
  }

  RAIL_StateTransitions_t transitions = {
    .success = states[0],
    .error = states[1]
  };

  RAIL_Status_t ret = RAIL_SetTxTransitions(railHandle, &transitions);
  responsePrint(sl_cli_get_command_string(args, 0), "TxTransitions:%s", (ret ? "Error" : "Set"));
}

void setRxTransitions(sl_cli_command_arg_t *args)
{
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }
  RAIL_RadioState_t states[2];
  if (stringToState(sl_cli_get_argument_string(args, 0), &states[0])
      || stringToState(sl_cli_get_argument_string(args, 1), &states[1])) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x16, "Invalid states");
    return;
  }
  RAIL_StateTransitions_t transitions = {
    .success = states[0],
    .error = states[1]
  };
  RAIL_Status_t ret = RAIL_SetRxTransitions(railHandle, &transitions);
  if (ret == RAIL_STATUS_NO_ERROR) {
    rxSuccessTransition = states[0];
  }
  responsePrint(sl_cli_get_command_string(args, 0), "RxTransitions:%s", (ret ? "Error" : "Set"));
}

void getRxTransitions(sl_cli_command_arg_t *args)
{
  RAIL_StateTransitions_t transitions;
  RAIL_Status_t ret = RAIL_GetRxTransitions(railHandle, &transitions);

  if (ret == RAIL_STATUS_NO_ERROR) {
    const char *success = getRfStateName(transitions.success);
    const char *error = getRfStateName(transitions.error);
    responsePrint(sl_cli_get_command_string(args, 0), "Success:%s,Error:%s", success, error);
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x28, "Get rx transitions failed");
  }
}

void getTxTransitions(sl_cli_command_arg_t *args)
{
  RAIL_StateTransitions_t transitions;
  RAIL_Status_t ret = RAIL_GetTxTransitions(railHandle, &transitions);

  if (ret == RAIL_STATUS_NO_ERROR) {
    const char *success = getRfStateName(transitions.success);
    const char *error = getRfStateName(transitions.error);
    responsePrint(sl_cli_get_command_string(args, 0), "Success:%s,Error:%s", success, error);
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x27, "Get tx transitions failed");
  }
}

void setTimings(sl_cli_command_arg_t *args)
{
  uint16_t timing[6] = { 0 };
  for (int i = 0; i < sl_cli_get_argument_count(args); i++) {
    timing[i] = sl_cli_get_argument_uint16(args, i);
  }
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_StateTiming_t timings =
  { timing[0], timing[1], timing[2], timing[3], timing[4], timing[5] };
  if (!RAIL_SetStateTiming(railHandle, &timings)) {
    responsePrint(sl_cli_get_command_string(args, 0), "IdleToRx:%u,RxToTx:%u,IdleToTx:%u,TxToRx:%u,"
                                                      "RxSearch:%u,Tx2RxSearch:%u",
                  timings.idleToRx, timings.rxToTx, timings.idleToTx,
                  timings.txToRx, timings.rxSearchTimeout, timings.txToRxSearchTimeout);
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x18, "Setting timings failed");
  }
}

void setTxFifoThreshold(sl_cli_command_arg_t *args)
{
  if (railDataConfig.txMethod != FIFO_MODE) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x19, "Tx is not in FIFO mode");
    return;
  }

  uint16_t txFifoThreshold = sl_cli_get_argument_uint16(args, 0);
  txFifoThreshold = RAIL_SetTxFifoThreshold(railHandle, txFifoThreshold);
  responsePrint(sl_cli_get_command_string(args, 0), "TxFifoThreshold:%d", txFifoThreshold);
}

void setRxFifoThreshold(sl_cli_command_arg_t *args)
{
  uint16_t rxFifoThreshold = sl_cli_get_argument_uint16(args, 0);
  rxFifoThreshold = RAIL_SetRxFifoThreshold(railHandle, rxFifoThreshold);
  responsePrint(sl_cli_get_command_string(args, 0), "RxFifoThreshold:%d", rxFifoThreshold);
}

void setEventConfig(sl_cli_command_arg_t *args)
{
  RAIL_Events_t eventMask = sl_cli_get_argument_uint32(args, 0);
  RAIL_Events_t eventConfig = sl_cli_get_argument_uint32(args, 1);

  if (sl_cli_get_argument_count(args) >= 4) {
    eventMask |= (((RAIL_Events_t)sl_cli_get_argument_uint32(args, 2)) << 32U);
    eventConfig |= (((RAIL_Events_t)sl_cli_get_argument_uint32(args, 3)) << 32U);
  }

  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_ConfigEvents(railHandle, eventMask, eventConfig);
  // Avoid use of %ll long-long formats due to iffy printf library support
  if (sl_cli_get_argument_count(args) >= 4) {
    responsePrint(sl_cli_get_command_string(args, 0), "Mask:0x%x%08x,Values:0x%x%08x",
                  (uint32_t)(eventMask >> 32),
                  (uint32_t)eventMask,
                  (uint32_t)(eventConfig >> 32),
                  (uint32_t)eventConfig);
  } else {
    responsePrint(sl_cli_get_command_string(args, 0), "Mask:0x%x,Values:0x%x",
                  (uint32_t)eventMask,
                  (uint32_t)eventConfig);
  }
}

void delayUs(sl_cli_command_arg_t *args)
{
  uint32_t delayUs = sl_cli_get_argument_uint32(args, 0);

  // Do not measure any interrupt processing overhead during the delay.
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  // Measure the actual delay vs expected.
  uint32_t startTime = RAIL_GetTime();
  RAIL_Status_t status = RAIL_DelayUs(delayUs);
  uint32_t actualDelay = RAIL_GetTime() - startTime;

  CORE_EXIT_CRITICAL();
  responsePrint(sl_cli_get_command_string(args, 0), "Success:%s,ActualDelay:%d",
                status == RAIL_STATUS_NO_ERROR ? "True" : "False",
                actualDelay);
}

#ifdef RAIL_PA_AUTO_MODE
#include "pa_auto_mode.h"
void configPaAutoMode(sl_cli_command_arg_t *args)
{
  uint8_t index = sl_cli_get_argument_uint8(args, 0);

  int16_t min = sl_cli_get_argument_int16(args, 1);
  int16_t max = sl_cli_get_argument_int16(args, 2);
  uint8_t mode = sl_cli_get_argument_uint8(args, 3);

  // Make sure the mode is valid
  if (mode > RAIL_TX_POWER_MODE_NONE) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x01, "Invalid mode (%d) specified", mode);
    return;
  }

  RAIL_PaAutoModeConfig[index].min = min;
  RAIL_PaAutoModeConfig[index].max = max;
  RAIL_PaAutoModeConfig[index].mode = mode;

  responsePrint(sl_cli_get_command_string(args, 0), "Min:%d,Max:%d,Mode:%s", min, max, powerModes[mode]);
}

void enablePaAutoMode(sl_cli_command_arg_t *args)
{
  bool enable = !!sl_cli_get_argument_uint8(args, 0);
  RAIL_EnablePaAutoMode(railHandle, enable);

  responsePrint(sl_cli_get_command_string(args, 0), "enable:%s", enable ? "True" : "False");
}
#else
void configPaAutoMode(sl_cli_command_arg_t *args)
{
  responsePrintError(sl_cli_get_command_string(args, 0), 0x01, "PA Auto Mode plugin must be enabled to use this feature!");
}

void enablePaAutoMode(sl_cli_command_arg_t *args)
{
  responsePrintError(sl_cli_get_command_string(args, 0), 0x01, "PA Auto Mode plugin must be enabled to use this feature!");
}
#endif
