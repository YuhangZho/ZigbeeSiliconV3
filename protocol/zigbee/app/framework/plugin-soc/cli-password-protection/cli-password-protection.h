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
#ifndef CLI_PASSWORD_PROTECTION_H
#define CLI_PASSWORD_PROTECTION_H

typedef struct {
  uint8_t password[EMBER_ENCRYPTION_KEY_SIZE];
} passwordTokType;

#endif
