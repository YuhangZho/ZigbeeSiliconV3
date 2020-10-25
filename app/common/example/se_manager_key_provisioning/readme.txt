se_manager_key_provisioning

This example uses the SE Manager API to perform the key provisioning and OTP
initialization in Series 2 device (wireless SoC and modules).

The example redirects standard I/O to the virtual serial port (VCOM) of the
Starter Kit. By default the serial port setting is 115200 bps and 8-N-1
configuration.

The example has been instrumented with code to count the number of clock cycles
spent in key provisioning and OTP initialization. The results are printed to 
stdout, i.e. the VCOM serial port console. This feature can be disabled by 
defining SE_MANAGER_PRINT=0 (default is 1) in IDE setting
(Preprocessor->Defined symbols).

The secure boot and tamper (Secure Vault only) configuration, 128-bit AES key 
(SE only), public sign key and public command key can be programmed to the SE or
VSE OTP by pressing the push button PB0 two times.

The corresponding private sign key (rootsign-unsafe-privkey.pem) and private 
command key (cmd-unsafe-privkey.pem) can be found in Windows folder below.
C:\SiliconLabs\SimplicityStudio\v5\developer\adapter_packs\secmgr\scripts\
offline

The key provisioning and OTP initialization will fail if the key and OTP had
already provisioned and initialized.

The key cannot be read if it has not been provisioned before.

The secure boot cannot be enabled if the private sign key has not been
provisioned before.

For device with Virtual Secure Element (VSE), a reset will be issued when 
running specified SE Manager APIs.

Warning: Loading the secure boot and tamper configuration, 128-bit AES key, 
public sign key and public command key into the SE or VSE are a ONE-TIME-ONLY 
process. These assignment operations are irrevocable and persist for the life of
the device.

Following SE Manager APIs are used in this example.
sl_se_init
sl_se_deinit
sl_se_init_command_context
sl_se_deinit_command_context
sl_se_get_se_version
sl_se_read_otp
sl_se_read_pubkey
sl_se_init_otp_key
sl_se_init_otp
sl_se_aes_crypt_ecb
sl_se_read_executed_command (VSE only)
sl_se_ack_command (VSE only)


How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. Upgrade the SE or VSE firmware to the latest version (see AN1222 for details)
   https://www.silabs.com/documents/public/application-notes/an1222-efr32xg2x-production-programming.pdf
3. Build the project and download to the Starter Kit
4. Open any terminal program and connect to the device's VCOM port
   (Note: If using puTTY, Local echo must be set to force on)
5. Follow the instructions shown on the console


Peripherals Used:
HFRCODPLL - 38 MHz
USART0/1  - 115200 baud, 8-N-1
GPIO
SE/VSE

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

Board:  Silicon Labs EFR32xG22 2.4 GHz 6 dBm Radio Board (BRD4182A) + 
        Wireless Starter Kit Mainboard (BRD4001A)
Device: EFR32MG22C224F512IM40
PA05 - USART1 TX
PA06 - USART1 RX
PB04 - Board Controller VCOM Enable
PB00 - WSTK push button PB0
PB01 - WSTK push button PB1