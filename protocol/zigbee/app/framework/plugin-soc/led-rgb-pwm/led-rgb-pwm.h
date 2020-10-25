/***************************************************************************//**
 * @file
 * @brief Definitions for the LED RGB PWM plugin.
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

#ifndef SILABS_LED_RGB_PWM_H
#define SILABS_LED_RGB_PWM_H

void emberAfLedRgbPwmComputeRgbFromXy(uint8_t endpoint);
void emberAfLedRgbPwmComputeRgbFromColorTemp(uint8_t endpoint);
void emberAfLedRgbComputeRgbFromHSV(uint8_t endpoint);

#endif
