/***************************************************************************//**
 * @file
 * @brief Heartbeat task configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef HEARTBEAT_LED_CONFIG_H
#define HEARTBEAT_LED_CONFIG_H
// <o HEARTBEAT_LED_INSTANCE> Led instance name (led must be instanciated in the project)
// <led0=> LED 0
// <led1=> LED 1
// <led2=> LED 2
// <i> Default: led0
#define HEARTBEAT_LED_INSTANCE        led0

// <o HEARTBEAT_TOOGLE_DELAY_MS> Delay between led toogle in millisecond
// <i> Default: 500
#define HEARTBEAT_TOOGLE_DELAY_MS     500

#endif
