/***************************************************************************//**
 * @file
 * @brief Scan for VoBLE devices header file
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

#ifndef SCAN_H_
#define SCAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "ble-callbacks.h"

/***********************************************************************************************//**
 * \defgroup scan Scan Code
 * \brief Scan for VoBLE devices implementation
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup Scan
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup scan
 * @{
 **************************************************************************************************/

/***************************************************************************************************
 * Public Function Declarations
 **************************************************************************************************/

/***********************************************************************************************//**
 *  \brief  Check if device with VoBLE service found.
 *  \return  true if device found, otherwise false
 **************************************************************************************************/
bool SCAN_Is_Device_Found(void);

/***********************************************************************************************//**
 *  \brief  Twirled scan indicator.
 *  \return  None.
 **************************************************************************************************/
void SCAN_Indication(void);

/** @} (end addtogroup scan) */
/** @} (end addtogroup Scan) */

#ifdef __cplusplus
};
#endif

#endif /* SCAN_H_ */
