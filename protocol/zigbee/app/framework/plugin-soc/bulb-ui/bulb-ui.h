/***************************************************************************//**
 * @file
 * @brief Definitions for the Bulb UI plugin.
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

#ifndef SILABS_BULB_UI_H
#define SILABS_BULB_UI_H

/** @brief Starts the search for joinable networks.
 *
 * Starts the bulb-UI search for joinable networks.  This
 * is not normally required, as the bulb-UI will do this automatically.
 * However, some plugins that interrupt the normal bulb-UI behavior,
 * such as the manufacturing library cluster server, may need to kick off
 * a network search when their task is complete.
 *
 */
void emberAfPluginBulbUiInitiateNetworkSearch(void);

#endif
