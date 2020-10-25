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

#ifndef SL_RAIL_UTIL_INIT_CONFIG_H
#define SL_RAIL_UTIL_INIT_CONFIG_H

#include "rail_chip_specific.h"

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Init Configuration

// <q SL_RAIL_UTIL_INIT_ENABLE> Enable RAIL Initialization
// <i> Default: 1
#define SL_RAIL_UTIL_INIT_ENABLE 1

// <h> Init Support Selection
// <q SL_RAIL_UTIL_BLE_SUPPORT_ENABLE> Enable Bluetooth BLE Support
// <i> Default: 1
#define SL_RAIL_UTIL_BLE_SUPPORT_ENABLE 1
// <q SL_RAIL_UTIL_SCHEDULER_SUPPORT_ENABLE> Enable Dynamic Multiprotocol (DMP) Support
// <i> Default: 0
#define SL_RAIL_UTIL_SCHEDULER_SUPPORT_ENABLE 0
// <q SL_RAIL_UTIL_INIT_COMPLETE_CALLBACK_ENABLE> Enable Init Complete Callback
// <i> Default: 1
#define SL_RAIL_UTIL_INIT_COMPLETE_CALLBACK_ENABLE 1
// </h>

// </h>

// <h> DMA Configuration

// <q SL_RAIL_UTIL_DMA_ENABLE> Allocate DMA channel to RAIL (to decrease channel config switching time).
// <i> Default: 1
#define SL_RAIL_UTIL_DMA_ENABLE  1

// <h> DMA Selection
// <q SL_RAIL_UTIL_DMA_DMADRV_ENABLE> Use DMA Driver (i.e. auto-select DMA channel)
// <i> Default: 1
#define SL_RAIL_UTIL_DMA_DMADRV_ENABLE  1
// <o SL_RAIL_UTIL_DMA_CHANNEL> Use Specific DMA Channel (if auto-selection disabled)
// <0-16:1>
// <i> Default: 0
#define SL_RAIL_UTIL_DMA_CHANNEL  0
// </h>

// </h>

// <h> Channel Configuration

// <q SL_RAIL_UTIL_SETUP_CHANNEL_CONFIG_ON_INIT_ENABLE> Setup Channel Configuration on Init
// <i> Default: 1
#define SL_RAIL_UTIL_SETUP_CHANNEL_CONFIG_ON_INIT_ENABLE 1

// <h> Proprietary Channel Configuration
// <q SL_RAIL_UTIL_SETUP_PROPRIETARY_CHANNEL_CONFIG_ON_INIT_ENABLE> Setup Proprietary Channel Config on Init (i.e. use channelConfigs from rail_config.c/h)
// <i> Default: 1
#define SL_RAIL_UTIL_SETUP_PROPRIETARY_CHANNEL_CONFIG_ON_INIT_ENABLE 1
// <o SL_RAIL_UTIL_SETUP_PROPRIETARY_CHANNEL_CONFIG_ON_INIT_INDEX> If enabled, use this Proprietary Channel Config array Index (i.e. use index X of channelConfigs[X])
// <0-255:1>
// <i> Default: 0
#define SL_RAIL_UTIL_SETUP_PROPRIETARY_CHANNEL_CONFIG_ON_INIT_INDEX 0
// </h>

// </h>

// <h> Calibration Configuration

// <q SL_RAIL_UTIL_SETUP_CAL_ON_INIT_ENABLE> Setup Calibrations on Init
// <i> Default: 1
#define SL_RAIL_UTIL_SETUP_CAL_ON_INIT_ENABLE 1

// <h> Specific Calibration Configurations
// <q SL_RAIL_UTIL_CAL_TEMP_ENABLE> Enable Temperature Calibrations (e.g. voltage controlled oscillator calibration)
// <i> Default: 1
#define SL_RAIL_UTIL_CAL_TEMP_ENABLE 1
// <q SL_RAIL_UTIL_CAL_ONETIME_ENABLE> Enable One-time Calibrations (e.g. image rejection calibration)
// <i> Default: 1
#define SL_RAIL_UTIL_CAL_ONETIME_ENABLE 1
// </h>

// </h>

// <h> Auto Transition Configuration

// <q SL_RAIL_UTIL_SETUP_TRANSITIONS_ON_INIT_ENABLE> Enable Automatic Radio State Transitions
// <i> Default: 1
#define SL_RAIL_UTIL_SETUP_TRANSITIONS_ON_INIT_ENABLE 1

// <h> TX Transitions
// <o SL_RAIL_UTIL_TRANSITION_TX_SUCCESS> Transition on TX Success
// <RAIL_RF_STATE_RX=> RX
// <RAIL_RF_STATE_IDLE=> Idle
// <i> Default: RAIL_RF_STATE_IDLE
#define SL_RAIL_UTIL_TRANSITION_TX_SUCCESS  RAIL_RF_STATE_IDLE
// <o SL_RAIL_UTIL_TRANSITION_TX_ERROR> Transition on TX Error
// <RAIL_RF_STATE_RX=> RX
// <RAIL_RF_STATE_IDLE=> Idle
// <i> Default: RAIL_RF_STATE_IDLE
#define SL_RAIL_UTIL_TRANSITION_TX_ERROR  RAIL_RF_STATE_IDLE
// </h>

// <h> RX Transitions
// <o SL_RAIL_UTIL_TRANSITION_RX_SUCCESS> Transition on RX Success
// <RAIL_RF_STATE_RX=> RX
// <RAIL_RF_STATE_TX=> TX
// <RAIL_RF_STATE_IDLE=> Idle
// <i> Default: RAIL_RF_STATE_IDLE
#define SL_RAIL_UTIL_TRANSITION_RX_SUCCESS  RAIL_RF_STATE_IDLE
// <o SL_RAIL_UTIL_TRANSITION_RX_ERROR> Transition on RX Error
// <RAIL_RF_STATE_RX=> RX
// <RAIL_RF_STATE_IDLE=> Idle
// <i> Default: RAIL_RF_STATE_IDLE
#define SL_RAIL_UTIL_TRANSITION_RX_ERROR  RAIL_RF_STATE_IDLE
// </h>

// </h>

// <h> Event Configuration

// <q SL_RAIL_UTIL_SETUP_EVENTS_ON_INIT_ENABLE> Enable Setup of Radio Events
// <i> Default: 1
#define SL_RAIL_UTIL_SETUP_EVENTS_ON_INIT_ENABLE 1

// <h> RX Events
// <q SL_RAIL_UTIL_EVENT_RSSI_AVERAGE_DONE_ENABLE> RSSI Average Done
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_RSSI_AVERAGE_DONE_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_RX_FIFO_ALMOST_FULL_ENABLE> RX FIFO, Almost Full
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_RX_FIFO_ALMOST_FULL_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_RX_PACKET_RECEIVED_ENABLE> RX Packet Received
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_RX_PACKET_RECEIVED_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_RX_PREAMBLE_LOST_ENABLE> RX Preamble Lost
// <i> Default: 0
#define SL_RAIL_UTIL_EVENT_RX_PREAMBLE_LOST_ENABLE 0
// <q SL_RAIL_UTIL_EVENT_RX_PREAMBLE_DETECT_ENABLE> RX Preamble Detect
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_RX_PREAMBLE_DETECT_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_RX_SYNC1_DETECT_ENABLE> RX Sync1 Detect
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_RX_SYNC1_DETECT_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_RX_SYNC2_DETECT_ENABLE> RX Sync2 Detect
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_RX_SYNC2_DETECT_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_RX_FRAME_ERROR_ENABLE> RX Frame Error
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_RX_FRAME_ERROR_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_RX_FIFO_FULL_ENABLE> RX FIFO, Full
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_RX_FIFO_FULL_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_RX_FIFO_OVERFLOW_ENABLE> RX FIFO, Overflow
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_RX_FIFO_OVERFLOW_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_RX_ADDRESS_FILTERED_ENABLE> RX Address Filtered
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_RX_ADDRESS_FILTERED_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_RX_TIMEOUT_ENABLE> RX Timeout
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_RX_TIMEOUT_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_RX_TX_SCHEDULED_RX_TX_STARTED_ENABLE> Scheduled RX/TX Started
// <i> Default: 0
#define SL_RAIL_UTIL_EVENT_RX_TX_SCHEDULED_RX_TX_STARTED_ENABLE 0
// <q SL_RAIL_UTIL_EVENT_RX_SCHEDULED_RX_END_ENABLE> Scheduled RX End
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_RX_SCHEDULED_RX_END_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_RX_SCHEDULED_RX_MISSED_ENABLE> Scheduled RX Missed
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_RX_SCHEDULED_RX_MISSED_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_RX_PACKET_ABORTED_ENABLE> RX Packet Aborted
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_RX_PACKET_ABORTED_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_RX_FILTER_PASSED_ENABLE> RX Filter Passed
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_RX_FILTER_PASSED_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_RX_TIMING_LOST_ENABLE> RX Timing Lost
// <i> Default: 0
#define SL_RAIL_UTIL_EVENT_RX_TIMING_LOST_ENABLE 0
// <q SL_RAIL_UTIL_EVENT_RX_TIMING_DETECT_ENABLE> RX Timing Detect
// <i> Default: 0
#define SL_RAIL_UTIL_EVENT_RX_TIMING_DETECT_ENABLE 0
// <q SL_RAIL_UTIL_EVENT_RX_CHANNEL_HOPPING_COMPLETE_ENABLE> RX Channel Hopping Complete
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_RX_CHANNEL_HOPPING_COMPLETE_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_RX_DUTY_CYCLE_RX_END_ENABLE> RX Duty Cycle RX End
// <i> Default: 0
#define SL_RAIL_UTIL_EVENT_RX_DUTY_CYCLE_RX_END_ENABLE 0
// <q SL_RAIL_UTIL_EVENT_RX_ACK_TIMEOUT_ENABLE> RX ACK, Timeout
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_RX_ACK_TIMEOUT_ENABLE 1
// </h>

// <h> TX Events
// <q SL_RAIL_UTIL_EVENT_TX_FIFO_ALMOST_EMPTY_ENABLE> TX FIFO, Almost Empty
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_TX_FIFO_ALMOST_EMPTY_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_TX_PACKET_SENT_ENABLE> TX Packet Sent
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_TX_PACKET_SENT_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_TX_ABORTED_ENABLE> TX Aborted
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_TX_ABORTED_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_TX_BLOCKED_ENABLE> TX Blocked
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_TX_BLOCKED_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_TX_UNDERFLOW_ENABLE> TX FIFO, Underflow
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_TX_UNDERFLOW_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_TX_CHANNEL_CLEAR_ENABLE> TX CCA, Channel Clear
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_TX_CHANNEL_CLEAR_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_TX_CHANNEL_BUSY_ENABLE> TX CCA, Channel Busy
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_TX_CHANNEL_BUSY_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_TX_CCA_RETRY_ENABLE> TX CCA, Retry
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_TX_CCA_RETRY_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_TX_START_CCA_ENABLE> TX CCA, Started
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_TX_START_CCA_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_TX_STARTED_ENABLE> TX Started
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_TX_STARTED_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_TX_SCHEDULED_TX_MISSED_ENABLE> Scheduled TX Missed
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_TX_SCHEDULED_TX_MISSED_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_TXACK_PACKET_SENT_ENABLE> TX ACK, Packet Sent
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_TXACK_PACKET_SENT_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_TXACK_ABORTED_ENABLE> TX ACK, Aborted
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_TXACK_ABORTED_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_TXACK_BLOCKED_ENABLE> TX ACK, Blocked
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_TXACK_BLOCKED_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_TXACK_UNDERFLOW_ENABLE> TX ACK, Underflow
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_TXACK_UNDERFLOW_ENABLE 1
// </h>

// <h> Protocol-specific Events
// <q SL_RAIL_UTIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND_ENABLE> IEEE 802.15.4 Data Request Command
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND_ENABLE 1
// <q SL_RAIL_UTIL_EVENT_ZWAVE_BEAM_ENABLE> Z-Wave Beam
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_ZWAVE_BEAM_ENABLE 1
// </h>

// <h> Dynamic Multiprotocol (DMP) specific Events
// <q SL_RAIL_UTIL_EVENT_CONFIG_UNSCHEDULED_ENABLE> Config Unscheduled
// <i> Default: 0
#define SL_RAIL_UTIL_EVENT_CONFIG_UNSCHEDULED_ENABLE 0
// <q SL_RAIL_UTIL_EVENT_CONFIG_SCHEDULED_ENABLE> Config Scheduled
// <i> Default: 0
#define SL_RAIL_UTIL_EVENT_CONFIG_SCHEDULED_ENABLE 0
// <q SL_RAIL_UTIL_EVENT_SCHEDULER_STATUS_ENABLE> Scheduler Status
// <i> Default: 0
#define SL_RAIL_UTIL_EVENT_SCHEDULER_STATUS_ENABLE 0
// </h>

// <h> Maintenance Events
// <q SL_RAIL_UTIL_EVENT_CAL_NEEDED_ENABLE> Calibration Needed
// <i> Default: 1
#define SL_RAIL_UTIL_EVENT_CAL_NEEDED_ENABLE 1
// </h>

// </h>

// <<< end of configuration section >>>

#endif // SL_RAIL_UTIL_INIT_CONFIG_H
