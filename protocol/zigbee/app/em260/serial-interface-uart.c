/***************************************************************************//**
 * @file
 * @brief Implementation of the interface described in serial-interface.h
 * using the ASH UART protocol.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include PLATFORM_HEADER

#include "stack/include/ember.h"
#include "stack/include/error.h"

#include "hal/hal.h"
#include "stack/framework/debug.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/util/ezsp/ezsp-frame-utilities.h"
#include "em260-callbacks.h"
#include "serial/serial.h"
#include "serial-interface.h"
#include "hal/micro/generic/ash-protocol.h"
#include "hal/micro/generic/ash-ncp.h"
#include "stack/include/mfglib.h"
#include "serial-interface-uart-config.h"
const uint8_t ashPort = ASH_PORT;
//------------------------------------------------------------------------------
// Global Variables

static EmberMessageBuffer ezspBuffer = EMBER_NULL_MESSAGE_BUFFER;

// This points to contents of ezspBuffer.
//
// When receiving a command or sending a response, ezspBuffer is the buffer
// statically allocated by ASH.  These are allocated first thing and are
// guaranteed to be contiguous.  When sending a callback, ezspBuffer is
// a freshly allocated buffer, but for callbacks we only write within the
// first 32 bytes, so the lack of contiguity is not a concern.
uint8_t *ezspFrameContents;

static uint16_t delayTestStartTime;
static uint16_t delayTestDelayLength = 0;

//------------------------------------------------------------------------------
// Serial Interface Downwards

void serialInit(void)
{
  EmberStatus status;
  SerialBaudRate baud;
  baud = ashReadConfigOrDefault(baudRate, ASH_BAUD_RATE);
#ifdef CORTEXM3_EFR32_MICRO
  COM_Init_t initData = (COM_Init_t) ASH_COM_INIT;
  #ifdef ASH_USART
  initData.uartdrvinit.uartinit.baudRate = baud;
  #elif defined (ASH_LEUART)
  initData.uartdrvinit.leuartinit.baudRate = baud;
  #endif
  status = COM_Init((COM_Port_t) ashPort, &initData);
#else
  status = emberSerialInit(ashPort, baud, PARITY_NONE, 1);
#endif
  assert(status == EMBER_SUCCESS);
  ashStart();
}

void serialAllocateBuffers(void)
{
  ezspBuffer = ashAllocateStaticBuffers();
  emberHoldMessageBuffer(ezspBuffer);
  ezspFrameContents = emberMessageBufferContents(ezspBuffer);
}

EmberMessageBuffer serialGetEzspBuffer(void)
{
  return ezspBuffer;
}

bool serialCommandReceived(void)
{
  ashSendExec();
  if (ezspBuffer != EMBER_NULL_MESSAGE_BUFFER) {
    return true;
  }
  if (delayTestDelayLength == 0
      || (elapsedTimeInt16u(delayTestStartTime,
                            halCommonGetInt16uMillisecondTick())
          > delayTestDelayLength)) {
    delayTestDelayLength = 0;
    ashReceive(&ezspBuffer);
  }
  if (ezspBuffer != EMBER_NULL_MESSAGE_BUFFER) {
    ezspFrameContents = emberMessageBufferContents(ezspBuffer);
    return true;
  } else {
    return false;
  }
}

void serialResponseReady(bool highPriority)
{
  serialDebugTrace(); // Trace responses before randomization!
  ashSend(ezspBuffer, highPriority);
  ezspBuffer = EMBER_NULL_MESSAGE_BUFFER;
  ashSendExec();
}

void serialDebugTrace(void)
{
  if (ezspBuffer != EMBER_NULL_MESSAGE_BUFFER) {
    emDebugBinaryFormat(EM_DEBUG_EZSP, "b", ezspBuffer);
  }
}

EmberMessageBuffer serialFillBufferFromCommand(uint8_t startIndex,
                                               uint8_t length)
{
  EmberMessageBuffer buffer;
  buffer = emberFillLinkedBuffers(NULL, length);
  if (buffer != EMBER_NULL_MESSAGE_BUFFER) {
    emberCopyBufferBytes(buffer, 0, ezspBuffer, startIndex, length);
  }
  return buffer;
}

uint8_t serialGetCommandLength(void)
{
  return emberMessageBufferLength(ezspBuffer);
}

void serialSetResponseLength(uint8_t data)
{
  emberSetMessageBufferLength(ezspBuffer, data);
}

bool serialCallbackResponse(void)
{
  Buffer callback = emBufferQueueRemoveHead(&callbackQueue);
  uint8_t callbackLength = emberMessageBufferLength(callback);

  ezspBuffer = callback;

#ifdef EMBER_AF_PLUGIN_SECURE_EZSP
  if (emSecureEzspIsOn()) {
    emberSetLinkedBuffersLength(ezspBuffer,
                                emberMessageBufferLength(ezspBuffer)
                                + SECURE_EZSP_OVERHEAD_LENGTH);
  }
#endif //EMBER_AF_PLUGIN_SECURE_EZSP

  ezspFrameContents = emberMessageBufferContents(ezspBuffer);
  ezspWritePointer = ezspFrameContents + callbackLength;
  return true;
}

void serialMfglibFillPacket(uint8_t *packet)
{
  uint8_t length = emberGetLinkedBuffersByte(ezspBuffer, EZSP_PARAMETERS_INDEX);
  emberCopyFromLinkedBuffers(ezspBuffer,
                             EZSP_PARAMETERS_INDEX,
                             packet,
                             length + 1);
}

void serialDelayTest(uint16_t delay)
{
  delayTestStartTime = halCommonGetInt16uMillisecondTick();
  delayTestDelayLength = delay;
}

void serialPendingCallbacks(bool pending)
{
}
