/***************************************************************************//**
 * @file
 * @brief Implements the color control server for color temperature bulbs.
 * Note: This is HA certifiable and has passed HA certification for at least
 * one customer project.
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

// Nothing here for now..
#ifndef SILABS_LED_TEMP_PWM_H
#define SILABS_LED_TEMP_PWM_H

enum {
  EMBER_ZCL_COLOR_TEMP_MOVE_MODE_STOP = 0x00,
  EMBER_ZCL_COLOR_TEMP_MOVE_MODE_UP   = 0x01,
  EMBER_ZCL_COLOR_TEMP_MOVE_MODE_DOWN = 0x03,
};

#endif
