/***************************************************************************//**
 * @file
 * @brief Sniff HAL configuration
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

#ifndef APP_HAL_CONFIG_H
#define APP_HAL_CONFIG_H

#include "hal-config-types.h"

#undef BSP_SERIAL_APP_PORT
#define BSP_SERIAL_APP_PORT HAL_SERIAL_PORT_VUART

// undefine UARTNCP to prevent auto-enabling USART
#undef BSP_UARTNCP_USART_PORT
#define HAL_SERIAL_VUART_ENABLE 1

#define HAL_WDOG_ENABLE 1

#endif //APP_HAL_CONFIG_H
