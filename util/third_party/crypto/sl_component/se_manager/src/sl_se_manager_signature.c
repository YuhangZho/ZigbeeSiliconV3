/***************************************************************************//**
 * @file
 * @brief Silicon Labs Secure Element Manager API.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: APACHE-2.0
 *
 * This software is subject to an open source license and is distributed by
 * Silicon Laboratories Inc. pursuant to the terms of the Apache License,
 * Version 2.0 available at https://www.apache.org/licenses/LICENSE-2.0.
 * Such terms and conditions may be further supplemented by the Silicon Labs
 * Master Software License Agreement (MSLA) available at www.silabs.com and its
 * sections applicable to open source software.
 *
 ******************************************************************************/
#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)

#include "sl_se_manager.h"
#include "sli_se_manager_internal.h"
#include "em_se.h"
#include "em_core.h"
#include "em_assert.h"
#include "em_system.h"
#include <string.h>

/// @addtogroup sl_se_manager
/// @{

// -----------------------------------------------------------------------------
// Global Functions

/***************************************************************************//**
 * ECC signature generation.
 ******************************************************************************/
sl_status_t sl_se_ecc_sign(sl_se_command_context_t *cmd_ctx,
                           const sl_se_key_descriptor_t *key,
                           sl_se_hash_type_t hash_alg,
                           bool hashed_message,
                           const unsigned char *message,
                           size_t message_len,
                           unsigned char *signature,
                           size_t signature_len)
{
  if (cmd_ctx == NULL || key == NULL || message == NULL || signature == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  sl_status_t status;
  uint32_t command_word = SLI_SE_COMMAND_SIGNATURE_SIGN;

  if (hashed_message == false) {
    switch (hash_alg) {
      case SL_SE_HASH_SHA1:
        command_word |= SLI_SE_COMMAND_OPTION_HASH_SHA1;
        break;

      case SL_SE_HASH_SHA224:
        command_word |= SLI_SE_COMMAND_OPTION_HASH_SHA224;
        break;

      case SL_SE_HASH_SHA256:
        command_word |= SLI_SE_COMMAND_OPTION_HASH_SHA256;
        break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
      case SL_SE_HASH_SHA384:
        command_word |= SLI_SE_COMMAND_OPTION_HASH_SHA384;
        break;

      case SL_SE_HASH_SHA512:
        command_word |= SLI_SE_COMMAND_OPTION_HASH_SHA512;
        break;
#endif
      default:
        return SL_STATUS_INVALID_PARAMETER;
    }
  }

  // Setup SE command and parameters
  sli_se_command_init(cmd_ctx, command_word);
  // Add key parameters to command
  sli_add_key_parameters(cmd_ctx, key, status);
  // Message size (number of bytes)
  SE_addParameter(se_cmd, message_len);
  // Add key metadata block to command
  sli_add_key_metadata(cmd_ctx, key, status);
  // Add key input block to command
  sli_add_key_input(cmd_ctx, key, status);

  SE_DataTransfer_t message_buffer = SE_DATATRANSFER_DEFAULT(message, message_len);
  SE_addDataInput(se_cmd, &message_buffer);

  #if defined(SL_SE_KEY_TYPE_ECC_EDDSA)
    // EdDSA requires the message twice
    SE_DataTransfer_t repeated_message_buffer;
    if (key->type == SL_SE_KEY_TYPE_ECC_EDDSA) {
      repeated_message_buffer.next = (void*)SE_DATATRANSFER_STOP;
      repeated_message_buffer.data = (void*)message;
      repeated_message_buffer.length = message_len;
      SE_addDataInput(se_cmd, (SE_DataTransfer_t*)&repeated_message_buffer);
    }
  #endif

  SE_DataTransfer_t signature_buffer = SE_DATATRANSFER_DEFAULT(signature, signature_len);
  SE_addDataOutput(se_cmd, &signature_buffer);

  return sli_se_execute_and_wait(cmd_ctx);
}

/***************************************************************************//**
 * ECC signature verification.
 ******************************************************************************/
sl_status_t sl_se_ecc_verify(sl_se_command_context_t *cmd_ctx,
                             const sl_se_key_descriptor_t *key,
                             sl_se_hash_type_t hash_alg,
                             bool hashed_message,
                             const unsigned char *message,
                             size_t message_len,
                             const unsigned char *signature,
                             size_t signature_len)
{
  if (cmd_ctx == NULL || key == NULL || message == NULL || signature == NULL) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  SE_Command_t *se_cmd = &cmd_ctx->command;
  sl_status_t status;
  uint32_t command_word = SLI_SE_COMMAND_SIGNATURE_VERIFY;

  if (hashed_message == false) {
    switch (hash_alg) {
      case SL_SE_HASH_SHA1:
        command_word |= SLI_SE_COMMAND_OPTION_HASH_SHA1;
        break;

      case SL_SE_HASH_SHA224:
        command_word |= SLI_SE_COMMAND_OPTION_HASH_SHA224;
        break;

      case SL_SE_HASH_SHA256:
        command_word |= SLI_SE_COMMAND_OPTION_HASH_SHA256;
        break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
      case SL_SE_HASH_SHA384:
        command_word |= SLI_SE_COMMAND_OPTION_HASH_SHA384;
        break;

      case SL_SE_HASH_SHA512:
        command_word |= SLI_SE_COMMAND_OPTION_HASH_SHA512;
        break;

#endif
      default:
        return SL_STATUS_INVALID_PARAMETER;
    }
  }

  // Setup SE command and parameters
  sli_se_command_init(cmd_ctx, command_word);
  // Add key parameters to command
  sli_add_key_parameters(cmd_ctx, key, status);
  // Message size (number of bytes)
  SE_addParameter(se_cmd, message_len);
  // Add key metadata block to command
  sli_add_key_metadata(cmd_ctx, key, status);
  // Add key input block to command
  sli_add_key_input(cmd_ctx, key, status);

  SE_DataTransfer_t message_buffer = SE_DATATRANSFER_DEFAULT(message,
                                                             message_len);
  SE_DataTransfer_t signature_buffer = SE_DATATRANSFER_DEFAULT(signature,
                                                               signature_len);

  #if defined(SL_SE_KEY_TYPE_ECC_EDDSA)
    if (key->type == SL_SE_KEY_TYPE_ECC_EDDSA) {
      SE_addDataInput(se_cmd, &signature_buffer);
      SE_addDataInput(se_cmd, &message_buffer);
    } else
  #endif
  {
    SE_addDataInput(se_cmd, &message_buffer);
    SE_addDataInput(se_cmd, &signature_buffer);
  }

  return sli_se_execute_and_wait(cmd_ctx);
}

/** @} (end addtogroup sl_se) */

#endif // defined(SEMAILBOX_PRESENT)
