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
 * Print startup selection.
 ******************************************************************************/
static void print_startup(void);

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Print asymmetric key algorithm option.
 ******************************************************************************/
static void print_key_algo_option(void);

/***************************************************************************//**
 * Save public key for verification.
 ******************************************************************************/
static void copy_public_key(void);

/***************************************************************************//**
 * Compare public key after exporting or transferring key.
 ******************************************************************************/
static void compare_public_key(void);
#endif

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
static uint8_t example_string[] = "SE Manager Asymmetric Key Handling Example";

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/// Asymmetric key algorithm selection
static uint8_t asymmetric_key_algo_select;

static const char *asymmetric_key_algo_string[] = {
  "ECC Weierstrass Prime",
  "ECC Montgomery",
  "ECC EdDSA (Ed25519)"
};

/// ECC Montgomery key selection for ECDH
static uint8_t ecc_montgomery_key_select;

static const char *ecc_montgomery_key_string[] = {
  "ECC X25519",
  "ECC X448"
};

static const sl_se_key_type_t ecc_montgomery_key[] = {
  SL_SE_KEY_TYPE_ECC_X25519,
  SL_SE_KEY_TYPE_ECC_X448
};

/// Buffer for public key verification
static uint8_t copy_buf[DOMAIN_SIZE * 6 + DOMAIN_SIZE * 2];
#endif

/// ECC Weierstrass Prime key selection
static uint8_t ecc_weierstrass_prime_key_select;

static const char *ecc_weierstrass_prime_key_string[] = {
  "ECC P192",
  "ECC P256",
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  "ECC P384",
  "ECC P521",
  "ECC Custom (secp256k1 in this example)"
#endif
};

static const sl_se_key_type_t ecc_weierstrass_prime_key[] = {
  SL_SE_KEY_TYPE_ECC_P192,
  SL_SE_KEY_TYPE_ECC_P256,
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  SL_SE_KEY_TYPE_ECC_P384,
  SL_SE_KEY_TYPE_ECC_P521,
  SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM
#endif
};

/// Selected key to run the example
static sl_se_key_type_t selected_key;

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
        print_startup();
      } else {
        app_state = SE_MANAGER_IDLE;
      }
      break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case SELECT_KEY_ALGO:
      if (pb0_press) {
        pb0_press = false;
        asymmetric_key_algo_select++;
        if (asymmetric_key_algo_select == 3) {
          asymmetric_key_algo_select = 0;
        }
        printf("  + Current asymmetric key algorithm is %s.\n",
               asymmetric_key_algo_string[asymmetric_key_algo_select]);
      }
      if (pb1_press) {
        pb1_press = false;
        print_key_algo_option();
      }
      break;

    case SELECT_MONTGOMERY_KEY:
      if (pb0_press) {
        pb0_press = false;
        ecc_montgomery_key_select++;
        if (ecc_montgomery_key_select == 2) {
          ecc_montgomery_key_select = 0;
        }
        printf("  + Current ECC Montgomery key is %s.\n",
               ecc_montgomery_key_string[ecc_montgomery_key_select]);
      }
      if (pb1_press) {
        pb1_press = false;
        selected_key = ecc_montgomery_key[ecc_montgomery_key_select];
        app_state = CREATE_PLAIN_KEY;
      }
      break;
#endif

    case SELECT_WEIERSTRASS_KEY:
      if (pb0_press) {
        pb0_press = false;
        ecc_weierstrass_prime_key_select++;
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        if (ecc_weierstrass_prime_key_select == 5) {
#else
        if (ecc_weierstrass_prime_key_select == 2) {
#endif
          ecc_weierstrass_prime_key_select = 0;
        }
        printf("  + Current ECC Weierstrass Prime key is %s.\n",
               ecc_weierstrass_prime_key_string[ecc_weierstrass_prime_key_select]);
      }
      if (pb1_press) {
        pb1_press = false;
        selected_key = ecc_weierstrass_prime_key[ecc_weierstrass_prime_key_select];
        app_state = CREATE_PLAIN_KEY;
      }
      break;

    case CREATE_PLAIN_KEY:
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
      printf("\n  . Asymmetric key handling");
      printf("\n  + %s",
             asymmetric_key_algo_string[asymmetric_key_algo_select]);
      if (asymmetric_key_algo_select == 0) {
        printf(" - %s \n",
               ecc_weierstrass_prime_key_string[ecc_weierstrass_prime_key_select]);
      } else if (asymmetric_key_algo_select == 1) {
        printf(" - %s \n", ecc_montgomery_key_string[ecc_montgomery_key_select]);
      } else {
        printf("\n");
      }
#else
      printf("\n  . Asymmetric key handling");
      printf("\n  + ECC Weierstrass Prime - %s\n",
             ecc_weierstrass_prime_key_string[ecc_weierstrass_prime_key_select]);
#endif
      printf("\n  . Plain asymmetric key\n");
      printf("  + Generate a plain asymmetric key... ");
      if (create_plain_asymmetric_key(selected_key) == SL_STATUS_OK) {
        app_state = EXPORT_PLAIN_PUBLIC_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case EXPORT_PLAIN_PUBLIC_KEY:
      printf("  + Export an asymmetric public key from plain asymmetric "
             "key... ");
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
      // For compare public key after exporting and transferring key
      memset(get_asymmetric_pubkey_buf_ptr(), 0, ECC_PUBKEY_SIZE);
      memset(get_asymmetric_custom_pubkey_buf_ptr(), 0, sizeof(copy_buf));
#endif
      if (export_asymmetric_pubkey_from_plain(selected_key) == SL_STATUS_OK) {
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        app_state = IMPORT_PLAIN_KEY;
#else
        print_startup();
#endif
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case IMPORT_PLAIN_KEY:
      printf("\n  . Import and export asymmetric key\n");
      printf("  + Import a plain asymmetric key from buffer into an exportable "
             "wrapped key... ");
      if (import_plain_asymmetric_key(selected_key) == SL_STATUS_OK) {
        app_state = EXPORT_WRAP_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case EXPORT_WRAP_KEY:
      printf("  + Export a wrapped asymmetric key into a plain key buffer... ");
      if (export_wrap_asymmetric_key(selected_key) == SL_STATUS_OK) {
        app_state = COMPARE_EXPORT_PUBLIC_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case COMPARE_EXPORT_PUBLIC_KEY:
      printf("  + Export an asymmetric public key from plain asymmetric "
             "key... ");
      copy_public_key();
      if (export_asymmetric_pubkey_from_plain(selected_key) == SL_STATUS_OK) {
        printf("  + Compare export of asymmetric public key with import "
               "one... ");
        compare_public_key();
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case CREATE_WRAP_KEY:
      printf("\n  . Wrapped asymmetric key\n");
      printf("  + Generate a non-exportable wrapped asymmetric key... ");
      if (create_wrap_asymmetric_key(selected_key) == SL_STATUS_OK) {
        app_state = EXPORT_WRAP_PUBLIC_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case EXPORT_WRAP_PUBLIC_KEY:
      printf("  + Export an asymmetric public key from wrapped asymmetric "
             "key... ");
      if (export_asymmetric_pubkey_from_wrap(selected_key) == SL_STATUS_OK) {
        // For compare public key after transferring key
        copy_public_key();
        app_state = CREATE_VOLATILE_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case CREATE_VOLATILE_KEY:
      printf("\n  . Volatile asymmetric key\n");
      printf("  + Generate a non-exportable asymmetric key into a volatile SE "
             "key slot... ");
      if (create_volatile_asymmetric_key(selected_key) == SL_STATUS_OK) {
        app_state = EXPORT_VOLTAILE_PUBLIC_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case EXPORT_VOLTAILE_PUBLIC_KEY:
      printf("  + Export an asymmetric public key from a volatile SE key "
             "slot... ");
      if (export_asymmetric_pubkey_from_volatile(selected_key)
          == SL_STATUS_OK) {
        app_state = DELETE_VOLATILE_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case DELETE_VOLATILE_KEY:
      printf("  + Delete a non-exportable asymmetric key in a volatile SE key"
             " slot... ");
      if (delete_volatile_asymmetric_key(selected_key) == SL_STATUS_OK) {
        app_state = TRANSFER_WRAP_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case TRANSFER_WRAP_KEY:
      printf("\n  . Transfer asymmetric key\n");
      printf("  + Transfer a non-exportable wrapped asymmetric key into a "
             "volatile SE key slot... ");
      if (transfer_wrap_asymmetric_key_to_volatile(selected_key)
          == SL_STATUS_OK) {
        app_state = TRANSFER_VOLATILE_KEY;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case TRANSFER_VOLATILE_KEY:
      printf("  + Transfer a non-exportable asymmetric key in a volatile SE key"
             " slot into a wrapped key buffer... ");
      if (transfer_volatile_asymmetric_key_to_wrap(selected_key)
          == SL_STATUS_OK) {
        printf("  + Delete a non-exportable asymmetric key in a volatile SE key"
               " slot... ");
        if (delete_volatile_asymmetric_key(selected_key) == SL_STATUS_OK) {
          app_state = COMPARE_TRANSFER_PUBLIC_KEY;
        } else {
          app_state = SE_MANAGER_EXIT;
        }
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case COMPARE_TRANSFER_PUBLIC_KEY:
      printf("  + Export an asymmetric public key from wrapped asymmetric "
             "key... ");
      if (export_asymmetric_pubkey_from_wrap(selected_key) == SL_STATUS_OK) {
        printf("  + Compare transfer of asymmetric public key with original "
               "one... ");
        compare_public_key();
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;
#endif

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
 * Print startup selection.
 ******************************************************************************/
static void print_startup(void)
{
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  printf("\n  . Current asymmetric key algorithm is %s.\n",
         asymmetric_key_algo_string[asymmetric_key_algo_select]);
  printf("  + Press PB0 to select asymmetric key algorithm (%s/%s/%s), press"
         " PB1 to next option or run if %s is selected.\n",
         asymmetric_key_algo_string[0],
         asymmetric_key_algo_string[1],
         asymmetric_key_algo_string[2],
         asymmetric_key_algo_string[2]);
  app_state = SELECT_KEY_ALGO;
#else
  printf("\n  . Current ECC Weierstrass Prime key is %s.\n",
         ecc_weierstrass_prime_key_string[ecc_weierstrass_prime_key_select]);
  printf("  + Press PB0 to select ECC Weierstrass Prime key (%s/%s), press PB1 "
         "to run.\n",
         ecc_weierstrass_prime_key_string[0],
         ecc_weierstrass_prime_key_string[1]);
  app_state = SELECT_WEIERSTRASS_KEY;
#endif
}

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Print asymmetric key algorithm option.
 ******************************************************************************/
static void print_key_algo_option(void)
{
  if (asymmetric_key_algo_select == 0) {
    printf("\n  . Current ECC Weierstrass Prime key is %s.\n",
           ecc_weierstrass_prime_key_string[ecc_weierstrass_prime_key_select]);
    printf("  + Press PB0 to select ECC Weierstrass Prime key (%s/%s/%s/%s/%s),"
           " press PB1 to run.\n",
           ecc_weierstrass_prime_key_string[0],
           ecc_weierstrass_prime_key_string[1],
           ecc_weierstrass_prime_key_string[2],
           ecc_weierstrass_prime_key_string[3],
           ecc_weierstrass_prime_key_string[4]);
    app_state = SELECT_WEIERSTRASS_KEY;
  } else if (asymmetric_key_algo_select == 1) {
    printf("\n  . Current ECC Montgomery key is %s.\n",
           ecc_montgomery_key_string[ecc_montgomery_key_select]);
    printf("  + Press PB0 to select ECC Montgomery key (%s/%s), press PB1 to "
           "run.\n",
           ecc_montgomery_key_string[0],
           ecc_montgomery_key_string[1]);
    app_state = SELECT_MONTGOMERY_KEY;
  } else {
    // ECC EdDSA key for digital signature
    selected_key = SL_SE_KEY_TYPE_ECC_EDDSA;
    app_state = CREATE_PLAIN_KEY;
  }
}

/***************************************************************************//**
 * Save public key for verification.
 ******************************************************************************/
static void copy_public_key(void)
{
  if (selected_key == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    memcpy(copy_buf, get_asymmetric_custom_pubkey_buf_ptr(), sizeof(copy_buf));
    memset(get_asymmetric_custom_pubkey_buf_ptr(), 0, sizeof(copy_buf));
  } else {
    memcpy(copy_buf, get_asymmetric_pubkey_buf_ptr(), ECC_PUBKEY_SIZE);
    memset(get_asymmetric_pubkey_buf_ptr(), 0, ECC_PUBKEY_SIZE);
  }
}

/***************************************************************************//**
 * Compare public key after exporting or transferring key.
 ******************************************************************************/
static void compare_public_key(void)
{
  if (selected_key == SL_SE_KEY_TYPE_ECC_WEIERSTRASS_PRIME_CUSTOM) {
    if (memcmp(copy_buf, get_asymmetric_custom_pubkey_buf_ptr(),
               sizeof(copy_buf)) != 0) {
      printf("Failed\n");
      app_state = SE_MANAGER_EXIT;
      return;
    }
  } else {
    if (memcmp(copy_buf, get_asymmetric_pubkey_buf_ptr(),
               ECC_PUBKEY_SIZE) != 0) {
      printf("Failed\n");
      app_state = SE_MANAGER_EXIT;
      return;
    }
  }

  printf("OK\n");
  if (app_state == COMPARE_EXPORT_PUBLIC_KEY) {
    app_state = CREATE_WRAP_KEY;
  } else {
    print_startup();
  }
}
#endif
