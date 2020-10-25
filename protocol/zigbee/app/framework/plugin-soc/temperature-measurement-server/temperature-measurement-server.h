/***************************************************************************//**
 * @file
 * @brief Definitions for the Temperature Measurement Server plugin.
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

#ifndef SILABS_TEMPERATURE_MEASUREMENT_SERVER_H
#define SILABS_TEMPERATURE_MEASUREMENT_SERVER_H

#define EMBER_TEMPERATURE_MEASUREMENT_SERVER_OVER_TEMPERATURE_NORMAL      0
#define EMBER_TEMPERATURE_MEASUREMENT_SERVER_OVER_TEMPERATURE_WARNING     1
#define EMBER_TEMPERATURE_MEASUREMENT_SERVER_OVER_TEMPERATURE_CRITICAL    2
//------------------------------------------------------------------------------
// Plugin public function declarations.

/** @brief Sets the hardware read interval.
 *
 * Sets the amount of time to wait (in seconds) between polls
 * of the temperature sensor. This function will never set the measurement
 * interval to be greater than the plugin specified maximum measurement
 * interval. If a value of 0 is given, the plugin specified maximum measurement
 * interval will be used for the polling interval.
 */
void emberAfPluginTemperatureMeasurementServerSetMeasurementInterval(
  uint32_t measurementRateS);

#endif //__TEMPERATURE_MEASUREMENT_SERVER_H__
