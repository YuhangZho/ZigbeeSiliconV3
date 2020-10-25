se_manager_secure_identity

This example uses the SE Manager API and mbed TLS to perform the secure identity
in Secure Vault device.

The example redirects standard I/O to the virtual serial port (VCOM) of the
Starter Kit. By default the serial port setting is 115200 bps and 8-N-1
configuration.

The example has been instrumented with code to count the number of clock cycles
spent in different secure identity operations. The results are printed to 
stdout, i.e. the VCOM serial port console. This feature can be disabled by 
defining SE_MANAGER_PRINT=0 (default is 1) in IDE setting (Preprocessor->Defined 
symbols).

The certificates are printed to stdout, i.e. the VCOM serial port console. This 
feature can be disabled by defining SE_MANAGER_PRINT_CERT=0 (default is 1) in 
IDE setting (Preprocessor->Defined symbols).

The CRYPTO hardware acceleration on mbed TLS can be switched off by defining 
NO_CRYPTO_ACCELERATION symbol in IDE setting (Preprocessor->Defined symbols). It
needs to increase the stack and heap size in IAR to use this option.

This example uses the CRYPTO engine in the Secure Element to accelerate the 
X.509 API functions of mbed TLS. The Private Device Key in the Secure Key 
Storage on the chip is used to sign the challenge from remote device.

See AN1268: Secure Identity for more information about this example.

Following SE Manager APIs are used in this example.
sl_se_init
sl_se_deinit
sl_se_init_command_context
sl_se_deinit_command_context
sl_se_get_random
sl_se_read_cert_size 
sl_se_read_cert 
sl_se_ecc_sign
sl_se_ecc_verify


How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Upgrade the SE firmware to the latest version (see AN1222 for details)
   https://www.silabs.com/documents/public/application-notes/an1222-efr32xg2x-production-programming.pdf
3. Build the project and download to the Starter Kit
4. Open any terminal program and connect to the device's VCOM port
   (Note: If using puTTY, Local echo must be set to force on)
5. The terminal screen should display the process steps of secure identity
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