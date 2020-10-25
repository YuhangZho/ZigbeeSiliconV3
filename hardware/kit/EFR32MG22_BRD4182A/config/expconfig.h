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

#define BSP_EXP_HEADER4_PIN    0
#define BSP_EXP_HEADER4_PORT   gpioPortC
#define BSP_EXP_HEADER6_PIN    1
#define BSP_EXP_HEADER6_PORT   gpioPortC
#define BSP_EXP_HEADER7_PIN    0
#define BSP_EXP_HEADER7_PORT   gpioPortB
#define BSP_EXP_HEADER8_PIN    2
#define BSP_EXP_HEADER8_PORT   gpioPortC
#define BSP_EXP_HEADER9_PIN    1
#define BSP_EXP_HEADER9_PORT   gpioPortB
#define BSP_EXP_HEADER10_PIN   3
#define BSP_EXP_HEADER10_PORT  gpioPortC
#define BSP_EXP_HEADER11_PIN   2
#define BSP_EXP_HEADER11_PORT  gpioPortD
#define BSP_EXP_HEADER12_PIN   5
#define BSP_EXP_HEADER12_PORT  gpioPortA
#define BSP_EXP_HEADER13_PIN   3
#define BSP_EXP_HEADER13_PORT  gpioPortD
#define BSP_EXP_HEADER14_PIN   6
#define BSP_EXP_HEADER14_PORT  gpioPortA
#define BSP_EXP_HEADER15_PIN   2
#define BSP_EXP_HEADER15_PORT  gpioPortB
#define BSP_EXP_HEADER16_PIN   3
#define BSP_EXP_HEADER16_PORT  gpioPortB

#define BSP_EXP_I2C            I2C0
#define BSP_EXP_I2C_SCL_PIN    BSP_EXP_HEADER15_PIN
#define BSP_EXP_I2C_SCL_PORT   BSP_EXP_HEADER15_PORT
#define BSP_EXP_I2C_SDA_PIN    BSP_EXP_HEADER16_PIN
#define BSP_EXP_I2C_SDA_PORT   BSP_EXP_HEADER16_PORT

#define BSP_EXP_SPI            USART0
#define BSP_EXP_SPI_CLK_PIN    BSP_EXP_HEADER8_PIN
#define BSP_EXP_SPI_CLK_PORT   BSP_EXP_HEADER8_PORT
#define BSP_EXP_SPI_CS_PIN     BSP_EXP_HEADER10_PIN
#define BSP_EXP_SPI_CS_PORT    BSP_EXP_HEADER10_PORT
#define BSP_EXP_SPI_MISO_PIN   BSP_EXP_HEADER6_PIN
#define BSP_EXP_SPI_MISO_PORT  BSP_EXP_HEADER6_PORT
#define BSP_EXP_SPI_MOSI_PIN   BSP_EXP_HEADER4_PIN
#define BSP_EXP_SPI_MOSI_PORT  BSP_EXP_HEADER4_PORT

#define BSP_EXP_USART          USART1
#define BSP_EXP_USART_RX_PIN   BSP_EXP_HEADER14_PIN
#define BSP_EXP_USART_RX_PORT  BSP_EXP_HEADER14_PORT
#define BSP_EXP_USART_TX_PIN   BSP_EXP_HEADER12_PIN
#define BSP_EXP_USART_TX_PORT  BSP_EXP_HEADER12_PORT

#endif /* EXPCONFIG_H */
