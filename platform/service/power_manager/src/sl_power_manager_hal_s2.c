/***************************************************************************//**
 * @file
 * @brief Power Manager HAL API implementation.
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

#include "em_device.h"
#if defined(_SILICON_LABS_32B_SERIES_2)
#include "em_emu.h"
#include "em_cmu.h"
#include "em_assert.h"
#include "sl_power_manager_config.h"
#include "sl_power_manager.h"
#include "sli_power_manager_private.h"
#include "sl_sleeptimer.h"
#include "sli_sleeptimer.h"

#include <stdbool.h>

/*******************************************************************************
 *********************************   DEFINES   *********************************
 ******************************************************************************/

// Time required by the hardware to come out of EM2 in microseconds.
// This value includes HW startup, emlib and sleepdrv execution time.
// Voltage scaling, HFXO startup and HFXO steady times are excluded from
// this because they are handled separately. RTCCSYNC time is also
// excluded and it is handled by RTCCSYNC code itself.
//TODO need to validate this value. how?
#if (_SILICON_LABS_32B_SERIES_2_CONFIG == 1)
#define EM2_WAKEUP_PROCESS_TIME_OVERHEAD_US (100u) //(380u)
#else // (_SILICON_LABS_32B_SERIES_2_CONFIG == 2),
#define EM2_WAKEUP_PROCESS_TIME_OVERHEAD_US (100u) //(345u)
#endif

// DPLL Locking delay related defines
#define DPLL_COARSECOUNT_VALUE  (5u)

// Time it takes to upscale voltage after EM2 in microseconds.
#define EM2_WAKEUP_VSCALE_OVERHEAD_US (35u)

// Default time value in microseconds required to wake-up the hfxo oscillator.
#define HFXO_WAKE_UP_TIME_DEFAULT_VALUE_US  (600u)

// high frequency oscillator wake-up time margin for possible variation
// A shift by 3 will be like a division by 8, so a percentage of 12.5%.
#define HFXO_START_UP_TIME_OVERHEAD_LOG2   3

// Default time value in microseconds for the HFXO minimum off time.
#define HFXO_MINIMUM_OFFTIME_DEFAULT_VALUE_US  (400u)

// IRQ Name depending on devices
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
#define HFXO_IRQ_NUMBER  HFXO00_IRQn
#define HFXO_IRQ_HANDLER_FUNCTION  HFXO00_IRQHandler
#elif defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)
#define HFXO_IRQ_NUMBER  HFXO0_IRQn
#define HFXO_IRQ_HANDLER_FUNCTION  HFXO0_IRQHandler
#endif

/*******************************************************************************
 *******************************  MACROS   *************************************
 ******************************************************************************/

/*******************************************************************************
* DPLL lock time can be approximately calculated by the equation:
*   COARSECOUNT * (M + 1) * Tref
* Where
*   - COARSECOUNT is calibration value in a hidden register. Its default value
*     is 5 and should not change with calibration.
*   - M is one the DPLL configuration parameter.
*   - Tref is the reference clock period.
*******************************************************************************/
#define DPLL_LOCKING_DELAY_US_FUNCTION(M, freq_ref) \
  ((uint64_t)(DPLL_COARSECOUNT_VALUE * ((M) +1)) * 1000000 + ((freq_ref) - 1)) / (freq_ref)

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

// Variables to save the relevant clock registers.
uint32_t cmu_sys_clock_register;

// Time in ticks required for HFXO start-up after wake-up from sleep.
static uint32_t hfxo_wakeup_time_tick = 0;

// Time in ticks required for the general wake-up process.
static uint32_t process_wakeup_overhead_tick = 0;

static bool is_hf_x_oscillator_used = false;
static bool is_dpll_used = false;
static bool is_entering_deepsleep = false;

static bool is_hf_x_oscillator_already_started = false;
static uint32_t hf_x_oscillator_wakeup_time_tc_inital = 0;

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/***************************************************************************//**
 * Do some hardware initialization if necessary.
 ******************************************************************************/
void sli_power_manager_init_hardware(void)
{
  EMU_EM23Init_TypeDef em23_init = EMU_EM23INIT_DEFAULT;

  uint32_t cmu_em01_grpA_clock_register;
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)
  uint32_t cmu_em01_grpB_clock_register;
#endif

  // Initializes EMU (voltage scaling in EM2/3)
#if defined(EMU_VSCALE_PRESENT)
  EMU_EM01Init_TypeDef em01_init = EMU_EM01INIT_DEFAULT;

  EMU_EM01Init(&em01_init);

#if (SL_POWER_MANAGER_CONFIG_VOLTAGE_SCALING_FAST_WAKEUP == 0)
  em23_init.vScaleEM23Voltage = emuVScaleEM23_LowPower;
#else
  em23_init.vScaleEM23Voltage = emuVScaleEM23_FastWakeup;
#endif
#endif

  EMU_EM23Init(&em23_init);

  // Get the current HF oscillator for the SYSCLK
  cmu_sys_clock_register = CMU->SYSCLKCTRL & _CMU_SYSCLKCTRL_CLKSEL_MASK;
  cmu_em01_grpA_clock_register = CMU->EM01GRPACLKCTRL & _CMU_EM01GRPACLKCTRL_CLKSEL_MASK;
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)
  cmu_em01_grpB_clock_register = CMU->EM01GRPBCLKCTRL & _CMU_EM01GRPBCLKCTRL_CLKSEL_MASK;
#endif

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)
  CMU->CLKEN0_SET = CMU_CLKEN0_HFXO0;

  CMU->CLKEN0_SET = CMU_CLKEN0_DPLL0;
#endif

  is_dpll_used = ((DPLL0->STATUS & _DPLL_STATUS_ENS_MASK) != 0);

  is_hf_x_oscillator_used = ((cmu_sys_clock_register == CMU_SYSCLKCTRL_CLKSEL_HFXO)
                             || (cmu_em01_grpA_clock_register == CMU_EM01GRPACLKCTRL_CLKSEL_HFXO));

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
  is_hf_x_oscillator_used |= (CMU->RADIOCLKCTRL & _CMU_RADIOCLKCTRL_EN_MASK) != 0;
#endif

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)
  is_hf_x_oscillator_used |= (cmu_em01_grpB_clock_register == CMU_EM01GRPBCLKCTRL_CLKSEL_HFXO);
#endif

  if (is_dpll_used && !is_hf_x_oscillator_used) {
    is_hf_x_oscillator_used |= (CMU->DPLLREFCLKCTRL & _CMU_DPLLREFCLKCTRL_CLKSEL_MASK) == _CMU_DPLLREFCLKCTRL_CLKSEL_HFXO;
  }

  // Set HFXO wakeup time to conservative default value
  hfxo_wakeup_time_tick = sli_power_manager_convert_delay_us_to_tick(HFXO_WAKE_UP_TIME_DEFAULT_VALUE_US);

  // Calculate DPLL locking delay from its configuration
  if (is_dpll_used) {
    uint32_t freq = 0;

    switch (CMU->DPLLREFCLKCTRL & _CMU_DPLLREFCLKCTRL_CLKSEL_MASK) {
      case _CMU_DPLLREFCLKCTRL_CLKSEL_HFXO:
        freq = SystemHFXOClockGet();
        break;

      case _CMU_DPLLREFCLKCTRL_CLKSEL_LFXO:
        freq = SystemLFXOClockGet();
        break;

      case _CMU_DPLLREFCLKCTRL_CLKSEL_CLKIN0:
        freq = SystemCLKIN0Get();
        break;

      default:
        EFM_ASSERT(false);
        break;
    }
    // Add DPLL Locking delay
    process_wakeup_overhead_tick += sli_power_manager_convert_delay_us_to_tick(DPLL_LOCKING_DELAY_US_FUNCTION((DPLL0->CFG1 & _DPLL_CFG1_M_MASK) >> _DPLL_CFG1_M_SHIFT, freq));
  }

  // Calculate and add other wake-up delays in ticks
#if defined(EMU_VSCALE_PRESENT) && (SL_POWER_MANAGER_CONFIG_VOLTAGE_SCALING_FAST_WAKEUP == 0)
  // Add Voltage scaling delay if applicable
  process_wakeup_overhead_tick += sli_power_manager_convert_delay_us_to_tick(EM2_WAKEUP_VSCALE_OVERHEAD_US);
#endif
  process_wakeup_overhead_tick += sli_power_manager_convert_delay_us_to_tick(EM2_WAKEUP_PROCESS_TIME_OVERHEAD_US);
}

/***************************************************************************//**
 * Save the CMU HF clock select state, oscillator enable, and voltage scaling.
 ******************************************************************************/
void sli_power_manager_save_states(void)
{
  EMU_Save();
}

/***************************************************************************//**
 * Handle pre-sleep operations if any are necessary, like manually disabling
 * oscillators, change clock settings, etc.
 ******************************************************************************/
void EMU_EM23PresleepHook(void) // In order to be a bit more efficient, we would be better to somehow check that we are not going back to deepsleep. In that case we could skip that.
{
  // Change the HF Clocks to be on FSRCO before sleep and disable Radio Clock
  if (is_entering_deepsleep) {
    is_entering_deepsleep = false;

    CMU->SYSCLKCTRL = (CMU->SYSCLKCTRL & ~_CMU_SYSCLKCTRL_CLKSEL_MASK) | _CMU_SYSCLKCTRL_CLKSEL_FSRCO;
    if (is_hf_x_oscillator_used) {
      HFXO0->CTRL_SET = HFXO_CTRL_DISONDEMAND;
    }
  }
}

/***************************************************************************//**
 * Handle post-sleep operations. The idea is to start HFXO ASAP when we know we
 * will need it.
 ******************************************************************************/
void EMU_EM23PostsleepHook(void)
{
  // Poke sleeptimer to determine if power manager's timer expired before the
  // ISR handler executes.
  if (is_hf_x_oscillator_used
      && sli_sleeptimer_hal_is_int_status_set(SLEEPTIMER_EVENT_COMP)
      && sli_sleeptimer_is_power_manager_timer_next_to_expire()) {
    HFXO0->CTRL_SET = HFXO_CTRL_FORCEEN;

    hf_x_oscillator_wakeup_time_tc_inital = sl_sleeptimer_get_tick_count();
    is_hf_x_oscillator_already_started = true;
  }
}

/***************************************************************************//**
 * Handle pre-deepsleep operations if any are necessary, like manually disabling
 * oscillators, change clock settings, etc.
 ******************************************************************************/
void sli_power_manager_handle_pre_deepsleep_operations(void)
{
  is_entering_deepsleep = true;
}

/***************************************************************************//**
 * Handle post-sleep operations if any are necessary, like manually enabling
 * oscillators, change clock settings, etc.
 ******************************************************************************/
void sli_power_manager_restore_high_freq_accuracy_clk(void)
{
  if (!is_hf_x_oscillator_used) {
    return;
  }

  if (!is_hf_x_oscillator_already_started) { // For the cases where it's not started from an early wake up
    HFXO0->CTRL_SET = HFXO_CTRL_FORCEEN;
    hf_x_oscillator_wakeup_time_tc_inital = sl_sleeptimer_get_tick_count();
  }

  CMU->SYSCLKCTRL = (CMU->SYSCLKCTRL & ~_CMU_SYSCLKCTRL_CLKSEL_MASK) | _CMU_SYSCLKCTRL_CLKSEL_HFXO;

  hfxo_wakeup_time_tick = sl_sleeptimer_get_tick_count() - hf_x_oscillator_wakeup_time_tc_inital;
  is_hf_x_oscillator_already_started = false;

  // Those must be performed in 2 different register writes
  HFXO0->CTRL_CLR = HFXO_CTRL_DISONDEMAND;
  HFXO0->CTRL_CLR = HFXO_CTRL_FORCEEN;

  SystemCoreClockUpdate();
}

/***************************************************************************//**
 * Restore CMU HF clock select state, oscillator enable, and voltage scaling.
 ******************************************************************************/
void sli_power_manager_restore_states(void)
{
  EMU_Restore();

  CMU->SYSCLKCTRL = (CMU->SYSCLKCTRL & ~_CMU_SYSCLKCTRL_CLKSEL_MASK) | cmu_sys_clock_register;

  // Wait for DPLL to lock
#if 0 // TODO This seems to cause issues in some cases. That has to be fixed.
  if (is_dpll_used) {
    while (!(DPLL0->STATUS && _DPLL_STATUS_RDY_MASK)) {
    }
  }
#endif
}

/***************************************************************************//**
 * Applies energy mode.
 *
 * @param em  Energy mode to apply:
 *            SL_POWER_MANAGER_EM0
 *            SL_POWER_MANAGER_EM1
 *            SL_POWER_MANAGER_EM2
 *
 * @note EMU_EnterEM2() and EMU_EnterEM3() has the parameter 'restore' set to
 *       true in the Power Manager. When set to true, the parameter 'restore'
 *       allows the EMU driver to save and restore oscillators, clocks and
 *       voltage scaling. When the processor returns from EM2 or EM3, its
 *       execution resumes in a clean and stable state.
 ******************************************************************************/
void sli_power_manager_apply_em(sl_power_manager_em_t em)
{
  // Perform required actions according to energy mode
  switch (em) {
    case SL_POWER_MANAGER_EM1:
      EMU_EnterEM1();
      break;

    case SL_POWER_MANAGER_EM2:
      EMU_EnterEM2(false);
      break;

    case SL_POWER_MANAGER_EM3:
      EMU_EnterEM3(false);
      break;

    case SL_POWER_MANAGER_EM0:
    default:
      EFM_ASSERT(false);
      break;
  }
}

/*******************************************************************************
 * Returns if the HFXO is enabled or not.
 ******************************************************************************/
bool sli_power_manager_is_high_frequency_running(void)
{
  return is_hf_x_oscillator_used;
}

/*******************************************************************************
 * Returns the default minimum offtime for HFXO.
 ******************************************************************************/
uint32_t sli_power_manager_get_default_high_frequency_minimum_offtime(void)
{
  return sli_power_manager_convert_delay_us_to_tick(HFXO_MINIMUM_OFFTIME_DEFAULT_VALUE_US);
}

/*******************************************************************************
 * Gets the delay associated the wake-up process from EM23.
 *
 * @return Delay for the complete wake-up process with full restore.
 ******************************************************************************/
uint32_t sli_power_manager_get_wakeup_process_time_overhead(void)
{
  uint32_t delay = 0;
  uint32_t delay_temp = 0;

  // Add HFXO start-up delay if applicable
  if (hfxo_wakeup_time_tick != 0) {
    delay_temp = hfxo_wakeup_time_tick;
    delay_temp += delay_temp >> HFXO_START_UP_TIME_OVERHEAD_LOG2;
    delay += delay_temp;
  }

  // Add all additional overhead wake-up delays (DPLL, VSCALE, general wake-up process)
  delay += process_wakeup_overhead_tick;

  return delay;
}
#endif
