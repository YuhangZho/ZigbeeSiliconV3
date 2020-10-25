/***************************************************************************//**
 * @brief Bluetooth API migration helper from Bluetooth SDK v2.x to v3.0
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

#ifndef SL_BT_API_MIGRATE_HELPER_H
#define SL_BT_API_MIGRATE_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sli_bt_migrate_types.h"

SL_BT_API_MIGRATE("Replaced by sl_bt_wait_event()")
struct gecko_cmd_packet* gecko_wait_event(void);

SL_BT_API_MIGRATE("Replaced by sl_bt_pop_event()")
struct gecko_cmd_packet* gecko_peek_event(void);

SL_BT_API_MIGRATE("Replaced by sl_bt_event_pending()")
int gecko_event_pending(void);

SL_BT_API_MIGRATE("Replaced by sl_bt_init_stack()")
errorcode_t gecko_stack_init(const gecko_configuration_t *config);

SL_BT_API_MIGRATE("Replaced by sl_bt_init_multiprotocol()")
void gecko_init_multiprotocol(const void *config);

SL_BT_API_MIGRATE("Replaced by sl_bt_priority_handle()")
void gecko_priority_handle(void);

SL_BT_API_MIGRATE("Replaced by sl_bt_external_signal()")
void gecko_external_signal(uint32 signals);

SL_BT_API_MIGRATE("Replaced by sl_bt_send_system_awake()")
void gecko_send_system_awake();

SL_BT_API_MIGRATE("Replaced by sl_bt_send_system_error()")
void gecko_send_system_error(uint16 reason, uint8 data_len, const uint8* data);

SL_BT_API_MIGRATE("Replaced by sl_bt_class_dfu_init()")
void gecko_bgapi_class_dfu_init();

SL_BT_API_MIGRATE("Replaced by sl_bt_class_system_init()")
void gecko_bgapi_class_system_init();

SL_BT_API_MIGRATE("Renamed to 'gap'. Use sl_bt_class_gap_init()")
void gecko_bgapi_class_le_gap_init();

SL_BT_API_MIGRATE("Replaced by sl_bt_class_sync_init()")
void gecko_bgapi_class_sync_init();

SL_BT_API_MIGRATE("Renamed to 'connection'. Use sl_bt_class_connection_init()")
void gecko_bgapi_class_le_connection_init();

SL_BT_API_MIGRATE("Replaced by sl_bt_class_gatt_init()")
void gecko_bgapi_class_gatt_init();

SL_BT_API_MIGRATE("Replaced by sl_bt_class_gatt_server_init()")
void gecko_bgapi_class_gatt_server_init();

SL_BT_API_MIGRATE("This class is removed. Softtimers are moved to system class")
void gecko_bgapi_class_hardware_init();

SL_BT_API_MIGRATE("Renamed to 'nvm'. Use sl_bt_class_nvm_init()")
void gecko_bgapi_class_flash_init();

SL_BT_API_MIGRATE("Replaced by sl_bt_class_test_init()")
void gecko_bgapi_class_test_init();

SL_BT_API_MIGRATE("Replaced by sl_bt_class_sm_init()")
void gecko_bgapi_class_sm_init();

SL_BT_API_MIGRATE("Replaced by sl_bt_class_coex_init()")
void gecko_bgapi_class_coex_init();

SL_BT_API_MIGRATE("Replaced by sl_bt_class_cte_transmitter_init()")
void gecko_bgapi_class_cte_transmitter_init();

SL_BT_API_MIGRATE("Replaced by sl_bt_class_cte_receiver_init()")
void gecko_bgapi_class_cte_receiver_init();

SL_BT_API_MIGRATE("Replaced by sl_bt_init_afh()")
void gecko_init_afh();

SL_BT_API_MIGRATE("Replaced by sl_bt_init_periodic_advertising()")
void gecko_init_periodic_advertising();

SL_BT_API_MIGRATE("Replaced by sl_bt_init_whitelisting()")
void gecko_init_whitelisting();

SL_BT_API_MIGRATE("Removed. Use sl_bt_init_stack() and individual class initialization functions")
errorcode_t gecko_init(const gecko_configuration_t *config);

SL_BT_API_MIGRATE("Replaced by sl_bt_send_rsp_user_message_to_target()")
void gecko_send_rsp_user_message_to_target(uint16 result, uint8 data_len, const uint8* data_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_send_evt_user_message_to_host()")
void gecko_send_evt_user_message_to_host(uint8 data_len, const uint8* data_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_dfu_reset()")
void* gecko_cmd_dfu_reset(uint8 dfu);

SL_BT_API_MIGRATE("Replaced by sl_bt_dfu_flash_set_address()")
struct gecko_msg_dfu_flash_set_address_rsp_t* gecko_cmd_dfu_flash_set_address(uint32 address);

SL_BT_API_MIGRATE("Replaced by sl_bt_dfu_flash_upload()")
struct gecko_msg_dfu_flash_upload_rsp_t* gecko_cmd_dfu_flash_upload(uint8 data_len, const uint8* data_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_dfu_flash_upload_finish()")
struct gecko_msg_dfu_flash_upload_finish_rsp_t* gecko_cmd_dfu_flash_upload_finish();

SL_BT_API_MIGRATE("Replaced by sl_bt_system_hello()")
struct gecko_msg_system_hello_rsp_t* gecko_cmd_system_hello();

SL_BT_API_MIGRATE("Replaced by sl_bt_system_reset()")
void* gecko_cmd_system_reset(uint8 dfu);

SL_BT_API_MIGRATE("Replaced by sl_bt_system_get_identity_address()")
struct gecko_msg_system_get_bt_address_rsp_t* gecko_cmd_system_get_bt_address();

SL_BT_API_MIGRATE("Replaced by sl_bt_system_set_identity_address()")
struct gecko_msg_system_set_bt_address_rsp_t* gecko_cmd_system_set_bt_address(bd_addr address);

SL_BT_API_MIGRATE("Replaced by sl_bt_system_set_max_tx_power()")
struct gecko_msg_system_set_tx_power_rsp_t* gecko_cmd_system_set_tx_power(int16 power);

SL_BT_API_MIGRATE("Replaced by sl_bt_system_get_random_data()")
struct gecko_msg_system_get_random_data_rsp_t* gecko_cmd_system_get_random_data(uint8 length);

SL_BT_API_MIGRATE("Replaced by sl_bt_system_halt()")
struct gecko_msg_system_halt_rsp_t* gecko_cmd_system_halt(uint8 halt);

SL_BT_API_MIGRATE("Replaced by sl_bt_ota_set_device_name()")
struct gecko_msg_system_set_device_name_rsp_t* gecko_cmd_system_set_device_name(uint8 type, uint8 name_len, const uint8* name_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_system_linklayer_configure()")
struct gecko_msg_system_linklayer_configure_rsp_t* gecko_cmd_system_linklayer_configure(uint8 key, uint8 data_len, const uint8* data_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_system_get_counters()")
struct gecko_msg_system_get_counters_rsp_t* gecko_cmd_system_get_counters(uint8 reset);

SL_BT_API_MIGRATE("Replaced by sl_bt_system_data_buffer_write()")
struct gecko_msg_system_data_buffer_write_rsp_t* gecko_cmd_system_data_buffer_write(uint8 data_len, const uint8* data_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_system_set_identity_address()")
struct gecko_msg_system_set_identity_address_rsp_t* gecko_cmd_system_set_identity_address(bd_addr address, uint8 type);

SL_BT_API_MIGRATE("Replaced by sl_bt_system_data_buffer_clear()")
struct gecko_msg_system_data_buffer_clear_rsp_t* gecko_cmd_system_data_buffer_clear();

SL_BT_API_MIGRATE("Replaced by sl_bt_connection_open()")
struct gecko_msg_le_gap_open_rsp_t* gecko_cmd_le_gap_open(bd_addr address, uint8 address_type);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_start() and sl_bt_advertiser_stop(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_set_mode_rsp_t* gecko_cmd_le_gap_set_mode(uint8 discover, uint8 connect);

SL_BT_API_MIGRATE("Replaced by sl_bt_scanner_start()")
struct gecko_msg_le_gap_discover_rsp_t* gecko_cmd_le_gap_discover(uint8 mode);

SL_BT_API_MIGRATE("Replaced by sl_bt_scanner_stop()")
struct gecko_msg_le_gap_end_procedure_rsp_t* gecko_cmd_le_gap_end_procedure();

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_set_timing() and sl_bt_advertiser_set_channel_map(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_set_adv_parameters_rsp_t* gecko_cmd_le_gap_set_adv_parameters(uint16 interval_min, uint16 interval_max, uint8 channel_map);

SL_BT_API_MIGRATE("Replaced by sl_bt_le_gap_set_conn_parameters()")
struct gecko_msg_le_gap_set_conn_parameters_rsp_t* gecko_cmd_le_gap_set_conn_parameters(uint16 min_interval, uint16 max_interval, uint16 latency, uint16 timeout);

SL_BT_API_MIGRATE("Replaced by sl_bt_connection_set_default_parameters()")
struct gecko_msg_le_gap_set_scan_parameters_rsp_t* gecko_cmd_le_gap_set_scan_parameters(uint16 scan_interval, uint16 scan_window, uint8 active);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_set_data() and sl_bt_ota_set_advertising_data(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_set_adv_data_rsp_t* gecko_cmd_le_gap_set_adv_data(uint8 scan_rsp, uint8 adv_data_len, const uint8* adv_data_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_set_timing(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_set_adv_timeout_rsp_t* gecko_cmd_le_gap_set_adv_timeout(uint8 maxevents);

SL_BT_API_MIGRATE("Replaced by sl_bt_connection_set_default_preferred_phy()")
struct gecko_msg_le_gap_set_conn_phy_rsp_t* gecko_cmd_le_gap_set_conn_phy(uint8 preferred_phy, uint8 accepted_phy);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_start(), sl_bt_advertiser_stop(), sl_bt_advertiser_set_timing() and sl_bt_advertiser_set_configuration(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_bt5_set_mode_rsp_t* gecko_cmd_le_gap_bt5_set_mode(uint8 handle, uint8 discover, uint8 connect, uint16 maxevents, uint8 address_type);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_set_timing(), sl_bt_advertiser_set_channel_map() and sl_bt_advertiser_set_report_scan_request(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_bt5_set_adv_parameters_rsp_t* gecko_cmd_le_gap_bt5_set_adv_parameters(uint8 handle, uint16 interval_min, uint16 interval_max, uint8 channel_map, uint8 report_scan);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_set_data() and sl_bt_ota_set_advertising_data(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_bt5_set_adv_data_rsp_t* gecko_cmd_le_gap_bt5_set_adv_data(uint8 handle, uint8 scan_rsp, uint8 adv_data_len, const uint8* adv_data_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_gap_set_privacy_mode()")
struct gecko_msg_le_gap_set_privacy_mode_rsp_t* gecko_cmd_le_gap_set_privacy_mode(uint8 privacy, uint8 interval);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_set_timing(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_set_advertise_timing_rsp_t* gecko_cmd_le_gap_set_advertise_timing(uint8 handle, uint32 interval_min, uint32 interval_max, uint16 duration, uint8 maxevents);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_set_channel_map(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_set_advertise_channel_map_rsp_t* gecko_cmd_le_gap_set_advertise_channel_map(uint8 handle, uint8 channel_map);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_set_report_scan_request(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_set_advertise_report_scan_request_rsp_t* gecko_cmd_le_gap_set_advertise_report_scan_request(uint8 handle, uint8 report_scan_req);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_set_phy(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_set_advertise_phy_rsp_t* gecko_cmd_le_gap_set_advertise_phy(uint8 handle, uint8 primary_phy, uint8 secondary_phy);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_set_configuration(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_set_advertise_configuration_rsp_t* gecko_cmd_le_gap_set_advertise_configuration(uint8 handle, uint32 configurations);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_clear_configuration(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_clear_advertise_configuration_rsp_t* gecko_cmd_le_gap_clear_advertise_configuration(uint8 handle, uint32 configurations);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_start(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_start_advertising_rsp_t* gecko_cmd_le_gap_start_advertising(uint8 handle, uint8 discover, uint8 connect);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_stop(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_stop_advertising_rsp_t* gecko_cmd_le_gap_stop_advertising(uint8 handle);

SL_BT_API_MIGRATE("Replaced by sl_bt_scanner_set_timing()")
struct gecko_msg_le_gap_set_discovery_timing_rsp_t* gecko_cmd_le_gap_set_discovery_timing(uint8 phys, uint16 scan_interval, uint16 scan_window);

SL_BT_API_MIGRATE("Replaced by sl_bt_scanner_set_mode()")
struct gecko_msg_le_gap_set_discovery_type_rsp_t* gecko_cmd_le_gap_set_discovery_type(uint8 phys, uint8 scan_type);

SL_BT_API_MIGRATE("Replaced by sl_bt_scanner_start()")
struct gecko_msg_le_gap_start_discovery_rsp_t* gecko_cmd_le_gap_start_discovery(uint8 scanning_phy, uint8 mode);

SL_BT_API_MIGRATE("Replaced by sl_bt_gap_set_data_channel_classification()")
struct gecko_msg_le_gap_set_data_channel_classification_rsp_t* gecko_cmd_le_gap_set_data_channel_classification(uint8 channel_map_len, const uint8* channel_map_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_connection_open()")
struct gecko_msg_le_gap_connect_rsp_t* gecko_cmd_le_gap_connect(bd_addr address, uint8 address_type, uint8 initiating_phy);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_set_tx_power(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_set_advertise_tx_power_rsp_t* gecko_cmd_le_gap_set_advertise_tx_power(uint8 handle, int16 power);

SL_BT_API_MIGRATE("This command is removed. Single event sl_bt_evt_scanner_scan_report is used for advertising reports.")
struct gecko_msg_le_gap_set_discovery_extended_scan_response_rsp_t* gecko_cmd_le_gap_set_discovery_extended_scan_response(uint8 enable);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_start_periodic_advertising(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_start_periodic_advertising_rsp_t* gecko_cmd_le_gap_start_periodic_advertising(uint8 handle, uint16 interval_min, uint16 interval_max, uint32 flags);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_stop_periodic_advertising(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_stop_periodic_advertising_rsp_t* gecko_cmd_le_gap_stop_periodic_advertising(uint8 handle);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_set_long_data(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_set_long_advertising_data_rsp_t* gecko_cmd_le_gap_set_long_advertising_data(uint8 handle, uint8 packet_type);

SL_BT_API_MIGRATE("Replaced by sl_bt_gap_enable_whitelisting()")
struct gecko_msg_le_gap_enable_whitelisting_rsp_t* gecko_cmd_le_gap_enable_whitelisting(uint8 enable);

SL_BT_API_MIGRATE("Replaced by sl_bt_connection_set_default_parameters()")
struct gecko_msg_le_gap_set_conn_timing_parameters_rsp_t* gecko_cmd_le_gap_set_conn_timing_parameters(uint16 min_interval, uint16 max_interval, uint16 latency, uint16 timeout, uint16 min_ce_length, uint16 max_ce_length);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_set_random_address(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_set_advertise_random_address_rsp_t* gecko_cmd_le_gap_set_advertise_random_address(uint8 handle, uint8 addr_type, bd_addr address);

SL_BT_API_MIGRATE("Replaced by sl_bt_advertiser_clear_random_address(). Make sure the advertising handle is already obtained with sl_bt_advertiser_create_set() prior to call any advertiser commands.")
struct gecko_msg_le_gap_clear_advertise_random_address_rsp_t* gecko_cmd_le_gap_clear_advertise_random_address(uint8 handle);

SL_BT_API_MIGRATE("Replaced by sl_bt_sync_open()")
struct gecko_msg_sync_open_rsp_t* gecko_cmd_sync_open(uint8 adv_sid, uint16 skip, uint16 timeout, bd_addr address, uint8 address_type);

SL_BT_API_MIGRATE("Replaced by sl_bt_sync_close()")
struct gecko_msg_sync_close_rsp_t* gecko_cmd_sync_close(uint8 sync);

SL_BT_API_MIGRATE("Replaced by sl_bt_connection_set_parameters()")
struct gecko_msg_le_connection_set_parameters_rsp_t* gecko_cmd_le_connection_set_parameters(uint8 connection, uint16 min_interval, uint16 max_interval, uint16 latency, uint16 timeout);

SL_BT_API_MIGRATE("Replaced by sl_bt_connection_get_rssi()")
struct gecko_msg_le_connection_get_rssi_rsp_t* gecko_cmd_le_connection_get_rssi(uint8 connection);

SL_BT_API_MIGRATE("Replaced by sl_bt_connection_disable_slave_latency()")
struct gecko_msg_le_connection_disable_slave_latency_rsp_t* gecko_cmd_le_connection_disable_slave_latency(uint8 connection, uint8 disable);

SL_BT_API_MIGRATE("Replaced by sl_bt_connection_set_preferred_phy()")
struct gecko_msg_le_connection_set_phy_rsp_t* gecko_cmd_le_connection_set_phy(uint8 connection, uint8 phy);

SL_BT_API_MIGRATE("Replaced by sl_bt_connection_close()")
struct gecko_msg_le_connection_close_rsp_t* gecko_cmd_le_connection_close(uint8 connection);

SL_BT_API_MIGRATE("Replaced by sl_bt_connection_set_parameters()")
struct gecko_msg_le_connection_set_timing_parameters_rsp_t* gecko_cmd_le_connection_set_timing_parameters(uint8 connection, uint16 min_interval, uint16 max_interval, uint16 latency, uint16 timeout, uint16 min_ce_length, uint16 max_ce_length);

SL_BT_API_MIGRATE("Replaced by sl_bt_connection_read_channel_map()")
struct gecko_msg_le_connection_read_channel_map_rsp_t* gecko_cmd_le_connection_read_channel_map(uint8 connection);

SL_BT_API_MIGRATE("Replaced by sl_bt_connection_set_preferred_phy()")
struct gecko_msg_le_connection_set_preferred_phy_rsp_t* gecko_cmd_le_connection_set_preferred_phy(uint8 connection, uint8 preferred_phy, uint8 accepted_phy);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_set_max_mtu()")
struct gecko_msg_gatt_set_max_mtu_rsp_t* gecko_cmd_gatt_set_max_mtu(uint16 max_mtu);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_discover_primary_services()")
struct gecko_msg_gatt_discover_primary_services_rsp_t* gecko_cmd_gatt_discover_primary_services(uint8 connection);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_discover_primary_services_by_uuid()")
struct gecko_msg_gatt_discover_primary_services_by_uuid_rsp_t* gecko_cmd_gatt_discover_primary_services_by_uuid(uint8 connection, uint8 uuid_len, const uint8* uuid_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_discover_characteristics()")
struct gecko_msg_gatt_discover_characteristics_rsp_t* gecko_cmd_gatt_discover_characteristics(uint8 connection, uint32 service);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_discover_characteristics_by_uuid()")
struct gecko_msg_gatt_discover_characteristics_by_uuid_rsp_t* gecko_cmd_gatt_discover_characteristics_by_uuid(uint8 connection, uint32 service, uint8 uuid_len, const uint8* uuid_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_set_characteristic_notification()")
struct gecko_msg_gatt_set_characteristic_notification_rsp_t* gecko_cmd_gatt_set_characteristic_notification(uint8 connection, uint16 characteristic, uint8 flags);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_discover_descriptors()")
struct gecko_msg_gatt_discover_descriptors_rsp_t* gecko_cmd_gatt_discover_descriptors(uint8 connection, uint16 characteristic);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_read_characteristic_value()")
struct gecko_msg_gatt_read_characteristic_value_rsp_t* gecko_cmd_gatt_read_characteristic_value(uint8 connection, uint16 characteristic);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_read_characteristic_value_by_uuid()")
struct gecko_msg_gatt_read_characteristic_value_by_uuid_rsp_t* gecko_cmd_gatt_read_characteristic_value_by_uuid(uint8 connection, uint32 service, uint8 uuid_len, const uint8* uuid_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_write_characteristic_value()")
struct gecko_msg_gatt_write_characteristic_value_rsp_t* gecko_cmd_gatt_write_characteristic_value(uint8 connection, uint16 characteristic, uint8 value_len, const uint8* value_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_write_characteristic_value_without_response()")
struct gecko_msg_gatt_write_characteristic_value_without_response_rsp_t* gecko_cmd_gatt_write_characteristic_value_without_response(uint8 connection, uint16 characteristic, uint8 value_len, const uint8* value_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_prepare_characteristic_value_write()")
struct gecko_msg_gatt_prepare_characteristic_value_write_rsp_t* gecko_cmd_gatt_prepare_characteristic_value_write(uint8 connection, uint16 characteristic, uint16 offset, uint8 value_len, const uint8* value_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_execute_characteristic_value_write()")
struct gecko_msg_gatt_execute_characteristic_value_write_rsp_t* gecko_cmd_gatt_execute_characteristic_value_write(uint8 connection, uint8 flags);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_send_characteristic_confirmation()")
struct gecko_msg_gatt_send_characteristic_confirmation_rsp_t* gecko_cmd_gatt_send_characteristic_confirmation(uint8 connection);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_read_descriptor_value()")
struct gecko_msg_gatt_read_descriptor_value_rsp_t* gecko_cmd_gatt_read_descriptor_value(uint8 connection, uint16 descriptor);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_write_descriptor_value()")
struct gecko_msg_gatt_write_descriptor_value_rsp_t* gecko_cmd_gatt_write_descriptor_value(uint8 connection, uint16 descriptor, uint8 value_len, const uint8* value_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_find_included_services()")
struct gecko_msg_gatt_find_included_services_rsp_t* gecko_cmd_gatt_find_included_services(uint8 connection, uint32 service);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_read_multiple_characteristic_values()")
struct gecko_msg_gatt_read_multiple_characteristic_values_rsp_t* gecko_cmd_gatt_read_multiple_characteristic_values(uint8 connection, uint8 characteristic_list_len, const uint8* characteristic_list_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_read_characteristic_value_from_offset()")
struct gecko_msg_gatt_read_characteristic_value_from_offset_rsp_t* gecko_cmd_gatt_read_characteristic_value_from_offset(uint8 connection, uint16 characteristic, uint16 offset, uint16 maxlen);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_prepare_characteristic_value_reliable_write()")
struct gecko_msg_gatt_prepare_characteristic_value_reliable_write_rsp_t* gecko_cmd_gatt_prepare_characteristic_value_reliable_write(uint8 connection, uint16 characteristic, uint16 offset, uint8 value_len, const uint8* value_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_server_read_attribute_value()")
struct gecko_msg_gatt_server_read_attribute_value_rsp_t* gecko_cmd_gatt_server_read_attribute_value(uint16 attribute, uint16 offset);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_server_read_attribute_type()")
struct gecko_msg_gatt_server_read_attribute_type_rsp_t* gecko_cmd_gatt_server_read_attribute_type(uint16 attribute);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_server_write_attribute_value()")
struct gecko_msg_gatt_server_write_attribute_value_rsp_t* gecko_cmd_gatt_server_write_attribute_value(uint16 attribute, uint16 offset, uint8 value_len, const uint8* value_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_server_send_user_read_response()")
struct gecko_msg_gatt_server_send_user_read_response_rsp_t* gecko_cmd_gatt_server_send_user_read_response(uint8 connection, uint16 characteristic, uint8 att_errorcode, uint8 value_len, const uint8* value_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_server_send_user_write_response()")
struct gecko_msg_gatt_server_send_user_write_response_rsp_t* gecko_cmd_gatt_server_send_user_write_response(uint8 connection, uint16 characteristic, uint8 att_errorcode);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_server_send_characteristic_notification()")
struct gecko_msg_gatt_server_send_characteristic_notification_rsp_t* gecko_cmd_gatt_server_send_characteristic_notification(uint8 connection, uint16 characteristic, uint8 value_len, const uint8* value_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_server_find_attribute()")
struct gecko_msg_gatt_server_find_attribute_rsp_t* gecko_cmd_gatt_server_find_attribute(uint16 start, uint8 type_len, const uint8* type_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_server_set_capabilities()")
struct gecko_msg_gatt_server_set_capabilities_rsp_t* gecko_cmd_gatt_server_set_capabilities(uint32 caps, uint32 reserved);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_server_set_max_mtu()")
struct gecko_msg_gatt_server_set_max_mtu_rsp_t* gecko_cmd_gatt_server_set_max_mtu(uint16 max_mtu);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_server_get_mtu()")
struct gecko_msg_gatt_server_get_mtu_rsp_t* gecko_cmd_gatt_server_get_mtu(uint8 connection);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_server_enable_capabilities()")
struct gecko_msg_gatt_server_enable_capabilities_rsp_t* gecko_cmd_gatt_server_enable_capabilities(uint32 caps);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_server_disable_capabilities()")
struct gecko_msg_gatt_server_disable_capabilities_rsp_t* gecko_cmd_gatt_server_disable_capabilities(uint32 caps);

SL_BT_API_MIGRATE("Replaced by sl_bt_gatt_server_get_enabled_capabilities()")
struct gecko_msg_gatt_server_get_enabled_capabilities_rsp_t* gecko_cmd_gatt_server_get_enabled_capabilities();

SL_BT_API_MIGRATE("Replaced by sl_bt_system_set_soft_timer()")
struct gecko_msg_hardware_set_soft_timer_rsp_t* gecko_cmd_hardware_set_soft_timer(uint32 time, uint8 handle, uint8 single_shot);

SL_BT_API_MIGRATE("This command is removed. Use Sleeptimer API to get timer counter.")
struct gecko_msg_hardware_get_time_rsp_t* gecko_cmd_hardware_get_time();

SL_BT_API_MIGRATE("Replaced by sl_bt_system_set_lazy_soft_timer()")
struct gecko_msg_hardware_set_lazy_soft_timer_rsp_t* gecko_cmd_hardware_set_lazy_soft_timer(uint32 time, uint32 slack, uint8 handle, uint8 single_shot);

SL_BT_API_MIGRATE("Replaced by sl_bt_nvm_erase_all()")
struct gecko_msg_flash_ps_erase_all_rsp_t* gecko_cmd_flash_ps_erase_all();

SL_BT_API_MIGRATE("Replaced by sl_bt_nvm_save()")
struct gecko_msg_flash_ps_save_rsp_t* gecko_cmd_flash_ps_save(uint16 key, uint8 value_len, const uint8* value_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_nvm_load()")
struct gecko_msg_flash_ps_load_rsp_t* gecko_cmd_flash_ps_load(uint16 key);

SL_BT_API_MIGRATE("Replaced by sl_bt_nvm_erase()")
struct gecko_msg_flash_ps_erase_rsp_t* gecko_cmd_flash_ps_erase(uint16 key);

SL_BT_API_MIGRATE("Replaced by sl_bt_test_dtm_tx()")
struct gecko_msg_test_dtm_tx_rsp_t* gecko_cmd_test_dtm_tx(uint8 packet_type, uint8 length, uint8 channel, uint8 phy);

SL_BT_API_MIGRATE("Replaced by sl_bt_test_dtm_rx()")
struct gecko_msg_test_dtm_rx_rsp_t* gecko_cmd_test_dtm_rx(uint8 channel, uint8 phy);

SL_BT_API_MIGRATE("Replaced by sl_bt_test_dtm_end()")
struct gecko_msg_test_dtm_end_rsp_t* gecko_cmd_test_dtm_end();

SL_BT_API_MIGRATE("Replaced by sl_bt_sm_set_bondable_mode()")
struct gecko_msg_sm_set_bondable_mode_rsp_t* gecko_cmd_sm_set_bondable_mode(uint8 bondable);

SL_BT_API_MIGRATE("Replaced by sl_bt_sm_configure()")
struct gecko_msg_sm_configure_rsp_t* gecko_cmd_sm_configure(uint8 flags, uint8 io_capabilities);

SL_BT_API_MIGRATE("Replaced by sl_bt_sm_store_bonding_configuration()")
struct gecko_msg_sm_store_bonding_configuration_rsp_t* gecko_cmd_sm_store_bonding_configuration(uint8 max_bonding_count, uint8 policy_flags);

SL_BT_API_MIGRATE("Replaced by sl_bt_sm_increase_security()")
struct gecko_msg_sm_increase_security_rsp_t* gecko_cmd_sm_increase_security(uint8 connection);

SL_BT_API_MIGRATE("Replaced by sl_bt_sm_delete_bonding()")
struct gecko_msg_sm_delete_bonding_rsp_t* gecko_cmd_sm_delete_bonding(uint8 bonding);

SL_BT_API_MIGRATE("Replaced by sl_bt_sm_delete_bondings()")
struct gecko_msg_sm_delete_bondings_rsp_t* gecko_cmd_sm_delete_bondings();

SL_BT_API_MIGRATE("Replaced by sl_bt_sm_enter_passkey()")
struct gecko_msg_sm_enter_passkey_rsp_t* gecko_cmd_sm_enter_passkey(uint8 connection, int32 passkey);

SL_BT_API_MIGRATE("Replaced by sl_bt_sm_passkey_confirm()")
struct gecko_msg_sm_passkey_confirm_rsp_t* gecko_cmd_sm_passkey_confirm(uint8 connection, uint8 confirm);

SL_BT_API_MIGRATE("Replaced by sl_bt_sm_set_oob_data()")
struct gecko_msg_sm_set_oob_data_rsp_t* gecko_cmd_sm_set_oob_data(uint8 oob_data_len, const uint8* oob_data_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_sm_list_all_bondings()")
struct gecko_msg_sm_list_all_bondings_rsp_t* gecko_cmd_sm_list_all_bondings();

SL_BT_API_MIGRATE("Replaced by sl_bt_sm_bonding_confirm()")
struct gecko_msg_sm_bonding_confirm_rsp_t* gecko_cmd_sm_bonding_confirm(uint8 connection, uint8 confirm);

SL_BT_API_MIGRATE("Replaced by sl_bt_sm_set_debug_mode()")
struct gecko_msg_sm_set_debug_mode_rsp_t* gecko_cmd_sm_set_debug_mode();

SL_BT_API_MIGRATE("Replaced by sl_bt_sm_set_passkey()")
struct gecko_msg_sm_set_passkey_rsp_t* gecko_cmd_sm_set_passkey(int32 passkey);

SL_BT_API_MIGRATE("Replaced by sl_bt_sm_use_sc_oob()")
struct gecko_msg_sm_use_sc_oob_rsp_t* gecko_cmd_sm_use_sc_oob(uint8 enable);

SL_BT_API_MIGRATE("Replaced by sl_bt_sm_set_sc_remote_oob_data()")
struct gecko_msg_sm_set_sc_remote_oob_data_rsp_t* gecko_cmd_sm_set_sc_remote_oob_data(uint8 oob_data_len, const uint8* oob_data_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_sm_add_to_whitelist()")
struct gecko_msg_sm_add_to_whitelist_rsp_t* gecko_cmd_sm_add_to_whitelist(bd_addr address, uint8 address_type);

SL_BT_API_MIGRATE("Replaced by sl_bt_sm_set_minimum_key_size()")
struct gecko_msg_sm_set_minimum_key_size_rsp_t* gecko_cmd_sm_set_minimum_key_size(uint8 minimum_key_size);

SL_BT_API_MIGRATE("Replaced by sl_bt_coex_set_options()")
struct gecko_msg_coex_set_options_rsp_t* gecko_cmd_coex_set_options(uint32 mask, uint32 options);

SL_BT_API_MIGRATE("Replaced by sl_bt_coex_get_counters()")
struct gecko_msg_coex_get_counters_rsp_t* gecko_cmd_coex_get_counters(uint8 reset);

SL_BT_API_MIGRATE("Replaced by sl_bt_coex_set_parameters()")
struct gecko_msg_coex_set_parameters_rsp_t* gecko_cmd_coex_set_parameters(uint8 priority, uint8 request, uint8 pwm_period, uint8 pwm_dutycycle);

SL_BT_API_MIGRATE("Replaced by sl_bt_coex_set_directional_priority_pulse()")
struct gecko_msg_coex_set_directional_priority_pulse_rsp_t* gecko_cmd_coex_set_directional_priority_pulse(uint8 pulse);

SL_BT_API_MIGRATE("Replaced by sl_bt_cte_transmitter_enable_cte_response()")
struct gecko_msg_cte_transmitter_enable_cte_response_rsp_t* gecko_cmd_cte_transmitter_enable_cte_response(uint8 connection, uint8 cte_types, uint8 switching_pattern_len, const uint8* switching_pattern_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_cte_transmitter_disable_cte_response()")
struct gecko_msg_cte_transmitter_disable_cte_response_rsp_t* gecko_cmd_cte_transmitter_disable_cte_response(uint8 connection);

SL_BT_API_MIGRATE("Replaced by sl_bt_cte_transmitter_start_connectionless_cte()")
struct gecko_msg_cte_transmitter_start_connectionless_cte_rsp_t* gecko_cmd_cte_transmitter_start_connectionless_cte(uint8 adv, uint8 cte_length, uint8 cte_type, uint8 cte_count, uint8 switching_pattern_len, const uint8* switching_pattern_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_cte_transmitter_stop_connectionless_cte()")
struct gecko_msg_cte_transmitter_stop_connectionless_cte_rsp_t* gecko_cmd_cte_transmitter_stop_connectionless_cte(uint8 adv);

SL_BT_API_MIGRATE("Replaced by sl_bt_cte_transmitter_set_dtm_parameters()")
struct gecko_msg_cte_transmitter_set_dtm_parameters_rsp_t* gecko_cmd_cte_transmitter_set_dtm_parameters(uint8 cte_length, uint8 cte_type, uint8 switching_pattern_len, const uint8* switching_pattern_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_cte_transmitter_clear_dtm_parameters()")
struct gecko_msg_cte_transmitter_clear_dtm_parameters_rsp_t* gecko_cmd_cte_transmitter_clear_dtm_parameters();

SL_BT_API_MIGRATE("Replaced by sl_bt_cte_receiver_configure()")
struct gecko_msg_cte_receiver_configure_rsp_t* gecko_cmd_cte_receiver_configure(uint8 flags);

SL_BT_API_MIGRATE("Replaced by sl_bt_cte_receiver_start_iq_sampling()")
struct gecko_msg_cte_receiver_start_iq_sampling_rsp_t* gecko_cmd_cte_receiver_start_iq_sampling(uint8 connection, uint16 interval, uint8 cte_length, uint8 cte_type, uint8 slot_durations, uint8 switching_pattern_len, const uint8* switching_pattern_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_cte_receiver_stop_iq_sampling()")
struct gecko_msg_cte_receiver_stop_iq_sampling_rsp_t* gecko_cmd_cte_receiver_stop_iq_sampling(uint8 connection);

SL_BT_API_MIGRATE("Replaced by sl_bt_cte_receiver_start_connectionless_iq_sampling()")
struct gecko_msg_cte_receiver_start_connectionless_iq_sampling_rsp_t* gecko_cmd_cte_receiver_start_connectionless_iq_sampling(uint8 sync, uint8 slot_durations, uint8 cte_count, uint8 switching_pattern_len, const uint8* switching_pattern_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_cte_receiver_stop_connectionless_iq_sampling()")
struct gecko_msg_cte_receiver_stop_connectionless_iq_sampling_rsp_t* gecko_cmd_cte_receiver_stop_connectionless_iq_sampling(uint8 sync);

SL_BT_API_MIGRATE("Replaced by sl_bt_cte_receiver_set_dtm_parameters()")
struct gecko_msg_cte_receiver_set_dtm_parameters_rsp_t* gecko_cmd_cte_receiver_set_dtm_parameters(uint8 cte_length, uint8 cte_type, uint8 slot_durations, uint8 switching_pattern_len, const uint8* switching_pattern_data);

SL_BT_API_MIGRATE("Replaced by sl_bt_cte_receiver_clear_dtm_parameters()")
struct gecko_msg_cte_receiver_clear_dtm_parameters_rsp_t* gecko_cmd_cte_receiver_clear_dtm_parameters();

#ifdef __cplusplus
}
#endif

#endif
