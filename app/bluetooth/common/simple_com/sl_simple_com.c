/***************************************************************************//**
 * @file
 * @brief Simple Communication Interface (UART)
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

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "sl_status.h"
#include "em_common.h"
#include "em_core.h"
#include "sl_uartdrv_instances.h"
#include "sl_app_assert.h"
#include "sl_simple_com.h"
#include "sl_device_init_hfxo_config.h"

// -----------------------------------------------------------------------------
// Helper defines for both EXT and VCOM Uart port usage

#if defined (SL_SIMPLE_COM_UARTDRV_INSTANCE) \
  && (SL_SIMPLE_COM_UARTDRV_INSTANCE == SL_SIMPLE_COM_UARTDRV_INSTANCE_VCOM)
#include "sl_uartdrv_usart_vcom_config.h"
#define SIMPLE_COM_UARTDRV_HANDLE     sl_uartdrv_usart_vcom_handle
#define SL_UARTDRV_USART_BAUDRATE     SL_UARTDRV_USART_VCOM_BAUDRATE
#if SL_UARTDRV_USART_VCOM_PERIPHERAL_NO == 0
#define USART_RX_IRQHandler  USART0_RX_IRQHandler
#define USART_RX_IRQn        USART0_RX_IRQn
#elif SL_UARTDRV_USART_VCOM_PERIPHERAL_NO == 1
#define USART_RX_IRQHandler  USART1_RX_IRQHandler
#define USART_RX_IRQn        USART1_RX_IRQn
#elif SL_UARTDRV_USART_VCOM_PERIPHERAL_NO == 2
#define USART_RX_IRQHandler  USART2_RX_IRQHandler
#define USART_RX_IRQn        USART2_RX_IRQn
#elif SL_UARTDRV_USART_VCOM_PERIPHERAL_NO == 3
#define USART_RX_IRQHandler  USART3_RX_IRQHandler
#define USART_RX_IRQn        USART3_RX_IRQn
#else // SL_UARTDRV_USART_EXP_PERIPHERAL_NO
#error "Unsupported USART peripheral!"
#endif // SL_UARTDRV_USART_VCOM_PERIPHERAL_NO
#elif defined (SL_SIMPLE_COM_UARTDRV_INSTANCE) \
  && (SL_SIMPLE_COM_UARTDRV_INSTANCE == SL_SIMPLE_COM_UARTDRV_INSTANCE_EXP)
#include "sl_uartdrv_usart_exp_config.h"
#define SIMPLE_COM_UARTDRV_HANDLE     sl_uartdrv_usart_exp_handle
#define SL_UARTDRV_USART_BAUDRATE     SL_UARTDRV_USART_EXP_BAUDRATE
#if SL_UARTDRV_USART_EXP_PERIPHERAL_NO == 0
#define USART_RX_IRQHandler  USART0_RX_IRQHandler
#define USART_RX_IRQn        USART0_RX_IRQn
#elif SL_UARTDRV_USART_EXP_PERIPHERAL_NO == 1
#define USART_RX_IRQHandler  USART1_RX_IRQHandler
#define USART_RX_IRQn        USART1_RX_IRQn
#elif SL_UARTDRV_USART_EXP_PERIPHERAL_NO == 2
#define USART_RX_IRQHandler  USART2_RX_IRQHandler
#define USART_RX_IRQn        USART2_RX_IRQn
#elif SL_UARTDRV_USART_EXP_PERIPHERAL_NO == 3
#define USART_RX_IRQHandler  USART3_RX_IRQHandler
#define USART_RX_IRQn        USART3_RX_IRQn
#else // SL_UARTDRV_USART_EXP_PERIPHERAL_NO
#error "Unsupported UART peripheral!"
#endif // SL_UARTDRV_USART_EXP_PERIPHERAL_NO
#else
#error "Invalid UARTDRV instance selected!"
#endif

// calculate the maximum amount of time to wait for UART TX buffer to empty
#define TIME_TO_WAIT_TX_CALLBACK  SL_DEVICE_INIT_HFXO_FREQ / SL_UARTDRV_USART_BAUDRATE * 4

// Uart receive and transmit buffers
static uint8_t rx_buf[SL_SIMPLE_COM_RX_BUF_SIZE] = { 0 };
static uint8_t tx_buf[SL_SIMPLE_COM_TX_BUF_SIZE] = { 0 };

// Internal Uart receive and transmit callback
static void transmit_cb(UARTDRV_Handle_t handle,
                        Ecode_t transferStatus,
                        uint8_t *data,
                        UARTDRV_Count_t transferCount);
static void receive_cb(UARTDRV_Handle_t handle,
                       Ecode_t transferStatus,
                       uint8_t *data,
                       UARTDRV_Count_t transferCount);
// Helper functions to ensure reception
static Ecode_t abort_receive(UARTDRV_Handle_t handle);
static Ecode_t get_tail_buffer(UARTDRV_Buffer_FifoQueue_t *queue,
                               UARTDRV_Buffer_t **buffer);
static Ecode_t dequeue_buffer(UARTDRV_Buffer_FifoQueue_t *queue,
                              UARTDRV_Buffer_t **buffer);
static void disable_receiver(UARTDRV_Handle_t handle);

// -----------------------------------------------------------------------------
// Public functions (API implementation)

/**************************************************************************//**
 * Simple Comm Init.
 *****************************************************************************/
void sl_simple_com_init(void)
{
  // clear RX and TX buffers
  memset(rx_buf, 0, sizeof(rx_buf));
  memset(tx_buf, 0, sizeof(tx_buf));
}

/**************************************************************************//**
 * Uart transmit function
 *
 * Transmits len bytes of data through Uart interface using DMA.
 *
 * @param[out] len Message lenght
 * @param[out] data Message data
 *****************************************************************************/
void sl_simple_com_transmit(uint32_t len, uint8_t *data)
{
  Ecode_t ec;
  // make a copy of the data to be sent to guarantee its integrity until
  // transmission completes
  memcpy((void *)tx_buf, (void *)data, (size_t)len);
  // Transmit data using a non-blocking transmit function
  ec = UARTDRV_Transmit(SIMPLE_COM_UARTDRV_HANDLE,
                        tx_buf,
                        len,
                        transmit_cb);
  sl_app_assert(ECODE_EMDRV_UARTDRV_OK == ec,
                "[E: 0x%04x] Failed to start transmitting\n",
                (int)ec);
}

/**************************************************************************//**
 * Uart receive function
 *
 * Starts reception on Uart interface using DMA.
 *
 * @note The problem with standard uart reception is that it needs the exact
 * amount of bytes to read. This implementation starts a timer to measure idle
 * time on the bus and based on that puts the received message into the buffer.
 *****************************************************************************/
void sl_simple_com_receive(void)
{
  Ecode_t ec;
  CORE_SetNvicRamTableHandler(USART_RX_IRQn,
                              (void *)USART_RX_IRQHandler);
  // Clear pending RX interrupt flag in NVIC
  NVIC_ClearPendingIRQ(USART_RX_IRQn);
  NVIC_EnableIRQ(USART_RX_IRQn);
  // Setup RX timeout to 255 bit-time
  SIMPLE_COM_UARTDRV_HANDLE->peripheral.uart->TIMECMP1 = \
    (USART_TIMECMP1_TSTOP_RXACT
     | USART_TIMECMP1_TSTART_RXEOF
     | (0xff << _USART_TIMECMP1_TCMPVAL_SHIFT));
  // Clear any USART interrupt flags
  USART_IntClear(SIMPLE_COM_UARTDRV_HANDLE->peripheral.uart,
                 _USART_IF_MASK);
  USART_IntEnable(SIMPLE_COM_UARTDRV_HANDLE->peripheral.uart,
                  USART_IF_TXIDLE | USART_IF_TCMP1);
  // Start reception with callback set
  ec = UARTDRV_Receive(SIMPLE_COM_UARTDRV_HANDLE,
                       rx_buf,
                       sizeof(rx_buf),
                       receive_cb);
  sl_app_assert(ECODE_EMDRV_UARTDRV_OK == ec,
                "[E: 0x%04x] Failed to start receiving\n",
                (int)ec);
}

/**************************************************************************//**
 * Uart transmit completed callback
 *
 * Called after Uart transmit is finished.
 *
 * @param[in] status Status of the transmission
 *
 * @note Weak implementation
 *  Called after DMA is complete, the Uart transmission might be still ongoing
 *****************************************************************************/
SL_WEAK void sl_simple_com_transmit_cb(sl_status_t status)
{
  (void)status;
}

/**************************************************************************//**
 * Uart receive completed callback
 *
 * Called after Uart receive is finished.
 *
 * @param[in] status Status of the reception
 * @param[in] len Received message length
 * @param[in] data Data received
 *
 * @note Weak implementation
 *****************************************************************************/
SL_WEAK void sl_simple_com_receive_cb(sl_status_t status,
                                      uint32_t len,
                                      uint8_t *data)
{
  (void)status;
  (void)data;
  (void)len;
}

// -----------------------------------------------------------------------------
// Private functions

/**************************************************************************//**
 * Uart interrupt handler
 *
 * Called when the set timer for tx idle states finished.
 *
 * @note automatically restarts the timer and starts a new reception.
 * @note TODO: this should be provided by UARTDRV
 *****************************************************************************/
void USART_RX_IRQHandler(void)
{
  // RX timeout, stop transfer and handle what we got in buffer
  if (SIMPLE_COM_UARTDRV_HANDLE->peripheral.uart->IF & USART_IF_TCMP1) {
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();
    // stop the timer
    SIMPLE_COM_UARTDRV_HANDLE->peripheral.uart->TIMECMP1 &= \
      ~_USART_TIMECMP1_TSTART_MASK;
    SIMPLE_COM_UARTDRV_HANDLE->peripheral.uart->TIMECMP1 |= \
      USART_TIMECMP1_TSTART_RXEOF;
    // clear timer interrupt
    USART_IntClear(SIMPLE_COM_UARTDRV_HANDLE->peripheral.uart,
                   USART_IF_TCMP1);
    // abort receive operation
    (void)abort_receive(SIMPLE_COM_UARTDRV_HANDLE);
    sl_simple_com_receive();
    CORE_EXIT_ATOMIC();
  }
}

/**************************************************************************//**
 * Internal Uart transmit completed callback
 *
 * Called after Uart transmit is finished.
 *
 * @param[in] handle Connection handle
 * @param[in] transferStatus Status of the transfer
 * @param[in] data Transmitted data
 * @param[in] transferCount Number of sent bytes
 *
 * @note Calls public sl_simple_com_transmit_cb
 *  Called after DMA is complete, the Uart transmission might be still ongoing
 *****************************************************************************/
static void transmit_cb(UARTDRV_Handle_t handle,
                        Ecode_t transferStatus,
                        uint8_t *data,
                        UARTDRV_Count_t transferCount)
{
  uint16_t timeout = 0;
  (void)data;
  (void)transferCount;

  // wait until UART finished transfer
  while ((!(UARTDRV_GetPeripheralStatus(handle) & UARTDRV_STATUS_TXC))
         && (timeout < TIME_TO_WAIT_TX_CALLBACK)) {
    timeout++;
  }
  // Call public callback API
  sl_simple_com_transmit_cb(ECODE_EMDRV_UARTDRV_OK == transferStatus
                            ? SL_STATUS_OK : SL_STATUS_FAIL);
  // Clear TX buffer
  memset(tx_buf, 0, sizeof(tx_buf));
}

/**************************************************************************//**
 * Internal Uart receive completed callback
 *
 * Called after Uart receive is finished.
 *
 * @param[in] handle Connection handle
 * @param[in] transferStatus Status of the transfer
 * @param[in] data Received data
 * @param[in] transferCount Number of received bytes
 *
 * @note Calls public sl_simple_com_receive_cb
 *****************************************************************************/
static void receive_cb(UARTDRV_Handle_t handle,
                       Ecode_t transferStatus,
                       uint8_t *data,
                       UARTDRV_Count_t transferCount)
{
  (void)handle;
  // Call public callback API
  sl_simple_com_receive_cb(ECODE_EMDRV_UARTDRV_OK == transferStatus
                           ? SL_STATUS_OK : SL_STATUS_FAIL,
                           transferCount,
                           data);
  // Clear RX buffer
  memset(rx_buf, 0, sizeof(rx_buf));
}

/**************************************************************************//**
 * Aborted reception handler
 *
 * @param[in] handle Connection handle
 *****************************************************************************/
static Ecode_t abort_receive(UARTDRV_Handle_t handle)
{
  UARTDRV_Buffer_t *rxBuffer;
  Ecode_t status;
  CORE_DECLARE_IRQ_STATE;

  if (handle == NULL) {
    return ECODE_EMDRV_UARTDRV_ILLEGAL_HANDLE;
  }

  CORE_ENTER_ATOMIC();
  if (handle->rxQueue->used == 0) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_UARTDRV_IDLE;
  }

  // -------------------------------
  // Stop the current transfer
  (void)DMADRV_StopTransfer(handle->rxDmaCh);
  handle->rxDmaActive = false;
  // Update the transfer status of the active transfer
  status = get_tail_buffer(handle->rxQueue, &rxBuffer);
  // If an abort was in progress when DMA completed, the ISR could be deferred
  // until after the critical section. In this case, the buffers no longer
  // exist, even though the DMA complete callback was called.
  if (status == ECODE_EMDRV_UARTDRV_QUEUE_EMPTY) {
    return ECODE_EMDRV_UARTDRV_QUEUE_EMPTY;
  }
  EFM_ASSERT(rxBuffer != NULL);
  (void)DMADRV_TransferRemainingCount(handle->rxDmaCh,
                                      (int *)&rxBuffer->itemsRemaining);
  rxBuffer->transferStatus = ECODE_EMDRV_UARTDRV_ABORTED;

  // -------------------------------
  // Dequeue all transfers and call callback
  while (handle->rxQueue->used > 0) {
    (void)dequeue_buffer(handle->rxQueue, &rxBuffer);
    // Call the callback
    if (rxBuffer->callback != NULL) {
      if (rxBuffer->callback != NULL) {
        rxBuffer->callback(handle,
                           ECODE_EMDRV_UARTDRV_OK,
                           rxBuffer->data,
                           rxBuffer->transferCount - rxBuffer->itemsRemaining);
      }
    }
  }

  // -------------------------------
  // Disable the receiver
  if (handle->fcType != uartdrvFlowControlHwUart) {
    disable_receiver(handle);
  }
  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_UARTDRV_OK;
}

/**************************************************************************//**
 * Gets the buffer tail.
 *
 * @param[in] queue Input buffer
 * @param[out] buffer Output buffer
 *****************************************************************************/
static Ecode_t get_tail_buffer(UARTDRV_Buffer_FifoQueue_t *queue,
                               UARTDRV_Buffer_t **buffer)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (queue->used == 0) {
    *buffer = NULL;
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_UARTDRV_QUEUE_EMPTY;
  }
  *buffer = &queue->fifo[queue->tail];

  CORE_EXIT_ATOMIC();
  return ECODE_EMDRV_UARTDRV_OK;
}

/**************************************************************************//**
 * Dequeues buffer
 *
 * Moves through the buffer.
 *
 * @param[in] queue Input buffer
 * @param[out] buffer Output buffer
 *****************************************************************************/
static Ecode_t dequeue_buffer(UARTDRV_Buffer_FifoQueue_t *queue,
                              UARTDRV_Buffer_t **buffer)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (queue->used == 0) {
    *buffer = NULL;
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_UARTDRV_QUEUE_EMPTY;
  }
  *buffer = &queue->fifo[queue->tail];
  queue->tail = (queue->tail + 1) % queue->size;
  queue->used--;
  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_UARTDRV_OK;
}

/**************************************************************************//**
 * Disables receiver.
 *
 * @param[in] handle Connection handle
 *****************************************************************************/
static void disable_receiver(UARTDRV_Handle_t handle)
{
#if (defined(LEUART_COUNT) && (LEUART_COUNT > 0) \
  && !defined(_SILICON_LABS_32B_SERIES_2))       \
  || (defined(EUART_COUNT) && (EUART_COUNT > 0) )
  if (handle->type == uartdrvUartTypeUart)
#endif
  {
    // Disable Rx route
    #if defined(USART_ROUTEPEN_RXPEN)
    handle->peripheral.uart->ROUTEPEN &= ~USART_ROUTEPEN_RXPEN;
    #elif defined(USART_ROUTE_RXPEN)
    handle->peripheral.uart->ROUTE &= ~USART_ROUTE_RXPEN;
    #elif defined(GPIO_USART_ROUTEEN_RXPEN)
    GPIO->USARTROUTE_CLR[handle->uartNum].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN;
    #endif
    // Disable Rx
    handle->peripheral.uart->CMD = USART_CMD_RXDIS;
  }
#if defined(LEUART_COUNT) && (LEUART_COUNT > 0) \
  && !defined(_SILICON_LABS_32B_SERIES_2)
  else if (handle->type == uartdrvUartTypeLeuart) {
    // Wait for prevous register writes to sync
    while ((handle->peripheral.leuart->SYNCBUSY & LEUART_SYNCBUSY_CMD) != 0U) {
    }

    // Disable Rx route
    #if defined(LEUART_ROUTEPEN_RXPEN)
    handle->peripheral.leuart->ROUTEPEN &= ~LEUART_ROUTEPEN_RXPEN;
    #else
    handle->peripheral.leuart->ROUTE &= ~LEUART_ROUTE_RXPEN;
    #endif
    // Disable Rx
    handle->peripheral.leuart->CMD = LEUART_CMD_RXDIS;
  }
#elif defined(EUART_COUNT) && (EUART_COUNT > 0)
  else if (handle->type == uartdrvUartTypeEuart) {
    if (EUSART_StatusGet(handle->peripheral.euart) &  EUSART_STATUS_TXENS) {
      EUSART_Enable(handle->peripheral.euart, eusartEnableTx);
    } else {
      EUSART_Enable(handle->peripheral.euart, eusartDisable);
    }
  }
#endif
}
