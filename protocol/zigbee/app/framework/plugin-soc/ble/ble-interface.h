/***************************************************************************//**
 * @file
 * @brief Definitions for the BLE plugin.
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

#include "sl_bt_rtos_adaptation.h"
#include "sl_bt_ll_config.h"
#include "sl_bt_stack_config.h"

void emberAfPluginBleGetConfig(sl_bt_configuration_t* config);

sl_bt_bluetooth_ll_priorities *emberAfPluginBleGetLlPrioritiesConfig(void);

bool emberAfPluginBleHasEventPending(void);
