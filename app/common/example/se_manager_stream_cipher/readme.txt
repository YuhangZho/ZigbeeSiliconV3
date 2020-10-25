se_manager_stream_cipher

This example uses the SE Manager API to preform the stream ciphers in EFR32xG21A
and EFR32xG21B (wireless SoC and modules).

The example redirects standard I/O to the virtual serial port (VCOM) of the
Starter Kit. By default the serial port setting is 115200 bps and 8-N-1
configuration.

The example has been instrumented with code to count the number of clock cycles
spent in different stream ciphers. The results are printed to stdout, i.e. the
VCOM serial port console. This feature can be disabled by defining 
SE_MANAGER_PRINT=0 (default is 1) in IDE setting (Preprocessor->Defined 
symbols).

In this example, test vectors are used to verify the stream ciphers.

Stream ciphers:
AES CMAC
AES GCM encryption and decryption
ChaCha20 encryption and decryption (Secure Vault only)

Following SE Manager APIs are used in this example.
sl_se_init
sl_se_deinit
sl_se_init_command_context
sl_se_deinit_command_context
sl_se_validate_key
sl_se_get_storage_size
sl_se_generate_key
sl_se_cmac_starts
sl_se_cmac_update
sl_se_cmac_finish
sl_se_gcm_starts
sl_se_gcm_update
sl_se_gcm_finish
sl_se_chacha20_crypt (Secure Vault only)


How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Upgrade the SE firmware to the latest version (see AN1222 for details)
   https://www.silabs.com/documents/public/application-notes/an1222-efr32xg2x-production-programming.pdf
3. Build the project and download to the Starter Kit
4. Open any terminal program and connect to the device's VCOM port
   (Note: If using puTTY, Local echo must be set to force on)
5. The terminal screen should display the process steps of stream cipher example


Peripherals Used:
HFRCODPLL - 38 MHz
USART0    - 115200 baud, 8-N-1
SE

Board:  Silicon Labs EFR32xG21 2.4 GHz 10 dBm Radio Board (BRD4181A) + 
        Wireless Starter Kit Mainboard (BRD4001A)
Device: EFM32MG21A010F1024IM32
PA05 - USART0 TX
PA06 - USART0 RX
PD04 - Board Controller VCOM Enable

Board:  Silicon Labs EFR32xG21B 2.4 GHz 10 dBm Radio Board (BRD4181C) + 
        Wireless Starter Kit Mainboard (BRD4001A)
Device: EFM32MG21B010F1024IM32
PA05 - USART0 TX
PA06 - USART0 RX
PD04 - Board Controller VCOM Enable