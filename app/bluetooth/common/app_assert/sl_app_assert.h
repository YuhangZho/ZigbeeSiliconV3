/***************************************************************************//**
 * @file
 * @brief Application assert interface
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

#ifndef SL_APP_ASSERT_H
#define SL_APP_ASSERT_H

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT
#ifdef SL_CATALOG_PRINTF_PRESENT
#include "printf.h"
#endif // SL_CATALOG_PRINTF_PRESENT
#include "sl_app_assert_config.h"

#if defined(SL_APP_ASSERT_ENABLE) && SL_APP_ASSERT_ENABLE
#ifdef SL_CATALOG_PRINTF_PRESENT
#define sl_app_assert(expr, ...) \
  do {                           \
    if (!(expr)) {               \
      printf("[%s] ", __func__); \
      printf(__VA_ARGS__);       \
      while (1) ;                \
    }                            \
  } while (0)
#else // SL_CATALOG_PRINTF_PRESENT
#define sl_app_assert(expr, ...) \
  do {                           \
    if (!(expr)) {               \
      while (1) ;                \
    }                            \
  } while (0)
#endif // SL_CATALOG_PRINTF_PRESENT
#else // SL_APP_ASSERT_ENABLE
#define sl_app_assert(expr, ...)
#endif // SL_APP_ASSERT_ENABLE

#endif // SL_APP_ASSERT_H
