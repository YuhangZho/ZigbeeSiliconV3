/***************************************************************************//**
 * @file
 * @brief Tokens for the Bulb UI plugin.
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

#define CREATOR_REBOOT_MONITOR  (0x000c)
#define NVM3KEY_REBOOT_MONITOR  (NVM3KEY_DOMAIN_USER | 0x000c)

#define CREATOR_SHORT_REBOOT_MONITOR (0x000e)
#define NVM3KEY_SHORT_REBOOT_MONITOR (NVM3KEY_DOMAIN_USER | 0x000e)

#ifdef DEFINETYPES
// Include or define any typedef for tokens here
#endif //DEFINETYPES
#ifdef DEFINETOKENS
// Define the actual token storage information here

DEFINE_COUNTER_TOKEN(REBOOT_MONITOR, tokTypeStackBootCounter, 0)
DEFINE_COUNTER_TOKEN(SHORT_REBOOT_MONITOR, tokTypeStackBootCounter, 0)

#endif //DEFINETOKENS
