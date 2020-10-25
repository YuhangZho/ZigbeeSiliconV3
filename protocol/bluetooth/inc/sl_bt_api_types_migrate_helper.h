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

#ifndef SL_BT_API_TYPES_MIGRATE_HELPER_H
#define SL_BT_API_TYPES_MIGRATE_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

#error BGLIB_MSG_ID is replaced by SL_BT_MSG_ID
#define BGLIB_MSG_ID(HDR) ((HDR) & 0xffff00f8)

#error BGLIB_MSG_HEADER_LEN is replaced by SL_BT_MSG_HEADER_LEN
#define BGLIB_MSG_HEADER_LEN (4)

#error BGLIB_MSG_LEN is replaced by SL_BT_MSG_LEN
#define BGLIB_MSG_LEN(HDR) ((((HDR) & 0x7) << 8) | (((HDR) & 0xff00) >> 8))

#error gecko_evt_dfu_boot_id is replaced by sl_bt_evt_dfu_boot_id
#define gecko_evt_dfu_boot_id                             (((uint32)0x20) | 0x80 | 0x00000000)

#error gecko_evt_dfu_boot_failure_id is replaced by sl_bt_evt_dfu_boot_failure_id
#define gecko_evt_dfu_boot_failure_id                     (((uint32)0x20) | 0x80 | 0x01000000)

#error gecko_evt_system_boot_id is replaced by sl_bt_evt_system_boot_id
#define gecko_evt_system_boot_id                          (((uint32)0x20) | 0x80 | 0x00010000)

#error gecko_evt_system_external_signal_id is replaced by sl_bt_evt_system_external_signal_id
#define gecko_evt_system_external_signal_id               (((uint32)0x20) | 0x80 | 0x03010000)

#error gecko_evt_system_awake_id is replaced by sl_bt_evt_system_awake_id
#define gecko_evt_system_awake_id                         (((uint32)0x20) | 0x80 | 0x04010000)

#error gecko_evt_system_hardware_error_id is replaced by sl_bt_evt_system_hardware_error_id
#define gecko_evt_system_hardware_error_id                (((uint32)0x20) | 0x80 | 0x05010000)

#error gecko_evt_system_error_id is replaced by sl_bt_evt_system_error_id
#define gecko_evt_system_error_id                         (((uint32)0x20) | 0x80 | 0x06010000)

#error gecko_evt_le_gap_scan_response_id is replaced by sl_bt_evt_scanner_scan_report_id
#define gecko_evt_le_gap_scan_response_id                 (((uint32)0x20) | 0x80 | 0x00030000)

#error gecko_evt_le_gap_adv_timeout_id is replaced by sl_bt_evt_advertiser_timeout_id
#define gecko_evt_le_gap_adv_timeout_id                   (((uint32)0x20) | 0x80 | 0x01030000)

#error gecko_evt_le_gap_scan_request_id is replaced by sl_bt_evt_advertiser_scan_request_id
#define gecko_evt_le_gap_scan_request_id                  (((uint32)0x20) | 0x80 | 0x02030000)

#error gecko_evt_le_gap_extended_scan_response_id is replaced by sl_bt_evt_scanner_scan_report_id
#define gecko_evt_le_gap_extended_scan_response_id        (((uint32)0x20) | 0x80 | 0x04030000)

#error gecko_evt_sync_opened_id is replaced by sl_bt_evt_sync_opened_id
#define gecko_evt_sync_opened_id                          (((uint32)0x20) | 0x80 | 0x00420000)

#error gecko_evt_sync_closed_id is replaced by sl_bt_evt_sync_closed_id
#define gecko_evt_sync_closed_id                          (((uint32)0x20) | 0x80 | 0x01420000)

#error gecko_evt_sync_data_id is replaced by sl_bt_evt_sync_data_id
#define gecko_evt_sync_data_id                            (((uint32)0x20) | 0x80 | 0x02420000)

#error gecko_evt_le_connection_opened_id is replaced by sl_bt_evt_connection_opened_id
#define gecko_evt_le_connection_opened_id                 (((uint32)0x20) | 0x80 | 0x00080000)

#error gecko_evt_le_connection_closed_id is replaced by sl_bt_evt_connection_closed_id
#define gecko_evt_le_connection_closed_id                 (((uint32)0x20) | 0x80 | 0x01080000)

#error gecko_evt_le_connection_parameters_id is replaced by sl_bt_evt_connection_parameters_id
#define gecko_evt_le_connection_parameters_id             (((uint32)0x20) | 0x80 | 0x02080000)

#error gecko_evt_le_connection_rssi_id is replaced by sl_bt_evt_connection_rssi_id
#define gecko_evt_le_connection_rssi_id                   (((uint32)0x20) | 0x80 | 0x03080000)

#error gecko_evt_le_connection_phy_status_id is replaced by sl_bt_evt_connection_phy_status_id
#define gecko_evt_le_connection_phy_status_id             (((uint32)0x20) | 0x80 | 0x04080000)

#error gecko_evt_gatt_mtu_exchanged_id is replaced by sl_bt_evt_gatt_mtu_exchanged_id
#define gecko_evt_gatt_mtu_exchanged_id                   (((uint32)0x20) | 0x80 | 0x00090000)

#error gecko_evt_gatt_service_id is replaced by sl_bt_evt_gatt_service_id
#define gecko_evt_gatt_service_id                         (((uint32)0x20) | 0x80 | 0x01090000)

#error gecko_evt_gatt_characteristic_id is replaced by sl_bt_evt_gatt_characteristic_id
#define gecko_evt_gatt_characteristic_id                  (((uint32)0x20) | 0x80 | 0x02090000)

#error gecko_evt_gatt_descriptor_id is replaced by sl_bt_evt_gatt_descriptor_id
#define gecko_evt_gatt_descriptor_id                      (((uint32)0x20) | 0x80 | 0x03090000)

#error gecko_evt_gatt_characteristic_value_id is replaced by sl_bt_evt_characteristic_value_id
#define gecko_evt_gatt_characteristic_value_id            (((uint32)0x20) | 0x80 | 0x04090000)

#error gecko_evt_gatt_descriptor_value_id is replaced by sl_bt_evt_gatt_descriptor_value_id
#define gecko_evt_gatt_descriptor_value_id                (((uint32)0x20) | 0x80 | 0x05090000)

#error gecko_evt_gatt_procedure_completed_id is replaced by sl_bt_evt_gatt_procedure_completed_id
#define gecko_evt_gatt_procedure_completed_id             (((uint32)0x20) | 0x80 | 0x06090000)

#error gecko_evt_gatt_server_attribute_value_id is replaced by sl_bt_evt_gatt_server_attribute_value_id
#define gecko_evt_gatt_server_attribute_value_id          (((uint32)0x20) | 0x80 | 0x000a0000)

#error gecko_evt_gatt_server_user_read_request_id is replaced by sl_bt_evt_gatt_server_user_read_request_id
#define gecko_evt_gatt_server_user_read_request_id        (((uint32)0x20) | 0x80 | 0x010a0000)

#error gecko_evt_gatt_server_user_write_request_id is replaced by sl_bt_evt_gatt_server_user_write_request_id
#define gecko_evt_gatt_server_user_write_request_id       (((uint32)0x20) | 0x80 | 0x020a0000)

#error gecko_evt_gatt_server_characteristic_status_id is replaced by sl_bt_evt_gatt_server_characteristic_status_id
#define gecko_evt_gatt_server_characteristic_status_id    (((uint32)0x20) | 0x80 | 0x030a0000)

#error gecko_evt_gatt_server_execute_write_completed_id is replaced by sl_bt_evt_gatt_server_execute_write_completed_id
#define gecko_evt_gatt_server_execute_write_completed_id  (((uint32)0x20) | 0x80 | 0x040a0000)

#error gecko_evt_hardware_soft_timer_id is replaced by sl_bt_evt_system_soft_timer_id
#define gecko_evt_hardware_soft_timer_id                  (((uint32)0x20) | 0x80 | 0x000c0000)

#error gecko_evt_test_dtm_completed_id is replaced by sl_bt_evt_test_dtm_completed_id
#define gecko_evt_test_dtm_completed_id                   (((uint32)0x20) | 0x80 | 0x000e0000)

#error gecko_evt_sm_passkey_display_id is replaced by sl_bt_evt_sm_passkey_display_id
#define gecko_evt_sm_passkey_display_id                   (((uint32)0x20) | 0x80 | 0x000f0000)

#error gecko_evt_sm_passkey_request_id is replaced by sl_bt_evt_sm_passkey_request_id
#define gecko_evt_sm_passkey_request_id                   (((uint32)0x20) | 0x80 | 0x010f0000)

#error gecko_evt_sm_confirm_passkey_id is replaced by sl_bt_evt_sm_confirm_passkey_id
#define gecko_evt_sm_confirm_passkey_id                   (((uint32)0x20) | 0x80 | 0x020f0000)

#error gecko_evt_sm_bonded_id is replaced by sl_bt_evt_sm_bonded_id
#define gecko_evt_sm_bonded_id                            (((uint32)0x20) | 0x80 | 0x030f0000)

#error gecko_evt_sm_bonding_failed_id is replaced by sl_bt_evt_sm_bonding_failed_id
#define gecko_evt_sm_bonding_failed_id                    (((uint32)0x20) | 0x80 | 0x040f0000)

#error gecko_evt_sm_list_bonding_entry_id is replaced by sl_bt_evt_sm_list_bonding_entry_id
#define gecko_evt_sm_list_bonding_entry_id                (((uint32)0x20) | 0x80 | 0x050f0000)

#error gecko_evt_sm_list_all_bondings_complete_id is replaced by sl_bt_evt_sm_list_all_bondings_complete_id
#define gecko_evt_sm_list_all_bondings_complete_id        (((uint32)0x20) | 0x80 | 0x060f0000)

#error gecko_evt_sm_confirm_bonding_id is replaced by sl_bt_evt_sm_confirm_bonding_id
#define gecko_evt_sm_confirm_bonding_id                   (((uint32)0x20) | 0x80 | 0x090f0000)

#error gecko_evt_cte_receiver_iq_report_id is replaced by sl_bt_evt_cte_receiver_iq_report_id
#define gecko_evt_cte_receiver_iq_report_id               (((uint32)0x20) | 0x80 | 0x00450000)

#error gecko_evt_user_message_to_host_id is replaced by sl_bt_evt_user_message_to_host_id
#define gecko_evt_user_message_to_host_id                 (((uint32)0x20) | 0x80 | 0x00ff0000)

#error gecko_msg_dfu_boot_evt_t is replaced by sl_bt_evt_dfu_boot_t
struct gecko_msg_dfu_boot_evt_t;

#error gecko_msg_dfu_boot_failure_evt_t is replaced by sl_bt_evt_boot_failure_t
struct gecko_msg_dfu_boot_failure_evt_t;

#error gecko_msg_system_boot_evt_t is replaced by sl_bt_evt_system_boot_t
struct gecko_msg_system_boot_evt_t;

#error gecko_msg_system_external_signal_evt_t is replaced by sl_bt_evt_system_external_signal_t
struct gecko_msg_system_external_signal_evt_t;

#error gecko_msg_system_hardware_error_evt_t is replaced by sl_bt_evt_system_hardware_error_t
struct gecko_msg_system_hardware_error_evt_t;

#error gecko_msg_system_error_evt_t is replaced by sl_bt_evt_system_error_t
struct gecko_msg_system_error_evt_t;

#error gecko_msg_le_gap_scan_response_evt_t is replaced by sl_bt_evt_scanner_scan_report_t
struct gecko_msg_le_gap_scan_response_evt_t;

#error gecko_msg_le_gap_adv_timeout_evt_t is replaced by sl_bt_evt_advertiser_timeout_t
struct gecko_msg_le_gap_adv_timeout_evt_t;

#error gecko_msg_le_gap_scan_request_evt_t is replaced by sl_bt_evt_advertiser_scan_request_t
struct gecko_msg_le_gap_scan_request_evt_t;

#error gecko_msg_le_gap_extended_scan_response_evt_t is replaced by sl_bt_evt_scanner_scan_report_t
struct gecko_msg_le_gap_extended_scan_response_evt_t;

#error gecko_msg_sync_opened_evt_t is replaced by sl_bt_evt__sync_opened_t
struct gecko_msg_sync_opened_evt_t;

#error gecko_msg_sync_closed_evt_t is replaced by sl_bt_evt__sync_closed_t
struct gecko_msg_sync_closed_evt_t;

#error gecko_msg_sync_data_evt_t is replaced by sl_bt_evt__sync_data_t
struct gecko_msg_sync_data_evt_t;

#error gecko_msg_le_connection_opened_evt_t is replaced by sl_bt_evt_connection_opened_t
struct gecko_msg_le_connection_opened_evt_t;

#error gecko_msg_le_connection_closed_evt_t is replaced by sl_bt_evt_connection_closed_t
struct gecko_msg_le_connection_closed_evt_t;

#error gecko_msg_le_connection_parameters_evt_t is replaced by sl_bt_evt_connection_parameters_t
struct gecko_msg_le_connection_parameters_evt_t;

#error gecko_msg_le_connection_rssi_evt_t is replaced by sl_bt_evt_connection_rssi_t
struct gecko_msg_le_connection_rssi_evt_t;

#error gecko_msg_le_connection_phy_status_evt_t is replaced by sl_bt_evt_connection_phy_status_t
struct gecko_msg_le_connection_phy_status_evt_t;

#error gecko_msg_gatt_mtu_exchanged_evt_t is replaced by sl_bt_evt_
struct gecko_msg_gatt_mtu_exchanged_evt_t;

#error gecko_msg_gatt_service_evt_t is replaced by sl_bt_evt_gatt_service_t
struct gecko_msg_gatt_service_evt_t;

#error gecko_msg_gatt_characteristic_evt_t is replaced by sl_bt_evt_gatt_characteristic_t
struct gecko_msg_gatt_characteristic_evt_t;

#error gecko_msg_gatt_descriptor_evt_t is replaced by sl_bt_evt_gatt_descriptor_t
struct gecko_msg_gatt_descriptor_evt_t;

#error gecko_msg_gatt_characteristic_value_evt_t is replaced by sl_bt_evt_gatt_characteristic_value_t
struct gecko_msg_gatt_characteristic_value_evt_t;

#error gecko_msg_gatt_descriptor_value_evt_t is replaced by sl_bt_evt_gatt_descriptor_value_t
struct gecko_msg_gatt_descriptor_value_evt_t;

#error gecko_msg_gatt_procedure_completed_evt_t is replaced by sl_bt_evt_gatt_procedure_completed_t
struct gecko_msg_gatt_procedure_completed_evt_t;

#error gecko_msg_gatt_server_attribute_value_evt_t is replaced by sl_bt_evt_gatt_server_attribute_value_t
struct gecko_msg_gatt_server_attribute_value_evt_t;

#error gecko_msg_gatt_server_user_read_request_evt_t is replaced by sl_bt_evt_gatt_server_user_read_request_t
struct gecko_msg_gatt_server_user_read_request_evt_t;

#error gecko_msg_gatt_server_user_write_request_evt_t is replaced by sl_bt_evt_gatt_server_user_write_request_t
struct gecko_msg_gatt_server_user_write_request_evt_t;

#error gecko_msg_gatt_server_characteristic_status_evt_t is replaced by sl_bt_evt_gatt_server_characteristic_status_t
struct gecko_msg_gatt_server_characteristic_status_evt_t;

#error gecko_msg_gatt_server_execute_write_completed_evt_t is replaced by sl_bt_evt_gatt_server_execute_write_completed_t
struct gecko_msg_gatt_server_execute_write_completed_evt_t;

#error gecko_msg_hardware_soft_timer_evt_t is replaced by sl_bt_evt_system_soft_timer_t
struct gecko_msg_hardware_soft_timer_evt_t;

#error gecko_msg_test_dtm_completed_evt_t is replaced by sl_bt_evt_test_dtm_completed_t
struct gecko_msg_test_dtm_completed_evt_t;

#error gecko_msg_sm_passkey_display_evt_t is replaced by sl_bt_evt_sm_passkey_display_t
struct gecko_msg_sm_passkey_display_evt_t;

#error gecko_msg_sm_passkey_request_evt_t is replaced by sl_bt_evt_sm_passkey_request_t
struct gecko_msg_sm_passkey_request_evt_t;

#error gecko_msg_sm_confirm_passkey_evt_t is replaced by sl_bt_evt_sm_confirm_passkey_t
struct gecko_msg_sm_confirm_passkey_evt_t;

#error gecko_msg_sm_bonded_evt_t is replaced by sl_bt_evt_sm_bonded_t
struct gecko_msg_sm_bonded_evt_t;

#error gecko_msg_sm_bonding_failed_evt_t is replaced by sl_bt_evt_sm_bonding_failed_t
struct gecko_msg_sm_bonding_failed_evt_t;

#error gecko_msg_sm_list_bonding_entry_evt_t is replaced by sl_bt_evt_sm_list_bonding_entry_t
struct gecko_msg_sm_list_bonding_entry_evt_t;

#error gecko_msg_sm_confirm_bonding_evt_t is replaced by sl_bt_evt_sm_confirm_bonding_t
struct gecko_msg_sm_confirm_bonding_evt_t;

#error gecko_msg_cte_receiver_iq_report_evt_t is replaced by sl_bt_evt_cte_receiver_iq_report_t
struct gecko_msg_cte_receiver_iq_report_evt_t;

#error gecko_msg_user_message_to_host_evt_t is replaced by sl_bt_evt_user_message_to_host_t
struct gecko_msg_user_message_to_host_evt_t;

#ifdef __cplusplus
}
#endif

#endif
