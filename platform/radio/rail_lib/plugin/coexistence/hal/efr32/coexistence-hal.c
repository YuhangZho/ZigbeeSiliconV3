/***************************************************************************//**
 * @file
 * @brief Radio coexistence EFR32 utilities
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

#include "em_core.h"
#include "em_cmu.h"
#include "em_prs.h"
#include "coexistence-hal.h"

#ifdef SL_RAIL_UTIL_COEX_PHY_SELECT_PORT
static COEX_HAL_GpioConfig_t phySelectCfg = {
  .port = SL_RAIL_UTIL_COEX_PHY_SELECT_PORT,
  .pin = SL_RAIL_UTIL_COEX_PHY_SELECT_PIN,
  .intNo = SL_RAIL_UTIL_COEX_PHY_SELECT_INTNO,
  .polarity = SL_RAIL_UTIL_COEX_PHY_SELECT_ASSERT_LEVEL
};
#endif //SL_RAIL_UTIL_COEX_PHY_SELECT_PORT

#ifdef SL_RAIL_UTIL_COEX_GNT_PORT
static COEX_HAL_GpioConfig_t ptaGntCfg = {
  .port = SL_RAIL_UTIL_COEX_GNT_PORT,
  .pin = SL_RAIL_UTIL_COEX_GNT_PIN,
  .intNo = SL_RAIL_UTIL_COEX_GNT_INTNO,
  .polarity = SL_RAIL_UTIL_COEX_GNT_ASSERT_LEVEL
};
#endif //SL_RAIL_UTIL_COEX_GNT_PORT

#ifdef SL_RAIL_UTIL_COEX_PRI_PORT
static COEX_HAL_GpioConfig_t ptaPriCfg = {
  .port = SL_RAIL_UTIL_COEX_PRI_PORT,
  .pin = SL_RAIL_UTIL_COEX_PRI_PIN,
  .polarity = SL_RAIL_UTIL_COEX_PRI_ASSERT_LEVEL
};
#endif //SL_RAIL_UTIL_COEX_PRI_PORT

#ifdef SL_RAIL_UTIL_COEX_REQ_PORT
static COEX_HAL_GpioConfig_t ptaReqCfg = {
  .port = SL_RAIL_UTIL_COEX_REQ_PORT,
  .pin = SL_RAIL_UTIL_COEX_REQ_PIN,
  .intNo = SL_RAIL_UTIL_COEX_REQ_INTNO,
  .polarity = SL_RAIL_UTIL_COEX_REQ_ASSERT_LEVEL
};
#endif //SL_RAIL_UTIL_COEX_REQ_PORT

#ifdef SL_RAIL_UTIL_COEX_PWM_REQ_PORT
static COEX_HAL_GpioConfig_t ptaPwmReqCfg = {
  .port = SL_RAIL_UTIL_COEX_PWM_REQ_PORT,
  .pin = SL_RAIL_UTIL_COEX_PWM_REQ_PIN,
  .intNo = SL_RAIL_UTIL_COEX_PWM_REQ_INTNO,
  .polarity = SL_RAIL_UTIL_COEX_PWM_REQ_ASSERT_LEVEL
};
#endif //SL_RAIL_UTIL_COEX_PWM_REQ_PORT

#ifdef SL_RAIL_UTIL_COEX_RHO_PORT
static COEX_HAL_GpioConfig_t rhoCfg = {
  .port = SL_RAIL_UTIL_COEX_RHO_PORT,
  .pin = SL_RAIL_UTIL_COEX_RHO_PIN,
  .intNo = SL_RAIL_UTIL_COEX_RHO_INTNO,
  .polarity = SL_RAIL_UTIL_COEX_RHO_ASSERT_LEVEL
};
#endif //SL_RAIL_UTIL_COEX_RHO_PORT

#define GPIO_FLAG(x) (1ul << x)

static void (*reqCallback)(void) = NULL;
static void (*gntCallback)(void) = NULL;
static void (*rhoCallback)(void) = NULL;
static void (*phySelectCallback)(void) = NULL;

static void COEX_HAL_REQ_ISR(uint8_t pin)
{
  (void)pin;
  reqCallback();
}

static void COEX_HAL_GNT_ISR(uint8_t pin)
{
  (void)pin;
  gntCallback();
}

static void COEX_HAL_RHO_ISR(uint8_t pin)
{
  (void)pin;
  rhoCallback();
}

static void COEX_HAL_PHY_SELECT_ISR(uint8_t pin)
{
  (void)pin;
  phySelectCallback();
}

static void setGpioConfig(COEX_GpioHandle_t gpioHandle)
{
  if (gpioHandle != NULL) {
    COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;

    GPIO_PinModeSet((GPIO_Port_TypeDef)gpio->port,
                    gpio->pin,
                    (GPIO_Mode_TypeDef)gpio->mode,
                    GPIO_PinOutGet((GPIO_Port_TypeDef)gpio->port,
                                   gpio->pin));
  }
}

static bool isGpioInSet(COEX_GpioHandle_t gpioHandle, bool defaultValue);

static void enableGpioInt(COEX_GpioHandle_t gpioHandle,
                          bool enabled,
                          bool *wasAsserted)
{
  if (gpioHandle != NULL) {
    COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;
    COEX_GpioConfig_t *coexGpio = &(gpio->config);

    bool intAsserted = (coexGpio->options & COEX_GPIO_OPTION_INT_ASSERTED) != 0U;
    bool intDeasserted = (coexGpio->options & COEX_GPIO_OPTION_INT_DEASSERTED) != 0U;

    if (enabled) {
      // Disable triggering and clear any stale events
      GPIO_ExtIntConfig((GPIO_Port_TypeDef)gpio->port,
                        gpio->pin,
                        gpio->intNo,
                        false,
                        false,
                        false);
      if (wasAsserted != NULL) {
        *wasAsserted = false; // Ensures we won't miss GNT assertion
      }
      // Register callbacks before setting up and enabling pin interrupt
      GPIOINT_CallbackRegister(gpio->intNo, gpio->isr);
      // Enable both edges' interrupt
      GPIO_ExtIntConfig((GPIO_Port_TypeDef)gpio->port,
                        gpio->pin,
                        gpio->intNo,
                        gpio->polarity ? intAsserted : intDeasserted,
                        gpio->polarity ? intDeasserted : intAsserted,
                        true);
    } else {
      GPIO_IntDisable(GPIO_FLAG(gpio->intNo));
      GPIO_IntClear(GPIO_FLAG(gpio->intNo));
    }
  }
}

static void setGpio(COEX_GpioHandle_t gpioHandle, bool enabled)
{
  if (gpioHandle != NULL) {
    COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;

    if (enabled == gpio->polarity) {
      GPIO_PinOutSet((GPIO_Port_TypeDef)gpio->port, gpio->pin);
    } else {
      GPIO_PinOutClear((GPIO_Port_TypeDef)gpio->port, gpio->pin);
    }
  }
}

#ifndef SL_RAIL_UTIL_COEX_GPIO_PULL_UP_DOWN
#define SL_RAIL_UTIL_COEX_GPIO_PULL_UP_DOWN 1
#endif //SL_RAIL_UTIL_COEX_GPIO_PULL_UP_DOWN

#if SL_RAIL_UTIL_COEX_GPIO_PULL_UP_DOWN
#define GPIO_CONFIG_AND (gpioModeWiredAndPullUp)
#define GPIO_CONFIG_OR  (gpioModeWiredOrPullDown)
#else //SL_RAIL_UTIL_COEX_GPIO_PULL_UP_DOWN
#define GPIO_CONFIG_AND (gpioModeWiredAnd)
#define GPIO_CONFIG_OR  (gpioModeWiredOr)
#endif //SL_RAIL_UTIL_COEX_GPIO_PULL_UP_DOWN

static void configGpio(COEX_GpioHandle_t gpioHandle, COEX_GpioConfig_t *coexGpio)
{
  if (gpioHandle != NULL) {
    COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;
    bool defaultAsserted = (coexGpio->options & COEX_GPIO_OPTION_DEFAULT_ASSERTED) != 0U;
    gpio->config = *coexGpio;

    if ((coexGpio->options & COEX_GPIO_OPTION_SHARED) != 0U) {
      gpio->mode = gpio->polarity ? GPIO_CONFIG_OR : GPIO_CONFIG_AND;
    } else if ((coexGpio->options & COEX_GPIO_OPTION_OUTPUT) != 0U) {
      gpio->mode = gpioModePushPull;
    } else {
      gpio->mode = gpioModeInputPull;
    }
    setGpioConfig(gpio);
    setGpio(gpio, defaultAsserted);
  }
}

static void setGpioFlag(COEX_GpioHandle_t gpioHandle, bool enabled)
{
  if (gpioHandle != NULL) {
    COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;

    if (enabled) {
      GPIO_IntSet(GPIO_FLAG(gpio->intNo));
    } else {
      GPIO_IntClear(GPIO_FLAG(gpio->intNo));
    }
  }
}

static bool isGpioOutSet(COEX_GpioHandle_t gpioHandle, bool defaultValue)
{
  if (gpioHandle != NULL) {
    COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;
    return !!GPIO_PinOutGet((GPIO_Port_TypeDef)gpio->port,
                            gpio->pin) == !!gpio->polarity;
  } else {
    return defaultValue;
  }
}

static bool isGpioInSet(COEX_GpioHandle_t gpioHandle, bool defaultValue)
{
  if (gpioHandle != NULL) {
    COEX_HAL_GpioConfig_t *gpio = (COEX_HAL_GpioConfig_t*)gpioHandle;
#if SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
    return COEX_GetGpioInputOverride(gpio->config.index);
#else //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
    return !!GPIO_PinInGet((GPIO_Port_TypeDef)gpio->port,
                           gpio->pin) == !!gpio->polarity;
#endif //SL_RAIL_UTIL_COEX_OVERRIDE_GPIO_INPUT
  } else {
    return defaultValue;
  }
}

static const COEX_HalCallbacks_t coexHalCallbacks = {
  .setGpio = &setGpio,
  .setGpioFlag = &setGpioFlag,
  .enableGpioInt = &enableGpioInt,
  .configGpio = &configGpio,
  .isGpioOutSet = &isGpioOutSet,
  .isGpioInSet = &isGpioInSet
};

static void GPIOINT_InitSafe(void)
{
  // Enable GPIO clock for configuring interrupts
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Turn on GPIO interrupts only if they weren't enabled elsewhere
  if (CORE_NvicIRQDisabled(GPIO_ODD_IRQn)
      || CORE_NvicIRQDisabled(GPIO_EVEN_IRQn)) {
    GPIOINT_Init();
  }
}

void COEX_HAL_CallAtomic(COEX_AtomicCallback_t cb, void *arg)
{
  CORE_CRITICAL_SECTION((*cb)(arg); )
}

bool COEX_HAL_ConfigPhySelect(COEX_HAL_GpioConfig_t *gpioConfig)
{
  bool status = false;

  gpioConfig->isr = &COEX_HAL_PHY_SELECT_ISR;
  status = COEX_ConfigPhySelect(gpioConfig);
  if (status) {
    phySelectCallback = gpioConfig->config.cb;
  }
  return status;
}

bool COEX_HAL_ConfigRequest(COEX_HAL_GpioConfig_t *gpioConfig)
{
  bool status = false;

  gpioConfig->isr = &COEX_HAL_REQ_ISR;
  status = COEX_ConfigRequest(gpioConfig);
  if (status) {
    reqCallback = gpioConfig->config.cb;
  }
  return status;
}

bool COEX_HAL_ConfigRadioHoldOff(COEX_HAL_GpioConfig_t *gpioConfig)
{
  bool status = false;

  gpioConfig->isr = &COEX_HAL_RHO_ISR;
  status = COEX_ConfigRadioHoldOff(gpioConfig);
  if (status) {
    rhoCallback = gpioConfig->config.cb;
  }
  return status;
}

#ifdef SL_RAIL_UTIL_COEX_RX_ACTIVE_PORT
bool COEX_HAL_ConfigRxActive(void)
{
  CMU_ClockEnable(cmuClock_PRS, true);
#ifdef _SILICON_LABS_32B_SERIES_1
#if SL_RAIL_UTIL_COEX_RX_ACTIVE_ASSERT_LEVEL
  PRS->CH[SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL].CTRL = PRS_CH_CTRL_INV;
#endif //SL_RAIL_UTIL_COEX_RX_ACTIVE_ASSERT_LEVEL
  volatile uint32_t * routeRegister;
  // Configure rx active PRS output to selected channel and location
  if (SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL < 4) {
    routeRegister = &PRS->ROUTELOC0;
  } else if (SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL < 8) {
    routeRegister = &PRS->ROUTELOC1;
  } else if (SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL < 12) {
    routeRegister = &PRS->ROUTELOC2;
  } else {
    return false; // error
  }
  PRS_SourceAsyncSignalSet(SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL,
                           0U,
                           PRS_MODEM_FRAMEDET);
  // Route PRS CH/LOC to RX_ACTIVE GPIO output
  BUS_RegMaskedWrite(routeRegister,
                     0xFFU << ((SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL & 3) * 8),
                     SL_RAIL_UTIL_COEX_RX_ACTIVE_LOC << ((SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL & 3) * 8));
  BUS_RegMaskedSet(&PRS->ROUTEPEN, (1u << SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL));
#else //!_SILICON_LABS_32B_SERIES_1
  PRS_SourceAsyncSignalSet(SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL,
                           0U,
                           PRS_ASYNC_MODEML_FRAMEDET);
  PRS_Combine(SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL,
              WRAP_PRS_ASYNC(SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL - 1),
              (SL_RAIL_UTIL_COEX_RX_ACTIVE_ASSERT_LEVEL != 0U)
              ? prsLogic_A : prsLogic_NOT_A);
  PRS_PinOutput(SL_RAIL_UTIL_COEX_RX_ACTIVE_CHANNEL,
                prsTypeAsync,
                SL_RAIL_UTIL_COEX_RX_ACTIVE_PORT,
                SL_RAIL_UTIL_COEX_RX_ACTIVE_PIN);
#endif //_SILICON_LABS_32B_SERIES_1
  GPIO_PinModeSet(SL_RAIL_UTIL_COEX_RX_ACTIVE_PORT,
                  SL_RAIL_UTIL_COEX_RX_ACTIVE_PIN,
                  gpioModePushPull,
                  0);
  return true;
}
#endif //SL_RAIL_UTIL_COEX_RX_ACTIVE_PORT

bool COEX_HAL_ConfigPriority(COEX_HAL_GpioConfig_t *gpioConfig)
{
  return COEX_ConfigPriority(gpioConfig);
}

bool COEX_HAL_ConfigGrant(COEX_HAL_GpioConfig_t *gpioConfig)
{
  bool status = false;

  gpioConfig->isr = &COEX_HAL_GNT_ISR;
  status = COEX_ConfigGrant(gpioConfig);
  if (status) {
    gntCallback = gpioConfig->config.cb;
  }
  return status;
}

bool COEX_HAL_ConfigPwmRequest(COEX_HAL_GpioConfig_t *gpioConfig)
{
  return COEX_ConfigPwmRequest(gpioConfig);
}

void COEX_HAL_Init(void)
{
  COEX_SetHalCallbacks(&coexHalCallbacks);
  COEX_InitHalConfigOptions();
  GPIOINT_InitSafe();

  #ifdef SL_RAIL_UTIL_COEX_RX_ACTIVE_PORT
  COEX_HAL_ConfigRxActive();
  #endif //SL_RAIL_UTIL_COEX_RX_ACTIVE_PORT
  #ifdef SL_RAIL_UTIL_COEX_PHY_SELECT_PORT
  COEX_HAL_ConfigPhySelect(&phySelectCfg);
  #endif //SL_RAIL_UTIL_COEX_PHY_ENABLE_PORT
  #ifdef SL_RAIL_UTIL_COEX_REQ_PORT
  COEX_HAL_ConfigRequest(&ptaReqCfg);
  #endif //SL_RAIL_UTIL_COEX_REQ_PORT
  #ifdef SL_RAIL_UTIL_COEX_PRI_PORT
  COEX_HAL_ConfigPriority(&ptaPriCfg);
  #endif //SL_RAIL_UTIL_COEX_PRI_PORT
  #ifdef SL_RAIL_UTIL_COEX_PWM_REQ_PORT
  COEX_HAL_ConfigPwmRequest(&ptaPwmReqCfg);
  #endif //SL_RAIL_UTIL_COEX_PWM_REQ_PORT
  #ifdef SL_RAIL_UTIL_COEX_GNT_PORT
  COEX_HAL_ConfigGrant(&ptaGntCfg);
  #endif //SL_RAIL_UTIL_COEX_GNT_PORT
  #ifdef SL_RAIL_UTIL_COEX_RHO_PORT
  COEX_HAL_ConfigRadioHoldOff(&rhoCfg);
  #endif //SL_RAIL_UTIL_COEX_RHO_PORT
}
