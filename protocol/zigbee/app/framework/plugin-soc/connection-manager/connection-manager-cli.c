/***************************************************************************//**
 * @file
 * @brief Command line interface functions for the connection manager plugin
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
#include "connection-manager.h"

void emAfPluginConnectionManagerSetForceShortPollCommand(void)
{
  bool forceShortPoll = (bool)emberUnsignedCommandArgument(0);
  halCommonSetToken(TOKEN_FORCE_SHORT_POLL, &forceShortPoll);
}
