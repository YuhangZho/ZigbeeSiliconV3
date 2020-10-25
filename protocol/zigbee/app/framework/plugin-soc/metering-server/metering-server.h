/***************************************************************************//**
 * @file
 * @brief Definitions for the Metering Server plugin.
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

#ifndef METERING_SERVER_H_
#define METERING_SERVER_H_

/** @brief Sets the fast polling mode.
 *
 * Enables and disables fast polling mode.
 *
 * @param enableFastPolling False: disable, True: enable.
 */
void emberAfPluginMeteringServerEnableFastPolling(bool enableFastPolling);

#endif /* METERING_SERVER_H_ */
