/***************************************************************************//**
 * @file
 * @brief CLI for the Green Power Server plugin.
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
#include EMBER_AF_API_GREEN_POWER_SERVER
#include EMBER_AF_API_GREEN_POWER_COMMON

#ifndef EMBER_AF_GENERATE_CLI
#error The Green Power Server plugin is not compatible with the legacy CLI.
#endif

// Sink Commissioning Mode Enter
void emberAfPluginGreenPowerServerCommissioningMode(void)
{
  uint8_t options = emberUnsignedCommandArgument(0);
  uint16_t gpmAddressSecurity = emberUnsignedCommandArgument(1);
  uint16_t gpmAddressPairing = emberUnsignedCommandArgument(2);
  uint8_t endpoint = emberUnsignedCommandArgument(3);
  emberAfGreenPowerClusterGpSinkCommissioningModeCallback(options,
                                                          gpmAddressSecurity,
                                                          gpmAddressPairing,
                                                          endpoint);
}

// Sink Commissioning Window Extend
void emberAfGreenPowerClusterGpSinkCliCommissioningWindowExtend(void)
{
  uint16_t commissioningWindow = emberUnsignedCommandArgument(0);
  emberAfGreenPowerClusterGpSinkCommissioningWindowExtend(commissioningWindow);
}

void emberAfPluginGreenPowerServerCliClearSinkTable(void)
{
  emberGpSinkTableClearAll();
}

void emberAfPluginGreenPowerServerCliSinkTablePrint(void)
{
  bool tableEmpty = true;
  emberAfCorePrintln("\n\rSt Optn Ap SourceId Ep Dv Alis Gr So FrameCtr");
  for (uint8_t index = 0; index < EMBER_GP_SINK_TABLE_SIZE; index++) {
    EmberGpSinkTableEntry entry = { 0 };
    EmberStatus status = emberGpSinkTableGetEntry(index, &entry);
    if (status == EMBER_SUCCESS
        && entry.status == EMBER_GP_SINK_TABLE_ENTRY_STATUS_ACTIVE) {
      tableEmpty = false;
      emberAfCorePrintln("%x %2x %x %4x %x %x %2x %x %x %4x",
                         entry.status,
                         entry.options,
                         entry.gpd.applicationId,
                         entry.gpd.id.sourceId,
                         entry.gpd.endpoint,
                         entry.deviceId,
                         entry.assignedAlias,
                         entry.groupcastRadius,
                         entry.securityOptions,
                         entry.gpdSecurityFrameCounter);
    }
  }
  if (tableEmpty) {
    emberAfCorePrintln("\n\rSink Table Empty");
  }
}
