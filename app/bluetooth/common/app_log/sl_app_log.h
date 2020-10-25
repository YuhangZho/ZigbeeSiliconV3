/***************************************************************************//**
 * @file
 * @brief Application logging interface
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

#ifndef SL_APP_LOG_H
#define SL_APP_LOG_H

#include "printf.h"
#include "sl_app_log_config.h"

#if defined(SL_APP_LOG_ENABLE) && SL_APP_LOG_ENABLE
#define sl_app_log(...)    printf(__VA_ARGS__)
#else // SL_APP_LOG_ENABLE
#define sl_app_log(...)
#endif // SL_APP_LOG_ENABLE

#endif // SL_APP_LOG_H
