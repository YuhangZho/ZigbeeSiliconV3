/***************************************************************************//**
 * @file
 * @brief CLI for the LV Shutdown plugin.
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

#include "af.h"
#include "lv-shutdown.h"

#ifdef EMBER_AF_LEGACY_CLI
  #error The Low Voltage Shutdown plugin is not compatible with the legacy CLI.
#endif

// ****** CLI Command handlers ******
void emberAfPluginLowVoltageShutdownEnableCommand(void)
{
  bool enable = emberUnsignedCommandArgument(0);
  emberAfPluginLowVoltageShutdownEnable(enable);
}

void emberAfPluginLowVoltageShutdownStatusCommand(void)
{
  emberAfAppPrint("Low voltage shutdown logic is ");
  if (emberAfPluginLowVoltageShutdownEnabled()) {
    emberAfAppPrintln("enabled.");
  } else {
    emberAfAppPrintln("disabled.");
  }
}

void emberAfPluginLowVoltageShutdownPrintVddCommand()
{
  emberAfAppPrintln("VDD voltage is %d mV",
                    emberAfPluginLowVoltageShutdownGetVoltage());
}
