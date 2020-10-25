/***************************************************************************//**
 * @file
 * @brief This callback file is created for your convenience. You may add application
 * code to this file.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
#include CONFIGURATION_HEADER
#include EMBER_AF_API_EMBER_TYPES
#include "app/util/ezsp/ezsp-enum.h"
#include "app/xncp/xncp-sample-custom-ezsp-protocol.h"
#include EMBER_AF_API_HAL
#include EMBER_AF_API_ZIGBEE_PRO
#include EMBER_AF_API_STACK
#include "stack/include/gp-types.h"

#if (defined EMBER_AF_PLUGIN_MULTIRAIL_DEMO)

#include EMBER_AF_API_MULTIRAIL_DEMO

// If defined and the local packet queue is empty, a received packet will trigger
// sending a queued packet from the global queue.
#define USE_MULTIRAIL_HANDLE_TO_SEND_GP_STUB_PACKET

#ifndef EMBER_APPL_GP_BIDIRECTIONAL_TX_QUEUE_SIZE
  #define EMBER_APPL_GP_BIDIRECTIONAL_TX_QUEUE_SIZE 1
#endif

#ifndef GP_RX_OFFSET_USEC
  #define GP_RX_OFFSET_USEC 20500
#endif

extern uint8_t sl_mac_lower_mac_get_radio_channel(uint8_t mac_index);
extern bool emGpAddressMatch(const EmberGpAddress *a1, const EmberGpAddress *a2);
extern EmberStatus emberAfPluginXncpSendCustomEzspMessage(uint8_t length, uint8_t *payload);

//Outgoing Bi-directional GPDF queue
static EmberGpTxQueueEntry emAppGpTxQueue[EMBER_APPL_GP_BIDIRECTIONAL_TX_QUEUE_SIZE];

// Frees one entry
static void emAppFreeGpTxQueueEntry(EmberGpTxQueueEntry* entry)
{
  if (entry->inUse) {   // avoid trying to free the buffer after a reset
    emberReleaseMessageBuffer(entry->asdu);
    entry->inUse = false;
  }
  entry->asdu = EMBER_NULL_MESSAGE_BUFFER;
  MEMSET(&(entry->addr), 0xFF, sizeof(EmberGpAddress));
  entry->queueEntryLifetimeMs = 0;
}

// Resets the entire queue
static void emAppInitGpTxQueue(void)
{
  for (int i = 0; i < EMBER_APPL_GP_BIDIRECTIONAL_TX_QUEUE_SIZE; i++) {
    EmberGpTxQueueEntry* entry = &(emAppGpTxQueue[i]);
    emAppFreeGpTxQueueEntry(entry);
  }
}

#if (defined USE_MULTIRAIL_HANDLE_TO_SEND_GP_STUB_PACKET)
// Gets an entry from the Gp Stub queue if available and RAIL 2 is ready
static EmberGpTxQueueEntry* emAppGetGpStubTxQueue(EmberGpAddress* addr)
{
  extern EmberGpTxQueueEntry emGpTxQueue;
  extern EmberMessageBuffer emGpdfMakeHeader(bool useCca,
                                             EmberGpAddress *src,
                                             EmberGpAddress *dst);
  extern void emClearGpTxQueue(void);
  // Steal the GP Queue to send it through the additional RAIL handle
  // Check if RAIL 2 handle available and there is anything in GP Stub queue
  if (emberAfPluginMultirailDemoGetHandle()
      && emGpTxQueue.inUse
      && emGpAddressMatch(addr, &(emGpTxQueue.addr))) {
    // Allocate a buffer and prepare a outgoing MAC header using gpd address in the emGpTxQueue
    EmberMessageBuffer header = emGpdfMakeHeader(true, NULL, &(emGpTxQueue.addr));
    // Add the command Id from the queue to the buffer
    uint8_t len = emberMessageBufferLength(header) + 1;
    emberAppendToLinkedBuffers(header, &(emGpTxQueue.gpdCommandId), 1);
    // Copy Command Payload from the queue to the buffer and update the length
    emberSetLinkedBuffersLength(header,
                                emberMessageBufferLength(header)
                                + emberMessageBufferLength(emGpTxQueue.asdu));
    emberCopyBufferBytes(header,
                         len,
                         emGpTxQueue.asdu,
                         0,
                         emberMessageBufferLength(emGpTxQueue.asdu));
    // Clear the Stub queue because everything is serialised in header
    emClearGpTxQueue();
    // Prepare a RAIL frame to be transported using the additional handle
    uint8_t outPktLength = emberMessageBufferLength(header);
    uint8_t outPkt[128]; //128 = MAX size
    // RAIL Frame : [Total Length (excludes itself) | <-----MAC FRAME ---->| 2 byte CRC]
    outPkt[0] = outPktLength + 2;
    // Copy the data from the buffer
    emberCopyFromLinkedBuffers(header,
                               0,
                               &outPkt[1],
                               outPktLength);
    // Free the header as the rail frame will be submitted with a new buffer asdu
    emberReleaseMessageBuffer(header);

    // This entry is exempt from marking (see emberAfMarkBuffersCallback below),
    // since it is allocated and released within the context of the same function
    // call (appGpScheduleOutgoingGpdf).
    static EmberGpTxQueueEntry copyOfGpStubTxQueue;
    copyOfGpStubTxQueue.inUse = true;
    copyOfGpStubTxQueue.asdu = emberFillLinkedBuffers(outPkt, (outPkt[0] + 1));
    MEMCOPY(&(copyOfGpStubTxQueue.addr), addr, sizeof(EmberGpAddress));
    return &copyOfGpStubTxQueue;
  }
  return NULL;
}
#endif // USE_MULTIRAIL_HANDLE_TO_SEND_GP_STUB_PACKET

// Finds an entry in the TX queue given the gpd address
static EmberGpTxQueueEntry* emAppGetGpTxQueue(EmberGpAddress* addr)
{
  if (addr == NULL) {
    return NULL;
  }
  for (int i = 0; i < EMBER_APPL_GP_BIDIRECTIONAL_TX_QUEUE_SIZE; i++) {
    EmberGpTxQueueEntry* entry = &(emAppGpTxQueue[i]);
    if (entry->inUse && emGpAddressMatch(addr, &(entry->addr))) {
      return entry;
    }
  }
  return NULL;
}

#ifdef USE_MULTIRAIL_HANDLE_TO_SEND_GP_STUB_PACKET
static EmberGpTxQueueEntry* emAppGetGpTxQueueWrapper(EmberGpAddress* addr)
{
  if (addr == NULL) {
    return NULL;
  }
  EmberGpTxQueueEntry* entry = emAppGetGpTxQueue(addr);
  if (!entry) {
    entry = emAppGetGpStubTxQueue(addr);
  }
  return entry;
}
#else
  #define emAppGetGpTxQueueWrapper(addr) emAppGetGpTxQueue(addr)
#endif // USE_MULTIRAIL_HANDLE_TO_SEND_GP_STUB_PACKET

static bool emAppSetGpTxQueue(EmberGpTxQueueEntry* entry)
{
  if (entry == NULL) {
    return false;
  }
  EmberGpTxQueueEntry* oldEntryIfAny = emAppGetGpTxQueue(&(entry->addr));
  if (oldEntryIfAny) {
    // Overwrite and return
    MEMCOPY(oldEntryIfAny, entry, sizeof(EmberGpTxQueueEntry));
    oldEntryIfAny->inUse = true;
    return true;
  }
  // If no existing , then find the first empty and set the data
  for (int i = 0; i < EMBER_APPL_GP_BIDIRECTIONAL_TX_QUEUE_SIZE; i++) {
    if (!(emAppGpTxQueue[i].inUse)) {
      MEMCOPY(&(emAppGpTxQueue[i]), entry, sizeof(EmberGpTxQueueEntry));
      emAppGpTxQueue[i].inUse = true;
      return true;
    }
  }
  return false;
}

/** @brief Incoming Custom EZSP Message Callback
 *
 * This function is called when the NCP receives a custom EZSP message from the
 * HOST.  The message length and payload is passed to the callback in the first
 * two arguments.  The implementation can then fill in the replyPayload and set
 * the replayPayloadLength to the number of bytes in the replyPayload.
 * See documentation for the function ezspCustomFrame on sending these messages
 * from the HOST.
 *
 * @param messageLength The length of the messagePayload.
 * @param messagePayload The custom message that was sent from the HOST.
 * Ver.: always
 * @param replyPayloadLength The length of the replyPayload.  This needs to be
 * set by the implementation in order for a properly formed respose to be sent
 * back to the HOST. Ver.: always
 * @param replyPayload The custom message to send back to the HOST in respose
 * to the custom message. Ver.: always
 *
 * @return An ::EmberStatus indicating the result of the custom message
 * handling.  This returned status is always the first byte of the EZSP
 * response.
 */
EmberStatus emberAfPluginXncpIncomingCustomFrameCallback(uint8_t messageLength,
                                                         uint8_t *messagePayload,
                                                         uint8_t *replyPayloadLength,
                                                         uint8_t *replyPayload)
{
  // In the sample custom EZSP protocol, the command ID is always the first byte
  // of a custom EZSP command.
  uint8_t index = 0;
  uint8_t commandId = messagePayload[index++];

  switch (commandId) {
    case EMBER_CUSTOM_EZSP_COMMAND_INIT_APP_GP_TX_QUEUE: {
      emAppInitGpTxQueue();
      RAIL_Handle_t h =  emberAfPluginMultirailDemoInit(NULL,
                                                        NULL,
                                                        true,
                                                        RAIL_GetTxPowerDbm(emberGetRailHandle()),
                                                        NULL,
                                                        0,
                                                        0xFFFF,
                                                        NULL);
      return ((h == NULL) ? EMBER_ERR_FATAL : EMBER_SUCCESS);
    }
    break;
    case EMBER_CUSTOM_EZSP_COMMAND_SET_APP_GP_TX_QUEUE: {
      EmberGpTxQueueEntry entry = { 0 };
      entry.addr.applicationId = messagePayload[index++];
      if (entry.addr.applicationId == EMBER_GP_APPLICATION_SOURCE_ID) {
        entry.addr.id.sourceId = (uint32_t)messagePayload[index++];
        entry.addr.id.sourceId += (((uint32_t)messagePayload[index++]) << 8);
        entry.addr.id.sourceId += (((uint32_t)messagePayload[index++]) << 16);
        entry.addr.id.sourceId += (((uint32_t)messagePayload[index++]) << 24);
      } else {
        // Initial support only for GPD Src ID
        return EMBER_INVALID_CALL;
      }
      if (messageLength >= index) {
        entry.asdu = emberFillLinkedBuffers(&messagePayload[index], (messageLength - index));
        if (emAppSetGpTxQueue(&entry)) {
          return EMBER_SUCCESS;
        }
        // Failed entry, free message buffer
        emberReleaseMessageBuffer(entry.asdu);
      }
      return EMBER_INVALID_CALL;
    }
    break;
    case EMBER_CUSTOM_EZSP_COMMAND_GET_APP_GP_TX_QUEUE: {
      EmberGpAddress addr;
      addr.applicationId = messagePayload[index++];
      if (addr.applicationId == EMBER_GP_APPLICATION_SOURCE_ID) {
        addr.id.sourceId = (uint32_t)messagePayload[index++];
        addr.id.sourceId += (((uint32_t)messagePayload[index++]) << 8);
        addr.id.sourceId += (((uint32_t)messagePayload[index++]) << 16);
        addr.id.sourceId += (((uint32_t)messagePayload[index++]) << 24);
      } else {
        // Initial support only for GPD Src ID
        return EMBER_INVALID_CALL;
      }
      EmberGpTxQueueEntry* entry = emAppGetGpTxQueue(&addr);
      if (entry) {
        replyPayload[0] = entry->addr.applicationId;

        replyPayload[1] = entry->addr.id.sourceId >> 0;
        replyPayload[2] = entry->addr.id.sourceId >> 8;
        replyPayload[3] = entry->addr.id.sourceId >> 16;
        replyPayload[4] = entry->addr.id.sourceId >> 24;

        uint8_t outPktLength = emberMessageBufferLength(entry->asdu);
        emberCopyFromLinkedBuffers(entry->asdu,
                                   0,
                                   &replyPayload[5],//outPkt,
                                   outPktLength);
        *replyPayloadLength = (outPktLength + 5);
        return EMBER_SUCCESS;
      }
      return EMBER_INVALID_CALL;
    }
    break;
    case EMBER_CUSTOM_EZSP_COMMAND_SEND_APP_GP_RAW: {
      uint8_t channel = messagePayload[index++];
      uint8_t delayInMsec = messagePayload[index++];
      uint8_t* buff = &messagePayload[index];
      uint8_t size = messageLength - index;
      RAIL_ScheduleTxConfig_t scheduledTxConfig = {
        .mode = RAIL_TIME_DELAY,
        .when = (delayInMsec * 1000)
      };
      RAIL_SchedulerInfo_t schedulerInfo = {
        .priority = 50,
        .slipTime = 2000,
        .transactionTime = 5000
      };
      RAIL_Status_t s = emberAfPluginMultirailDemoSend(buff,
                                                       size,
                                                       ((channel == 0) ? sl_mac_lower_mac_get_radio_channel(0) : channel),
                                                       ((delayInMsec == 0) ? NULL : &scheduledTxConfig),
                                                       &schedulerInfo);
      return ((s == RAIL_STATUS_NO_ERROR) ? EMBER_SUCCESS : EMBER_INVALID_CALL);
    }
    break;
    default:
      return EMBER_INVALID_CALL;
      break;
  }
  return EMBER_INVALID_CALL;
}

#define macToAppDelay(macTimeStamp) ((RAIL_GetTime() & 0x00FFFFFF) - (macTimeStamp))

static void appGpScheduleOutgoingGpdf(EmberZigbeePacketType packetType,
                                      int8u* packetData,
                                      int8u size_p)
{
  //MAC Frame  : [<---------------MAC Header------------->||<------------------------------------NWK Frame----------------------------------->]
  //              FC(2) | Seq(1) | DstPan(2) | DstAddr(2) || FC(1) | ExtFC(0/1) | SrcId(0/4) | SecFc(0/4) | MIC(0/4) | <------GPDF(1/n)------>
  //              FC    : ExtFC Present(b7)=1| AC(b6)=0| Protocol Ver(b5-b2)=3 GP frames| Frame Type(b1-b0) = 0
  //              ExtFC :  rxAfteTX (b6) = 1 |  AppId(b2-b0) = 0
  if (packetType == EMBER_ZIGBEE_PACKET_TYPE_RAW_MAC
      && size_p > 9 // minimum
      && ((packetData[7] & 0x3C) == 0x0C)            // FC : GP Frames has Protocol Version = 3 ?
      && (((packetData[7] & 0xC3) == 0x01)           // FC : ExtFC(b7) = 0 | AC(b6) = 0 | Frame type(b1-b0) = 1 = Maint, Frame
          || (((packetData[7] & 0xC3) == 0x80)     // FC : ExtFC(b7) = 1 | AC(b6) = 0 | Frame type(b1-b0) = 0 = Data frame
              && ((packetData[8] & 0xC0) == 0x40)))) { // ExtFC :  Direction(b7) = 0 | rxAfteTX (b6) = 1
    // The last 3 bytes of packetData contain the MAC time stamp
    const uint32_t macTimeStamp = ((uint32_t)packetData[size_p - 3] << 16)
                                  + ((uint32_t)packetData[size_p - 2] << 8)
                                  + ((uint32_t)packetData[size_p - 1]);

    // Do we have enough time to schedule the response?
    if (macToAppDelay(macTimeStamp) < GP_RX_OFFSET_USEC) {
      EmberGpAddress gpdAddr;
      gpdAddr.applicationId = EMBER_GP_APPLICATION_SOURCE_ID;
      gpdAddr.id.sourceId = 0;
      if (((packetData[7] & 0xC3) == 0x80)   // FC : ExtFC(b7) = 1 | AC(b6) = 0 | Frame type(b1-b0) = 0 = Data frame
          && ((packetData[8] & 0x07) == EMBER_GP_APPLICATION_SOURCE_ID)) {// ExtFC :  App Id (b2-b0) = 0
        memcpy(&gpdAddr.id.sourceId, &packetData[9], sizeof(EmberGpSourceId));
      }
      // Is there a queued response for this source ID?
      EmberGpTxQueueEntry* entry = emAppGetGpTxQueueWrapper(&gpdAddr);
      if (entry) {
        uint8_t outPktLength = emberMessageBufferLength(entry->asdu);
        uint8_t outPkt[128];
        emberCopyFromLinkedBuffers(entry->asdu,
                                   0,
                                   outPkt,
                                   outPktLength);

        // Schedule sending the response.
        RAIL_SchedulerInfo_t schedulerInfo = {
          .priority = 50,
          .slipTime = 2000,
          .transactionTime = 5000
        };
        RAIL_ScheduleTxConfig_t scheduledTxConfig = {
          .mode = RAIL_TIME_DELAY,
          // We could reuse macToAppDelay here, but recalculating the delay
          // will give us the most up-to-date timings:
          .when = GP_RX_OFFSET_USEC - macToAppDelay(macTimeStamp)
        };

        RAIL_Status_t UNUSED status = emberAfPluginMultirailDemoSend(outPkt,
                                                                     outPktLength,
                                                                     sl_mac_lower_mac_get_radio_channel(0),
                                                                     &scheduledTxConfig,
                                                                     &schedulerInfo);
        emAppFreeGpTxQueueEntry(entry);
      }
    }
  }
}

// Example of sending and GPDF ack frame back to GPD
EmberPacketAction emberPacketHandoffIncoming(EmberZigbeePacketType packetType,
                                             EmberMessageBuffer packetBuffer,
                                             uint8_t index,
                                             void *data)
{
  uint8_t size_p = emberMessageBufferLength(packetBuffer) - index;
  uint8_t packetData[128];
  // Flat packet : [<-----MAC Frame----->|<--8 bytes Appended Info-->]
  emberCopyFromLinkedBuffers(packetBuffer,
                             index,
                             packetData,
                             size_p);
  appGpScheduleOutgoingGpdf(packetType,
                            packetData,
                            size_p);
  return EMBER_ACCEPT_PACKET;
}

EmberEventControl emAppGpTransmitCompleteEventControl;
void emAppGpTransmitCompleteEventHandler(void)
{
  emberEventControlSetInactive(emAppGpTransmitCompleteEventControl);
#if (defined USE_MULTIRAIL_HANDLE_TO_SEND_GP_STUB_PACKET)
  extern void emberDGpSentHandler(EmberStatus status,
                                  uint8_t gpepHandle);
  emberDGpSentHandler(EMBER_SUCCESS, 0);
#endif // USE_MULTIRAIL_HANDLE_TO_SEND_GP_STUB_PACKET
}

/** @brief A callback called whenever a secondary instance RAIL event occurs.
 *
 * @param[in] handle A handle for a RAIL instance.
 * @param[in] events A bit mask of RAIL events (full list in rail_types.h)
 */
void emberAfPluginMultirailDemoRailEventCallback(RAIL_Handle_t handle,
                                                 RAIL_Events_t events)
{
  if (events & RAIL_EVENT_TX_PACKET_SENT) {
    emberEventControlSetDelayMS(emAppGpTransmitCompleteEventControl, 0);
  }
  (void)handle; // unreferenced parameter
  uint8_t payload[9];
  payload[0] = EMBER_CUSTOM_EZSP_CALLBACK_APP_GP_TX_QUEUE_EVENT;
  payload[1] = events >> 0;
  payload[2] = events >> 8;
  payload[3] = events >> 16;
  payload[4] = events >> 24;
  payload[5] = events >> 32;
  payload[6] = events >> 40;
  payload[7] = events >> 48;
  payload[8] = events >> 56;
  emberAfPluginXncpSendCustomEzspMessage(sizeof(payload), payload);
}
#endif //EMBER_AF_PLUGIN_MULTIRAIL_DEMO

/** @brief Mark Buffers
 *
 * This function is called when the garbage collector runs. Any buffers held by
 * the application must be marked.
 */
void emberAfMarkBuffersCallback(void)
{
#ifdef EMBER_AF_PLUGIN_MULTIRAIL_DEMO
  for (int i = 0; i < EMBER_APPL_GP_BIDIRECTIONAL_TX_QUEUE_SIZE; i++) {
    // Mark all our queue entries, lest the garbage collector releases them
    // behind our backs.
    emMarkBuffer(&emAppGpTxQueue[i].asdu);
  }
#endif // EMBER_AF_PLUGIN_MULTIRAIL_DEMO
}
