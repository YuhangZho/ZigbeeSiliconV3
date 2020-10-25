/***************************************************************************//**
 * @file app_se_firmware_image.h
 * @brief The (V)SE firmware image data.
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
#ifndef APP_SE_FIRMWARE_IMAGE_H
#define APP_SE_FIRMWARE_IMAGE_H

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_se_manager_se_firmware_upgrade.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// The start address of the (V)SE firmware image
#define SE_FIRMWARE_ADDR        (0x00040000)

/// The size of the (V)SE firmware image
#if defined(CRYPTOACC_PRESENT)
#define SE_FIRMWARE_SIZE        (16550UL)
#else
#define SE_FIRMWARE_SIZE        (41207UL)
#endif

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Get start address of the (V)SE firmware image.
 *
 * @returns Returns start address of the (V)SE firmware image.
 ******************************************************************************/
const uint8_t * get_se_firmware_addr(void);

/***************************************************************************//**
 * Get size of the (V)SE firmware image.
 *
 * @returns Returns size of the (V)SE firmware image.
 ******************************************************************************/
uint32_t get_se_firmware_size(void);

#endif  // APP_SE_FIRMWARE_IMAGE_H
