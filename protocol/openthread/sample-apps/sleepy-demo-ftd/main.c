/*
 *  Copyright (c) 2020, The OpenThread Authors.
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

#include "sl_led.h"
#include "sl_simple_led_instances.h"
#include "sl_button.h"
#include "sl_simple_button.h"
#include "openthread-system.h"
#include <assert.h>
#include <common/logging.hpp>
#include <openthread-core-config.h>
#include <string.h>
#include <openthread/cli.h>
#include <openthread/config.h>
#include <openthread/dataset_ftd.h>
#include <openthread/diag.h>
#include <openthread/instance.h>
#include <openthread/message.h>
#include <openthread/tasklet.h>
#include <openthread/thread.h>
#include <openthread/udp.h>
#include <openthread/platform/logging.h>

#if defined(SL_COMPONENT_CATALOG_PRESENT)
  #include "sl_component_catalog.h"
#endif
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
  #include "sl_power_manager.h"
#endif

// Constants
#define MULTICAST_ADDR "ff03::1"
#define MULTICAST_PORT 123
#define RECV_PORT 234
#define MTD_MESSAGE "mtd button"
#define FTD_MESSAGE "ftd button"

// Prototypes
void setNetworkConfiguration(otInstance *aInstance);
void handleNetifStateChanged(uint32_t aFlags, void *aContext);
void initUdp(void);
void applicationTick(void);
void sFtdReceiveCallback(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo);

// Variables
static otInstance *        instance;
static otUdpSocket         sFtdSocket;
static bool                sHaveSwitchAddress = false;
static otIp6Address        sSwitchAddress;
static bool                sFtdButtonPressed              = false;

void otTaskletsSignalPending(otInstance *aInstance)
{
    (void)aInstance;
}

int main(int argc, char *argv[])
{
    otSysInit(argc, argv);

    instance = otInstanceInitSingle();
    assert(instance);

    sl_led_turn_off(&sl_led_led0);
    otCliUartInit(instance);
    otCliOutputFormat("sleepy-demo-ftd started\r\n");

    setNetworkConfiguration(instance);
    otSetStateChangedCallback(instance, handleNetifStateChanged, instance);
    initUdp();

    otIp6SetEnabled(instance, true);
    otThreadSetEnabled(instance, true);

    while (!otSysPseudoResetWasRequested())
    {
        otTaskletsProcess(instance);
        otSysProcessDrivers(instance);
        applicationTick();
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
        // Let the CPU go to sleep if the system allows it.
        sl_power_manager_sleep();
#endif // SL_CATALOG_POWER_MANAGER_PRESENT
    }

    otInstanceFinalize(instance);
    return 0;
}

/*
 * Provide, if required an "otPlatLog()" function
 */
#if OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_APP
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    OT_UNUSED_VARIABLE(aLogLevel);
    OT_UNUSED_VARIABLE(aLogRegion);
    OT_UNUSED_VARIABLE(aFormat);

    va_list ap;
    va_start(ap, aFormat);
    otCliPlatLogv(aLogLevel, aLogRegion, aFormat, ap);
    va_end(ap);
}
#endif

/**
 * Override default network settings, such as panid, so the devices can join a network
 */
void setNetworkConfiguration(otInstance *aInstance)
{
    static char          aNetworkName[] = "SleepyEFR32";
    otOperationalDataset aDataset;

    memset(&aDataset, 0, sizeof(otOperationalDataset));

    /*
     * Fields that can be configured in otOperationDataset to override defaults:
     *     Network Name, Mesh Local Prefix, Extended PAN ID, PAN ID, Delay Timer,
     *     Channel, Channel Mask Page 0, Network Master Key, PSKc, Security Policy
     */
    aDataset.mActiveTimestamp                      = 1;
    aDataset.mComponents.mIsActiveTimestampPresent = true;

    /* Set Channel to 15 */
    aDataset.mChannel                      = 15;
    aDataset.mComponents.mIsChannelPresent = true;

    /* Set Pan ID to 2222 */
    aDataset.mPanId                      = (otPanId)0x2222;
    aDataset.mComponents.mIsPanIdPresent = true;

    /* Set Extended Pan ID to C0DE1AB5C0DE1AB5 */
    uint8_t extPanId[OT_EXT_PAN_ID_SIZE] = {0xC0, 0xDE, 0x1A, 0xB5, 0xC0, 0xDE, 0x1A, 0xB5};
    memcpy(aDataset.mExtendedPanId.m8, extPanId, sizeof(aDataset.mExtendedPanId));
    aDataset.mComponents.mIsExtendedPanIdPresent = true;

    /* Set master key to 1234C0DE1AB51234C0DE1AB51234C0DE */
    uint8_t key[OT_MASTER_KEY_SIZE] = {0x12, 0x34, 0xC0, 0xDE, 0x1A, 0xB5, 0x12, 0x34, 0xC0, 0xDE, 0x1A, 0xB5};
    memcpy(aDataset.mMasterKey.m8, key, sizeof(aDataset.mMasterKey));
    aDataset.mComponents.mIsMasterKeyPresent = true;

    /* Set Network Name to SleepyEFR32 */
    size_t length = strlen(aNetworkName);
    assert(length <= OT_NETWORK_NAME_MAX_SIZE);
    memcpy(aDataset.mNetworkName.m8, aNetworkName, length);
    aDataset.mComponents.mIsNetworkNamePresent = true;

    otDatasetSetActive(aInstance, &aDataset);
}

void handleNetifStateChanged(uint32_t aFlags, void *aContext)
{
    if ((aFlags & OT_CHANGED_THREAD_ROLE) != 0)
    {
        otDeviceRole changedRole = otThreadGetDeviceRole(aContext);

        switch (changedRole)
        {
        case OT_DEVICE_ROLE_LEADER:
            otCliOutputFormat("sleepy-demo-ftd changed to leader\r\n");
            break;
        case OT_DEVICE_ROLE_ROUTER:
            otCliOutputFormat("sleepy-demo-ftd changed to router\r\n");
            break;

        case OT_DEVICE_ROLE_CHILD:
            break;

        case OT_DEVICE_ROLE_DETACHED:
        case OT_DEVICE_ROLE_DISABLED:
            break;
        }
    }
}

void initUdp(void)
{
    otError    error;
    otSockAddr sockaddr;

    memset(&sockaddr, 0, sizeof(sockaddr));

    otIp6AddressFromString(MULTICAST_ADDR, &sockaddr.mAddress);
    sockaddr.mPort = MULTICAST_PORT;

    error = otUdpOpen(instance, &sFtdSocket, sFtdReceiveCallback, NULL);
    if (error != OT_ERROR_NONE)
    {
        otCliOutputFormat("FTD failed to open udp multicast\r\n");
        return;
    }

    error = otUdpBind(&sFtdSocket, &sockaddr);
    if (error != OT_ERROR_NONE)
    {
        otUdpClose(&sFtdSocket);
        otCliOutputFormat("FTD failed to bind udp multicast\r\n");
        return;
    }
}

void sl_button_on_change(const sl_button_t *handle) {
    if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
        sFtdButtonPressed = true;
    }
}

void applicationTick(void)
{
    otError       error = 0;
    otMessageInfo messageInfo;
    otMessage *   message = NULL;
    char *        payload = FTD_MESSAGE;

    if (sFtdButtonPressed == true)
    {
        sFtdButtonPressed = false;

        if (sHaveSwitchAddress)
        {
            memset(&messageInfo, 0, sizeof(messageInfo));
            memcpy(&messageInfo.mPeerAddr, &sSwitchAddress, sizeof messageInfo.mPeerAddr);
            messageInfo.mPeerPort = RECV_PORT;

            message = otUdpNewMessage(instance, NULL);

            if (message != NULL)
            {
                error = otMessageAppend(message, payload, (uint16_t)strlen(payload));

                if (error == OT_ERROR_NONE)
                {
                    error = otUdpSend(&sFtdSocket, message, &messageInfo);

                    if (error == OT_ERROR_NONE)
                    {
                        return;
                    }
                }
            }

            if (message != NULL)
            {
                otMessageFree(message);
            }
        }
    }
}

void sFtdReceiveCallback(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    OT_UNUSED_VARIABLE(aContext);
    OT_UNUSED_VARIABLE(aMessage);
    OT_UNUSED_VARIABLE(aMessageInfo);
    uint8_t buf[1500];
    int     length;

    sl_led_toggle(&sl_led_led0);

    length      = otMessageRead(aMessage, otMessageGetOffset(aMessage), buf, sizeof(buf) - 1);
    buf[length] = '\0';
    otCliOutputFormat("Message Received: %s\r\n", buf);

    sHaveSwitchAddress = true;
    memcpy(&sSwitchAddress, &aMessageInfo->mPeerAddr, sizeof sSwitchAddress);
}
