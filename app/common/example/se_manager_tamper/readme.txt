se_manager_tamper

This example uses the SE Manager API to demonstrate the tamper feature in Secure 
Vault device.

The example redirects standard I/O to the virtual serial port (VCOM) of the
Starter Kit. By default the serial port setting is 115200 bps and 8-N-1
configuration.

The example has been instrumented with code to count the number of clock cycles
spent in different tamper operations. The results are printed to stdout, i.e.
the VCOM serial port console. This feature can be disabled by defining 
SE_MANAGER_PRINT=0 (default is 1) in IDE setting (Preprocessor->Defined 
symbols).

Tamper Responses:
Level   Responses       Description
0       Ignore          No action is taken
1       Interrupt       The SETAMPERHOST interrupt on the host is triggered
2       Filter          A counter in the tamper filter is increased
4       Reset           The device is reset
7       Erase OTP       Erases the OTP configuration of the device
                        This makes the device and all wrapped secrets
                        unrecoverable!

Tamper Signals:
Number  Name                    Default level   User level in this example 
0       Reserved                -               -
1       Filter Counter          0               1
2       SE Watchdog             4               4
3       Reserved                -               -
4       SE RAM CRC              4               4
5       SE Hardfault            4               4
6       Reserved                -               -
7       Software Assertion      4               4
8       Reserved                -               4
9       UserCodeAuth            0               0
10      MailboxAuth             0               1
11      DCIAuth                 0               0
12      OTP Read                4               4
13      Reserved                -               -
14      Self-test               4               4
15      TRNG Monitor            0               1
16      PRS0                    0               1 (Push button PB0)
17      PRS1                    0               1 (None)
18      PRS2                    0               2 (Push button PB0)
19      PRS3                    0               2 (None)
20      PRS4                    0               4 (Push button PB1)
21      PRS5                    0               4 (Software)
22      PRS6                    0               7 (None)
23      PRS7                    0               7 (None)
24      DECOUPLE BOD            4               4
25      TempSensor              0               2
26      VGlitch Falling         0               2
27      VGlitch Rising          0               2
28      SecureLock              4               4
29      SE Debug                0               0
30      Digital glitch          0               2
31      SE ICACHE               4               4

Note: The device should disconnect from the debugger when running this example. 

Warning: The tamper configuration is located in immutable SE OTP. Once the 
tamper configuration is programmed, this assignment operation is irrevocable and
persist for the life of the device.	

See AN1247: Tamper Configuration and Use for more information about this 
example.

Following SE Manager APIs are used in this example.
sl_se_init
sl_se_deinit
sl_se_init_command_context
sl_se_deinit_command_context
sl_se_get_reset_cause
sl_se_get_status
sl_se_read_otp
sl_se_init_otp


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

Board:  Silicon Labs EFR32xG21B 2.4 GHz 10 dBm Radio Board (BRD4181C) + 
        Wireless Starter Kit Mainboard (BRD4001A)
Device: EFM32MG21B010F1024IM32
PA05 - USART0 TX
PA06 - USART0 RX
PD04 - Board Controller VCOM Enable
PB00 - WSTK push button PB0
PB01 - WSTK push button PB1