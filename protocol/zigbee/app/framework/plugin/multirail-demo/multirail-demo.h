/***************************************************************************//**
 * @file: multirail-demo.h
 * @brief Prototypes of routines to utilise a second instance of RAIL.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef MULTIRAIL_DEMO_H
#define MULTIRAIL_DEMO_H

#include "rail.h"
#include "rail_types.h"

/*
 * Please note that all these functions are provided for your convenience.
 * You can override any of them by using your own implementation.
 */

/** @brief Initialize a new RAIL handle.
 *
 * @param railCfg RAIL init options (NULL to use the current Zigbee config)
 * @param txPowerConfig RAIL power options (NULL to use defaults)
 * @param paAutoMode Whether an automatic PA mode is enabled
 * @param defaultTxPower Default TX power in deci-dBm
 * @param txFifo Pointer to a TX FIFO buffer (NULL to use the default)
 * @param txFifoSize Size of the TX buffer
 * @param panId PAN ID (use 0xFFFF if not needed)
 * @param ieeeAddr Long IEEE address (use NULL if not needed)
 *
 * @return New RAIL handle, NULL if not successfully initialized.
 */
RAIL_Handle_t emberAfPluginMultirailDemoInit(RAIL_Config_t *railCfg,
                                             RAIL_TxPowerConfig_t *txPowerConfig,
                                             bool paAutoMode,
                                             RAIL_TxPower_t defaultTxPower,
                                             uint8_t *txFifo,
                                             uint16_t txFifoSize,
                                             uint16_t panId,
                                             const uint8_t *ieeeAddr);

/** @brief Return the second RAIL handle.
 *
 * @return The second RAIL handle. Null if not initialized.
 */
RAIL_Handle_t emberAfPluginMultirailDemoGetHandle(void);

/** @brief Send a packet using the second RAIL instance.
 *
 * @param buff Pointer to the data (make sue it is properly formatted)
 * @param size Size of the data
 * @param channel Channel (11-26, or 0 to use the current Zigbee channel)
 * @param scheduledTxConfig (optional)
 * @param schedulerInfo (optional) Scheduler configuration (NULL to use defaults)
 *
 * @return RAIL status.
 */
RAIL_Status_t emberAfPluginMultirailDemoSend(const uint8_t *buff,
                                             uint32_t size,
                                             uint8_t channel,
                                             RAIL_ScheduleTxConfig_t *scheduledTxConfig,
                                             RAIL_SchedulerInfo_t *schedulerInfo);

#endif // MULTIRAIL_DEMO_H
