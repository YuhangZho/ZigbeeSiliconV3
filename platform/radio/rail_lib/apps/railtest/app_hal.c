/***************************************************************************//**
 * @file
 * @brief This file handles the hardware interactions for RAILtest
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <stdio.h>

#include "rail.h"

#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"

#include "gpiointerrupt.h"
#include "hal_common.h"

#include "app_common.h"
#include "sl_rail_test_config.h"

#if defined(SL_RAIL_TEST_GRAPHICS_SUPPORT_ENABLE)
#include "sl_rail_test_graphics.h"
#endif

#if defined(SL_RAIL_TEST_LED_SUPPORT_ENABLE)
#include "sl_simple_led_instances.h"
#endif

#if defined(SL_RAIL_TEST_BUTTON_SUPPORT_ENABLE)
#include "sl_simple_button_instances.h"
#endif

#if defined(SL_CATALOG_IOSTREAM_USART_PRESENT)
#include "sl_iostream_usart_vcom_config.h"
#endif

volatile bool serEvent = false;

static void gpioSerialWakeupCallback(uint8_t pin)
{
  if (pin == SL_IOSTREAM_USART_VCOM_RX_PIN) {
    serEvent = true;
  }
}

/******************************************************************************
 * Application HAL Initialization
 *****************************************************************************/
void appHalInit(void)
{
#if !defined(SL_RAIL_UTIL_IC_SIMULATION_BUILD)
#if defined (SL_RAIL_TEST_PER_PORT) && defined(SL_RAIL_TEST_PER_PIN)
  // For PER test
  GPIO_PinModeSet(SL_RAIL_TEST_PER_PORT, SL_RAIL_TEST_PER_PIN, gpioModePushPull, 1);
#endif // SL_RAIL_TEST_PER_PORT && SL_RAIL_TEST_PER_PIN

  // For 'sleep'
  GPIOINT_Init();
  GPIOINT_CallbackRegister(SL_IOSTREAM_USART_VCOM_RX_PIN, gpioSerialWakeupCallback);
#endif
}

void PeripheralDisable(void)
{
  LedsDisable();
  disableGraphics();
  // Disable the buttons on the board
  deinitButtons();
}

void PeripheralEnable(void)
{
  enableGraphics();
  updateGraphics();
  // Enable the buttons on the board
  initButtons();
}

/**
 * Delay for the specified number of microseconds by busy waiting.
 * @param microseconds Time to delay for in microseconds.
 */
void usDelay(uint32_t microseconds)
{
  uint32_t start = RAIL_GetTime();
  while ((RAIL_GetTime() - start) < microseconds) {
  }
}

void serialWaitForTxIdle(void)
{
  // This is grody and not very serial-independent... but we need to
  // wait for the serial output to have completely cleared the UART
  // before sleeping, and this beats a wishful-thinking worst-case
  // timed delay.
  while ((USART_StatusGet(SL_IOSTREAM_USART_VCOM_PERIPHERAL)
          & USART_STATUS_TXIDLE) == 0) {
  }
}

/******************************************************************************
 * Configurable Peripherals
 *****************************************************************************/

// Graphics
#if defined(SL_RAIL_TEST_GRAPHICS_SUPPORT_ENABLE)

void disableGraphics(void)
{
  sl_rail_test_graphics_clear();
  sl_rail_test_graphics_sleep();

#if defined(HAL_VCOM_ENABLE) && defined(BSP_VCOM_ENABLE_PORT)
  // Some boards use the same pin for VCOM and the display so re-init that pin
  // here just to be safe.
  GPIO_PinModeSet(BSP_VCOM_ENABLE_PORT, BSP_VCOM_ENABLE_PIN, gpioModePushPull, 1);
#endif
}

void enableGraphics(void)
{
  sl_rail_test_graphics_wakeup();
}

// Update any LCD text that should be changed
void updateGraphics(void)
{
  if (logLevel & PERIPHERAL_ENABLE) {
    // Force a redraw
    sl_rail_test_graphics_update();
  }
}

#else

void updateGraphics(void)
{
}
void disableGraphics(void)
{
}
void enableGraphics(void)
{
}

#endif // defined(SL_RAIL_TEST_GRAPHICS_SUPPORT_ENABLE)

// LED's
#if defined(SL_RAIL_TEST_LED_SUPPORT_ENABLE)

static sl_led_t const *sl_leds[] = { &sl_led_led0, &sl_led_led1 };

void LedSet(int led)
{
  if (logLevel & PERIPHERAL_ENABLE) {
    sl_led_turn_on(sl_leds[led]);
  }
}

void LedToggle(int led)
{
  if (logLevel & PERIPHERAL_ENABLE) {
    sl_led_toggle(sl_leds[led]);
  }
}

void LedsDisable(void)
{
  sl_led_turn_off(&sl_led_led0);
  sl_led_turn_off(&sl_led_led1);
}

#else

void LedSet(int led)
{
  (void)led;
}
void LedToggle(int led)
{
  (void)led;
}
void LedsDisable(void)
{
}

#endif // defined(SL_RAIL_TEST_LED_SUPPORT_ENABLE)

// Buttons
#if defined(SL_RAIL_TEST_BUTTON_SUPPORT_ENABLE)

extern const sl_button_t *simple_button_array[];
extern const uint8_t simple_button_count;

// Holds Enable/Disable status of the buttons on the board
static bool initButtonStatus = true;

void deinitButtons(void)
{
  initButtonStatus = false;
}

void initButtons(void)
{
  initButtonStatus = true;
}

void gpio0LongPress(void)
{
  radioTransmit(0, NULL);
}

void gpio0ShortPress(void)
{
  radioTransmit(1, NULL);
}

void gpio1LongPress(void)
{
}

void gpio1ShortPress(void)
{
  if (!inAppMode(NONE, NULL) || inRadioState(RAIL_RF_STATE_TX, NULL)) {
    return;
  }

  if (inRadioState(RAIL_RF_STATE_RX, NULL)) {
    RAIL_Idle(railHandle, RAIL_IDLE_ABORT, false);
  }

  // Check if next channel exists
  if (RAIL_IsValidChannel(railHandle, channel + 1)
      == RAIL_STATUS_NO_ERROR) {
    channel++;
  } else {
    // Find initial channel
    channel = 0;
    while (RAIL_IsValidChannel(railHandle, channel)
           != RAIL_STATUS_NO_ERROR) {
      channel++;
    }
  }

  changeChannel(channel);
}

// This is the reimplementation of a WEAK function in button.c.
void sl_button_on_change(const sl_button_t *handle)
{
  if (initButtonStatus) {
    #define GET_TIME_IN_MS() (RAIL_GetTime() / 1000)
    // Todo: Reimplement this code. Define the number of buttons here due to
    // not being able to use the extern variable simple_button_count.
    #define INTERNAL_BUTTON_NUM 2

    static uint32_t gpioTimeCapture[INTERNAL_BUTTON_NUM];
    // Hold true if a Negative Edge is encountered for the button press
    static bool gpioNegEdge[INTERNAL_BUTTON_NUM];

    void(*gpioLongPress_arr[])(void) = { gpio0LongPress, gpio1LongPress };
    void(*gpioShortPress_arr[])(void) = { gpio0ShortPress, gpio1ShortPress };

    for (uint8_t i = 0; i < simple_button_count; i++) {
      if (simple_button_array[i] == handle) {
        if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
          // Negative Edge
          gpioTimeCapture[i] = GET_TIME_IN_MS();
          gpioNegEdge[i] = true;
        } else {
          // Positive Edge with a preceeding Negative Edge
          if (gpioNegEdge[i]) {
            gpioNegEdge[i] = false;
            if ((elapsedTimeInt32u(gpioTimeCapture[i], GET_TIME_IN_MS())
                 > SL_RAIL_TEST_BUTTON_LONG_HOLD_DURATION_MS)) {
              (*gpioLongPress_arr[i])();
            } else {
              (*gpioShortPress_arr[i])();
            }
          }
        }
      }
    }
  } // initButtonStatus
}

#else

void deinitButtons(void)
{
}
void initButtons(void)
{
}
void gpio0LongPress(void)
{
}
void gpio0ShortPress(void)
{
}
void gpio1LongPress(void)
{
}
void gpio1ShortPress(void)
{
}

#endif // defined(SL_RAIL_TEST_BUTTON_SUPPORT_ENABLE)
