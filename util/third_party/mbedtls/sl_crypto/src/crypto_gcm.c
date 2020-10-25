/***************************************************************************//**
 * @file
 * @brief AES-GCM acceleration for Silicon Labs series-1 devices
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
/*
 * This file includes alternative plugin implementations of various
 * functions in gcm.c using the CRYPTO accelerator incorporated
 * in MCU Series-1 devices from Silicon Laboratories.
 */

/*
 *  NIST SP800-38D compliant GCM implementation
 * http://csrc.nist.gov/publications/nistpubs/800-38D/SP-800-38D.pdf
 *
 * See also:
 * [MGV] http://csrc.nist.gov/groups/ST/toolkit/BCM/documents/proposedmodes/gcm/gcm-revised-spec.pdf
 *
 */

#include "em_device.h"

#if defined(CRYPTO_PRESENT)

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_GCM_C)
#if defined(MBEDTLS_GCM_ALT)

#include "crypto_management.h"
#include "em_crypto.h"
#include "em_core.h"
#include "mbedtls/gcm.h"
#include "mbedtls/aes.h"
#include "mbedtls/platform.h"
#include <string.h>

/*
 * The GCM algorithm is based on two main functions, GHASH and GCTR, defined
 * in http://csrc.nist.gov/publications/nistpubs/800-38D/SP-800-38D.pdf
 *
 * The GHASH and GCTR fuctions can be implemented with the following
 * instruction sequences that can be executed by the CRYPTO peripheral.
 */

/*
 * GCTR_SEQUENCE for encryption and decryption.
 *
 * Procedure is executed with following assumptions:
 * - data to be processed is in DATA0
 * - DATA1 contains counter which is preincremented
 * - DATA3 is used as temporary register
 * - result is stored in DATA0
 *
 *   CRYPTO_CMD_INSTR_DATA0TODATA3
 *   CRYPTO_CMD_INSTR_DATA1INC
 *   CRYPTO_CMD_INSTR_DATA1TODATA0
 *   CRYPTO_CMD_INSTR_AESENC
 *   CRYPTO_CMD_INSTR_DATA3TODATA0XOR
 */

/*
 * GHASH_SEQUENCE for encryption, decryption and tag
 *
 * Procedure is executed with following assumptions:
 * - data to be processed is in DATA0 ( A[i] or C[i] )
 * - temporary GHASH is stored in DDATA0
 * - H is stored in DATA2 (part of DDATA3)
 * - DDATA1 is used temporary for holding input to the MMUL instruction
 *   (MMUL DDATA0 = (DDATA1 * V1) mod P V1 != DDATA0,DDATA1)
 *
 *  Note: The DDATA3 in SELDDATA0DDATA3 is for the BBSWAP128.
 *
 *  CRYPTO_CMD_INSTR_SELDDATA0DDATA2
 *  CRYPTO_CMD_INSTR_XOR
 *  CRYPTO_CMD_INSTR_BBSWAP128
 *  CRYPTO_CMD_INSTR_DDATA0TODDATA1
 *  CRYPTO_CMD_INSTR_SELDDATA0DDATA3
 *  CRYPTO_CMD_INSTR_MMUL
 *  CRYPTO_CMD_INSTR_BBSWAP128
 */

/*
 * CRYPTO register usage:
 *  DATA0 - temporary result and plaintext
 *  DATA1 - Counter state
 *  DATA2 - Hash key
 *  DATA3 - used temporary
 *
 *  DDATA0 - temporary results in the GHASH function (Xi-1)
 *  DDATA1 - temporary for MMUL operands
 *  DDATA2 - overlaps DATA0 and DATA1
 *  DDATA3 - overlaps DATA2 and DATA3
 *  DDATA4 - KEYBUF
 */

// Parameter validation macros
#define GCM_VALIDATE_RET(cond) \
  MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_GCM_BAD_INPUT)
#define GCM_VALIDATE(cond) \
  MBEDTLS_INTERNAL_VALIDATE(cond)

// Implementation that should never be optimized out by the compiler.
static void mbedtls_zeroize(void *v, size_t n)
{
  volatile unsigned char *p = v; while (n--) *p++ = 0;
}

// Initialize a context
void mbedtls_gcm_init(mbedtls_gcm_context *ctx)
{
  GCM_VALIDATE(ctx != NULL);

  memset(ctx, 0, sizeof(mbedtls_gcm_context) );
}

// Set key
int mbedtls_gcm_setkey(mbedtls_gcm_context *ctx,
                       mbedtls_cipher_id_t cipher,
                       const unsigned char *key,
                       unsigned int keybits)
{
  GCM_VALIDATE_RET(ctx != NULL);
  GCM_VALIDATE_RET(key != NULL);
  GCM_VALIDATE_RET(cipher == MBEDTLS_CIPHER_ID_AES);

  // CRYPTO supports 128 and 256 bits keys. Otherwise return
  // MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED which is expected by
  // the GCM selftest.
  if ((keybits != 128) && (keybits != 256)) {
    return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
  }

  // Store key in gcm context
  ctx->keybits = keybits;
  memcpy(ctx->key, key, keybits / 8);

  return(0);
}

// Read value of a CRYPTO DATA register into an unaligned RAM buffer
static inline void crypto_data_read_unaligned(volatile uint32_t * reg,
                                              uint8_t * buf)
{
  // Check buffer pointer is 32bit-aligned, if not, read into temporary buffer
  // and then move to user buffer.
  if ((uint32_t)buf & 0x3) {
    uint32_t temp[4];
    CRYPTO_DataRead(reg, temp);
    memcpy(buf, temp, 16);
  } else {
    CRYPTO_DataRead(reg, (uint32_t *)buf);
  }
}

// Write data in an unaligned RAM buffer into CRYPTO DATA register
__STATIC_INLINE void crypto_data_write_unaligned(volatile uint32_t * reg,
                                                 const uint8_t * buf)
{
  // Check if buffer pointer is 32bit-aligned, if not move to temporary buffer
  // before writing.
  if ((uint32_t)buf & 0x3) {
    uint32_t temp[4];
    memcpy(temp, buf, 16);
    CRYPTO_DataWrite(reg, temp);
  } else {
    CRYPTO_DataWrite(reg, (const uint32_t *)buf);
  }
}

// Write data in an unaligned RAM buffer into CRYPTO DATA register
__STATIC_INLINE void gcm_restore_crypto_state(mbedtls_gcm_context *ctx,
                                              CRYPTO_TypeDef *crypto)
{
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  // Restore state
  if (ctx->keybits == 128) {
    // Set AES-128 mode
    crypto->CTRL = CRYPTO_CTRL_AES_AES128
      | CRYPTO_CTRL_INCWIDTH_INCWIDTH4 | CRYPTO_CTRL_DMA0RSEL_DATA0;
    // Load 128 bits key
    CRYPTO_DataWrite(&crypto->KEYBUF, ctx->key);
  }
  else {
    // Set AES-256 mode
    crypto->CTRL = CRYPTO_CTRL_AES_AES256
      | CRYPTO_CTRL_INCWIDTH_INCWIDTH4 | CRYPTO_CTRL_DMA0RSEL_DATA0;
    // Load 256 bits key
    CRYPTO_DDataWrite(&crypto->KEYBUF, ctx->key);
  }

  crypto->WAC = CRYPTO_WAC_MODULUS_GCMBIN128 | CRYPTO_WAC_RESULTWIDTH_128BIT;
  CRYPTO_DataWrite(&crypto->DATA2, ctx->ghash_key);
  CRYPTO_DataWrite(&crypto->DATA1, ctx->gctr_state);
  // Write GHASH state (128 bits) temporarily to DATA0 (in DDATA2) then
  // move it to DDATA0 (256 bits) where CRYPTO expects it.
  CRYPTO_DataWrite(&crypto->DATA0, ctx->ghash_state);
  crypto->CMD = CRYPTO_CMD_INSTR_DDATA2TODDATA0;

  CORE_EXIT_CRITICAL();
}

// Start GCM streaming operation
int mbedtls_gcm_starts(mbedtls_gcm_context *ctx,
                       int mode,
                       const unsigned char *iv,
                       size_t iv_len,
                       const unsigned char *add,
                       size_t add_len)
{
  bool            store_state_and_release;
  CRYPTO_TypeDef *crypto;
  uint32_t        temp[4];
  CORE_DECLARE_IRQ_STATE;

  GCM_VALIDATE_RET(ctx != NULL);
  GCM_VALIDATE_RET(iv != NULL);
  GCM_VALIDATE_RET(add_len == 0 || add != NULL);

  if (iv_len != 12
      ||// AD are limited to 2^64 bits, so 2^61 bytes.
      ((uint64_t) add_len) >> 61 != 0) {
    return(MBEDTLS_ERR_GCM_BAD_INPUT);
  }

  // Check if this context has already acquired a crypto device, which means
  // the caller should be mbedtls_gcm_crypt_and_tag() which will perform GCM
  // on a full block and call starts, update, finish in a sequence meaning we
  // will not need to store the state in between.
  if (ctx->device == 0) {
    ctx->device = crypto_management_acquire();
    store_state_and_release = true;
  } else {
    store_state_and_release = false;
  }
  crypto = ctx->device;

  // Setup CRTL and KEY
  //   - AES-128 or AES-256 mode
  //   - width of counter in CTR cipher mode to 4 bytes.
  //   - associate DATA0 with DMA instructions which are used in order to
  //     synchronize with MCU load/store during instruction sequence loops
  //     (a DMA engine is not used).
  if (ctx->keybits == 128) {
    // Set AES-128 mode
    crypto->CTRL = CRYPTO_CTRL_AES_AES128
      | CRYPTO_CTRL_INCWIDTH_INCWIDTH4 | CRYPTO_CTRL_DMA0RSEL_DATA0;
    // Load 128 bits key
    CRYPTO_DataWrite(&crypto->KEYBUF, ctx->key);
  }
  else {
    // Set AES-256 mode
    crypto->CTRL = CRYPTO_CTRL_AES_AES256
      | CRYPTO_CTRL_INCWIDTH_INCWIDTH4 | CRYPTO_CTRL_DMA0RSEL_DATA0;
    // Load 256 bits key
    CRYPTO_DDataWrite(&crypto->KEYBUF, ctx->key);
  }

  // Set GCM modulus, operand and result widths
  crypto->WAC   = CRYPTO_WAC_MODULUS_GCMBIN128 | CRYPTO_WAC_RESULTWIDTH_128BIT;
  crypto->SEQCTRL  = 16;
  crypto->SEQCTRLB = 0;

  // Load the initial counter value Y0 into DATA1.
  //   Y0 = IV | 1
  //   That is, the 12 byte Initial Vector concatenated with the initial
  //   counter value 1 in the least significant 32bit word.
  memcpy(ctx->gctr_state, iv, 12);
  ctx->gctr_state[3] = 0x01000000;
  CRYPTO_DataWrite(&crypto->DATA1, ctx->gctr_state);

  CORE_ENTER_CRITICAL();

  // Organize and Compute the initial values.
  CRYPTO_EXECUTE_6(crypto,
                   // Compute hash key H=E(K,0) in DATA2 (LSB DDATA3)
                   CRYPTO_CMD_INSTR_DATA0TODATA0XOR,// DATA0 = 0
                   CRYPTO_CMD_INSTR_AESENC,         // DATA0 = AESENC(DATA0)
                   CRYPTO_CMD_INSTR_SELDDATA2DDATA2,// Select DATA0 for BBSWAP
                   CRYPTO_CMD_INSTR_BBSWAP128,      // Swap bit in H, for GMAC
                   // Move resulting hash key (DDATA0 LSB) to DATA2 (DDATA3 LSB)
                   CRYPTO_CMD_INSTR_DDATA0TODDATA3,
                   // Prepare GHASH(H,A)
                   CRYPTO_CMD_INSTR_CLR             // DDATA0 = 0
                   );

  CORE_EXIT_CRITICAL();

  // Remember mode and additional authentication length
  ctx->mode = mode;
  ctx->add_len = add_len;

  // Process additional authentication data if present.
  if (add_len) {
    crypto->SEQCTRLB = 0; // Sequence B is not used for auth data

    // Set SEQCTRL_LENGTHA to loop through all blocks
    // We need to do set SEQCTRL_LENGTHA to 16*num_complete_blocks, plus 16 if
    // there is an in-complete block at the end.
    crypto->SEQCTRL = (add_len & 0xFFFFFFF0) + ((add_len & 0xF) ? 16 : 0);

    CORE_ENTER_CRITICAL();

    // Run GHASH sequence on additional authentication data
    CRYPTO_EXECUTE_8(crypto,
                     // Load additional auth data
                     CRYPTO_CMD_INSTR_DMA0TODATA,
                     // GHASH_SEQUENCE (see desc above)
                     CRYPTO_CMD_INSTR_SELDDATA0DDATA2,
                     CRYPTO_CMD_INSTR_XOR,
                     CRYPTO_CMD_INSTR_BBSWAP128,
                     CRYPTO_CMD_INSTR_DDATA0TODDATA1,
                     CRYPTO_CMD_INSTR_SELDDATA0DDATA3,
                     CRYPTO_CMD_INSTR_MMUL,
                     CRYPTO_CMD_INSTR_BBSWAP128
                     );

    // First loop through and write data for all complete blocks
    while (add_len >= 16) {
      add_len  -= 16;
      // Wait for sequencer to accept data
      while (!(crypto->STATUS & CRYPTO_STATUS_DMAACTIVE)) ;
      crypto_data_write_unaligned(&crypto->DATA0, add);
      add      += 16;
    }
    if (add_len > 0) {
      // For last in-complete block, use temporary buffer for zero padding.
      memset(temp, 0, 16);
      memcpy(temp, add, add_len);
      while (!(crypto->STATUS & CRYPTO_STATUS_DMAACTIVE)) ;
      CRYPTO_DataWrite(&crypto->DATA0, temp);
    }
    // Wait for completion
    while (!CRYPTO_InstructionSequenceDone(crypto)) ;

    CORE_EXIT_CRITICAL();
  }

  if (store_state_and_release) {
    CORE_ENTER_CRITICAL();
    // Move GHASH state in DDATA0 temporarily to DATA0 (in DDATA2) in order to
    // read only the 128 bits value (since DDATA0 is 256 bits wide).
    crypto->CMD = CRYPTO_CMD_INSTR_DDATA0TODDATA2;
    CRYPTO_DataRead(&crypto->DATA0, ctx->ghash_state);
    CRYPTO_DataRead(&crypto->DATA2, ctx->ghash_key);
    CORE_EXIT_CRITICAL();

    crypto_management_release(crypto);
    ctx->device = 0;
  }

  return(0);
}

// Update a GCM streaming operation with more input data to be
// encrypted or decrypted.
int mbedtls_gcm_update(mbedtls_gcm_context *ctx,
                       size_t length,
                       const unsigned char *input,
                       unsigned char *output)
{
  bool            restore_state_and_release;
  CRYPTO_TypeDef *crypto;
  uint32_t        temp[4];
  unsigned int    sequence_loop_length;
  bool            last_block_is_incomplete;
  CORE_DECLARE_IRQ_STATE;

  GCM_VALIDATE_RET(ctx != NULL);
  GCM_VALIDATE_RET(length == 0 || input != NULL);
  GCM_VALIDATE_RET(length == 0 || output != NULL);

  if (length == 0) {
    return 0;
  }

  // Total length is restricted to 2^39 - 256 bits, ie 2^36 - 2^5 bytes
  // Also check for possible overflow.
  if (ctx->len + length < ctx->len
      || (uint64_t) ctx->len + length > 0xFFFFFFFE0ull) {
    return(MBEDTLS_ERR_GCM_BAD_INPUT);
  }

  // Check if this context has already acquired a crypto device, which means
  // the caller should be mbedtls_gcm_crypt_and_tag() which will perform GCM
  // on a full block and call starts, update, finish in a sequence meaning we
  // will not need to store the state in between.
  if (ctx->device == 0) {
    ctx->device = crypto_management_acquire();
    crypto = ctx->device;
    restore_state_and_release = true;
    gcm_restore_crypto_state(ctx, crypto);
  } else {
    restore_state_and_release = false;
    crypto = ctx->device;
  }

  ctx->len += length;

  // Calculate total sequence length 16*num_complete_blocks, plus 16 if
  // there is an in-complete block at the end.
  last_block_is_incomplete = length & 0xF;
  sequence_loop_length =
    (length & 0xFFFFFFF0) + (last_block_is_incomplete ? 16 : 0);

  CORE_ENTER_CRITICAL();

  if (ctx->mode == MBEDTLS_GCM_DECRYPT) {
    crypto->SEQCTRL  = sequence_loop_length;
    crypto->SEQCTRLB = 0;

    // Start decryption sequence
    CRYPTO_EXECUTE_14(crypto,
                      CRYPTO_CMD_INSTR_DMA0TODATA,  // Load Ciphertext

                      // GHASH_SEQUENCE (see desc above)
                      CRYPTO_CMD_INSTR_SELDDATA0DDATA2,
                      CRYPTO_CMD_INSTR_XOR,
                      CRYPTO_CMD_INSTR_BBSWAP128,
                      CRYPTO_CMD_INSTR_DDATA0TODDATA1,
                      CRYPTO_CMD_INSTR_SELDDATA0DDATA3,
                      CRYPTO_CMD_INSTR_MMUL,
                      CRYPTO_CMD_INSTR_BBSWAP128,

                      // GCTR_SEQUENCE (see desc above)
                      CRYPTO_CMD_INSTR_DATA0TODATA3,
                      CRYPTO_CMD_INSTR_DATA1INC,
                      CRYPTO_CMD_INSTR_DATA1TODATA0,
                      CRYPTO_CMD_INSTR_AESENC,
                      CRYPTO_CMD_INSTR_DATA3TODATA0XOR,
                      CRYPTO_CMD_INSTR_DATATODMA0   // Store Plaintext
                      );
  } else {
    // For encryption we need to handle the last block differently if it is
    // incomplete. We need to zeroize bits outside len(PT) in DATA0 before
    // the GHASH operation. We do this by using a DMA0TODATA instruction in
    // the B sequence, meaning that the sequencer will wait for the MCU core
    // to zeroize bits and write them back to DATA0.
    if (last_block_is_incomplete) {
      crypto->SEQCTRL  = sequence_loop_length - 16;
      crypto->SEQCTRLB = 16;
    } else {
      crypto->SEQCTRL  = sequence_loop_length;
      crypto->SEQCTRLB = 0;
    }

    // Start encryption sequence
    CRYPTO_EXECUTE_17(crypto,
                      CRYPTO_CMD_INSTR_DMA0TODATA,  // Load Plaintext

                      // GCTR_SEQUENCE (see desc above)
                      CRYPTO_CMD_INSTR_DATA0TODATA3,
                      CRYPTO_CMD_INSTR_DATA1INC,
                      CRYPTO_CMD_INSTR_DATA1TODATA0,
                      CRYPTO_CMD_INSTR_AESENC,
                      CRYPTO_CMD_INSTR_DATA3TODATA0XOR,
                      CRYPTO_CMD_INSTR_DATATODMA0,  // Store Ciphertext

                      CRYPTO_CMD_INSTR_EXECIFB,
                      CRYPTO_CMD_INSTR_DMA0TODATA,  // Load X XOR MSB(CIPH(CB))
                      CRYPTO_CMD_INSTR_EXECALWAYS,

                      // GHASH_SEQUENCE (see desc above)
                      CRYPTO_CMD_INSTR_SELDDATA0DDATA2,
                      CRYPTO_CMD_INSTR_XOR,
                      CRYPTO_CMD_INSTR_BBSWAP128,
                      CRYPTO_CMD_INSTR_DDATA0TODDATA1,
                      CRYPTO_CMD_INSTR_SELDDATA0DDATA3,
                      CRYPTO_CMD_INSTR_MMUL,
                      CRYPTO_CMD_INSTR_BBSWAP128
                      );
  }

  while (length >= 16) {
    length  -= 16;
    // Wait for sequencer to accept data
    while (!(crypto->STATUS & CRYPTO_STATUS_DMAACTIVE)) ;
    crypto_data_write_unaligned(&crypto->DATA0, input);
    input   += 16;
    // Wait for sequencer to finish iteration and make data available
    while (!(crypto->STATUS & CRYPTO_STATUS_DMAACTIVE)) ;
    crypto_data_read_unaligned(&crypto->DATA0, output);
    output  += 16;
  }

  if (length > 0) {
    // The last block is incomplete, so we need to zero pad bits outside len(PT)
    // Use temporary buffer for zero padding
    memset(temp, 0, 16);
    memcpy(temp, input, length);

    while (!(crypto->STATUS & CRYPTO_STATUS_DMAACTIVE)) ;
    // Write last input data (PT/CT)
    CRYPTO_DataWrite(&crypto->DATA0, temp);
    while (!(crypto->STATUS & CRYPTO_STATUS_DMAACTIVE)) ;
    // Read last output data (CT/PT)
    CRYPTO_DataRead(&crypto->DATA0, temp);

    if (ctx->mode == MBEDTLS_GCM_ENCRYPT) {
      // For encryption, when the last block is incomplete we need to
      // zeroize bits outside len(PT) in DATA0 before the GHASH operation.
      memset(&((uint8_t*)temp)[length], 0, 16 - length);
      CRYPTO_DataWrite(&crypto->DATA0, temp);
    }

    // Copy to output buffer now while CRYPTO performs GHASH.
    memcpy(output, temp, length);
  }

  // Wait for completion
  while (!CRYPTO_InstructionSequenceDone(crypto)) ;

  CORE_EXIT_CRITICAL();

  if (restore_state_and_release) {

    CORE_ENTER_CRITICAL();
    CRYPTO_DataRead(&crypto->DATA1, ctx->gctr_state);
    // Move GHASH state in DDATA0 temporarily to DATA2 (in DDATA3) in order to
    // read only the 128 bits value (since DDATA0 is 256 bits wide).
    crypto->CMD = CRYPTO_CMD_INSTR_DDATA0TODDATA3;
    CRYPTO_DataRead(&crypto->DATA2, ctx->ghash_state);
    CORE_EXIT_CRITICAL();

    crypto_management_release(crypto);
    ctx->device = 0;
  }

  return(0);
}

// Finish GCM streaming operation
int mbedtls_gcm_finish(mbedtls_gcm_context *ctx,
                       unsigned char *tag,
                       size_t tag_len)
{
  bool            restore_state_and_release;
  uint64_t        bit_len;
  uint32_t        temp[4];
  CRYPTO_TypeDef *crypto = ctx->device;
  CORE_DECLARE_IRQ_STATE;

  GCM_VALIDATE_RET(ctx != NULL);
  GCM_VALIDATE_RET(tag != NULL);

  // Check if this context has already acquired a crypto device, which means
  // the caller should be mbedtls_gcm_crypt_and_tag() which will perform GCM
  // on a full block and call starts, update, finish in a sequence meaning we
  // will not need to store the state in between.
  if (ctx->device == 0) {
    ctx->device = crypto_management_acquire();
    crypto = ctx->device;
    restore_state_and_release = true;
    gcm_restore_crypto_state(ctx, crypto);
  } else {
    restore_state_and_release = false;
    crypto = ctx->device;
  }

  // Compute last part of the GHASH and authentication tag:
  //
  // Xm+n+1 = (Xm+n XOR (len(A)|len(C))) * H
  //
  // Compute the final authentication tag now.
  // T = MSBt (GHASH(H, A, C) XOR E(K, Y0))
  // where GHASH(H, A, C) is Xm+n+1 which is stored in DATA0 (LSWord of DDATA2)
  // and Counter value Yn is stored in DATA1. We assume N (in Yn) is not bigger
  // than 2^32 so E(K, Y0) derived by clearing DATA1 which gives Y0.
  crypto->SEQCTRL = 16;
  crypto->SEQCTRLB = 0;

  CORE_ENTER_CRITICAL();

  // Temporarily load length fields value 128 bits into DATA0 register
  // Then move it into LSB of DDATA1 (for optimal performance)
  bit_len = ctx->add_len * 8;
  crypto->DATA0 = __REV((uint32_t) (bit_len>>32));
  crypto->DATA0 = __REV((uint32_t) bit_len);
  bit_len = ctx->len * 8;
  crypto->DATA0 = __REV((uint32_t) (bit_len>>32));
  crypto->DATA0 = __REV((uint32_t) bit_len);

  // Calculate last part of GHASH (in DDATA0)
  // adding in length fields in DATA0 ( LSB of DDATA2 )
  CRYPTO_EXECUTE_16(crypto,
                    // Move length fields in DATA0 (in DDATA2) to DDATA1
                    CRYPTO_CMD_INSTR_DDATA2TODDATA1,
                    // See descripton of GHASH_SEQUENCE above.
                    CRYPTO_CMD_INSTR_SELDDATA0DDATA1, // A[i] and Xi-1
                    CRYPTO_CMD_INSTR_XOR,
                    CRYPTO_CMD_INSTR_BBSWAP128,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA1,
                    CRYPTO_CMD_INSTR_SELDDATA0DDATA3, // temp result and H
                    CRYPTO_CMD_INSTR_MMUL,       // Xi is stored in DDATA0
                    CRYPTO_CMD_INSTR_BBSWAP128,
                    CRYPTO_CMD_INSTR_DDATA0TODDATA3,
                    CRYPTO_CMD_INSTR_DATA2TODATA0,
                    // Calculate AuthTag
                    CRYPTO_CMD_INSTR_DATA1INCCLR,
                    CRYPTO_CMD_INSTR_DATA0TODATA3,
                    CRYPTO_CMD_INSTR_DATA1INC,
                    CRYPTO_CMD_INSTR_DATA1TODATA0,
                    CRYPTO_CMD_INSTR_AESENC,
                    CRYPTO_CMD_INSTR_DATA3TODATA0XOR
                    ); // DATA0 = DATA0 ^ DATA3

  // Wait for completion
  while (!CRYPTO_InstructionSequenceDone(crypto)) ;

  if (tag_len) {
    CRYPTO_DataRead(&crypto->DATA0, temp);
    memcpy(tag, temp, tag_len);
  }

  CORE_EXIT_CRITICAL();

  if (restore_state_and_release) {
    crypto_management_release(crypto);
    ctx->device = 0;
  }

  return(0);
}

// Perform full GCM block encryption or decryption operation with tag
int mbedtls_gcm_crypt_and_tag(mbedtls_gcm_context *ctx,
                              int mode,
                              size_t length,
                              const unsigned char *iv,
                              size_t iv_len,
                              const unsigned char *add,
                              size_t add_len,
                              const unsigned char *input,
                              unsigned char *output,
                              size_t tag_len,
                              unsigned char *tag)
{
  int ret;

  ctx->device = crypto_management_acquire();

  ret = mbedtls_gcm_starts(ctx, mode, iv, iv_len, add, add_len);
  if (ret != 0) {
    goto exit;
  }

  ret = mbedtls_gcm_update(ctx, length, input, output);
  if (ret != 0) {
    goto exit;
  }

  ret = mbedtls_gcm_finish(ctx, tag, tag_len);
  if (ret != 0) {
    goto exit;
  }

  exit:
  crypto_management_release(ctx->device);
  return(ret);
}

// Perform full GCM decryption and tag verification
int mbedtls_gcm_auth_decrypt(mbedtls_gcm_context *ctx,
                             size_t length,
                             const unsigned char *iv,
                             size_t iv_len,
                             const unsigned char *add,
                             size_t add_len,
                             const unsigned char *tag,
                             size_t tag_len,
                             const unsigned char *input,
                             unsigned char *output)
{
  int ret;
  unsigned char check_tag[16];
  size_t i;
  int diff;

  if ((ret = mbedtls_gcm_crypt_and_tag(ctx, MBEDTLS_GCM_DECRYPT, length,
                                       iv, iv_len,
                                       add, add_len,
                                       input, output,
                                       tag_len, check_tag)) != 0) {
    return(ret);
  }

  // Check tag in "constant-time"
  for (diff = 0, i = 0; i < tag_len; i++) {
    diff |= tag[i] ^ check_tag[i];
  }

  if (diff != 0) {
    mbedtls_zeroize(output, length);
    return(MBEDTLS_ERR_GCM_AUTH_FAILED);
  }

  return(0);
}

// Free GCM context
void mbedtls_gcm_free(mbedtls_gcm_context *ctx)
{
  mbedtls_zeroize(ctx, sizeof(mbedtls_gcm_context) );
}

#endif // MBEDTLS_GCM_ALT

#endif // MBEDTLS_GCM_C

#endif // CRYPTO_PRESENT
