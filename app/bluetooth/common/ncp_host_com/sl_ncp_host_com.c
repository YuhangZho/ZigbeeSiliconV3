/***************************************************************************//**
 * @file
 * @brief Bluetooth Network Co-Processor (NCP) Host Communication Interface
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
#include <stdbool.h>
#include "em_core.h"
#include "sl_bt_ncp_host.h"
#include "sl_simple_com.h"
#include "sl_ncp_host_com.h"
#include "sl_component_catalog.h"
#if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
#include "sl_wake_lock.h"
#endif // SL_CATALOG_WAKE_LOCK_PRESENT

// Library definition for adaptation layer
// Adaptation layer is used to pass information to SL_BT_API protocol
SL_BT_API_DEFINE();

// Uart reception buffer
typedef struct {
  uint16_t len;
  uint8_t buf[SL_NCP_HOST_COM_BUF_SIZE];
  bool available;
} buf_t;

static volatile bool write_completed = false;
static buf_t buf = { 0 };

/**************************************************************************//**
 * NCP host communication initialization.
 *****************************************************************************/
void sl_ncp_host_com_init(void)
{
  // Initialize simple uart communication with DMA.
  sl_simple_com_init();
  buf.len = 0;
  // Register communication interface functions in adaptation layer
  SL_BT_API_INITIALIZE_NONBLOCK(sl_ncp_host_com_write,
                                sl_ncp_host_com_read,
                                sl_ncp_host_com_peek);
}

/**************************************************************************//**
 * Transmit function
 *
 * Transmits len bytes of data from adaptation layer through Uart interface.
 *
 * @param[out] len Message lenght
 * @param[out] data Message data
 *
 * @note After transmit the reception is automatically started.
 *****************************************************************************/
void sl_ncp_host_com_write(uint32_t len, uint8_t *data)
{
  write_completed = false;
  #if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
  // Wake up other controller
  sl_wake_lock_set_remote_req();
  #endif // SL_CATALOG_WAKE_LOCK_PRESENT
  sl_simple_com_transmit(len, data);
  while (!write_completed) ;
  // Start to receive the response as soon as the transmit is completed
  sl_simple_com_receive();
}

/**************************************************************************//**
 * Receive function
 *
 * Copies received data from Uart interface to adaptation layer
 *
 * @param[out] len Message lenght
 * @param[out] data Message data
 *
 * @return Received message length
 *****************************************************************************/
int32_t sl_ncp_host_com_read(uint32_t len, uint8_t *data)
{
  (void)data;
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  // Check if there is data in the buffer from Uart
  if (len <= buf.len) {
    // Copy data to adaptation layer
    memcpy((void *)data, (void *)buf.buf, (size_t)len);
    buf.len -= len;
    memmove((void *)buf.buf, (void *)&buf.buf[len], buf.len);
  } else {
    len = -1;
  }
  CORE_EXIT_ATOMIC();
  return len;
}

/**************************************************************************//**
 * Gives back already received message length.
 *
 * This function checks if data arrived from Uart interface. This way the calls
 * can be non blocking.
 *
 * @param[out] len Message lenght
 * @param[out] data Message data
 *
 * @return Buffer length
 *****************************************************************************/
int32_t sl_ncp_host_com_peek(void)
{
  return buf.len;
}

/**************************************************************************//**
 * Uart transmit completed callback
 *
 * Called after Uart transmit is finished.
 *
 * @param[in] status Status of the transmission
 *****************************************************************************/
void sl_simple_com_transmit_cb(sl_status_t status)
{
  (void)status;

  #if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
  // Signal other controller that it can go to sleep
  sl_wake_lock_clear_remote_req();
  #endif // SL_CATALOG_WAKE_LOCK_PRESENT

  write_completed = true;
}

/**************************************************************************//**
 * Uart receive completed callback
 *
 * Called after Uart receive is finished. Puts the message to the reception
 * buffer.
 *
 * @param[in] status Status of the reception
 * @param[in] len Received message length
 * @param[in] data Data received
 *****************************************************************************/
void sl_simple_com_receive_cb(sl_status_t status,
                              uint32_t len,
                              uint8_t *data)
{
  (void)status;
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  // command fits into command buffer; otherwise discard it
  if (len <= (sizeof(buf.buf) - buf.len)) {
    memcpy((void *)&buf.buf[buf.len], (void *)data, (size_t)len);
    buf.len += len;
  }
  CORE_EXIT_ATOMIC();
}

bool sl_ncp_host_is_ok_to_sleep(void)
{
  if (buf.len != 0) {
    return false;
  } else {
    return true;
  }
}
