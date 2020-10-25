/***************************************************************************//**
 * @brief Bluetooth API data types
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

#ifndef SL_BT_TYPES_H
#define SL_BT_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef SL_BT_API
#define SL_BT_API
#endif

/**
 * The maximum SL_BT_API command payload size.
 */
#ifndef SL_BT_MAX_PAYLOAD_SIZE
#define SL_BT_MAX_PAYLOAD_SIZE 256
#endif

#ifndef SL_BT_TYPE_UINT8ARRARY
#define SL_BT_TYPE_UINT8ARRARY
/** @brief Variable-length uint8_t array */
typedef struct {
  uint8_t len;    /**< Number of bytes stored in @p data */
  uint8_t data[]; /**< Data bytes*/
} uint8array;
#endif

#ifndef SL_BT_TYPE_BYTE_ARRARY
#define SL_BT_TYPE_BYTE_ARRARY
/** @brief Variable-length int8_t array */
typedef struct {
  uint16_t len;    /**< Number of bytes stored in @p data */
  int8_t   data[]; /**< Data bytes*/
} byte_array;
#endif

#ifndef SL_BT_TYPE_BDADDR
#define SL_BT_TYPE_BDADDR
/** @brief Bluetooth address */
typedef struct {
  uint8_t addr[6]; /**< @brief Bluetooth address in reverse byte order */
} bd_addr;
#endif

#ifndef SL_BT_TYPE_UUID128
#define SL_BT_TYPE_UUID128
/** @brief 128-bit UUID */
typedef struct {
  uint8_t data[16]; /**< 128-bit UUID */
} uuid_128;
#endif

#ifndef SL_BT_TYPE_AES_KEY128
#define SL_BT_TYPE_AES_KEY128
/** @brief 128-bit AES key */
typedef struct {
  uint8_t data[16]; /**< 128-bit AES key */
} aes_key_128;
#endif

#ifndef SL_BT_TYPE_UUID64
#define SL_BT_TYPE_UUID64
/** @brief 64-bit UUID */
typedef struct {
  uint8_t data[8]; /**< 128-bit UUID */
} sl_bt_uuid_64_t;
#endif

#if !defined _MSC_VER && !defined __APPLE__
/* Don't define under MSVC and OSX */

#ifndef memcpy
void * memcpy(void *OUT, const void *IN, size_t N);
#endif
#ifndef memcmp
int  memcmp(const void *S1, const void *S2, size_t N);
#endif
#ifndef memset
void * memset(void *DST, int c, size_t LENGTH);
#endif
#ifndef memmove
void * memmove(void *DST, const void *src, size_t LENGTH);
#endif
#endif

typedef enum sl_bt_parameter_types
{
  sl_bt_msg_parameter_uint8 = 2,
  sl_bt_msg_parameter_int8 = 3,
  sl_bt_msg_parameter_uint16 = 4,
  sl_bt_msg_parameter_int16 = 5,
  sl_bt_msg_parameter_uint32 = 6,
  sl_bt_msg_parameter_int32 = 7,
  sl_bt_msg_parameter_uint8array = 8,
  sl_bt_msg_parameter_string = 9,
  sl_bt_msg_parameter_hwaddr = 10,
  sl_bt_msg_parameter_bytearray = 11
} sl_bt_parameter_types_t;

typedef enum sl_bt_msg_types
{
  sl_bt_msg_type_cmd = 0x00,
  sl_bt_msg_type_rsp = 0x00,
  sl_bt_msg_type_evt = 0x80
} sl_bt_msg_types_t;

enum sl_bt_dev_types
{
  sl_bt_dev_type_default = 0x20
};

/* Compatibility */
#ifndef PACKSTRUCT
/*Default packed configuration*/
#ifdef __GNUC__
#ifdef _WIN32
#define PACKSTRUCT( decl ) decl __attribute__((__packed__,gcc_struct))
#else
#define PACKSTRUCT( decl ) decl __attribute__((__packed__))
#endif
#define ALIGNED __attribute__((aligned(0x4)))
#elif __IAR_SYSTEMS_ICC__

#define PACKSTRUCT( decl ) __packed decl

#define ALIGNED
#elif _MSC_VER  /*msvc*/

#define PACKSTRUCT( decl ) __pragma( pack(push, 1) ) decl __pragma( pack(pop) )
#define ALIGNED
#else
#define PACKSTRUCT(a) a PACKED
#endif
#endif

/**
 * @addtogroup sl_bt_common_types Common Types
 * @brief Common types potentially used by several classes
 * @{
 */

/**
 * @brief Value used to indicate an invalid bonding handle
 */
#define SL_BT_INVALID_BONDING_HANDLE ((uint8_t) 0xFF)

/**
 * @brief Value used to indicate an invalid advertising set handle
 */
#define SL_BT_INVALID_ADVERTISING_SET_HANDLE ((uint8_t) 0xFF)

/** @} */ // end addtogroup sl_bt_common_types


/**
 * @addtogroup sl_bt_dfu Device Firmware Update
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_dfu_reset_id                                       0x00000020
#define sl_bt_cmd_dfu_flash_set_address_id                           0x01000020
#define sl_bt_cmd_dfu_flash_upload_id                                0x02000020
#define sl_bt_cmd_dfu_flash_upload_finish_id                         0x03000020
#define sl_bt_rsp_dfu_reset_id                                       0x00000020
#define sl_bt_rsp_dfu_flash_set_address_id                           0x01000020
#define sl_bt_rsp_dfu_flash_upload_id                                0x02000020
#define sl_bt_rsp_dfu_flash_upload_finish_id                         0x03000020


/**
 * @addtogroup sl_bt_evt_dfu_boot sl_bt_evt_dfu_boot
 * @{
 * @brief This event indicates that the device booted in DFU mode and is now
 * ready to receive commands related to device firmware upgrade (DFU).
 */

/** @brief Identifier of the boot event */
#define sl_bt_evt_dfu_boot_id                                        0x000000a0

/***************************************************************************//**
 * @brief Data structure of the boot event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_dfu_boot_s
{
  uint32_t version; /**< The version of the bootloader */
});

typedef struct sl_bt_evt_dfu_boot_s sl_bt_evt_dfu_boot_t;

/** @} */ // end addtogroup sl_bt_evt_dfu_boot

/**
 * @addtogroup sl_bt_evt_dfu_boot_failure sl_bt_evt_dfu_boot_failure
 * @{
 * @brief This event indicates that an error, which prevents the device from
 * booting, has occurred in bootloader.
 */

/** @brief Identifier of the boot_failure event */
#define sl_bt_evt_dfu_boot_failure_id                                0x010000a0

/***************************************************************************//**
 * @brief Data structure of the boot_failure event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_dfu_boot_failure_s
{
  uint16_t reason; /**< The reason for boot failure. */
});

typedef struct sl_bt_evt_dfu_boot_failure_s sl_bt_evt_dfu_boot_failure_t;

/** @} */ // end addtogroup sl_bt_evt_dfu_boot_failure

/** @} */ // end addtogroup sl_bt_dfu

/**
 * @addtogroup sl_bt_system System
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_system_hello_id                                    0x00010020
#define sl_bt_cmd_system_reset_id                                    0x01010020
#define sl_bt_cmd_system_halt_id                                     0x0c010020
#define sl_bt_cmd_system_linklayer_configure_id                      0x0e010020
#define sl_bt_cmd_system_set_max_tx_power_id                         0x16010020
#define sl_bt_cmd_system_set_identity_address_id                     0x13010020
#define sl_bt_cmd_system_get_identity_address_id                     0x15010020
#define sl_bt_cmd_system_get_random_data_id                          0x0b010020
#define sl_bt_cmd_system_data_buffer_write_id                        0x12010020
#define sl_bt_cmd_system_data_buffer_clear_id                        0x14010020
#define sl_bt_cmd_system_get_counters_id                             0x0f010020
#define sl_bt_cmd_system_set_soft_timer_id                           0x19010020
#define sl_bt_cmd_system_set_lazy_soft_timer_id                      0x1a010020
#define sl_bt_rsp_system_hello_id                                    0x00010020
#define sl_bt_rsp_system_reset_id                                    0x01010020
#define sl_bt_rsp_system_halt_id                                     0x0c010020
#define sl_bt_rsp_system_linklayer_configure_id                      0x0e010020
#define sl_bt_rsp_system_set_max_tx_power_id                         0x16010020
#define sl_bt_rsp_system_set_identity_address_id                     0x13010020
#define sl_bt_rsp_system_get_identity_address_id                     0x15010020
#define sl_bt_rsp_system_get_random_data_id                          0x0b010020
#define sl_bt_rsp_system_data_buffer_write_id                        0x12010020
#define sl_bt_rsp_system_data_buffer_clear_id                        0x14010020
#define sl_bt_rsp_system_get_counters_id                             0x0f010020
#define sl_bt_rsp_system_set_soft_timer_id                           0x19010020
#define sl_bt_rsp_system_set_lazy_soft_timer_id                      0x1a010020

/**
 * @brief System boot mode
 */
typedef enum
{
  system_boot_mode_normal   = 0x0, /**< (0x0) Boot to normal mode */
  system_boot_mode_uart_dfu = 0x1, /**< (0x1) Boot to UART DFU mode */
  system_boot_mode_ota_dfu  = 0x2  /**< (0x2) Boot to OTA DFU mode */
} system_boot_mode_t;


/**
 * @brief Link Layer Configuration Keys
 */
typedef enum
{
  system_linklayer_config_key_halt               = 0x1, /**< (0x1) Same as
                                                             system_halt
                                                             command, value-0
                                                             Stop Radio 1- Start
                                                             Radio */
  system_linklayer_config_key_priority_range     = 0x2, /**< (0x2) Sets the RAIL
                                                             priority_mapping
                                                             offset field of the
                                                             link layer priority
                                                             configuration
                                                             structure to the
                                                             first byte of the
                                                             value field. */
  system_linklayer_config_key_scan_channels      = 0x3, /**< (0x3) Sets channels
                                                             to scan on. The
                                                             first byte of the
                                                             value is the
                                                             channel map. 0x1 =
                                                             Channel 37, 0x2 =
                                                             Channel 38, 0x4 =
                                                             Channel 39 */
  system_linklayer_config_key_set_flags          = 0x4, /**< (0x4) Sets the link
                                                             layer configuration
                                                             flags. The value is
                                                             a little endian
                                                             32-bit integer.
                                                             Flag Values:
                                                               - 0x00000001 -
                                                                 Disable Feature
                                                                 Exchange when
                                                                 slave
                                                               - 0x00000002 -
                                                                 Disable Feature
                                                                 Exchange when
                                                                 master */
  system_linklayer_config_key_clr_flags          = 0x5, /**< (0x5) Value is
                                                             flags to clear.
                                                             Flags are the same
                                                             as in SET_FLAGS
                                                             command. */
  system_linklayer_config_key_set_afh_interval   = 0x7, /**< (0x7) Set
                                                             afh_scan_interval
                                                             field of Link Layer
                                                             priority
                                                             configuration
                                                             structure. */
  system_linklayer_config_key_set_priority_table = 0x9  /**< (0x9) Value
                                                             contains priority
                                                             table to be copied
                                                             over the existing
                                                             table. If value is
                                                             smaller than full
                                                             table then only
                                                             those values are
                                                             updated. see @ref
                                                             sl_btbluetooth_ll_priorities
                                                             struct for the
                                                             definition of
                                                             priority table. */
} system_linklayer_config_key_t;



/**
 * @addtogroup sl_bt_evt_system_boot sl_bt_evt_system_boot
 * @{
 * @brief Indicates that the device has started and the radio is ready
 *
 * This event carries the firmware build number and other software and hardware
 * identification codes.
 */

/** @brief Identifier of the boot event */
#define sl_bt_evt_system_boot_id                                     0x000100a0

/***************************************************************************//**
 * @brief Data structure of the boot event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_system_boot_s
{
  uint16_t major;      /**< Major release version */
  uint16_t minor;      /**< Minor release version */
  uint16_t patch;      /**< Patch release number */
  uint16_t build;      /**< Build number */
  uint32_t bootloader; /**< Bootloader version */
  uint16_t hw;         /**< Hardware type */
  uint32_t hash;       /**< Version hash */
});

typedef struct sl_bt_evt_system_boot_s sl_bt_evt_system_boot_t;

/** @} */ // end addtogroup sl_bt_evt_system_boot

/**
 * @addtogroup sl_bt_evt_system_error sl_bt_evt_system_error
 * @{
 * @brief Indicates that an error has occurred
 *
 * See error codes table for more information.
 */

/** @brief Identifier of the error event */
#define sl_bt_evt_system_error_id                                    0x060100a0

/***************************************************************************//**
 * @brief Data structure of the error event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_system_error_s
{
  uint16_t   reason; /**< Result code
                            - <b>0:</b> success
                            - <b>Non-zero:</b> an error has occurred */
  uint8array data;   /**< Data related to the error; this field can be empty. */
});

typedef struct sl_bt_evt_system_error_s sl_bt_evt_system_error_t;

/** @} */ // end addtogroup sl_bt_evt_system_error

/**
 * @addtogroup sl_bt_evt_system_hardware_error sl_bt_evt_system_hardware_error
 * @{
 * @brief Indicates that a hardware-related error has occurred.
 */

/** @brief Identifier of the hardware_error event */
#define sl_bt_evt_system_hardware_error_id                           0x050100a0

/***************************************************************************//**
 * @brief Data structure of the hardware_error event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_system_hardware_error_s
{
  uint16_t status; /**< Result code
                          - <b>0:</b> success
                          - <b>Non-zero:</b> an error has occurred */
});

typedef struct sl_bt_evt_system_hardware_error_s sl_bt_evt_system_hardware_error_t;

/** @} */ // end addtogroup sl_bt_evt_system_hardware_error

/**
 * @addtogroup sl_bt_evt_system_external_signal sl_bt_evt_system_external_signal
 * @{
 * @brief Indicates that the external signals have been received
 *
 * External signals are generated from the native application.
 */

/** @brief Identifier of the external_signal event */
#define sl_bt_evt_system_external_signal_id                          0x030100a0

/***************************************************************************//**
 * @brief Data structure of the external_signal event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_system_external_signal_s
{
  uint32_t extsignals; /**< Bitmask of external signals received since last
                            event. */
});

typedef struct sl_bt_evt_system_external_signal_s sl_bt_evt_system_external_signal_t;

/** @} */ // end addtogroup sl_bt_evt_system_external_signal

/**
 * @addtogroup sl_bt_evt_system_awake sl_bt_evt_system_awake
 * @{
 * @brief Indicates that the device is awake and no longer in sleep mode
 *
 * <b>NOTE:</b> Stack does not generate this event by itself because sleep and
 * wakeup are managed by applications. If this event is needed, call function
 * @ref sl_bt_send_system_awake, which signals the stack to send this event.
 */

/** @brief Identifier of the awake event */
#define sl_bt_evt_system_awake_id                                    0x040100a0



/** @} */ // end addtogroup sl_bt_evt_system_awake

/**
 * @addtogroup sl_bt_evt_system_soft_timer sl_bt_evt_system_soft_timer
 * @{
 * @brief Indicates that a soft timer has lapsed.
 */

/** @brief Identifier of the soft_timer event */
#define sl_bt_evt_system_soft_timer_id                               0x070100a0

/***************************************************************************//**
 * @brief Data structure of the soft_timer event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_system_soft_timer_s
{
  uint8_t handle; /**< Timer Handle */
});

typedef struct sl_bt_evt_system_soft_timer_s sl_bt_evt_system_soft_timer_t;

/** @} */ // end addtogroup sl_bt_evt_system_soft_timer

/** @} */ // end addtogroup sl_bt_system

/**
 * @addtogroup sl_bt_gap GAP
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_gap_set_privacy_mode_id                            0x01020020
#define sl_bt_cmd_gap_set_data_channel_classification_id             0x02020020
#define sl_bt_cmd_gap_enable_whitelisting_id                         0x03020020
#define sl_bt_rsp_gap_set_privacy_mode_id                            0x01020020
#define sl_bt_rsp_gap_set_data_channel_classification_id             0x02020020
#define sl_bt_rsp_gap_enable_whitelisting_id                         0x03020020

/**
 * @brief Device Address Types
 */
typedef enum
{
  gap_public_address               = 0x0, /**< (0x0) Public device address */
  gap_static_address               = 0x1, /**< (0x1) Static device address */
  gap_random_resolvable_address    = 0x2, /**< (0x2) Private resolvable random
                                               address */
  gap_random_nonresolvable_address = 0x3  /**< (0x3) Private non-resolvable
                                               random address */
} gap_address_type_t;


/**
 * @brief PHY Types in gap Class
 */
typedef enum
{
  gap_1m_phy    = 0x1, /**< (0x1) 1M PHY */
  gap_2m_phy    = 0x2, /**< (0x2) 2M PHY */
  gap_coded_phy = 0x4  /**< (0x4) Coded PHY */
} gap_phy_type_t;



/** @} */ // end addtogroup sl_bt_gap

/**
 * @addtogroup sl_bt_advertiser Advertiser
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_advertiser_create_set_id                           0x01040020
#define sl_bt_cmd_advertiser_set_timing_id                           0x03040020
#define sl_bt_cmd_advertiser_set_phy_id                              0x06040020
#define sl_bt_cmd_advertiser_set_channel_map_id                      0x04040020
#define sl_bt_cmd_advertiser_set_tx_power_id                         0x0b040020
#define sl_bt_cmd_advertiser_set_report_scan_request_id              0x05040020
#define sl_bt_cmd_advertiser_set_random_address_id                   0x10040020
#define sl_bt_cmd_advertiser_clear_random_address_id                 0x11040020
#define sl_bt_cmd_advertiser_set_configuration_id                    0x07040020
#define sl_bt_cmd_advertiser_clear_configuration_id                  0x08040020
#define sl_bt_cmd_advertiser_set_data_id                             0x0f040020
#define sl_bt_cmd_advertiser_set_long_data_id                        0x0e040020
#define sl_bt_cmd_advertiser_start_id                                0x09040020
#define sl_bt_cmd_advertiser_stop_id                                 0x0a040020
#define sl_bt_cmd_advertiser_start_periodic_advertising_id           0x0c040020
#define sl_bt_cmd_advertiser_stop_periodic_advertising_id            0x0d040020
#define sl_bt_cmd_advertiser_delete_set_id                           0x02040020
#define sl_bt_rsp_advertiser_create_set_id                           0x01040020
#define sl_bt_rsp_advertiser_set_timing_id                           0x03040020
#define sl_bt_rsp_advertiser_set_phy_id                              0x06040020
#define sl_bt_rsp_advertiser_set_channel_map_id                      0x04040020
#define sl_bt_rsp_advertiser_set_tx_power_id                         0x0b040020
#define sl_bt_rsp_advertiser_set_report_scan_request_id              0x05040020
#define sl_bt_rsp_advertiser_set_random_address_id                   0x10040020
#define sl_bt_rsp_advertiser_clear_random_address_id                 0x11040020
#define sl_bt_rsp_advertiser_set_configuration_id                    0x07040020
#define sl_bt_rsp_advertiser_clear_configuration_id                  0x08040020
#define sl_bt_rsp_advertiser_set_data_id                             0x0f040020
#define sl_bt_rsp_advertiser_set_long_data_id                        0x0e040020
#define sl_bt_rsp_advertiser_start_id                                0x09040020
#define sl_bt_rsp_advertiser_stop_id                                 0x0a040020
#define sl_bt_rsp_advertiser_start_periodic_advertising_id           0x0c040020
#define sl_bt_rsp_advertiser_stop_periodic_advertising_id            0x0d040020
#define sl_bt_rsp_advertiser_delete_set_id                           0x02040020

/**
 * @brief Advertiser Connectable Mode
 */
typedef enum
{
  advertiser_non_connectable           = 0x0, /**< (0x0) Non-connectable
                                                   non-scannable. */
  advertiser_directed_connectable      = 0x1, /**< (0x1) Directed connectable
                                                   <b>(RESERVED, DO NOT USE)</b> */
  advertiser_connectable_scannable     = 0x2, /**< (0x2) Undirected connectable
                                                   scannable. This mode can only
                                                   be used in legacy advertising
                                                   PDUs. */
  advertiser_scannable_non_connectable = 0x3, /**< (0x3) Undirected scannable
                                                   (Non-connectable but responds
                                                   to scan requests) */
  advertiser_connectable_non_scannable = 0x4  /**< (0x4) Undirected connectable
                                                   non-scannable. This mode can
                                                   only be used in extended
                                                   advertising PDUs. */
} advertiser_connectable_mode_t;


/**
 * @brief Advertiser Discoverable Mode
 */
typedef enum
{
  advertiser_non_discoverable     = 0x0, /**< (0x0) Not discoverable */
  advertiser_limited_discoverable = 0x1, /**< (0x1) Discoverable using both
                                              limited and general discovery
                                              procedures */
  advertiser_general_discoverable = 0x2, /**< (0x2) Discoverable using general
                                              discovery procedure */
  advertiser_broadcast            = 0x3, /**< (0x3) Device is not discoverable
                                              in either limited or generic
                                              discovery procedure but may be
                                              discovered using the Observation
                                              procedure. */
  advertiser_user_data            = 0x4  /**< (0x4) Send advertising and/or scan
                                              response data defined by the user.
                                              The limited/general discoverable
                                              flags are defined by the user. */
} advertiser_discoverable_mode_t;


/**
 * @brief Advertising Address Type
 */
typedef enum
{
  advertiser_identity_address = 0x0, /**< (0x0) Use public or static device
                                          address, or an identity address if
                                          privacy mode is enabled. */
  advertiser_non_resolvable   = 0x1  /**< (0x1) Use non resolvable address type;
                                          advertising mode must also be
                                          non-connectable. */
} advertiser_adv_address_type_t;



/**
 * @addtogroup sl_bt_evt_advertiser_timeout sl_bt_evt_advertiser_timeout
 * @{
 * @brief Indicates that the advertiser has completed the configured number of
 * advertising events in the advertising set and advertising has stopped
 *
 * The maximum number of advertising events can be configured by the maxevents
 * parameter in the command @ref sl_bt_advertiser_set_timing.
 */

/** @brief Identifier of the timeout event */
#define sl_bt_evt_advertiser_timeout_id                              0x010400a0

/***************************************************************************//**
 * @brief Data structure of the timeout event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_advertiser_timeout_s
{
  uint8_t handle; /**< The advertising set handle */
});

typedef struct sl_bt_evt_advertiser_timeout_s sl_bt_evt_advertiser_timeout_t;

/** @} */ // end addtogroup sl_bt_evt_advertiser_timeout

/**
 * @addtogroup sl_bt_evt_advertiser_scan_request sl_bt_evt_advertiser_scan_request
 * @{
 * @brief Reports any scan request received in advertising mode if the scan
 * request notification is enabled
 *
 * Do not confuse this event with the scan response.
 */

/** @brief Identifier of the scan_request event */
#define sl_bt_evt_advertiser_scan_request_id                         0x020400a0

/***************************************************************************//**
 * @brief Data structure of the scan_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_advertiser_scan_request_s
{
  uint8_t handle;       /**< Advertising set handle where scan request was
                             received */
  bd_addr address;      /**< Bluetooth address of the scanning device */
  uint8_t address_type; /**< Scanner address type. Values:
                               - <b>0:</b> Public address
                               - <b>1:</b> Random address */
  uint8_t bonding;      /**< Bonding handle if the remote scanning device has
                             previously bonded with the local device. Values:
                               - <b>SL_BT_INVALID_BONDING_HANDLE (0xff):</b> No
                                 bonding
                               - <b>Other:</b> Bonding handle */
});

typedef struct sl_bt_evt_advertiser_scan_request_s sl_bt_evt_advertiser_scan_request_t;

/** @} */ // end addtogroup sl_bt_evt_advertiser_scan_request

/**
 * @cond RESTRICTED
 * @addtogroup sl_bt_evt_advertiser_periodic_advertising_status sl_bt_evt_advertiser_periodic_advertising_status
 * @{
 * @brief This event indicates a status update in the periodic advertising.
 */

/** @brief Identifier of the periodic_advertising_status event */
#define sl_bt_evt_advertiser_periodic_advertising_status_id          0x030400a0

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Data structure of the periodic_advertising_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_advertiser_periodic_advertising_status_s
{
  uint8_t  sid;    /**< The advertising set handle */
  uint32_t status; /**< Reserved for future */
});

typedef struct sl_bt_evt_advertiser_periodic_advertising_status_s sl_bt_evt_advertiser_periodic_advertising_status_t;
/** @endcond */ // end restricted struct

/** @} @endcond */ // end addtogroup sl_bt_evt_advertiser_periodic_advertising_status

/** @} */ // end addtogroup sl_bt_advertiser

/**
 * @addtogroup sl_bt_scanner Scanner
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_scanner_set_timing_id                              0x01050020
#define sl_bt_cmd_scanner_set_mode_id                                0x02050020
#define sl_bt_cmd_scanner_start_id                                   0x03050020
#define sl_bt_cmd_scanner_stop_id                                    0x05050020
#define sl_bt_rsp_scanner_set_timing_id                              0x01050020
#define sl_bt_rsp_scanner_set_mode_id                                0x02050020
#define sl_bt_rsp_scanner_start_id                                   0x03050020
#define sl_bt_rsp_scanner_stop_id                                    0x05050020

/**
 * @brief Scanner Discovery Mode
 */
typedef enum
{
  scanner_discover_limited     = 0x0, /**< (0x0) Discover only limited
                                           discoverable devices. */
  scanner_discover_generic     = 0x1, /**< (0x1) Discover limited and generic
                                           discoverable devices. */
  scanner_discover_observation = 0x2  /**< (0x2) Discover all devices. */
} scanner_discover_mode_t;



/**
 * @addtogroup sl_bt_evt_scanner_scan_report sl_bt_evt_scanner_scan_report
 * @{
 * @brief Reports an advertising or scan response packet that is received by the
 * device's radio while in scanning mode
 */

/** @brief Identifier of the scan_report event */
#define sl_bt_evt_scanner_scan_report_id                             0x010500a0

/***************************************************************************//**
 * @brief Data structure of the scan_report event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_scanner_scan_report_s
{
  uint8_t    packet_type;       /**< <b>Bits 0..2</b> : advertising packet type
                                       - <b>000</b> : Connectable scannable
                                         undirected advertising
                                       - <b>001</b> : Connectable undirected
                                         advertising
                                       - <b>010</b> : Scannable undirected
                                         advertising
                                       - <b>011</b> : Non-connectable
                                         non-scannable undirected advertising
                                       - <b>100</b> : Scan Response. Note that
                                         this is received only if the device is
                                         in active scan mode.

                                     <b>Bits 3..4</b> : Reserved for future

                                     <b>Bits 5..6</b> : data completeness
                                       - <b>00:</b> Complete
                                       - <b>01:</b> Incomplete, more data to
                                         come in new events
                                       - <b>10:</b> Incomplete, data truncated,
                                         no more to come

                                     <b>Bit 7</b> : legacy or extended
                                     advertising
                                       - <b>0:</b> Legacy advertising PDUs used
                                       - <b>1:</b> Extended advertising PDUs
                                         used */
  bd_addr    address;           /**< Bluetooth address of the remote device */
  uint8_t    address_type;      /**< Advertiser address type. Values:
                                       - <b>0:</b> Public address
                                       - <b>1:</b> Random address
                                       - <b>255:</b> No address provided
                                         (anonymous advertising) */
  uint8_t    bonding;           /**< Bonding handle if the remote advertising
                                     device has previously bonded with the local
                                     device. Values:
                                       - <b>SL_BT_INVALID_BONDING_HANDLE
                                         (0xff):</b> No bonding
                                       - <b>Other:</b> Bonding handle */
  uint8_t    primary_phy;       /**< Enum @ref gap_phy_type_t. The PHY on which
                                     advertising packets are transmitted on the
                                     primary advertising channel. Values:
                                       - <b>gap_1m_phy (0x1):</b> 1M PHY
                                       - <b>gap_coded_phy (0x4):</b> Coded PHY */
  uint8_t    secondary_phy;     /**< Enum @ref gap_phy_type_t. The PHY on which
                                     advertising packets are transmitted on the
                                     secondary advertising channel. Values:
                                       - <b>gap_1m_phy (0x1):</b> 1M PHY
                                       - <b>gap_2m_phy (0x2):</b> 2M PHY
                                       - <b>gap_coded_phy (0x4):</b> Coded PHY */
  uint8_t    adv_sid;           /**< Advertising set identifier */
  int8_t     tx_power;          /**< TX power value in the received packet
                                     header. Units: dBm
                                       - Valid value range: -127 to 126
                                       - Value 127: information unavailable */
  int8_t     rssi;              /**< Signal strength indicator (RSSI) in the
                                     last received packet. Units: dBm
                                       - Range: -127 to +20 */
  uint8_t    channel;           /**< The channel number on which the last packet
                                     was received */
  uint16_t   periodic_interval; /**< The periodic advertising interval. Value 0
                                     indicates no periodic advertising.
                                     Otherwise,
                                       - Range: 0x06 to 0xFFFF
                                       - Unit: 1.25 ms
                                       - Time range: 7.5 ms to 81.92 s */
  uint8array data;              /**< Advertising or scan response data */
});

typedef struct sl_bt_evt_scanner_scan_report_s sl_bt_evt_scanner_scan_report_t;

/** @} */ // end addtogroup sl_bt_evt_scanner_scan_report

/** @} */ // end addtogroup sl_bt_scanner

/**
 * @addtogroup sl_bt_sync Synchronization
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_sync_set_parameters_id                             0x02420020
#define sl_bt_cmd_sync_open_id                                       0x00420020
#define sl_bt_cmd_sync_close_id                                      0x01420020
#define sl_bt_rsp_sync_set_parameters_id                             0x02420020
#define sl_bt_rsp_sync_open_id                                       0x00420020
#define sl_bt_rsp_sync_close_id                                      0x01420020

/**
 * @brief Advertiser Clock Accuracy
 */
typedef enum
{
  sync_clock_accuracy_500 = 0x500, /**< (0x500) Clock accuracy 500 ppm */
  sync_clock_accuracy_250 = 0x250, /**< (0x250) Clock accuracy 250 ppm */
  sync_clock_accuracy_150 = 0x150, /**< (0x150) Clock accuracy 150 ppm */
  sync_clock_accuracy_100 = 0x100, /**< (0x100) Clock accuracy 100 ppm */
  sync_clock_accuracy_75  = 0x75,  /**< (0x75) Clock accuracy 75 ppm */
  sync_clock_accuracy_50  = 0x50,  /**< (0x50) Clock accuracy 50 ppm */
  sync_clock_accuracy_30  = 0x30,  /**< (0x30) Clock accuracy 30 ppm */
  sync_clock_accuracy_20  = 0x20   /**< (0x20) Clock accuracy 20 ppm */
} sync_advertiser_clock_accuracy_t;



/**
 * @addtogroup sl_bt_evt_sync_opened sl_bt_evt_sync_opened
 * @{
 * @brief Indicates that a periodic advertising synchronization has been opened.
 */

/** @brief Identifier of the opened event */
#define sl_bt_evt_sync_opened_id                                     0x004200a0

/***************************************************************************//**
 * @brief Data structure of the opened event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sync_opened_s
{
  uint16_t sync;           /**< Periodic advertising synchronization handle */
  uint8_t  adv_sid;        /**< Advertising set identifier */
  bd_addr  address;        /**< Address of the advertiser */
  uint8_t  address_type;   /**< Advertiser address type. Values:
                                  - <b>0:</b> Public address
                                  - <b>1:</b> Random address */
  uint8_t  adv_phy;        /**< Enum @ref gap_phy_type_t. The advertiser PHY.
                                Values:
                                  - <b>gap_1m_phy (0x1):</b> 1M PHY
                                  - <b>gap_2m_phy (0x2):</b> 2M PHY
                                  - <b>gap_coded_phy (0x4):</b> Coded PHY */
  uint16_t adv_interval;   /**< The periodic advertising interval. Value in
                                units of 1.25 ms
                                  - Range: 0x06 to 0xFFFF
                                  - Time range: 7.5 ms to 81.92 s */
  uint16_t clock_accuracy; /**< Enum @ref sync_advertiser_clock_accuracy_t. The
                                advertiser clock accuracy. */
  uint8_t  bonding;        /**< Bonding handle. Values:
                                  - <b>SL_BT_INVALID_BONDING_HANDLE (0xff):</b>
                                    No bonding
                                  - <b>Other:</b> Bonding handle */
});

typedef struct sl_bt_evt_sync_opened_s sl_bt_evt_sync_opened_t;

/** @} */ // end addtogroup sl_bt_evt_sync_opened

/**
 * @addtogroup sl_bt_evt_sync_data sl_bt_evt_sync_data
 * @{
 * @brief Reports a received periodic advertisement packet
 */

/** @brief Identifier of the data event */
#define sl_bt_evt_sync_data_id                                       0x024200a0

/***************************************************************************//**
 * @brief Data structure of the data event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sync_data_s
{
  uint16_t   sync;        /**< Periodic advertising synchronization handle */
  int8_t     tx_power;    /**< TX power value in the received packet header.
                               Units: dBm
                                 - Valid value range: -127 to 126
                                 - Value 127: information unavailable */
  int8_t     rssi;        /**< Signal strength indicator (RSSI) in the latest
                               received packet. Units: dBm
                                 - Range: -127 to +20 */
  uint8_t    data_status; /**< Data completeness:
                                 - <b>0:</b> Complete
                                 - <b>1:</b> Incomplete, more data to come in
                                   new events
                                 - <b>2:</b> Incomplete, data truncated, no more
                                   to come */
  uint8array data;        /**< Periodic advertising data */
});

typedef struct sl_bt_evt_sync_data_s sl_bt_evt_sync_data_t;

/** @} */ // end addtogroup sl_bt_evt_sync_data

/**
 * @addtogroup sl_bt_evt_sync_closed sl_bt_evt_sync_closed
 * @{
 * @brief Indicates that periodic advertising synchronization was lost or a
 * synchronization establishment procedure was canceled
 *
 * The synchronization establishment procedure can be cancelled explicitly by
 * the application by issuing command @ref sl_bt_sync_close, or internally due
 * to synchronization failing. Synchronization can fail for example due to
 * incompatible sync CTE type.
 */

/** @brief Identifier of the closed event */
#define sl_bt_evt_sync_closed_id                                     0x014200a0

/***************************************************************************//**
 * @brief Data structure of the closed event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sync_closed_s
{
  uint16_t reason; /**< Result code
                          - <b>0:</b> success
                          - <b>Non-zero:</b> an error has occurred */
  uint16_t sync;   /**< Periodic advertising synchronization handle */
});

typedef struct sl_bt_evt_sync_closed_s sl_bt_evt_sync_closed_t;

/** @} */ // end addtogroup sl_bt_evt_sync_closed

/** @} */ // end addtogroup sl_bt_sync

/**
 * @addtogroup sl_bt_connection Connection
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_connection_set_default_parameters_id               0x00060020
#define sl_bt_cmd_connection_set_default_preferred_phy_id            0x01060020
#define sl_bt_cmd_connection_open_id                                 0x04060020
#define sl_bt_cmd_connection_set_parameters_id                       0x06060020
#define sl_bt_cmd_connection_set_preferred_phy_id                    0x08060020
#define sl_bt_cmd_connection_disable_slave_latency_id                0x03060020
#define sl_bt_cmd_connection_get_rssi_id                             0x02060020
#define sl_bt_cmd_connection_read_channel_map_id                     0x07060020
#define sl_bt_cmd_connection_close_id                                0x05060020
#define sl_bt_rsp_connection_set_default_parameters_id               0x00060020
#define sl_bt_rsp_connection_set_default_preferred_phy_id            0x01060020
#define sl_bt_rsp_connection_open_id                                 0x04060020
#define sl_bt_rsp_connection_set_parameters_id                       0x06060020
#define sl_bt_rsp_connection_set_preferred_phy_id                    0x08060020
#define sl_bt_rsp_connection_disable_slave_latency_id                0x03060020
#define sl_bt_rsp_connection_get_rssi_id                             0x02060020
#define sl_bt_rsp_connection_read_channel_map_id                     0x07060020
#define sl_bt_rsp_connection_close_id                                0x05060020

/**
 * @brief Security Modes
 */
typedef enum
{
  connection_mode1_level1 = 0x0, /**< (0x0) No security */
  connection_mode1_level2 = 0x1, /**< (0x1) Unauthenticated pairing with
                                      encryption */
  connection_mode1_level3 = 0x2, /**< (0x2) Authenticated pairing with
                                      encryption */
  connection_mode1_level4 = 0x3  /**< (0x3) Authenticated Secure Connections
                                      pairing with encryption using a 128-bit
                                      strength encryption key */
} connection_security_t;



/**
 * @addtogroup sl_bt_evt_connection_opened sl_bt_evt_connection_opened
 * @{
 * @brief Indicates that a new connection was opened
 *
 * This event does not indicate that the connection was established (i.e., that
 * a data packet was received within 6 connection interval). If the connection
 * does not get established, an @ref sl_bt_evt_connection_closed event may
 * immediately follow. This event also reports whether the connected devices are
 * already bonded, and what the role of the Bluetooth device (Slave or Master)
 * is. An open connection can be closed with @ref sl_bt_connection_close command
 * by giving the connection handle obtained from this event.
 */

/** @brief Identifier of the opened event */
#define sl_bt_evt_connection_opened_id                               0x000600a0

/***************************************************************************//**
 * @brief Data structure of the opened event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_connection_opened_s
{
  bd_addr address;      /**< Remote device address */
  uint8_t address_type; /**< Enum @ref gap_address_type_t. Remote device address
                             type. Values:
                               - <b>gap_public_address (0x0):</b> Public device
                                 address
                               - <b>gap_static_address (0x1):</b> Static device
                                 address
                               - <b>gap_random_resolvable_address (0x2):</b>
                                 Private resolvable random address
                               - <b>gap_random_nonresolvable_address (0x3):</b>
                                 Private non-resolvable random address */
  uint8_t master;       /**< Device role in connection. Values:
                               - <b>0:</b> Slave
                               - <b>1:</b> Master */
  uint8_t connection;   /**< Handle for new connection */
  uint8_t bonding;      /**< Bonding handle. Values:
                               - <b>SL_BT_INVALID_BONDING_HANDLE (0xff):</b> No
                                 bonding
                               - <b>Other:</b> Bonding handle */
  uint8_t advertiser;   /**< The local advertising set that this connection was
                             opened to. Values:
                               - <b>SL_BT_INVALID_ADVERTISING_SET_HANDLE
                                 (0xff):</b> Invalid value or not applicable.
                                 Ignore this field
                               - <b>Other:</b> The advertising set handle */
});

typedef struct sl_bt_evt_connection_opened_s sl_bt_evt_connection_opened_t;

/** @} */ // end addtogroup sl_bt_evt_connection_opened

/**
 * @addtogroup sl_bt_evt_connection_parameters sl_bt_evt_connection_parameters
 * @{
 * @brief Triggered whenever the connection parameters are changed and at any
 * time a connection is established
 */

/** @brief Identifier of the parameters event */
#define sl_bt_evt_connection_parameters_id                           0x020600a0

/***************************************************************************//**
 * @brief Data structure of the parameters event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_connection_parameters_s
{
  uint8_t  connection;    /**< Connection handle */
  uint16_t interval;      /**< Connection interval. Time = Value x 1.25 ms */
  uint16_t latency;       /**< Slave latency (how many connection intervals the
                               slave can skip) */
  uint16_t timeout;       /**< Supervision timeout. Time = Value x 10 ms */
  uint8_t  security_mode; /**< Enum @ref connection_security_t. Connection
                               security mode. Values:
                                 - <b>connection_mode1_level1 (0x0):</b> No
                                   security
                                 - <b>connection_mode1_level2 (0x1):</b>
                                   Unauthenticated pairing with encryption
                                 - <b>connection_mode1_level3 (0x2):</b>
                                   Authenticated pairing with encryption
                                 - <b>connection_mode1_level4 (0x3):</b>
                                   Authenticated Secure Connections pairing with
                                   encryption using a 128-bit strength
                                   encryption key */
  uint16_t txsize;        /**< Maximum Data Channel PDU Payload size that the
                               controller can send in an air packet */
});

typedef struct sl_bt_evt_connection_parameters_s sl_bt_evt_connection_parameters_t;

/** @} */ // end addtogroup sl_bt_evt_connection_parameters

/**
 * @addtogroup sl_bt_evt_connection_phy_status sl_bt_evt_connection_phy_status
 * @{
 * @brief Indicates that PHY update procedure is completed.
 */

/** @brief Identifier of the phy_status event */
#define sl_bt_evt_connection_phy_status_id                           0x040600a0

/***************************************************************************//**
 * @brief Data structure of the phy_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_connection_phy_status_s
{
  uint8_t connection; /**< Connection handle */
  uint8_t phy;        /**< Current active PHY. See values from @ref
                           sl_bt_connection_set_preferred_phy command. */
});

typedef struct sl_bt_evt_connection_phy_status_s sl_bt_evt_connection_phy_status_t;

/** @} */ // end addtogroup sl_bt_evt_connection_phy_status

/**
 * @addtogroup sl_bt_evt_connection_rssi sl_bt_evt_connection_rssi
 * @{
 * @brief Triggered when an connection_get_rssi command has completed.
 */

/** @brief Identifier of the rssi event */
#define sl_bt_evt_connection_rssi_id                                 0x030600a0

/***************************************************************************//**
 * @brief Data structure of the rssi event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_connection_rssi_s
{
  uint8_t connection; /**< Connection handle */
  uint8_t status;     /**< Command complete status:
                             - <b>0x00:</b> The command succeeded
                             - <b>0x01-0xFF:</b> The command failed. See
                               Bluetooth Core specification v5.0 [Vol 2] Part D,
                               Error Codes */
  int8_t  rssi;       /**< RSSI in the latest received packet of the connection.
                           Units: dBm. Range: -127 to +20. Ignore this parameter
                           if the command fails. */
});

typedef struct sl_bt_evt_connection_rssi_s sl_bt_evt_connection_rssi_t;

/** @} */ // end addtogroup sl_bt_evt_connection_rssi

/**
 * @addtogroup sl_bt_evt_connection_closed sl_bt_evt_connection_closed
 * @{
 * @brief Indicates that a connection was closed.
 */

/** @brief Identifier of the closed event */
#define sl_bt_evt_connection_closed_id                               0x010600a0

/***************************************************************************//**
 * @brief Data structure of the closed event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_connection_closed_s
{
  uint16_t reason;     /**< Result code
                              - <b>0:</b> success
                              - <b>Non-zero:</b> an error has occurred */
  uint8_t  connection; /**< Handle of the closed connection */
});

typedef struct sl_bt_evt_connection_closed_s sl_bt_evt_connection_closed_t;

/** @} */ // end addtogroup sl_bt_evt_connection_closed

/** @} */ // end addtogroup sl_bt_connection

/**
 * @addtogroup sl_bt_gatt GATT Client
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_gatt_set_max_mtu_id                                0x00090020
#define sl_bt_cmd_gatt_discover_primary_services_id                  0x01090020
#define sl_bt_cmd_gatt_discover_primary_services_by_uuid_id          0x02090020
#define sl_bt_cmd_gatt_find_included_services_id                     0x10090020
#define sl_bt_cmd_gatt_discover_characteristics_id                   0x03090020
#define sl_bt_cmd_gatt_discover_characteristics_by_uuid_id           0x04090020
#define sl_bt_cmd_gatt_discover_descriptors_id                       0x06090020
#define sl_bt_cmd_gatt_set_characteristic_notification_id            0x05090020
#define sl_bt_cmd_gatt_send_characteristic_confirmation_id           0x0d090020
#define sl_bt_cmd_gatt_read_characteristic_value_id                  0x07090020
#define sl_bt_cmd_gatt_read_characteristic_value_from_offset_id      0x12090020
#define sl_bt_cmd_gatt_read_multiple_characteristic_values_id        0x11090020
#define sl_bt_cmd_gatt_read_characteristic_value_by_uuid_id          0x08090020
#define sl_bt_cmd_gatt_write_characteristic_value_id                 0x09090020
#define sl_bt_cmd_gatt_write_characteristic_value_without_response_id 0x0a090020
#define sl_bt_cmd_gatt_prepare_characteristic_value_write_id         0x0b090020
#define sl_bt_cmd_gatt_prepare_characteristic_value_reliable_write_id 0x13090020
#define sl_bt_cmd_gatt_execute_characteristic_value_write_id         0x0c090020
#define sl_bt_cmd_gatt_read_descriptor_value_id                      0x0e090020
#define sl_bt_cmd_gatt_write_descriptor_value_id                     0x0f090020
#define sl_bt_rsp_gatt_set_max_mtu_id                                0x00090020
#define sl_bt_rsp_gatt_discover_primary_services_id                  0x01090020
#define sl_bt_rsp_gatt_discover_primary_services_by_uuid_id          0x02090020
#define sl_bt_rsp_gatt_find_included_services_id                     0x10090020
#define sl_bt_rsp_gatt_discover_characteristics_id                   0x03090020
#define sl_bt_rsp_gatt_discover_characteristics_by_uuid_id           0x04090020
#define sl_bt_rsp_gatt_discover_descriptors_id                       0x06090020
#define sl_bt_rsp_gatt_set_characteristic_notification_id            0x05090020
#define sl_bt_rsp_gatt_send_characteristic_confirmation_id           0x0d090020
#define sl_bt_rsp_gatt_read_characteristic_value_id                  0x07090020
#define sl_bt_rsp_gatt_read_characteristic_value_from_offset_id      0x12090020
#define sl_bt_rsp_gatt_read_multiple_characteristic_values_id        0x11090020
#define sl_bt_rsp_gatt_read_characteristic_value_by_uuid_id          0x08090020
#define sl_bt_rsp_gatt_write_characteristic_value_id                 0x09090020
#define sl_bt_rsp_gatt_write_characteristic_value_without_response_id 0x0a090020
#define sl_bt_rsp_gatt_prepare_characteristic_value_write_id         0x0b090020
#define sl_bt_rsp_gatt_prepare_characteristic_value_reliable_write_id 0x13090020
#define sl_bt_rsp_gatt_execute_characteristic_value_write_id         0x0c090020
#define sl_bt_rsp_gatt_read_descriptor_value_id                      0x0e090020
#define sl_bt_rsp_gatt_write_descriptor_value_id                     0x0f090020

/**
 * @brief Attribute Protocol Opcode
 */
typedef enum
{
  gatt_read_by_type_request      = 0x8,  /**< (0x8) Read by type request */
  gatt_read_by_type_response     = 0x9,  /**< (0x9) Read by type response */
  gatt_read_request              = 0xa,  /**< (0xa) Read request */
  gatt_read_response             = 0xb,  /**< (0xb) Read response */
  gatt_read_blob_request         = 0xc,  /**< (0xc) Read blob request */
  gatt_read_blob_response        = 0xd,  /**< (0xd) Read blob response */
  gatt_read_multiple_request     = 0xe,  /**< (0xe) Read multiple request */
  gatt_read_multiple_response    = 0xf,  /**< (0xf) Read multiple response */
  gatt_write_request             = 0x12, /**< (0x12) Write request */
  gatt_write_response            = 0x13, /**< (0x13) Write response */
  gatt_write_command             = 0x52, /**< (0x52) Write command */
  gatt_prepare_write_request     = 0x16, /**< (0x16) Prepare write request */
  gatt_prepare_write_response    = 0x17, /**< (0x17) Prepare write response */
  gatt_execute_write_request     = 0x18, /**< (0x18) Execute write request */
  gatt_execute_write_response    = 0x19, /**< (0x19) Execute write response */
  gatt_handle_value_notification = 0x1b, /**< (0x1b) Notification */
  gatt_handle_value_indication   = 0x1d  /**< (0x1d) Indication */
} gatt_att_opcode_t;


/**
 * @brief Characteristic Client Configuration Flag
 */
typedef enum
{
  gatt_disable      = 0x0, /**< (0x0) Disable notifications and indications */
  gatt_notification = 0x1, /**< (0x1) Notification */
  gatt_indication   = 0x2  /**< (0x2) Indication */
} gatt_client_config_flag_t;


/**
 * @brief Execute Write Flag
 */
typedef enum
{
  gatt_cancel = 0x0, /**< (0x0) Cancel all queued writes */
  gatt_commit = 0x1  /**< (0x1) Commit all queued writes */
} gatt_execute_write_flag_t;



/**
 * @addtogroup sl_bt_evt_gatt_mtu_exchanged sl_bt_evt_gatt_mtu_exchanged
 * @{
 * @brief Indicates that an ATT_MTU exchange procedure is completed
 *
 * The @p mtu parameter describes new MTU size. MTU size 23 is used before this
 * event is received.
 */

/** @brief Identifier of the mtu_exchanged event */
#define sl_bt_evt_gatt_mtu_exchanged_id                              0x000900a0

/***************************************************************************//**
 * @brief Data structure of the mtu_exchanged event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_mtu_exchanged_s
{
  uint8_t  connection; /**< Connection handle */
  uint16_t mtu;        /**< Exchanged ATT_MTU */
});

typedef struct sl_bt_evt_gatt_mtu_exchanged_s sl_bt_evt_gatt_mtu_exchanged_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_mtu_exchanged

/**
 * @addtogroup sl_bt_evt_gatt_service sl_bt_evt_gatt_service
 * @{
 * @brief Indicate that a GATT service in the remote GATT database was
 * discovered
 *
 * This event is generated after issuing either the @ref
 * sl_bt_gatt_discover_primary_services or @ref
 * sl_bt_gatt_discover_primary_services_by_uuid command.
 */

/** @brief Identifier of the service event */
#define sl_bt_evt_gatt_service_id                                    0x010900a0

/***************************************************************************//**
 * @brief Data structure of the service event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_service_s
{
  uint8_t    connection; /**< Connection handle */
  uint32_t   service;    /**< GATT service handle */
  uint8array uuid;       /**< Service UUID in little endian format */
});

typedef struct sl_bt_evt_gatt_service_s sl_bt_evt_gatt_service_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_service

/**
 * @addtogroup sl_bt_evt_gatt_characteristic sl_bt_evt_gatt_characteristic
 * @{
 * @brief Indicates that a GATT characteristic in the remote GATT database was
 * discovered
 *
 * This event is generated after issuing either the @ref
 * sl_bt_gatt_discover_characteristics or @ref
 * sl_bt_gatt_discover_characteristics_by_uuid command.
 */

/** @brief Identifier of the characteristic event */
#define sl_bt_evt_gatt_characteristic_id                             0x020900a0

/***************************************************************************//**
 * @brief Data structure of the characteristic event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_characteristic_s
{
  uint8_t    connection;     /**< Connection handle */
  uint16_t   characteristic; /**< GATT characteristic handle */
  uint8_t    properties;     /**< Characteristic properties */
  uint8array uuid;           /**< Characteristic UUID in little endian format */
});

typedef struct sl_bt_evt_gatt_characteristic_s sl_bt_evt_gatt_characteristic_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_characteristic

/**
 * @addtogroup sl_bt_evt_gatt_descriptor sl_bt_evt_gatt_descriptor
 * @{
 * @brief Indicates that a GATT characteristic descriptor in the remote GATT
 * database was discovered
 *
 * It is generated after issuing the @ref sl_bt_gatt_discover_descriptors
 * command.
 */

/** @brief Identifier of the descriptor event */
#define sl_bt_evt_gatt_descriptor_id                                 0x030900a0

/***************************************************************************//**
 * @brief Data structure of the descriptor event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_descriptor_s
{
  uint8_t    connection; /**< Connection handle */
  uint16_t   descriptor; /**< GATT characteristic descriptor handle */
  uint8array uuid;       /**< Descriptor UUID in little endian format */
});

typedef struct sl_bt_evt_gatt_descriptor_s sl_bt_evt_gatt_descriptor_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_descriptor

/**
 * @addtogroup sl_bt_evt_gatt_characteristic_value sl_bt_evt_gatt_characteristic_value
 * @{
 * @brief Indicates that the value of one or several characteristics in the
 * remote GATT server was received
 *
 * It is triggered by several commands: @ref
 * sl_bt_gatt_read_characteristic_value, @ref
 * sl_bt_gatt_read_characteristic_value_from_offset, @ref
 * sl_bt_gatt_read_characteristic_value_by_uuid, @ref
 * sl_bt_gatt_read_multiple_characteristic_values; and when the remote GATT
 * server sends indications or notifications after enabling notifications with
 * @ref sl_bt_gatt_set_characteristic_notification. The parameter @p att_opcode
 * indicates which type of GATT transaction triggered this event. In particular,
 * if the @p att_opcode type is @ref gatt_handle_value_indication (0x1d), the
 * application needs to confirm the indication with @ref
 * sl_bt_gatt_send_characteristic_confirmation.
 */

/** @brief Identifier of the characteristic_value event */
#define sl_bt_evt_gatt_characteristic_value_id                       0x040900a0

/***************************************************************************//**
 * @brief Data structure of the characteristic_value event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_characteristic_value_s
{
  uint8_t    connection;     /**< Connection handle */
  uint16_t   characteristic; /**< GATT characteristic handle. This value is
                                  normally received from the gatt_characteristic
                                  event. */
  uint8_t    att_opcode;     /**< Enum @ref gatt_att_opcode_t. Attribute opcode,
                                  which indicates the GATT transaction used. */
  uint16_t   offset;         /**< Value offset */
  uint8array value;          /**< Characteristic value */
});

typedef struct sl_bt_evt_gatt_characteristic_value_s sl_bt_evt_gatt_characteristic_value_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_characteristic_value

/**
 * @addtogroup sl_bt_evt_gatt_descriptor_value sl_bt_evt_gatt_descriptor_value
 * @{
 * @brief Indicates that the value of a descriptor in the remote GATT server was
 * received
 *
 * This event is generated by the @ref sl_bt_gatt_read_descriptor_value command.
 */

/** @brief Identifier of the descriptor_value event */
#define sl_bt_evt_gatt_descriptor_value_id                           0x050900a0

/***************************************************************************//**
 * @brief Data structure of the descriptor_value event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_descriptor_value_s
{
  uint8_t    connection; /**< Connection handle */
  uint16_t   descriptor; /**< GATT characteristic descriptor handle */
  uint16_t   offset;     /**< Value offset */
  uint8array value;      /**< Descriptor value */
});

typedef struct sl_bt_evt_gatt_descriptor_value_s sl_bt_evt_gatt_descriptor_value_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_descriptor_value

/**
 * @addtogroup sl_bt_evt_gatt_procedure_completed sl_bt_evt_gatt_procedure_completed
 * @{
 * @brief Indicates that the current GATT procedure was completed successfully
 * or that it failed with an error
 *
 * All GATT commands excluding @ref
 * sl_bt_gatt_write_characteristic_value_without_response and @ref
 * sl_bt_gatt_send_characteristic_confirmation will trigger this event. As a
 * result, the application must wait for this event before issuing another GATT
 * command (excluding the two aforementioned exceptions).
 */

/** @brief Identifier of the procedure_completed event */
#define sl_bt_evt_gatt_procedure_completed_id                        0x060900a0

/***************************************************************************//**
 * @brief Data structure of the procedure_completed event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_procedure_completed_s
{
  uint8_t  connection; /**< Connection handle */
  uint16_t result;     /**< Result code
                              - <b>0:</b> success
                              - <b>Non-zero:</b> an error has occurred */
});

typedef struct sl_bt_evt_gatt_procedure_completed_s sl_bt_evt_gatt_procedure_completed_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_procedure_completed

/** @} */ // end addtogroup sl_bt_gatt

/**
 * @addtogroup sl_bt_gatt_server GATT Server
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_gatt_server_set_capabilities_id                    0x080a0020
#define sl_bt_cmd_gatt_server_enable_capabilities_id                 0x0c0a0020
#define sl_bt_cmd_gatt_server_disable_capabilities_id                0x0d0a0020
#define sl_bt_cmd_gatt_server_get_enabled_capabilities_id            0x0e0a0020
#define sl_bt_cmd_gatt_server_set_max_mtu_id                         0x0a0a0020
#define sl_bt_cmd_gatt_server_get_mtu_id                             0x0b0a0020
#define sl_bt_cmd_gatt_server_find_attribute_id                      0x060a0020
#define sl_bt_cmd_gatt_server_read_attribute_value_id                0x000a0020
#define sl_bt_cmd_gatt_server_read_attribute_type_id                 0x010a0020
#define sl_bt_cmd_gatt_server_write_attribute_value_id               0x020a0020
#define sl_bt_cmd_gatt_server_send_user_read_response_id             0x030a0020
#define sl_bt_cmd_gatt_server_send_user_write_response_id            0x040a0020
#define sl_bt_cmd_gatt_server_send_characteristic_notification_id    0x050a0020
#define sl_bt_rsp_gatt_server_set_capabilities_id                    0x080a0020
#define sl_bt_rsp_gatt_server_enable_capabilities_id                 0x0c0a0020
#define sl_bt_rsp_gatt_server_disable_capabilities_id                0x0d0a0020
#define sl_bt_rsp_gatt_server_get_enabled_capabilities_id            0x0e0a0020
#define sl_bt_rsp_gatt_server_set_max_mtu_id                         0x0a0a0020
#define sl_bt_rsp_gatt_server_get_mtu_id                             0x0b0a0020
#define sl_bt_rsp_gatt_server_find_attribute_id                      0x060a0020
#define sl_bt_rsp_gatt_server_read_attribute_value_id                0x000a0020
#define sl_bt_rsp_gatt_server_read_attribute_type_id                 0x010a0020
#define sl_bt_rsp_gatt_server_write_attribute_value_id               0x020a0020
#define sl_bt_rsp_gatt_server_send_user_read_response_id             0x030a0020
#define sl_bt_rsp_gatt_server_send_user_write_response_id            0x040a0020
#define sl_bt_rsp_gatt_server_send_characteristic_notification_id    0x050a0020

/**
 * @brief Characteristic Status Flag
 */
typedef enum
{
  gatt_server_client_config = 0x1, /**< (0x1) Characteristic client
                                        configuration has been changed. */
  gatt_server_confirmation  = 0x2  /**< (0x2) Characteristic confirmation has
                                        been received. */
} gatt_server_characteristic_status_flag_t;



/**
 * @addtogroup sl_bt_evt_gatt_server_attribute_value sl_bt_evt_gatt_server_attribute_value
 * @{
 * @brief Indicates that the value of an attribute in the local GATT database
 * was changed by a remote GATT client
 *
 * The parameter @p att_opcode describes which GATT procedure was used to change
 * the value.
 */

/** @brief Identifier of the attribute_value event */
#define sl_bt_evt_gatt_server_attribute_value_id                     0x000a00a0

/***************************************************************************//**
 * @brief Data structure of the attribute_value event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_server_attribute_value_s
{
  uint8_t    connection; /**< Connection handle */
  uint16_t   attribute;  /**< Attribute Handle */
  uint8_t    att_opcode; /**< Enum @ref gatt_att_opcode_t. Attribute opcode that
                              informs the procedure from which the value was
                              received. */
  uint16_t   offset;     /**< Value offset */
  uint8array value;      /**< Value */
});

typedef struct sl_bt_evt_gatt_server_attribute_value_s sl_bt_evt_gatt_server_attribute_value_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_server_attribute_value

/**
 * @addtogroup sl_bt_evt_gatt_server_user_read_request sl_bt_evt_gatt_server_user_read_request
 * @{
 * @brief Indicates that a remote GATT client is attempting to read a value of
 * an attribute from the local GATT database, where the attribute was defined in
 * the GATT database XML file to have the type="user"
 *
 * The parameter @p att_opcode informs which GATT procedure was used to read the
 * value. The application needs to respond to this request by using the @ref
 * sl_bt_gatt_server_send_user_read_response command within 30 seconds,
 * otherwise further GATT transactions are not allowed by the remote side.
 */

/** @brief Identifier of the user_read_request event */
#define sl_bt_evt_gatt_server_user_read_request_id                   0x010a00a0

/***************************************************************************//**
 * @brief Data structure of the user_read_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_server_user_read_request_s
{
  uint8_t  connection;     /**< Connection handle */
  uint16_t characteristic; /**< GATT characteristic handle. This value is
                                normally received from the gatt_characteristic
                                event. */
  uint8_t  att_opcode;     /**< Enum @ref gatt_att_opcode_t. Attribute opcode
                                that informs the procedure from which the value
                                was received. */
  uint16_t offset;         /**< Value offset */
});

typedef struct sl_bt_evt_gatt_server_user_read_request_s sl_bt_evt_gatt_server_user_read_request_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_server_user_read_request

/**
 * @addtogroup sl_bt_evt_gatt_server_user_write_request sl_bt_evt_gatt_server_user_write_request
 * @{
 * @brief Indicates that a remote GATT client is attempting to write a value of
 * an attribute into the local GATT database, where the attribute was defined in
 * the GATT database XML file to have the type="user"
 *
 * The parameter @p att_opcode informs which attribute procedure was used to
 * write the value. If the @p att_opcode is @ref gatt_write_request (see @ref
 * gatt_att_opcode), the application needs to respond to this request by using
 * the @ref sl_bt_gatt_server_send_user_write_response command within 30
 * seconds, otherwise further GATT transactions are not allowed by the remote
 * side. If the value of @p att_opcode is @ref gatt_execute_write_request, it
 * indicates that this is a queued prepare write request received earlier and
 * now the GATT server is processing the execute write. The event @ref
 * sl_bt_evt_gatt_server_execute_write_completed will be emitted after all
 * queued requests have been processed.
 */

/** @brief Identifier of the user_write_request event */
#define sl_bt_evt_gatt_server_user_write_request_id                  0x020a00a0

/***************************************************************************//**
 * @brief Data structure of the user_write_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_server_user_write_request_s
{
  uint8_t    connection;     /**< Connection handle */
  uint16_t   characteristic; /**< GATT characteristic handle. This value is
                                  normally received from the gatt_characteristic
                                  event. */
  uint8_t    att_opcode;     /**< Enum @ref gatt_att_opcode_t. Attribute opcode
                                  that informs the procedure from which the
                                  value was received. */
  uint16_t   offset;         /**< Value offset */
  uint8array value;          /**< Value */
});

typedef struct sl_bt_evt_gatt_server_user_write_request_s sl_bt_evt_gatt_server_user_write_request_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_server_user_write_request

/**
 * @addtogroup sl_bt_evt_gatt_server_characteristic_status sl_bt_evt_gatt_server_characteristic_status
 * @{
 * @brief Indicates either that a local Client Characteristic Configuration
 * descriptor was changed by the remote GATT client, or that a confirmation from
 * the remote GATT client was received upon a successful reception of the
 * indication
 *
 * Confirmation by the remote GATT client should be received within 30 seconds
 * after an indication was sent with the @ref
 * sl_bt_gatt_server_send_characteristic_notification command, otherwise further
 * GATT transactions over this connection are not allowed by the stack.
 */

/** @brief Identifier of the characteristic_status event */
#define sl_bt_evt_gatt_server_characteristic_status_id               0x030a00a0

/***************************************************************************//**
 * @brief Data structure of the characteristic_status event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_server_characteristic_status_s
{
  uint8_t  connection;          /**< Connection handle */
  uint16_t characteristic;      /**< GATT characteristic handle. This value is
                                     normally received from the
                                     gatt_characteristic event. */
  uint8_t  status_flags;        /**< Enum @ref
                                     gatt_server_characteristic_status_flag_t.
                                     Describes whether Client Characteristic
                                     Configuration was changed or if a
                                     confirmation was received. Values:
                                       - <b>gatt_server_client_config (0x1):</b>
                                         Characteristic client configuration has
                                         been changed.
                                       - <b>gatt_server_confirmation (0x2):</b>
                                         Characteristic confirmation has been
                                         received. */
  uint16_t client_config_flags; /**< Enum @ref gatt_client_config_flag_t. This
                                     field carries the new value of the Client
                                     Characteristic Configuration. If the
                                     status_flags is 0x2 (confirmation
                                     received), the value of this field can be
                                     ignored. */
});

typedef struct sl_bt_evt_gatt_server_characteristic_status_s sl_bt_evt_gatt_server_characteristic_status_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_server_characteristic_status

/**
 * @addtogroup sl_bt_evt_gatt_server_execute_write_completed sl_bt_evt_gatt_server_execute_write_completed
 * @{
 * @brief Execute write completed event indicates that the execute write command
 * from a remote GATT client has completed with the given result
 */

/** @brief Identifier of the execute_write_completed event */
#define sl_bt_evt_gatt_server_execute_write_completed_id             0x040a00a0

/***************************************************************************//**
 * @brief Data structure of the execute_write_completed event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_gatt_server_execute_write_completed_s
{
  uint8_t  connection; /**< Connection handle */
  uint16_t result;     /**< Execute write result */
});

typedef struct sl_bt_evt_gatt_server_execute_write_completed_s sl_bt_evt_gatt_server_execute_write_completed_t;

/** @} */ // end addtogroup sl_bt_evt_gatt_server_execute_write_completed

/** @} */ // end addtogroup sl_bt_gatt_server

/**
 * @addtogroup sl_bt_nvm NVM
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_nvm_save_id                                        0x020d0020
#define sl_bt_cmd_nvm_load_id                                        0x030d0020
#define sl_bt_cmd_nvm_erase_id                                       0x040d0020
#define sl_bt_cmd_nvm_erase_all_id                                   0x010d0020
#define sl_bt_rsp_nvm_save_id                                        0x020d0020
#define sl_bt_rsp_nvm_load_id                                        0x030d0020
#define sl_bt_rsp_nvm_erase_id                                       0x040d0020
#define sl_bt_rsp_nvm_erase_all_id                                   0x010d0020

/**
 * @{ @name Defined Keys
 * @anchor nvm_keys
 *
 * Define keys
 */
/** Crystal tuning value override */
#define NVM_KEY_CTUNE 0x32      

/** @} */


/** @} */ // end addtogroup sl_bt_nvm

/**
 * @addtogroup sl_bt_test Testing Commands
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_test_dtm_tx_id                                     0x000e0020
#define sl_bt_cmd_test_dtm_rx_id                                     0x010e0020
#define sl_bt_cmd_test_dtm_end_id                                    0x020e0020
#define sl_bt_rsp_test_dtm_tx_id                                     0x000e0020
#define sl_bt_rsp_test_dtm_rx_id                                     0x010e0020
#define sl_bt_rsp_test_dtm_end_id                                    0x020e0020

/**
 * @brief Test Packet Types
 */
typedef enum
{
  test_pkt_prbs9    = 0x0,  /**< (0x0) PRBS9 packet payload */
  test_pkt_11110000 = 0x1,  /**< (0x1) 11110000 packet payload */
  test_pkt_10101010 = 0x2,  /**< (0x2) 10101010 packet payload */
  test_pkt_11111111 = 0x4,  /**< (0x4) 11111111 packet payload */
  test_pkt_00000000 = 0x5,  /**< (0x5) 00000000 packet payload */
  test_pkt_00001111 = 0x6,  /**< (0x6) 00001111 packet payload */
  test_pkt_01010101 = 0x7,  /**< (0x7) 01010101 packet payload */
  test_pkt_pn9      = 0xfd, /**< (0xfd) PN9 continuously modulated output */
  test_pkt_carrier  = 0xfe  /**< (0xfe) Unmodulated carrier */
} test_packet_type_t;


/**
 * @brief Test PHY Types
 */
typedef enum
{
  test_phy_1m   = 0x1, /**< (0x1) 1M PHY */
  test_phy_2m   = 0x2, /**< (0x2) 2M PHY */
  test_phy_125k = 0x3, /**< (0x3) 125k Coded PHY */
  test_phy_500k = 0x4  /**< (0x4) 500k Coded PHY */
} test_phy_t;



/**
 * @addtogroup sl_bt_evt_test_dtm_completed sl_bt_evt_test_dtm_completed
 * @{
 * @brief Indicates that the radio has processed a test start or end command
 *
 * The @p result parameter indicates the success of the command.
 *
 * After the receiver or transmitter test is stopped, the @p number_of_packets
 * parameter in this event indicates the number of received or transmitted
 * packets.
 */

/** @brief Identifier of the dtm_completed event */
#define sl_bt_evt_test_dtm_completed_id                              0x000e00a0

/***************************************************************************//**
 * @brief Data structure of the dtm_completed event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_test_dtm_completed_s
{
  uint16_t result;            /**< Command result */
  uint16_t number_of_packets; /**< Number of packets

                                   Only valid for @ref sl_bt_test_dtm_end
                                   command. */
});

typedef struct sl_bt_evt_test_dtm_completed_s sl_bt_evt_test_dtm_completed_t;

/** @} */ // end addtogroup sl_bt_evt_test_dtm_completed

/** @} */ // end addtogroup sl_bt_test

/**
 * @addtogroup sl_bt_sm Security Manager
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_sm_configure_id                                    0x010f0020
#define sl_bt_cmd_sm_set_minimum_key_size_id                         0x140f0020
#define sl_bt_cmd_sm_set_debug_mode_id                               0x0f0f0020
#define sl_bt_cmd_sm_add_to_whitelist_id                             0x130f0020
#define sl_bt_cmd_sm_store_bonding_configuration_id                  0x020f0020
#define sl_bt_cmd_sm_set_bondable_mode_id                            0x000f0020
#define sl_bt_cmd_sm_set_passkey_id                                  0x100f0020
#define sl_bt_cmd_sm_set_oob_data_id                                 0x0a0f0020
#define sl_bt_cmd_sm_use_sc_oob_id                                   0x110f0020
#define sl_bt_cmd_sm_set_sc_remote_oob_data_id                       0x120f0020
#define sl_bt_cmd_sm_increase_security_id                            0x040f0020
#define sl_bt_cmd_sm_enter_passkey_id                                0x080f0020
#define sl_bt_cmd_sm_passkey_confirm_id                              0x090f0020
#define sl_bt_cmd_sm_bonding_confirm_id                              0x0e0f0020
#define sl_bt_cmd_sm_list_all_bondings_id                            0x0b0f0020
#define sl_bt_cmd_sm_delete_bonding_id                               0x060f0020
#define sl_bt_cmd_sm_delete_bondings_id                              0x070f0020
#define sl_bt_rsp_sm_configure_id                                    0x010f0020
#define sl_bt_rsp_sm_set_minimum_key_size_id                         0x140f0020
#define sl_bt_rsp_sm_set_debug_mode_id                               0x0f0f0020
#define sl_bt_rsp_sm_add_to_whitelist_id                             0x130f0020
#define sl_bt_rsp_sm_store_bonding_configuration_id                  0x020f0020
#define sl_bt_rsp_sm_set_bondable_mode_id                            0x000f0020
#define sl_bt_rsp_sm_set_passkey_id                                  0x100f0020
#define sl_bt_rsp_sm_set_oob_data_id                                 0x0a0f0020
#define sl_bt_rsp_sm_use_sc_oob_id                                   0x110f0020
#define sl_bt_rsp_sm_set_sc_remote_oob_data_id                       0x120f0020
#define sl_bt_rsp_sm_increase_security_id                            0x040f0020
#define sl_bt_rsp_sm_enter_passkey_id                                0x080f0020
#define sl_bt_rsp_sm_passkey_confirm_id                              0x090f0020
#define sl_bt_rsp_sm_bonding_confirm_id                              0x0e0f0020
#define sl_bt_rsp_sm_list_all_bondings_id                            0x0b0f0020
#define sl_bt_rsp_sm_delete_bonding_id                               0x060f0020
#define sl_bt_rsp_sm_delete_bondings_id                              0x070f0020

/**
 * @brief Bonding Keys
 */
typedef enum
{
  sm_bonding_key_ltk         = 0x1,  /**< (0x1) LTK saved in master */
  sm_bonding_key_addr_public = 0x2,  /**< (0x2) Public Address */
  sm_bonding_key_addr_static = 0x4,  /**< (0x4) Static Address */
  sm_bonding_key_irk         = 0x8,  /**< (0x8) Identity resolving key for
                                          resolvable private addresses */
  sm_bonding_key_edivrand    = 0x10, /**< (0x10) EDIV+RAND received from slave */
  sm_bonding_key_csrk        = 0x20, /**< (0x20) Connection signature resolving
                                          key */
  sm_bonding_key_masterid    = 0x40  /**< (0x40) EDIV+RAND sent to master */
} sm_bonding_key_t;


/**
 * @brief SMP I/O Capabilities
 */
typedef enum
{
  sm_io_capability_displayonly     = 0x0, /**< (0x0) Display Only */
  sm_io_capability_displayyesno    = 0x1, /**< (0x1) Display with Yes/No-buttons */
  sm_io_capability_keyboardonly    = 0x2, /**< (0x2) Keyboard Only */
  sm_io_capability_noinputnooutput = 0x3, /**< (0x3) No Input and No Output */
  sm_io_capability_keyboarddisplay = 0x4  /**< (0x4) Display with Keyboard */
} sm_io_capability_t;



/**
 * @addtogroup sl_bt_evt_sm_passkey_display sl_bt_evt_sm_passkey_display
 * @{
 * @brief Indicates a request to display the passkey to the user.
 */

/** @brief Identifier of the passkey_display event */
#define sl_bt_evt_sm_passkey_display_id                              0x000f00a0

/***************************************************************************//**
 * @brief Data structure of the passkey_display event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sm_passkey_display_s
{
  uint8_t  connection; /**< Connection handle */
  uint32_t passkey;    /**< Passkey. Range: 0 to 999999.
                              - NOTE! When displaying the passkey to the user,
                                prefix the number with zeros to obtain a 6 digit
                                number
                              - Example: Passkey value is 42
                              - Number to display to the user is 000042 */
});

typedef struct sl_bt_evt_sm_passkey_display_s sl_bt_evt_sm_passkey_display_t;

/** @} */ // end addtogroup sl_bt_evt_sm_passkey_display

/**
 * @addtogroup sl_bt_evt_sm_passkey_request sl_bt_evt_sm_passkey_request
 * @{
 * @brief Indicates a request for the passkey prompt displayed on the remote
 * device
 *
 * Use the command @ref sl_bt_sm_enter_passkey to input the passkey value.
 */

/** @brief Identifier of the passkey_request event */
#define sl_bt_evt_sm_passkey_request_id                              0x010f00a0

/***************************************************************************//**
 * @brief Data structure of the passkey_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sm_passkey_request_s
{
  uint8_t connection; /**< Connection handle */
});

typedef struct sl_bt_evt_sm_passkey_request_s sl_bt_evt_sm_passkey_request_t;

/** @} */ // end addtogroup sl_bt_evt_sm_passkey_request

/**
 * @addtogroup sl_bt_evt_sm_confirm_passkey sl_bt_evt_sm_confirm_passkey
 * @{
 * @brief Indicates a request for passkey display and confirmation by the user
 *
 * Use the command @ref sl_bt_sm_passkey_confirm to accept or reject the
 * displayed passkey.
 */

/** @brief Identifier of the confirm_passkey event */
#define sl_bt_evt_sm_confirm_passkey_id                              0x020f00a0

/***************************************************************************//**
 * @brief Data structure of the confirm_passkey event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sm_confirm_passkey_s
{
  uint8_t  connection; /**< Connection handle */
  uint32_t passkey;    /**< Passkey. Range: 0 to 999999.
                              - NOTE! When displaying the passkey to the user,
                                prefix the number with zeros to obtain a 6 digit
                                number
                              - Example: Passkey value is 42
                              - Number to display to the user is 000042 */
});

typedef struct sl_bt_evt_sm_confirm_passkey_s sl_bt_evt_sm_confirm_passkey_t;

/** @} */ // end addtogroup sl_bt_evt_sm_confirm_passkey

/**
 * @addtogroup sl_bt_evt_sm_bonded sl_bt_evt_sm_bonded
 * @{
 * @brief Triggered after the pairing or bonding procedure is successfully
 * completed.
 */

/** @brief Identifier of the bonded event */
#define sl_bt_evt_sm_bonded_id                                       0x030f00a0

/***************************************************************************//**
 * @brief Data structure of the bonded event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sm_bonded_s
{
  uint8_t connection; /**< Connection handle */
  uint8_t bonding;    /**< Bonding handle. Values:
                             - <b>SL_BT_INVALID_BONDING_HANDLE (0xff):</b>
                               Pairing completed without bonding - the pairing
                               key will be discarded after disconnection.
                             - <b>Other:</b> Procedure completed, pairing key
                               stored with given bonding handle */
});

typedef struct sl_bt_evt_sm_bonded_s sl_bt_evt_sm_bonded_t;

/** @} */ // end addtogroup sl_bt_evt_sm_bonded

/**
 * @addtogroup sl_bt_evt_sm_bonding_failed sl_bt_evt_sm_bonding_failed
 * @{
 * @brief This event is triggered if the pairing or bonding procedure fails.
 */

/** @brief Identifier of the bonding_failed event */
#define sl_bt_evt_sm_bonding_failed_id                               0x040f00a0

/***************************************************************************//**
 * @brief Data structure of the bonding_failed event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sm_bonding_failed_s
{
  uint8_t  connection; /**< Connection handle */
  uint16_t reason;     /**< Describes error that occurred */
});

typedef struct sl_bt_evt_sm_bonding_failed_s sl_bt_evt_sm_bonding_failed_t;

/** @} */ // end addtogroup sl_bt_evt_sm_bonding_failed

/**
 * @addtogroup sl_bt_evt_sm_confirm_bonding sl_bt_evt_sm_confirm_bonding
 * @{
 * @brief Indicates a user request to display that the new bonding request is
 * received and for the user to confirm the request
 *
 * Use the command @ref sl_bt_sm_bonding_confirm to accept or reject the bonding
 * request.
 */

/** @brief Identifier of the confirm_bonding event */
#define sl_bt_evt_sm_confirm_bonding_id                              0x090f00a0

/***************************************************************************//**
 * @brief Data structure of the confirm_bonding event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sm_confirm_bonding_s
{
  uint8_t connection;     /**< Connection handle */
  uint8_t bonding_handle; /**< Bonding handle for the request. Range: 0 to 31,
                               or SL_BT_INVALID_BONDING_HANDLE (0xff).
                                 - NOTE! When the bonding handle is anything
                                   other than SL_BT_INVALID_BONDING_HANDLE
                                   (0xff), a bonding already exists for this
                                   connection. Overwriting the existing bonding
                                   is a potential security risk. */
});

typedef struct sl_bt_evt_sm_confirm_bonding_s sl_bt_evt_sm_confirm_bonding_t;

/** @} */ // end addtogroup sl_bt_evt_sm_confirm_bonding

/**
 * @addtogroup sl_bt_evt_sm_list_bonding_entry sl_bt_evt_sm_list_bonding_entry
 * @{
 * @brief Triggered by the command @ref sl_bt_sm_list_all_bondings if bondings
 * exist in the local database
 */

/** @brief Identifier of the list_bonding_entry event */
#define sl_bt_evt_sm_list_bonding_entry_id                           0x050f00a0

/***************************************************************************//**
 * @brief Data structure of the list_bonding_entry event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_sm_list_bonding_entry_s
{
  uint8_t bonding;      /**< Bonding handle */
  bd_addr address;      /**< Bluetooth address of the remote device */
  uint8_t address_type; /**< Enum @ref gap_address_type_t. Address type. Values:
                               - <b>gap_public_address (0x0):</b> Public device
                                 address
                               - <b>gap_static_address (0x1):</b> Static device
                                 address
                               - <b>gap_random_resolvable_address (0x2):</b>
                                 Private resolvable random address
                               - <b>gap_random_nonresolvable_address (0x3):</b>
                                 Private non-resolvable random address */
});

typedef struct sl_bt_evt_sm_list_bonding_entry_s sl_bt_evt_sm_list_bonding_entry_t;

/** @} */ // end addtogroup sl_bt_evt_sm_list_bonding_entry

/**
 * @addtogroup sl_bt_evt_sm_list_all_bondings_complete sl_bt_evt_sm_list_all_bondings_complete
 * @{
 * @brief Triggered by the @ref sl_bt_sm_list_all_bondings and follows @ref
 * sl_bt_evt_sm_list_bonding_entry events
 */

/** @brief Identifier of the list_all_bondings_complete event */
#define sl_bt_evt_sm_list_all_bondings_complete_id                   0x060f00a0



/** @} */ // end addtogroup sl_bt_evt_sm_list_all_bondings_complete

/** @} */ // end addtogroup sl_bt_sm

/**
 * @addtogroup sl_bt_ota OTA
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_ota_set_device_name_id                             0x01100020
#define sl_bt_cmd_ota_set_advertising_data_id                        0x02100020
#define sl_bt_cmd_ota_set_configuration_id                           0x03100020
#define sl_bt_cmd_ota_set_rf_path_id                                 0x04100020
#define sl_bt_rsp_ota_set_device_name_id                             0x01100020
#define sl_bt_rsp_ota_set_advertising_data_id                        0x02100020
#define sl_bt_rsp_ota_set_configuration_id                           0x03100020
#define sl_bt_rsp_ota_set_rf_path_id                                 0x04100020


/** @} */ // end addtogroup sl_bt_ota

/**
 * @addtogroup sl_bt_coex Coexistence
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_coex_set_options_id                                0x00200020
#define sl_bt_cmd_coex_set_parameters_id                             0x02200020
#define sl_bt_cmd_coex_set_directional_priority_pulse_id             0x03200020
#define sl_bt_cmd_coex_get_counters_id                               0x01200020
#define sl_bt_rsp_coex_set_options_id                                0x00200020
#define sl_bt_rsp_coex_set_parameters_id                             0x02200020
#define sl_bt_rsp_coex_set_directional_priority_pulse_id             0x03200020
#define sl_bt_rsp_coex_get_counters_id                               0x01200020

/**
 * @brief Coexistence Options
 */
typedef enum
{
  coex_option_enable        = 0x100, /**< (0x100) Enable coexistence feature */
  coex_option_tx_abort      = 0x400, /**< (0x400) Abort transmission if grant is
                                          denied */
  coex_option_high_priority = 0x800  /**< (0x800) Enable priority signal */
} coex_option_t;



/** @} */ // end addtogroup sl_bt_coex

/**
 * @cond RESTRICTED
 * @addtogroup sl_bt_l2cap L2CAP
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_l2cap_coc_send_connection_request_id               0x01430020
#define sl_bt_cmd_l2cap_coc_send_connection_response_id              0x02430020
#define sl_bt_cmd_l2cap_coc_send_le_flow_control_credit_id           0x03430020
#define sl_bt_cmd_l2cap_coc_send_disconnection_request_id            0x04430020
#define sl_bt_cmd_l2cap_coc_send_data_id                             0x05430020
#define sl_bt_rsp_l2cap_coc_send_connection_request_id               0x01430020
#define sl_bt_rsp_l2cap_coc_send_connection_response_id              0x02430020
#define sl_bt_rsp_l2cap_coc_send_le_flow_control_credit_id           0x03430020
#define sl_bt_rsp_l2cap_coc_send_disconnection_request_id            0x04430020
#define sl_bt_rsp_l2cap_coc_send_data_id                             0x05430020

/**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Result values for LE Credit Based Connection Response
 */
typedef enum
{
  l2cap_connection_successful            = 0x0,  /**< (0x0) Connection
                                                      successful */
  l2cap_le_psm_not_supported             = 0x2,  /**< (0x2) Connection refused -
                                                      LE_PSM not supported */
  l2cap_no_resources_available           = 0x4,  /**< (0x4) Connection refused -
                                                      no resources available */
  l2cap_insufficient_authentication      = 0x5,  /**< (0x5) Connection refused -
                                                      insufficient
                                                      authentication */
  l2cap_insufficient_authorization       = 0x6,  /**< (0x6) Connection refused -
                                                      insufficient authorization */
  l2cap_insufficient_encryption_key_size = 0x7,  /**< (0x7) Connection refused -
                                                      insufficient encryption
                                                      key size */
  l2cap_insufficient_encryption          = 0x8,  /**< (0x8) Connection refused -
                                                      insufficient encryption */
  l2cap_invalid_source_cid               = 0x9,  /**< (0x9) Connection refused -
                                                      invalid Source CID */
  l2cap_source_cid_already_allocated     = 0x10, /**< (0x10) Connection refused
                                                      \- Source CID already
                                                      allocated */
  l2cap_unacceptable_parameters          = 0x11  /**< (0x11) Connection refused
                                                      \- unacceptable parameters */
} l2cap_coc_connection_result_t;
/** @endcond */


/**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Reason Codes of Command Reject
 */
typedef enum
{
  l2cap_command_not_understood = 0x0, /**< (0x0) Command not understood e.g.,
                                           unknown command code */
  l2cap_signaling_mtu_exceeded = 0x1, /**< (0x1) Command size has exceeded
                                           signaling MTU size */
  l2cap_invalid_cid_request    = 0x2  /**< (0x2) An invalid CID is included in
                                           the command */
} l2cap_command_reject_reason_t;
/** @endcond */


/**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Signaling Command Codes
 */
typedef enum
{
  l2cap_disconnection_request = 0x6,  /**< (0x6) Disconnection request */
  l2cap_connection_request    = 0x14, /**< (0x14) LE credit-based connection
                                           request */
  l2cap_flow_control_credit   = 0x16  /**< (0x16) LE Flow Control Credit */
} l2cap_command_code_t;
/** @endcond */



/**
 * @cond RESTRICTED
 * @addtogroup sl_bt_evt_l2cap_coc_connection_request sl_bt_evt_l2cap_coc_connection_request
 * @{
 * @brief Indicates that an LE credit-based connection request is received
 *
 * The application will respond with @ref
 * sl_bt_l2cap_coc_send_connection_response command.
 */

/** @brief Identifier of the coc_connection_request event */
#define sl_bt_evt_l2cap_coc_connection_request_id                    0x014300a0

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Data structure of the coc_connection_request event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_l2cap_coc_connection_request_s
{
  uint8_t  connection;          /**< Handle of the LE connection which opening
                                     of connection-oriented channel is requested */
  uint16_t le_psm;              /**< LE Protocol/Service Multiplexer - LE_PSM */
  uint16_t source_cid;          /**< The source CID represents a channel
                                     endpoint on the device sending the request */
  uint16_t mtu;                 /**< The MTU size of the device sending the
                                     request. */
  uint16_t mps;                 /**< The MPS size of the device sending the
                                     request. */
  uint16_t initial_credit;      /**< The initial credit value of the device
                                     sending the request */
  uint8_t  flags;               /**< This field indicates the security mode of
                                     the LE connection to let applications
                                     evaluate security requirements.
                                       - <b>Bit 0:</b> Encrypted
                                       - <b>Bit 1:</b> Authenticated
                                       - <b>Bit 2:</b> Authorized

                                     Remaining bits are reserved for future use. */
  uint8_t  encryption_key_size; /**< Key size used by the link */
});

typedef struct sl_bt_evt_l2cap_coc_connection_request_s sl_bt_evt_l2cap_coc_connection_request_t;
/** @endcond */ // end restricted struct

/** @} @endcond */ // end addtogroup sl_bt_evt_l2cap_coc_connection_request

/**
 * @cond RESTRICTED
 * @addtogroup sl_bt_evt_l2cap_coc_connection_response sl_bt_evt_l2cap_coc_connection_response
 * @{
 * @brief Indicates that an LE credit-based connection response is received
 *
 * Ignore other fields if the result field indicates that the connection was
 * refused.
 */

/** @brief Identifier of the coc_connection_response event */
#define sl_bt_evt_l2cap_coc_connection_response_id                   0x024300a0

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Data structure of the coc_connection_response event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_l2cap_coc_connection_response_s
{
  uint8_t  connection;      /**< Handle of the LE connection which response
                                 message is received */
  uint16_t destination_cid; /**< The destination CID represents a channel
                                 endpoint on the device sending the response,
                                 which will become destination channel endpoint
                                 of the device receiving the event */
  uint16_t mtu;             /**< The MTU size of the device sending the response */
  uint16_t mps;             /**< The MPS size of the device sending the request */
  uint16_t initial_credit;  /**< The initial credit value of the device sending
                                 the request. */
  uint16_t l2cap_errorcode; /**< Enum @ref l2cap_coc_connection_result_t. The
                                 l2cap_errorcode field indicates the result of
                                 the connection request. */
});

typedef struct sl_bt_evt_l2cap_coc_connection_response_s sl_bt_evt_l2cap_coc_connection_response_t;
/** @endcond */ // end restricted struct

/** @} @endcond */ // end addtogroup sl_bt_evt_l2cap_coc_connection_response

/**
 * @cond RESTRICTED
 * @addtogroup sl_bt_evt_l2cap_coc_le_flow_control_credit sl_bt_evt_l2cap_coc_le_flow_control_credit
 * @{
 * @brief Indicates that a LE flow control credit is received so the channel
 * endpoint on remote device is capable of receiving more data
 */

/** @brief Identifier of the coc_le_flow_control_credit event */
#define sl_bt_evt_l2cap_coc_le_flow_control_credit_id                0x034300a0

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Data structure of the coc_le_flow_control_credit event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_l2cap_coc_le_flow_control_credit_s
{
  uint8_t  connection; /**< Handle of the LE connection which LE flow control
                            credit message is received */
  uint16_t cid;        /**< The CID represents the destination channel endpoint
                            of the device receiving the flow control credit. */
  uint16_t credits;    /**< The credit value indicates the additional number of
                            PDUs that the device can send. */
});

typedef struct sl_bt_evt_l2cap_coc_le_flow_control_credit_s sl_bt_evt_l2cap_coc_le_flow_control_credit_t;
/** @endcond */ // end restricted struct

/** @} @endcond */ // end addtogroup sl_bt_evt_l2cap_coc_le_flow_control_credit

/**
 * @cond RESTRICTED
 * @addtogroup sl_bt_evt_l2cap_coc_channel_disconnected sl_bt_evt_l2cap_coc_channel_disconnected
 * @{
 * @brief Indicates that a L2CAP connection-oriented channel is disconnected.
 */

/** @brief Identifier of the coc_channel_disconnected event */
#define sl_bt_evt_l2cap_coc_channel_disconnected_id                  0x044300a0

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Data structure of the coc_channel_disconnected event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_l2cap_coc_channel_disconnected_s
{
  uint8_t  connection; /**< Handle of the LE connection, which the
                            connection-oriented channel is disconnected */
  uint16_t cid;        /**< The CID represents the destination channel endpoint
                            of the device receiving the event. */
  uint16_t reason;     /**< The reason for channel disconnection */
});

typedef struct sl_bt_evt_l2cap_coc_channel_disconnected_s sl_bt_evt_l2cap_coc_channel_disconnected_t;
/** @endcond */ // end restricted struct

/** @} @endcond */ // end addtogroup sl_bt_evt_l2cap_coc_channel_disconnected

/**
 * @cond RESTRICTED
 * @addtogroup sl_bt_evt_l2cap_coc_data sl_bt_evt_l2cap_coc_data
 * @{
 * @brief Indicates that there is data received on a L2CAP connection-oriented
 * channel.
 */

/** @brief Identifier of the coc_data event */
#define sl_bt_evt_l2cap_coc_data_id                                  0x054300a0

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Data structure of the coc_data event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_l2cap_coc_data_s
{
  uint8_t    connection; /**< Handle of the LE connection which data is received */
  uint16_t   cid;        /**< The CID represents the source channel endpoint on
                              the device sending data. */
  uint8array data;       /**< Data received. */
});

typedef struct sl_bt_evt_l2cap_coc_data_s sl_bt_evt_l2cap_coc_data_t;
/** @endcond */ // end restricted struct

/** @} @endcond */ // end addtogroup sl_bt_evt_l2cap_coc_data

/**
 * @cond RESTRICTED
 * @addtogroup sl_bt_evt_l2cap_command_rejected sl_bt_evt_l2cap_command_rejected
 * @{
 * @brief Indicates that the remote device received the command and it was
 * rejected
 *
 * This might happen for example the remote does not support the feature or the
 * wrong CID is in the command sent.
 */

/** @brief Identifier of the command_rejected event */
#define sl_bt_evt_l2cap_command_rejected_id                          0x064300a0

/***************************************************************************//**
 * @cond RESTRICTED
 *
 * Restricted/experimental API. Contact Silicon Labs sales for more information.
 *
 * @brief Data structure of the command_rejected event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_l2cap_command_rejected_s
{
  uint8_t  connection; /**< Handle of the LE connection which event is received */
  uint8_t  code;       /**< Enum @ref l2cap_command_code_t. Code of the command
                            has been rejected by the sender of the event. */
  uint16_t reason;     /**< Enum @ref l2cap_command_reject_reason_t. Reason
                            field describe why the L2CAP command was rejected. */
});

typedef struct sl_bt_evt_l2cap_command_rejected_s sl_bt_evt_l2cap_command_rejected_t;
/** @endcond */ // end restricted struct

/** @} @endcond */ // end addtogroup sl_bt_evt_l2cap_command_rejected

/** @} @endcond */ // end addtogroup sl_bt_l2cap

/**
 * @addtogroup sl_bt_cte_transmitter CTE Transmitter
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_cte_transmitter_set_dtm_parameters_id              0x04440020
#define sl_bt_cmd_cte_transmitter_clear_dtm_parameters_id            0x05440020
#define sl_bt_cmd_cte_transmitter_enable_connection_cte_id           0x00440020
#define sl_bt_cmd_cte_transmitter_disable_connection_cte_id          0x01440020
#define sl_bt_cmd_cte_transmitter_enable_connectionless_cte_id       0x02440020
#define sl_bt_cmd_cte_transmitter_disable_connectionless_cte_id      0x03440020
#define sl_bt_cmd_cte_transmitter_enable_silabs_cte_id               0x06440020
#define sl_bt_cmd_cte_transmitter_disable_silabs_cte_id              0x07440020
#define sl_bt_rsp_cte_transmitter_set_dtm_parameters_id              0x04440020
#define sl_bt_rsp_cte_transmitter_clear_dtm_parameters_id            0x05440020
#define sl_bt_rsp_cte_transmitter_enable_connection_cte_id           0x00440020
#define sl_bt_rsp_cte_transmitter_disable_connection_cte_id          0x01440020
#define sl_bt_rsp_cte_transmitter_enable_connectionless_cte_id       0x02440020
#define sl_bt_rsp_cte_transmitter_disable_connectionless_cte_id      0x03440020
#define sl_bt_rsp_cte_transmitter_enable_silabs_cte_id               0x06440020
#define sl_bt_rsp_cte_transmitter_disable_silabs_cte_id              0x07440020


/** @} */ // end addtogroup sl_bt_cte_transmitter

/**
 * @addtogroup sl_bt_cte_receiver CTE Receiver
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_cte_receiver_set_dtm_parameters_id                 0x05450020
#define sl_bt_cmd_cte_receiver_clear_dtm_parameters_id               0x06450020
#define sl_bt_cmd_cte_receiver_set_sync_cte_type_id                  0x09450020
#define sl_bt_cmd_cte_receiver_configure_id                          0x00450020
#define sl_bt_cmd_cte_receiver_enable_connection_cte_id              0x01450020
#define sl_bt_cmd_cte_receiver_disable_connection_cte_id             0x02450020
#define sl_bt_cmd_cte_receiver_enable_connectionless_cte_id          0x03450020
#define sl_bt_cmd_cte_receiver_disable_connectionless_cte_id         0x04450020
#define sl_bt_cmd_cte_receiver_enable_silabs_cte_id                  0x07450020
#define sl_bt_cmd_cte_receiver_disable_silabs_cte_id                 0x08450020
#define sl_bt_rsp_cte_receiver_set_dtm_parameters_id                 0x05450020
#define sl_bt_rsp_cte_receiver_clear_dtm_parameters_id               0x06450020
#define sl_bt_rsp_cte_receiver_set_sync_cte_type_id                  0x09450020
#define sl_bt_rsp_cte_receiver_configure_id                          0x00450020
#define sl_bt_rsp_cte_receiver_enable_connection_cte_id              0x01450020
#define sl_bt_rsp_cte_receiver_disable_connection_cte_id             0x02450020
#define sl_bt_rsp_cte_receiver_enable_connectionless_cte_id          0x03450020
#define sl_bt_rsp_cte_receiver_disable_connectionless_cte_id         0x04450020
#define sl_bt_rsp_cte_receiver_enable_silabs_cte_id                  0x07450020
#define sl_bt_rsp_cte_receiver_disable_silabs_cte_id                 0x08450020


/**
 * @addtogroup sl_bt_evt_cte_receiver_dtm_iq_report sl_bt_evt_cte_receiver_dtm_iq_report
 * @{
 * @brief IQ sample report from DTM CTE packets.
 */

/** @brief Identifier of the dtm_iq_report event */
#define sl_bt_evt_cte_receiver_dtm_iq_report_id                      0x024500a0

/***************************************************************************//**
 * @brief Data structure of the dtm_iq_report event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_cte_receiver_dtm_iq_report_s
{
  uint16_t   status;          /**< Status of CTE IQ sampling */
  uint8_t    channel;         /**< The channel on which the CTE packet was
                                   received */
  int8_t     rssi;            /**< RSSI in the received CTE packet. Unit: dBm */
  uint8_t    rssi_antenna_id; /**< The ID of the antenna on which RSSI was
                                   measured */
  uint8_t    cte_type;        /**< The CTE type
                                     - <b>0:</b> AoA CTE response
                                     - <b>1:</b> AoD CTE response with 1us slots
                                     - <b>2:</b> AoD CTE response with 2us slots */
  uint8_t    slot_durations;  /**< Slot durations
                                     - <b>1:</b> Switching and sampling slots
                                       are 1 us each
                                     - <b>2:</b> Switching and sampling slots
                                       are 2 us each */
  uint16_t   event_counter;   /**< The event counter of the periodic advertising
                                   train or the connection */
  uint8array samples;         /**< IQ samples of the received CTE packet. I and
                                   Q samples follow each other alternately (I,
                                   Q, I, Q, ...) */
});

typedef struct sl_bt_evt_cte_receiver_dtm_iq_report_s sl_bt_evt_cte_receiver_dtm_iq_report_t;

/** @} */ // end addtogroup sl_bt_evt_cte_receiver_dtm_iq_report

/**
 * @addtogroup sl_bt_evt_cte_receiver_connection_iq_report sl_bt_evt_cte_receiver_connection_iq_report
 * @{
 * @brief IQ sample report from connection CTE packets.
 */

/** @brief Identifier of the connection_iq_report event */
#define sl_bt_evt_cte_receiver_connection_iq_report_id               0x004500a0

/***************************************************************************//**
 * @brief Data structure of the connection_iq_report event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_cte_receiver_connection_iq_report_s
{
  uint16_t   status;          /**< Status of CTE IQ sampling */
  uint8_t    connection;      /**< Connection handle or periodic advertising
                                   synchronization handle */
  uint8_t    phy;             /**< The PHY on which the packet is received.
                                     - <b>1:</b> 1M PHY
                                     - <b>2:</b> 2M PHY */
  uint8_t    channel;         /**< The channel on which the CTE packet was
                                   received */
  int8_t     rssi;            /**< RSSI in the received CTE packet. Unit: dBm */
  uint8_t    rssi_antenna_id; /**< The ID of the antenna on which RSSI was
                                   measured */
  uint8_t    cte_type;        /**< The CTE type
                                     - <b>0:</b> AoA CTE response
                                     - <b>1:</b> AoD CTE response with 1us slots
                                     - <b>2:</b> AoD CTE response with 2us slots */
  uint8_t    slot_durations;  /**< Slot durations
                                     - <b>1:</b> Switching and sampling slots
                                       are 1 us each
                                     - <b>2:</b> Switching and sampling slots
                                       are 2 us each */
  uint16_t   event_counter;   /**< The event counter of the connection */
  uint8array samples;         /**< IQ samples of the received CTE packet. I and
                                   Q samples follow each other alternately (I,
                                   Q, I, Q, ...) */
});

typedef struct sl_bt_evt_cte_receiver_connection_iq_report_s sl_bt_evt_cte_receiver_connection_iq_report_t;

/** @} */ // end addtogroup sl_bt_evt_cte_receiver_connection_iq_report

/**
 * @addtogroup sl_bt_evt_cte_receiver_connectionless_iq_report sl_bt_evt_cte_receiver_connectionless_iq_report
 * @{
 * @brief IQ sample report from connectionless CTE packets.
 */

/** @brief Identifier of the connectionless_iq_report event */
#define sl_bt_evt_cte_receiver_connectionless_iq_report_id           0x014500a0

/***************************************************************************//**
 * @brief Data structure of the connectionless_iq_report event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_cte_receiver_connectionless_iq_report_s
{
  uint16_t   status;          /**< Status of CTE IQ sampling */
  uint16_t   sync;            /**< Periodic advertising synchronization handle */
  uint8_t    channel;         /**< The channel on which the CTE packet was
                                   received */
  int8_t     rssi;            /**< RSSI in the received CTE packet. Unit: dBm */
  uint8_t    rssi_antenna_id; /**< The ID of the antenna on which RSSI was
                                   measured */
  uint8_t    cte_type;        /**< The CTE type
                                     - <b>0:</b> AoA CTE response
                                     - <b>1:</b> AoD CTE response with 1us slots
                                     - <b>2:</b> AoD CTE response with 2us slots */
  uint8_t    slot_durations;  /**< Slot durations
                                     - <b>1:</b> Switching and sampling slots
                                       are 1 us each
                                     - <b>2:</b> Switching and sampling slots
                                       are 2 us each */
  uint16_t   event_counter;   /**< The event counter of the periodic advertising
                                   train */
  uint8array samples;         /**< IQ samples of the received CTE packet. I and
                                   Q samples follow each other alternately (I,
                                   Q, I, Q, ...) */
});

typedef struct sl_bt_evt_cte_receiver_connectionless_iq_report_s sl_bt_evt_cte_receiver_connectionless_iq_report_t;

/** @} */ // end addtogroup sl_bt_evt_cte_receiver_connectionless_iq_report

/**
 * @addtogroup sl_bt_evt_cte_receiver_silabs_iq_report sl_bt_evt_cte_receiver_silabs_iq_report
 * @{
 * @brief IQ samples report from Silicon Labs CTE packets.
 */

/** @brief Identifier of the silabs_iq_report event */
#define sl_bt_evt_cte_receiver_silabs_iq_report_id                   0x034500a0

/***************************************************************************//**
 * @brief Data structure of the silabs_iq_report event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_cte_receiver_silabs_iq_report_s
{
  uint16_t   status;          /**< Status of CTE IQ sampling */
  bd_addr    address;         /**< Bluetooth address of the remote device */
  uint8_t    address_type;    /**< Advertiser address type. Values:
                                     - <b>0:</b> Public address
                                     - <b>1:</b> Random address
                                     - <b>255:</b> No address provided
                                       (anonymous advertising) */
  uint8_t    phy;             /**< The PHY on which the packet is received.
                                     - <b>1:</b> 1M PHY
                                     - <b>2:</b> 2M PHY */
  uint8_t    channel;         /**< The channel on which the CTE packet was
                                   received */
  int8_t     rssi;            /**< RSSI in the received CTE packet. Unit: dBm */
  uint8_t    rssi_antenna_id; /**< The ID of the antenna on which RSSI was
                                   measured */
  uint8_t    cte_type;        /**< The CTE type
                                     - <b>0:</b> AoA CTE response
                                     - <b>1:</b> AoD CTE response with 1us slots
                                     - <b>2:</b> AoD CTE response with 2us slots */
  uint8_t    slot_durations;  /**< Slot durations
                                     - <b>1:</b> Switching and sampling slots
                                       are 1 us each
                                     - <b>2:</b> Switching and sampling slots
                                       are 2 us each */
  uint16_t   packet_counter;  /**< The event counter of the periodic advertising
                                   train or the connection */
  uint8array samples;         /**< IQ samples of the received CTE packet. I and
                                   Q samples follow each other alternately (I,
                                   Q, I, Q, ...) */
});

typedef struct sl_bt_evt_cte_receiver_silabs_iq_report_s sl_bt_evt_cte_receiver_silabs_iq_report_t;

/** @} */ // end addtogroup sl_bt_evt_cte_receiver_silabs_iq_report

/** @} */ // end addtogroup sl_bt_cte_receiver

/**
 * @addtogroup sl_bt_user User Messaging
 * @{
 */

/* Command and Response IDs */
#define sl_bt_cmd_user_message_to_target_id                          0x00ff0020
#define sl_bt_rsp_user_message_to_target_id                          0x00ff0020


/**
 * @addtogroup sl_bt_evt_user_message_to_host sl_bt_evt_user_message_to_host
 * @{
 * @brief Used by the target application on a device to initiate communication
 * and send a message to the NCP host
 *
 * Do not send event messages in the context of the user command handling.
 */

/** @brief Identifier of the message_to_host event */
#define sl_bt_evt_user_message_to_host_id                            0x00ff00a0

/***************************************************************************//**
 * @brief Data structure of the message_to_host event
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_evt_user_message_to_host_s
{
  uint8array message; /**< The message */
});

typedef struct sl_bt_evt_user_message_to_host_s sl_bt_evt_user_message_to_host_t;

/** @} */ // end addtogroup sl_bt_evt_user_message_to_host

/** @} */ // end addtogroup sl_bt_user


/***************************************************************************//**
 * @struct sl_bt_msg
 *
 ******************************************************************************/
PACKSTRUCT( struct sl_bt_msg {
  /** API protocol header consisting of event identifier and data length */
  uint32_t   header;

  /** Union of API event types */
  union {
    uint8_t handle;
    sl_bt_evt_dfu_boot_t                                         evt_dfu_boot; /**< Data field for dfu boot event*/
    sl_bt_evt_dfu_boot_failure_t                                 evt_dfu_boot_failure; /**< Data field for dfu boot_failure event*/
    sl_bt_evt_system_boot_t                                      evt_system_boot; /**< Data field for system boot event*/
    sl_bt_evt_system_error_t                                     evt_system_error; /**< Data field for system error event*/
    sl_bt_evt_system_hardware_error_t                            evt_system_hardware_error; /**< Data field for system hardware_error event*/
    sl_bt_evt_system_external_signal_t                           evt_system_external_signal; /**< Data field for system external_signal event*/
    sl_bt_evt_system_soft_timer_t                                evt_system_soft_timer; /**< Data field for system soft_timer event*/
    sl_bt_evt_advertiser_timeout_t                               evt_advertiser_timeout; /**< Data field for advertiser timeout event*/
    sl_bt_evt_advertiser_scan_request_t                          evt_advertiser_scan_request; /**< Data field for advertiser scan_request event*/
    sl_bt_evt_advertiser_periodic_advertising_status_t           evt_advertiser_periodic_advertising_status; /**< Data field for advertiser periodic_advertising_status event*/
    sl_bt_evt_scanner_scan_report_t                              evt_scanner_scan_report; /**< Data field for scanner scan_report event*/
    sl_bt_evt_sync_opened_t                                      evt_sync_opened; /**< Data field for sync opened event*/
    sl_bt_evt_sync_data_t                                        evt_sync_data; /**< Data field for sync data event*/
    sl_bt_evt_sync_closed_t                                      evt_sync_closed; /**< Data field for sync closed event*/
    sl_bt_evt_connection_opened_t                                evt_connection_opened; /**< Data field for connection opened event*/
    sl_bt_evt_connection_parameters_t                            evt_connection_parameters; /**< Data field for connection parameters event*/
    sl_bt_evt_connection_phy_status_t                            evt_connection_phy_status; /**< Data field for connection phy_status event*/
    sl_bt_evt_connection_rssi_t                                  evt_connection_rssi; /**< Data field for connection rssi event*/
    sl_bt_evt_connection_closed_t                                evt_connection_closed; /**< Data field for connection closed event*/
    sl_bt_evt_gatt_mtu_exchanged_t                               evt_gatt_mtu_exchanged; /**< Data field for gatt mtu_exchanged event*/
    sl_bt_evt_gatt_service_t                                     evt_gatt_service; /**< Data field for gatt service event*/
    sl_bt_evt_gatt_characteristic_t                              evt_gatt_characteristic; /**< Data field for gatt characteristic event*/
    sl_bt_evt_gatt_descriptor_t                                  evt_gatt_descriptor; /**< Data field for gatt descriptor event*/
    sl_bt_evt_gatt_characteristic_value_t                        evt_gatt_characteristic_value; /**< Data field for gatt characteristic_value event*/
    sl_bt_evt_gatt_descriptor_value_t                            evt_gatt_descriptor_value; /**< Data field for gatt descriptor_value event*/
    sl_bt_evt_gatt_procedure_completed_t                         evt_gatt_procedure_completed; /**< Data field for gatt procedure_completed event*/
    sl_bt_evt_gatt_server_attribute_value_t                      evt_gatt_server_attribute_value; /**< Data field for gatt_server attribute_value event*/
    sl_bt_evt_gatt_server_user_read_request_t                    evt_gatt_server_user_read_request; /**< Data field for gatt_server user_read_request event*/
    sl_bt_evt_gatt_server_user_write_request_t                   evt_gatt_server_user_write_request; /**< Data field for gatt_server user_write_request event*/
    sl_bt_evt_gatt_server_characteristic_status_t                evt_gatt_server_characteristic_status; /**< Data field for gatt_server characteristic_status event*/
    sl_bt_evt_gatt_server_execute_write_completed_t              evt_gatt_server_execute_write_completed; /**< Data field for gatt_server execute_write_completed event*/
    sl_bt_evt_test_dtm_completed_t                               evt_test_dtm_completed; /**< Data field for test dtm_completed event*/
    sl_bt_evt_sm_passkey_display_t                               evt_sm_passkey_display; /**< Data field for sm passkey_display event*/
    sl_bt_evt_sm_passkey_request_t                               evt_sm_passkey_request; /**< Data field for sm passkey_request event*/
    sl_bt_evt_sm_confirm_passkey_t                               evt_sm_confirm_passkey; /**< Data field for sm confirm_passkey event*/
    sl_bt_evt_sm_bonded_t                                        evt_sm_bonded; /**< Data field for sm bonded event*/
    sl_bt_evt_sm_bonding_failed_t                                evt_sm_bonding_failed; /**< Data field for sm bonding_failed event*/
    sl_bt_evt_sm_confirm_bonding_t                               evt_sm_confirm_bonding; /**< Data field for sm confirm_bonding event*/
    sl_bt_evt_sm_list_bonding_entry_t                            evt_sm_list_bonding_entry; /**< Data field for sm list_bonding_entry event*/
    sl_bt_evt_l2cap_coc_connection_request_t                     evt_l2cap_coc_connection_request; /**< Data field for l2cap coc_connection_request event*/
    sl_bt_evt_l2cap_coc_connection_response_t                    evt_l2cap_coc_connection_response; /**< Data field for l2cap coc_connection_response event*/
    sl_bt_evt_l2cap_coc_le_flow_control_credit_t                 evt_l2cap_coc_le_flow_control_credit; /**< Data field for l2cap coc_le_flow_control_credit event*/
    sl_bt_evt_l2cap_coc_channel_disconnected_t                   evt_l2cap_coc_channel_disconnected; /**< Data field for l2cap coc_channel_disconnected event*/
    sl_bt_evt_l2cap_coc_data_t                                   evt_l2cap_coc_data; /**< Data field for l2cap coc_data event*/
    sl_bt_evt_l2cap_command_rejected_t                           evt_l2cap_command_rejected; /**< Data field for l2cap command_rejected event*/
    sl_bt_evt_cte_receiver_dtm_iq_report_t                       evt_cte_receiver_dtm_iq_report; /**< Data field for cte_receiver dtm_iq_report event*/
    sl_bt_evt_cte_receiver_connection_iq_report_t                evt_cte_receiver_connection_iq_report; /**< Data field for cte_receiver connection_iq_report event*/
    sl_bt_evt_cte_receiver_connectionless_iq_report_t            evt_cte_receiver_connectionless_iq_report; /**< Data field for cte_receiver connectionless_iq_report event*/
    sl_bt_evt_cte_receiver_silabs_iq_report_t                    evt_cte_receiver_silabs_iq_report; /**< Data field for cte_receiver silabs_iq_report event*/
    sl_bt_evt_user_message_to_host_t                             evt_user_message_to_host; /**< Data field for user message_to_host event*/
    uint8_t payload[SL_BT_MAX_PAYLOAD_SIZE];
  } data;
});

/** BT API message type */
typedef struct sl_bt_msg sl_bt_msg_t;

#endif