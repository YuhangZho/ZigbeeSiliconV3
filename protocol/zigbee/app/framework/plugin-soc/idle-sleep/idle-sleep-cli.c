/***************************************************************************//**
 * @file
 * @brief CLI for the Idle Sleep plugin.
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
#include "idle-sleep.h"

#ifndef EMBER_AF_GENERATE_CLI
  #error The Idle/Sleep plugin is not compatible with the legacy CLI.
#endif

void emberAfPluginIdleSleepStatusCommand(void)
{
  emberAfCorePrintln("Stay awake when not joined: %p",
                     (emAfStayAwakeWhenNotJoined ? "yes" : "no"));
  emberAfCorePrintln("Forced stay awake:          %p",
                     (emAfForceEndDeviceToStayAwake ? "yes" : "no"));
}

void emberAfPluginIdleSleepStayAwakeCommand(void)
{
  bool stayAwake = (bool)emberUnsignedCommandArgument(0);
  if (stayAwake) {
    emberAfCorePrintln("Forcing device to stay awake");
  } else {
    emberAfCorePrintln("Allowing device to go to sleep");
  }
  emberAfForceEndDeviceToStayAwake(stayAwake);
}

void emberAfPluginIdleSleepAwakeWhenNotJoinedCommand(void)
{
  emAfStayAwakeWhenNotJoined = (bool)emberUnsignedCommandArgument(0);
}
