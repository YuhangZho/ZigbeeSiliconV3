/***************************************************************************//**
 * @file app_process.c
 * @brief Top level application functions.
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_process.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Disable GPIO interrupt for push buttons.
 ******************************************************************************/
static void disable_gpio_int(void);

/***************************************************************************//**
 * Print tamper configuration and test instructions.
 ******************************************************************************/
static void print_tamper_conf(void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Push button 0 status
static bool pb0_press;

/// Push button 1 status
static bool pb1_press;

/// State machine state variable
static state_t app_state = SE_MANAGER_INIT;

/// String for example
static uint8_t example_string[] = "SE Manager Tamper Example";

/// Strings for tamper sources
static const char *tamper_source[SL_SE_TAMPER_SIGNAL_NUM_SIGNALS] = {
  NULL,
  "Filter counter         : ",
  "SE watchdog            : ",
  NULL,
  "SE RAM CRC             : ",
  "SE hard fault          : ",
  NULL,
  "SE software assertion  : ",
  NULL,
  "User secure boot       : ",
  "Mailbox authorization  : ",
  "DCI authorization      : ",
  "Flash integrity        : ",
  NULL,
  "Self test              : ",
  "TRNG monitor           : ",
  "PRS0                   : ",
  "PRS1                   : ",
  "PRS2                   : ",
  "PRS3                   : ",
  "PRS4                   : ",
  "PRS5                   : ",
  "PRS6                   : ",
  "PRS7                   : ",
  "Decouple BOD           : ",
  "Temperature sensor     : ",
  "Voltage glitch falling : ",
  "Voltage glitch rising  : ",
  "Secure lock            : ",
  "SE debug               : ",
  "Digital glitch         : ",
  "SE ICACHE              : "
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Application state machine, called infinitely.
 ******************************************************************************/
void app_process_action(void)
{
  switch (app_state) {
    case SE_MANAGER_INIT:
      printf("\n%s - Core running at %" PRIu32 " kHz.\n", example_string,
             CMU_ClockFreqGet(cmuClock_CORE) / 1000);
      printf("  . SE manager initialization... ");
      if (init_se_manager() == SL_STATUS_OK) {
        app_state = RD_RST_CAUSE;
      } else {
        app_state = SE_MANAGER_IDLE;
      }
      break;

    case RD_RST_CAUSE:
      printf("\n  . Read EMU RSTCAUSE register... ");
      if (get_reset_cause() == SL_STATUS_OK) {
        printf("  + The EMU RSTCAUSE register (MSB..LSB): ");
        printf("%08lX\n", *get_rst_cause_buf_ptr());
        if (*get_rst_cause_buf_ptr() & EMU_RSTCAUSE_SETAMPER) {
          printf("  + The tamper reset is observed\n");
        }
        app_state = GET_OTP_CONF;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case GET_OTP_CONF:
      printf("\n  . Read SE OTP configuration... ");
      if (get_se_otp_conf() == SL_STATUS_OK) {
        print_tamper_conf();
        enable_tamper_int();
        disable_gpio_int();
        init_tamper_prs();
        app_state = WAIT_TAMPER_SIGNAL;
      } else {
        printf("  + Cannot read SE OTP configuration.\n");
        printf("  + Press PB0 to initialize SE OTP for tamper configuration or "
               "press PB1 to abort\n");
        app_state = WAIT_KEY_SELECT;
      }
      break;

    case WAIT_KEY_SELECT:
      if (pb0_press) {
        pb0_press = false;
        printf("  + Warning: The OTP configuration cannot be changed once "
               "written!\n");
        printf("  + Press PB0 to confirm or press PB1 to abort if you are not "
               "sure.\n");
        app_state = WAIT_KEY_CONFIRM;
        break;
      }
      if (pb1_press) {
        pb1_press = false;
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case WAIT_KEY_CONFIRM:
      if (pb0_press) {
        pb0_press = false;
        printf("  + Initialize SE OTP for tamper configuration... ");
        if (init_se_otp_conf() == SL_STATUS_OK) {
          app_state = GET_OTP_CONF;
        } else {
          app_state = SE_MANAGER_EXIT;
          break;
        }
      }
      if (pb1_press) {
        pb1_press = false;
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case WAIT_TAMPER_SIGNAL:
      if (get_tamper_int_status()) {
        printf("\n  . Get tamper status... ");
        if (get_se_status() == SL_STATUS_OK) {
          printf("  + Recorded tamper status (MSB..LSB): ");
          printf("%08lX\n", get_se_status_buf_ptr()->tamper_status);
          printf("  + Currently active tamper sources (MSB..LSB): ");
          printf("%08lX\n", get_se_status_buf_ptr()->tamper_status_raw);

          // Check if tamper filter interrupt is triggered
          if (get_se_status_buf_ptr()->tamper_status & 0x00000002) {
            printf("  + Tamper filter threshold is reached, issue a reset "
                   "through PRS\n");
            printf("  ");
            // Issue a tamper reset through PRS
            PRS_PulseTrigger(0x01 << SW_RST_TAMPER_PRS_CH);
          }
        }
      }
      break;

    case SE_MANAGER_EXIT:
      printf("\n  . SE manager deinitialization... ");
      deinit_se_manager();
      app_state = SE_MANAGER_IDLE;
      break;

    case SE_MANAGER_IDLE:
    default:
      break;
  }
}

/***************************************************************************//**
 * Button callback, called if any button is pressed or released.
 ******************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  // Check if any button was pressed
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (&sl_button_btn0 == handle) {
      pb0_press = true;
    }
    if (&sl_button_btn1 == handle) {
      pb1_press = true;
    }
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Disable GPIO interrupt for push buttons.
 ******************************************************************************/
static void disable_gpio_int(void)
{
  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_DisableIRQ(GPIO_EVEN_IRQn);
  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_DisableIRQ(GPIO_ODD_IRQn);
}

/***************************************************************************//**
 * Print tamper configuration and test instructions.
 ******************************************************************************/
static void print_tamper_conf(void)
{
  uint32_t i;

  // Print tamper configuration
  printf("  + Secure boot: ");
  if (get_se_otp_conf_buf_ptr()->enable_secure_boot) {
    printf("Enabled\n");
  } else {
    printf("Disabled\n");
  }

  printf("  + Tamper source level\n");
  for (i = 0; i < SL_SE_TAMPER_SIGNAL_NUM_SIGNALS; i++) {
    if (tamper_source[i] != NULL) {
      printf("    %s %d\n", tamper_source[i],
             get_se_otp_conf_buf_ptr()->tamper_levels[i]);
    }
  }

  printf("  + Reset period for the tamper filter counter: ~32 ms x %u\n",
         1 << get_se_otp_conf_buf_ptr()->tamper_filter_period);
  printf("  + Activation threshold for the tamper filter: %d\n",
         256 / (1 << get_se_otp_conf_buf_ptr()->tamper_filter_threshold));
  if (get_se_otp_conf_buf_ptr()->tamper_flags) {
    printf("  + Digital glitch detector always on: Enabled\n");
  } else {
    printf("  + Digital glitch detector always on: Disabled\n");
  }
  printf("  + Tamper reset threshold: %d\n",
         get_se_otp_conf_buf_ptr()->tamper_reset_threshold);

  // Print test instructions
  printf("\n  . Test instructions:\n");
  printf("  + Press PB0 to increase filter counter and tamper status is "
         "displayed.\n");
  printf("  + PRS will issue a tamper reset if filter counter reaches %d "
         "within ~32 ms x %u.\n",
         256 / (1 << get_se_otp_conf_buf_ptr()->tamper_filter_threshold),
         1 << get_se_otp_conf_buf_ptr()->tamper_filter_period);
  printf("  + Press PB1 to issue a tamper result.\n");
  if (get_se_otp_conf_buf_ptr()->tamper_reset_threshold == 0) {
    printf("  + Device will never enter diagnostic mode\n");
  } else {
    printf("  + Device will enter diagnostic mode if tamper reset reaches "
           "%d.\n", get_se_otp_conf_buf_ptr()->tamper_reset_threshold);
  }
}
