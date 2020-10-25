/***************************************************************************//**
 * @file
 * @brief Board HW abstraction header for BRD4166A
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

#ifndef BOARD_H
#define BOARD_H

#define BOARD_BUTTON0_EM4WUEN_MASK 0x10           /**< Mask to enable EM4 wake-up BTN0 */

#define BOARD_RGBLED_PRESENT       1              /**< RGB LED present on board        */
#define BOARD_RGBLED_PWR_EN_PORT   gpioPortJ      /**< RGB LED Power Enable port       */
#define BOARD_RGBLED_PWR_EN_PIN    14             /**< RGB LED Power Enable pin        */
#define BOARD_RGBLED_COM_PORT      gpioPortI      /**< RGB LED COM port                */
#define BOARD_RGBLED_COM0_PORT     gpioPortI      /**< RGB LED COM0 port               */
#define BOARD_RGBLED_COM0_PIN      0              /**< RGB LED COM0 pin                */
#define BOARD_RGBLED_COM1_PORT     gpioPortI      /**< RGB LED COM1 port               */
#define BOARD_RGBLED_COM1_PIN      1              /**< RGB LED COM1 pin                */
#define BOARD_RGBLED_COM2_PORT     gpioPortI      /**< RGB LED COM2 port               */
#define BOARD_RGBLED_COM2_PIN      2              /**< RGB LED COM2 pin                */
#define BOARD_RGBLED_COM3_PORT     gpioPortI      /**< RGB LED COM3 port               */
#define BOARD_RGBLED_COM3_PIN      3              /**< RGB LED COM3 pin                */

void rgb_led_init(void);
void rgb_led_deinit(void);
void rgb_led_set(uint8_t m, uint8_t r, uint8_t g, uint8_t b);

#endif // BOARD_H
