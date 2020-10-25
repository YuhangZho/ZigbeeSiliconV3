/***************************************************************************//**
 * @file
 * @brief Coexistence configuration header file
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

#ifndef SL_RAIL_UTIL_COEX_CONFIG_H
#define SL_RAIL_UTIL_COEX_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Coexistence Configuration
// <e SL_RAIL_UTIL_COEX_ENABLE> Enable/Disable
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_ENABLE 1
// <q SL_RAIL_UTIL_COEX_TX_ABORT> Abort transmission mid-packet if Grant is lost
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_TX_ABORT 1
// </e>
// </h>

// <h> Radio Hold-off (RHO) Signal Configuration
// <e SL_RAIL_UTIL_COEX_RHO_ENABLED> Enable/Disable
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_RHO_ENABLED 1
// <q SL_RAIL_UTIL_COEX_RHO_ASSERT_LEVEL> Polarity of the RHO signal level
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_RHO_ASSERT_LEVEL 1
// </e>
// </h>

// <h> Priority Signal (Priority) Configuration
// <e SL_RAIL_UTIL_COEX_PRI_ENABLED> Enable/Disable
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_PRI_ENABLED 1
// <q SL_RAIL_UTIL_COEX_PRIORITY_DEFAULT> Default Enabled/Disabled
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_PRIORITY_DEFAULT 1
// <q SL_RAIL_UTIL_COEX_PRI_ASSERT_LEVEL> Polarity of Priority signal level
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_PRI_ASSERT_LEVEL 1
// <q SL_RAIL_UTIL_COEX_PRI_SHARED> Enable PRIORITY shared mode
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_PRI_SHARED 1
// </e>
// </h>

// <h> Directional Priority Signal (DP) Configuration
// <e SL_RAIL_UTIL_COEX_DP_ENABLED> Enable/Disable
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_DP_ENABLED 1
// <o SL_RAIL_UTIL_COEX_DP_PULSE_WIDTH_US> Microseconds to hold DP pulse
// <0-255:1>
// <i> Default: 20
#define SL_RAIL_UTIL_COEX_DP_PULSE_WIDTH_US 20
// </e>
// </h>

// <h> Request Signal (Request) Configuration
// <e SL_RAIL_UTIL_COEX_REQ_ENABLED> Enable/Disable
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_REQ_ENABLED 1
// <o SL_RAIL_UTIL_COEX_REQ_WINDOW> Specify the number of microseconds between asserting Request and starting RX/TX (BLE only)
// <0-255:1>
// <i> Default: 500
#define SL_RAIL_UTIL_COEX_REQ_WINDOW 500
// <q SL_RAIL_UTIL_COEX_REQ_SHARED> Configure the Request signal for shared mode
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_REQ_SHARED 1
// <q SL_RAIL_UTIL_COEX_REQ_ASSERT_LEVEL> Polarity of Request signal level
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_REQ_ASSERT_LEVEL 1
// <o SL_RAIL_UTIL_COEX_REQ_BACKOFF> Maximum back-off time in microseconds after Request was de-asserted
// <0-255:1>
// <i> Default: 15
#define SL_RAIL_UTIL_COEX_REQ_BACKOFF 15
// </e>
// </h>

// <h> PWM Request Signal (PWM Request) Configuration
// <e SL_RAIL_UTIL_COEX_PWM_REQ_ENABLED> Enable/Disable
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_PWM_REQ_ENABLED 1
// <q SL_RAIL_UTIL_COEX_PWM_DEFAULT_ENABLED> Default Enabled/Disabled
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_PWM_DEFAULT_ENABLED 1
// <o SL_RAIL_UTIL_COEX_PWM_REQ_PERIOD> PWM Request Period (5ms to 109ms in 0.5ms steps)
// <5-109>
// <i> Default: 78
#define SL_RAIL_UTIL_COEX_PWM_REQ_PERIOD 78
// <q SL_RAIL_UTIL_COEX_PWM_REQ_ASSERT_LEVEL> Polarity of PWM Request signal level when shared Request enabled
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_PWM_REQ_ASSERT_LEVEL 1
// <q SL_RAIL_UTIL_COEX_PWM_PRIORITY> Assert a high priority when local device is asserting PWM
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_PWM_PRIORITY 1
// <o SL_RAIL_UTIL_COEX_PWM_REQ_DUTYCYCLE> PWM Request Duty-Cycle (1% to 95% in 1% steps)
// <1-95:1>
// <i> Default: 20
#define SL_RAIL_UTIL_COEX_PWM_REQ_DUTYCYCLE 20
// <q SL_RAIL_UTIL_COEX_SCANPWM_DEFAULT_ENABLED> Enable PWM only when local device is scanning
// <i> Default: 0
#define SL_RAIL_UTIL_COEX_SCANPWM_DEFAULT_ENABLED 0
// </e>
// </h>

// <h> Grant Signal (Grant) Configuration
// <e SL_RAIL_UTIL_COEX_GNT_ENABLED> Enable/Disable
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_GNT_ENABLED 1
// <q SL_RAIL_UTIL_COEX_GNT_ASSERT_LEVEL> Polarity of GRANT signal level
// <i> Default: 1
#define SL_RAIL_UTIL_COEX_GNT_ASSERT_LEVEL 1
// </e>
// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>

#if SL_RAIL_UTIL_COEX_GNT_ENABLED
// Pin used for Grant signal
// <gpio optional=true> SL_RAIL_UTIL_COEX_GNT
// $[GPIO_SL_RAIL_UTIL_COEX_GNT]
#define SL_RAIL_UTIL_COEX_GNT_PORT               gpioPortC
#define SL_RAIL_UTIL_COEX_GNT_PIN                9
// [GPIO_SL_RAIL_UTIL_COEX_GNT]$
#ifndef SL_RAIL_UTIL_COEX_GNT_PORT
#error "SL_RAIL_UTIL_COEX_GNT undefined"
#endif //SL_RAIL_UTIL_COEX_GNT_PORT
#endif //SL_RAIL_UTIL_COEX_GNT_ENABLED

#if SL_RAIL_UTIL_COEX_PRI_ENABLED
// Pin used for Priority signal
// <gpio optional=true> SL_RAIL_UTIL_COEX_PRI
// $[GPIO_SL_RAIL_UTIL_COEX_PRI]
#define SL_RAIL_UTIL_COEX_PRI_PORT               gpioPortD
#define SL_RAIL_UTIL_COEX_PRI_PIN                13
// [GPIO_SL_RAIL_UTIL_COEX_PRI]$
#ifndef SL_RAIL_UTIL_COEX_PRI_PORT
#error "SL_RAIL_UTIL_COEX_PRI undefined"
#endif //SL_RAIL_UTIL_COEX_PRI_PORT
#endif //SL_RAIL_UTIL_COEX_PRI_ENABLED

#if SL_RAIL_UTIL_COEX_REQ_ENABLED
// Pin used for Request signal
// <gpio optional=true> SL_RAIL_UTIL_COEX_REQ
// $[GPIO_SL_RAIL_UTIL_COEX_REQ]
#define SL_RAIL_UTIL_COEX_REQ_PORT               gpioPortC
#define SL_RAIL_UTIL_COEX_REQ_PIN                10
// [GPIO_SL_RAIL_UTIL_COEX_REQ]$
#ifndef SL_RAIL_UTIL_COEX_REQ_PORT
#error "SL_RAIL_UTIL_COEX_REQ undefined"
#endif //SL_RAIL_UTIL_COEX_REQ_PORT
#endif //SL_RAIL_UTIL_COEX_REQ_ENABLED

#if SL_RAIL_UTIL_COEX_PWM_REQ_ENABLED
// Pin used for PWM Request signal
// <gpio optional=true> SL_RAIL_UTIL_COEX_PWM_REQ
// $[GPIO_SL_RAIL_UTIL_COEX_PWM_REQ]
#define SL_RAIL_UTIL_COEX_PWM_REQ_PORT           gpioPortC
#define SL_RAIL_UTIL_COEX_PWM_REQ_PIN            11
// [GPIO_SL_RAIL_UTIL_COEX_PWM_REQ]$
#ifndef SL_RAIL_UTIL_COEX_PWM_REQ_PORT
#error "SL_RAIL_UTIL_COEX_PWM_REQ undefined"
#endif //SL_RAIL_UTIL_COEX_PWM_REQ_PORT
#endif //SL_RAIL_UTIL_COEX_PWM_REQ_ENABLED

#if SL_RAIL_UTIL_COEX_RHO_ENABLED
// Pin used for Radio Hold-off signal
// <gpio optional=true> SL_RAIL_UTIL_COEX_RHO
// $[GPIO_SL_RAIL_UTIL_COEX_RHO]
#define SL_RAIL_UTIL_COEX_RHO_PORT               gpioPortC
#define SL_RAIL_UTIL_COEX_RHO_PIN                8
// [GPIO_SL_RAIL_UTIL_COEX_RHO]$
#ifndef SL_RAIL_UTIL_COEX_RHO_PORT
#error "SL_RAIL_UTIL_COEX_RHO undefined"
#endif //SL_RAIL_UTIL_COEX_RHO_PORT
#endif //SL_RAIL_UTIL_COEX_RHO_ENABLED

#if SL_RAIL_UTIL_COEX_DP_ENABLED
// Pin used for Directional Priority signal
// <prs gpio=true optional=true> SL_RAIL_UTIL_COEX_DP
// $[PRS_SL_RAIL_UTIL_COEX_DP]
#define SL_RAIL_UTIL_COEX_DP_CHANNEL             3

// PRS CH3 on PD12
#define SL_RAIL_UTIL_COEX_DP_PORT                gpioPortD
#define SL_RAIL_UTIL_COEX_DP_PIN                 12
// [PRS_SL_RAIL_UTIL_COEX_DP]$

// Directional Priority timer module
// <timer channel=CC0 optional=true> SL_RAIL_UTIL_COEX_DP
// $[TIMER_SL_RAIL_UTIL_COEX_DP]
#define SL_RAIL_UTIL_COEX_DP_PERIPHERAL          TIMER1
#define SL_RAIL_UTIL_COEX_DP_PERIPHERAL_NO       1

#define SL_RAIL_UTIL_COEX_DP_CC0_CHANNEL         1
// [TIMER_SL_RAIL_UTIL_COEX_DP]$
#ifndef SL_RAIL_UTIL_COEX_DP_PORT
#error "SL_RAIL_UTIL_COEX_DP undefined"
#endif //SL_RAIL_UTIL_COEX_DP_PORT
#endif //SL_RAIL_UTIL_COEX_DP_ENABLED

// <<< sl:end pin_tool >>>

#endif // SL_RAIL_UTIL_COEX_CONFIG_H
