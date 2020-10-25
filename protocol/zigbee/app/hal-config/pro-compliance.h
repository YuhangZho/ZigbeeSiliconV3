/***************************************************************************//**
 * @file
 * @brief Pro Compliance App HAL configuration
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

// Board specific overrides
// Boards 4171A, 4176A, 4182A do not connect the RTS/CTS pins,
// therefore we can not use hardware flow control.
#if defined(BOARD_BRD4171A) || defined(BOARD_BRD4176A)
#define HAL_SERIAL_APP_FLOW_CONTROL          HAL_USART_FLOW_CONTROL_NONE
#define HAL_SERIAL_USART1_ENABLE 1
// no VCOM ENABLE pin on 4171A and 4176A
#elif defined(BOARD_BRD4182A)
#define HAL_SERIAL_APP_FLOW_CONTROL          HAL_USART_FLOW_CONTROL_NONE
#define HAL_SERIAL_USART1_ENABLE 1
#define HAL_VCOM_ENABLE 1
#else
#define HAL_SERIAL_APP_FLOW_CONTROL          HAL_USART_FLOW_CONTROL_HWUART
#define HAL_SERIAL_USART0_ENABLE 1
#define HAL_VCOM_ENABLE 1
#endif

#define HAL_SERIAL_APP_BAUD_RATE             115200
#define HAL_SERIAL_APP_RXSTART               16
#define HAL_SERIAL_APP_RXSTOP                16
#define HAL_SERIAL_APP_RX_QUEUE_SIZE         64
#define HAL_SERIAL_APP_TX_QUEUE_SIZE         128

#define HAL_WDOG_ENABLE 1

#endif //APP_HAL_CONFIG_H
