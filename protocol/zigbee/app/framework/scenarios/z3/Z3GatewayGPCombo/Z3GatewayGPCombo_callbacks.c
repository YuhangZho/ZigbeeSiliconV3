/***************************************************************************//**
 * @file
 * @brief
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

// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "af.h"
#include "app/framework/util/af-main.h"
#include "app/framework/util/util.h"

#include "app/util/zigbee-framework/zigbee-device-common.h"
#include "stack/include/trust-center.h"
#include <stdlib.h>
#include "app/xncp/xncp-sample-custom-ezsp-protocol.h"

// the number of tokens that can be written using ezspSetToken and read
// using ezspGetToken
#define MFGSAMP_NUM_EZSP_TOKENS 8
// the size of the tokens that can be written using ezspSetToken and
// read using ezspGetToken
#define MFGSAMP_EZSP_TOKEN_SIZE 8
// the number of manufacturing tokens
#define MFGSAMP_NUM_EZSP_MFG_TOKENS 11
// the size of the largest EZSP Mfg token, EZSP_MFG_CBKE_DATA
// please refer to app/util/ezsp/ezsp-enum.h
#define MFGSAMP_EZSP_TOKEN_MFG_MAXSIZE 92

#define GREEN_POWER_MAX_FRAME_LENGTH 70
#define GREEN_POWER_COMMAND_INDEX 0

EmberStatus emberAfTrustCenterStartNetworkKeyUpdate(void);

// Forward declarations of custom cli command functions
static void mfgappTokenDump(void);
static void changeNwkKeyCommand(void);
static void printNextKeyCommand(void);
static void versionCommand(void);
static void setTxPowerCommand(void);
// Gp App Tx Queue Custom Command Interface
static void gpAppGpTxQueueInit(void);
static void gpAppGpTxQueueSet(void);
static void gpAppGpSendGpRaw(void);
static void gpAppGpTxQueueGet(void);

EmberCommandEntry emberAfCustomCommands[] = {
  //emberCommandEntryAction("print_srt", printSourceRouteTable, "", ""),// Concentrator plugin, should eb used. for this
  emberCommandEntryAction("tokdump", mfgappTokenDump, "", ""),
  emberCommandEntryAction("changeNwkKey", changeNwkKeyCommand, "", ""),
  emberCommandEntryAction("printNextKey", printNextKeyCommand, "", ""),
  emberCommandEntryAction("version", versionCommand, "", ""),
  emberCommandEntryAction("txPower", setTxPowerCommand, "s", ""),
  // Gp App Tx Queue Custom Command Interface
  emberCommandEntryAction("gpAppTxQInit", gpAppGpTxQueueInit, "", "Initialises the Application Gp Tx Queue"),
  emberCommandEntryAction("gpAppTxQSet", gpAppGpTxQueueSet, "uwb", "Sets a packet on Application Gp Tx Queue"),
  emberCommandEntryAction("gpAppGpSendGpRaw", gpAppGpSendGpRaw, "uub", "Sends a raw packet using App rail handle"),
  emberCommandEntryAction("gpAppTxQGet", gpAppGpTxQueueGet, "uw", "Gets a packet from Application Gp Tx Queue"),
  emberCommandEntryTerminator()
};

//// ******* test of token dump code

// the manufacturing tokens are enumerated in app/util/ezsp/ezsp-protocol.h
// the names are enumerated here to make it easier for the user
const char * ezspMfgTokenNames[] =
{
  "EZSP_MFG_CUSTOM_VERSION...",
  "EZSP_MFG_STRING...........",
  "EZSP_MFG_BOARD_NAME.......",
  "EZSP_MFG_MANUF_ID.........",
  "EZSP_MFG_PHY_CONFIG.......",
  "EZSP_MFG_BOOTLOAD_AES_KEY.",
  "EZSP_MFG_ASH_CONFIG.......",
  "EZSP_MFG_EZSP_STORAGE.....",
  "EZSP_STACK_CAL_DATA.......",
  "EZSP_MFG_CBKE_DATA........",
  "EZSP_MFG_INSTALLATION_CODE"
};

// IAS ACE Server side callbacks
bool emberAfIasAceClusterArmCallback(uint8_t armMode,
                                     uint8_t* armDisarmCode,
                                     uint8_t zoneId)
{
  uint16_t armDisarmCodeLength = emberAfStringLength(armDisarmCode);
  EmberNodeId sender = emberGetSender();
  uint16_t i;

  emberAfAppPrint("IAS ACE Arm Received %x", armMode);

  // Start i at 1 to skip over leading character in the byte array as it is the
  // length byte
  for (i = 1; i < armDisarmCodeLength; i++) {
    emberAfAppPrint("%c", armDisarmCode[i]);
  }
  emberAfAppPrintln(" %x", zoneId);

  emberAfFillCommandIasAceClusterArmResponse(armMode);
  emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, sender);

  return true;
}

bool emberAfIasAceClusterBypassCallback(uint8_t numberOfZones,
                                        uint8_t* zoneIds,
                                        uint8_t* armDisarmCode)
{
  EmberNodeId sender = emberGetSender();
  uint8_t i;

  emberAfAppPrint("IAS ACE Cluster Bypass for zones ");

  for (i = 0; i < numberOfZones; i++) {
    emberAfAppPrint("%d ", zoneIds[i]);
  }
  emberAfAppPrintln("");

  emberAfFillCommandIasAceClusterBypassResponse(numberOfZones,
                                                zoneIds,
                                                numberOfZones);
  emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, sender);

  return true;
}

// Called to dump all of the tokens. This dumps the indices, the names,
// and the values using ezspGetToken and ezspGetMfgToken. The indices
// are used for read and write functions below.
static void mfgappTokenDump(void)
{
  EmberStatus status;
  uint8_t tokenData[MFGSAMP_EZSP_TOKEN_MFG_MAXSIZE];
  uint8_t index, i, tokenLength;

  // first go through the tokens accessed using ezspGetToken
  emberAfCorePrintln("(data shown little endian)");
  emberAfCorePrintln("Tokens:");
  emberAfCorePrintln("idx  value:");
  for (index = 0; index < MFGSAMP_NUM_EZSP_TOKENS; index++) {
    // get the token data here
    status = ezspGetToken(index, tokenData);
    emberAfCorePrint("[%d]", index);
    if (status == EMBER_SUCCESS) {
      // Print out the token data
      for (i = 0; i < MFGSAMP_EZSP_TOKEN_SIZE; i++) {
        emberAfCorePrint(" %X", tokenData[i]);
      }

      emberSerialWaitSend(APP_SERIAL);
      emberAfCorePrintln("");
    } else {
      // handle when ezspGetToken returns an error
      emberAfCorePrintln(" ... error 0x%x ...",
                         status);
    }
  }

  // now go through the tokens accessed using ezspGetMfgToken
  // the manufacturing tokens are enumerated in app/util/ezsp/ezsp-protocol.h
  // this file contains an array (ezspMfgTokenNames) representing the names.
  emberAfCorePrintln("Manufacturing Tokens:");
  emberAfCorePrintln("idx  token name                 len   value");
  for (index = 0; index < MFGSAMP_NUM_EZSP_MFG_TOKENS; index++) {
    // ezspGetMfgToken returns a length, be careful to only access
    // valid token indices.
    tokenLength = ezspGetMfgToken(index, tokenData);
    emberAfCorePrintln("[%x] %p: 0x%x:", index,
                       ezspMfgTokenNames[index], tokenLength);

    // Print out the token data
    for (i = 0; i < tokenLength; i++) {
      if ((i != 0) && ((i % 8) == 0)) {
        emberAfCorePrintln("");
        emberAfCorePrint("                                    :");
        emberSerialWaitSend(APP_SERIAL);
      }
      emberAfCorePrint(" %X", tokenData[i]);
    }
    emberSerialWaitSend(APP_SERIAL);
    emberAfCorePrintln("");
  }
  emberAfCorePrintln("");
}

static void changeNwkKeyCommand(void)
{
  EmberStatus status = emberAfTrustCenterStartNetworkKeyUpdate();

  if (status != EMBER_SUCCESS) {
    emberAfCorePrintln("Change Key Error %x", status);
  } else {
    emberAfCorePrintln("Change Key Success");
  }
}

static void dcPrintKey(uint8_t label, uint8_t *key)
{
  uint8_t i;
  emberAfCorePrintln("key %x: ", label);
  for (i = 0; i < EMBER_ENCRYPTION_KEY_SIZE; i++) {
    emberAfCorePrint("%x", key[i]);
  }
  emberAfCorePrintln("");
}

static void printNextKeyCommand(void)
{
  EmberKeyStruct nextNwkKey;
  EmberStatus status;

  status = emberGetKey(EMBER_NEXT_NETWORK_KEY,
                       &nextNwkKey);

  if (status != EMBER_SUCCESS) {
    emberAfCorePrintln("Error getting key");
  } else {
    dcPrintKey(1, nextNwkKey.key.contents);
  }
}

static void versionCommand(void)
{
  emberAfCorePrintln("Version:  0.1 Alpha");
  emberAfCorePrintln(" %s", __DATE__);
  emberAfCorePrintln(" %s", __TIME__);
  emberAfCorePrintln("");
}

static void setTxPowerCommand(void)
{
  int8_t dBm = (int8_t)emberSignedCommandArgument(0);

  emberSetRadioPower(dBm);
}

/* This sample application demostrates an NCP using a custom protocol to
 * communicate with the host. As an example protocol, the NCP has defined
 * commands so that the host can drive Green Power on the NCP.
 *
 * The host sends custom EZSP commands to the NCP, and the NCP acts on them
 * based on the functionality in the code found below.
 * This sample application is meant to be paired with the ncp-uart-hw-gp
 * sample application in the NCP Application Framework.
 */
static void sendGreenPowerCommand(uint8_t command,
                                  uint8_t* possibleParam,
                                  uint8_t paramLength,
                                  bool replyExpected)
{
  EmberStatus status;
  uint8_t commandLength = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  uint8_t replyLength = GREEN_POWER_MAX_FRAME_LENGTH;
  uint8_t replyPayload[GREEN_POWER_MAX_FRAME_LENGTH];

  if ((possibleParam != NULL) && (paramLength > 0)) {
    commandLength = paramLength;
    if (paramLength > GREEN_POWER_MAX_FRAME_LENGTH) {
      commandLength = GREEN_POWER_MAX_FRAME_LENGTH;
    }
    MEMCOPY(commandPayload, possibleParam, commandLength);
  } else {
    return;
  }
  // Send the command to the NCP.
  status = ezspCustomFrame(commandLength,
                           commandPayload,
                           &replyLength,
                           replyPayload);
  emberAfCorePrintln("Send custom frame Command Id = 0x%X Status = 0x%X", command, status);
  // If we were expecting a response, display it.
  if (replyExpected
      && status == 0
      && replyLength > 0) {
    emberAfCorePrint("Response : ");
    emberAfCorePrintBuffer(replyPayload, replyLength, true);
    emberAfCorePrintln("");
  }
}

static void gpAppGpTxQueueInit(void)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  commandPayload[idx++] = EMBER_CUSTOM_EZSP_COMMAND_INIT_APP_GP_TX_QUEUE;
  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        false);
}

static void gpAppGpTxQueueSet(void)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  commandPayload[idx] = EMBER_CUSTOM_EZSP_COMMAND_SET_APP_GP_TX_QUEUE;
  idx += sizeof(uint8_t);
  commandPayload[idx] = emberUnsignedCommandArgument(0); // AppId = 0
  idx += sizeof(uint8_t);
  uint32_t srCId = (uint32_t)emberUnsignedCommandArgument(1);
  commandPayload[idx++] = srCId >> 0;
  commandPayload[idx++] = srCId >> 8;
  commandPayload[idx++] = srCId >> 16;
  commandPayload[idx++] = srCId >> 24;
  uint8_t dataLength = 0;
  const uint8_t *data = emberStringCommandArgument(2, &dataLength); //mpdu
  if (data
      && dataLength) {
    //emberAfCorePrint("Gp Frame Length = %d , received dataLength %d : ",GREEN_POWER_MAX_FRAME_LENGTH, dataLength);
    //emberAfCorePrintBuffer(data, dataLength, 1);
    memcpy(&commandPayload[idx], data, dataLength);
    idx += dataLength;
  }
  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        false);
}

static void gpAppGpTxQueueGet(void)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  commandPayload[idx] = EMBER_CUSTOM_EZSP_COMMAND_GET_APP_GP_TX_QUEUE;
  idx += sizeof(uint8_t);
  commandPayload[idx] = emberUnsignedCommandArgument(0); // AppId = 0
  idx += sizeof(uint8_t);
  uint32_t srCId = (uint32_t)emberUnsignedCommandArgument(1);
  commandPayload[idx++] = srCId >> 0;
  commandPayload[idx++] = srCId >> 8;
  commandPayload[idx++] = srCId >> 16;
  commandPayload[idx++] = srCId >> 24;
  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        true);
}

static void gpAppGpSendGpRaw(void)
{
  uint8_t idx = 0;
  uint8_t commandPayload[GREEN_POWER_MAX_FRAME_LENGTH];
  commandPayload[idx] = EMBER_CUSTOM_EZSP_COMMAND_SEND_APP_GP_RAW;
  idx += sizeof(uint8_t);
  commandPayload[idx] = emberUnsignedCommandArgument(0); // Channel
  idx += sizeof(uint8_t);
  commandPayload[idx] = emberUnsignedCommandArgument(1); // delay
  idx += sizeof(uint8_t);
  uint8_t dataLength = 0;
  const uint8_t *data = emberStringCommandArgument(2, &dataLength); //mpdu
  if (data
      && dataLength) {
    //emberAfCorePrint("Gp Frame Length = %d , received dataLength %d : ",GREEN_POWER_MAX_FRAME_LENGTH, dataLength);
    //emberAfCorePrintBuffer(data, dataLength, 1);
    memcpy(&commandPayload[idx], data, dataLength);
    idx += dataLength;
  }
  sendGreenPowerCommand(commandPayload[GREEN_POWER_COMMAND_INDEX],
                        commandPayload,
                        idx,
                        false);
}

/** @brief Gp Notification
 *
 * This function is called by the Green Power Sink Plugin to notify the
 * application of a Green Power Gp Notification of an incomming gpd command.
 * Return true to handle in application.
 *
 * @param options from the incoming Gp Notification Command Ver.: always
 * @param addr GPD address        Ver.: always
 * @param gpdSecurityFrameCounter Ver.: always
 * @param gpdCommandId            Ver.: always
 * @param gpdCommandPayload first byte is length of the payload Ver.: always
 * @param gppShortAddress         Ver.: always
 * @param gppDistance             Ver.: always
 */
bool emberAfGreenPowerClusterGpNotificationForwardCallback(uint16_t options,
                                                           EmberGpAddress * addr,
                                                           uint32_t gpdSecurityFrameCounter,
                                                           uint8_t gpdCommandId,
                                                           uint8_t * gpdCommandPayload,
                                                           uint16_t gppShortAddress,
                                                           uint8_t  gppDistance)
{
  emberAfCorePrintln("Received Notification GpdAdd [AppId = %d, SrcId = 0x%4X] for Cmd = %d withRxAfterTx = %d",
                     addr->applicationId,
                     addr->id.sourceId,
                     gpdCommandId,
                     (options & 0x0800) ? true : false);
  return true;
}
// EZSP Custom Frame handler for messages from host
void ezspCustomFrameHandler(int8u payloadLength,
                            int8u* payload)
{
  emberAfCorePrint("EZSP Custom Frame : ");
  emberAfCorePrintBuffer(payload, payloadLength, true);
  emberAfCorePrintln("");
}
