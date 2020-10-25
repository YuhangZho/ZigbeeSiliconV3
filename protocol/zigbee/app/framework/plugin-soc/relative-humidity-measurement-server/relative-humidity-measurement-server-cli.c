/***************************************************************************//**
 * @file
 * @brief CLI for the Relative Humidity Measurement Server plugin.
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
#include EMBER_AF_API_HUMIDITY

//******************************************************************************
// CLI function to manually generate a humidity read
//******************************************************************************
void emAfRelativeHumidityMeasurementServerReadCommand(void)
{
  // No result will be printed here, as the callback that generates the value
  // is already implemented in relative-humidity-measurement-server.c.
  // As such, the callback will generate the diagnostic print.
  halHumidityStartRead();
}
