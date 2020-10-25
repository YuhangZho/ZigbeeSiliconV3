/***************************************************************************//**
 * @file
 * @brief Health Thermometer GATT service configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_HEALTH_THERMOMETER_CONFIG_H
#define SL_HEALTH_THERMOMETER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <o SL_BT_HT_MEASUREMENT_INTERVAL_SEC> Health Thermometer Measurement Interval (UUID: 2A21) <1-65535>
// <i> Default: 1
// <i> The Measurement Interval is the time interval between two Temperature Measurement indications.
#define SL_BT_HT_MEASUREMENT_INTERVAL_SEC   1

// <o SL_BT_HT_TEMPERATURE_TYPE> Health Thermometer Temperature Type (UUID: 2A1D)
// <SL_BT_HT_TEMPERATURE_TYPE_ARMPIT=> Armpit
// <SL_BT_HT_TEMPERATURE_TYPE_BODY=> Body (general)
// <SL_BT_HT_TEMPERATURE_TYPE_EAR=> Ear (usually ear lobe)
// <SL_BT_HT_TEMPERATURE_TYPE_FINGER=> Finger
// <SL_BT_HT_TEMPERATURE_TYPE_GASTRO_INTESTINAL_TRACT=> Gastro-intestinal Tract
// <SL_BT_HT_TEMPERATURE_TYPE_MOUTH=> Mouth
// <SL_BT_HT_TEMPERATURE_TYPE_RECTUM=> Rectum
// <SL_BT_HT_TEMPERATURE_TYPE_TOE=> Toe
// <SL_BT_HT_TEMPERATURE_TYPE_TYMPANUM=> Tympanum (ear drum)
// <i> Default: SL_BT_HT_TEMPERATURE_TYPE_BODY
#define SL_BT_HT_TEMPERATURE_TYPE           SL_BT_HT_TEMPERATURE_TYPE_BODY

// <<< end of configuration section >>>

#endif // SL_HEALTH_THERMOMETER_CONFIG_H
