/***************************************************************************//**
 * @file
 * @brief Definitions for the Gateway Relay MQTT plugin.
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

#ifndef SILABS_CHANNEL_MQTT_H
#define SILABS_CHANNEL_MQTT_H

/** @brief Sets the heart beat interval.
 *
 * Sets the MQTT heart beat interval in milliseconds.
 *
 * @param intervalMs The heart beat interval in milliseconds.
 */
void emberPluginGatewayRelayMqttSetHeartBeat(uint16_t intervalMs);

#endif
