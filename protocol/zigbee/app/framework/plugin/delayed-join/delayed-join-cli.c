/***************************************************************************//**
 * @file
 * @brief User interface for delayd join.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "app/framework/util/common.h"
#include "app/framework/util/util.h"
#include "app/framework/plugin/delayed-join/delayed-join.h"

#include "app/framework/util/af-main.h"
#include "stack/include/trust-center.h"

// *****************************************************************************
// Functions

void emAfDelayedJoinActivateCommand(void)
{
  #if defined(EZSP_HOST)
  EzspStatus status = emberAfSetEzspPolicy(EZSP_TRUST_CENTER_POLICY,
                                           (EZSP_DECISION_ALLOW_JOINS
                                            | EZSP_DECISION_ALLOW_UNSECURED_REJOINS
                                            | EZSP_DECISION_DEFER_JOINS),
                                           "Trust Center Policy",
                                           "Delayed joins");

  if (EZSP_SUCCESS != status) {
    emberAfCorePrintln("%p: %p: 0x%X",
                       EMBER_AF_PLUGIN_DELAYED_JOIN_PLUGIN_NAME,
                       "failed to configure delayed joining",
                       status);
  }
  #endif // EZSP_HOST
}

void emAfDelayedJoinSendNetworkKeyCommand(void)
{
  EmberNodeId targetShort = (EmberNodeId)emberUnsignedCommandArgument(0);
  EmberEUI64  targetLong;
  emberCopyBigEndianEui64Argument(1, targetLong);
  EmberNodeId parentShort = (EmberNodeId)emberUnsignedCommandArgument(2);

  if (EMBER_SUCCESS != emberUnicastCurrentNetworkKey(targetShort,
                                                     targetLong,
                                                     parentShort)) {
    emberAfCorePrintln("%p: %p 0x%X",
                       EMBER_AF_PLUGIN_DELAYED_JOIN_PLUGIN_NAME,
                       "failed to send the encrypted network key to ",
                       targetShort);
  }
}

void emAfDelayedJoinSetNetworkKeyTimeoutCommand(void)
{
  uint8_t seconds = (uint8_t)emberUnsignedCommandArgument(0);
  emberAfPluginDelayedJoinSetNetworkKeyTimeout(seconds);
}
