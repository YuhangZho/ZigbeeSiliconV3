/***************************************************************************//**
 * @file
 * @brief Code to use a HAL battery monitor plugin to act as a server for the power
 * configuration cluster.
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

#include "app/framework/include/af.h"

#define MILLI_TO_DECI_CONVERSION_FACTOR 100

//------------------------------------------------------------------------------
// Implemented callbacks

void emberAfPluginBatteryMonitorDataCallback(uint16_t voltageMilliV)
{
  EmberAfStatus afStatus;
  uint8_t voltageDeciV;
  uint8_t i;
  uint8_t endpoint;

  emberAfAppPrintln("New voltage reading: %d mV", voltageMilliV);

  // convert from mV to 100 mV, which are the units specified by zigbee spec for
  // the power configuration cluster's voltage attribute.
  voltageDeciV = ((uint8_t) (voltageMilliV / MILLI_TO_DECI_CONVERSION_FACTOR));

  // Cycle through all endpoints, check to see if the endpoint has a power
  // configuration server, and if so update the voltage attribute
  for (i = 0; i < emberAfEndpointCount(); i++) {
    endpoint = emberAfEndpointFromIndex(i);
    if (emberAfContainsServer(endpoint, ZCL_POWER_CONFIG_CLUSTER_ID)) {
      afStatus = emberAfWriteServerAttribute(endpoint,
                                             ZCL_POWER_CONFIG_CLUSTER_ID,
                                             ZCL_BATTERY_VOLTAGE_ATTRIBUTE_ID,
                                             &voltageDeciV,
                                             ZCL_INT8U_ATTRIBUTE_TYPE);

      if (EMBER_ZCL_STATUS_SUCCESS != afStatus) {
        emberAfAppPrintln("Power Configuration Server: failed to write value "
                          "0x%x to cluster 0x%x attribute ID 0x%x: error 0x%x",
                          voltageDeciV,
                          ZCL_POWER_CONFIG_CLUSTER_ID,
                          ZCL_BATTERY_VOLTAGE_ATTRIBUTE_ID,
                          afStatus);
      }
    }
  }
}
