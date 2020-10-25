/***************************************************************************//**
 * @file
 * @brief EZSP UART NCP HAL configuration
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

#define HAL_SERIAL_VUART_ENABLE 1
#define HAL_SERIAL_USART0_ENABLE 1

#define HAL_UARTNCP_BAUD_RATE             115200
#define HAL_UARTNCP_FLOW_CONTROL          HAL_USART_FLOW_CONTROL_HW
#define HAL_UARTNCP_RXSTART               16
#define HAL_UARTNCP_RXSTOP                16
#define HAL_UARTNCP_RX_QUEUE_SIZE         64
#define HAL_UARTNCP_TX_QUEUE_SIZE         128

#define HAL_VCOM_ENABLE                               (1)
#define HAL_WDOG_ENABLE 1

#endif //APP_HAL_CONFIG_H
