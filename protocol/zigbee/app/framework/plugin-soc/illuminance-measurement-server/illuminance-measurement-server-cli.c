/***************************************************************************//**
 * @file
 * @brief CLI for the Illuminance Measurement Server plugin.
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
#include "illuminance-measurement-server.h"
#include EMBER_AF_API_ILLUMINANCE

#define ILLUMINANCE_MEASUREMENT_SERVER_CALIBRATION_LUX_MAX   2000
#define ILLUMINANCE_MEASUREMENT_SERVER_CALIBRATION_LUX_MIN   50

//******************************************************************************
// CLI function to calibrate the illuminance sensor with
// its overlaid materials.
//******************************************************************************
void emAfIlluminanceMeasurementServerCalibrateCommand(void)
{
  uint8_t multiplier;
  uint32_t extLux = (uint32_t)emberUnsignedCommandArgument(0);

  if ((extLux < ILLUMINANCE_MEASUREMENT_SERVER_CALIBRATION_LUX_MIN)
      || (extLux > ILLUMINANCE_MEASUREMENT_SERVER_CALIBRATION_LUX_MAX)) {
    emberAfAppPrintln("Error in calibration, value must be between %d and %d",
                      ILLUMINANCE_MEASUREMENT_SERVER_CALIBRATION_LUX_MIN,
                      ILLUMINANCE_MEASUREMENT_SERVER_CALIBRATION_LUX_MAX);
  } else {
    multiplier = halIlluminanceCalibrate(extLux);
    halCommonSetToken(TOKEN_SI1141_MULTIPLIER, &multiplier);
    emberAfAppPrintln("Multiplier Set: %d", multiplier);
  }
}
