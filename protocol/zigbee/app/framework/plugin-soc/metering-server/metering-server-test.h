/***************************************************************************//**
 * @file
 * @brief Test definitions for the Metering Server plugin.
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

#define EMBER_AF_API_POWER_METER "../../util/plugin/plugin-afv2/power-meter-cs5463/power-meter.h"
#define TEST_METERING_SERVER_METER_STATUS_ERROR_NONE                    0
#define TEST_METERING_SUMMATION_FORMAT                               0x33
#define TEST_METERING_MULTIPLIER                                        1
#define TEST_METERING_DIVISOR                                     3600000
#define EMBER_AF_PLUGIN_METERING_SERVER_MAX_SAMPLES_PER_SESSION         5
#define EMBER_AF_SIMPLE_METERING_CLUSTER_SERVER_ENDPOINT_COUNT          1
#define EMBER_AF_PLUGIN_METERING_SERVER_MAX_SAMPLING_SESSIONS           5
#define EMBER_AF_PLUGIN_METERING_SERVER_METER_PROFILES                  5
