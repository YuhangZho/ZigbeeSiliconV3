/*
 *  Copyright (c) 2017, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction for UART communication.
 *
 */

#include <stddef.h>
#include <string.h>
#include "openthread-system.h"
#include <openthread/platform/uart.h>
#include "utils/code_utils.h"
#include "em_core.h"
#include "uartdrv.h"
#include "sl_uartdrv_instances.h"

enum
{
    kReceiveFifoSize = 128,
};

// In order to reduce the probability of data loss due to disabled interrupts, we use
// two duplicate receive buffers so we can always have one "active" receive request.
#define RECEIVE_BUFFER_SIZE 128
static uint8_t              sReceiveBuffer1[RECEIVE_BUFFER_SIZE];
static uint8_t              sReceiveBuffer2[RECEIVE_BUFFER_SIZE];
static const uint8_t *      sTransmitBuffer = NULL;
static volatile uint16_t    sTransmitLength = 0;

typedef struct ReceiveFifo_t
{
    // The data buffer
    uint8_t mBuffer[kReceiveFifoSize];
    // The offset of the first item written to the list.
    volatile uint16_t mHead;
    // The offset of the next item to be written to the list.
    volatile uint16_t mTail;
} ReceiveFifo_t;

static ReceiveFifo_t sReceiveFifo;

static void processReceive(void);

static void receiveDone(UARTDRV_Handle_t aHandle, Ecode_t aStatus, uint8_t *aData, UARTDRV_Count_t aCount)
{
    OT_UNUSED_VARIABLE(aStatus);

    // We can only write if incrementing mTail doesn't equal mHead
    if (sReceiveFifo.mHead != (sReceiveFifo.mTail + aCount) % kReceiveFifoSize)
    {
        memcpy(sReceiveFifo.mBuffer + sReceiveFifo.mTail, aData, aCount);
        sReceiveFifo.mTail                       = (sReceiveFifo.mTail + aCount) % kReceiveFifoSize;
    }

    UARTDRV_Receive(aHandle, aData, aCount, receiveDone);
    otSysEventSignalPending();
}

static void transmitDone(UARTDRV_Handle_t aHandle, Ecode_t aStatus, uint8_t *aData, UARTDRV_Count_t aCount)
{
    OT_UNUSED_VARIABLE(aHandle);
    OT_UNUSED_VARIABLE(aStatus);
    OT_UNUSED_VARIABLE(aData);
    OT_UNUSED_VARIABLE(aCount);

    sTransmitLength = 0;
    otSysEventSignalPending();
}

static void processReceive(void)
{
    // Copy tail to prevent multiple reads
    uint16_t tail = sReceiveFifo.mTail;

    // If the data wraps around, process the first part
    if (sReceiveFifo.mHead > tail)
    {
        otPlatUartReceived(sReceiveFifo.mBuffer + sReceiveFifo.mHead, kReceiveFifoSize - sReceiveFifo.mHead);

        // Reset the buffer mHead back to zero.
        sReceiveFifo.mHead = 0;
    }

    // For any data remaining, process it
    if (sReceiveFifo.mHead != tail)
    {
        otPlatUartReceived(sReceiveFifo.mBuffer + sReceiveFifo.mHead, tail - sReceiveFifo.mHead);

        // Set mHead to the local tail we have cached
        sReceiveFifo.mHead = tail;
    }
}

otError otPlatUartFlush(void)
{
    return OT_ERROR_NOT_IMPLEMENTED;
}

static void processTransmit(void)
{
    if (sTransmitBuffer != NULL && sTransmitLength == 0)
    {
        sTransmitBuffer = NULL;
        otPlatUartSendDone();
    }
}

otError otPlatUartEnable(void)
{
    sReceiveFifo.mHead = 0;
    sReceiveFifo.mTail = 0;
    sl_uartdrv_init_instances();

    // When one receive request is completed, the other buffer is used for a separate receive request, issued immediately.
    for (uint8_t i = 0; i < RECEIVE_BUFFER_SIZE; i++) {
        UARTDRV_Receive(sl_uartdrv_usart_vcom_handle, &sReceiveBuffer1[i], sizeof(sReceiveBuffer1[i]), receiveDone);
        UARTDRV_Receive(sl_uartdrv_usart_vcom_handle, &sReceiveBuffer2[i], sizeof(sReceiveBuffer2[i]), receiveDone);
    }

    return OT_ERROR_NONE;
}

otError otPlatUartDisable(void)
{
    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    otError error = OT_ERROR_NONE;

    otEXPECT_ACTION(sTransmitBuffer == NULL, error = OT_ERROR_BUSY);

    sTransmitBuffer = aBuf;
    sTransmitLength = aBufLength;

    UARTDRV_Transmit(sl_uartdrv_usart_vcom_handle, (uint8_t *)sTransmitBuffer, sTransmitLength, transmitDone);

exit:
    return error;
}

void efr32UartProcess(void)
{
    processReceive();
    processTransmit();
}
