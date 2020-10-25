/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

// This callback file is created for your convenience. You may add application code
// to this file. If you regenerate this file over a previous version, the previous
// version will be overwritten and any code you have added will be lost.

#include "app/framework/include/af.h"

/** @brief Trust Center Join
 *
 * This callback is called from within the application framework's
 * implementation of emberTrustCenterJoinHandler or
 * ezspTrustCenterJoinHandler. This callback provides the same arguments
 * passed to the TrustCenterJoinHandler. For more information about the
 * TrustCenterJoinHandler please see documentation included in
 * stack/include/trust-center.h.
 *
 * @param newNodeId   Ver.: always
 * @param newNodeEui64   Ver.: always
 * @param parentOfNewNode   Ver.: always
 * @param status   Ver.: always
 * @param decision   Ver.: always
 */
void emberAfTrustCenterJoinCallback(EmberNodeId newNodeId,
                                    EmberEUI64 newNodeEui64,
                                    EmberNodeId parentOfNewNode,
                                    EmberDeviceUpdate status,
                                    EmberJoinDecision decision)
{
  emberAfRegistrationPrintln("New node joined, shortID=0x%2x", newNodeId);
}

/** @brief Registration
 *
 * This callback is called when the device joins a network and the process of
 * registration is complete. This callback provides a success value of true if
 * the registration process was successful and a value of false if
 * registration failed.
 *
 * @param success true if registration succeeded, false otherwise.  Ver.:
 * always
 */
void emberAfRegistrationCallback(bool success)
{
  emberAfRegistrationPrintln("Registration %p", success ? "complete" : "failed");
}

EmberCommandEntry emberAfCustomCommands[] = {
  /* Sample Custom CLI commands */
  // emberCommandEntrySubMenu("sub-menu", customSubMenu, "Sub menu of custom commands"),
  emberCommandEntryTerminator()
};
