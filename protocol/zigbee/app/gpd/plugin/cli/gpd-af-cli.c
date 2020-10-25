/***************************************************************************//**
 * @file
 * @brief Application main entry and initialisations.
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
#include "gpd-components-common.h"
#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include CONFIGURATION_HEADER
#include EMBER_AF_API_COMMAND_INTERPRETER2

// If serial plugin is enabled for debug print
#if defined(EMBER_AF_PLUGIN_SERIAL)
#include EMBER_AF_API_SERIAL
#endif

#if defined(EMBER_AF_PLUGIN_EMBER_MINIMAL_PRINTF)
#define gpdDebugPrintf(...) emberSerialGuaranteedPrintf(APP_SERIAL, __VA_ARGS__)
#else
#define gpdDebugPrintf(...)
#endif

extern void emberGpdAppSingleEventCommission(void);

void emberGpdAfCliChannelReq(void)
{
}

void emberGpdAfCliCommReq(void)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  uint8_t action = emberUnsignedCommandArgument(0);
  if (action == 1) {
    emberGpdAfPluginCommission(gpd);
  } else if (action == 0) {
    emberGpdAfPluginDeCommission(gpd);
  } else if (action == 0xff) {
    emberGpdAppSingleEventCommission();
  }
}

void emberGpdAfCliNodeInfo(void)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  if (gpd->addr.appId == EMBER_GPD_APP_ID_SRC_ID) {
    gpdDebugPrintf("App Id = %d SrcId = 0x%4x\n", gpd->addr.appId, gpd->addr.id.srcId);
  } else if (gpd->addr.appId == EMBER_GPD_APP_ID_IEEE_ID) {
    uint8_t *ieee = gpd->addr.id.ieee;
    gpdDebugPrintf("App Id = %d Ep = %d IEEE=0x%x%x%x%x%x%x%x%x\n",
                   gpd->addr.appId, gpd->addr.gpdEndpoint,
                   ieee[7], ieee[6], ieee[5], ieee[4], ieee[3], ieee[2], ieee[1], ieee[0]);
  }

  gpdDebugPrintf("Auto-Comm=%d\n", gpd->autoCommissioning);
  gpdDebugPrintf("Channel=%d\n", gpd->channel);
  gpdDebugPrintf("GPD State=%d\n", gpd->gpdState);
  gpdDebugPrintf("GPD Bidir=%d\n", gpd->rxAfterTx);
  gpdDebugPrintf("  GPD rxWindow=%d msec\n", gpd->minRxWindow);
  gpdDebugPrintf("  GPD rxOffset=%d msec\n", gpd->rxOffset);
  gpdDebugPrintf("Sec Level=%d\n", gpd->securityLevel);
  gpdDebugPrintf("  Sec KeyType=%d\n", gpd->securityKeyType);
  gpdDebugPrintf("  Sec Key={%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x} \n",
                 gpd->securityKey[0], gpd->securityKey[1], gpd->securityKey[2], gpd->securityKey[3],
                 gpd->securityKey[4], gpd->securityKey[5], gpd->securityKey[6], gpd->securityKey[7],
                 gpd->securityKey[8], gpd->securityKey[9], gpd->securityKey[10], gpd->securityKey[11],
                 gpd->securityKey[12], gpd->securityKey[13], gpd->securityKey[14], gpd->securityKey[15]);
  gpdDebugPrintf("  Sec FC=%d\n", gpd->securityFrameCounter);
}
void emberGpdAfCliNodeSetAutoComm(void)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  gpd->autoCommissioning = (bool)emberUnsignedCommandArgument(0);
}
void emberGpdAfCliNodeSetAddr(void)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  gpd->addr.appId = emberUnsignedCommandArgument(0);
  if (gpd->addr.appId == EMBER_GPD_APP_ID_SRC_ID) {
    gpd->addr.id.srcId = ((uint32_t)emberUnsignedCommandArgument(1) << 24)   \
                         + ((uint32_t)emberUnsignedCommandArgument(2) << 16) \
                         + ((uint32_t)emberUnsignedCommandArgument(3) << 8)  \
                         + emberUnsignedCommandArgument(4);
  } else if (gpd->addr.appId == EMBER_GPD_APP_ID_IEEE_ID) {
    // Endpoint
    gpd->addr.gpdEndpoint = (uint8_t)emberUnsignedCommandArgument(1);
    // A simple way to use the command for testing
    gpd->addr.id.ieee[0] = (uint8_t)emberUnsignedCommandArgument(2);
    gpd->addr.id.ieee[1] = (uint8_t)emberUnsignedCommandArgument(3);
    gpd->addr.id.ieee[2] = (uint8_t)emberUnsignedCommandArgument(4);
    gpd->addr.id.ieee[3] = (uint8_t)emberUnsignedCommandArgument(5);
    gpd->addr.id.ieee[4] = (uint8_t)emberUnsignedCommandArgument(6);
    gpd->addr.id.ieee[5] = (uint8_t)emberUnsignedCommandArgument(7);
    gpd->addr.id.ieee[6] = (uint8_t)emberUnsignedCommandArgument(8);
    gpd->addr.id.ieee[7] = (uint8_t)emberUnsignedCommandArgument(9);
  }
}
void emberGpdAfCliNodeSetBiDirParam(void)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  gpd->rxAfterTx = (bool)emberUnsignedCommandArgument(0);
  if (gpd->rxAfterTx) {
    gpd->rxOffset = emberUnsignedCommandArgument(1);
    gpd->minRxWindow = emberUnsignedCommandArgument(2);
  }
}
void emberGpdAfCliNodeSetChannel(void)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  gpd->channel = emberUnsignedCommandArgument(0);
}
void emberGpdAfCliNodeSetSecFc(void)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  gpd->securityFrameCounter = (uint32_t)emberUnsignedCommandArgument(0);
}
void emberGpdAfCliNodeSetSecKey(void)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  uint8_t argLength;
  uint8_t *key = emberStringCommandArgument(0, &argLength);
  if (argLength == 16) {
    memcpy(gpd->securityKey, key, 16);
  }
}
void emberGpdAfCliNodeSetSecKeyType(void)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  gpd->securityKeyType = emberUnsignedCommandArgument(0);
}

void emberGpdAfCliNodeSetSecLevel(void)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  gpd->securityLevel = emberUnsignedCommandArgument(0);
}

// Sends a GPDF using the gpd node instance
// <frame type> <command with payload>
void emberGpdAfCliSendGpdf(void)
{
  EmberGpd_t * gpd = emberGpdGetGpd();
  uint8_t frameType = emberUnsignedCommandArgument(0);
  uint8_t cmdPayloadLength;
  uint8_t *cmdPayload = emberStringCommandArgument(1, &cmdPayloadLength);
  emberAfGpdfSend(frameType, gpd, cmdPayload, cmdPayloadLength, 1);
}

void emberGpdAfCliResetCommand(void)
{
  halReboot();
}
