/***************************************************************************//**
 * @file
 * @brief Proprietary application logic.
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
#include "os.h"
#include "rail.h"
#include "sl_rail_util_init.h"
#include "sl_app_assert.h"
#include "app_bluetooth.h"
#include "printf.h"
#include "rtos_err.h"

// Proprietary Application task
#define PROPRIETARY_APP_TASK_PRIO         6u
#define PROPRIETARY_APP_TASK_STACK_SIZE   (2000 / sizeof(CPU_STK))
#define PACKET_HEADER_LEN (2)
#define PROP_RX_BUF_SIZE (256)
#define LIGHT_CONTROL_DATA_BYTE (0x0F)

#define DEMO_CONTROL_PAYLOAD_CMD_DATA                  (0x0F)
#define DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT              (0x80)
#define DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT_SHIFT        (7)
#define DEMO_CONTROL_PAYLOAD_CMD_MASK                  (0x70)
#define DEMO_CONTROL_PAYLOAD_CMD_MASK_SHIFT            (4)

typedef enum {
  DEMO_CONTROL_CMD_ADVERTISE = 0,
  DEMO_CONTROL_CMD_LIGHT_TOGGLE = 1,
  DEMO_CONTROL_CMD_LIGHT_STATE_REPORT = 2,
  DEMO_CONTROL_CMD_LIGHT_STATE_GET = 3,
} demo_control_command_t;

typedef enum {
  DEMO_CONTROL_ROLE_LIGHT = 0,
  DEMO_CONTROL_ROLE_SWITCH = 1,
} demo_control_role_t;

typedef enum {
  PROP_STATE_ADVERTISE              = 0x00,
  PROP_STATE_READY                  = 0x01
} prop_state_t;

typedef struct {
  prop_state_t state;
} proprietary_t;

proprietary_t proprietary = {
  .state = PROP_STATE_ADVERTISE
};

typedef enum {
  PROP_PKT_ADVERTISE                = 0x00,
  PROP_PKT_STATUS                   = 0x01,
} prop_pkt;

static CPU_STK proprietary_app_task_stk[PROPRIETARY_APP_TASK_STACK_SIZE];
static OS_TCB  proprietary_app_task_tcb;
static void    proprietary_app_task(void *p_arg);
static OS_TMR  proprietary_timer;
static OS_Q    proprietary_queue;

static uint8_t data_packet[] =
{
  0x0F, 0x16, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
  0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0x00
};

static uint8_t proprietary_rx_buf[PROP_RX_BUF_SIZE];
static void proprietary_timer_callback(void *p_tmr, void *p_arg);
/// Contains the last RAIL Rx/Tx error events
static volatile uint64_t current_rail_err = 0;
/// Contains the status of RAIL Calibration
static volatile RAIL_Status_t calibration_status = 0;

static prop_msg_t proprietary_queue_pend(RTOS_ERR *err);

void init_prop_app(void);

void proprietary_queue_post(prop_msg_t msg, RTOS_ERR *err);
static prop_msg_t proprietary_queue_pend(RTOS_ERR *err);

void init_prop_app(void)
{
  RTOS_ERR err;

  OSTmrCreate(&proprietary_timer,         /* Pointer to user-allocated timer. */
              "Proprietary Timer",       /* Name used for debugging.         */
              0,                         /* 0 initial delay.                 */
              10,                        /* 100 Timer Ticks period.          */
              OS_OPT_TMR_PERIODIC,       /* Timer is periodic.               */
              &proprietary_timer_callback, /* Called when timer expires.       */
              DEF_NULL,                  /* No arguments to callback.        */
              &err);

  OSQCreate((OS_Q     *)&proprietary_queue,
            (CPU_CHAR *)"Proprietary Queue",
            (OS_MSG_QTY) 32,
            (RTOS_ERR *)&err);

  // Create the Proprietary Application task
  OSTaskCreate(&proprietary_app_task_tcb,
               "Proprietary App Task",
               proprietary_app_task,
               0u,
               PROPRIETARY_APP_TASK_PRIO,
               &proprietary_app_task_stk[0u],
               (PROPRIETARY_APP_TASK_STACK_SIZE / 10u),
               PROPRIETARY_APP_TASK_STACK_SIZE,
               0u,
               0u,
               0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &err);
}

static prop_msg_t proprietary_queue_pend(RTOS_ERR *err)
{
  prop_msg_t propMsg;
  OS_MSG_SIZE propMsgSize;
  propMsg = (prop_msg_t)OSQPend((OS_Q *)&proprietary_queue,
                                (OS_TICK) 0,
                                (OS_OPT) OS_OPT_PEND_BLOCKING,
                                (OS_MSG_SIZE *)&propMsgSize,
                                (CPU_TS *) DEF_NULL,
                                (RTOS_ERR *) err);

  sl_app_assert(err->Code == RTOS_ERR_NONE,
                "[E: 0x%04x] OS error proprietary_queue_pend\n",
                (int)err->Code);

  return propMsg;
}

/**************************************************************************//**
 * Proprietary post queue.
 *
 * @param msg message type
 * @param err error message
 *
 * Post error message
 *****************************************************************************/
void proprietary_queue_post(prop_msg_t msg, RTOS_ERR *err)
{
  OSQPost((OS_Q *)&proprietary_queue,
          (void *) msg,
          (OS_MSG_SIZE) sizeof(void*),
          (OS_OPT) OS_OPT_POST_FIFO + OS_OPT_POST_ALL,
          (RTOS_ERR *) err);

  sl_app_assert(err->Code == RTOS_ERR_NONE,
                "[E: 0x%04x] OS error proprietary_queue_post\n",
                (int)err->Code);
}

/**************************************************************************//**
 * Proprietary timer callback.
 *
 * @param p_tmr is pointer to the user-allocated timer.
 * @param p_arg is argument passed when creating the timer.
 *
 * Called when timer expires
 *****************************************************************************/
static void proprietary_timer_callback(void *p_tmr, void *p_arg)
{
  PP_UNUSED_PARAM(p_tmr);
  PP_UNUSED_PARAM(p_arg);
  RTOS_ERR err;

  proprietary_queue_post(PROP_TIMER_EXPIRED, &err);
}

/**************************************************************************//**
 * Proprietary packet send.
 *
 * @param pktType Packet type
 *
 * Sends a packet using RAIL. The data shall be in data_packet
 *****************************************************************************/
static void proprietaryTxPacket(prop_pkt pktType)
{
  RAIL_SchedulerInfo_t schedulerInfo;
  RAIL_Status_t res;
  RAIL_Handle_t rail_handle = sl_rail_util_get_handle();

  // This assumes the Tx time is around 200us
  schedulerInfo = (RAIL_SchedulerInfo_t){ .priority = 100,
                                          .slipTime = 100000,
                                          .transactionTime = 200 };

  // address of light
  Mem_Copy((void *)&data_packet[PACKET_HEADER_LEN],
           (void *)demo.own_addr.addr,
           sizeof(demo.own_addr.addr));
  // light role
  data_packet[LIGHT_CONTROL_DATA_BYTE] &= ~DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT;
  data_packet[LIGHT_CONTROL_DATA_BYTE] |= (DEMO_CONTROL_ROLE_LIGHT << DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT_SHIFT)
                                          & DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT;
  // advertisement packet
  if (PROP_PKT_ADVERTISE == pktType) {
    data_packet[LIGHT_CONTROL_DATA_BYTE] &= ~DEMO_CONTROL_PAYLOAD_CMD_MASK;
    data_packet[LIGHT_CONTROL_DATA_BYTE] |= ((uint8_t)DEMO_CONTROL_CMD_ADVERTISE << DEMO_CONTROL_PAYLOAD_CMD_MASK_SHIFT)
                                            & DEMO_CONTROL_PAYLOAD_CMD_MASK;
    // status packet
  } else if (PROP_PKT_STATUS == pktType) {
    data_packet[LIGHT_CONTROL_DATA_BYTE] &= ~DEMO_CONTROL_PAYLOAD_CMD_MASK;
    data_packet[LIGHT_CONTROL_DATA_BYTE] |= ((uint8_t)DEMO_CONTROL_CMD_LIGHT_STATE_REPORT << DEMO_CONTROL_PAYLOAD_CMD_MASK_SHIFT)
                                            & DEMO_CONTROL_PAYLOAD_CMD_MASK;
    data_packet[LIGHT_CONTROL_DATA_BYTE] &= ~0x01;
    data_packet[LIGHT_CONTROL_DATA_BYTE] |= (uint8_t)demo.light;
  } else {
  }

  RAIL_WriteTxFifo((RAIL_Handle_t)rail_handle, data_packet, sizeof(data_packet), true);

  res = RAIL_StartTx((RAIL_Handle_t)rail_handle,
                     0,
                     RAIL_TX_OPTIONS_DEFAULT,
                     &schedulerInfo);

  if (res != RAIL_STATUS_NO_ERROR) {
    // Try once to resend the packet 100ms later in case of error
    RAIL_ScheduleTxConfig_t scheduledTxConfig = { .when = RAIL_GetTime() + 100000,
                                                  .mode = RAIL_TIME_ABSOLUTE };

    // Transmit this packet at the specified time or up to 50 ms late
    res = RAIL_StartScheduledTx((RAIL_Handle_t)sl_rail_util_get_handle(),
                                0,
                                RAIL_TX_OPTIONS_DEFAULT,
                                &scheduledTxConfig,
                                &schedulerInfo);
  }
}

/**************************************************************************//**
 * Proprietary Application task.
 *
 * @param p_arg Pointer to an optional data area which can pass parameters to
 *              the task when the task executes.
 *
 * This is a minimal Proprietary Application task that only configures the
 * radio.
 *****************************************************************************/
static void proprietary_app_task(void *p_arg)
{
  PP_UNUSED_PARAM(p_arg);
  RTOS_ERR err;
  prop_msg_t propMsg;

  RAIL_Handle_t rail_handle = sl_rail_util_get_handle();

  // Start reception
  RAIL_StartRx((RAIL_Handle_t)rail_handle, 0, NULL);

  proprietary.state = PROP_STATE_ADVERTISE;
  OSTmrStart(&proprietary_timer, &err);
  sl_app_assert(err.Code == RTOS_ERR_NONE,
                "[E: 0x%04x] OS Timer error\n",
                (int)err.Code);

  while (DEF_TRUE) {
    // pending on proprietary message queue
    propMsg = proprietary_queue_pend(&err);

    switch (proprietary.state) {
      case PROP_STATE_ADVERTISE:
        switch (propMsg) {
          case PROP_TIMER_EXPIRED:
            proprietaryTxPacket(PROP_PKT_ADVERTISE);
            break;

          case PROP_TOGGLE_MODE:
            OSTmrStop(&proprietary_timer, OS_OPT_TMR_NONE, DEF_NULL, &err);
            sl_app_assert(err.Code == RTOS_ERR_NONE,
                          "[E: 0x%04x] OS Timer error\n",
                          (int)err.Code);
            demo_queue_post(DEMO_EVT_RAIL_READY, &err);
            proprietary.state = PROP_STATE_READY;
            break;

          default:
            break;
        }
        break;

      case PROP_STATE_READY:
        switch (propMsg) {
          case PROP_STATUS_SEND:
            proprietaryTxPacket(PROP_PKT_STATUS);
            break;

          case PROP_TOGGLE_MODE:
            OSTmrStart(&proprietary_timer, &err);
            sl_app_assert(err.Code == RTOS_ERR_NONE,
                          "[E: 0x%04x] OS Timer error\n",
                          (int)err.Code);
            demo_queue_post(DEMO_EVT_RAIL_ADVERTISE, &err);
            proprietary.state = PROP_STATE_ADVERTISE;
            break;

          case PROP_TOGGLE_RXD:
            light_pend(&err);
            if (demo.light == demo_light_off) {
              demo.light = demo_light_on;
            } else {
              demo.light = demo_light_off;
            }
            light_post(&err);
            demo_queue_post(DEMO_EVT_LIGHT_CHANGED_RAIL, &err);
            break;

          default:
            break;
        }
        break;
    }
  }
}

/******************************************************************************
 * RAIL callback, called if a RAIL event occurs.
 *****************************************************************************/
void sl_rail_app_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  RTOS_ERR err;
  demo_control_role_t role;
  demo_control_command_t command;

  //----------------- RX --------------------------
  //----------------- RX --------------------------
  //----------------- RX --------------------------
  // Handle Rx events
  if ( events & RAIL_EVENTS_RX_COMPLETION ) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      RAIL_RxPacketInfo_t packet_info;

      RAIL_GetRxPacketInfo(rail_handle,
                           RAIL_RX_PACKET_HANDLE_NEWEST,
                           &packet_info);

      if ((packet_info.packetStatus != RAIL_RX_PACKET_READY_SUCCESS)
          && (packet_info.packetStatus != RAIL_RX_PACKET_READY_CRC_ERROR)) {
        // RAIL_EVENT_RX_PACKET_RECEIVED must be handled last in order to return
        // early on aborted packets here.
        return;
      }

      // Read packet data into our packet structure
      uint16_t length = packet_info.packetBytes;
      Mem_Copy(proprietary_rx_buf,
               packet_info.firstPortionData,
               packet_info.firstPortionBytes);
      Mem_Copy(proprietary_rx_buf + packet_info.firstPortionBytes,
               packet_info.lastPortionData,
               length - packet_info.firstPortionBytes);

      // accept packets in ready mode only
      if (PROP_STATE_READY == proprietary.state) {
        // packet sent by switch
        role = (demo_control_role_t)((proprietary_rx_buf[DEMO_CONTROL_PAYLOAD_CMD_DATA] & DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT) >> DEMO_CONTROL_PAYLOAD_SRC_ROLE_BIT_SHIFT);
        if (DEMO_CONTROL_ROLE_SWITCH == role) {
          // TODO: handle only those packets that include the Light's address
          if (DEF_YES == Mem_Cmp((void *)demo.own_addr.addr,
                                 (void *)&proprietary_rx_buf[PACKET_HEADER_LEN],
                                 sizeof(demo.own_addr.addr))) {
            // packet contains toggle command
            command = (demo_control_command_t)((proprietary_rx_buf[DEMO_CONTROL_PAYLOAD_CMD_DATA] & DEMO_CONTROL_PAYLOAD_CMD_MASK) >> DEMO_CONTROL_PAYLOAD_CMD_MASK_SHIFT);
            if (DEMO_CONTROL_CMD_LIGHT_TOGGLE == command) {
              proprietary_queue_post(PROP_TOGGLE_RXD, &err);
            }
          }
        } else {
          // TODO: Anything app specific when something else is received
        }
      }
    }//if (events & RAIL_EVENT_RX_PACKET_RECEIVED)
  }//if ( events & RAIL_EVENTS_RX_COMPLETION )

  //----------------- TX --------------------------
  //----------------- TX --------------------------
  //----------------- TX --------------------------
  // Handle Tx events
  if ( events & RAIL_EVENTS_TX_COMPLETION) {
    if ((events & RAIL_EVENT_TX_PACKET_SENT)
        | (events & RAIL_EVENT_TX_ABORTED)
        | (events & RAIL_EVENT_TX_BLOCKED)
        | (events & RAIL_EVENT_TX_UNDERFLOW)
        | (events & RAIL_EVENT_TX_CHANNEL_BUSY)
        | (events & RAIL_EVENT_SCHEDULER_STATUS)) {
      // nothing to do for these events
    }

    RAIL_YieldRadio(rail_handle);
  }//if ( events & RAIL_EVENTS_TX_COMPLETION)

  // Perform all calibrations when needed
  if ( events & RAIL_EVENT_CAL_NEEDED ) {
    calibration_status = RAIL_Calibrate(rail_handle, NULL, RAIL_CAL_ALL_PENDING);
    if (calibration_status != RAIL_STATUS_NO_ERROR) {
      current_rail_err = (events & RAIL_EVENT_CAL_NEEDED);
    }
  }
}
