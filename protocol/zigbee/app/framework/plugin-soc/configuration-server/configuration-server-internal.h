/***************************************************************************//**
 * @file
 * @brief Internal definitions for the Configuration Server plugin.
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

#ifndef SILABS_CONFIGURATION_SERVER_INTERNAL_H
#define SILABS_CONFIGURATION_SERVER_INTERNAL_H

/** @brief Reads data from the tokens using the creator.
 *
 * Local method for reading the configurable tokens from their respective
 * creators. Note:  as this is used by the cluster command, data needs to be
 * in the format required for the local command.
 *
 */
void emAfPluginConfigurationServerReadTokenDataFromCreator(uint16_t creator, uint8_t *data);

#endif
