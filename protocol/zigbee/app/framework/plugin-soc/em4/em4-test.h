/***************************************************************************//**
 * @file
 * @brief Include file for em4 unit tests
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

#ifndef SILABS_EM4_TEST_H
#define SILABS_EM4_TEST_H

#define EMBER_ENABLE_EM4

#define EMBER_AF_PLUGIN_IDLE_SLEEP_MINIMUM_SLEEP_DURATION_MS 0
#define EMBER_AF_PLUGIN_IDLE_SLEEP_BACKOFF_SLEEP_MS 0

#undef halPowerDown
#define halPowerDown(...)

#undef halPowerUp
#define halPowerUp(...)

#undef halSleepForMilliseconds
#define halSleepForMilliseconds(...)

#undef halBeforeEM4
#define halBeforeEM4(data1, data2)

#endif //SILABS_EM4_TEST_H
