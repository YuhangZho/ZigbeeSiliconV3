/***************************************************************************//**
 * @file
 * @brief Definitions for the Configuration Server plugin.
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

#ifndef SILABS_CONFIGURATION_SERVER_H
#define SILABS_CONFIGURATION_SERVER_H

/** @brief Returns true if the OTA configuration server is locked.
 *
 * Some custom clusters, such as configuration server and
 * manufacturing library server, should be locked.
 * This function indicates to the cluster plugins
 * that the functionality is locked down.
 *
 * @return A ::bool value that is true if the configuration server is locked.
 *
 */
bool emberAfPluginConfigurationServerLocked(void);

#if defined(EMBER_AF_PLUGIN_LED_RGB_PWM) || defined(EMBER_AF_PLUGIN_LED_TEMP_PWM) || defined(EMBER_AF_PLUGIN_LED_DIM_PWM)
/** @brief Returns the minimum on time, in microseconds, supported by the
 *  power supply.
 *
 * Different bulb power supplies have requirements for minimum and maximum on
 * time during the PWM cycle. If the PWM is not enabled long enough, this can
 * cause damage.
 *
 * @return A ::uint16_t value that is the minimum number of microseconds the
 * PWM must be on during a PWM cycle if it is not completely off.
 *
 */
uint16_t emberAfPluginConfigurationServerReadMinOnTimeUs(void);

/** @brief Returns the maximum on time, in microseconds, supported by the
 *  power supply.
 *
 * Different bulb power supplies have requirements for minimum and maximum on
 * time during the PWM cycle. Some power supplies require the supply not be
 * enabled for the full cycle in a 100% on condition.
 *
 * @return A ::uint16_t value that is the maximum number of microseconds the
 * PWM must be on during a PWM cycle.
 *
 */
uint16_t emberAfPluginConfigurationServerReadMaxOnTimeUs(void);
#endif //(EMBER_AF_PLUGIN_LED_RGB_PWM) || defined(EMBER_AF_PLUGIN_LED_TEMP_PWM) || defined(EMBER_AF_PLUGIN_LED_DIM_PWM)
#endif //__CONFIGURATION_SERVER_H__
