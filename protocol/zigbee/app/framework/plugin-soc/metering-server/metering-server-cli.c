/***************************************************************************//**
 * @file
 * @brief CLI for the Metering Server plugin.
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
#include "metering-interface.h"
#include "metering-server.h"

// plugin simple-metering-server profiles <data:1>
void emberAfPluginMeteringServerCliProfiles(void)
{
#if (EMBER_AF_PLUGIN_SIMPLE_METERING_SERVER_METER_PROFILES != 0)
  emberAfPluginMeteringServerInterfaceEnableProfiles(
    (uint8_t)emberUnsignedCommandArgument(0));
#else
  emberAfCorePrintln("Not applicable for 0 configured profiles.");
#endif
}

void emberAfPluginMeteringServerCliDisableFastPolling(void)
{
  bool setFastPolling = (bool)emberUnsignedCommandArgument(0);
  emberAfPluginMeteringServerEnableFastPolling(setFastPolling);
}
