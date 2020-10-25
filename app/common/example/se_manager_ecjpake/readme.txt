se_manager_ecjpake

This example uses the SE Manager API to perform the Elliptic Curve Password 
Authenticated Key Exchange by Juggling (ECJPAKE) in EFR32xG21A and EFR32xG21B 
(wireless SoC and modules).

The example redirects standard I/O to the virtual serial port (VCOM) of the
Starter Kit. By default the serial port setting is 115200 bps and 8-N-1
configuration.

The example has been instrumented with code to count the number of clock cycles
spent in different ECJPAKE operations. The results are printed to stdout, i.e. 
the VCOM serial port console. This feature can be disabled by defining 
SE_MANAGER_PRINT=0 (default is 1) in IDE setting (Preprocessor->Defined 
symbols).

In ECJPAKE protocol, a shared secret is derived between two peers using a 
shared, known password.

In this example, SE Manager is used to initialize two peers that will perform a 
key exchange - a client, and a server. The SE Manager APIs will be used to 
perform the key exchange procedure, then the two shared secrets that are 
generated for each peer are compared.

Following SE Manager APIs are used in this example.
sl_se_init
sl_se_deinit
sl_se_init_command_context
sl_se_deinit_command_context
sl_se_ecjpake_init
sl_se_ecjpake_setup
sl_se_ecjpake_check
sl_se_ecjpake_write_round_one
sl_se_ecjpake_write_round_two
sl_se_ecjpake_read_round_one
sl_se_ecjpake_read_round_two
sl_se_ecjpake_derive_secret
sl_se_ecjpake_free


How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Upgrade the SE firmware to the latest version (see AN1222 for details)
   https://www.silabs.com/documents/public/application-notes/an1222-efr32xg2x-production-programming.pdf
3. Build the project and download to the Starter Kit
4. Open any terminal program and connect to the device's VCOM port
   (Note: If using puTTY, Local echo must be set to force on)
5. The terminal screen should display the process steps of ECJPAKE example


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