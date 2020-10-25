/***************************************************************************//**
 * @file
 * @brief This file implements the radio config commands for RAILtest
 *   applications.
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
#include "rail_features.h"

#include "rail_ble.h"
#include "rail_ieee802154.h"
#include "rail_zwave.h"

#include "app_common.h"
#include "sl_rail_util_init.h"
#include "sl_rail_util_pa_config.h"

bool verifyConfigEnabled = false;

// Channel Config Selection Variable
uint8_t configIndex = 0;

const char* paStrings[] = RAIL_TX_POWER_MODE_NAMES;

/******************************************************************************
 * Command Line Interface functions
 *****************************************************************************/
void getConfigIndex(sl_cli_command_arg_t *args)
{
#if SL_RAIL_TEST_RADIO_CONFIG_EXTERNAL_SUPPORT_ENABLED
  responsePrint(sl_cli_get_command_string(args, 0), "configIndex:%d", configIndex);
#else
  responsePrintError(sl_cli_get_command_string(args, 0), 0x22, "External radio config support not enabled");
#endif // SL_RAIL_TEST_RADIO_CONFIG_EXTERNAL_SUPPORT_ENABLED
}

void setConfigIndex(sl_cli_command_arg_t *args)
{
#if SL_RAIL_TEST_RADIO_CONFIG_EXTERNAL_SUPPORT_ENABLED
  uint8_t proposedIndex = sl_cli_get_argument_uint8(args, 0);

  // Be sure that the proposed index is valid. Scan through all possible
  // indexes and check for the last NULL parameter since you can't
  // use sizeof on an extern-ed array without an explicit index.
  for (uint8_t i = 0; i <= proposedIndex; i++) {
    if (channelConfigs[i] == NULL) {
      responsePrintError(sl_cli_get_command_string(args, 0), 0x11,
                         "Invalid radio config index '%d'",
                         proposedIndex);
      return;
    }
  }

  configIndex = proposedIndex;
  RAIL_Idle(railHandle, RAIL_IDLE_ABORT, true);
  // Load the channel configuration for the specified index.
  channel = RAIL_ConfigChannels(railHandle,
                                channelConfigs[configIndex],
                                &sl_rail_util_on_channel_config_change);
  responsePrint(sl_cli_get_command_string(args, 0), "configIndex:%d,firstAvailableChannel:%d",
                configIndex,
                channel);
  if (receiveModeEnabled) {
    (void) RAIL_StartRx(railHandle, channel, NULL);
  }
#else // !SL_RAIL_TEST_RADIO_CONFIG_EXTERNAL_SUPPORT_ENABLED
  responsePrintError(sl_cli_get_command_string(args, 0), 0x22, "External radio config support not enabled");
#endif // SL_RAIL_TEST_RADIO_CONFIG_EXTERNAL_SUPPORT_ENABLED
}

void setTransitionTime(sl_cli_command_arg_t *args)
{
  RAIL_SetTransitionTime((RAIL_Time_t)sl_cli_get_argument_uint32(args, 0));
  responsePrint(sl_cli_get_command_string(args, 0),
                "DMP transition time:%d,"
                "Status:Success",
                RAIL_GetTransitionTime());
}

void getTransitionTime(sl_cli_command_arg_t *args)
{
  responsePrint(sl_cli_get_command_string(args, 0),
                "DMP transition time:%d",
                RAIL_GetTransitionTime());
}

/******************************************************************************
 * Callbacks
 *****************************************************************************/
// This function gets called at the end of sl_rail_util_on_channel_config_change().
void sl_rail_app_on_channel_config_change(RAIL_Handle_t railHandle,
                                          const RAIL_ChannelConfigEntry_t *entry)
{
  (void)entry;
  // Now that the radio config changed, verify the new data contents.
  RAIL_ConfigVerification(railHandle, &configVerify, NULL, NULL);
  if (verifyConfigEnabled && RAIL_Verify(&configVerify, RAIL_VERIFY_DURATION_MAX, true)
      == RAIL_STATUS_INVALID_STATE) {
    responsePrint("verifyRadio", "Radio data corruption detected!");
    while (1) ;
  }

  counters.radioConfigChanged++;
}

/******************************************************************************
 * Boot up radio configuration functions
 *****************************************************************************/
static uint16_t getChannelStatic(RAIL_Handle_t railHandle)
{
  uint16_t channel;
  if (RAIL_STATUS_NO_ERROR == RAIL_GetChannel(railHandle, &channel)) {
    return channel;
  } else {
    return 0;
  }
}

// external radio config
static uint16_t applyExternalConfig(RAIL_Handle_t railHandle)
{
  (void) railHandle;
  uint16_t channel = 0;
#if SL_RAIL_TEST_RADIO_CONFIG_EXTERNAL_SUPPORT_ENABLED
  channel = RAIL_ConfigChannels(railHandle,
                                channelConfigs[configIndex],
                                &sl_rail_util_on_channel_config_change);
#else // !SL_RAIL_TEST_RADIO_CONFIG_EXTERNAL_SUPPORT_ENABLED
  responsePrintError("ExternalRadioConfig", 0x22,
                     "External radio config support not enabled");
#endif // SL_RAIL_TEST_RADIO_CONFIG_EXTERNAL_SUPPORT_ENABLED
  return channel;
}

// internal_ble_1mbps_viterbi radio config
#if RAIL_FEAT_2G4_RADIO
/*
 * Analogous CLI commands:
 * > rx 0
 * > setBleMode 1
 * > setBle1Mbps 1 // 0 for non-viterbi, 1 for viterbi
 * > setTimings 100 150 100 150 0 0
 * > rx 1
 */
static uint16_t applyBleDefault1MbpsViterbiConfig(RAIL_Handle_t railHandle)
{
  // Override BLE's default timings to get rid of the default rx search timeout
  RAIL_StateTiming_t timings = {
    .idleToRx = 100,
    .txToRx = 150,
    .idleToTx = 100,
    .rxToTx = 150,
    .rxSearchTimeout = 0,
    .txToRxSearchTimeout = 0,
  };

  RAIL_BLE_Init(railHandle);
  (void)RAIL_BLE_ConfigPhy1MbpsViterbi(railHandle);
  (void)RAIL_SetStateTiming(railHandle, &timings);
  return getChannelStatic(railHandle);
}
#endif

// internal_ble_2mbps_viterbi radio config
#if RAIL_FEAT_2G4_RADIO
/*
 * Analogous CLI commands:
 * > rx 0
 * > setBleMode 1
 * > setBle2Mbps 1 // 0 for non-viterbi, 1 for viterbi
 * > setTimings 100 150 100 150 0 0
 * > rx 1
 */
static uint16_t applyBleDefault2MbpsViterbiConfig(RAIL_Handle_t railHandle)
{
  // Override BLE's default timings to get rid of the default rx search timeout
  RAIL_StateTiming_t timings = {
    .idleToRx = 100,
    .txToRx = 150,
    .idleToTx = 100,
    .rxToTx = 150,
    .rxSearchTimeout = 0,
    .txToRxSearchTimeout = 0,
  };

  RAIL_BLE_Init(railHandle);
  (void)RAIL_BLE_ConfigPhy2MbpsViterbi(railHandle);
  (void)RAIL_SetStateTiming(railHandle, &timings);
  return getChannelStatic(railHandle);
}
#endif

// internal_ble_125kbps_coded radio config
#if RAIL_FEAT_2G4_RADIO
/*
 * Analogous CLI commands:
 * > rx 0
 * > setBleMode 1
 * > setBleCoding 1 // 0=125kbps, 1=125kbps_DSA, 2=500kbps, 3=500kbps_DSA
 * > setTimings 100 150 100 150 0 0
 * > rx 1
 */
static uint16_t applyBleDefault125KbpsCodedConfig(RAIL_Handle_t railHandle)
{
  // Override BLE's default timings to get rid of the default rx search timeout
  RAIL_StateTiming_t timings = {
    .idleToRx = 100,
    .txToRx = 150,
    .idleToTx = 100,
    .rxToTx = 150,
    .rxSearchTimeout = 0,
    .txToRxSearchTimeout = 0,
  };

  RAIL_BLE_Init(railHandle);
  (void)RAIL_BLE_ConfigPhyCoded(railHandle, RAIL_BLE_Coding_125kbps);
  (void)RAIL_SetStateTiming(railHandle, &timings);
  return getChannelStatic(railHandle);
}
#endif

// internal_ble_500kbps_coded radio config
#if RAIL_FEAT_2G4_RADIO
/*
 * Analogous CLI commands:
 * > rx 0
 * > setBleMode 1
 * > setBleCoding 3 // 0=125kbps, 1=125kbps_DSA, 2=500kbps, 3=500kbps_DSA
 * > setTimings 100 150 100 150 0 0
 * > rx 1
 */
static uint16_t applyBleDefault500KbpsCodedConfig(RAIL_Handle_t railHandle)
{
  // Override BLE's default timings to get rid of the default rx search timeout
  RAIL_StateTiming_t timings = {
    .idleToRx = 100,
    .txToRx = 150,
    .idleToTx = 100,
    .rxToTx = 150,
    .rxSearchTimeout = 0,
    .txToRxSearchTimeout = 0,
  };

  RAIL_BLE_Init(railHandle);
  (void)RAIL_BLE_ConfigPhyCoded(railHandle, RAIL_BLE_Coding_500kbps);
  (void)RAIL_SetStateTiming(railHandle, &timings);
  return getChannelStatic(railHandle);
}
#endif

// internal_ieee802154_2p4ghz radio config
static RAIL_Status_t setIeee802154Mode(RAIL_Handle_t railHandle,
                                       RAIL_RadioState_t ackConfigRxTransitionsSuccess,
                                       RAIL_RadioState_t ackConfigTxTransitionsSuccess,
                                       uint16_t timingsIdleToTx,
                                       uint16_t timingsIdleToRx,
                                       uint16_t timingsRxToTx,
                                       uint16_t timingsTxToRx,
                                       uint16_t ackConfigAckTimeout)
{
  RAIL_DataConfig_t railDataConfig = {
    .txSource = TX_PACKET_DATA,
    .rxSource = RX_PACKET_DATA,
    .txMethod = PACKET_MODE,
    .rxMethod = PACKET_MODE,
  };
  RAIL_ConfigData(railHandle, &railDataConfig);

  RAIL_StateTiming_t timings = {
    .idleToTx = timingsIdleToTx,
    .idleToRx = timingsIdleToRx,
    .rxToTx = timingsRxToTx,
    // Make txToRx slightly lower than desired to make sure we get to
    // RX in time
    .txToRx = timingsTxToRx,
    .rxSearchTimeout = 0,
    .txToRxSearchTimeout = 0
  };

  RAIL_IEEE802154_Config_t config = {
    .addresses = NULL,
    .ackConfig = {
      .enable = true,
      .ackTimeout = ackConfigAckTimeout,
      .rxTransitions = {
        .success = ackConfigRxTransitionsSuccess,
        .error = RAIL_RF_STATE_RX // ignored
      },
      .txTransitions = {
        .success = ackConfigTxTransitionsSuccess,
        .error = RAIL_RF_STATE_RX // ignored
      }
    },
    .timings = timings,
    .framesMask = RAIL_IEEE802154_ACCEPT_STANDARD_FRAMES,
    .promiscuousMode = false,
    .isPanCoordinator = false,
    .defaultFramePendingInOutgoingAcks = false,
  };

  return RAIL_IEEE802154_Init(railHandle, &config);
}

static void setTxLengthStatic(RAIL_Handle_t railHandle, uint32_t length)
{
  txDataLen = length;
  if (railDataConfig.txMethod == PACKET_MODE) {
    RAIL_WriteTxFifo(railHandle, txData, txDataLen, true);
  }
}

static bool setTxPayloadStatic(RAIL_Handle_t railHandle,
                               uint8_t offset, uint16_t len, uint8_t *array)
{
  // Read as many bytes as have been supplied and set them
  for (int i = 0; i < len; i++) {
    uint32_t index = offset + i;
    // Make sure this fits in the txData buffer
    if (index >= sizeof(txData)) {
      return false;
    }
    txData[index] = array[i];
  }

  if (railDataConfig.txMethod == PACKET_MODE) {
    RAIL_WriteTxFifo(railHandle, txData, txDataLen, true);
  }
  return true;
}

#if RAIL_FEAT_2G4_RADIO
/*
 * Analogous CLI commands:
 * > rx 0
 * > config2p4ghz802154 0 0 // 0 = antDiv disabled; 0 = coex disabled
 * > enable802154 rx 100 192 672
 * > setpromiscuousmode 1
 * > rx 1
 * > settxlength 11
 * > settxpayload 0 0x0c 0x63 0x88 0xbe 0xff 0xff 0xff 0xff 0x11 0x22 0x04
 * > tx 1
 */
static uint16_t applyIeee802154Default2p4GhzConfig(RAIL_Handle_t railHandle,
                                                   bool antDivEnabled,
                                                   bool coexEnabled)
{
  (void)setIeee802154Mode(railHandle,
                          RAIL_RF_STATE_RX, // ackConfigRxTransitionsSuccess
                          RAIL_RF_STATE_RX, // ackConfigTxTransitionsSuccess
                          100,              // timingsIdleToTx
                          100,              // timingsIdleToRx
                          192,              // timingsRxToTx
                          182,              // timingsTxToRx
                          672);             // ackConfigAckTimeout
  // Enable promiscous mode since no PANID or destination address is specified.
  (void)RAIL_IEEE802154_SetPromiscuousMode(railHandle, true);

  if (antDivEnabled && coexEnabled) {
    (void)RAIL_IEEE802154_Config2p4GHzRadioAntDivCoex(railHandle);
  } else if (antDivEnabled) {
    (void)RAIL_IEEE802154_Config2p4GHzRadioAntDiv(railHandle);
  } else if (coexEnabled) {
    (void)RAIL_IEEE802154_Config2p4GHzRadioCoex(railHandle);
  } else {
    (void)RAIL_IEEE802154_Config2p4GHzRadio(railHandle);
  }

  setTxLengthStatic(railHandle, 11);
  uint8_t array[11] = { 0x0c, 0x63, 0x88, 0xbe, 0xff,
                        0xff, 0xff, 0xff, 0x11, 0x22,
                        0x04 }; // 11 byte payload
  (void)setTxPayloadStatic(railHandle, 0, sizeof(array), array); // change at offset 0
  return getChannelStatic(railHandle);
}
#endif

// internal_ieee802154_gb915mhz radio config
#if RAIL_FEAT_SUBGIG_RADIO
/*
 * Analogous CLI commands:
 * > rx 0
 * > config915mhz802154
 * > enable802154 rx 100 450 1800
 * > setpromiscuousmode 1
 * > rx 1
 * > settxlength 20
 * > settxpayload 0 0x18 0x28
 * > tx 1
 */
static uint16_t applyIeee802154Defaultgb915MhzConfig(RAIL_Handle_t railHandle)
{
  (void)setIeee802154Mode(railHandle,
                          RAIL_RF_STATE_RX, // ackConfigRxTransitionsSuccess
                          RAIL_RF_STATE_RX, // ackConfigTxTransitionsSuccess
                          100,              // timingsIdleToTx
                          100,              // timingsIdleToRx
                          450,              // timingsRxToTx
                          440,              // timingsTxToRx
                          1800);            // ackConfigAckTimeout
  // Enable promiscous mode since no PANID or destination address is specified.
  (void)RAIL_IEEE802154_SetPromiscuousMode(railHandle, true);
  (void)RAIL_IEEE802154_ConfigGB915MHzRadio(railHandle);
  setTxLengthStatic(railHandle, 20);
  uint8_t array[2] = { 0x18, 0x28 }; // change 2 bytes in the payload
  (void)setTxPayloadStatic(railHandle, 0, sizeof(array), array); // change at offset 0
  return getChannelStatic(railHandle);
}
#endif

// internal_ieee802154_gb863mhz radio config
#if RAIL_FEAT_SUBGIG_RADIO
/*
 * Analogous CLI commands:
 * > rx 0
 * > config863mhz802154
 * > enable802154 rx 100 450 1800
 * > setpromiscuousmode 1
 * > rx 1
 * > settxlength 20
 * > settxpayload 0 0x18 0x28
 * > tx 1
 */
static uint16_t applyIeee802154Defaultgb863MhzConfig(RAIL_Handle_t railHandle)
{
  (void)setIeee802154Mode(railHandle,
                          RAIL_RF_STATE_RX, // ackConfigRxTransitionsSuccess
                          RAIL_RF_STATE_RX, // ackConfigTxTransitionsSuccess
                          100,              // timingsIdleToTx
                          100,              // timingsIdleToRx
                          450,              // timingsRxToTx
                          440,              // timingsTxToRx
                          1800);            // ackConfigAckTimeout
  // Enable promiscous mode since no PANID or destination address is specified.
  (void)RAIL_IEEE802154_SetPromiscuousMode(railHandle, true);
  (void)RAIL_IEEE802154_ConfigGB863MHzRadio(railHandle);
  setTxLengthStatic(railHandle, 20);
  uint8_t array[2] = { 0x18, 0x28 }; // change 2 bytes in the payload
  (void)setTxPayloadStatic(railHandle, 0, sizeof(array), array); // change at offset 0
  return getChannelStatic(railHandle);
}
#endif

// internal_zwave radio config
#if RAIL_FEAT_SUBGIG_RADIO
/*
 * Analogous CLI commands:
 * > rx 0
 * > setZWaveMode 1 3
 * > setZWaveRegion 0 // 0 = European Union
 * > setTxLength 200
 * > setTxPayload 7 20
 * > rx 1
 */
static uint16_t applyZWaveDefaultConfig(RAIL_Handle_t railHandle,
                                        const RAIL_ZWAVE_RegionConfig_t *region)
{
  RAIL_ZWAVE_Config_t config = {
    .options = RAIL_ZWAVE_OPTION_PROMISCUOUS_MODE
               | RAIL_ZWAVE_OPTION_DETECT_BEAM_FRAMES,
  };
  (void)RAIL_ZWAVE_Init(railHandle, &config);
  (void)RAIL_ZWAVE_ConfigRegion(railHandle, region);
  (void)RAIL_ZWAVE_SetNodeId(railHandle, RAIL_ZWAVE_NODE_ID_DEFAULT);
  setTxLengthStatic(railHandle, 200);
  uint8_t array[1] = { 0x20 }; // change 1 byte in the payload
  (void)setTxPayloadStatic(railHandle, 7, sizeof(array), array); // change at offset 7
  return getChannelStatic(railHandle);
}
#endif

uint16_t applyDefaultRadioConfig(RAIL_Handle_t railHandle,
                                 RadioConfigType_t configSelection)
{
  switch (configSelection) {
    case RADIO_CONFIG_EXTERNAL:
      return applyExternalConfig(railHandle);
#if RAIL_FEAT_2G4_RADIO
    case RADIO_CONFIG_INTERNAL_BLE_1MBPS:
      return applyBleDefault1MbpsViterbiConfig(railHandle);
    case RADIO_CONFIG_INTERNAL_BLE_2MBPS:
      return applyBleDefault2MbpsViterbiConfig(railHandle);
    case RADIO_CONFIG_INTERNAL_BLE_CODED_125KBPS:
      return applyBleDefault125KbpsCodedConfig(railHandle);
    case RADIO_CONFIG_INTERNAL_BLE_CODED_500KBPS:
      return applyBleDefault500KbpsCodedConfig(railHandle);
    case RADIO_CONFIG_INTERNAL_IEEE802154_2P4GHZ:
      return applyIeee802154Default2p4GhzConfig(railHandle, false, false);
    case RADIO_CONFIG_INTERNAL_IEEE802154_2P4GHZ_ANTDIV:
      return applyIeee802154Default2p4GhzConfig(railHandle, true, false);
    case RADIO_CONFIG_INTERNAL_IEEE802154_2P4GHZ_COEX:
      return applyIeee802154Default2p4GhzConfig(railHandle, false, true);
    case RADIO_CONFIG_INTERNAL_IEEE802154_2P4GHZ_ANTDIV_COEX:
      return applyIeee802154Default2p4GhzConfig(railHandle, true, true);
#endif
#if RAIL_FEAT_SUBGIG_RADIO
    case RADIO_CONFIG_INTERNAL_IEEE802154_GB868_915MHZ:
      return applyIeee802154Defaultgb915MhzConfig(railHandle);
    case RADIO_CONFIG_INTERNAL_IEEE802154_GB868_863MHZ:
      return applyIeee802154Defaultgb863MhzConfig(railHandle);
    case RADIO_CONFIG_INTERNAL_ZWAVE_EU: // European Union
      return applyZWaveDefaultConfig(railHandle, &RAIL_ZWAVE_REGION_EU);
    case RADIO_CONFIG_INTERNAL_ZWAVE_US: // United States
      return applyZWaveDefaultConfig(railHandle, &RAIL_ZWAVE_REGION_US);
    case RADIO_CONFIG_INTERNAL_ZWAVE_ANZ: // Australia
      return applyZWaveDefaultConfig(railHandle, &RAIL_ZWAVE_REGION_ANZ);
    case RADIO_CONFIG_INTERNAL_ZWAVE_HK: // Hong Kong
      return applyZWaveDefaultConfig(railHandle, &RAIL_ZWAVE_REGION_HK);
    case RADIO_CONFIG_INTERNAL_ZWAVE_MY: // Malaysia
      return applyZWaveDefaultConfig(railHandle, &RAIL_ZWAVE_REGION_MY);
    case RADIO_CONFIG_INTERNAL_ZWAVE_IN: // India
      return applyZWaveDefaultConfig(railHandle, &RAIL_ZWAVE_REGION_IN);
    case RADIO_CONFIG_INTERNAL_ZWAVE_JP: // Japan
      return applyZWaveDefaultConfig(railHandle, &RAIL_ZWAVE_REGION_JP);
    case RADIO_CONFIG_INTERNAL_ZWAVE_RU: // Russia
      return applyZWaveDefaultConfig(railHandle, &RAIL_ZWAVE_REGION_RU);
    case RADIO_CONFIG_INTERNAL_ZWAVE_IL: // Israel
      return applyZWaveDefaultConfig(railHandle, &RAIL_ZWAVE_REGION_IL);
    case RADIO_CONFIG_INTERNAL_ZWAVE_KR: // Korea
      return applyZWaveDefaultConfig(railHandle, &RAIL_ZWAVE_REGION_KR);
    case RADIO_CONFIG_INTERNAL_ZWAVE_CN: // China
      return applyZWaveDefaultConfig(railHandle, &RAIL_ZWAVE_REGION_CN);
#endif
    default:
#if RAIL_FEAT_2G4_RADIO
      return applyBleDefault1MbpsViterbiConfig(railHandle);
#elif RAIL_FEAT_SUBGIG_RADIO
      return applyZWaveDefaultConfig(railHandle, &RAIL_ZWAVE_REGION_EU);
#else
      #error "Radio support is not included in this build."
#endif
  }
}

void disableIncompatibleProtocols(RAIL_PtiProtocol_t newProtocol)
{
  if ((newProtocol != RAIL_PTI_PROTOCOL_BLE)
      && RAIL_BLE_IsEnabled(railHandle)) {
    (void) RAIL_BLE_Deinit(railHandle);
  }
  if ((newProtocol != RAIL_PTI_PROTOCOL_ZIGBEE)
      && RAIL_IEEE802154_IsEnabled(railHandle)) {
    (void) RAIL_IEEE802154_Deinit(railHandle);
  }
  if ((newProtocol != RAIL_PTI_PROTOCOL_ZWAVE)
      && RAIL_ZWAVE_IsEnabled(railHandle)) {
    (void) RAIL_ZWAVE_Deinit(railHandle);
  }
}

void configRfSenseSelectiveOokWakeupPhy(sl_cli_command_arg_t *args)
{
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }

  RAIL_Status_t status = RAIL_ConfigRfSenseSelectiveOokWakeupPhy(railHandle);
  responsePrint(sl_cli_get_command_string(args, 0), "RFSense Wakeup PHY:%s", status ? "Disabled" : "Enabled");
}
