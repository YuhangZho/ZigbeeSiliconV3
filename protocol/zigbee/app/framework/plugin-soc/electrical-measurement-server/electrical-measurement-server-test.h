/***************************************************************************//**
 * @file
 * @brief Test definitions for the Electrical Measurement Server plugin.
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
#define EMBER_AF_PLUGIN_ELECTRICAL_MEASUREMENT_SERVER_MAX_MEASUREMENT_FREQUENCY_S 30
#define EMBER_AF_PLUGIN_ELECTRICAL_MEASUREMENT_SERVER_MAX_VOLTAGE_RATING_V 260
#define EMBER_AF_PLUGIN_ELECTRICAL_MEASUREMENT_SERVER_MAX_CURRENT_RATING_A  16
#define EMBER_AF_PLUGIN_ELECTRICAL_MEASUREMENT_SERVER_MAX_POWER_RATING_W  4000
#define EMBER_AF_PLUGIN_ELECTRICAL_MEASUREMENT_SERVER_DEFAULT_REPORTABLE_RMS_VOLTAGE_CHANGE_M_C 500
#define EMBER_AF_PLUGIN_ELECTRICAL_MEASUREMENT_SERVER_DEFAULT_REPORTABLE_RMS_CURRENT_CHANGE_M_C 500
#define EMBER_AF_PLUGIN_ELECTRICAL_MEASUREMENT_SERVER_DEFAULT_REPORTABLE_ACTIVE_POWER_CHANGE_M_C
