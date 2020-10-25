/***************************************************************************//**
 * @file
 * @brief Definitions for the LV Shutdown plugin.
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

// Obtain a single reading of the VDD voltage (used to check for shutdown)
// Result is in millivolts.
uint16_t emberAfPluginLowVoltageShutdownGetVoltage(void);

// Enable or disable shutdown logic at runtime for testing purposes
void emberAfPluginLowVoltageShutdownEnable(bool enable);

// Check whether the shutdown logic has is enabled at runtime (as it can be
// artificially disabled)
bool emberAfPluginLowVoltageShutdownEnabled(void);

// Force a shutdown event (regardless of VDD) for testing
void emberAfPluginLowVoltageShutdownForceShutdown(void);
