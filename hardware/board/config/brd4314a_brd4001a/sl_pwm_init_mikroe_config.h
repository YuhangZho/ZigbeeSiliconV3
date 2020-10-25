#ifndef PWM_INIT_MIKROE_CONFIG_H
#define PWM_INIT_MIKROE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

// <<< Use Configuration Wizard in Context Menu >>>

// <h>PWM configuration

// <o SL_PWM_MIKROE_FREQUENCY> PWM frequency [Hz]
// <i> Default: 10000
#define SL_PWM_MIKROE_FREQUENCY       10000

// <o SL_PWM_MIKROE_POLARITY> Polarity
// <PWM_ACTIVE_HIGH=> Active high
// <PWM_ACTIVE_LOW=> Active low
// <i> Default: PWM_ACTIVE_HIGH
#define SL_PWM_MIKROE_POLARITY        PWM_ACTIVE_HIGH
// </h> end pwm configuration

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>

// <timer channel=OUTPUT> SL_PWM_MIKROE
// $[TIMER_SL_PWM_MIKROE]
#define SL_PWM_MIKROE_PERIPHERAL                 TIMER4
#define SL_PWM_MIKROE_PERIPHERAL_NO              4

#define SL_PWM_MIKROE_OUTPUT_CHANNEL             0
// TIMER4 CC0 on PB04
#define SL_PWM_MIKROE_OUTPUT_PORT                gpioPortB
#define SL_PWM_MIKROE_OUTPUT_PIN                 4

// [TIMER_SL_PWM_MIKROE]$

// <<< sl:end pin_tool >>>

#ifdef __cplusplus
}
#endif

#endif // PWM_INIT_MIKROE_CONFIG_H
