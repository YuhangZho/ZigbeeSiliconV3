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
#endif

/***************************************************************************//**
 * Print selected asymmetric key algorithm for digital signature.
 ******************************************************************************/
static void print_key_algo(void);

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

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/// String for example
static uint8_t example_string[] = "SE Manager Digital Signature (ECDSA and EdDSA) Example";
#else
static uint8_t example_string[] = "SE Manager Digital Signature (ECDSA) Example";
#endif

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/// Symmetric key type (plain or wrap or volatile)
static uint8_t asymmetric_key_type_select;

static const char *asymmetric_key_type_string[] = {
  "plaintext",
  "wrapped",
  "volatile"
};

/// Asymmetric key algorithm selection
static uint8_t asymmetric_key_algo_select;

static const char *asymmetric_key_algo_string[] = {
  "ECC Weierstrass Prime",
  "ECC EdDSA (Ed25519)"
};
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

/// Plain message size selection
static uint8_t plain_msg_size_select;

static const uint32_t plain_msg_size[] = {
  PLAIN_MSG_SIZE / 10,
  PLAIN_MSG_SIZE / 2,
  PLAIN_MSG_SIZE
};

/// Hash algorithm selection for digital signature
static uint8_t hash_algo_select;

static const char *hash_algo_string[] = {
  "SHA1",
  "SHA224",
  "SHA256",
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  "SHA384",
  "SHA512"
#endif
};

static const sl_se_hash_type_t hash_algo[] = {
  SL_SE_HASH_SHA1,
  SL_SE_HASH_SHA224,
  SL_SE_HASH_SHA256,
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  SL_SE_HASH_SHA384,
  SL_SE_HASH_SHA512
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
        printf("  + Fill %d bytes plain message buffer with random number... ",
               PLAIN_MSG_SIZE);
        if (generate_random_number(get_plain_msg_buf_ptr(),
                                   PLAIN_MSG_SIZE) == SL_STATUS_OK) {
          print_startup();
        } else {
          app_state = SE_MANAGER_IDLE;
        }
      } else {
        app_state = SE_MANAGER_IDLE;
      }
      break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case SELECT_KEY_TYPE:
      if (pb0_press) {
        pb0_press = false;
        asymmetric_key_type_select++;
        if (asymmetric_key_type_select == 3) {
          asymmetric_key_type_select = 0;
        }
        printf("  + Current asymmetric key is a %s key.\n",
               asymmetric_key_type_string[asymmetric_key_type_select]);
      }
      if (pb1_press) {
        pb1_press = false;
        printf("\n  . Current asymmetric key algorithm is %s.\n",
               asymmetric_key_algo_string[asymmetric_key_algo_select]);
        printf("  + Press PB0 to select asymmetric key algorithm (%s/%s), press"
               " PB1 to next option.\n",
               asymmetric_key_algo_string[0],
               asymmetric_key_algo_string[1]);
        app_state = SELECT_KEY_ALGO;
      }
      break;

    case SELECT_KEY_ALGO:
      if (pb0_press) {
        pb0_press = false;
        asymmetric_key_algo_select++;
        if (asymmetric_key_algo_select == 2) {
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
        printf("\n  . Current Hash algorithm for signature is %s.\n",
               hash_algo_string[hash_algo_select]);
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        printf("  + Press PB0 to select Hash algorithm (SHA1/224/256/384/512) "
               "for signature, press PB1 to next option.\n");
#else
        printf("  + Press PB0 to select Hash algorithm (SHA1/224/256) for "
               "signature, press PB1 to next option.\n");
#endif
        app_state = SELECT_HASH_TYPE;
      }
      break;

    case SELECT_HASH_TYPE:
      if (pb0_press) {
        pb0_press = false;
        hash_algo_select++;
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        if (hash_algo_select == 5) {
#else
        if (hash_algo_select == 3) {
#endif
          hash_algo_select = 0;
        }
        printf("  + Current Hash algorithm for signature is %s.\n",
               hash_algo_string[hash_algo_select]);
      }
      if (pb1_press) {
        pb1_press = false;
        printf("\n  . Current data length is %lu bytes.\n",
               plain_msg_size[plain_msg_size_select]);
        printf("  + Press PB0 to select data length (%lu or %lu or %lu), press "
               "PB1 to run.\n", plain_msg_size[0], plain_msg_size[1],
               plain_msg_size[2]);
        app_state = SELECT_DATA_SIZE;
      }
      break;

    case SELECT_DATA_SIZE:
      if (pb0_press) {
        pb0_press = false;
        plain_msg_size_select++;
        if (plain_msg_size_select == 3) {
          plain_msg_size_select = 0;
        }
        printf("  + Current data length is %lu bytes.\n",
               plain_msg_size[plain_msg_size_select]);
      }
      if (pb1_press) {
        pb1_press = false;
        set_plain_msg_len(plain_msg_size[plain_msg_size_select]);
        print_key_algo();
      }
      break;

    case CREATE_PLAIN_KEY:
      printf("  + Generate a plain asymmetric key... ");
      if (create_plain_asymmetric_key(selected_key) == SL_STATUS_OK) {
        app_state = SIGN_MESSAGE;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case CREATE_WRAP_KEY:
      printf("  + Generate a non-exportable wrapped asymmetric key... ");
      if (create_wrap_asymmetric_key(selected_key) == SL_STATUS_OK) {
        app_state = SIGN_MESSAGE;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case CREATE_VOLATILE_KEY:
      printf("  + Generate a non-exportable asymmetric key into a volatile SE "
             "key slot... ");
      if (create_volatile_asymmetric_key(selected_key) == SL_STATUS_OK) {
        app_state = SIGN_MESSAGE;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case DELETE_VOLATILE_KEY:
      printf("  + Delete a non-exportable asymmetric key in a volatile SE key"
             " slot... ");
      if (delete_volatile_asymmetric_key(selected_key) == SL_STATUS_OK) {
        print_startup();
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;
#endif

    case SIGN_MESSAGE:
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
      if (asymmetric_key_algo_select == 0) {
        printf("  + Sign %lu bytes message with %s and %s private key... ",
               plain_msg_size[plain_msg_size_select],
               hash_algo_string[hash_algo_select],
               asymmetric_key_type_string[asymmetric_key_type_select]);
      } else {
        printf("  + Sign %lu bytes message with %s private key... ",
               plain_msg_size[plain_msg_size_select],
               asymmetric_key_type_string[asymmetric_key_type_select]);
      }
#else
      printf("  + Sign %lu bytes message with %s and plaintext private key... ",
             plain_msg_size[plain_msg_size_select],
             hash_algo_string[hash_algo_select]);
#endif
      if (sign_message(hash_algo[hash_algo_select]) == SL_STATUS_OK) {
        app_state = VERIFY_SIGNATURE;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case VERIFY_SIGNATURE:
      // It needs to get public key from private key in this example
      printf("  + Export public key from private key... ");
      if (export_public_key() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
      if (asymmetric_key_algo_select == 0) {
        printf("  + Verify signature with %s and %s public key... ",
               hash_algo_string[hash_algo_select],
               asymmetric_key_type_string[asymmetric_key_type_select]);
      } else {
        printf("  + Verify signature with %s public key... ",
               asymmetric_key_type_string[asymmetric_key_type_select]);
      }
#else
      printf("  + Verify signature with %s and plaintext public key... ",
             hash_algo_string[hash_algo_select]);
#endif
      if (verify_signature(hash_algo[hash_algo_select]) == SL_STATUS_OK) {
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        if (asymmetric_key_type_select == 2) {
          app_state = DELETE_VOLATILE_KEY;
        } else {
          print_startup();
        }
#else
        print_startup();
#endif
      } else {
        app_state = SE_MANAGER_EXIT;
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
 * Print startup selection.
 ******************************************************************************/
static void print_startup(void)
{
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  printf("\n  . Current asymmetric key is a %s key.\n",
         asymmetric_key_type_string[asymmetric_key_type_select]);
  printf("  + Press PB0 to select a %s or %s or %s key, press PB1 to next "
         "option.\n",
         asymmetric_key_type_string[0],
         asymmetric_key_type_string[1],
         asymmetric_key_type_string[2]);
  app_state = SELECT_KEY_TYPE;
#else
  printf("\n  . Current ECC Weierstrass Prime key is %s.\n",
         ecc_weierstrass_prime_key_string[ecc_weierstrass_prime_key_select]);
  printf("  + Press PB0 to select ECC Weierstrass Prime key (%s/%s), press PB1 "
         "to next option.\n",
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
           " press PB1 to next option.\n",
           ecc_weierstrass_prime_key_string[0],
           ecc_weierstrass_prime_key_string[1],
           ecc_weierstrass_prime_key_string[2],
           ecc_weierstrass_prime_key_string[3],
           ecc_weierstrass_prime_key_string[4]);
    app_state = SELECT_WEIERSTRASS_KEY;
  } else {
    // ECC EdDSA key for digital signature
    selected_key = SL_SE_KEY_TYPE_ECC_EDDSA;
    printf("\n  . Current data length is %lu bytes.\n",
           plain_msg_size[plain_msg_size_select]);
    printf("  + Press PB0 to select data length (%lu or %lu or %lu), press "
           "PB1 to run.\n", plain_msg_size[0], plain_msg_size[1],
           plain_msg_size[2]);
    app_state = SELECT_DATA_SIZE;
  }
}
#endif

/***************************************************************************//**
 * Print selected asymmetric key algorithm for digital signature.
 ******************************************************************************/
static void print_key_algo(void)
{
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  printf("\n  . Digital signature");
  printf("\n  + %s",
         asymmetric_key_algo_string[asymmetric_key_algo_select]);
  if (asymmetric_key_algo_select == 0) {
    printf(" - %s \n",
           ecc_weierstrass_prime_key_string[ecc_weierstrass_prime_key_select]);
  } else {
    printf("\n");
  }

  if (asymmetric_key_type_select == 0) {
    app_state = CREATE_PLAIN_KEY;
  } else if (asymmetric_key_type_select == 1) {
    app_state = CREATE_WRAP_KEY;
  } else {
    app_state = CREATE_VOLATILE_KEY;
  }
#else
  printf("\n  . Digital signature");
  printf("\n  + ECC Weierstrass Prime - %s\n",
         ecc_weierstrass_prime_key_string[ecc_weierstrass_prime_key_select]);
  app_state = CREATE_PLAIN_KEY;
#endif
}
