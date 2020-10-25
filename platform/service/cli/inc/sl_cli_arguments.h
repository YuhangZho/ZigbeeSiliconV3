/***************************************************************************//**
 * @file
 * @brief Types for argument parsing in the CLI framework.
 * @version x.y.z
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef SL_CLI_ARGUMENTS_H
#define SL_CLI_ARGUMENTS_H

#include <stdint.h>
#include <stdbool.h>

#include "sl_cli_config.h"
#include "sl_cli_types.h"

/***************************************************************************//**
 * @addtogroup cli
 * @{
 ******************************************************************************/

// Argument types
// Note: The offset between mandatory and optional must be 0x10.
/// @brief UINT8 argument type
#define SL_CLI_ARG_UINT8      (0x00U)
/// @brief UINT16 argument type
#define SL_CLI_ARG_UINT16     (0x01U)
/// @brief UINT32 argument type
#define SL_CLI_ARG_UINT32     (0x02U)
/// @brief INT8 argument type
#define SL_CLI_ARG_INT8       (0x03U)
/// @brief INT16 argument type
#define SL_CLI_ARG_INT16      (0x04U)
/// @brief INT32 argument type
#define SL_CLI_ARG_INT32      (0x05U)
/// @brief STRING argument type
#define SL_CLI_ARG_STRING     (0x06U)
/// @brief HEX argument type
#define SL_CLI_ARG_HEX        (0x07U)
/// @brief Optional UINT8 argument type
#define SL_CLI_ARG_UINT8OPT   (0x10U)
/// @brief Optional UINT16 argument type
#define SL_CLI_ARG_UINT16OPT  (0x11U)
/// @brief Optional UINT32 argument type
#define SL_CLI_ARG_UINT32OPT  (0x12U)
/// @brief Optional INT8 argument type
#define SL_CLI_ARG_INT8OPT    (0x13U)
/// @brief Optional INT16 argument type
#define SL_CLI_ARG_INT16OPT   (0x14U)
/// @brief Optional INT32 argument type
#define SL_CLI_ARG_INT32OPT   (0x15U)
/// @brief Optional STRING argument type
#define SL_CLI_ARG_STRINGOPT  (0x16U)
/// @brief Optional HEX argument type
#define SL_CLI_ARG_HEXOPT     (0x17U)
/// @brief ADDITIONAL argument type
#define SL_CLI_ARG_ADDITIONAL (0x20U)
/// @brief WILDCARD argument type
#define SL_CLI_ARG_WILDCARD   (0x21U)
/// @brief GROUP argument type
#define SL_CLI_ARG_GROUP      (0xFEU)
/// @brief END argument type
#define SL_CLI_ARG_END        (0xFFU)

//****************************************************************************

/** @} (end addtogroup cli) */

#endif // SL_CLI_ARGUMENTS_H
