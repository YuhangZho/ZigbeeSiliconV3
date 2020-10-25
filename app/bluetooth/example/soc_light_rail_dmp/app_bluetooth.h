/***************************************************************************//**
 * @file
 * @brief Application interface.
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
#ifndef APP_BLUETOOTH_H
#define APP_BLUETOOTH_H

#include "os.h"
#include "sl_bt_types.h"

typedef enum {
  DEMO_EVT_NONE                     = 0x00,
  DEMO_EVT_BOOTED                   = 0x01,
  DEMO_EVT_BLUETOOTH_CONNECTED      = 0x02,
  DEMO_EVT_BLUETOOTH_DISCONNECTED   = 0x03,
  DEMO_EVT_RAIL_READY               = 0x04,
  DEMO_EVT_RAIL_ADVERTISE           = 0x05,
  DEMO_EVT_LIGHT_CHANGED_BLUETOOTH  = 0x06,
  DEMO_EVT_LIGHT_CHANGED_RAIL       = 0x07,
  DEMO_EVT_INDICATION               = 0x08,
  DEMO_EVT_INDICATION_SUCCESSFUL    = 0x09,
  DEMO_EVT_INDICATION_FAILED        = 0x0A,
  DEMO_EVT_BUTTON0_PRESSED          = 0x0B,
  DEMO_EVT_BUTTON1_PRESSED          = 0x0C,
  DEMO_EVT_CLEAR_DIRECTION          = 0x0D
} demo_msg_t;

typedef enum {
  demo_light_off = 0,
  demo_light_on  = 1
} demo_light_t;

typedef enum {
  PROP_STATUS_SEND                  = 0x00,
  PROP_TIMER_EXPIRED                = 0x01,
  PROP_TOGGLE_MODE                  = 0x02,
  PROP_TOGGLE_RXD                   = 0x03
} prop_msg_t;

typedef enum {
  DEMO_STATE_INIT       = 0x00,
  DEMO_STATE_READY      = 0x01
} demo_state_t;

typedef enum {
  demo_light_direction_bluetooth   = 0,
  demo_light_direction_proprietary = 1,
  demo_light_direction_button      = 2,
  demo_light_direction_invalid     = 3
} demo_light_direction_t;

typedef struct {
  uint8_t addr[8];
} demo_light_src_addr_t;

typedef struct {
  demo_state_t state;
  demo_light_t light;
  gatt_client_config_flag_t light_ind;
  demo_light_direction_t direction;
  gatt_client_config_flag_t direction_ind;
  demo_light_src_addr_t src_addr;
  gatt_client_config_flag_t src_addr_ind;
  uint8_t conn_bluetooth_in_use;
  uint8_t conn_proprietary_in_use;
  bool indication_ongoing;
  bool indication_pending;
  demo_light_src_addr_t own_addr;
} demo_t;

extern void light_pend(RTOS_ERR *err);
extern void light_post(RTOS_ERR *err);
extern void demo_queue_post(demo_msg_t msg, RTOS_ERR *err);
extern void init_demo_app(void);
extern demo_t demo;

#endif // APP_BLUETOOTH_H
