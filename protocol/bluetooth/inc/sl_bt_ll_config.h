/***************************************************************************//**
 * @brief Bluetooth Link Layer configuration
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

#ifndef SL_BT_BLUETOOTH_LL_PRIORITIES_DEFINED
#define SL_BT_BLUETOOTH_LL_PRIORITIES_DEFINED

typedef struct {
  uint8_t scan_min;
  uint8_t scan_max;
  uint8_t adv_min;
  uint8_t adv_max;
  uint8_t conn_min;
  uint8_t conn_max;
  uint8_t init_min;
  uint8_t init_max;
  uint8_t rail_mapping_offset;
  uint8_t rail_mapping_range;
  uint8_t _reserved;
  uint8_t adv_step;
  uint8_t scan_step;
} sl_bt_bluetooth_ll_priorities;

//Default priority configuration
#define SL_BT_BLUETOOTH_PRIORITIES_DEFAULT { 191, 143, 175, 127, 135, 0, 55, 15, 16, 16, 0, 4, 4 }

#define SL_BT_BLUETOOTH_PA_AUTOMODE 0xff
#endif
