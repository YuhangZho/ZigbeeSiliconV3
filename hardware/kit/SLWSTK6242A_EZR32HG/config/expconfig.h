/***************************************************************************//**
 * @file
 * @brief Provide EXP header configuration parameters.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef EXPCONFIG_H
#define EXPCONFIG_H

#define BSP_EXP_HEADER3_PIN      2
#define BSP_EXP_HEADER3_PIN      gpioPortF
#define BSP_EXP_HEADER4_PIN      10
#define BSP_EXP_HEADER4_PORT     gpioPortE
#define BSP_EXP_HEADER5_PIN      4
#define BSP_EXP_HEADER5_PIN      gpioPortF
#define BSP_EXP_HEADER6_PIN      11
#define BSP_EXP_HEADER6_PORT     gpioPortE
#define BSP_EXP_HEADER7_PIN      9
#define BSP_EXP_HEADER7_PIN      gpioPortC
#define BSP_EXP_HEADER8_PIN      12
#define BSP_EXP_HEADER8_PORT     gpioPortE
#define BSP_EXP_HEADER9_PIN      10
#define BSP_EXP_HEADER9_PIN      gpioPortC
#define BSP_EXP_HEADER10_PIN     13
#define BSP_EXP_HEADER10_PORT    gpioPortE
#define BSP_EXP_HEADER11_PIN     11
#define BSP_EXP_HEADER11_PIN     gpioPortB
#define BSP_EXP_HEADER12_PIN     4
#define BSP_EXP_HEADER12_PORT    gpioPortD
#define BSP_EXP_HEADER14_PIN     5
#define BSP_EXP_HEADER14_PORT    gpioPortD
#define BSP_EXP_HEADER15_PIN     7
#define BSP_EXP_HEADER15_PIN     gpioPortD
#define BSP_EXP_HEADER16_PIN     6
#define BSP_EXP_HEADER16_PORT    gpioPortD

#define BSP_EXP_I2C              I2C0
#define BSP_EXP_I2C_ROUTE_LOC    _I2C_ROUTE_LOCATION_LOC1
#define BSP_EXP_I2C_SCL_PIN      BSP_EXP_HEADER15_PIN
#define BSP_EXP_I2C_SCL_PORT     BSP_EXP_HEADER15_PORT
#define BSP_EXP_I2C_SDA_PIN      BSP_EXP_HEADER16_PIN
#define BSP_EXP_I2C_SDA_PORT     BSP_EXP_HEADER16_PORT

#define BSP_EXP_SPI              USART0
#define BSP_EXP_SPI_ROUTE_LOC    _USART_ROUTE_LOCATION_LOC0
#define BSP_EXP_SPI_CLK_PIN      BSP_EXP_HEADER8_PIN
#define BSP_EXP_SPI_CLK_PORT     BSP_EXP_HEADER8_PORT
#define BSP_EXP_SPI_CS_PIN       BSP_EXP_HEADER10_PIN
#define BSP_EXP_SPI_CS_PORT      BSP_EXP_HEADER10_PORT
#define BSP_EXP_SPI_MISO_PIN     BSP_EXP_HEADER6_PIN
#define BSP_EXP_SPI_MISO_PORT    BSP_EXP_HEADER6_PORT
#define BSP_EXP_SPI_MOSI_PIN     BSP_EXP_HEADER4_PIN
#define BSP_EXP_SPI_MOSI_PORT    BSP_EXP_HEADER4_PORT

#define BSP_EXP_USART            LEUART0
#define BSP_EXP_USART_ROUTE_LOC  _LEUART_ROUTE_LOCATION_LOC0
#define BSP_EXP_USART_CTS_PIN    BSP_EXP_HEADER3_PIN
#define BSP_EXP_USART_CTS_PORT   BSP_EXP_HEADER3_PORT
#define BSP_EXP_USART_RTS_PIN    BSP_EXP_HEADER5_PIN
#define BSP_EXP_USART_RTS_PORT   BSP_EXP_HEADER5_PORT
#define BSP_EXP_USART_RX_PIN     BSP_EXP_HEADER14_PIN
#define BSP_EXP_USART_RX_PORT    BSP_EXP_HEADER14_PORT
#define BSP_EXP_USART_TX_PIN     BSP_EXP_HEADER12_PIN
#define BSP_EXP_USART_TX_PORT    BSP_EXP_HEADER12_PORT

#endif /* EXPCONFIG_H */
