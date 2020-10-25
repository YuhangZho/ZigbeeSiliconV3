/***************************************************************************//**
 * @file
 * @brief CLI commands for forming/joining using the hardware buttons.
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

void emberAfPluginButtonJoiningButton0Command(void);
void emberAfPluginButtonJoiningButton1Command(void);

extern void emberAfPluginButtonJoiningPressButton(uint8_t button);

#if !defined(EMBER_AF_GENERATE_CLI)
EmberCommandEntry emberAfPluginButtonJoiningCommands[] = {
  emberCommandEntryAction("button0",
                          emberAfPluginButtonJoiningButton0Command,
                          "",
                          "Press button 0"),
  emberCommandEntryAction("button1",
                          emberAfPluginButtonJoiningButton1Command,
                          "",
                          "Press button 1"),
  emberCommandEntryTerminator(),
};
#endif // EMBER_AF_GENERATE_CLI

void emberAfPluginButtonJoiningButton0Command(void)
{
  emberAfPluginButtonJoiningPressButton(BUTTON0);
}

void emberAfPluginButtonJoiningButton1Command(void)
{
  emberAfPluginButtonJoiningPressButton(BUTTON1);
}
