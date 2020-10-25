/***************************************************************************//**
 * @file
 * @brief Code for delayed join.
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

#include "app/framework/util/af-main.h"
#include "app/framework/security/af-security.h"
#include "delayed-join.h"
// *****************************************************************************
// Globals

// *****************************************************************************
// Functions

void emberAfTrustCenterJoinCallback(EmberNodeId newNodeId,
                                    EmberEUI64 newNodeEui64,
                                    EmberNodeId parentOfNewNode,
                                    EmberDeviceUpdate status,
                                    EmberJoinDecision decision)
{
  emberAfCorePrintln("%p: 0x%2X %p %X %p: 0x%2X",
                     EMBER_AF_PLUGIN_DELAYED_JOIN_PLUGIN_NAME,
                     newNodeId,
                     " is attempting to do ",
                     status,
                     " with parentId",
                     parentOfNewNode);
  emberAfPluginDelayedJoinCallback(newNodeId,
                                   newNodeEui64,
                                   parentOfNewNode,
                                   status,
                                   decision);
}

void emberAfPluginDelayedJoinStackStatusCallback(EmberStatus status)
{
  #if defined(EZSP_HOST)
  if (status == EMBER_NETWORK_UP) {
    EzspStatus status = emberAfSetEzspPolicy(EZSP_TRUST_CENTER_POLICY,
                                             (EZSP_DECISION_ALLOW_JOINS
                                              | EZSP_DECISION_ALLOW_UNSECURED_REJOINS
                                              | EZSP_DECISION_DEFER_JOINS),
                                             "Trust Center Policy",
                                             "Delay joins");

    if (EZSP_SUCCESS != status) {
      emberAfCorePrintln("%p: %p: 0x%X",
                         EMBER_AF_PLUGIN_DELAYED_JOIN_PLUGIN_NAME,
                         "failed to configure delayed joining",
                         status);
    }
  }
  #endif // EZSP_HOST
}

void emberAfPluginDelayedJoinSetNetworkKeyTimeout(uint8_t timeout)
{
  #if defined(EZSP_HOST)
  if (EZSP_SUCCESS != ezspSetValue(EZSP_VALUE_NWK_KEY_TIMEOUT, 1, &timeout)) {
    emberAfCorePrintln("%p: %p",
                       EMBER_AF_PLUGIN_DELAYED_JOIN_PLUGIN_NAME,
                       "\nCould not set the network timeout value. Acceptable timeout range[3,255]\n");
  }

  #else
  if (EMBER_SUCCESS != emberSetupDelayedJoin(timeout)) {
    emberAfCorePrintln("%p: %p",
                       EMBER_AF_PLUGIN_DELAYED_JOIN_PLUGIN_NAME,
                       "\nCould not set the network timeout value. Acceptable timeout range[3,255]\n");
  }

  #endif
}
