/***************************************************************************//**
 * @file
 * @brief Silicon Labs Secure Element Manager Internal key defines
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
#ifndef SL_SE_MANAGER_INTERNAL_KEYS
#define SL_SE_MANAGER_INTERNAL_KEYS

#include "em_device.h"
#include "sl_se_manager_defines.h"

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)

/// Key descriptor for internal application attestation key
  #define SL_SE_APPLICATION_ATTESTATION_KEY                       \
    {                                                             \
      .type = SL_SE_KEY_TYPE_ECC_P256,                            \
      .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE                      \
               | SL_SE_KEY_FLAG_IS_DEVICE_GENERATED               \
               | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY \
               | SL_SE_KEY_FLAG_ASYMMETRIC_SIGNING_ONLY,          \
      .storage = {                                                \
        .method = SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE,           \
        .location = {                                             \
          .slot = SL_SE_KEY_SLOT_APPLICATION_ATTESTATION_KEY,     \
        },                                                        \
      },                                                          \
    }

/// Key descriptor for internal SE attestation key
/// @note: Can only be used to get the public part
  #define SL_SE_SYSTEM_ATTESTATION_KEY                            \
    {                                                             \
      .type = SL_SE_KEY_TYPE_ECC_P256,                            \
      .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE                      \
               | SL_SE_KEY_FLAG_IS_DEVICE_GENERATED               \
               | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY \
               | SL_SE_KEY_FLAG_ASYMMETRIC_SIGNING_ONLY,          \
      .storage = {                                                \
        .method = SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE,           \
        .location = {                                             \
          .slot = 0xFFUL,                                         \
        },                                                        \
      },                                                          \
    }

#endif // _SILICON_LABS_SECURITY_FEATURE_VAULT

/// Key descriptor for secure boot public key
#define SL_SE_APPLICATION_SECURE_BOOT_KEY                     \
  {                                                           \
    .type = SL_SE_KEY_TYPE_ECC_P256,                          \
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY  \
             | SL_SE_KEY_FLAG_ASYMMETRIC_SIGNING_ONLY,        \
    .storage = {                                              \
      .method = SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE,         \
      .location = {                                           \
        .slot = SL_SE_KEY_SLOT_APPLICATION_SECURE_BOOT_KEY,   \
      },                                                      \
    },                                                        \
  }

/// Key descriptor for secure debug public key
#define SL_SE_APPLICATION_SECURE_DEBUG_KEY                    \
  {                                                           \
    .type = SL_SE_KEY_TYPE_ECC_P256,                          \
    .flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY  \
             | SL_SE_KEY_FLAG_ASYMMETRIC_SIGNING_ONLY,        \
    .storage = {                                              \
      .method = SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE,         \
      .location = {                                           \
        .slot = SL_SE_KEY_SLOT_APPLICATION_SECURE_DEBUG_KEY,  \
      },                                                      \
    },                                                        \
  }

/// Key descriptor for application AES-128 key
#define SL_SE_APPLICATION_AES_128_KEY                         \
  {                                                           \
    .type = SL_SE_KEY_TYPE_AES_128,                           \
    .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE,                   \
    .storage = {                                              \
      .method = SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE,         \
      .location = {                                           \
        .slot = SL_SE_KEY_SLOT_APPLICATION_AES_128_KEY,       \
      },                                                      \
    },                                                        \
  }

#endif // SL_SE_MANAGER_INTERNAL_KEYS
