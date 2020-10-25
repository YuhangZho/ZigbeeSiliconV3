se_manager_kdf

This example uses the SE Manager API to perform the key derivation (HKDF and 
PBKDF2) in Secure Vault device.

The example redirects standard I/O to the virtual serial port (VCOM) of the
Starter Kit. By default the serial port setting is 115200 bps and 8-N-1
configuration.

The example has been instrumented with code to count the number of clock cycles
spent in different key derivation operations. The results are printed to stdout,
i.e. the VCOM serial port console. This feature can be disabled by defining 
SE_MANAGER_PRINT=0 (default is 1) in IDE setting (Preprocessor->Defined 
symbols).

Abstracted from Wikipedia:
In cryptography, a key derivation function (KDF) is a cryptographic hash 
function that derives one or more secret keys from a secret value such as a 
master key, a password, or a passphrase using a pseudo random function. KDFs can
be used to stretch keys into longer keys or to obtain keys of a required format,
such as converting a group element that is the result of a Diffie–Hellman key 
exchange into a symmetric key for use with AES.

HKDF extracts a pseudo random key (PRK) using an HMAC hash function (e.g. 
HMAC-SHA256) on an optional salt (acting as a key) and any potentially weak 
input key material (IKM) (acting as data). It then generates similarly 
cryptographically strong output key material (OKM) of any desired length by 
repeatedly generating PRK-keyed hash-blocks and then appending them into the 
output key material, finally truncating to the desired length.

PBKDF2 applies a pseudo random function, such as hash-based message 
authentication code (HMAC), to the input password or passphrase along with a 
salt value and repeats the process many times to produce a derived key, which 
can then be used as a cryptographic key in subsequent operations. The added 
computational work makes password cracking much more difficult, and is known as 
key stretching.

In this example, test vectors are used to verify the HKDF and PBKDF2 operations.

Following SE Manager APIs are used in this example.
sl_se_init
sl_se_deinit
sl_se_init_command_context
sl_se_deinit_command_context
sl_se_derive_key_hkdf
sl_se_derive_key_pbkdf2


How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Upgrade the SE firmware to the latest version (see AN1222 for details)
   https://www.silabs.com/documents/public/application-notes/an1222-efr32xg2x-production-programming.pdf
3. Build the project and download to the Starter Kit
4. Open any terminal program and connect to the device's VCOM port
   (Note: If using puTTY, Local echo must be set to force on)
5. The terminal screen should display the process steps of key derivation 
   example


Peripherals Used:
HFRCODPLL - 38 MHz
USART0    - 115200 baud, 8-N-1
SE

Board:  Silicon Labs EFR32xG21B 2.4 GHz 10 dBm Radio Board (BRD4181C) + 
        Wireless Starter Kit Mainboard (BRD4001A)
Device: EFM32MG21B010F1024IM32
PA05 - USART0 TX
PA06 - USART0 RX
PD04 - Board Controller VCOM Enable