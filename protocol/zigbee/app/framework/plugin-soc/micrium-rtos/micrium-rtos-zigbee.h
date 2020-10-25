/***************************************************************************//**
 * @file
 * @brief Definitions for the Micrium RTOS plugin.
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
#ifndef SILABS_MICRIUM_RTOS_ZIGBEE_H
#define SILABS_MICRIUM_RTOS_ZIGBEE_H

//------------------------------------------------------------------------------
// Event flags and semaphores

// Event flag and event flag group for the zigbee task yield management
#define ZIGBEE_TASK_YIELD (OS_FLAGS)0x0001
extern OS_FLAG_GRP zigbeeTaskEventFlags;

#endif //SILABS_MICRIUM_RTOS_ZIGBEE_H
