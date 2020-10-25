/***************************************************************************//**
 * @file
 * @brief AES-CMAC abstraction based on CRYPTOACC
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
 * functions in gmac.c using the CRYPTOACC accelerator incorporated
 * in MCU devices from Silicon Laboratories.
 */

/*
 * http://csrc.nist.gov/publications/nistpubs/800-38D/SP-800-38D.pdf
 *
 * See also:
 * [MGV] http://csrc.nist.gov/groups/ST/toolkit/BCM/documents/proposedmodes/gcm/gcm-revised-spec.pdf
 *
 * We use the algorithm described as Shoup's method with 4-bit tables in
 * [MGV] 4.1, pp. 12-13, to enhance speed without using too much memory.
 */

#include "em_device.h"

#if defined(CRYPTOACC_PRESENT)

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_GCM_ALT) && defined(MBEDTLS_GCM_C)
#include "cryptoacc_management.h"
#include "mbedtls/gcm.h"
#include "mbedtls/aes.h"
#include "mbedtls/platform_util.h"
#include "sx_aes.h"
#include "sx_math.h"
#include "sx_errors.h"
#include "cryptolib_def.h"
#include <string.h>

/* Parameter validation macros */
#define GCM_VALIDATE_RET( cond ) \
    MBEDTLS_INTERNAL_VALIDATE_RET( cond, MBEDTLS_ERR_GCM_BAD_INPUT )
#define GCM_VALIDATE( cond ) \
    MBEDTLS_INTERNAL_VALIDATE( cond )

/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}

/*
 * Initialize a context
 */
void mbedtls_gcm_init( mbedtls_gcm_context *ctx )
{
    GCM_VALIDATE( ctx != NULL );

    memset( ctx, 0, sizeof( mbedtls_gcm_context ) );
}

// Set key
int mbedtls_gcm_setkey(mbedtls_gcm_context *ctx,
                       mbedtls_cipher_id_t cipher,
                       const unsigned char *key,
                       unsigned int keybits)
{
    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( key != NULL );
    GCM_VALIDATE_RET(cipher == MBEDTLS_CIPHER_ID_AES);
    GCM_VALIDATE_RET( keybits == 128 || keybits == 192 || keybits == 256 );

    /* Store key in gcm context */
    ctx->keybits = keybits;
    memcpy(ctx->key, key, keybits/8);

    return 0;
}

int mbedtls_gcm_starts( mbedtls_gcm_context *ctx,
                int mode,
                const unsigned char *iv,
                size_t iv_len,
                const unsigned char *add,
                size_t add_len )
{
    int status;
    uint32_t sx_ret;
    block_t  key;
    block_t  aad;
    block_t  nonce;
    block_t  hw_ctx;
    block_t  dummy = NULL_blk;

    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( iv != NULL );
    GCM_VALIDATE_RET( add_len == 0 || add != NULL );

    if ( (iv_len != AES_IV_GCM_SIZE) ||
         /* AD are limited to 2^64 bits, so 2^61 bytes */
         ( (uint64_t) add_len ) >> 61 != 0 )
    {
        return( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    /* Store input in context data structure. */
    ctx->dir = mode == MBEDTLS_AES_ENCRYPT ? ENC : DEC;
    ctx->add_len    = add_len;

    if (add_len == 0) {
        memcpy(ctx->sx_ctx, iv, AES_IV_GCM_SIZE);
    } else {
        key = block_t_convert(ctx->key, ctx->keybits / 8);
        nonce = block_t_convert(iv, AES_IV_GCM_SIZE);
        aad = block_t_convert(add, add_len);
        hw_ctx = block_t_convert(ctx->sx_ctx, AES_CTX_xCM_SIZE);

        status = cryptoacc_management_acquire();
        if (status != 0) {
            return status;
        }
        /* Execute GCM operation */
        if (ctx->dir == ENC) {
            sx_ret = sx_aes_gcm_encrypt_init((const block_t *)&key, (const block_t *)&dummy, &dummy,
                                             (const block_t *)&nonce, &hw_ctx, (const block_t *)&aad);
        } else {
            sx_ret = sx_aes_gcm_decrypt_init((const block_t *)&key, (const block_t *)&dummy, &dummy,
                                             (const block_t *)&nonce, &hw_ctx, (const block_t *)&aad);
        }
        cryptoacc_management_release();

        if (sx_ret != CRYPTOLIB_SUCCESS) {
            return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
        }
    }
    return 0;
}

int mbedtls_gcm_update( mbedtls_gcm_context *ctx,
                size_t length,
                const unsigned char *input,
                unsigned char *output )
{
    int status;
    uint32_t sx_ret;
    block_t  data_in;
    block_t  data_out;
    block_t  key;
    block_t  nonce;
    block_t  hw_ctx;
    block_t  dummy = NULL_blk;

    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( length == 0 || input != NULL );
    GCM_VALIDATE_RET( length == 0 || output != NULL );

    if (length == 0) {
        return 0;
    }

    /* Total length is restricted to 2^39 - 256 bits, ie 2^36 - 2^5 bytes
     * Also check for possible overflow */
    if( ctx->len + length < ctx->len ||
        (uint64_t) ctx->len + length > 0xFFFFFFFE0ull )
    {
        return( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    key = block_t_convert(ctx->key, ctx->keybits / 8);
    data_in = block_t_convert(input, length);
    data_out = block_t_convert(output, length);
    hw_ctx = block_t_convert(ctx->sx_ctx, AES_CTX_xCM_SIZE);

    if (ctx->add_len == 0 && ctx->len == 0) {
        /* If there were no additional authentcation data then
           mbedtls_gcm_starts did not 'CTX_BEGIN' the GCM operation
           in the CRYPTOACC, so we need to 'CTX_BEGIN' now. */
        nonce = block_t_convert(ctx->sx_ctx, AES_IV_GCM_SIZE);

        status = cryptoacc_management_acquire();
        if (status != 0) {
            return status;
        }
        /* Execute GCM operation */
        if (ctx->dir == ENC) {
            sx_ret = sx_aes_gcm_encrypt_init((const block_t *)&key, (const block_t *)&data_in, &data_out,
                                             (const block_t *)&nonce, &hw_ctx, (const block_t *)&dummy);
        } else {
            sx_ret = sx_aes_gcm_decrypt_init((const block_t *)&key, (const block_t *)&data_in, &data_out,
                                             (const block_t *)&nonce, &hw_ctx, (const block_t *)&dummy);
        }
        cryptoacc_management_release();
    }
    else {
        status = cryptoacc_management_acquire();
        if (status != 0) {
            return status;
        }
        /* Execute GCM operation */
        if (ctx->dir == ENC) {
            sx_ret = sx_aes_gcm_encrypt_update((const block_t *)&key, (const block_t *)&data_in, &data_out,
                                               (const block_t *)&hw_ctx, &hw_ctx);
        } else {
            sx_ret = sx_aes_gcm_decrypt_update((const block_t *)&key, (const block_t *)&data_in, &data_out,
                                               (const block_t *)&hw_ctx, &hw_ctx);
        }
        cryptoacc_management_release();
    }

    ctx->len += length;

    if (sx_ret != CRYPTOLIB_SUCCESS) {
        return MBEDTLS_ERR_AES_HW_ACCEL_FAILED;
    }
    else {
        return 0;
    }
}

int mbedtls_gcm_finish( mbedtls_gcm_context *ctx,
                unsigned char *tag,
                size_t tag_len )
{
    int status;
    uint32_t sx_ret;
    block_t key;
    block_t _tag;
    uint8_t tagbuf[16];
    uint8_t lena_lenc[16];
    block_t lena_lenc_blk = NULL_blk;
    block_t dummy = NULL_blk;
    block_t hw_ctx;

    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( tag != NULL );

    if (ctx->add_len == 0 && ctx->len == 0) {
        /* If there were no data and additional authentcation data then
           mbedtls_gcm_starts and update did not start the GCM operation,
           so we need to run the whole GCM now. */
        return mbedtls_gcm_crypt_and_tag( ctx,
                                          ctx->dir == ENC ? MBEDTLS_GCM_ENCRYPT :
                                          MBEDTLS_GCM_DECRYPT,
                                          0, ctx->sx_ctx, AES_IV_GCM_SIZE, 0, 0, 0, 0,
                                          tag_len, tag);
    }
    else {
        key = block_t_convert(ctx->key, ctx->keybits / 8);
        _tag = block_t_convert(tagbuf, 16); // CRYPTOACC supports only 128bits tags
        hw_ctx = block_t_convert(ctx->sx_ctx, AES_CTX_xCM_SIZE);

        // build lena_lenc block as big endian byte array
        sx_math_u64_to_u8array(ctx->add_len<<3, &lena_lenc[0], sx_big_endian);
        sx_math_u64_to_u8array(ctx->len<<3,   &lena_lenc[8], sx_big_endian);
        lena_lenc_blk = block_t_convert(lena_lenc, 16);
        status = cryptoacc_management_acquire();
        if (status != 0) {
            return status;
        }
        if (ctx->dir == ENC) {
            sx_ret = sx_aes_gcm_encrypt_final((const block_t *)&key, (const block_t *)&dummy, &dummy,
                                              (const block_t *)&hw_ctx, &_tag, (const block_t *)&lena_lenc_blk);
        } else {
            sx_ret = sx_aes_gcm_decrypt_final((const block_t *)&key, (const block_t *)&dummy, &dummy,
                                              (const block_t *)&hw_ctx, &_tag, (const block_t *)&lena_lenc_blk);
        }
        cryptoacc_management_release();

        if (sx_ret != CRYPTOLIB_SUCCESS) {
            return( MBEDTLS_ERR_AES_HW_ACCEL_FAILED );
        }

        memcpy(tag, tagbuf, tag_len);
        return( 0 );
    }
}

int mbedtls_gcm_crypt_and_tag( mbedtls_gcm_context *ctx,
                       int mode,
                       size_t length,
                       const unsigned char *iv,
                       size_t iv_len,
                       const unsigned char *add,
                       size_t add_len,
                       const unsigned char *input,
                       unsigned char *output,
                       size_t tag_len,
                       unsigned char *tag )
{
    int status;
    uint32_t sx_ret;
    sx_aes_mode_t dir = mode == MBEDTLS_AES_ENCRYPT ? ENC : DEC;
    block_t key;
    block_t aad;
    block_t _tag;
    block_t nonce;
    block_t data_in;
    block_t data_out;
    uint8_t tagbuf[16];

    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( iv != NULL );
    GCM_VALIDATE_RET( add_len == 0 || add != NULL );
    GCM_VALIDATE_RET( length == 0 || input != NULL );
    GCM_VALIDATE_RET( length == 0 || output != NULL );
    GCM_VALIDATE_RET( tag != NULL );

    if ( /* IV length is required to be 96 bits for CRYPTOACC.*/
        (iv_len != AES_IV_GCM_SIZE)
         /* AD are limited to 2^64 bits, so 2^61 bytes */
        || ( ( (uint64_t) add_len ) >> 61 != 0 ) ) {
        return( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    key = block_t_convert(ctx->key, ctx->keybits / 8);
    nonce = block_t_convert(iv, iv_len);
    aad = block_t_convert(add, add_len);
    _tag = block_t_convert(tagbuf, sizeof(tagbuf)); // CRYPTOACC supports only 128bits tags
    data_in = block_t_convert(input, length);
    data_out = block_t_convert(output, length);

    status = cryptoacc_management_acquire();
    if (status != 0) {
        return status;
    }
    /* Execute GCM operation */
    if (dir == ENC) {
        sx_ret = sx_aes_gcm_encrypt((const block_t *)&key, (const block_t *)&data_in, &data_out,
                                    (const block_t *)&nonce, &_tag, (const block_t *)&aad);
    } else {
        sx_ret = sx_aes_gcm_decrypt((const block_t *)&key, (const block_t *)&data_in, &data_out,
                                    (const block_t *)&nonce, &_tag, (const block_t *)&aad);
    }
    cryptoacc_management_release();

    if (sx_ret != CRYPTOLIB_SUCCESS) {
        mbedtls_zeroize( output, length );
        return( MBEDTLS_ERR_AES_HW_ACCEL_FAILED );
    }

    memcpy(tag, tagbuf, tag_len);
    return( 0 );
}

int mbedtls_gcm_auth_decrypt( mbedtls_gcm_context *ctx,
                      size_t length,
                      const unsigned char *iv,
                      size_t iv_len,
                      const unsigned char *add,
                      size_t add_len,
                      const unsigned char *tag,
                      size_t tag_len,
                      const unsigned char *input,
                      unsigned char *output )
{
    int status;
    uint32_t sx_ret;
    block_t key;
    block_t aad;
    block_t _tag;
    block_t nonce;
    block_t data_in;
    block_t data_out;
    uint8_t tagbuf[16];

    /* Check input parameters. */
    GCM_VALIDATE_RET( ctx != NULL );
    GCM_VALIDATE_RET( iv != NULL );
    GCM_VALIDATE_RET( add_len == 0 || add != NULL );
    GCM_VALIDATE_RET( tag != NULL );
    GCM_VALIDATE_RET( length == 0 || input != NULL );
    GCM_VALIDATE_RET( length == 0 || output != NULL );

    if ( /* IV length is required to be 96 bits for CRYPTOACC.*/
         (iv_len != AES_IV_GCM_SIZE)
         /* AD are limited to 2^64 bits, so 2^61 bytes */
         || ( ( (uint64_t) add_len ) >> 61 != 0 ) ) {
        return( MBEDTLS_ERR_GCM_BAD_INPUT );
    }

    key = block_t_convert(ctx->key, ctx->keybits / 8);
    nonce = block_t_convert(iv, iv_len);
    aad = block_t_convert(add, add_len);
    _tag = block_t_convert(tagbuf, sizeof(tagbuf)); // CRYPTOACC supports only 128bits tags
    data_in = block_t_convert(input, length);
    data_out = block_t_convert(output, length);

    status = cryptoacc_management_acquire();
    if (status != 0) {
        return status;
    }
    /* Execute GCM operation */
    sx_ret = sx_aes_gcm_decrypt((const block_t *)&key, (const block_t *)&data_in, &data_out,
                                (const block_t *)&nonce, &_tag, (const block_t *)&aad);
    cryptoacc_management_release();

    if (sx_ret == CRYPTOLIB_SUCCESS) {
        if (memcmp_time_cst((uint8_t*)tag, tagbuf, tag_len) == 0) {
            return( 0 );
        } else {
            mbedtls_zeroize( output, length );
            return( MBEDTLS_ERR_GCM_AUTH_FAILED );
        }
    } else {
        mbedtls_zeroize( output, length );
        return( MBEDTLS_ERR_AES_HW_ACCEL_FAILED );
    }
}

void mbedtls_gcm_free( mbedtls_gcm_context *ctx )
{
    if( ctx == NULL )
        return;
    mbedtls_zeroize( ctx, sizeof( mbedtls_gcm_context ) );
}

#endif /* MBEDTLS_GCM_ALT && MBEDTLS_GCM_C */

#endif /* CRYPTOACC_PRESENT */
