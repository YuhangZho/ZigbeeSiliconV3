se_manager_host_firmware_upgrade

This example uses the SE Manager API to upgrade the host firmware in Series 2
device (wireless SoC and modules).

The example redirects standard I/O to the virtual serial port (VCOM) of the
Starter Kit. By default the serial port setting is 115200 bps and 8-N-1
configuration.

The example has been instrumented with code to count the number of clock cycles
spent in upgrading the host firmware. The results are printed to stdout, i.e. 
the VCOM serial port console. This feature can be disabled by defining 
SE_MANAGER_PRINT=0 (default is 1) in IDE setting (Preprocessor->Defined 
symbols).

The start address of host firmware image (HOST_FIRMWARE_ADDR) is defined in 
host_firmware_image.h and the default value is 0x00040000.

The host firmware image (.bin) can be converted to a C source file by Bin2C.exe 
utility in Windows folder below.
C:\SiliconLabs\SimplicityStudio\v5\developer\sdks\gecko_sdk_suite\v3.0\util\
third_party\emwin\exe

Copy the host firmware image data array (discard the last NULL-0x00 character) 
in coverted C file to host_firmware_image array in app_host_firmware_image.c and
update the value of HOST_FIRMWARE_SIZE in app_host_firmware_image.h.

The current host firmware image in host_firmware_image.c is a VCOM example.

For device with Virtual Secure Element (VSE), a reset will be issued when 
running SE Manager APIs.

Note: The device should disconnect from the debugger when upgrading the host 
firmware.

Following SE Manager APIs are used in this example.
sl_se_init
sl_se_deinit
sl_se_init_command_context
sl_se_deinit_command_context
sl_se_get_upgrade_status_host_image
sl_se_upgrade_status_clear
sl_se_check_host_image
sl_se_apply_host_image
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