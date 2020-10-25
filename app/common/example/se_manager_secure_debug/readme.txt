se_manager_secure_debug

This example uses the SE Manager API to perform secure debug in EFR32xG21A and 
EFR32xG21B (wireless SoC and modules).

The example redirects standard I/O to the virtual serial port (VCOM) of the
Starter Kit. By default the serial port setting is 115200 bps and 8-N-1
configuration.

The example has been instrumented with code to count the number of clock cycles
spent in different secure debug operations. The results are printed to stdout,
i.e. the VCOM serial port console. This feature can be disabled by defining 
SE_MANAGER_PRINT=0 (default is 1) in IDE setting (Preprocessor->Defined 
symbols).

For demonstration purpose, a private command key is stored in device memory to 
sign the access certificate for secure debug unlock. The device's public command
key in the SE OTP must match with the public key of this private command key to
perform secure debug unlock.

The default private command key (cmd-unsafe-privkey.pem) can be found in Windows
folder below.
C:\SiliconLabs\SimplicityStudio\v5\developer\adapter_packs\secmgr\scripts\
offline

The public key of this default private command key is:
X - B1BC6F6FA56640ED522B2EE0F5B3CF7E5D48F60BE8148F0DC08440F0A4E1DCA4
Y - 7C04119ED6A1BE31B7707E5F9D001A659A051003E95E1B936F05C37EA793AD63

If the device does not have public command key in the SE OTP, the program will
program above public key to the device by pressing the push button PB0 two 
times.

User can change the private command key (private_command_key[]) in 
app_se_manager_secure_debug.c to match with the device's public command key in 
the SE OTP for secure debug unlock test.

The hard-coded private command key is an insecure method so user should find a 
way to import the signed access certificate for secure debug unlock.

Note: The device should disconnect from the debugger when locking or unlocking 
the debug interface.

Warning: Loading a public command key into the SE and disable the device erase 
are a ONE-TIME-ONLY process. Both of these assignment operations are irrevocable
and persist for the life of the device.
	  
See AN1190: Series 2 Secure Debug for more information about secure debug.

Following SE Manager APIs are used in this example.
sl_se_init
sl_se_deinit
sl_se_init_command_context
sl_se_deinit_command_context
sl_se_get_status
sl_se_get_debug_lock_status
sl_se_validate_key
sl_se_get_storage_size
sl_se_generate_key
sl_se_export_public_key
sl_se_read_pubkey
sl_se_init_otp_key
sl_se_apply_debug_lock
sl_se_erase_device
sl_se_enable_secure_debug
sl_se_disable_secure_debug
sl_se_disable_device_erase
sl_se_set_debug_options
sl_se_get_serialnumber
sl_se_get_challenge
sl_se_ecc_sign
sl_se_open_debug
sl_se_roll_challenge


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