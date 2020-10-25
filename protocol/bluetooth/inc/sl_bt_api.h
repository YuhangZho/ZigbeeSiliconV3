/***************************************************************************//**
 * @brief SL_BT_API command declarations
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef SL_BT_API_H
#define SL_BT_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "sl_bt_types.h"
#include "sl_status.h"



#if defined(__IAR_SYSTEMS_ICC__)
  #define SL_BT_DEPRECATED_API #pragma deprecated
#elif defined(__GNUC__)
  #define SL_BT_DEPRECATED_API __attribute__((deprecated))
#else
  #define SL_BT_DEPRECATED_API
#endif

#define SL_BT_MSG_ID(HDR) ((HDR)&0xffff00f8)
#define SL_BT_MSG_HEADER_LEN (4)
#define SL_BT_MSG_LEN(HDR) ((((HDR)&0x7)<<8)|(((HDR)&0xff00)>>8))
#define SL_BT_BIT_ENCRYPTED (1 << 6) // Bit indicating whether the packet is encrypted
#define SL_BT_MSG_ENCRYPTED(HDR) ((HDR) & SL_BT_BIT_ENCRYPTED)

/**
 * Internal command for handling api commands
 */
typedef void (*sl_bt_handler)(const void*);

/**
 * Internal function for setting the command handler function. Used by API library.
 *
 * @param cmd_handler_delegate Pointer to command handler delegate function
 */
void sli_bt_set_cmd_handler_delegate(void (*cmd_handler_delegate)(uint32_t,
                                                                  sl_bt_handler,
                                                                  const void*));


/**
 * @addtogroup sl_bt_dfu Device Firmware Update
 * @{
 *
 * @brief Device Firmware Update
 *
 * These commands and events are related to controlling firmware updates over
 * the configured host interface and are available only when the device is
 * booted in DFU mode. <b>DFU process:</b>
 *   1. Boot device to DFU mode with @ref sl_bt_dfu_reset command
 *   2. Wait for @ref sl_bt_evt_dfu_boot event
 *   3. Send command @ref sl_bt_dfu_flash_set_address command to start the
 *      firmware update
 *   4. Upload the firmware with @ref sl_bt_dfu_flash_upload commands until all
 *      data is uploaded
 *   5. Send @ref sl_bt_dfu_flash_upload_finish command when all data is
 *      uploaded
 *   6. Finalize DFU firmware update with @ref sl_bt_dfu_reset command
 *
 * DFU mode is using the UART baudrate set in bootloader.
 */

/***************************************************************************//**
 *
 * Reset the system. The command does not have a response but it triggers one of
 * the boot events (normal reset or boot to DFU mode) after re-boot.
 *
 * @param[in] dfu Enum @ref system_boot_mode_t. Boot mode. Values:
 *     - <b>system_boot_mode_normal (0x0):</b> Boot to normal mode
 *     - <b>system_boot_mode_uart_dfu (0x1):</b> Boot to UART DFU mode
 *     - <b>system_boot_mode_ota_dfu (0x2):</b> Boot to OTA DFU mode
 *
 * @b Events
 *   - @ref sl_bt_evt_system_boot - Sent after the device has booted in normal
 *     mode
 *   - @ref sl_bt_evt_dfu_boot - Sent after the device has booted in UART DFU
 *     mode
 *
 ******************************************************************************/
void sl_bt_dfu_reset(uint8_t dfu);

/***************************************************************************//**
 *
 * After re-booting the local device in DFU mode, this command defines the
 * starting address on the flash where the new firmware will be written.
 *
 * @param[in] address The offset in the flash where the new firmware is uploaded
 *   to. Always use the value 0x00000000.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_dfu_flash_set_address(uint32_t address);

/***************************************************************************//**
 *
 * Upload the whole firmware image file into the Bluetooth device. The passed
 * data length must be a multiple of 4 bytes. Because the BGAPI command payload
 * size is limited, multiple commands need to be issued one after the other
 * until the whole .bin firmware image file is uploaded to the device. After
 * each command, the next address of the flash sector in memory to write to is
 * automatically updated by the bootloader.
 *
 * @param[in] data_len Array length
 * @param[in] data An array of data which will be written onto the flash.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_dfu_flash_upload(size_t data_len, const uint8_t* data);

/***************************************************************************//**
 *
 * Inform the device that the DFU file is fully uploaded. To return the device
 * back to normal mode, issue the command @ref sl_bt_dfu_reset.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_dfu_flash_upload_finish();

/** @} */ // end addtogroup sl_bt_dfu

/**
 * @addtogroup sl_bt_system System
 * @{
 *
 * @brief System
 *
 * Commands and events in this class can be used to access and query the local
 * device.
 */

/***************************************************************************//**
 *
 * Verify whether the communication between the host and the device is
 * functional.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_hello();

/***************************************************************************//**
 *
 * Reset the system. The command does not have a response but it triggers one of
 * the boot events (normal reset or boot to DFU mode) depending on the selected
 * boot mode.
 *
 * @param[in] dfu Enum @ref system_boot_mode_t. Boot mode. Values:
 *     - <b>system_boot_mode_normal (0x0):</b> Boot to normal mode
 *     - <b>system_boot_mode_uart_dfu (0x1):</b> Boot to UART DFU mode
 *     - <b>system_boot_mode_ota_dfu (0x2):</b> Boot to OTA DFU mode
 *
 * @b Events
 *   - @ref sl_bt_evt_system_boot - Sent after the device has booted in normal
 *     mode.
 *   - @ref sl_bt_evt_dfu_boot - Sent after the device has booted in UART DFU
 *     mode.
 *
 ******************************************************************************/
void sl_bt_system_reset(uint8_t dfu);

/***************************************************************************//**
 *
 * Force radio to idle state and allow device to sleep. Advertising, scanning,
 * connections, and software timers are halted by this command. Halted
 * operations resume after calling this command with parameter 0. Connections
 * stay alive if system is resumed before connection supervision timeout.
 *
 * Use this command only for a short time period (a few seconds at maximum).
 * Although it halts Bluetooth activity, all tasks and operations still exist
 * inside the stack with their own concepts of time. Halting the system for a
 * long time period may have negative consequences on stack's internal states.
 *
 * <b>NOTE:</b> The software timer is also halted. Hardware interrupts are the
 * only way to wake up from energy mode 2 when the system is halted.
 *
 * @param[in] halt Values:
 *     - <b>1:</b> halt
 *     - <b>0:</b> resume
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_halt(uint8_t halt);

/***************************************************************************//**
 *
 * Send configuration data to the link layer. This command fine tunes low-level
 * Bluetooth operations.
 *
 * @param[in] key @parblock
 *   Enum @ref system_linklayer_config_key_t. Key to configure. Values:
 *     - <b>system_linklayer_config_key_halt (0x1):</b> Same as system_halt
 *       command, value-0 Stop Radio 1- Start Radio
 *     - <b>system_linklayer_config_key_priority_range (0x2):</b> Sets the RAIL
 *       priority_mapping offset field of the link layer priority configuration
 *       structure to the first byte of the value field.
 *     - <b>system_linklayer_config_key_scan_channels (0x3):</b> Sets channels
 *       to scan on. The first byte of the value is the channel map. 0x1 =
 *       Channel 37, 0x2 = Channel 38, 0x4 = Channel 39
 *     - <b>system_linklayer_config_key_set_flags (0x4):</b> Sets the link layer
 *       configuration flags. The value is a little endian 32-bit integer. Flag
 *       Values:
 *         - 0x00000001 - Disable Feature Exchange when slave
 *         - 0x00000002 - Disable Feature Exchange when master
 *
 *     - <b>system_linklayer_config_key_clr_flags (0x5):</b> Value is flags to
 *       clear. Flags are the same as in SET_FLAGS command.
 *     - <b>system_linklayer_config_key_set_afh_interval (0x7):</b> Set
 *       afh_scan_interval field of Link Layer priority configuration structure.
 *     - <b>system_linklayer_config_key_set_priority_table (0x9):</b> Value
 *       contains priority table to be copied over the existing table. If value
 *       is smaller than full table then only those values are updated. see @ref
 *       sl_btbluetooth_ll_priorities struct for the definition of priority
 *       table.
 *   @endparblock
 * @param[in] data_len Array length
 * @param[in] data Configuration data. Length and contents of the data field
 *   depend on the key value used.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_linklayer_configure(uint8_t key,
                                             size_t data_len,
                                             const uint8_t* data);

/***************************************************************************//**
 *
 * Set the global maximum TX power for Bluetooth. The returned value is the
 * selected maximum output power level after applying the RF path compensation.
 * If the GATT server contains a TX power service, the TX Power Level attribute
 * will be updated accordingly.
 *
 * The selected power level may be less than the specified value if the device
 * does not meet the power requirements. For Bluetooth connections, the maximum
 * TX power is limited to 10 dBm if Adaptive Frequency Hopping (AFH) is not
 * enabled.
 *
 * By default, the global maximum TX power value is 8 dBm.
 *
 * <b>NOTE:</b> Do not use this command while advertising, scanning, or during
 * connection.
 *
 * @param[in] power TX power in 0.1 dBm steps. For example, the value of 10 is 1
 *   dBm and 55 is 5.5 dBm.
 * @param[out] set_power The selected maximum power level
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_set_max_tx_power(int16_t power, int16_t *set_power);

/***************************************************************************//**
 *
 * Set the device's Bluetooth identity address. The address can be a public
 * device address or a static device address. A valid address set with this
 * command will be written into persistent storage using NVM keys. The stack
 * returns an error if the static device address does not conform to the
 * Bluetooth specification.
 *
 * The new address will be effective in the next system reboot. The stack will
 * use the address in the NVM keys when present. Otherwise, it uses the default
 * Bluetooth public device address which is programmed at production.
 *
 * The stack treats 00:00:00:00:00:00 and ff:ff:ff:ff:ff:ff as invalid
 * addresses. Therefore, passing one of them into this command will cause the
 * stack to delete the NVM keys and use the default address in the next system
 * reboot.
 *
 * <b>Note:</b> Because the NVM keys are located in flash and flash wearing can
 * occur, avoid calling this command regularly.
 *
 * @param[in] address Bluetooth identity address in little endian format
 * @param[in] type Address type
 *     - <b>0:</b> Public device address
 *     - <b>1:</b> Static device address
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_set_identity_address(bd_addr address, uint8_t type);

/***************************************************************************//**
 *
 * Read the Bluetooth identity address used by the device, which can be a public
 * or random static device address.
 *
 * @param[out] address Bluetooth public address in little endian format
 * @param[out] type Address type
 *     - <b>0:</b> Public device address
 *     - <b>1:</b> Static random address
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_get_identity_address(bd_addr *address, uint8_t *type);

/***************************************************************************//**
 *
 * Get random data up to 16 bytes.
 *
 * @param[in] length Length of random data. Maximum length is 16 bytes.
 * @param[in] max_data_size Size of output buffer passed in @p data
 * @param[out] data_len On return, set to the length of output data written to
 *   @p data
 * @param[out] data Random data
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_get_random_data(uint8_t length,
                                         size_t max_data_size,
                                         size_t *data_len,
                                         uint8_t *data);

/***************************************************************************//**
 *
 * Write data into the system data buffer. Data will be appended to the end of
 * existing data.
 *
 * @param[in] data_len Array length
 * @param[in] data Data to write
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_data_buffer_write(size_t data_len,
                                           const uint8_t* data);

/***************************************************************************//**
 *
 * Remove all data from the system data buffer.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_data_buffer_clear();

/***************************************************************************//**
 *
 * Get packet and error counters. Passing a non-zero value also resets counters.
 *
 * @param[in] reset Reset counters if the parameter value is not zero.
 * @param[out] tx_packets The number of successfully transmitted packets
 * @param[out] rx_packets The number of successfully received packets
 * @param[out] crc_errors The number of received packets with CRC errors
 * @param[out] failures The number of radio failures, such as aborted TX/RX
 *   packets, scheduling failures, and so on.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_get_counters(uint8_t reset,
                                      uint16_t *tx_packets,
                                      uint16_t *rx_packets,
                                      uint16_t *crc_errors,
                                      uint16_t *failures);

/***************************************************************************//**
 *
 * Start a software timer. Multiple concurrent timers can be running
 * simultaneously. 256 unique timer handles (IDs) are available. The maximum
 * number of concurrent timers is configurable at device initialization. Up to
 * 16 concurrent timers can be configured. The default configuration is 4. As
 * the RAM for storing timer data is pre-allocated at initialization, an
 * application should not configure the amount more than it needs for minimizing
 * RAM usage.
 *
 * @param[in] time @parblock
 *   Frequency interval of events, which indicates how often to send events in
 *   hardware clock ticks (1 second is equal to 32768 ticks).
 *
 *   The smallest interval value supported is 328, which is around 10
 *   milliseconds. Any parameters between 0 and 328 will be rounded up to 328.
 *   The maximum value is 2147483647, which corresponds to about 18.2 hours.
 *
 *   If @p time is 0, removes the scheduled timer with the same handle.
 *   @endparblock
 * @param[in] handle Timer handle to use, which is returned in timeout event
 * @param[in] single_shot Timer mode. Values:
 *     - <b>0:</b> false (timer is repeating)
 *     - <b>1:</b> true (timer runs only once)
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_system_soft_timer - Sent after this timer has lapsed.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_set_soft_timer(uint32_t time,
                                        uint8_t handle,
                                        uint8_t single_shot);

/***************************************************************************//**
 *
 * Start a software timer with slack. The slack parameter allows the stack to
 * optimize wakeups and save power. The timer event is triggered between time
 * and time + @p slack. See also description of @ref sl_bt_system_set_soft_timer
 * command.
 *
 * @param[in] time @parblock
 *   Interval between how often to send events in hardware clock ticks (1 second
 *   is equal to 32768 ticks).
 *
 *   The smallest interval value supported is 328, which is around 10
 *   milliseconds. Any parameters between 0 and 328 will be rounded up to 328.
 *   The maximum value is 2147483647, which corresponds to about 18.2 hours.
 *
 *   If @p time is 0, removes the scheduled timer with the same handle.
 *   @endparblock
 * @param[in] slack Slack time in hardware clock ticks
 * @param[in] handle Timer handle to use, which is returned in timeout event
 * @param[in] single_shot Timer mode. Values:
 *     - <b>0:</b> false (timer is repeating)
 *     - <b>1:</b> true (timer runs only once)
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_system_soft_timer - Sent after this timer has lapsed.
 *
 ******************************************************************************/
sl_status_t sl_bt_system_set_lazy_soft_timer(uint32_t time,
                                             uint32_t slack,
                                             uint8_t handle,
                                             uint8_t single_shot);

/** @} */ // end addtogroup sl_bt_system

/**
 * @addtogroup sl_bt_gap GAP
 * @{
 *
 * @brief GAP
 *
 * The commands and events in this class are related to the Generic Access
 * Profile (GAP) in Bluetooth.
 */

/***************************************************************************//**
 *
 * Enable or disable the privacy feature on all GAP roles. New privacy mode will
 * take effect for advertising next time advertising is enabled, for scanning
 * next time scanning is enabled, and for initiating on the next open connection
 * command. When privacy is enabled and the device is advertising or scanning,
 * the stack will maintain a periodic timer with the specified time interval as
 * a timeout value. At each timeout, the stack will generate a new private
 * resolvable address and use it in advertising data packets and scanning
 * requests.
 *
 * By default, privacy feature is disabled.
 *
 * @param[in] privacy Values:
 *     - <b>0:</b> Disable privacy
 *     - <b>1:</b> Enable privacy
 * @param[in] interval The minimum time interval between a private address
 *   change. This parameter is ignored if this command is issued to disable
 *   privacy mode. Values:
 *     - <b>0:</b> Use default interval, 15 minutes
 *     - <b>others:</b> The time interval in minutes
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gap_set_privacy_mode(uint8_t privacy, uint8_t interval);

/***************************************************************************//**
 *
 * Specify a channel classification for data channels. This classification
 * persists until overwritten with a subsequent command or until the system is
 * reset.
 *
 * @param[in] channel_map_len Array length
 * @param[in] channel_map @parblock
 *   This parameter is 5 bytes and contains 37 1-bit fields.
 *   The nth such field (in the range 0 to 36) contains the value for the link
 *   layer channel index n.
 *     - <b>0:</b> Channel n is bad.
 *     - <b>1:</b> Channel n is unknown.
 *
 *   The rest of most significant bits are reserved for future use and shall be
 *   set to 0.
 *   At least two channels shall be marked as unknown.
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gap_set_data_channel_classification(size_t channel_map_len,
                                                      const uint8_t* channel_map);

/***************************************************************************//**
 *
 * Enable or disable whitelisting. The setting will be effective the next time
 * that scanning is enabled. To add devices to the whitelist, either bond with
 * the device or add it manually with @ref sl_bt_sm_add_to_whitelist.
 *
 * @param[in] enable 1 enable, 0 disable whitelisting.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gap_enable_whitelisting(uint8_t enable);

/** @} */ // end addtogroup sl_bt_gap

/**
 * @addtogroup sl_bt_advertiser Advertiser
 * @{
 *
 * @brief Advertiser
 *
 * The commands and events in this class are related to advertising
 * functionalities in GAP peripheral and broadcaster roles.
 */

/***************************************************************************//**
 *
 * Create an advertising set. The handle of the created advertising set is
 * returned in response.
 *
 * @param[out] handle Advertising set handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_create_set(uint8_t *handle);

/***************************************************************************//**
 *
 * Set the advertising timing parameters of the given advertising set. This
 * setting will take effect next time that advertising is enabled.
 *
 * @param[in] handle Advertising set handle
 * @param[in] interval_min @parblock
 *   Minimum advertising interval. Value in units of 0.625 ms
 *     - Range: 0x20 to 0xFFFF
 *     - Time range: 20 ms to 40.96 s
 *
 *   Default value: 100 ms
 *   @endparblock
 * @param[in] interval_max @parblock
 *   Maximum advertising interval. Value in units of 0.625 ms
 *     - Range: 0x20 to 0xFFFF
 *     - Time range: 20 ms to 40.96 s
 *     - Note: interval_max should be bigger than interval_min
 *
 *   Default value: 200 ms
 *   @endparblock
 * @param[in] duration @parblock
 *   Advertising duration for this advertising set. Value 0 indicates no
 *   advertising duration limit and advertising continues until it is disabled.
 *   A non-zero value sets the duration in units of 10 ms. The duration begins
 *   at the start of the first advertising event of this advertising set.
 *     - Range: 0x0001 to 0xFFFF
 *     - Time range: 10 ms to 655.35 s
 *
 *   Default value: 0
 *   @endparblock
 * @param[in] maxevents @parblock
 *   If non-zero, indicates the maximum number of advertising events to send
 *   before the advertiser is stopped. Value 0 indicates no maximum number
 *   limit.
 *
 *   Default value: 0
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_timing(uint8_t handle,
                                        uint32_t interval_min,
                                        uint32_t interval_max,
                                        uint16_t duration,
                                        uint8_t maxevents);

/***************************************************************************//**
 *
 * Set advertising PHYs of the given advertising set. This setting will take
 * effect next time that advertising is enabled. The invalid parameter error is
 * returned if a PHY value is invalid or the device does not support a given
 * PHY.
 *
 * @param[in] handle Advertising set handle
 * @param[in] primary_phy @parblock
 *   Enum @ref gap_phy_type_t. The PHY on which the advertising packets are
 *   transmitted on the primary advertising channel. If legacy advertising PDUs
 *   are used, 1M PHY must be used. Values:
 *     - <b>gap_1m_phy (0x1):</b> 1M PHY
 *     - <b>gap_coded_phy (0x4):</b> Coded PHY
 *
 *   Default value: gap_1m_phy
 *   @endparblock
 * @param[in] secondary_phy @parblock
 *   Enum @ref gap_phy_type_t. The PHY on which the advertising packets are
 *   transmitted on the secondary advertising channel. Values:
 *     - <b>gap_1m_phy (0x1):</b> 1M PHY
 *     - <b>gap_2m_phy (0x2):</b> 2M PHY
 *     - <b>gap_coded_phy (0x4):</b> Coded PHY
 *
 *   Default value: gap_1m_phy
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_phy(uint8_t handle,
                                     uint8_t primary_phy,
                                     uint8_t secondary_phy);

/***************************************************************************//**
 *
 * Set the primary advertising channel map of the given advertising set. This
 * setting will take effect next time that advertising is enabled.
 *
 * @param[in] handle Advertising set handle
 * @param[in] channel_map @parblock
 *   Advertising channel map which determines which of the three channels will
 *   be used for advertising. This value is given as a bitmask. Values:
 *     - <b>1:</b> Advertise on CH37
 *     - <b>2:</b> Advertise on CH38
 *     - <b>3:</b> Advertise on CH37 and CH38
 *     - <b>4:</b> Advertise on CH39
 *     - <b>5:</b> Advertise on CH37 and CH39
 *     - <b>6:</b> Advertise on CH38 and CH39
 *     - <b>7:</b> Advertise on all channels
 *
 *   Recommended value: 7
 *
 *   Default value: 7
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_channel_map(uint8_t handle,
                                             uint8_t channel_map);

/***************************************************************************//**
 *
 * Limit the maximum advertising TX power on the given advertising set. If the
 * value goes over the global value that was set using @ref
 * sl_bt_system_set_max_tx_power command, the global value will be the maximum
 * limit. The maximum TX power of legacy advertising is further constrained to
 * be less than +10 dBm. Extended advertising TX power can be +10 dBm and over
 * if Adaptive Frequency Hopping is enabled.
 *
 * This setting will take effect next time advertising is enabled.
 *
 * By default, maximum advertising TX power is limited by the global value.
 *
 * @param[in] handle Advertising set handle
 * @param[in] power TX power in 0.1 dBm steps. For example, the value of 10 is 1
 *   dBm and 55 is 5.5 dBm.
 * @param[out] set_power The selected maximum advertising TX power
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_tx_power(uint8_t handle,
                                          int16_t power,
                                          int16_t *set_power);

/***************************************************************************//**
 *
 * Enable or disable the scan request notification of a given advertising set.
 * This setting will take effect next time that advertising is enabled.
 *
 * @param[in] handle Advertising set handle
 * @param[in] report_scan_req @parblock
 *   If non-zero, enables scan request notification and scan requests will be
 *   reported as events.
 *
 *   Default value: 0
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_advertiser_scan_request - Triggered when a scan request is
 *     received during advertising if the scan request notification is enabled
 *     by this command.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_report_scan_request(uint8_t handle,
                                                     uint8_t report_scan_req);

/***************************************************************************//**
 *
 * Set the advertiser on an advertising set to use a random address. This
 * overrides the default advertiser address which is either the public device
 * address programmed at production or the address written into persistent
 * storage using @ref sl_bt_system_set_identity_address command. This setting is
 * stored in RAM only and does not change the identity address in persistent
 * storage.
 *
 * When setting a resolvable random address, the address parameter is ignored.
 * The stack generates a private resolvable random address and set it as the
 * advertiser address. The generated address is returned in the response.
 *
 * To use the default advertiser address, remove this setting using @ref
 * sl_bt_advertiser_clear_random_address command.
 *
 * Wrong state error is returned if advertising has been enabled on the
 * advertising set. Invalid parameter error is returned if the advertising set
 * handle is invalid or the address does not conforms to the Bluetooth
 * specification.
 *
 * @param[in] handle Advertising set handle
 * @param[in] addr_type Address type:
 *     - <b>1:</b> Static device address
 *     - <b>2:</b> Private resolvable random address
 *     - <b>3:</b> Private non-resolvable random address. This type can only be
 *       used for non-connectable advertising.
 * @param[in] address The random address to set. Ignore this field when setting
 *   a resolvable random address.
 * @param[out] address_out The resolvable random address set for the advetiser.
 *   Ignore this field when setting other types of random address.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_random_address(uint8_t handle,
                                                uint8_t addr_type,
                                                bd_addr address,
                                                bd_addr *address_out);

/***************************************************************************//**
 *
 * Clear the random address previously set for the advertiser address on an
 * advertising set. A random address can be set using @ref
 * sl_bt_advertiser_set_random_address command. The default advertiser address
 * will be used after this operation.
 *
 * Wrong state error is returned if advertising has been enabled on the
 * advertising set. Invalid parameter error is returned if the advertising set
 * handle is invalid.
 *
 * @param[in] handle Advertising set handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_clear_random_address(uint8_t handle);

/***************************************************************************//**
 *
 * Enable advertising configuration flags on the given advertising set. The
 * configuration change will take effect next time that advertising is enabled.
 *
 * These configuration flags can be disabled using @ref
 * sl_bt_advertiser_clear_configuration.
 *
 * @param[in] handle Advertising set handle
 * @param[in] configurations @parblock
 *   Advertising configuration flags to enable. This value can be a bitmask of
 *   multiple flags. Flags:
 *     - <b>1 (Bit 0):</b> Use legacy advertising PDUs.
 *     - <b>2 (Bit 1):</b> Omit advertiser's address from all PDUs (anonymous
 *       advertising). This flag is effective only in extended advertising.
 *     - <b>4 (Bit 2):</b> Use gap_non_resolvable address type. Advertising must
 *       be in non-connectable mode if this configuration is enabled.
 *     - <b>8 (Bit 3):</b> Include TX power in advertising packets. This flag is
 *       effective only in extended advertising.
 *
 *   Default value: 1
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_configuration(uint8_t handle,
                                               uint32_t configurations);

/***************************************************************************//**
 *
 * Disable advertising configuration flags on the given advertising set. The
 * configuration change will take effect next time that advertising is enabled.
 *
 * These configuration flags can be enabled using @ref
 * sl_bt_advertiser_set_configuration.
 *
 * @param[in] handle Advertising set handle
 * @param[in] configurations Advertising configuration flags to disable. This
 *   value can be a bitmask of multiple flags. See @ref
 *   sl_bt_advertiser_set_configuration for possible flags.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_clear_configuration(uint8_t handle,
                                                 uint32_t configurations);

/***************************************************************************//**
 *
 * Set user-defined data in advertising packets, scan response packets, or
 * periodic advertising packets. Maximum 31 bytes of data can be set for legacy
 * advertising. Maximum 191 bytes of data can be set for connectable extended
 * advertising. Maximum 253 bytes of data can be set for periodic and
 * non-connectable extended advertising. For setting longer advertising data,
 * use command @ref sl_bt_advertiser_set_long_data.
 *
 * If advertising mode is currently enabled, the new advertising data will be
 * used immediately. Advertising mode can be enabled using command @ref
 * sl_bt_advertiser_start. Periodic advertising mode can be enabled using
 * command @ref sl_bt_advertiser_start_periodic_advertising.
 *
 * The invalid parameter error will be returned in the following situations:
 *   - Data length is more than 31 bytes but the advertiser can only advertise
 *     using legacy advertising PDUs.
 *   - Data is too long to fit into a single advertisement.
 *   - Set data of the advertising data packet when the advertiser is
 *     advertising in scannable mode using extended advertising PDUs.
 *   - Set data of the scan response data packet when the advertiser is
 *     advertising in connectable mode using extended advertising PDUs.
 *
 * Note that the user-defined data may be overwritten by the system when the
 * advertising is later enabled in a discoverable mode other than user_data.
 *
 * @param[in] handle Advertising set handle
 * @param[in] packet_type This value selects whether data is intended for
 *   advertising packets, scan response packets or periodic advertising packets.
 *     - <b>0:</b> Advertising packets
 *     - <b>1:</b> Scan response packets
 *     - <b>8:</b> Periodic advertising packets
 * @param[in] adv_data_len Array length
 * @param[in] adv_data Data to be set
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_data(uint8_t handle,
                                      uint8_t packet_type,
                                      size_t adv_data_len,
                                      const uint8_t* adv_data);

/***************************************************************************//**
 *
 * Set advertising data for a specified packet type and advertising set. Data
 * currently in the system data buffer will be extracted as the advertising
 * data. The buffer will be emptied after this command regardless of the
 * completion status.
 *
 * Prior to calling this command, add data to the buffer with one or multiple
 * calls of @ref sl_bt_system_data_buffer_write.
 *
 * Maximum 31 bytes of data can be set for legacy advertising. Maximum 191 bytes
 * of data can be set for connectable extended advertising. Maximum 1650 bytes
 * of data can be set for periodic and non-connectable extended advertising, but
 * advertising parameters may limit the amount of data that can be sent in a
 * single advertisement.
 *
 * See @ref sl_bt_advertiser_set_data for more details on advertising data.
 *
 * @param[in] handle Advertising set handle
 * @param[in] packet_type This value selects whether data is intended for
 *   advertising packets, scan response packets, or periodic advertising
 *   packets. Values:
 *     - <b>0:</b> Advertising packets
 *     - <b>1:</b> Scan response packets
 *     - <b>8:</b> Periodic advertising packets
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_set_long_data(uint8_t handle, uint8_t packet_type);

/***************************************************************************//**
 *
 * Start advertising of a given advertising set with specified discoverable and
 * connectable modes.
 *
 * The number of concurrent advertising is limited by MAX_ADVERTISERS
 * configuration.
 *
 * The number of concurrent connectable advertising is also limited by
 * MAX_CONNECTIONS configuration. For example, only one connectable advertising
 * can be enabled if the device has (MAX_CONNECTIONS - 1) connections when this
 * command is called. The limitation does not apply to non-connectable
 * advertising.
 *
 * The default advertising configuration in the stack is set to using legacy
 * advertising PDUs on 1M PHY. The stack will automatically select extended
 * advertising PDUs if either of the following has occurred with the default
 * configuration:
 *   1. The connectable mode is set to advertiser_connectable_non_scannable.
 *   2. The primary advertising PHY is set to Coded PHY by @ref
 *      sl_bt_advertiser_set_phy.
 *   3. The user advertising data length is more than 31 bytes.
 *   4. Periodic advertising is enabled.
 *
 * If currently set parameters can't be used, an error is returned.
 * Specifically, this command fails with the connection limit exceeded error if
 * it causes the number of connections exceeding the configured MAX_CONNECTIONS
 * value. It fails with the invalid parameter error if one of the following use
 * cases occurs:
 *   1. Non-resolvable random address is used but the connectable mode is
 *      advertiser_connectable_scannable or
 *      advertiser_connectable_non_scannable.
 *   2. advertiser_connectable_non_scannable is the connectable mode but using
 *      legacy advertising PDUs has been explicitly enabled with command @ref
 *      sl_bt_advertiser_set_configuration.
 *   3. Coded PHY is the primary advertising PHY but using legacy advertising
 *      PDUs has been explicitly enabled with command @ref
 *      sl_bt_advertiser_set_configuration.
 *   4. advertiser_connectable_scannable is the connectable mode but using
 *      extended advertising PDUs has been explicitly enabled or the primary
 *      advertising PHY is set to Coded PHY.
 *
 * If advertising is enabled in user_data mode, use @ref
 * sl_bt_advertiser_set_data to set advertising and scan response data before
 * issuing this command. When advertising is enabled in modes other than
 * user_data, advertising and scan response data is generated by the stack using
 * the following procedure:
 *   1. Add a flags field to advertising data.
 *   2. Add a TX power level field to advertising data if the TX power service
 *      exists in the local GATT database.
 *   3. Add a slave connection interval range field to advertising data if the
 *      GAP peripheral preferred connection parameters characteristic exists in
 *      the local GATT database.
 *   4. Add a list of 16-bit service UUIDs to advertising data if there are one
 *      or more 16-bit service UUIDs to advertise. The list is complete if all
 *      advertised 16-bit UUIDs are in advertising data. Otherwise, the list is
 *      incomplete.
 *   5. Add a list of 128-bit service UUIDs to advertising data if there are one
 *      or more 128-bit service UUIDs to advertise and there is still free space
 *      for this field. The list is complete if all advertised 128-bit UUIDs are
 *      in advertising data. Otherwise, the list is incomplete. Note that an
 *      advertising data packet can contain at most one 128-bit service UUID.
 *   6. Try to add the full local name to advertising data if the device is not
 *      in privacy mode. If the full local name does not fit into the remaining
 *      free space, the advertised name is a shortened version by cutting off
 *      the end if the free space has at least 6 bytes. Otherwise, the local
 *      name is added to scan response data.
 *
 * Event @ref sl_bt_evt_connection_opened will be received when a remote device
 * opens a connection to the advertiser on this advertising set and also
 * advertising on the given set stops.
 *
 * Event @ref sl_bt_evt_advertiser_timeout will be received when the number of
 * advertising events set by @ref sl_bt_advertiser_set_timing command is done
 * and advertising with the current set has stopped.
 *
 * @param[in] handle Advertising set handle
 * @param[in] discover Enum @ref advertiser_discoverable_mode_t. Discoverable
 *   mode. Values:
 *     - <b>advertiser_non_discoverable (0x0):</b> Not discoverable
 *     - <b>advertiser_limited_discoverable (0x1):</b> Discoverable using both
 *       limited and general discovery procedures
 *     - <b>advertiser_general_discoverable (0x2):</b> Discoverable using
 *       general discovery procedure
 *     - <b>advertiser_broadcast (0x3):</b> Device is not discoverable in either
 *       limited or generic discovery procedure but may be discovered using the
 *       Observation procedure.
 *     - <b>advertiser_user_data (0x4):</b> Send advertising and/or scan
 *       response data defined by the user. The limited/general discoverable
 *       flags are defined by the user.
 * @param[in] connect Enum @ref advertiser_connectable_mode_t. Connectable mode.
 *   Values:
 *     - <b>advertiser_non_connectable (0x0):</b> Non-connectable non-scannable.
 *     - <b>advertiser_directed_connectable (0x1):</b> Directed connectable
 *       <b>(RESERVED, DO NOT USE)</b>
 *     - <b>advertiser_connectable_scannable (0x2):</b> Undirected connectable
 *       scannable. This mode can only be used in legacy advertising PDUs.
 *     - <b>advertiser_scannable_non_connectable (0x3):</b> Undirected scannable
 *       (Non-connectable but responds to scan requests)
 *     - <b>advertiser_connectable_non_scannable (0x4):</b> Undirected
 *       connectable non-scannable. This mode can only be used in extended
 *       advertising PDUs.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_advertiser_timeout - Triggered when the number of
 *     advertising events set by @ref sl_bt_advertiser_set_timing command is
 *     done and advertising has stopped on the given advertising set.
 *   - @ref sl_bt_evt_connection_opened - Triggered when a remote device opens a
 *     connection to the advertiser on the specified advertising set and also
 *     advertising with the current set stops.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_start(uint8_t handle,
                                   uint8_t discover,
                                   uint8_t connect);

/***************************************************************************//**
 *
 * Stop the advertising of the given advertising set. Counterpart with @ref
 * sl_bt_advertiser_start.
 *
 * This command does not affect the enable state of the periodic advertising
 * set, i.e., periodic advertising is not stopped.
 *
 * @param[in] handle Advertising set handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_stop(uint8_t handle);

/***************************************************************************//**
 *
 * Start periodic advertising on the given advertising set. The stack enables
 * the advertising set automatically if the set was not enabled and the set can
 * advertise using extended advertising PDUs beside the syncInfo (which is
 * needed for the periodic advertising).
 *
 * The invalid parameter error is returned if the application has configured
 * legacy advertising PDUs or anonymous advertising, or the advertising set is
 * enabled using legacy advertising PDUs.
 *
 * To stop periodic advertising, use @ref
 * sl_bt_advertiser_stop_periodic_advertising command with the handle received
 * in response from this command.
 *
 * @param[in] handle Advertising set handle
 * @param[in] interval_min @parblock
 *   Minimum periodic advertising interval. Value in units of 1.25 ms
 *     - Range: 0x06 to 0xFFFF
 *     - Time range: 7.5 ms to 81.92 s
 *
 *   Default value: 100 ms
 *   @endparblock
 * @param[in] interval_max @parblock
 *   Maximum periodic advertising interval. Value in units of 1.25 ms
 *     - Range: 0x06 to 0xFFFF
 *     - Time range: 7.5 ms to 81.92 s
 *     - Note: interval_max should be bigger than interval_min
 *
 *   Default value: 200 ms
 *   @endparblock
 * @param[in] flags Periodic advertising configurations. Bitmask of the
 *   following:
 *     - <b>Bit 0:</b> Include TX power in advertising PDU
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_start_periodic_advertising(uint8_t handle,
                                                        uint16_t interval_min,
                                                        uint16_t interval_max,
                                                        uint32_t flags);

/***************************************************************************//**
 *
 * Stop the periodic advertising on the given advertising set. Counterpart with
 * @ref sl_bt_advertiser_start_periodic_advertising.
 *
 * This command does not affect the enable state of the advertising set, i.e.,
 * legacy or extended advertising is not stopped.
 *
 * @param[in] handle Advertising set handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_stop_periodic_advertising(uint8_t handle);

/***************************************************************************//**
 *
 * Delete an advertising set.
 *
 * @param[in] handle Advertising set handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_advertiser_delete_set(uint8_t handle);

/** @} */ // end addtogroup sl_bt_advertiser

/**
 * @addtogroup sl_bt_scanner Scanner
 * @{
 *
 * @brief Scanner
 *
 * The commands and events in this class are related to scanning functionalities
 * in GAP central and observer roles.
 */

/***************************************************************************//**
 *
 * Set the scanning timing parameters on the specified PHYs. If the device is
 * currently scanning for advertising devices on PHYs, new parameters will take
 * effect when scanning is restarted.
 *
 * @param[in] phys PHYs for which the parameters are set.
 *     - <b>1:</b> 1M PHY
 *     - <b>4:</b> Coded PHY
 *     - <b>5:</b> 1M PHY and Coded PHY
 * @param[in] scan_interval @parblock
 *   Scan interval is defined as the time interval when the device starts its
 *   last scan until it begins the subsequent scan. In other words, how often to
 *   scan
 *     - Time = Value x 0.625 ms
 *     - Range: 0x0004 to 0xFFFF
 *     - Time Range: 2.5 ms to 40.96 s
 *
 *   Default value: 10 ms
 *
 *   A variable delay occurs when switching channels at the end of each scanning
 *   interval, which is included in the scanning interval time. During the
 *   switch time, advertising packets are not received by the device. The switch
 *   time variation is use case-dependent. For example, if scanning while
 *   keeping active connections, the channel switch time might be longer than
 *   when scanning without any active connections. Increasing the scanning
 *   interval reduces the amount of time in which the device can't receive
 *   advertising packets because it switches channels less often.
 *
 *   After every scan interval, the scanner changes the frequency at which it
 *   operates. It cycles through all three advertising channels in a round robin
 *   fashion. According to the specification, all three channels must be used by
 *   a scanner.
 *   @endparblock
 * @param[in] scan_window @parblock
 *   Scan window defines the duration of the scan which must be less than or
 *   equal to the @p scan_interval
 *     - Time = Value x 0.625 ms
 *     - Range: 0x0004 to 0xFFFF
 *     - Time Range: 2.5 ms to 40.96 s
 *
 *   Default value: 10 ms Note that the packet reception is aborted if it's
 *   started just before the scan window ends.
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_scanner_set_timing(uint8_t phys,
                                     uint16_t scan_interval,
                                     uint16_t scan_window);

/***************************************************************************//**
 *
 * Set the scan mode on the specified PHYs. If the device is currently scanning
 * for advertising devices on PHYs, new parameters will take effect when
 * scanning is restarted.
 *
 * @param[in] phys PHYs for which the parameters are set.
 *     - <b>1:</b> 1M PHY
 *     - <b>4:</b> Coded PHY
 *     - <b>5:</b> 1M PHY and Coded PHY
 * @param[in] scan_mode @parblock
 *   Scan mode. Values:
 *     - <b>0:</b> Passive scanning
 *     - <b>1:</b> Active scanning
 *     - In passive scanning mode, the device only listens to advertising
 *       packets and does not transmit packets.
 *     - In active scanning mode, the device sends out a scan request packet
 *       upon receiving an advertising packet from a remote device. Then, it
 *       listens to the scan response packet from the remote device.
 *
 *   Default value: 0
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_scanner_set_mode(uint8_t phys, uint8_t scan_mode);

/***************************************************************************//**
 *
 * Start the GAP discovery procedure to scan for advertising devices on the
 * specified scanning PHY or to perform a device discovery. To cancel an ongoing
 * discovery process use the @ref sl_bt_scanner_stop command.
 *
 * The invalid parameter error will be returned if the scanning PHY value is
 * invalid or the device does not support the PHY.
 *
 * @param[in] scanning_phy Enum @ref gap_phy_type_t. The scanning PHY. Values:
 *     - <b>gap_1m_phy (0x1):</b> 1M PHY
 *     - <b>gap_coded_phy (0x4):</b> Coded PHY
 * @param[in] discover_mode Enum @ref scanner_discover_mode_t. Bluetooth
 *   discovery Mode. Values:
 *     - <b>scanner_discover_limited (0x0):</b> Discover only limited
 *       discoverable devices.
 *     - <b>scanner_discover_generic (0x1):</b> Discover limited and generic
 *       discoverable devices.
 *     - <b>scanner_discover_observation (0x2):</b> Discover all devices.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_scanner_scan_report - This event is triggered each time an
 *     advertising packet is received. Packets are not filtered in any way, so
 *     multiple events will be received for every advertising device in range.
 *
 ******************************************************************************/
sl_status_t sl_bt_scanner_start(uint8_t scanning_phy, uint8_t discover_mode);

/***************************************************************************//**
 *
 * Stop scanning for advertising devices. For more information about the
 * discovery, see the @ref sl_bt_scanner_start command.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_scanner_stop();

/** @} */ // end addtogroup sl_bt_scanner

/**
 * @addtogroup sl_bt_sync Synchronization
 * @{
 *
 * @brief Synchronization
 *
 * Provides periodic advertising synchronization feature.
 */

/***************************************************************************//**
 *
 * Configure periodic advertiser synchronization parameters. The specified
 * parameters take effect immediately for all advertisers that have not already
 * established synchronization.
 *
 * The application should determine skip and timeout values based on the
 * periodic advertising interval provided by the advertiser. If @p skip and @p
 * timeout are used, select appropriate values so that they allow a few
 * receiving attempts. Periodic advertising intervals are reported in event @ref
 * sl_bt_evt_scanner_scan_report.
 *
 * @param[in] skip The maximum number of periodic advertising packets that can
 *   be skipped after a successful receive.
 *     - Range: 0x0000 to 0x01F3
 *     - Default value: 0
 * @param[in] timeout The maximum permitted time between successful receives. If
 *   this time is exceeded, synchronization is lost. Unit: 10 ms.
 *     - Range: 0x06 to 0xFFFF
 *     - Unit: 10 ms
 *     - Time range: 100 ms ms to 163.84 s
 *     - Default value: 1000 ms
 * @param[in] flags No flags defined currently
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sync_set_parameters(uint16_t skip,
                                      uint16_t timeout,
                                      uint32_t flags);

/***************************************************************************//**
 *
 * Start establishing synchronization with the specified periodic advertiser in
 * parallel with other advertisers given in previous invocations of this
 * command. The stack will internally enable scanning when needed so that
 * synchronizations can occur. The scanning responses from the internal scanning
 * are not passed to the application unless the application has also enabled
 * scanning.
 *
 * Advertisers that have not already synced before the invocation of this
 * command will be synced using the @p skip and @p timeout values configured in
 * the most recent invocation of command @ref sl_bt_evt_scanner_scan_report.
 *
 * @param[in] address Address of the advertiser
 * @param[in] address_type Advertiser address type. Values:
 *     - <b>0:</b> Public address
 *     - <b>1:</b> Random address
 * @param[in] adv_sid Advertising set identifier
 * @param[out] sync A handle that will be assigned to the periodic advertising
 *   synchronization after the synchronization is established. This handle is
 *   valid only if the result code of this response is SL_STATUS_OK.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_sync_opened - Triggered after the synchronization is
 *     established.
 *   - @ref sl_bt_evt_sync_data - Indicates that a periodic advertisement packet
 *     is received.
 *   - @ref sl_bt_evt_sync_closed - Triggered after periodic advertising
 *     synchronization was lost or explicitly closed, or a synchronization
 *     establishment procedure was canceled.
 *
 ******************************************************************************/
sl_status_t sl_bt_sync_open(bd_addr address,
                            uint8_t address_type,
                            uint8_t adv_sid,
                            uint16_t *sync);

/***************************************************************************//**
 *
 * Closes a periodic advertising synchronization or cancels an ongoing attempt
 * of establishing a synchronization.
 *
 * @param[in] sync Periodic advertising synchronization handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_sync_closed - Triggered after a periodic advertising
 *     synchronization has been closed or canceled.
 *
 ******************************************************************************/
sl_status_t sl_bt_sync_close(uint16_t sync);

/** @} */ // end addtogroup sl_bt_sync

/**
 * @addtogroup sl_bt_connection Connection
 * @{
 *
 * @brief Connection
 *
 * The commands and events in this class are related to managing connection
 * establishment, parameter setting, and disconnection procedures.
 */

/***************************************************************************//**
 *
 * Set the default Bluetooth connection parameters. The values are valid for all
 * subsequent connections initiated by this device. To change parameters of an
 * already established connection, use the command @ref
 * sl_bt_connection_set_parameters.
 *
 * @param[in] min_interval @parblock
 *   Minimum value for the connection event interval. This must be set less than
 *   or equal to @p max_interval.
 *     - Time = Value x 1.25 ms
 *     - Range: 0x0006 to 0x0c80
 *     - Time Range: 7.5 ms to 4 s
 *
 *   Default value: 20 ms
 *   @endparblock
 * @param[in] max_interval @parblock
 *   Maximum value for the connection event interval. This must be set greater
 *   than or equal to @p min_interval.
 *     - Time = Value x 1.25 ms
 *     - Range: 0x0006 to 0x0c80
 *     - Time Range: 7.5 ms to 4 s
 *
 *   Default value: 50 ms
 *   @endparblock
 * @param[in] latency @parblock
 *   Slave latency, which defines how many connection intervals the slave can
 *   skip if it has no data to send
 *     - Range: 0x0000 to 0x01f4
 *
 *   Default value: 0
 *   @endparblock
 * @param[in] timeout @parblock
 *   Supervision timeout, which defines the time that the connection is
 *   maintained although the devices can't communicate at the currently
 *   configured connection intervals.
 *     - Range: 0x000a to 0x0c80
 *     - Time = Value x 10 ms
 *     - Time Range: 100 ms to 32 s
 *     - The value in milliseconds must be larger than (1 + @p latency) * @p
 *       max_interval * 2, where @p max_interval is given in milliseconds
 *
 *   Set the supervision timeout at a value which allows communication attempts
 *   over at least a few connection intervals.
 *
 *   Default value: 1000 ms
 *   @endparblock
 * @param[in] min_ce_length @parblock
 *   Minimum value for the connection event length. This must be set be less
 *   than or equal to @p max_ce_length.
 *     - Time = Value x 0.625 ms
 *     - Range: 0x0000 to 0xffff
 *
 *   Default value: 0x0000
 *
 *   Value is not currently used and is reserved for future. Set to 0.
 *   @endparblock
 * @param[in] max_ce_length @parblock
 *   Maximum value for the connection event length. This must be set greater
 *   than or equal to @p min_ce_length.
 *     - Time = Value x 0.625 ms
 *     - Range: 0x0000 to 0xffff
 *
 *   Default value: 0xffff
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_set_default_parameters(uint16_t min_interval,
                                                    uint16_t max_interval,
                                                    uint16_t latency,
                                                    uint16_t timeout,
                                                    uint16_t min_ce_length,
                                                    uint16_t max_ce_length);

/***************************************************************************//**
 *
 * Set default preferred and accepted PHYs. PHY settings will be used for all
 * subsequent connections. Non-preferred PHY can also be set if the remote
 * device does not accept any of the preferred PHYs.
 *
 * The parameter @p accepted_phy is used to specify PHYs that the stack can
 * accept in a remotely-initiated PHY update request. A PHY update will not
 * happen if none of the accepted PHYs are present in the request.
 *
 * <b>NOTE:</b> 2M and Coded PHYs are not supported by all devices.
 *
 * @param[in] preferred_phy @parblock
 *   Preferred PHYs. This parameter is a bitfield and multiple PHYs can be set.
 *     - <b>0x01:</b> 1M PHY
 *     - <b>0x02:</b> 2M PHY
 *     - <b>0x04:</b> Coded PHY
 *     - <b>0xff:</b> Any PHYs
 *
 *   Default: 0xff (no preference)
 *   @endparblock
 * @param[in] accepted_phy @parblock
 *   Accepted PHYs in remotely-initiated PHY update request. This parameter is a
 *   bitfield and multiple PHYs can be set.
 *     - <b>0x01:</b> 1M PHY
 *     - <b>0x02:</b> 2M PHY
 *     - <b>0x04:</b> Coded PHY
 *     - <b>0xff:</b> Any PHYs
 *
 *   Default: 0xff (all PHYs accepted)
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_set_default_preferred_phy(uint8_t preferred_phy,
                                                       uint8_t accepted_phy);

/***************************************************************************//**
 *
 * Connect to an advertising device with the specified initiating PHY on which
 * connectable advertisements on primary advertising channels are received. The
 * Bluetooth stack will enter a state where it continuously scans for the
 * connectable advertising packets from the remote device, which matches the
 * Bluetooth address given as a parameter. Scan parameters set in @ref
 * sl_bt_scanner_set_timing are used in this operation. Upon receiving the
 * advertising packet, the module will send a connection request packet to the
 * target device to initiate a Bluetooth connection. To cancel an ongoing
 * connection process, use @ref sl_bt_connection_close command with the handle
 * received in response from this command.
 *
 * A connection is opened in no-security mode. If the GATT client needs to read
 * or write the attributes on GATT server requiring encryption or
 * authentication, it must first encrypt the connection using an appropriate
 * authentication method.
 *
 * If a connection can't be established (for example, the remote device has gone
 * out of range, has entered into deep sleep, or is not advertising), the stack
 * will try to connect forever. In this case, the application will not get an
 * event related to the connection request. To recover from this situation, the
 * application can implement a timeout and call @ref sl_bt_connection_close to
 * cancel the connection request.
 *
 * This command fails with the connection limit exceeded error if the number of
 * connections attempted exceeds the configured MAX_CONNECTIONS value.
 *
 * This command fails with the invalid parameter error if the initiating PHY
 * value is invalid or the device does not support PHY.
 *
 * Later calls of this command have to wait for the ongoing command to complete.
 * A received event @ref sl_bt_evt_connection_opened indicates that the
 * connection opened successfully and a received event @ref
 * sl_bt_evt_connection_closed indicates that connection failures have occurred.
 *
 * @param[in] address Address of the device to connect to
 * @param[in] address_type Enum @ref gap_address_type_t. Address type of the
 *   device to connect to. Values:
 *     - <b>gap_public_address (0x0):</b> Public device address
 *     - <b>gap_static_address (0x1):</b> Static device address
 *     - <b>gap_random_resolvable_address (0x2):</b> Private resolvable random
 *       address
 *     - <b>gap_random_nonresolvable_address (0x3):</b> Private non-resolvable
 *       random address
 * @param[in] initiating_phy Enum @ref gap_phy_type_t. The initiating PHY.
 *   Values:
 *     - <b>gap_1m_phy (0x1):</b> 1M PHY
 *     - <b>gap_coded_phy (0x4):</b> Coded PHY
 * @param[out] connection Handle that will be assigned to the connection after
 *   the connection is established. This handle is valid only if the result code
 *   of this response is 0 (zero).
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_connection_opened - This event is triggered after the
 *     connection is opened and indicates whether the devices are already bonded
 *     and whether the role of the Bluetooth device is Slave or Master.
 *   - @ref sl_bt_evt_connection_parameters - This event indicates the
 *     connection parameters and security mode of the connection.
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_open(bd_addr address,
                                  uint8_t address_type,
                                  uint8_t initiating_phy,
                                  uint8_t *connection);

/***************************************************************************//**
 *
 * Request a change in the connection parameters of a Bluetooth connection.
 *
 * @param[in] connection Connection Handle
 * @param[in] min_interval Minimum value for the connection event interval. This
 *   must be set less than or equal to @p max_interval.
 *     - Time = Value x 1.25 ms
 *     - Range: 0x0006 to 0x0c80
 *     - Time Range: 7.5 ms to 4 s
 * @param[in] max_interval Maximum value for the connection event interval. This
 *   must be set greater than or equal to @p min_interval.
 *     - Time = Value x 1.25 ms
 *     - Range: 0x0006 to 0x0c80
 *     - Time Range: 7.5 ms to 4 s
 * @param[in] latency @parblock
 *   Slave latency, which defines how many connection intervals the slave can
 *   skip if it has no data to send
 *     - Range: 0x0000 to 0x01f4
 *
 *   Use 0x0000 for default value
 *   @endparblock
 * @param[in] timeout @parblock
 *   Supervision timeout, which defines the time that the connection is
 *   maintained although the devices can't communicate at the currently
 *   configured connection intervals.
 *     - Range: 0x000a to 0x0c80
 *     - Time = Value x 10 ms
 *     - Time Range: 100 ms to 32 s
 *     - The value in milliseconds must be larger than (1 + @p latency) * @p
 *       max_interval * 2, where @p max_interval is given in milliseconds
 *
 *   Set the supervision timeout at a value which allows communication attempts
 *   over at least a few connection intervals.
 *   @endparblock
 * @param[in] min_ce_length @parblock
 *   Minimum value for the connection event length. This must be set less than
 *   or equal to @p max_ce_length.
 *     - Time = Value x 0.625 ms
 *     - Range: 0x0000 to 0xffff
 *
 *   Value is not currently used and is reserved for future. Set to 0.
 *   @endparblock
 * @param[in] max_ce_length @parblock
 *   Maximum value for the connection event length. This must be set greater
 *   than or equal to @p min_ce_length.
 *     - Time = Value x 0.625 ms
 *     - Range: 0x0000 to 0xffff
 *
 *   Use 0xffff for no limitation.
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_connection_parameters - Triggered after new connection
 *     parameters are applied on the connection.
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_set_parameters(uint8_t connection,
                                            uint16_t min_interval,
                                            uint16_t max_interval,
                                            uint16_t latency,
                                            uint16_t timeout,
                                            uint16_t min_ce_length,
                                            uint16_t max_ce_length);

/***************************************************************************//**
 *
 * Sets preferred and accepted PHYs for the given connection. Event @ref
 * sl_bt_evt_connection_phy_status is received when PHY update procedure is
 * completed. Non-preferred PHY can also be set if remote device does not accept
 * any of the preferred PHYs.
 *
 * The parameter @p accepted_phy is used for specifying the PHYs that the stack
 * can accept in a remote initiated PHY update request. A PHY update will not
 * occur if none of the accepted PHYs presents in the request.
 *
 * <b>NOTE:</b> 2M and Coded PHYs are not supported by all devices.
 *
 * @param[in] connection Connection handle
 * @param[in] preferred_phy @parblock
 *   Preferred PHYs. This parameter is a bitfield and multiple PHYs can be set.
 *     - <b>0x01:</b> 1M PHY
 *     - <b>0x02:</b> 2M PHY
 *     - <b>0x04:</b> 125k Coded PHY (S=8)
 *     - <b>0x08:</b> 500k Coded PHY (S=2)
 *
 *   Default: 0xff (no preference)
 *   @endparblock
 * @param[in] accepted_phy @parblock
 *   Accepted PHYs in remotely-initiated PHY update requests. This parameter is
 *   a bitfield and multiple PHYs can be set.
 *     - <b>0x01:</b> 1M PHY
 *     - <b>0x02:</b> 2M PHY
 *     - <b>0x04:</b> Coded PHY
 *     - <b>0xff:</b> Any PHYs
 *
 *   Default: 0xff (all PHYs accepted)
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_connection_phy_status
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_set_preferred_phy(uint8_t connection,
                                               uint8_t preferred_phy,
                                               uint8_t accepted_phy);

/***************************************************************************//**
 *
 * Temporarily enable or disable slave latency. Used only when Bluetooth device
 * is acting as slave. When slave latency is disabled, the slave latency
 * connection parameter is not set to 0 but the device will wake up on every
 * connection interval to receive and send packets.
 *
 * @param[in] connection Connection Handle
 * @param[in] disable 0 enable, 1 disable slave latency. Default: 0
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_disable_slave_latency(uint8_t connection,
                                                   uint8_t disable);

/***************************************************************************//**
 *
 * Get the latest RSSI value of a Bluetooth connection. The RSSI value will be
 * reported in a @ref sl_bt_evt_connection_rssi event.
 *
 * @param[in] connection Connection handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_connection_rssi - Triggered when this command has
 *     completed.
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_get_rssi(uint8_t connection);

/***************************************************************************//**
 *
 * Read channel map for a specified connection.
 *
 * @param[in] connection Connection Handle
 * @param[in] max_channel_map_size Size of output buffer passed in @p
 *   channel_map
 * @param[out] channel_map_len On return, set to the length of output data
 *   written to @p channel_map
 * @param[out] channel_map @parblock
 *   This parameter is 5 bytes and contains 37 1-bit fields.
 *   The nth field (in the range 0 to 36) contains the value for the link layer
 *   channel index n.
 *     - <b>0:</b> Channel n is unused.
 *     - <b>1:</b> Channel n is used.
 *
 *   The most significant bits are reserved for future use.
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_read_channel_map(uint8_t connection,
                                              size_t max_channel_map_size,
                                              size_t *channel_map_len,
                                              uint8_t *channel_map);

/***************************************************************************//**
 *
 * Close a Bluetooth connection or cancel an ongoing connection establishment
 * process. The parameter is a connection handle which is reported in @ref
 * sl_bt_evt_connection_opened event or @ref sl_bt_connection_open command
 * response.
 *
 * @param[in] connection Handle of the connection to be closed
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_connection_closed
 *
 ******************************************************************************/
sl_status_t sl_bt_connection_close(uint8_t connection);

/** @} */ // end addtogroup sl_bt_connection

/**
 * @addtogroup sl_bt_gatt GATT Client
 * @{
 *
 * @brief GATT Client
 *
 * The commands and events in this class are used to browse and manage
 * attributes in a remote GATT server.
 */

/***************************************************************************//**
 *
 * Set the maximum size of ATT Message Transfer Units (MTU). Functionality is
 * the same as @ref sl_bt_gatt_server_set_max_mtu and this setting applies to
 * both GATT client and server. If the given value is too large according to the
 * maximum BGAPI payload size, the system will select the maximum possible value
 * as the maximum ATT_MTU. If maximum ATT_MTU is larger than 23, the GATT client
 * in the stack will automatically send an MTU exchange request after a
 * Bluetooth connection has been established.
 *
 * @param[in] max_mtu @parblock
 *   Maximum size of Message Transfer Units (MTU) allowed
 *     - Range: 23 to 250
 *
 *   Default: 247
 *   @endparblock
 * @param[out] max_mtu_out The maximum ATT_MTU selected by the system if this
 *   command succeeds
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_set_max_mtu(uint16_t max_mtu, uint16_t *max_mtu_out);

/***************************************************************************//**
 *
 * Discover all primary services of a remote GATT database. This command
 * generates a unique gatt_service event for every discovered primary service.
 * Received @ref sl_bt_evt_gatt_procedure_completed event indicates that this
 * GATT procedure has successfully completed or failed with an error.
 *
 * @param[in] connection Connection handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_service - Discovered service from remote GATT
 *     database
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_discover_primary_services(uint8_t connection);

/***************************************************************************//**
 *
 * Discover primary services with the specified UUID in a remote GATT database.
 * This command generates unique gatt_service event for every discovered primary
 * service. Received @ref sl_bt_evt_gatt_procedure_completed event indicates
 * that this GATT procedure was successfully completed or failed with an error.
 *
 * @param[in] connection Connection handle
 * @param[in] uuid_len Array length
 * @param[in] uuid Service UUID in little endian format
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_service - Discovered service from remote GATT
 *     database.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_discover_primary_services_by_uuid(uint8_t connection,
                                                         size_t uuid_len,
                                                         const uint8_t* uuid);

/***************************************************************************//**
 *
 * Find the services that are included by a service in a remote GATT database.
 * This command generates a unique gatt_service event for each included service.
 * The received @ref sl_bt_evt_gatt_procedure_completed event indicates that
 * this GATT procedure was successfully completed or failed with an error.
 *
 * @param[in] connection Connection handle
 * @param[in] service GATT service handle. This value is normally received from
 *   the gatt_service event.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_service - Discovered service from remote GATT
 *     database.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_find_included_services(uint8_t connection,
                                              uint32_t service);

/***************************************************************************//**
 *
 * Discover all characteristics of a GATT service from a remote GATT database.
 * This command generates a unique gatt_characteristic event for every
 * discovered characteristic. Received @ref sl_bt_evt_gatt_procedure_completed
 * event indicates that this GATT procedure was successfully completed or failed
 * with an error.
 *
 * @param[in] connection Connection handle
 * @param[in] service GATT service handle. This value is normally received from
 *   the gatt_service event.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_characteristic - Discovered characteristic from
 *     remote GATT database.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_discover_characteristics(uint8_t connection,
                                                uint32_t service);

/***************************************************************************//**
 *
 * Discover all characteristics of a GATT service in a remote GATT database
 * having the specified UUID. This command generates a unique
 * gatt_characteristic event for every discovered characteristic having the
 * specified UUID. Received @ref sl_bt_evt_gatt_procedure_completed event
 * indicates that this GATT procedure was successfully completed or failed with
 * an error.
 *
 * @param[in] connection Connection handle
 * @param[in] service GATT service handle. This value is normally received from
 *   the gatt_service event.
 * @param[in] uuid_len Array length
 * @param[in] uuid Characteristic UUID in little endian format
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_characteristic - Discovered characteristic from
 *     remote GATT database.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_discover_characteristics_by_uuid(uint8_t connection,
                                                        uint32_t service,
                                                        size_t uuid_len,
                                                        const uint8_t* uuid);

/***************************************************************************//**
 *
 * Discover all descriptors of a GATT characteristic in a remote GATT database.
 * It generates a unique gatt_descriptor event for every discovered descriptor.
 * Received @ref sl_bt_evt_gatt_procedure_completed event indicates that this
 * GATT procedure has successfully completed or failed with an error.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle. This value is normally
 *   received from the gatt_characteristic event.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_descriptor - Discovered descriptor from remote GATT
 *     database.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_discover_descriptors(uint8_t connection,
                                            uint16_t characteristic);

/***************************************************************************//**
 *
 * Enable or disable the notifications and indications sent from a remote GATT
 * server. This procedure discovers a characteristic client configuration
 * descriptor and writes the related configuration flags to a remote GATT
 * database. A received @ref sl_bt_evt_gatt_procedure_completed event indicates
 * that this GATT procedure was successfully completed or that it failed with an
 * error.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle. This value is normally
 *   received from the gatt_characteristic event.
 * @param[in] flags Enum @ref gatt_client_config_flag_t. Characteristic client
 *   configuration flags. Values:
 *     - <b>gatt_disable (0x0):</b> Disable notifications and indications
 *     - <b>gatt_notification (0x1):</b> Notification
 *     - <b>gatt_indication (0x2):</b> Indication
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *   - @ref sl_bt_evt_gatt_characteristic_value - If an indication or
 *     notification has been enabled for a characteristic, this event is
 *     triggered whenever an indication or notification is sent by the remote
 *     GATT server. The triggering conditions of the GATT server are defined by
 *     an upper level, for example by a profile. <b>As a result, it is possible
 *     that no values are ever received, or that it may take time, depending on
 *     how the server is configured.</b>
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_set_characteristic_notification(uint8_t connection,
                                                       uint16_t characteristic,
                                                       uint8_t flags);

/***************************************************************************//**
 *
 * Send a confirmation to a remote GATT server after receiving a characteristic
 * indication. The @ref sl_bt_evt_gatt_characteristic_value event carries the @p
 * att_opcode containing @ref gatt_handle_value_indication (0x1d), which reveals
 * that an indication has been received and must be confirmed with this command.
 * The confirmation needs to be sent within 30 seconds, otherwise further GATT
 * transactions are not allowed by the remote side.
 *
 * @param[in] connection Connection handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_send_characteristic_confirmation(uint8_t connection);

/***************************************************************************//**
 *
 * Read the value of a characteristic from a remote GATT database. A single @ref
 * sl_bt_evt_gatt_characteristic_value is generated if the characteristic value
 * fits in one ATT PDU. Otherwise, more than one @ref
 * sl_bt_evt_gatt_characteristic_value event is generated because the firmware
 * will automatically use the Read Long Characteristic Values procedure. A
 * received @ref sl_bt_evt_gatt_procedure_completed event indicates that all
 * data was read successfully or that an error response was received.
 *
 * Note that the GATT client does not verify if the requested attribute is a
 * characteristic value. Therefore, before calling this command, ensure that the
 * attribute handle is for a characteristic value, for example, by performing
 * characteristic discovery.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle. This value is normally
 *   received from the gatt_characteristic event.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_characteristic_value - Contains the data of a
 *     characteristic sent by the GATT Server.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_read_characteristic_value(uint8_t connection,
                                                 uint16_t characteristic);

/***************************************************************************//**
 *
 * Read a partial characteristic value with a specified offset and maximum
 * length from a remote GATT database. It is equivalent to @ref
 * sl_bt_gatt_read_characteristic_value if both the offset and maximum length
 * parameters are 0. A single @ref sl_bt_evt_gatt_characteristic_value event is
 * generated if the value to read fits in one ATT PDU. Otherwise, more than one
 * @ref sl_bt_evt_gatt_characteristic_value events are generated because the
 * firmware will automatically use the Read Long Characteristic Values
 * procedure. A received @ref sl_bt_evt_gatt_procedure_completed event indicates
 * that all data was read successfully or that an error response was received.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle. This value is normally
 *   received from the gatt_characteristic event.
 * @param[in] offset Offset of the characteristic value
 * @param[in] maxlen Maximum bytes to read. If this parameter is 0, all
 *   characteristic values starting at a given offset will be read.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_characteristic_value - Contains the data of a
 *     characteristic sent by the GATT Server.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_read_characteristic_value_from_offset(uint8_t connection,
                                                             uint16_t characteristic,
                                                             uint16_t offset,
                                                             uint16_t maxlen);

/***************************************************************************//**
 *
 * Read values of multiple characteristics from a remote GATT database at once.
 * The GATT server returns values in one ATT PDU as the response. If the total
 * set of values is greater than (ATT_MTU - 1) bytes in length, only the first
 * (ATT_MTU - 1) bytes are included. A single @ref
 * sl_bt_evt_gatt_characteristic_value event is generated, in which the
 * characteristic is set to 0 and data in the value parameter is a concatenation
 * of characteristic values in the order they were requested. The received @ref
 * sl_bt_evt_gatt_procedure_completed event indicates either that this GATT
 * procedure was successfully completed or failed with an error.
 *
 * Use this command only for characteristics values that have a known fixed
 * size, except the last one that could have variable length.
 *
 * When the remote GATT server is from Silicon Labs Bluetooth stack, the server
 * returns ATT Invalid PDU (0x04) if this command only reads one characteristic
 * value. The server returns ATT Application Error 0x80 if this command reads
 * the value of a user-type characteristic.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic_list_len Array length
 * @param[in] characteristic_list List of uint16 characteristic handles each in
 *   little endian format.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_characteristic_value - A concatenation of
 *     characteristic values in the order they were requested
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was either
 *     successfully completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_read_multiple_characteristic_values(uint8_t connection,
                                                           size_t characteristic_list_len,
                                                           const uint8_t* characteristic_list);

/***************************************************************************//**
 *
 * Read characteristic values of a service from a remote GATT database by giving
 * the UUID of the characteristic and the handle of the service containing this
 * characteristic. If multiple characteristic values are received in one ATT
 * PDU, then one @ref sl_bt_evt_gatt_characteristic_value event is generated for
 * each value. If the first characteristic value does not fit in one ATT PDU,
 * the firmware automatically uses the Read Long Characteristic Values procedure
 * and generate more @ref sl_bt_evt_gatt_characteristic_value events until the
 * value has been completely read. A received @ref
 * sl_bt_evt_gatt_procedure_completed event indicates that all data was read
 * successfully or that an error response was received.
 *
 * @param[in] connection Connection handle
 * @param[in] service GATT service handle. This value is normally received from
 *   the gatt_service event.
 * @param[in] uuid_len Array length
 * @param[in] uuid Characteristic UUID in little endian format
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_characteristic_value - Contains the data of a
 *     characteristic sent by the GATT Server.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_read_characteristic_value_by_uuid(uint8_t connection,
                                                         uint32_t service,
                                                         size_t uuid_len,
                                                         const uint8_t* uuid);

/***************************************************************************//**
 *
 * Write the value of a characteristic in a remote GATT database. If the given
 * value does not fit in one ATT PDU, "write long" GATT procedure is used
 * automatically. Received @ref sl_bt_evt_gatt_procedure_completed event
 * indicates that all data was written successfully or that an error response
 * was received.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle. This value is normally
 *   received from the gatt_characteristic event.
 * @param[in] value_len Array length
 * @param[in] value Characteristic value
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_write_characteristic_value(uint8_t connection,
                                                  uint16_t characteristic,
                                                  size_t value_len,
                                                  const uint8_t* value);

/***************************************************************************//**
 *
 * Write the value of a characteristic in a remote GATT server. It does not
 * generate an event. All failures on the server are ignored silently. For
 * example, if an error is generated in the remote GATT server and the given
 * value is not written into the database, no error message will be reported to
 * the local GATT client. Note that this command can't be used to write long
 * values. At most ATT_MTU - 3 amount of data can be sent once.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle. This value is normally
 *   received from the gatt_characteristic event.
 * @param[in] value_len Array length
 * @param[in] value Characteristic value
 * @param[out] sent_len The length of data sent to the remote GATT server
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_write_characteristic_value_without_response(uint8_t connection,
                                                                   uint16_t characteristic,
                                                                   size_t value_len,
                                                                   const uint8_t* value,
                                                                   uint16_t *sent_len);

/***************************************************************************//**
 *
 * Add a characteristic value to the write queue of a remote GATT server. It can
 * be used when long attributes need to be written or a set of values needs to
 * be written atomically. At most ATT_MTU - 5 amount of data can be sent at one
 * time. Writes are executed or canceled with the @ref
 * sl_bt_gatt_execute_characteristic_value_write command. Whether the writes
 * succeed or not is indicated in the response of the @ref
 * sl_bt_gatt_execute_characteristic_value_write command.
 *
 * In all use cases where the amount of data to transfer fits into the BGAPI
 * payload, use the command @ref sl_bt_gatt_write_characteristic_value to write
 * long values because it transparently performs the prepare_write and
 * execute_write commands.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle. This value is normally
 *   received from the gatt_characteristic event.
 * @param[in] offset Offset of the characteristic value
 * @param[in] value_len Array length
 * @param[in] value Value to write into the specified characteristic of the
 *   remote GATT database
 * @param[out] sent_len The length of data sent to the remote GATT server
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_prepare_characteristic_value_write(uint8_t connection,
                                                          uint16_t characteristic,
                                                          uint16_t offset,
                                                          size_t value_len,
                                                          const uint8_t* value,
                                                          uint16_t *sent_len);

/***************************************************************************//**
 *
 * Add a characteristic value to the write queue of a remote GATT server and
 * verifies whether the value was correctly received by the server. Received
 * @ref sl_bt_evt_gatt_procedure_completed event indicates that this GATT
 * procedure was successfully completed or failed with an error. Specifically,
 * error code 0x0194 (data_corrupted) will be returned if the value received
 * from the GATT server's response fails to pass the reliable write
 * verification. At most ATT_MTU - 5 amount of data can be sent at one time.
 * Writes are executed or canceled with the @ref
 * sl_bt_gatt_execute_characteristic_value_write command. Whether the writes
 * succeed or not is indicated in the response of the @ref
 * sl_bt_gatt_execute_characteristic_value_write command.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle. This value is normally
 *   received from the gatt_characteristic event.
 * @param[in] offset Offset of the characteristic value
 * @param[in] value_len Array length
 * @param[in] value Value to write into the specified characteristic of the
 *   remote GATT database
 * @param[out] sent_len The length of data sent to the remote GATT server
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_prepare_characteristic_value_reliable_write(uint8_t connection,
                                                                   uint16_t characteristic,
                                                                   uint16_t offset,
                                                                   size_t value_len,
                                                                   const uint8_t* value,
                                                                   uint16_t *sent_len);

/***************************************************************************//**
 *
 * Commit or cancel previously queued writes to a long characteristic of a
 * remote GATT server. Writes are sent to the queue with @ref
 * sl_bt_gatt_prepare_characteristic_value_write command. Content, offset, and
 * length of queued values are validated by this procedure. A received @ref
 * sl_bt_evt_gatt_procedure_completed event indicates that all data was written
 * successfully or that an error response was received.
 *
 * @param[in] connection Connection handle
 * @param[in] flags Enum @ref gatt_execute_write_flag_t. Execute write flag.
 *   Values:
 *     - <b>gatt_cancel (0x0):</b> Cancel all queued writes
 *     - <b>gatt_commit (0x1):</b> Commit all queued writes
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_execute_characteristic_value_write(uint8_t connection,
                                                          uint8_t flags);

/***************************************************************************//**
 *
 * Read the descriptor value of a characteristic in a remote GATT database. A
 * single @ref sl_bt_evt_gatt_descriptor_value event is generated if the
 * descriptor value fits in one ATT PDU. Otherwise, more than one @ref
 * sl_bt_evt_gatt_descriptor_value events are generated because the firmware
 * automatically uses the Read Long Characteristic Values procedure. A received
 * @ref sl_bt_evt_gatt_procedure_completed event indicates that all data was
 * read successfully or that an error response was received.
 *
 * @param[in] connection Connection handle
 * @param[in] descriptor GATT characteristic descriptor handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_descriptor_value - Descriptor value received from the
 *     remote GATT server.
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_read_descriptor_value(uint8_t connection,
                                             uint16_t descriptor);

/***************************************************************************//**
 *
 * Write the value of a characteristic descriptor in a remote GATT database. If
 * the given value does not fit in one ATT PDU, "write long" GATT procedure is
 * used automatically. Received @ref sl_bt_evt_gatt_procedure_completed event
 * indicates either that all data was written successfully or that an error
 * response was received.
 *
 * @param[in] connection Connection handle
 * @param[in] descriptor GATT characteristic descriptor handle
 * @param[in] value_len Array length
 * @param[in] value Descriptor value
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_gatt_procedure_completed - Procedure was successfully
 *     completed or failed with an error.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_write_descriptor_value(uint8_t connection,
                                              uint16_t descriptor,
                                              size_t value_len,
                                              const uint8_t* value);

/** @} */ // end addtogroup sl_bt_gatt

/**
 * @addtogroup sl_bt_gatt_server GATT Server
 * @{
 *
 * @brief GATT Server
 *
 * These commands and events are used by the local GATT server to manage the
 * local GATT database.
 */

/***************************************************************************//**
 *
 * Reset capabilities that should be enabled by the GATT database. A service is
 * visible to remote GATT clients if at least one of its capabilities is
 * enabled. The same applies to a characteristic and its attributes. Capability
 * identifiers and their corresponding bit flag values can be found in the
 * auto-generated database header file. See UG118: Blue Gecko Bluetooth Profile
 * Toolkit Developer's Guide for how to declare capabilities in the GATT
 * database.
 *
 * Changing the capabilities of a database effectively causes a database change
 * (attributes being added or removed) from a remote GATT client point of view.
 * If the database has a Generic Attribute service and Service Changed
 * characteristic, the stack will monitor the local database change status and
 * manage service changed indications for a GATT client that has enabled the
 * indication configuration of the Service Changed characteristic.
 *
 * @param[in] caps Bit flags of capabilities to reset. Value 0 sets the default
 *   database capabilities.
 * @param[in] reserved Use the value 0 on this reserved field. Do not use
 *   none-zero values because they are reserved for future use.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_set_capabilities(uint32_t caps,
                                               uint32_t reserved);

/***************************************************************************//**
 *
 * Enable additional capabilities in the local GATT database. Already enabled
 * capabilities keep unchanged after this command. See @ref
 * sl_bt_gatt_server_set_capabilities for more information.
 *
 * @param[in] caps Capabilities to enable
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_enable_capabilities(uint32_t caps);

/***************************************************************************//**
 *
 * Disable the given capabilities in the local GATT database. See @ref
 * sl_bt_gatt_server_set_capabilities for more information.
 *
 * @param[in] caps Capabilities to disable
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_disable_capabilities(uint32_t caps);

/***************************************************************************//**
 *
 * Get capabilities currently enabled in the local GATT database.
 *
 * @param[out] caps Enabled capabilities
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_get_enabled_capabilities(uint32_t *caps);

/***************************************************************************//**
 *
 * Set the maximum size of ATT Message Transfer Units (MTU). The functionality
 * is the same as @ref sl_bt_gatt_set_max_mtu and this setting applies to both
 * GATT client and server. If the given value is too large according to the
 * maximum BGAPI payload size, the system will select the maximum possible value
 * as the maximum ATT_MTU. If the maximum ATT_MTU is larger than 23, the GATT
 * client in the stack will automatically send an MTU exchange request after a
 * Bluetooth connection was established.
 *
 * @param[in] max_mtu @parblock
 *   Maximum size of Message Transfer Units (MTU) allowed
 *     - Range: 23 to 250
 *
 *   Default: 247
 *   @endparblock
 * @param[out] max_mtu_out The maximum ATT_MTU selected by the system if this
 *   command succeeded
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_set_max_mtu(uint16_t max_mtu,
                                          uint16_t *max_mtu_out);

/***************************************************************************//**
 *
 * Get the size of ATT Message Transfer Units (MTU) for a connection.
 *
 * @param[in] connection Connection handle
 * @param[out] mtu The maximum ATT_MTU used by the connection
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_get_mtu(uint8_t connection, uint16_t *mtu);

/***************************************************************************//**
 *
 * Find attributes of a certain type from a local GATT database. The type is
 * usually given as a 16-bit or 128-bit UUID in little endian format.
 *
 * @param[in] start Search start handle
 * @param[in] type_len Array length
 * @param[in] type The attribute type UUID
 * @param[out] attribute Attribute handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_find_attribute(uint16_t start,
                                             size_t type_len,
                                             const uint8_t* type,
                                             uint16_t *attribute);

/***************************************************************************//**
 *
 * Read the value of an attribute from a local GATT database. Only (maximum
 * BGAPI payload size - 3) amount of data can be read at once. The application
 * can continue reading with increased offset value if it receives (maximum
 * BGAPI payload size - 3) amount of data.
 *
 * @param[in] attribute Attribute handle
 * @param[in] offset Value offset
 * @param[in] max_value_size Size of output buffer passed in @p value
 * @param[out] value_len On return, set to the length of output data written to
 *   @p value
 * @param[out] value The attribute value
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_read_attribute_value(uint16_t attribute,
                                                   uint16_t offset,
                                                   size_t max_value_size,
                                                   size_t *value_len,
                                                   uint8_t *value);

/***************************************************************************//**
 *
 * Read the type of an attribute from a local GATT database. The type is a UUID,
 * usually 16 or 128 bits long in little endian format.
 *
 * @param[in] attribute Attribute handle
 * @param[in] max_type_size Size of output buffer passed in @p type
 * @param[out] type_len On return, set to the length of output data written to
 *   @p type
 * @param[out] type The attribute type UUID
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_read_attribute_type(uint16_t attribute,
                                                  size_t max_type_size,
                                                  size_t *type_len,
                                                  uint8_t *type);

/***************************************************************************//**
 *
 * Write the value of an attribute in the local GATT database. Writing the value
 * of a characteristic of the local GATT database will not trigger notifications
 * or indications to the remote GATT client if the characteristic has a property
 * to indicate or notify and the client has enabled notification or indication.
 * Notifications and indications are sent to the remote GATT client using @ref
 * sl_bt_gatt_server_send_characteristic_notification command.
 *
 * @param[in] attribute Attribute handle
 * @param[in] offset Value offset
 * @param[in] value_len Array length
 * @param[in] value Value
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_write_attribute_value(uint16_t attribute,
                                                    uint16_t offset,
                                                    size_t value_len,
                                                    const uint8_t* value);

/***************************************************************************//**
 *
 * Send a response to a @ref sl_bt_evt_gatt_server_user_read_request event. The
 * response needs to be sent within 30 seconds, otherwise no more GATT
 * transactions are allowed by the remote side. If attr_errorcode is set to 0,
 * the characteristic value is sent to the remote GATT client in the standard
 * way. Other attr_errorcode values will cause the local GATT server to send an
 * attribute protocol error response instead of the actual data. At most ATT_MTU
 * \- 1 amount of data can be sent at one time. The client will continue reading
 * by sending new read request with an increased offset value if it receives
 * ATT_MTU - 1 amount of data.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle received in the @ref
 *   sl_bt_evt_gatt_server_user_read_request event.
 * @param[in] att_errorcode Attribute protocol error code
 *     - <b>0:</b> No error
 *     - <b>Non-zero:</b> See Bluetooth specification, Host volume, Attribute
 *       Protocol, Error Codes table.
 * @param[in] value_len Array length
 * @param[in] value Characteristic value to send to the GATT client. Ignored if
 *   att_errorcode is not 0.
 * @param[out] sent_len The length of data sent to the remote GATT client
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_send_user_read_response(uint8_t connection,
                                                      uint16_t characteristic,
                                                      uint8_t att_errorcode,
                                                      size_t value_len,
                                                      const uint8_t* value,
                                                      uint16_t *sent_len);

/***************************************************************************//**
 *
 * Send a response to a @ref sl_bt_evt_gatt_server_user_write_request event when
 * parameter @p att_opcode in the event is @ref gatt_write_request (see @ref
 * gatt_att_opcode). The response needs to be sent within 30 seconds, otherwise
 * no more GATT transactions are allowed by the remote side. If attr_errorcode
 * is set to 0, the ATT protocol's write response is sent to indicate to the
 * remote GATT client that the write operation was processed successfully. Other
 * values will cause the local GATT server to send an ATT protocol error
 * response.
 *
 * @param[in] connection Connection handle
 * @param[in] characteristic GATT characteristic handle received in the @ref
 *   sl_bt_evt_gatt_server_user_write_request event
 * @param[in] att_errorcode Attribute protocol error code
 *     - <b>0:</b> No error
 *     - <b>Non-zero:</b> See Bluetooth specification, Host volume, Attribute
 *       Protocol, Error Codes table.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_send_user_write_response(uint8_t connection,
                                                       uint16_t characteristic,
                                                       uint8_t att_errorcode);

/***************************************************************************//**
 *
 * Send notifications or indications to one or more remote GATT clients. At most
 * ATT_MTU - 3 amount of data can be sent one time.
 *
 * A notification or indication is sent only if the client has enabled it by
 * setting the corresponding flag to the Client Characteristic Configuration
 * descriptor. If the Client Characteristic Configuration descriptor supports
 * both notifications and indications, the stack will always send a notification
 * even when the client has enabled both.
 *
 * A new indication to a GATT client can't be sent until an outstanding
 * indication procedure with the same client has completed. The procedure is
 * completed when a confirmation from the client is received. The confirmation
 * is indicated by @ref sl_bt_evt_gatt_server_characteristic_status.
 *
 * Error bg_err_wrong_state is returned if the characteristic does not have the
 * notification property, or if the client has not enabled the notification. The
 * same applies to the indication property, and in addition, bg_err_wrong_state
 * is returned if an indication procedure with the same client is outstanding.
 * Always check the response for this command for errors before trying to send
 * more data.
 *
 * @param[in] connection A handle of the connection over which the notification
 *   or indication is sent. Values:
 *     - <b>0xff:</b> Sends notification or indication to all connected devices.
 *     - <b>Other:</b> Connection handle
 * @param[in] characteristic Characteristic handle
 * @param[in] value_len Array length
 * @param[in] value Value to be notified or indicated
 * @param[out] sent_len The length of data sent if only one connected device is
 *   the receiver; otherwise an unused value.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_gatt_server_send_characteristic_notification(uint8_t connection,
                                                               uint16_t characteristic,
                                                               size_t value_len,
                                                               const uint8_t* value,
                                                               uint16_t *sent_len);

/** @} */ // end addtogroup sl_bt_gatt_server

/**
 * @addtogroup sl_bt_nvm NVM
 * @{
 *
 * @brief NVM
 *
 * Provide an interface to manage user data objects (key/value pairs) in the
 * flash memory. User data stored within the flash memory is persistent across
 * reset and power cycling of the device. Because Bluetooth bondings are also
 * stored in this flash area, in addition to the flash storage size, the space
 * available for user data also depends on the number of bondings the device has
 * at the time.
 *
 * On EFR32[B|M]G1x devices, either PS Store or NVM3 data storage driver can be
 * used. As PS Store is supported by the Bluetooth stack only, using NVM3 is
 * recommended if the device needs to support Dynamic Multiple Protocol (DMP)
 * cases. On EFR32[B|M]G2x devices, only NVM3 is supported. When NVM3 is used,
 * applications can also use the NVM3 APIs directly.
 *
 * In PS Store, the flash storage size is fixed at 2048 bytes. The maximum data
 * object size associated to a key is 56 bytes. A Bluetooth bonding uses at
 * maximum 138 bytes for secure connections and 174 bytes for legacy pairing.
 *
 * In NVM3, the flash store size is configurable, and the minimum is 3 flash
 * pages. The maximum data object size is configurable up to 4096 bytes. A
 * Bluetooth bonding uses at maximum 110 bytes for secure connections and 138
 * bytes for legacy pairing. See more details in AN1135 "Using Third Generation
 * NonVolatile Memory (NVM3) Data Storage".
 */

/***************************************************************************//**
 *
 * Store a value into the specified NVM key. Allowed NVM keys are in range from
 * 0x4000 to 0x407F. At most, 56 bytes user data can be stored in one NVM key.
 * Error code 0x018a (command_too_long) is returned if the value data is more
 * than 56 bytes.
 *
 * @param[in] key NVM key
 * @param[in] value_len Array length
 * @param[in] value Value to store into the specified NVM key
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_nvm_save(uint16_t key,
                           size_t value_len,
                           const uint8_t* value);

/***************************************************************************//**
 *
 * Retrieve the value of the specified NVM key.
 *
 * @param[in] key NVM key of the value to be retrieved
 * @param[in] max_value_size Size of output buffer passed in @p value
 * @param[out] value_len On return, set to the length of output data written to
 *   @p value
 * @param[out] value The returned value of the specified NVM key
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_nvm_load(uint16_t key,
                           size_t max_value_size,
                           size_t *value_len,
                           uint8_t *value);

/***************************************************************************//**
 *
 * Delete a single NVM key and its value from the persistent store.
 *
 * @param[in] key NVM key to delete
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_nvm_erase(uint16_t key);

/***************************************************************************//**
 *
 * Delete all NVM keys and their corresponding values.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_nvm_erase_all();

/** @} */ // end addtogroup sl_bt_nvm

/**
 * @addtogroup sl_bt_test Testing Commands
 * @{
 *
 * @brief Testing Commands
 *

 */

/***************************************************************************//**
 *
 * Start a transmitter test against a separate Bluetooth tester device. When the
 * command is processed by the radio, a @ref sl_bt_evt_test_dtm_completed event
 * is triggered. This event indicates whether the test started successfully.
 *
 * In the transmitter test, the device sends packets continuously with a fixed
 * interval. The type and length of each packet is set by @p packet_type and @p
 * length parameters. The parameter @p phy specifies which PHY is used to
 * transmit the packets. All devices support at least 1M PHY. A special packet
 * type, <b>test_pkt_carrier</b> , can be used to transmit continuous
 * unmodulated carrier. The <b>length</b> field is ignored in this mode.
 *
 * The test may be stopped using the @ref sl_bt_test_dtm_end command.
 *
 * @param[in] packet_type Enum @ref test_packet_type_t. Packet type to transmit.
 *   Values:
 *     - <b>test_pkt_prbs9 (0x0):</b> PRBS9 packet payload
 *     - <b>test_pkt_11110000 (0x1):</b> 11110000 packet payload
 *     - <b>test_pkt_10101010 (0x2):</b> 10101010 packet payload
 *     - <b>test_pkt_11111111 (0x4):</b> 11111111 packet payload
 *     - <b>test_pkt_00000000 (0x5):</b> 00000000 packet payload
 *     - <b>test_pkt_00001111 (0x6):</b> 00001111 packet payload
 *     - <b>test_pkt_01010101 (0x7):</b> 01010101 packet payload
 *     - <b>test_pkt_pn9 (0xfd):</b> PN9 continuously modulated output
 *     - <b>test_pkt_carrier (0xfe):</b> Unmodulated carrier
 * @param[in] length @parblock
 *   Packet length in bytes
 *
 *   <b>Range:</b> 0-255
 *   @endparblock
 * @param[in] channel @parblock
 *   Bluetooth channel
 *
 *   <b>Range:</b> 0-39
 *
 *   Channel is (F - 2402) / 2,
 *
 *   where F is frequency in MHz
 *   @endparblock
 * @param[in] phy Enum @ref test_phy_t. PHY to use. Values:
 *     - <b>test_phy_1m (0x1):</b> 1M PHY
 *     - <b>test_phy_2m (0x2):</b> 2M PHY
 *     - <b>test_phy_125k (0x3):</b> 125k Coded PHY
 *     - <b>test_phy_500k (0x4):</b> 500k Coded PHY
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_test_dtm_completed - This event is received when the
 *     command is processed.
 *
 ******************************************************************************/
sl_status_t sl_bt_test_dtm_tx(uint8_t packet_type,
                              uint8_t length,
                              uint8_t channel,
                              uint8_t phy);

/***************************************************************************//**
 *
 * Start a receiver test against a separate Bluetooth tester device. When the
 * command is processed by the radio, a @ref sl_bt_evt_test_dtm_completed event
 * is triggered. This event indicates whether the test started successfully.
 *
 * Parameter @p phy specifies which PHY is used to receive the packets. All
 * devices support at least 1M PHY.
 *
 * The test may be stopped using @ref sl_bt_test_dtm_end command. This will
 * trigger another @ref sl_bt_evt_test_dtm_completed event, which carries the
 * number of packets received during the test.
 *
 * @param[in] channel @parblock
 *   Bluetooth channel
 *
 *   <b>Range:</b> 0-39
 *
 *   Channel is (F - 2402) / 2,
 *
 *   where F is frequency in MHz
 *   @endparblock
 * @param[in] phy Enum @ref test_phy_t. PHY to use. Values:
 *     - <b>test_phy_1m (0x1):</b> 1M PHY
 *     - <b>test_phy_2m (0x2):</b> 2M PHY
 *     - <b>test_phy_125k (0x3):</b> 125k Coded PHY
 *     - <b>test_phy_500k (0x4):</b> 500k Coded PHY
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_test_dtm_completed - This event is received when the
 *     command is processed.
 *
 ******************************************************************************/
sl_status_t sl_bt_test_dtm_rx(uint8_t channel, uint8_t phy);

/***************************************************************************//**
 *
 * End a transmitter or a receiver test. When the command is processed by the
 * radio and the test has ended, a @ref sl_bt_evt_test_dtm_completed event is
 * triggered.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_test_dtm_completed - Received when the command is
 *     processed by the radio and the test has ended.
 *
 ******************************************************************************/
sl_status_t sl_bt_test_dtm_end();

/** @} */ // end addtogroup sl_bt_test

/**
 * @addtogroup sl_bt_sm Security Manager
 * @{
 *
 * @brief Security Manager
 *
 * The commands in this class manage Bluetooth security, including commands for
 * starting and stopping encryption and commands for management of all bonding
 * operations.
 *
 * The following procedure is used to bond with a remote device:
 *   - Use command @ref sl_bt_sm_configure to configure security requirements
 *     and I/O capabilities of this device.
 *   - Use command @ref sl_bt_sm_set_bondable_mode to set this device into
 *     bondable mode.
 *   - Use command @ref sl_bt_connection_open to open a connection to the remote
 *     device.
 *   - After the connection is open, use command @ref sl_bt_sm_increase_security
 *     to encrypt the connection. This will also start the bonding process.
 *
 * The following procedure can be used to respond to the bonding initiated by a
 * remote device:
 *   - Use command @ref sl_bt_sm_configure to configure security requirements
 *     and I/O capabilities of this device.
 *   - Use command @ref sl_bt_sm_set_bondable_mode to set this device into
 *     bondable mode.
 *   - Use command @ref sl_bt_advertiser_start to set this device into
 *     advertising and connectable mode.
 *   - Open a connection to this device from the remote device.
 *   - After the connection is open, start the bonding process on the remote
 *     device.
 *
 * If MITM is required, the application needs to display or ask the user to
 * enter a passkey during the process. See events @ref
 * sl_bt_evt_sm_passkey_display and @ref sl_bt_evt_sm_passkey_request for more
 * information.
 */

/***************************************************************************//**
 *
 * Configure security requirements and I/O capabilities of the system.
 *
 * @param[in] flags @parblock
 *   Security requirement bitmask.
 *
 *   Bit 0:
 *     - <b>0:</b> Allow bonding without MITM protection
 *     - <b>1:</b> Bonding requires MITM protection
 *
 *   Bit 1:
 *     - <b>0:</b> Allow encryption without bonding
 *     - <b>1:</b> Encryption requires bonding. Note that this setting will also
 *       enable bonding.
 *
 *   Bit 2:
 *     - <b>0:</b> Allow bonding with legacy pairing
 *     - <b>1:</b> Secure connections only
 *
 *   Bit 3:
 *     - <b>0:</b> Bonding request does not need to be confirmed
 *     - <b>1:</b> Bonding requests need to be confirmed. Received bonding
 *       requests are notified by @ref sl_bt_evt_sm_confirm_bonding
 *
 *   Bit 4:
 *     - <b>0:</b> Allow all connections
 *     - <b>1:</b> Allow connections only from bonded devices
 *
 *   Bit 5 to 7: Reserved
 *
 *   Default value: 0x00
 *   @endparblock
 * @param[in] io_capabilities Enum @ref sm_io_capability_t. I/O Capabilities.
 *   The default I/O Capability used by the stack is No Input and No Output.
 *   Values:
 *     - <b>sm_io_capability_displayonly (0x0):</b> Display Only
 *     - <b>sm_io_capability_displayyesno (0x1):</b> Display with Yes/No-buttons
 *     - <b>sm_io_capability_keyboardonly (0x2):</b> Keyboard Only
 *     - <b>sm_io_capability_noinputnooutput (0x3):</b> No Input and No Output
 *     - <b>sm_io_capability_keyboarddisplay (0x4):</b> Display with Keyboard
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_configure(uint8_t flags, uint8_t io_capabilities);

/***************************************************************************//**
 *
 * Set the minimum allowed key size used for bonding. The default value is 16
 * bytes.
 *
 * @param[in] minimum_key_size Minimum allowed key size for bonding. Range: 7 to
 *   16
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_set_minimum_key_size(uint8_t minimum_key_size);

/***************************************************************************//**
 *
 * Set Security Manager in debug mode. In this mode, the secure connections
 * bonding uses known debug keys, so that the encrypted packet can be opened by
 * Bluetooth protocol analyzer. To disable the debug mode, restart the device.
 *
 * Bondings made in debug mode are unsecure.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_set_debug_mode();

/***************************************************************************//**
 *
 * Add device to whitelist, which can be enabled with @ref
 * sl_bt_gap_enable_whitelisting.
 *
 * @param[in] address Address of the device added to whitelist
 * @param[in] address_type Enum @ref gap_address_type_t. Address type of the
 *   device added to whitelist. Values:
 *     - <b>gap_public_address (0x0):</b> Public device address
 *     - <b>gap_static_address (0x1):</b> Static device address
 *     - <b>gap_random_resolvable_address (0x2):</b> Private resolvable random
 *       address
 *     - <b>gap_random_nonresolvable_address (0x3):</b> Private non-resolvable
 *       random address
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_add_to_whitelist(bd_addr address, uint8_t address_type);

/***************************************************************************//**
 *
 * Set the maximum allowed bonding count and bonding policy. The maximum number
 * of bondings that can be supported depends on how much user data is stored in
 * the NVM and the NVM size. When bond policy value 1 or 2 is selected the stack
 * will automatically write the new bond, as per the policy, only if the maximum
 * allowed bonding count has been reached. If the stack is not able to write a
 * new bond for any other reason (e.g. nvm full) then an error will be thrown
 * through the bonding_failed event indicating why the bonding could not be
 * written. It is left up to the application to manually release space from the
 * nvm (e.g. by deleting one of the existing bonds or application data) so that
 * a new bond can be saved. The default value is 13.
 *
 * @param[in] max_bonding_count Maximum allowed bonding count. Range: 1 to 32
 * @param[in] policy_flags @parblock
 *   Bonding policy. Values:
 *     - <b>0:</b> If database is full, new bonding attempts will fail
 *     - <b>1:</b> New bonding will overwrite the oldest existing bonding
 *     - <b>2:</b> New bonding will overwrite the existing bonding that was used
 *       the longest time ago
 *
 *   Default: 0
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_store_bonding_configuration(uint8_t max_bonding_count,
                                                 uint8_t policy_flags);

/***************************************************************************//**
 *
 * Set whether the device should accept new bondings. By default, the device
 * does not accept new bondings.
 *
 * @param[in] bondable @parblock
 *   Bondable mode. Values:
 *     - <b>0:</b> New bondings not accepted
 *     - <b>1:</b> Bondings allowed
 *
 *   Default value: 0
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_set_bondable_mode(uint8_t bondable);

/***************************************************************************//**
 *
 * Enter a fixed passkey, which will be used in the @ref
 * sl_bt_evt_sm_passkey_display event.
 *
 * @param[in] passkey Passkey. Valid range: 0-999999. Set -1 to disable and
 *   start using random passkeys.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_set_passkey(int32_t passkey);

/***************************************************************************//**
 *
 * Set OOB data (out-of-band encryption data) for legacy pairing for a device.
 * OOB data may be, for example, a PIN code exchanged over an alternate path,
 * such as NFC. The device will not allow any other bonding if OOB data is set.
 * OOB data can't be set simultaneously with secure connections OOB data.
 *
 * @param[in] oob_data_len Array length
 * @param[in] oob_data OOB data. To set OOB data, send a 16-byte array. To clear
 *   OOB data, send a zero-length array.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_set_oob_data(size_t oob_data_len, const uint8_t* oob_data);

/***************************************************************************//**
 *
 * Enable the use of OOB data (out-of-band encryption data) for a device for
 * secure connections pairing. Enabling will generate new OOB data and confirm
 * values, which can be sent to the remote device. After enabling the secure
 * connections OOB data, the remote devices OOB data can be set with @ref
 * sl_bt_sm_set_sc_remote_oob_data. Calling this function will erase any set
 * remote device OOB data and confirm values. The device will not allow any
 * other bonding if OOB data is set. The secure connections OOB data cannot be
 * enabled simultaneously with legacy pairing OOB data.
 *
 * @param[in] enable Enable OOB with secure connections pairing. Values:
 *     - <b>0:</b> disable
 *     - <b>1:</b> enable
 * @param[in] max_oob_data_size Size of output buffer passed in @p oob_data
 * @param[out] oob_data_len On return, set to the length of output data written
 *   to @p oob_data
 * @param[out] oob_data OOB data. 32-byte array. The first 16-bytes contain
 *   randomly-generated OOB data and the last 16-bytes confirm value.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_use_sc_oob(uint8_t enable,
                                size_t max_oob_data_size,
                                size_t *oob_data_len,
                                uint8_t *oob_data);

/***************************************************************************//**
 *
 * Set OOB data and confirm values (out-of-band encryption) received from the
 * remote device for secure connections pairing. OOB data must be enabled with
 * @ref sl_bt_sm_use_sc_oob before setting the remote device OOB data.
 *
 * @param[in] oob_data_len Array length
 * @param[in] oob_data Remote device OOB data and confirm values. To set OOB
 *   data, send a 32-byte array. First 16-bytes is OOB data and last 16-bytes
 *   the confirm value. To clear OOB data, send a zero-length array.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_set_sc_remote_oob_data(size_t oob_data_len,
                                            const uint8_t* oob_data);

/***************************************************************************//**
 *
 * Enhance the security of a connection to current security requirements. On an
 * unencrypted connection, it will encrypt the connection and will also perform
 * bonding if requested by both devices. On an encrypted connection, it will
 * cause the connection to be re-encrypted.
 *
 * @param[in] connection Connection handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_connection_parameters - Triggered after increasing
 *     security has been completed successfully and indicates the latest
 *     security mode of the connection.
 *   - @ref sl_bt_evt_sm_bonded - Triggered if pairing or bonding was performed
 *     in this operation and the result is successful.
 *   - @ref sl_bt_evt_sm_bonding_failed - Triggered if pairing or bonding was
 *     performed in this operation and the result has failed.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_increase_security(uint8_t connection);

/***************************************************************************//**
 *
 * Enter a passkey after receiving a passkey request event.
 *
 * @param[in] connection Connection handle
 * @param[in] passkey Passkey. Valid range: 0-999999. Set -1 to cancel pairing.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_enter_passkey(uint8_t connection, int32_t passkey);

/***************************************************************************//**
 *
 * Accept or reject the reported passkey confirm value.
 *
 * @param[in] connection Connection handle
 * @param[in] confirm Acceptance. Values:
 *     - <b>0:</b> Reject
 *     - <b>1:</b> Accept confirm value
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_passkey_confirm(uint8_t connection, uint8_t confirm);

/***************************************************************************//**
 *
 * Accept or reject the bonding request.
 *
 * @param[in] connection Connection handle
 * @param[in] confirm Acceptance. Values:
 *     - <b>0:</b> Reject
 *     - <b>1:</b> Accept bonding request
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_bonding_confirm(uint8_t connection, uint8_t confirm);

/***************************************************************************//**
 *
 * List all bondings stored in the bonding database. Bondings are reported by
 * the @ref sl_bt_evt_sm_list_bonding_entry event for each bonding and the
 * report is ended with @ref sl_bt_evt_sm_list_all_bondings_complete event. Use
 * only for debugging purposes because reading from the persistent store is
 * relatively slow.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_sm_list_bonding_entry
 *   - @ref sl_bt_evt_sm_list_all_bondings_complete
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_list_all_bondings();

/***************************************************************************//**
 *
 * Delete specified bonding information or whitelist from the persistent store.
 *
 * @param[in] bonding Bonding handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_delete_bonding(uint8_t bonding);

/***************************************************************************//**
 *
 * Delete all bonding information and whitelist from the persistent store.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_sm_delete_bondings();

/** @} */ // end addtogroup sl_bt_sm

/**
 * @addtogroup sl_bt_ota OTA
 * @{
 *
 * @brief OTA
 *
 * Commands for configuring OTA DFU.
 */

/***************************************************************************//**
 *
 * Set the device name to be used during the OTA update. The name is stored in
 * the persistent store. Maximum name length is 17 bytes.
 *
 * Default is "OTA" if a name is not set.
 *
 * @param[in] name_len Array length
 * @param[in] name OTA device name
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_ota_set_device_name(size_t name_len, const uint8_t* name);

/***************************************************************************//**
 *
 * Set advertising packets in OTA. Maximum 31 bytes of data can be set.
 *
 * @param[in] packet_type This value selects whether data is intended for
 *   advertising packets or scan response packets.
 *     - <b>2:</b> OTA advertising packets
 *     - <b>4:</b> OTA scan response packets
 * @param[in] adv_data_len Array length
 * @param[in] adv_data Data to be set
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_ota_set_advertising_data(uint8_t packet_type,
                                           size_t adv_data_len,
                                           const uint8_t* adv_data);

/***************************************************************************//**
 *
 * Set OTA configuration. The setting is stored in the persistent store.
 *
 * @param[in] flags @parblock
 *   OTA configuration flags. This value is given as a bitmask. Flags:
 *
 *   Bit 0: Advertising address
 *
 *     - <b>0:</b> Use public device address
 *     - <b>1:</b> Use a random address
 *
 *   Bit 1: Application update version check. Check the version number and
 *   product ID of the application upgrade before applying. If the version
 *   number of the current application can not be determined, this
 *   implementation will assume that it is OK to apply the new image. Note that
 *   this is not a security feature.
 *
 *     - <b>0:</b> Disable version check
 *     - <b>1:</b> Enable version check
 *
 *   Bit 2 to 31: Reserved
 *
 *   Default value: 0
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_ota_set_configuration(uint32_t flags);

/***************************************************************************//**
 *
 * Set RF path antenna for OTA. Should be used only if device has multiple
 * antenna ports. The setting is stored in the persistent store.
 *
 * @param[in] enable If enabled antenna selection is used in OTA, otherwise
 *   default antenna is used.
 * @param[in] antenna Set antenna used in OTA. Value should be come from
 *   RAIL_AntennaSel_t enum. See antenna path selection in RAIL
 *   rail_chip_specific.h.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_ota_set_rf_path(uint8_t enable, uint8_t antenna);

/** @} */ // end addtogroup sl_bt_ota

/**
 * @addtogroup sl_bt_coex Coexistence
 * @{
 *
 * @brief Coexistence
 *
 * Coexistence BGAPI class. Coexistence interface is enabled and initialized
 * with @ref sl_bt_init_coex_hal function.
 */

/***************************************************************************//**
 *
 * Configure coexistence options at runtime.
 *
 * @param[in] mask Enum @ref coex_option_t. Mask defines which coexistence
 *   options are changed.
 * @param[in] options Enum @ref coex_option_t. Value of options to be changed.
 *   This parameter is used together with the mask parameter.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_coex_set_options(uint32_t mask, uint32_t options);

/***************************************************************************//**
 *
 * Configure coexistence parameters.
 *
 * @param[in] priority Coexistence priority threshold. Coexistence priority is
 *   toggled if priority is below this value.
 * @param[in] request Coexistence request threshold. Coexistence request is
 *   toggled if priority is below this value.
 * @param[in] pwm_period PWM functionality period length in 1ms units
 * @param[in] pwm_dutycycle PWM functionality dutycycle in percentage
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_coex_set_parameters(uint8_t priority,
                                      uint8_t request,
                                      uint8_t pwm_period,
                                      uint8_t pwm_dutycycle);

/***************************************************************************//**
 *
 * Set Directional Priority Pulse Width
 *
 * @param[in] pulse Directional priority pulse width in us
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_coex_set_directional_priority_pulse(uint8_t pulse);

/***************************************************************************//**
 *
 * Read coexistence statistic counters from the device. Response contains the
 * list of uint32 type counter values. Counters in the list are in following
 * order: low priority requested, high priority requested, low priority denied,
 * high priority denied, low-priority TX aborted, and high-priority TX aborted.
 * Passing a non-zero value also resets counters.
 *
 * @param[in] reset Reset counters if parameter value is not zero.
 * @param[in] max_counters_size Size of output buffer passed in @p counters
 * @param[out] counters_len On return, set to the length of output data written
 *   to @p counters
 * @param[out] counters Coexistence statistic counters
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_coex_get_counters(uint8_t reset,
                                    size_t max_counters_size,
                                    size_t *counters_len,
                                    uint8_t *counters);

/** @} */ // end addtogroup sl_bt_coex

/**
 * @cond RESTRICTED
 * @addtogroup sl_bt_l2cap L2CAP
 * @{
 *
 * @brief L2CAP
 *
 * The commands and events in this class are related to Logical Link Control and
 * Adaptation Protocol (L2CAP) in Bluetooth.
 */

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Send LE credit-based connection request.
 *
 * @param[in] connection Handle of the LE connection to be used for opening
 *   connection-oriented channel.
 * @param[in] le_psm LE Protocol/Service Multiplexer - LE_PSM
 * @param[in] mtu @parblock
 *   The maximum size of payload data that the application on the device sending
 *   the request can accept, i.e., the MTU corresponds to the maximum SDU size.
 *
 *   Range: 23 to 65533.
 *
 *   Application needs to handle segmentation and reassembly from PDU to SDU.
 *   @endparblock
 * @param[in] mps @parblock
 *   The maximum size of payload data that the L2CAP layer on the device sending
 *   the request can accept, i.e., the MPS corresponds to the maximum PDU
 *   payload size.
 *
 *   Range: 23 to 250.
 *
 *   That is the maximum size of data that the application can send using @ref
 *   sl_bt_l2cap_coc_send_data command or receive by @ref
 *   sl_bt_evt_l2cap_coc_data event.
 *   @endparblock
 * @param[in] initial_credit The initial credit value indicates the number of
 *   PDUs that the peer device can send.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_l2cap_coc_connection_response - Triggered when a LE
 *     credit-based connection connection response has been received in response
 *     to this command.
 *   - @ref sl_bt_evt_l2cap_coc_channel_disconnected - Triggered when a LE
 *     credit-based connection connection response has not been received within
 *     the 30 seconds timeout in response to this command.
 *
 * @endcond
 ******************************************************************************/
sl_status_t sl_bt_l2cap_coc_send_connection_request(uint8_t connection,
                                                    uint16_t le_psm,
                                                    uint16_t mtu,
                                                    uint16_t mps,
                                                    uint16_t initial_credit);

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Send LE credit-based connection response.
 *
 * @param[in] connection Handle of the LE connection to be used for opening
 *   connection-oriented channel
 * @param[in] cid The CID represents the destination channel endpoint of the
 *   device sending the response which is same as source CID field of
 *   corresponding request message
 * @param[in] mtu @parblock
 *   The maximum size of payload data that the application on the device sending
 *   the response can accept, i.e., the MTU corresponds to the maximum SDU size.
 *
 *   Range: 23 to 65533.
 *
 *   Application needs to handle segmentation and reassembly from PDU to SDU.
 *   @endparblock
 * @param[in] mps @parblock
 *   The maximum size of payload data that the L2CAP layer on the device sending
 *   the response can accept, i.e., the MPS corresponds to the maximum PDU
 *   payload size.
 *
 *   Range: 23 to 250.
 *
 *   That is the maximum size of data that the application is able to send using
 *   @ref sl_bt_l2cap_coc_send_data command or receive by @ref
 *   sl_bt_evt_l2cap_coc_data event.
 *   @endparblock
 * @param[in] initial_credit The initial credit value indicates the number of
 *   PDUs that the peer device can send
 * @param[in] l2cap_errorcode Enum @ref l2cap_coc_connection_result_t. The
 *   l2cap_errorcode field indicates the result of the connection request.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @endcond
 ******************************************************************************/
sl_status_t sl_bt_l2cap_coc_send_connection_response(uint8_t connection,
                                                     uint16_t cid,
                                                     uint16_t mtu,
                                                     uint16_t mps,
                                                     uint16_t initial_credit,
                                                     uint16_t l2cap_errorcode);

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Send LE flow control credit indicating that the channel endpoint on local
 * device is capable of receiving more data.
 *
 * @param[in] connection Handle of the LE connection for sending flow control
 *   credit.
 * @param[in] cid The CID represents the destination channel endpoint of the
 *   device sending the flow control credit.
 * @param[in] credits The credit value indicates the additional number of PDUs
 *   that the peer device can send.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @endcond
 ******************************************************************************/
sl_status_t sl_bt_l2cap_coc_send_le_flow_control_credit(uint8_t connection,
                                                        uint16_t cid,
                                                        uint16_t credits);

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Send L2CAP connection-oriented channel disconnection request.
 *
 * @param[in] connection Handle of the LE connection for terminating the
 *   connection-oriented channel
 * @param[in] cid The CID represents the destination channel endpoint of the
 *   device sending the disconnection request.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_l2cap_coc_channel_disconnected - Triggered when a L2CAP
 *     channel is disconnected in response to this command.
 *
 * @endcond
 ******************************************************************************/
sl_status_t sl_bt_l2cap_coc_send_disconnection_request(uint8_t connection,
                                                       uint16_t cid);

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Send data to a L2CAP LE connection-oriented channel.
 *
 * @param[in] connection Handle of the LE connection for sending data
 * @param[in] cid The CID represents the destination channel endpoint of the
 *   device sending data.
 * @param[in] data_len Array length
 * @param[in] data Data to be sent. Data length must be within the range of
 *   destination channel endpoint's MPS value.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @endcond
 ******************************************************************************/
sl_status_t sl_bt_l2cap_coc_send_data(uint8_t connection,
                                      uint16_t cid,
                                      size_t data_len,
                                      const uint8_t* data);

/** @} */ // end addtogroup sl_bt_l2cap
/** @endcond */

/**
 * @addtogroup sl_bt_cte_transmitter CTE Transmitter
 * @{
 *
 * @brief CTE Transmitter
 *
 * Commands and events in this class manage Constant Tone Extension (CTE)
 * transmission.
 *
 * CTE feature is only supported by specific devices. Commands from this class
 * will return bg_err_not_supported on devices that do not support CTE.
 */

/***************************************************************************//**
 *
 * Set the CTE-related parameters of the LE transmitter test.
 *
 * @param[in] cte_length @parblock
 *   Length of the Constant Tone Extension in 8 us units
 *     - <b>0:</b> No CTE
 *     - <b>0x02 to 0x14:</b> CTE length
 *
 *   Default: 0 (no CTE)
 *   @endparblock
 * @param[in] cte_type @parblock
 *   CTE type
 *     - <b>0:</b> AoA CTE
 *     - <b>1:</b> AoD CTE with 1 us slots
 *     - <b>2:</b> AoD CTE with 2 us slots
 *
 *   Default: 0
 *   @endparblock
 * @param[in] switching_pattern_len Array length
 * @param[in] switching_pattern Antenna switching pattern. Antennas will be
 *   switched in this order with the antenna switch pins during CTE. If the CTE
 *   is longer than the switching pattern, the pattern starts over. Default is
 *   the empty array.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_transmitter_set_dtm_parameters(uint8_t cte_length,
                                                     uint8_t cte_type,
                                                     size_t switching_pattern_len,
                                                     const uint8_t* switching_pattern);

/***************************************************************************//**
 *
 * Clear CTE-related parameters that were previously set for LE transmitter
 * test. Default values will be restored for these parameters.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_transmitter_clear_dtm_parameters();

/***************************************************************************//**
 *
 * Enable different types of CTE responses on a connection. CTE response will be
 * sent once requested by the peer device using the CTE Request procedure.
 *
 * @param[in] connection Connection handle
 * @param[in] cte_types CTE types. Bitmask of the following:
 *     - <b>Bit 0:</b> AoA CTE response
 *     - <b>Bit 1:</b> AoD CTE response with 1 us slots
 *     - <b>Bit 2:</b> AoD CTE response with 2 us slots
 * @param[in] switching_pattern_len Array length
 * @param[in] switching_pattern Antenna switching pattern. Antennas will be
 *   switched in this order with the antenna switch pins during CTE. If the CTE
 *   is longer than the switching pattern, the pattern starts over.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_transmitter_enable_connection_cte(uint8_t connection,
                                                        uint8_t cte_types,
                                                        size_t switching_pattern_len,
                                                        const uint8_t* switching_pattern);

/***************************************************************************//**
 *
 * Disable CTE responses on a connection.
 *
 * @param[in] connection Connection handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_transmitter_disable_connection_cte(uint8_t connection);

/***************************************************************************//**
 *
 * Start connectionless CTE transmit. CTEs will be transmitted in periodic
 * advertisement packets. As a result, a periodic advertising has to be started
 * prior this command.
 *
 * @param[in] handle Periodic advertising handle
 * @param[in] cte_length CTE length in 8 us units.
 *     - Range: 0x02 to 0x14
 *     - Time Range: 16 us to 160 us
 * @param[in] cte_type CTE type
 *     - <b>0:</b> AoA CTE
 *     - <b>1:</b> AoD CTE with 1 us slots
 *     - <b>2:</b> AoD CTE with 2 us slots
 * @param[in] cte_count The number of CTEs to be transmitted in each periodic
 *   advertising interval
 * @param[in] switching_pattern_len Array length
 * @param[in] switching_pattern Antenna switching pattern. Antennas will be
 *   switched in this order with the antenna switch pins during CTE. If the CTE
 *   is longer than the switching pattern, the pattern starts over.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_transmitter_enable_connectionless_cte(uint8_t handle,
                                                            uint8_t cte_length,
                                                            uint8_t cte_type,
                                                            uint8_t cte_count,
                                                            size_t switching_pattern_len,
                                                            const uint8_t* switching_pattern);

/***************************************************************************//**
 *
 * Stop the connectionless CTE transmit.
 *
 * @param[in] handle Periodic advertising handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_transmitter_disable_connectionless_cte(uint8_t handle);

/***************************************************************************//**
 *
 * Enable Silicon Labs CTE transmit. CTEs will be transmitted in extended
 * advertisement packets. As a result, extended advertising has to be started
 * prior this command.
 *
 * @param[in] handle Advertising handle
 * @param[in] cte_length CTE length in 8 us units.
 *     - Range: 0x02 to 0x14
 *     - Time Range: 16 us to 160 us
 * @param[in] cte_type CTE type
 *     - <b>0:</b> AoA CTE
 *     - <b>1:</b> AoD CTE with 1 us slots
 *     - <b>2:</b> AoD CTE with 2 us slots
 * @param[in] cte_count The number of CTEs to be transmitted in each extended
 *   advertising interval. Currently only cte_count = 1 is supported.
 * @param[in] switching_pattern_len Array length
 * @param[in] switching_pattern Antenna switching pattern. Antennas will be
 *   switched in this order with the antenna switch pins during CTE. If the CTE
 *   is longer than the switching pattern, the pattern starts over.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_transmitter_enable_silabs_cte(uint8_t handle,
                                                    uint8_t cte_length,
                                                    uint8_t cte_type,
                                                    uint8_t cte_count,
                                                    size_t switching_pattern_len,
                                                    const uint8_t* switching_pattern);

/***************************************************************************//**
 *
 * Disable Silicon Labs CTE transmit.
 *
 * @param[in] handle Advertising handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_transmitter_disable_silabs_cte(uint8_t handle);

/** @} */ // end addtogroup sl_bt_cte_transmitter

/**
 * @addtogroup sl_bt_cte_receiver CTE Receiver
 * @{
 *
 * @brief CTE Receiver
 *
 * Commands and events in this class manage Constant Tone Extension (CTE)
 * receiving.
 *
 * CTE feature is only supported by specific devices. Commands from this class
 * will return bg_err_not_supported on devices that do not support CTE.
 */

/***************************************************************************//**
 *
 * Set CTE-related parameters of LE receiver test.
 *
 * @param[in] cte_length @parblock
 *   Expected CTE length in 8 us units
 *     - <b>0:</b> No CTE
 *     - <b>0x02 to 0x14:</b> Expected CTE length
 *
 *   Default: 0 (no CTE)
 *   @endparblock
 * @param[in] cte_type @parblock
 *   Expected CTE type
 *     - <b>0:</b> Expect AoA CTE
 *     - <b>1:</b> Expect AoD CTE with 1 us slots
 *     - <b>2:</b> Expect AoD CTE with 2 us slots
 *
 *   Default: 0
 *   @endparblock
 * @param[in] slot_durations @parblock
 *   Slot durations
 *     - <b>1:</b> Switching and sampling slots are 1 us each
 *     - <b>2:</b> Switching and sampling slots are 2 us each
 *
 *   Default: 1
 *   @endparblock
 * @param[in] switching_pattern_len Array length
 * @param[in] switching_pattern Antenna switching pattern. Antennas will be
 *   switched in this order with the antenna switch pins during CTE. If the CTE
 *   is longer than the switching pattern, the pattern starts over. Default:
 *   empty array
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_cte_receiver_dtm_iq_report - Triggered when IQ samples
 *     have been received.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_set_dtm_parameters(uint8_t cte_length,
                                                  uint8_t cte_type,
                                                  uint8_t slot_durations,
                                                  size_t switching_pattern_len,
                                                  const uint8_t* switching_pattern);

/***************************************************************************//**
 *
 * Clear CTE-related parameters that were previously set for LE receiver test.
 * Default values will be restored for these parameters.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_clear_dtm_parameters();

/***************************************************************************//**
 *
 * Set the sync CTE type to limit what types of periodic advertisers to sync to.
 * The set parameter takes effect immediately for all advertisers that have not
 * already established synchronization.
 *
 * @param[in] sync_cte_type @parblock
 *   Sync CTE type flags. This value can be a bitmask of multiple flags. Flags:
 *     - <b>1 (bit 0):</b> Do not sync to packets with an AoA Constant Tone
 *       Extension
 *     - <b>2 (bit 1):</b> Do not sync to packets with an AoD Constant Tone
 *       Extension with 1 us slots
 *     - <b>4 (bit 2):</b> Do not sync to packets with an AoD Constant Tone
 *       Extension with 2 us slots
 *     - <b>8 (bit 3):</b> Bit not used
 *     - <b>16 (bit 4):</b> Do not sync to packets without a Constant Tone
 *       Extension
 *
 *   Default: 0
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_set_sync_cte_type(uint8_t sync_cte_type);

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * Configure the CTE sampling mode.
 *
 * @param[in] flags @parblock
 *   Values:
 *     - <b>0:</b> Disable raw sample mode, only picked IQ samples are reported
 *       (1 IQ sample pair / slot)
 *     - <b>1:</b> Enable raw sample mode, every IQ sample is reported.
 *
 *   Default: 0
 *   @endparblock
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @endcond
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_configure(uint8_t flags);

/***************************************************************************//**
 *
 * Start IQ samplings on a connection. A CTE requests will be initiated
 * periodically on the given connection and IQ sampling will be made on the
 * received CTE responses.
 *
 * @param[in] connection Connection handle
 * @param[in] interval Measurement interval
 *     - <b>0:</b> No interval. The request is initiated only once.
 *     - <b>Other values N:</b> Initiate the request every N-th connection
 *       events
 * @param[in] cte_length Minimum CTE length requested in 8 us units.
 *     - Range: 0x02 to 0x14
 *     - Time Range: 16 us to 160 us
 * @param[in] cte_type Requested CTE type
 *     - <b>0:</b> AoA CTE
 *     - <b>1:</b> AoD CTE with 1 us slots
 *     - <b>2:</b> AoD CTE with 2 us slots
 * @param[in] slot_durations Slot durations
 *     - <b>1:</b> Switching and sampling slots are 1 us each
 *     - <b>2:</b> Switching and sampling slots are 2 us each
 * @param[in] switching_pattern_len Array length
 * @param[in] switching_pattern Antenna switching pattern. Antennas will be
 *   switched in this order with the antenna switch pins during CTE. If the CTE
 *   is longer than the switching pattern, the pattern starts over.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_cte_receiver_connection_iq_report - Triggered when IQ
 *     samples have been received.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_enable_connection_cte(uint8_t connection,
                                                     uint16_t interval,
                                                     uint8_t cte_length,
                                                     uint8_t cte_type,
                                                     uint8_t slot_durations,
                                                     size_t switching_pattern_len,
                                                     const uint8_t* switching_pattern);

/***************************************************************************//**
 *
 * Stop the IQ sampling on a connection. CTEs will not be requested on the given
 * connection.
 *
 * @param[in] connection Connection handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_disable_connection_cte(uint8_t connection);

/***************************************************************************//**
 *
 * Start IQ sampling on a periodic advertising synchronization. IQ samples are
 * taken on each CTE found in the periodic advertisements.
 *
 * @param[in] sync Periodic advertising synchronization handle
 * @param[in] slot_durations Slot durations
 *     - <b>1:</b> Switching and sampling slots are 1 us each
 *     - <b>2:</b> Switching and sampling slots are 2 us each
 * @param[in] cte_count   - <b>0:</b> Sample and report all available CTEs
 *     - <b>Other values:</b> Maximum number of sampled CTEs in each periodic
 *       advertising interval
 * @param[in] switching_pattern_len Array length
 * @param[in] switching_pattern Antenna switching pattern. Antennas will be
 *   switched in this order with the antenna switch pins during CTE. If the CTE
 *   is longer than the switching pattern, the pattern starts over.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_cte_receiver_connectionless_iq_report - Triggered when IQ
 *     samples have been received.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_enable_connectionless_cte(uint16_t sync,
                                                         uint8_t slot_durations,
                                                         uint8_t cte_count,
                                                         size_t switching_pattern_len,
                                                         const uint8_t* switching_pattern);

/***************************************************************************//**
 *
 * Stop IQ sampling on a periodic advertising synchronization.
 *
 * @param[in] sync Periodic advertising synchronization handle
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_disable_connectionless_cte(uint16_t sync);

/***************************************************************************//**
 *
 * Enable IQ sampling of Silicon Labs CTE found in extended advertisements.
 *
 * @param[in] slot_durations Slot durations
 *     - <b>1:</b> Switching and sampling slots are 1 us each
 *     - <b>2:</b> Switching and sampling slots are 2 us each
 * @param[in] cte_count   - <b>0:</b> Sample and report all available CTEs
 *     - <b>Other values:</b> Maximum number of sampled CTEs in each extended
 *       advertising interval
 * @param[in] switching_pattern_len Array length
 * @param[in] switching_pattern Antenna switching pattern. Antennas will be
 *   switched in this order with the antenna switch pins during CTE. If the CTE
 *   is longer than the switching pattern, the pattern starts over.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 * @b Events
 *   - @ref sl_bt_evt_cte_receiver_silabs_iq_report - Triggered when IQ samples
 *     of Silicon Labs CTE have been received.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_enable_silabs_cte(uint8_t slot_durations,
                                                 uint8_t cte_count,
                                                 size_t switching_pattern_len,
                                                 const uint8_t* switching_pattern);

/***************************************************************************//**
 *
 * Disable IQ sampling of Silicon Labs CTE.
 *
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_cte_receiver_disable_silabs_cte();

/** @} */ // end addtogroup sl_bt_cte_receiver

/**
 * @addtogroup sl_bt_user User Messaging
 * @{
 *
 * @brief User Messaging
 *
 * This class provides one command and one event which can be used by a NCP host
 * and target to implement a communication mechanism with a custom proprietary
 * protocol. An application must decide whether and how the command and event
 * are used. The stack does not produce or consume any messages belonging to
 * this class.
 */

/***************************************************************************//**
 *
 * Used by an NCP host to send a message to the target application on device.
 * The application on target is must send the response with @ref
 * sl_bt_send_rsp_user_message_to_target.
 *
 * @param[in] data_len Array length
 * @param[in] data The message
 * @param[in] max_response_size Size of output buffer passed in @p response
 * @param[out] response_len On return, set to the length of output data written
 *   to @p response
 * @param[out] response The response message
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *
 ******************************************************************************/
sl_status_t sl_bt_user_message_to_target(size_t data_len,
                                         const uint8_t* data,
                                         size_t max_response_size,
                                         size_t *response_len,
                                         uint8_t *response);

/** @} */ // end addtogroup sl_bt_user




/** @addtogroup sl_bt_utility_functions Utility Functions
 *  @brief Utility functions for applications on SoC
 *  @{
 */

/**
 * Get the next event that requires processing by user application. In a bare
 * metal application, this blocks application until new event arrives. When RTOS
 * is used, this works the same way as @ref sl_bt_pop_event.
 *
 * @param event the pointer for storing the new event
 * @return SL_STATUS_OK if a new event is returned; other value indicates an
 *   error occurred
 */
sl_status_t sl_bt_wait_event(sl_bt_msg_t* event);

/**
 * Get the next event that requires processing by user application. Application
 * is not blocked if no event is waiting.
 *
 * @param event the pointer for storing the new event
 * @return SL_STATUS_OK if a new event is returned, or SL_STATUS_NOT_FOUND if no
 *   event is waiting; other value indicates an error occurred
 */
sl_status_t sl_bt_pop_event(sl_bt_msg_t* event);

/**
 * Check whether events are in queue pending for processing.
 * Call @ref sl_bt_wait_event or @ref sl_bt_pop_event to process pending events.
 *
 * @return true if event is pending; false otherwise
 */
bool sl_bt_event_pending(void);

/**
 * Check whether events are in queue pending for processing and return the next
 * event length in bytes if events are pending.
 * Call @ref sl_bt_wait_event or @ref sl_bt_pop_event to process pending events.
 *
 * @return the next event length if event is pending; 0 otherwise
 */
uint32_t sl_bt_event_pending_len();

/**
 * Handle an API command in binary format.
 *
 * This is povided to NCP target applications for processing commands received
 * from NCP transport.
 *
 * @param hdr the command header
 * @param data the command payload in a byte array
 */
void sl_bt_handle_command(uint32_t hdr, void* data);

/**
 * Stack internal function used by sl_bt_get_command_response() API.
 */
extern void* sli_bt_get_command_response();

/**
 * Get the response to the command currently been handled.
 *
 * This is provided to NCP target applications for processing commands received
 * from NCP transport.
 */
static inline sl_bt_msg_t* sl_bt_get_command_response()
{
  return (sl_bt_msg_t*) sli_bt_get_command_response();
}

/**
 * Priority message handler function if user application requires the use of
 * PendSV interrupt.
 *
 * If scheduler_callback function pointer in configuration struct is something
 * else than NULL, then stack will not install its own PendSV IRQ handler but
 * instead uses callback/handler functions.
 *
 * When application receives call to the scheduler_callback function it must
 * schedule the call to gecko_priority_handle function to later time to run on
 * high priority thread. This callback may happen inside radio IRQ so processing
 * must not block and has to happen as fast as possible.
 *
 * Recommended implementation: High priority thread is loop that waits on binary
 * semaphore and calls gecko_priority_handler. The scheduler_callback is simple
 * function that only signals the semaphore.
 *
 */
void sl_bt_priority_handle(void);

/**
 * Signals stack that external event has happened. Signals can be used to report
 * status change from interrupt context or from other threads to application.
 * Signals are bits that are automatically cleared after application has been
 * notified.
 *
 * @param signals is a bitmask defining active signals that are reported back to
 *   the application by system_external_signal-event.
 *
 */
void sl_bt_external_signal(uint32_t signals);

/**
 * Signals stack to send system_awake event when application received wakeup
 * signal.
 */
void sl_bt_send_system_awake();

/**
 * Signals stack to send system_error event when in case of an error.
 */
void sl_bt_send_system_error(uint16_t reason, uint8_t data_len, const uint8_t *data);

/**
 * Tells if a SL_BT_API message is sensitive.
 *
 * @param[in] message_id the SL_BT_API message identifier
 * @return 1 if the message is sensitive; otherwise 0
 */
uint8_t sl_bt_is_sensitive_message(uint32_t message_id);

/**
 * Sends the NCP host a message whose SL_BT_MSG_ID is
 * gecko_rsp_user_message_to_target_id.
 *
 * This a utility helping a NCP host and target application to exchange user
 * data. Do not use it in SoC mode.
 */
void sl_bt_send_rsp_user_message_to_target(uint16_t result, uint8_t data_len, uint8_t *data);

/**
 * Sends the NCP host a message whose SL_BT_MSG_ID is
 * gecko_evt_user_message_to_host_id.
 *
 * This a utility helping a NCP host and target application to exchange user
 * data. Do not use it in SoC mode.
 */
void sl_bt_send_evt_user_message_to_host(uint8_t data_len, uint8_t *data);

/** @} */ // end addtogroup sl_bt_utility_functions



#ifdef __cplusplus
}
#endif

#endif