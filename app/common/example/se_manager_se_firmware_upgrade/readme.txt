se_manager_se_firmware_upgrade

This example uses the SE Manager API to upgrade the Secure Element (SE) or
Virtual Secure Element (VSE) firmware in Series 2 device (wireless SoC and 
modules).

The example redirects standard I/O to the virtual serial port (VCOM) of the
Starter Kit. By default the serial port setting is 115200 bps and 8-N-1
configuration.

The example has been instrumented with code to count the number of clock cycles
spent in upgrading the (V)SE firmware. The results are printed to stdout, i.e. 
the VCOM serial port console. This feature can be disabled by defining 
SE_MANAGER_PRINT=0 (default is 1) in IDE setting (Preprocessor->Defined 
symbols).

The start address of (V)SE firmware image (SE_FIRMWARE_ADDR) is defined in 
se_firmware_image.h and the default value is 0x00040000.

The (V)SE firmware image validation will fail if the image version is equal to 
or less than the current (V)SE firmware version.
      
The (V)SE firmware image must be flashed to the device in .seu format. The 
latest (V)SE firmware image (.sec and .hex) can be found in Windows folders 
below.
SE:
C:\SiliconLabs\SimplicityStudio\v5\offline\efr32\firmware\series2config1\
se_firmware_package
VSE:
C:\SiliconLabs\SimplicityStudio\v5\offline\efr32\firmware\series2config2\
se_firmware_package

The (V)SE firmware image (.sec) can be converted to a C source file by Bin2C.exe
utility in Windows folder below.
C:\SiliconLabs\SimplicityStudio\v5\developer\sdks\gecko_sdk_suite\v3.0\util\
third_party\emwin\exe

Copy the (V)SE firmware image data array in coverted C file to se_firmware_image
array in app_se_firmware_image.c and update the value of SE_FIRMWARE_SIZE in 
app_se_firmware_image.h.

The current SE and VSE firmware images in se_firmware_image.c are v1.2.1.

For device with VSE, a reset will be issued when running specified SE Manager 
APIs.

Note: The device should disconnect from the debugger when upgrading the (V)SE 
firmware.

Following SE Manager APIs are used in this example.
sl_se_init
sl_se_deinit
sl_se_init_command_context
sl_se_deinit_command_context
sl_se_get_se_version
sl_se_get_upgrade_status_se_image
sl_se_check_se_image
sl_se_apply_se_image
sl_se_read_executed_command (VSE only)
sl_se_ack_command (VSE only)


How To Test:
1. Update the kit's firmware from the Simplicity Launcher (if necessary)
2. The upgrade (V)SE firmware image version must be higher than the current 
   (V)SE firmware version
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