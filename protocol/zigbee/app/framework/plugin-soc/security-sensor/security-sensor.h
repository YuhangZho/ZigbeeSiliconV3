/***************************************************************************//**
 * @file
 * @brief Definitions for the Security Sensor plugin.
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

#ifndef SILABS_SECURITY_SENSOR_H
#define SILABS_SECURITY_SENSOR_H

// Status bit definitions used when generating report to IAS Zone Server
#define EMBER_SECURITY_SENSOR_STATUS_ALARM     0x0001
#define EMBER_SECURITY_SENSOR_STATUS_NO_ALARM  0x0000
#define EMBER_SECURITY_SENSOR_STATUS_TAMPER    0x0004
#define EMBER_SECURITY_SENSOR_STATUS_NO_TAMPER 0x0000

#endif //SILABS_SECURITY_SENSOR_H
