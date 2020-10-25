/***************************************************************************//**
 * @file
 * @brief CLI password protection plugin.
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
#include "app/framework/util/af-main.h"

#ifdef ZA_CLI_FULL
//in command-interpreter2
extern void emCliPasswordProtectionEventHandler(void);
#endif

EmberEventControl emberAfPluginCliPasswordProtectionTimingEventControl;

void emberAfPluginCliPasswordProtectionTimingEventHandler(void)
{
  #ifdef ZA_CLI_FULL
  emCliPasswordProtectionEventHandler();
  #endif
}
