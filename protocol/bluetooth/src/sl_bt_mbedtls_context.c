#include "sl_bt_mbedtls_context.h"

size_t sl_bt_get_mbedtls_aes_ctx_size()
{
  return sizeof(mbedtls_aes_context);
}

size_t sl_bt_get_mbedtls_cipher_ctx_size()
{
  return sizeof(mbedtls_cipher_context_t);
}

size_t sl_bt_get_mbedtls_crt_drbg_ctx_size()
{
  return sizeof(mbedtls_ctr_drbg_context);
}

size_t sl_bt_get_mbedtls_entropy_ctx_size()
{
  return sizeof(mbedtls_entropy_context);
}
