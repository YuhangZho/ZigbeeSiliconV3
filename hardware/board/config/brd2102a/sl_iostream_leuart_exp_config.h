/***************************************************************************//**
 * @file
 * @brief SL_IOSTREAM_LEUART Config.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#ifndef SL_IOSTREAM_LEUART_EXP_CONFIG_H
#define SL_IOSTREAM_LEUART_EXP_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h>LEUART settings

// <o SL_IOSTREAM_LEUART_EXP_BAUDRATE> Baud rate
// <i> Default: 9600
#define SL_IOSTREAM_LEUART_EXP_BAUDRATE              9600

// <o SL_IOSTREAM_LEUART_EXP_PARITY> Parity mode to use
// <leuartNoParity=> No Parity
// <leuartEvenParity=> Even parity
// <leuartOddParity=> Odd parity
// <i> Default: leuartNoParity
#define SL_IOSTREAM_LEUART_EXP_PARITY                leuartNoParity

// <o SL_IOSTREAM_LEUART_EXP_STOP_BITS> Number of stop bits to use.
// <leuartStopbits1=> 1 stop bits
// <leuartStopbits2=> 2 stop bits
// <i> Default: leuartStopbits1
#define SL_IOSTREAM_LEUART_EXP_STOP_BITS             leuartStopbits1

// <o SL_IOSTREAM_LEUART_EXP_RX_BUFFER_SIZE> Receive buffer size
// <i> Default: 32
#define SL_IOSTREAM_LEUART_EXP_RX_BUFFER_SIZE    32

// <q SL_IOSTREAM_LEUART_EXP_CONVERT_BY_DEFAULT_LF_TO_CRLF> Convert \n to \r\n; It can be changed at runtime using the C API.
// <i> Default: 0
#define SL_IOSTREAM_LEUART_EXP_CONVERT_BY_DEFAULT_LF_TO_CRLF     0

// <q SL_IOSTREAM_LEUART_EXP_RESTRICT_ENERGY_MODE_TO_ALLOW_RECEPTION> Restrict the energy mode to allow the reception.
// <i> Default: 1
// <i> Limits the lowest energy mode the system can sleep to in order to keep the reception on. May cause higher power consumption.
#define SL_IOSTREAM_LEUART_EXP_RESTRICT_ENERGY_MODE_TO_ALLOW_RECEPTION    1

// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <leuart signal=TX,RX> SL_IOSTREAM_LEUART_EXP
// $[LEUART_SL_IOSTREAM_LEUART_EXP]
#define SL_IOSTREAM_LEUART_EXP_PERIPHERAL        LEUART0
#define SL_IOSTREAM_LEUART_EXP_PERIPHERAL_NO     0

// LEUART0 TX on PC14
#define SL_IOSTREAM_LEUART_EXP_TX_PORT           gpioPortC
#define SL_IOSTREAM_LEUART_EXP_TX_PIN            14
#define SL_IOSTREAM_LEUART_EXP_TX_LOC            5

// LEUART0 RX on PC15
#define SL_IOSTREAM_LEUART_EXP_RX_PORT           gpioPortC
#define SL_IOSTREAM_LEUART_EXP_RX_PIN            15
#define SL_IOSTREAM_LEUART_EXP_RX_LOC            5

// [LEUART_SL_IOSTREAM_LEUART_EXP]$
// <<< sl:end pin_tool >>>

#endif
