/***************************************************************************//**
 * @file
 * @brief
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
#include "em_ldma.h"
#include "dmadrv.h"

#include "rail.h"
#include "rail_ble.h" // for RAIL_BLE_State_t
#include "sl_rail_util_init.h"
#include "sl_rail_util_init_config.h"

#if SL_RAIL_UTIL_SETUP_PROPRIETARY_CHANNEL_CONFIG_ON_INIT_ENABLE
  #include "rail_config.h"
#endif

static RAIL_Handle_t sl_rail_handle;
#if SL_RAIL_UTIL_BLE_SUPPORT_ENABLE
static RAIL_BLE_State_t sl_rail_ble_state; // needed for BLE (ignored by others)
#endif // SL_RAIL_UTIL_BLE_SUPPORT_ENABLE
#if SL_RAIL_UTIL_SCHEDULER_SUPPORT_ENABLE
static RAILSched_Config_t sl_rail_sched_config;
#endif // SL_RAIL_UTIL_SCHEDULER_SUPPORT_ENABLE
static RAIL_Config_t sl_rail_config = {
  .eventsCallback = &sl_rail_util_on_event,
#if SL_RAIL_UTIL_BLE_SUPPORT_ENABLE
  .protocol = &sl_rail_ble_state,
#endif // SL_RAIL_UTIL_BLE_SUPPORT_ENABLE
#if SL_RAIL_UTIL_SCHEDULER_SUPPORT_ENABLE
  .scheduler = &sl_rail_sched_config,
#endif // SL_RAIL_UTIL_SCHEDULER_SUPPORT_ENABLE
};

RAIL_Handle_t sl_rail_util_get_handle(void)
{
  return sl_rail_handle;
}

void sl_rail_util_init(void)
{
#if SL_RAIL_UTIL_DMA_ENABLE
#if SL_RAIL_UTIL_DMA_DMADRV_ENABLE
  Ecode_t dmaError = DMADRV_Init();
  if ((dmaError == ECODE_EMDRV_DMADRV_ALREADY_INITIALIZED)
      || (dmaError == ECODE_EMDRV_DMADRV_OK)) {
    unsigned int channel;
    dmaError = DMADRV_AllocateChannel(&channel, NULL);
    if (dmaError == ECODE_EMDRV_DMADRV_OK) {
      (void) RAIL_UseDma(channel);
    }
  }
#else // !SL_RAIL_UTIL_DMA_DMADRV_ENABLE
  LDMA_Init_t ldmaInit = LDMA_INIT_DEFAULT;
  LDMA_Init(&ldmaInit);
  (void) RAIL_UseDma(SL_RAIL_UTIL_DMA_CHANNEL);
#endif // SL_RAIL_UTIL_DMA_DMADRV_ENABLE
#endif // SL_RAIL_UTIL_DMA_ENABLE

#if SL_RAIL_UTIL_INIT_ENABLE
  sl_rail_handle = RAIL_Init(&sl_rail_config,
#if SL_RAIL_UTIL_INIT_COMPLETE_CALLBACK_ENABLE
                             &sl_rail_util_on_rf_ready
#else
                             NULL
#endif // SL_RAIL_UTIL_INIT_COMPLETE_CALLBACK_ENABLE
                             );

#if SL_RAIL_UTIL_SETUP_CHANNEL_CONFIG_ON_INIT_ENABLE
  (void) RAIL_ConfigChannels(sl_rail_handle,
#if SL_RAIL_UTIL_SETUP_PROPRIETARY_CHANNEL_CONFIG_ON_INIT_ENABLE
                             channelConfigs[SL_RAIL_UTIL_SETUP_PROPRIETARY_CHANNEL_CONFIG_ON_INIT_INDEX],
#else
                             NULL,
#endif
                             &sl_rail_util_on_channel_config_change);
#endif // SL_RAIL_UTIL_SETUP_CHANNEL_CONFIG_ON_INIT_ENABLE

#if SL_RAIL_UTIL_SETUP_CAL_ON_INIT_ENABLE
  (void) RAIL_ConfigCal(sl_rail_handle,
                        (SL_RAIL_UTIL_CAL_TEMP_ENABLE ? RAIL_CAL_TEMP : 0U)
                        | (SL_RAIL_UTIL_CAL_ONETIME_ENABLE ? RAIL_CAL_ONETIME : 0U));
#endif // SL_RAIL_UTIL_SETUP_CAL_ON_INIT_ENABLE

#if SL_RAIL_UTIL_SETUP_EVENTS_ON_INIT_ENABLE
  (void) RAIL_ConfigEvents(sl_rail_handle,
                           RAIL_EVENTS_ALL,
                           (SL_RAIL_UTIL_EVENT_RSSI_AVERAGE_DONE_ENABLE ? RAIL_EVENT_RSSI_AVERAGE_DONE : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_ACK_TIMEOUT_ENABLE ? RAIL_EVENT_RX_ACK_TIMEOUT : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_FIFO_ALMOST_FULL_ENABLE ? RAIL_EVENT_RX_FIFO_ALMOST_FULL : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_PACKET_RECEIVED_ENABLE ? RAIL_EVENT_RX_PACKET_RECEIVED : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_PREAMBLE_LOST_ENABLE ? RAIL_EVENT_RX_PREAMBLE_LOST : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_PREAMBLE_DETECT_ENABLE ? RAIL_EVENT_RX_PREAMBLE_DETECT : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_SYNC1_DETECT_ENABLE ? RAIL_EVENT_RX_SYNC1_DETECT : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_SYNC2_DETECT_ENABLE ? RAIL_EVENT_RX_SYNC2_DETECT : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_FRAME_ERROR_ENABLE ? RAIL_EVENT_RX_FRAME_ERROR : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_FIFO_FULL_ENABLE ? RAIL_EVENT_RX_FIFO_FULL : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_FIFO_OVERFLOW_ENABLE ? RAIL_EVENT_RX_FIFO_OVERFLOW : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_ADDRESS_FILTERED_ENABLE ? RAIL_EVENT_RX_ADDRESS_FILTERED : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_TIMEOUT_ENABLE ? RAIL_EVENT_RX_TIMEOUT : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_TX_SCHEDULED_RX_TX_STARTED_ENABLE ? RAIL_EVENT_SCHEDULED_RX_STARTED : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_SCHEDULED_RX_END_ENABLE ? RAIL_EVENT_RX_SCHEDULED_RX_END : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_SCHEDULED_RX_MISSED_ENABLE ? RAIL_EVENT_RX_SCHEDULED_RX_MISSED : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_PACKET_ABORTED_ENABLE ? RAIL_EVENT_RX_PACKET_ABORTED : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_FILTER_PASSED_ENABLE ? RAIL_EVENT_RX_FILTER_PASSED : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_TIMING_LOST_ENABLE ? RAIL_EVENT_RX_TIMING_LOST : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_TIMING_DETECT_ENABLE ? RAIL_EVENT_RX_TIMING_DETECT : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_CHANNEL_HOPPING_COMPLETE_ENABLE ? RAIL_EVENT_RX_CHANNEL_HOPPING_COMPLETE : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_RX_DUTY_CYCLE_RX_END_ENABLE ? RAIL_EVENT_RX_DUTY_CYCLE_RX_END : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND_ENABLE ? RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_ZWAVE_BEAM_ENABLE ? RAIL_EVENT_ZWAVE_BEAM : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_TX_FIFO_ALMOST_EMPTY_ENABLE ? RAIL_EVENT_TX_FIFO_ALMOST_EMPTY : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_TX_PACKET_SENT_ENABLE ? RAIL_EVENT_TX_PACKET_SENT : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_TXACK_PACKET_SENT_ENABLE ? RAIL_EVENT_TXACK_PACKET_SENT : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_TX_ABORTED_ENABLE ? RAIL_EVENT_TX_ABORTED : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_TXACK_ABORTED_ENABLE ? RAIL_EVENT_TXACK_ABORTED : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_TX_BLOCKED_ENABLE ? RAIL_EVENT_TX_BLOCKED : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_TXACK_BLOCKED_ENABLE ? RAIL_EVENT_TXACK_BLOCKED : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_TX_UNDERFLOW_ENABLE ? RAIL_EVENT_TX_UNDERFLOW : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_TXACK_UNDERFLOW_ENABLE ? RAIL_EVENT_TXACK_UNDERFLOW : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_TX_CHANNEL_CLEAR_ENABLE ? RAIL_EVENT_TX_CHANNEL_CLEAR : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_TX_CHANNEL_BUSY_ENABLE ? RAIL_EVENT_TX_CHANNEL_BUSY : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_TX_CCA_RETRY_ENABLE ? RAIL_EVENT_TX_CCA_RETRY : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_TX_START_CCA_ENABLE ? RAIL_EVENT_TX_START_CCA : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_TX_STARTED_ENABLE ? RAIL_EVENT_TX_STARTED : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_TX_SCHEDULED_TX_MISSED_ENABLE ? RAIL_EVENT_TX_SCHEDULED_TX_MISSED : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_CONFIG_UNSCHEDULED_ENABLE ? RAIL_EVENT_CONFIG_UNSCHEDULED : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_CONFIG_SCHEDULED_ENABLE ? RAIL_EVENT_CONFIG_SCHEDULED : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_SCHEDULER_STATUS_ENABLE ? RAIL_EVENT_SCHEDULER_STATUS : RAIL_EVENTS_NONE)
                           | (SL_RAIL_UTIL_EVENT_CAL_NEEDED_ENABLE ? RAIL_EVENT_CAL_NEEDED : RAIL_EVENTS_NONE));
#endif // SL_RAIL_UTIL_SETUP_EVENTS_ON_INIT_ENABLE

#if SL_RAIL_UTIL_SETUP_TRANSITIONS_ON_INIT_ENABLE
  RAIL_StateTransitions_t tx_transitions = {
    .success = SL_RAIL_UTIL_TRANSITION_TX_SUCCESS,
    .error = SL_RAIL_UTIL_TRANSITION_TX_ERROR
  };
  RAIL_StateTransitions_t rx_transitions = {
    .success = SL_RAIL_UTIL_TRANSITION_RX_SUCCESS,
    .error = SL_RAIL_UTIL_TRANSITION_RX_ERROR
  };
  (void) RAIL_SetTxTransitions(sl_rail_handle, &tx_transitions);
  (void) RAIL_SetRxTransitions(sl_rail_handle, &rx_transitions);
#endif // SL_RAIL_UTIL_SETUP_TRANSITIONS_ON_INIT_ENABLE
#endif // SL_RAIL_UTIL_INIT_ENABLE
}
