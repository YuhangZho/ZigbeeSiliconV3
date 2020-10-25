/***************************************************************************//**
 * @brief Internal helper for Bluetooth API migration from Bluetooth SDK v2.x to v3.0
 *
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SLI_BT_MIGRATE_TYPES_H
#define SLI_BT_MIGRATE_TYPES_H

#include <string.h>
#include <stdint.h>
typedef uint8_t   uint8;
typedef uint16_t  uint16;
typedef uint32_t  uint32;
typedef int8_t    int8;
typedef int16_t   int16;
typedef int32_t   int32;

typedef uint16_t errorcode_t;

#ifndef SL_BT_TYPE_BDADDR
#define SL_BT_TYPE_BDADDR
typedef struct {
  uint8_t addr[6];
} bd_addr;
#endif

typedef struct {
  uint32_t v;
} gecko_configuration_t;

#if defined(__IAR_SYSTEMS_ICC__)
  #define SL_BT_API_MIGRATE(A)  _Pragma("deprecated")
#elif defined(__GNUC__)
  #define SL_BT_API_MIGRATE(A) __attribute__((error(A)))
#else
  #define SL_BT_API_MIGRATE(A)
#endif

struct gecko_msg_dfu_flash_set_address_rsp_t;
struct gecko_msg_dfu_flash_upload_rsp_t;
struct gecko_msg_dfu_flash_upload_finish_rsp_t;
struct gecko_msg_system_hello_rsp_t;
struct gecko_msg_system_get_bt_address_rsp_t;
struct gecko_msg_system_set_bt_address_rsp_t;
struct gecko_msg_system_set_tx_power_rsp_t;
struct gecko_msg_system_get_random_data_rsp_t;
struct gecko_msg_system_halt_rsp_t;
struct gecko_msg_system_set_device_name_rsp_t;
struct gecko_msg_system_linklayer_configure_rsp_t;
struct gecko_msg_system_get_counters_rsp_t;
struct gecko_msg_system_data_buffer_write_rsp_t;
struct gecko_msg_system_set_identity_address_rsp_t;
struct gecko_msg_system_data_buffer_clear_rsp_t;
struct gecko_msg_le_gap_open_rsp_t;
struct gecko_msg_le_gap_set_mode_rsp_t;
struct gecko_msg_le_gap_discover_rsp_t;
struct gecko_msg_le_gap_end_procedure_rsp_t;
struct gecko_msg_le_gap_set_adv_parameters_rsp_t;
struct gecko_msg_le_gap_set_conn_parameters_rsp_t;
struct gecko_msg_le_gap_set_scan_parameters_rsp_t;
struct gecko_msg_le_gap_set_adv_data_rsp_t;
struct gecko_msg_le_gap_set_adv_timeout_rsp_t;
struct gecko_msg_le_gap_set_conn_phy_rsp_t;
struct gecko_msg_le_gap_bt5_set_mode_rsp_t;
struct gecko_msg_le_gap_bt5_set_adv_parameters_rsp_t;
struct gecko_msg_le_gap_bt5_set_adv_data_rsp_t;
struct gecko_msg_le_gap_set_privacy_mode_rsp_t;
struct gecko_msg_le_gap_set_advertise_timing_rsp_t;
struct gecko_msg_le_gap_set_advertise_channel_map_rsp_t;
struct gecko_msg_le_gap_set_advertise_report_scan_request_rsp_t;
struct gecko_msg_le_gap_set_advertise_phy_rsp_t;
struct gecko_msg_le_gap_set_advertise_configuration_rsp_t;
struct gecko_msg_le_gap_clear_advertise_configuration_rsp_t;
struct gecko_msg_le_gap_start_advertising_rsp_t;
struct gecko_msg_le_gap_stop_advertising_rsp_t;
struct gecko_msg_le_gap_set_discovery_timing_rsp_t;
struct gecko_msg_le_gap_set_discovery_type_rsp_t;
struct gecko_msg_le_gap_start_discovery_rsp_t;
struct gecko_msg_le_gap_set_data_channel_classification_rsp_t;
struct gecko_msg_le_gap_connect_rsp_t;
struct gecko_msg_le_gap_set_advertise_tx_power_rsp_t;
struct gecko_msg_le_gap_set_discovery_extended_scan_response_rsp_t;
struct gecko_msg_le_gap_start_periodic_advertising_rsp_t;
struct gecko_msg_le_gap_stop_periodic_advertising_rsp_t;
struct gecko_msg_le_gap_set_long_advertising_data_rsp_t;
struct gecko_msg_le_gap_enable_whitelisting_rsp_t;
struct gecko_msg_le_gap_set_conn_timing_parameters_rsp_t;
struct gecko_msg_le_gap_set_advertise_random_address_rsp_t;
struct gecko_msg_le_gap_clear_advertise_random_address_rsp_t;
struct gecko_msg_sync_open_rsp_t;
struct gecko_msg_sync_close_rsp_t;
struct gecko_msg_le_connection_set_parameters_rsp_t;
struct gecko_msg_le_connection_get_rssi_rsp_t;
struct gecko_msg_le_connection_disable_slave_latency_rsp_t;
struct gecko_msg_le_connection_set_phy_rsp_t;
struct gecko_msg_le_connection_close_rsp_t;
struct gecko_msg_le_connection_set_timing_parameters_rsp_t;
struct gecko_msg_le_connection_read_channel_map_rsp_t;
struct gecko_msg_le_connection_set_preferred_phy_rsp_t;
struct gecko_msg_gatt_set_max_mtu_rsp_t;
struct gecko_msg_gatt_discover_primary_services_rsp_t;
struct gecko_msg_gatt_discover_primary_services_by_uuid_rsp_t;
struct gecko_msg_gatt_discover_characteristics_rsp_t;
struct gecko_msg_gatt_discover_characteristics_by_uuid_rsp_t;
struct gecko_msg_gatt_set_characteristic_notification_rsp_t;
struct gecko_msg_gatt_discover_descriptors_rsp_t;
struct gecko_msg_gatt_read_characteristic_value_rsp_t;
struct gecko_msg_gatt_read_characteristic_value_by_uuid_rsp_t;
struct gecko_msg_gatt_write_characteristic_value_rsp_t;
struct gecko_msg_gatt_write_characteristic_value_without_response_rsp_t;
struct gecko_msg_gatt_prepare_characteristic_value_write_rsp_t;
struct gecko_msg_gatt_execute_characteristic_value_write_rsp_t;
struct gecko_msg_gatt_send_characteristic_confirmation_rsp_t;
struct gecko_msg_gatt_read_descriptor_value_rsp_t;
struct gecko_msg_gatt_write_descriptor_value_rsp_t;
struct gecko_msg_gatt_find_included_services_rsp_t;
struct gecko_msg_gatt_read_multiple_characteristic_values_rsp_t;
struct gecko_msg_gatt_read_characteristic_value_from_offset_rsp_t;
struct gecko_msg_gatt_prepare_characteristic_value_reliable_write_rsp_t;
struct gecko_msg_gatt_server_read_attribute_value_rsp_t;
struct gecko_msg_gatt_server_read_attribute_type_rsp_t;
struct gecko_msg_gatt_server_write_attribute_value_rsp_t;
struct gecko_msg_gatt_server_send_user_read_response_rsp_t;
struct gecko_msg_gatt_server_send_user_write_response_rsp_t;
struct gecko_msg_gatt_server_send_characteristic_notification_rsp_t;
struct gecko_msg_gatt_server_find_attribute_rsp_t;
struct gecko_msg_gatt_server_set_capabilities_rsp_t;
struct gecko_msg_gatt_server_set_max_mtu_rsp_t;
struct gecko_msg_gatt_server_get_mtu_rsp_t;
struct gecko_msg_gatt_server_enable_capabilities_rsp_t;
struct gecko_msg_gatt_server_disable_capabilities_rsp_t;
struct gecko_msg_gatt_server_get_enabled_capabilities_rsp_t;
struct gecko_msg_hardware_set_soft_timer_rsp_t;
struct gecko_msg_hardware_get_time_rsp_t;
struct gecko_msg_hardware_set_lazy_soft_timer_rsp_t;
struct gecko_msg_flash_ps_erase_all_rsp_t;
struct gecko_msg_flash_ps_save_rsp_t;
struct gecko_msg_flash_ps_load_rsp_t;
struct gecko_msg_flash_ps_erase_rsp_t;
struct gecko_msg_test_dtm_tx_rsp_t;
struct gecko_msg_test_dtm_rx_rsp_t;
struct gecko_msg_test_dtm_end_rsp_t;
struct gecko_msg_sm_set_bondable_mode_rsp_t;
struct gecko_msg_sm_configure_rsp_t;
struct gecko_msg_sm_store_bonding_configuration_rsp_t;
struct gecko_msg_sm_increase_security_rsp_t;
struct gecko_msg_sm_delete_bonding_rsp_t;
struct gecko_msg_sm_delete_bondings_rsp_t;
struct gecko_msg_sm_enter_passkey_rsp_t;
struct gecko_msg_sm_passkey_confirm_rsp_t;
struct gecko_msg_sm_set_oob_data_rsp_t;
struct gecko_msg_sm_list_all_bondings_rsp_t;
struct gecko_msg_sm_bonding_confirm_rsp_t;
struct gecko_msg_sm_set_debug_mode_rsp_t;
struct gecko_msg_sm_set_passkey_rsp_t;
struct gecko_msg_sm_use_sc_oob_rsp_t;
struct gecko_msg_sm_set_sc_remote_oob_data_rsp_t;
struct gecko_msg_sm_add_to_whitelist_rsp_t;
struct gecko_msg_sm_set_minimum_key_size_rsp_t;
struct gecko_msg_coex_set_options_rsp_t;
struct gecko_msg_coex_get_counters_rsp_t;
struct gecko_msg_coex_set_parameters_rsp_t;
struct gecko_msg_coex_set_directional_priority_pulse_rsp_t;
struct gecko_msg_cte_transmitter_enable_cte_response_rsp_t;
struct gecko_msg_cte_transmitter_disable_cte_response_rsp_t;
struct gecko_msg_cte_transmitter_start_connectionless_cte_rsp_t;
struct gecko_msg_cte_transmitter_stop_connectionless_cte_rsp_t;
struct gecko_msg_cte_transmitter_set_dtm_parameters_rsp_t;
struct gecko_msg_cte_transmitter_clear_dtm_parameters_rsp_t;
struct gecko_msg_cte_receiver_configure_rsp_t;
struct gecko_msg_cte_receiver_start_iq_sampling_rsp_t;
struct gecko_msg_cte_receiver_stop_iq_sampling_rsp_t;
struct gecko_msg_cte_receiver_start_connectionless_iq_sampling_rsp_t;
struct gecko_msg_cte_receiver_stop_connectionless_iq_sampling_rsp_t;
struct gecko_msg_cte_receiver_set_dtm_parameters_rsp_t;
struct gecko_msg_cte_receiver_clear_dtm_parameters_rsp_t;
struct gecko_msg_user_message_to_target_rsp_t;

#endif
