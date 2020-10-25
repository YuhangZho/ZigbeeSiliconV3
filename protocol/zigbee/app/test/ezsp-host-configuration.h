/***************************************************************************//**
 * @file
 * @brief
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

// *******************************************************************
// Stack Profile Parameters
//

#define EMBER_STACK_PROFILE            0
#define EMBER_INDIRECT_TRANSMISSION_TIMEOUT 25000

// *******************************************************************
// Ember static memory requirements
//
// There are constants that define the amount of static memory
// required by the stack. If the application does not set them then
// the default values from ember-configuration-defaults.h are used.
//
// for example, this changes the default number of buffers to 8
// #define EMBER_PACKET_BUFFER_COUNT 8

#define EMBER_PACKET_BUFFER_COUNT 6

// *******************************************************************
// Application Handlers
//
// By default, a number of stub handlers are automatically provided
// that have no effect.  If the application would like to implement any
// of these handlers itself, it needs to define the appropriate macro

#define EZSP_APPLICATION_HAS_ENERGY_SCAN_RESULT_HANDLER
#define EZSP_APPLICATION_HAS_WAITING_FOR_RESPONSE
#define EZSP_APPLICATION_HAS_MFGLIB_HANDLER
