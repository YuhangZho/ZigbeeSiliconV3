se_manager_signature

This example uses the SE Manager API to perform the digital signature (ECDSA and
EdDSA) in EFR32xG21A and EFR32xG21B (wireless SoC and modules).

The example redirects standard I/O to the virtual serial port (VCOM) of the
Starter Kit. By default the serial port setting is 115200 bps and 8-N-1
configuration.

The example has been instrumented with code to count the number of clock cycles
spent in digital signature operations. The results are printed to stdout, i.e. 
the VCOM serial port console. This feature can be disabled by defining 
SE_MANAGER_PRINT=0 (default is 1) in IDE setting (Preprocessor->Defined 
symbols).

In this example, SE Manager is used to first generate a public/private key pair 
using a given Elliptic Curve Cryptography (ECC) curve. The private key is then 
used to sign the hash of a message buffer. The signature is then verified using 
the public key.

EFR32xG21A - plaintext asymmetric key
EFR32xG21B - plaintext or wrapped or volatile asymmetric key

Following ECC curves are supported in this example.
ECC Weierstrass Prime:
ECC P192
ECC P256
ECC P384 (Secure Vault only)
ECC P521 (Secure Vault only)
ECC Custom (secp256k1 in this example) (Secure Vault only)

ECC EdDSA: (Secure Vault only)
ECC Ed25519

The available Hash algorithms for digital signature are:
SHA-1
SHA-224
SHA-256
SHA-384 (Secure Vault only)
SHA-512 (Secure Vault only)

Following SE Manager APIs are used in this example.
sl_se_init
sl_se_deinit
sl_se_init_command_context
sl_se_deinit_command_context
sl_se_get_random
sl_se_validate_key
sl_se_get_storage_size
sl_se_generate_key
sl_se_delete_key (Secure Vault only)
sl_se_ecc_sign
sl_se_ecc_verify


How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Upgrade the SE firmware to the latest version (see AN1222 for details)
   https://www.silabs.com/documents/public/application-notes/an1222-efr32xg2x-production-programming.pdf
3. Build the project and download to the Starter Kit
4. Open any terminal program and connect to the device's VCOM port
   (Note: If using puTTY, Local echo must be set to force on)
5. Follow the instructions shown on the console


Peripherals Used:
HFRCODPLL - 38 MHz
USART0    - 115200 baud, 8-N-1
GPIO
SE

Board:  Silicon Labs EFR32xG21 2.4 GHz 10 dBm Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard (BRD4001A)
Device: EFM32MG21A010F1024IM32
PA05 - USART0 TX
PA06 - USART0 RX
PD04 - Board Controller VCOM Enable
PD02 - WSTK push button PB0
PD03 - WSTK push button PB1

Board:  Silicon Labs EFR32xG21B 2.4 GHz 10 dBm Radio Board (BRD4181C) + 
        Wireless Starter Kit Mainboard (BRD4001A)
Device: EFM32MG21B010F1024IM32
PA05 - USART0 TX
PA06 - USART0 RX
PD04 - Board Controller VCOM Enable
PB00 - WSTK push button PB0
PB01 - WSTK push button PB1