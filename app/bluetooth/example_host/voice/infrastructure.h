/***************************************************************************//**
 * @file
 * @brief Infrastructure for code development, macros for type conversion
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

#ifndef INFRASTRUCTURE_H
#define INFRASTRUCTURE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/**************************************************************************************************
 * Macros for type conversion
 *************************************************************************************************/

#define FLT_TO_UINT32(m, e)       (((uint32_t)(m) & 0x00FFFFFFU) | (uint32_t)((int32_t)(e) << 24))

#define UINT16_TO_BITSTREAM(p, n)   { *(p)++ = (uint8_t)(n); *(p)++ = (uint8_t)((n) >> 8); }

#define UINT8_TO_BITSTREAM(p, n)    { *(p)++ = (uint8_t)(n); }

#define UINT16_TO_BYTES(n)        ((uint8_t) (n)), ((uint8_t)((n) >> 8))

#define UINT32_TO_BITSTREAM(p, n)   { *(p)++ = (uint8_t)(n); *(p)++ = (uint8_t)((n) >> 8); \
                                      *(p)++ = (uint8_t)((n) >> 16); *(p)++ = (uint8_t)((n) >> 24); }

#define UINT16_TO_BYTE0(n)        ((uint8_t) (n))
#define UINT16_TO_BYTE1(n)        ((uint8_t) ((n) >> 8))

#define UINT32_TO_BYTE0(n)        ((uint8_t) (n))
#define UINT32_TO_BYTE1(n)        ((uint8_t) ((n) >> 8))
#define UINT32_TO_BYTE2(n)        ((uint8_t) ((n) >> 16))
#define UINT32_TO_BYTE3(n)        ((uint8_t) ((n) >> 24))

#define MIN(a, b)                  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)                  (((a) > (b)) ? (a) : (b))

#define COUNTOF(x)                ((sizeof (x)) / (sizeof ((x)[0])))

#ifdef __cplusplus
};
#endif

#endif /* INFRASTRUCTURE_H */
