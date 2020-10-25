/***************************************************************************//**
 * @file
 * @brief User-configurable stack memory allocation and convenience stubs
 * for little-used callbacks.
 *
 *
 * \b Note: Application developers should \b not modify any portion
 * of this file. Doing so may lead to mysterious bugs. Allocations should be
 * adjusted only with macros in a custom CONFIGURATION_HEADER.
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
#include "stack/include/ember-static-struct.h" // Required typedefs
#include "hal/micro/micro.h"
#include "stack/include/message.h" // Required for packetHandlers
#include "mac-child.h" // unified-mac

// *****************************************
// Memory Allocations & declarations
// *****************************************

extern uint8_t emAvailableMemory[];
#if defined(CORTEXM3)
  #define align(value) (((value) + 3) & ~0x03)
#else
  #define align(value) (value)
#endif

//------------------------------------------------------------------------------
// API Version

const uint8_t emApiVersion
  = (EMBER_API_MAJOR_VERSION << 4) + EMBER_API_MINOR_VERSION;

//------------------------------------------------------------------------------
// Multi PAN globals

#define NUM_MULTI_PAN_FORKS    (EMBER_AF_ZC_AND_ZR_DEVICE_COUNT == 0 \
                                ? 1                                  \
                                : EMBER_AF_ZC_AND_ZR_DEVICE_COUNT)
uint8_t emNumMultiPanForks = NUM_MULTI_PAN_FORKS;

uint8_t blackListedIdByteArray[NUM_MULTI_PAN_FORKS] = { 0 };
EmberMessageBuffer blacklistedIdsArray[NUM_MULTI_PAN_FORKS] = { EMBER_NULL_MESSAGE_BUFFER };
uint8_t panConflictMeters[NUM_MULTI_PAN_FORKS];
uint16_t trustCenterPolicies[NUM_MULTI_PAN_FORKS];

//------------------------------------------------------------------------------
// Multi PAN events

// In multi-PAN, we fork events that are required for both ZC and ZR networks.
// Each index of the event array maps 1-to-1 to the network index.
// For multi-network and single network, we only allocate 1 copy
// of the event (i.e. an array of element size 1). App Builder always defines
// EMBER_AF_ZC_AND_ZR_DEVICE_COUNT to the correct value.

// The following are not resizable, and we especially don't want their addresses
// to change, so we don't allocate them from emAvailableMemory

#define NUM_MULTI_PAN_FORKED_EVENTS 10
uint8_t emNumMultiPanForkedEvents = NUM_MULTI_PAN_FORKED_EVENTS;

EmberEventData emMultiPanEventData[(NUM_MULTI_PAN_FORKED_EVENTS * NUM_MULTI_PAN_FORKS) + 1];
EmberEventControl emBeaconEventControls[NUM_MULTI_PAN_FORKS];
EmberEventControl emPermitJoiningEventControls[NUM_MULTI_PAN_FORKS];
EmberEventControl emNetworkManagementEventControls[NUM_MULTI_PAN_FORKS];
EmberEventControl emNeighborExchangeEventControls[NUM_MULTI_PAN_FORKS];
EmberEventControl emRequestKeyEventControls[NUM_MULTI_PAN_FORKS];
EmberEventControl emSendParentAnnounceEventControls[NUM_MULTI_PAN_FORKS];
EmberEventControl emTransientLinkKeyEventControls[NUM_MULTI_PAN_FORKS];
EmberEventControl emGpTxEventControls[NUM_MULTI_PAN_FORKS];
EmberEventControl emSourceRouteUpdateEventControls[NUM_MULTI_PAN_FORKS];
EmberEventControl emAllowTcRejoinsUsingWellKnownKeyEventControls[NUM_MULTI_PAN_FORKS];

//------------------------------------------------------------------------------
// Packet Buffers

uint8_t emPacketBufferCount = EMBER_PACKET_BUFFER_COUNT;

// The actual memory for buffers.
uint16_t *heapMemory = (uint16_t *)&emAvailableMemory[0];
//uint8_t *emPacketBufferData = &emAvailableMemory[0];

#define END_emPacketBufferQueueLinks \
  (align(EMBER_PACKET_BUFFER_COUNT * 32))
uint32_t heapMemorySize = END_emPacketBufferQueueLinks;

//------------------------------------------------------------------------------
// MAC Layer

const EmberMacFilterMatchData zigbeeMacFilterList[] = {
#ifdef EMBER_AF_PLUGIN_GP_LIBRARY
  (EMBER_MAC_FILTER_MATCH_ON_PAN_DEST_BROADCAST
   | EMBER_MAC_FILTER_MATCH_ON_PAN_SOURCE_NONE
   | EMBER_MAC_FILTER_MATCH_ON_DEST_BROADCAST_SHORT
   | EMBER_MAC_FILTER_MATCH_ON_SOURCE_NONE),
  (EMBER_MAC_FILTER_MATCH_ON_PAN_DEST_BROADCAST
   | EMBER_MAC_FILTER_MATCH_ON_PAN_SOURCE_LOCAL
   | EMBER_MAC_FILTER_MATCH_ON_DEST_BROADCAST_SHORT
   | EMBER_MAC_FILTER_MATCH_ON_SOURCE_LONG),
#endif // EMBER_AF_PLUGIN_GP_LIBRARY
#ifdef EMBER_AF_PLUGIN_ZLL_LIBRARY
  // ZLL Scan requests
  (EMBER_MAC_FILTER_MATCH_ON_PAN_DEST_BROADCAST
   | EMBER_MAC_FILTER_MATCH_ON_PAN_SOURCE_NON_LOCAL
   | EMBER_MAC_FILTER_MATCH_ON_DEST_BROADCAST_SHORT
   | EMBER_MAC_FILTER_MATCH_ON_SOURCE_LONG),
  (EMBER_MAC_FILTER_MATCH_ON_PAN_DEST_BROADCAST
   | EMBER_MAC_FILTER_MATCH_ON_PAN_SOURCE_LOCAL
   | EMBER_MAC_FILTER_MATCH_ON_DEST_BROADCAST_SHORT
   | EMBER_MAC_FILTER_MATCH_ON_SOURCE_LONG),

  // All other incoming inter-pan ZLL messages
  (EMBER_MAC_FILTER_MATCH_ON_PAN_DEST_BROADCAST
   | EMBER_MAC_FILTER_MATCH_ON_PAN_SOURCE_NON_LOCAL
   | EMBER_MAC_FILTER_MATCH_ON_DEST_UNICAST_LONG
   | EMBER_MAC_FILTER_MATCH_ON_SOURCE_LONG),
  (EMBER_MAC_FILTER_MATCH_ON_PAN_DEST_BROADCAST
   | EMBER_MAC_FILTER_MATCH_ON_PAN_SOURCE_LOCAL
   | EMBER_MAC_FILTER_MATCH_ON_DEST_UNICAST_LONG
   | EMBER_MAC_FILTER_MATCH_ON_SOURCE_LONG),
#endif // EMBER_AF_PLUGIN_ZLL_LIBRARY
  EMBER_MAC_FILTER_MATCH_END
};

// Pass a pointer to child table in memory to the unified mac layer.
sl_mac_child_entry_t *emChildTableData = (sl_mac_child_entry_t *) &emAvailableMemory[END_emPacketBufferQueueLinks];
uint8_t emberChildTableSize = EMBER_CHILD_TABLE_SIZE;
#define END_emChildTableData                 \
  (END_emPacketBufferQueueLinks              \
   + align(EMBER_CHILD_TABLE_SIZE            \
           * EMBER_AF_ZC_AND_ZR_DEVICE_COUNT \
           * sizeof(sl_mac_child_entry_t)))

//------------------------------------------------------------------------------
// NWK Layer

#ifdef EMBER_DISABLE_RELAY
uint8_t emAllowRelay = false;
#else
uint8_t emAllowRelay = true;
#endif

uint16_t *emChildStatusData = (uint16_t *) &emAvailableMemory[END_emChildTableData];
#define END_emChildStatusData                \
  (END_emChildTableData                      \
   + align(EMBER_CHILD_TABLE_SIZE            \
           * EMBER_AF_ZC_AND_ZR_DEVICE_COUNT \
           * sizeof(uint16_t)))

uint32_t *emChildTimersData = (uint32_t *) &emAvailableMemory[END_emChildStatusData];
#define END_emChildTimersData                \
  (END_emChildStatusData                     \
   + align(EMBER_CHILD_TABLE_SIZE            \
           * EMBER_AF_ZC_AND_ZR_DEVICE_COUNT \
           * sizeof(uint32_t)))

int8_t *emChildPower = (int8_t *) &emAvailableMemory[END_emChildTimersData];
#define END_emChildPower \
  (END_emChildTimersData + align(EMBER_CHILD_TABLE_SIZE * sizeof(int8_t)))

// emChildLqi >> 8 keeps the average Lqi values mapped from 0-255 to 1-255 after initialization
// LQI data (0-255) is stored in a 16 bit variable so we end up with greater
// precision after the weighted averaging operation
uint16_t *emChildLqiData = (uint16_t *) &emAvailableMemory[END_emChildPower];
#define END_emChildLqiData                   \
  (END_emChildPower                          \
   + align(EMBER_CHILD_TABLE_SIZE            \
           * EMBER_AF_ZC_AND_ZR_DEVICE_COUNT \
           * sizeof(uint16_t)))

EmRouteTableEntry *emRouteData = (EmRouteTableEntry *) &emAvailableMemory[END_emChildLqiData];
uint8_t emRouteTableSize = EMBER_ROUTE_TABLE_SIZE;
#define END_emRouteData \
  (END_emChildLqiData + align(EMBER_ROUTE_TABLE_SIZE * sizeof(EmRouteTableEntry)))

// Duplicate the table for multi PAN mode depending on how many ZC and ZR devices are present.
uint8_t *emRouteRecordTableData = (uint8_t *) &emAvailableMemory[END_emRouteData];
#define END_emRouteRecordTableData                                                      \
  (END_emRouteData + align(((EMBER_CHILD_TABLE_SIZE + 7) >> 3) * EMBER_ROUTE_TABLE_SIZE \
                           * EMBER_AF_ZC_AND_ZR_DEVICE_COUNT))

EmDiscoveryTableEntry *emDiscoveryTable = (EmDiscoveryTableEntry *) &emAvailableMemory[END_emRouteRecordTableData];
uint8_t emDiscoveryTableSize = EMBER_DISCOVERY_TABLE_SIZE;
#define END_emDiscoveryTable \
  (END_emRouteRecordTableData + align(EMBER_DISCOVERY_TABLE_SIZE * sizeof(EmDiscoveryTableEntry)))

EmberMulticastTableEntry *emberMulticastTable = (EmberMulticastTableEntry *) &emAvailableMemory[END_emDiscoveryTable];
uint8_t emberMulticastTableSize = EMBER_MULTICAST_TABLE_SIZE;
#define END_emberMulticastTable \
  (END_emDiscoveryTable + align(EMBER_MULTICAST_TABLE_SIZE * sizeof(EmberMulticastTableEntry)))

// Broadcast table exists in all device types so we allocate memory to it even if no ZC and ZR device present.
// Duplicate the table if more than one ZC and ZR devices present.
EmBroadcastTableEntry *emBroadcastTableData = (EmBroadcastTableEntry *) &emAvailableMemory[END_emberMulticastTable];
#define END_emberBroadcastTable \
  (END_emberMulticastTable + align(EMBER_BROADCAST_TABLE_SIZE * sizeof(EmBroadcastTableEntry) * NUM_MULTI_PAN_FORKS))
uint8_t emBroadcastTableSize = EMBER_BROADCAST_TABLE_SIZE;

//------------------------------------------------------------------------------
// Network descriptor (multi-network support)

#if !defined(EMBER_MULTI_NETWORK_STRIPPED)
EmberNetworkInfo* emNetworkDescriptor = (EmberNetworkInfo *) &emAvailableMemory[END_emberBroadcastTable];
uint8_t emSupportedNetworks = EMBER_SUPPORTED_NETWORKS;
#define END_emNetworkDescriptor \
  (END_emberBroadcastTable + align((EMBER_SUPPORTED_NETWORKS) *sizeof(EmberNetworkInfo)))

// PAN info exists in all device types so we allocate memory to it even if no ZC and ZR device present.
// Duplicate it if more than one ZC and ZR devices present.
EmPanInfo * emPanInfoData = (EmPanInfo *) &emAvailableMemory[END_emNetworkDescriptor];
#define END_emPanInfoData \
  (END_emNetworkDescriptor + align(sizeof(EmPanInfo) * NUM_MULTI_PAN_FORKS))
#else
uint8_t emSupportedNetworks = 1;
#define END_emPanInfoData END_emberBroadcastTable
#endif // !defined(EMBER_MULTI_NETWORK_STRIPPED)

//------------------------------------------------------------------------------
// Neighbor Table
// Neighbor and frame counter table exist on end device as well but it contains only one entry.
// Duplicate neighbor and frame counter tables per ZC and ZR device.
// If there is no ZC and ZR device present, then allocate one entry for each end device.
EmNeighborTableEntry *emNeighborData = (EmNeighborTableEntry *) &emAvailableMemory[END_emPanInfoData];
uint8_t emRouterNeighborTableSize = EMBER_NEIGHBOR_TABLE_SIZE;
#define END_emNeighborData                                                                  \
  (END_emPanInfoData + align(((EMBER_NEIGHBOR_TABLE_SIZE * EMBER_AF_ZC_AND_ZR_DEVICE_COUNT) \
                              + EMBER_SUPPORTED_NETWORKS) * sizeof(EmNeighborTableEntry)))

uint32_t *emFrameCountersTable = (uint32_t *) &emAvailableMemory[END_emNeighborData];
#define END_emFrameCountersTable                                                                                      \
  (END_emNeighborData + align(((EMBER_NEIGHBOR_TABLE_SIZE + EMBER_CHILD_TABLE_SIZE) * EMBER_AF_ZC_AND_ZR_DEVICE_COUNT \
                               + EMBER_SUPPORTED_NETWORKS) * sizeof(uint32_t)))

//------------------------------------------------------------------------------
// NWK Retry Queue

EmRetryQueueEntry *emRetryQueue = (EmRetryQueueEntry *) &emAvailableMemory[END_emFrameCountersTable];
uint8_t emRetryQueueSize = EMBER_RETRY_QUEUE_SIZE;
#define END_emRetryQueue \
  (END_emFrameCountersTable + align(EMBER_RETRY_QUEUE_SIZE * sizeof(EmRetryQueueEntry)))

// NWK Store And Forward Queue

EmStoreAndForwardQueueEntry *emStoreAndForwardQueue = (EmStoreAndForwardQueueEntry *) &emAvailableMemory[END_emRetryQueue];
uint8_t emStoreAndForwardQueueSize = EMBER_STORE_AND_FORWARD_QUEUE_SIZE;
#define END_emStoreAndForwardQueue \
  (END_emRetryQueue + align(EMBER_STORE_AND_FORWARD_QUEUE_SIZE * sizeof(EmStoreAndForwardQueueEntry)))

//------------------------------------------------------------------------------
// Green Power stack tables
uint8_t emGpIncomingFCTokenTableSize = EMBER_GP_INCOMING_FC_TOKEN_TABLE_SIZE;
uint8_t emGpIncomingFCTokenTimeout = EMBER_GP_INCOMING_FC_TOKEN_TIMEOUT;

EmberGpProxyTableEntry *emGpProxyTable = (EmberGpProxyTableEntry *) &emAvailableMemory[END_emStoreAndForwardQueue];
uint8_t emGpProxyTableSize = EMBER_GP_PROXY_TABLE_SIZE;
#define END_emGpProxyTable \
  (END_emStoreAndForwardQueue + align(EMBER_GP_PROXY_TABLE_SIZE * sizeof(EmberGpProxyTableEntry)))

uint8_t emGpIncomingFCInSinkTokenTableSize = EMBER_GP_INCOMING_FC_IN_SINK_TOKEN_TABLE_SIZE;
uint8_t emGpIncomingFCInSinkTokenTimeout = EMBER_GP_INCOMING_FC_IN_SINK_TOKEN_TIMEOUT;

EmberGpSinkTableEntry *emGpSinkTable = (EmberGpSinkTableEntry *) &emAvailableMemory[END_emGpProxyTable];
uint8_t emGpSinkTableSize = EMBER_GP_SINK_TABLE_SIZE;
#define END_emGpSinkTable \
  (END_emGpProxyTable + align(EMBER_GP_SINK_TABLE_SIZE * sizeof(EmberGpSinkTableEntry)))

//------------------------------------------------------------------------------
// Source routing

#ifdef EMBER_AF_PLUGIN_SOURCE_ROUTE_LIBRARY

// Source Route info
uint8_t sourceRouteTableEntryCount[NUM_MULTI_PAN_FORKS];
uint8_t sourceRouteTableNewestIndex[NUM_MULTI_PAN_FORKS];
uint16_t sourceRouteUpdateConcentratorType[NUM_MULTI_PAN_FORKS];
uint16_t sourceRouteUpdateMinQS[NUM_MULTI_PAN_FORKS];
uint16_t sourceRouteUpdateMaxQS[NUM_MULTI_PAN_FORKS];
uint8_t sourceRouteUpdateRouteErrorThreshold[NUM_MULTI_PAN_FORKS];
uint8_t sourceRouteUpdateDeliveryFailureThreshold[NUM_MULTI_PAN_FORKS];
uint8_t sourceRouteUpdateMaxHops[NUM_MULTI_PAN_FORKS];
uint8_t sourceRouteUpdateRouteErrorCount[NUM_MULTI_PAN_FORKS];
uint8_t sourceRouteUpdateDeliveryFailureCount[NUM_MULTI_PAN_FORKS];
EmberNodeId sourceRouteUpdateNodeIdWithBrokenRoute[NUM_MULTI_PAN_FORKS];
bool sourceRouteUpdateSendAddressDiscoveryNextEvent[NUM_MULTI_PAN_FORKS];
bool sourceRouteUpdateActiveEvent[NUM_MULTI_PAN_FORKS];

SourceRouteTableEntry *emSourceRouteTableData = (SourceRouteTableEntry *) &emAvailableMemory[END_emGpSinkTable];
uint8_t emSourceRouteTableSize = EMBER_SOURCE_ROUTE_TABLE_SIZE;
#define END_emSourceRouteTable \
  (END_emGpSinkTable + align(EMBER_SOURCE_ROUTE_TABLE_SIZE * sizeof(SourceRouteTableEntry) * NUM_MULTI_PAN_FORKS))

#else
SourceRouteTableEntry *emSourceRouteTableData = NULL;
uint8_t emSourceRouteTableSize = 0;
#define END_emSourceRouteTable END_emGpSinkTable
#endif // EMBER_AF_PLUGIN_SOURCE_ROUTE_LIBRARY

//------------------------------------------------------------------------------
// Binding Table

uint16_t *emBindingRemoteNode = (uint16_t *) &emAvailableMemory[END_emSourceRouteTable];
uint8_t emberBindingTableSize = EMBER_BINDING_TABLE_SIZE;
#define END_emBindingRemoteNode \
  (END_emSourceRouteTable + align(EMBER_BINDING_TABLE_SIZE * sizeof(uint16_t)))

uint8_t *emBindingFlags = &emAvailableMemory[END_emBindingRemoteNode];
#define END_emBindingFlags \
  (END_emBindingRemoteNode + align(EMBER_BINDING_TABLE_SIZE))

//------------------------------------------------------------------------------
//End Device Timeouts

uint8_t *emEndDeviceTimeoutData = &emAvailableMemory[END_emBindingFlags];
#define END_emEndDeviceTimeoutData           \
  (END_emBindingFlags                        \
   + align(EMBER_CHILD_TABLE_SIZE            \
           * EMBER_AF_ZC_AND_ZR_DEVICE_COUNT \
           * sizeof(uint8_t)))

//------------------------------------------------------------------------------

// APS Layer

uint8_t emAddressTableSize = EMBER_ADDRESS_TABLE_SIZE;
EmAddressTableEntry *emAddressTable = (EmAddressTableEntry *) &emAvailableMemory[END_emEndDeviceTimeoutData];
#define END_emAddressTable \
  (END_emEndDeviceTimeoutData + align(EMBER_ADDRESS_TABLE_SIZE * sizeof(EmAddressTableEntry)))

uint8_t emMaxApsUnicastMessages = EMBER_APS_UNICAST_MESSAGE_COUNT;
EmApsUnicastMessageData *emApsUnicastMessageData = (EmApsUnicastMessageData *) &emAvailableMemory[END_emAddressTable];
#define END_emApsUnicastMessageData \
  (END_emAddressTable + align(EMBER_APS_UNICAST_MESSAGE_COUNT * sizeof(EmApsUnicastMessageData)))

uint16_t emberApsAckTimeoutMs =
  ((EMBER_APSC_MAX_ACK_WAIT_HOPS_MULTIPLIER_MS
    * EMBER_MAX_HOPS)
   + EMBER_APSC_MAX_ACK_WAIT_TERMINAL_SECURITY_MS);

uint8_t emFragmentDelayMs = EMBER_FRAGMENT_DELAY_MS;
uint8_t emberFragmentWindowSize = EMBER_FRAGMENT_WINDOW_SIZE;

uint8_t emberKeyTableSize = EMBER_KEY_TABLE_SIZE;
uint32_t* emIncomingApsFrameCounters = (uint32_t*)&emAvailableMemory[END_emApsUnicastMessageData];
#define END_emIncomingApsFrameCounters \
  (END_emApsUnicastMessageData + align(EMBER_KEY_TABLE_SIZE * sizeof(uint32_t)))

EmberTcLinkKeyRequestPolicy emberTrustCenterLinkKeyRequestPolicies[NUM_MULTI_PAN_FORKS];
EmberAppLinkKeyRequestPolicy emberAppLinkKeyRequestPolicies[NUM_MULTI_PAN_FORKS];

uint8_t emCertificateTableSize = EMBER_CERTIFICATE_TABLE_SIZE;

uint8_t emAppZdoConfigurationFlags =
  0

// Define this in order to receive supported ZDO request messages via
// the incomingMessageHandler callback.  A supported ZDO request is one that
// is handled by the EmberZNet stack.  The stack will continue to handle the
// request and send the appropriate ZDO response even if this configuration
// option is enabled.
#ifdef EMBER_APPLICATION_RECEIVES_SUPPORTED_ZDO_REQUESTS
  | EMBER_APP_RECEIVES_SUPPORTED_ZDO_REQUESTS
#endif

// Define this in order to receive unsupported ZDO request messages via
// the incomingMessageHandler callback.  An unsupported ZDO request is one that
// is not handled by the EmberZNet stack, other than to send a 'not supported'
// ZDO response.  If this configuration option is enabled, the stack will no
// longer send any ZDO response, and it is the application's responsibility
// to do so.  To see if a response is required, the application must check
// the APS options bitfield within the emberIncomingMessageHandler callback to see
// if the EMBER_APS_OPTION_ZDO_RESPONSE_REQUIRED flag is set.
#ifdef EMBER_APPLICATION_HANDLES_UNSUPPORTED_ZDO_REQUESTS
  | EMBER_APP_HANDLES_UNSUPPORTED_ZDO_REQUESTS
#endif

// Define this in order to receive the following ZDO request
// messages via the emberIncomingMessageHandler callback: SIMPLE_DESCRIPTOR_REQUEST,
// MATCH_DESCRIPTORS_REQUEST, and ACTIVE_ENDPOINTS_REQUEST.  If this
// configuration option is enabled, the stack will no longer send any ZDO
// response, and it is the application's responsibility to do so.
// To see if a response is required, the application must check
// the APS options bitfield within the emberIncomingMessageHandler callback to see
// if the EMBER_APS_OPTION_ZDO_RESPONSE_REQUIRED flag is set.
#ifdef EMBER_APPLICATION_HANDLES_ENDPOINT_ZDO_REQUESTS
  | EMBER_APP_HANDLES_ZDO_ENDPOINT_REQUESTS
#endif

// Define this in order to receive the following ZDO request
// messages via the emberIncomingMessageHandler callback: BINDING_TABLE_REQUEST,
// BIND_REQUEST, and UNBIND_REQUEST.  If this
// configuration option is enabled, the stack will no longer send any ZDO
// response, and it is the application's responsibility to do so.
// To see if a response is required, the application must check
// the APS options bitfield within the emberIncomingMessageHandler callback
// to see if the EMBER_APS_OPTION_ZDO_RESPONSE_REQUIRED flag is set.
#ifdef EMBER_APPLICATION_HANDLES_BINDING_ZDO_REQUESTS
  | EMBER_APP_HANDLES_ZDO_BINDING_REQUESTS
#endif
;

uint16_t emberTransientKeyTimeoutS = EMBER_TRANSIENT_KEY_TIMEOUT_S;

// This configuration is for non trust center node to assume a
// concentrator type of the trust center it join to, until it receive
// many-to-one route request from the trust center. For the trust center
// node, concentrator type is configured from the concentrator plugin.
// The stack by default assumes trust center be a low RAM concentrator.
// By setting to a low RAM concentrator, other devices send route record
// to the trust center even without receiving many-to-one route request.
EmberAssumeTrustCenterConcentratorType emberAssumedTrustCenterConcentratorType =
  EMBER_ASSUME_TRUST_CENTER_IS_LOW_RAM_CONCENTRATOR;
//------------------------------------------------------------------------------
// Memory Allocation
#ifndef RESERVED_AVAILABLE_MEMORY
  #define RESERVED_AVAILABLE_MEMORY 0
#endif
#define END_stackMemory  END_emIncomingApsFrameCounters + RESERVED_AVAILABLE_MEMORY

// On the XAP2B platform, emAvailableMemory is allocated automatically to fill
// the available space. On other platforms, we must allocate it here.
#if defined (CORTEXM3)
VAR_AT_SEGMENT(NO_STRIPPING uint8_t emAvailableMemory[END_stackMemory], __EMHEAP__);
#elif defined(EMBER_TEST)
#ifndef SIMULATED_EXTRA_MEMORY
#define SIMULATED_EXTRA_MEMORY 0
#endif
uint8_t emAvailableMemory[END_stackMemory + SIMULATED_EXTRA_MEMORY];
const uint16_t emAvailableMemorySize = END_stackMemory + SIMULATED_EXTRA_MEMORY;
#else
  #error "Unknown platform."
#endif

void emCheckAvailableMemory(void)
{
}

// *****************************************
// Non-dynamically configurable structures
// *****************************************
const uint8_t emTaskCount = EMBER_TASK_COUNT;
EmberTaskControl emTasks[EMBER_TASK_COUNT];

// *****************************************
// Stack Profile Parameters
// *****************************************

const uint8_t emberStackProfileId[8] = { 0, };

uint8_t emDefaultStackProfile = EMBER_STACK_PROFILE;
uint8_t emDefaultSecurityLevel = EMBER_SECURITY_LEVEL;
uint8_t emMaxEndDeviceChildren = EMBER_MAX_END_DEVICE_CHILDREN;
uint8_t emMaxHops = EMBER_MAX_HOPS;
uint16_t emberMacIndirectTimeout = EMBER_INDIRECT_TRANSMISSION_TIMEOUT;
uint8_t emberEndDevicekeepAliveSupportMode = EMBER_END_DEVICE_KEEP_ALIVE_SUPPORT_MODE;
uint8_t emberEndDevicePollTimeout = EMBER_END_DEVICE_POLL_TIMEOUT;
STATIC_ASSERT(EMBER_END_DEVICE_POLL_TIMEOUT <= MINUTES_16384, "End device timeout out of range");
uint16_t emberLinkPowerDeltaInterval = EMBER_LINK_POWER_DELTA_INTERVAL;
uint8_t emEndDeviceBindTimeout = EMBER_END_DEVICE_BIND_TIMEOUT;
uint8_t emRequestKeyTimeout = EMBER_REQUEST_KEY_TIMEOUT;
uint8_t emPanIdConflictReportThreshold = EMBER_PAN_ID_CONFLICT_REPORT_THRESHOLD;
uint8_t emZcAndZrCount = EMBER_AF_ZC_AND_ZR_DEVICE_COUNT;
uint8_t emZigbeeMaxNetworkRetries = EMBER_ZIGBEE_NUM_NETWORK_RETRIES_DEFAULT;

#ifndef EMBER_NO_STACK
uint8_t emEndDeviceConfiguration = EMBER_END_DEVICE_CONFIG_PERSIST_DATA_ON_PARENT;
#endif

// Normally multicasts do NOT go to the sleepy address (0xFFFF), they go to
// RxOnWhenIdle=true (0xFFFD).  This can be changed, but doing so is not
// ZigBee Pro Compliant and is possibly NOT interoperable.
bool emSendMulticastsToSleepyAddress = EMBER_SEND_MULTICASTS_TO_SLEEPY_ADDRESS;

// *****************************************
// ZigBee Light Link
// *****************************************

EmberZllPolicy emZllPolicy = EMBER_ZLL_POLICY_DISABLED;

// The number of groups required by the ZLL application.
uint8_t emZllGroupAddressesNeeded = EMBER_ZLL_GROUP_ADDRESSES;
int8_t emZllRssiThreshold = EMBER_ZLL_RSSI_THRESHOLD;
#ifdef EMBER_ZLL_APPLY_THRESHOLD_TO_ALL_INTERPANS
bool emZllApplyThresholdToAllInterpans = true;
#else
bool emZllApplyThresholdToAllInterpans = false;
#endif

// *****************************************
// Convenience Stubs
// *****************************************

// This macro is #define'd at the top of NCP framework applications.
// Since appbuilder generates stubs to callbacks, these convinience stubs
// are not needed for framework apps.
#ifndef __NCP_CONFIG__

#ifndef EMBER_APPLICATION_HAS_TRUST_CENTER_JOIN_HANDLER
EmberJoinDecision emberDefaultTrustCenterDecision = EMBER_USE_PRECONFIGURED_KEY;

EmberJoinDecision emberTrustCenterJoinHandler(EmberNodeId newNodeId,
                                              EmberEUI64 newNodeEui64,
                                              EmberDeviceUpdate status,
                                              EmberNodeId parentOfNewNode)
{
  //emzigbee-241-4
#if defined(EMBER_AF_PLUGIN_CONCENTRATOR)
  if (emberDefaultTrustCenterDecision != EMBER_DENY_JOIN
      && parentOfNewNode != emberGetNodeId()) {
    // stodo: not sure about the condition above,
    // because we should get here only if the new child is not our
    // direct child and it is multiple hops away?
    bool deviceLeft = (status == EMBER_DEVICE_LEFT);
    emChangeSourceRouteEntry(newNodeId, parentOfNewNode, false, deviceLeft);
  }
#endif

  if (status == EMBER_STANDARD_SECURITY_SECURED_REJOIN
      || status == EMBER_DEVICE_LEFT) {
    return EMBER_NO_ACTION;
  }

  return emberDefaultTrustCenterDecision;
}
#endif

#ifndef EMBER_APPLICATION_HAS_SWITCH_KEY_HANDLER
void emberSwitchNetworkKeyHandler(uint8_t sequenceNumber)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_ZIGBEE_KEY_ESTABLISHMENT_HANDLER
void emberZigbeeKeyEstablishmentHandler(EmberEUI64 partner, EmberKeyStatus status)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_CHILD_JOIN_HANDLER
void emberChildJoinHandler(uint8_t index, bool joining)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_POLL_COMPLETE_HANDLER
void emberPollCompleteHandler(EmberStatus status)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_BOOTLOAD_HANDLERS
void emberIncomingBootloadMessageHandler(EmberEUI64 longId,
                                         EmberMessageBuffer message)
{
}
void emberBootloadTransmitCompleteHandler(EmberMessageBuffer message,
                                          EmberStatus status)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_MAC_FILTER_MATCH_MESSAGE_HANDLER
void emberMacFilterMatchMessageHandler(const EmberMacFilterMatchStruct* macFilterMatchStruct)
{
  emberMacPassthroughMessageHandler(macFilterMatchStruct->legacyPassthroughType,
                                    macFilterMatchStruct->message);
}
#endif

#ifndef EMBER_APPLICATION_HAS_MAC_PASSTHROUGH_HANDLER
void emberMacPassthroughMessageHandler(EmberMacPassthroughType messageType,
                                       EmberMessageBuffer message)
{
}
#endif
#ifndef EMBER_APPLICATION_HAS_RAW_HANDLER
void emberRawTransmitCompleteHandler(EmberMessageBuffer message,
                                     EmberStatus status)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_INCOMING_MFG_TEST_MESSAGE_HANDLER
void emberIncomingMfgTestMessageHandler(uint8_t messageType,
                                        uint8_t dataLength,
                                        uint8_t *data)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_ENERGY_SCAN_RESULT_HANDLER
void emberEnergyScanResultHandler(uint8_t channel, int8_t maxRssiValue)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_DEBUG_HANDLER
void emberDebugHandler(EmberMessageBuffer message)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_POLL_HANDLER
void emberPollHandler(EmberNodeId childId, bool transmitExpected)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_REMOTE_BINDING_HANDLER
EmberStatus emberRemoteSetBindingHandler(EmberBindingTableEntry *entry)
{
  // Don't let anyone mess with our bindings.
  return EMBER_INVALID_BINDING_INDEX;
}
EmberStatus emberRemoteDeleteBindingHandler(uint8_t index)
{
  // Don't let anyone mess with our bindings.
  return EMBER_INVALID_BINDING_INDEX;
}
#endif

#ifndef EMBER_APPLICATION_HAS_INCOMING_MANY_TO_ONE_ROUTE_REQUEST_HANDLER
void emberIncomingManyToOneRouteRequestHandler(EmberNodeId source,
                                               EmberEUI64 longId,
                                               uint8_t cost)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_INCOMING_ROUTE_ERROR_HANDLER
void emberIncomingRouteErrorHandler(EmberStatus status,
                                    EmberNodeId target)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_INCOMING_NETWORK_STATUS_HANDLER
void emberIncomingNetworkStatusHandler(uint8_t errorCode,
                                       EmberNodeId target)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_SOURCE_ROUTING //DEPRECATED
//Deprecated
void emberIncomingRouteRecordHandler(EmberNodeId source,
                                     EmberEUI64 sourceEui,
                                     uint8_t relayCount,
                                     EmberMessageBuffer header,
                                     uint8_t relayListIndex)
{
}
//Deprecated
uint8_t emberAppendSourceRouteHandler(EmberNodeId destination,
                                      EmberMessageBuffer header)
{
  return 0;
}
#endif

#ifndef EMBER_APPLICATION_HAS_OVERRIDE_SOURCE_ROUTING
void emberOverrideIncomingRouteRecordHandler(EmberNodeId source,
                                             EmberEUI64 sourceEui,
                                             uint8_t relayCount,
                                             EmberMessageBuffer header,
                                             uint8_t relayListIndex,
                                             bool* consumed)
{
  emberIncomingRouteRecordHandler(source,
                                  sourceEui,
                                  relayCount,
                                  header,
                                  relayListIndex);
}

uint8_t emberOverrideAppendSourceRouteHandler(EmberNodeId destination,
                                              EmberMessageBuffer* header,
                                              bool* consumed)
{
  return 0;
}

void emberOverrideFurthurIndexForSourceRouteAddEntryHandler(EmberNodeId id, uint8_t* furtherIndex)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_GET_ENDPOINT
uint8_t emberGetEndpoint(uint8_t index)
{
  return emberEndpoints[index].endpoint;
}

bool emberGetEndpointDescription(uint8_t endpoint,
                                 EmberEndpointDescription *result)
{
  uint8_t i;
  EmberEndpoint *endpoints = emberEndpoints;
  for (i = 0; i < emberEndpointCount; i++, endpoints++) {
    if (endpoints->endpoint == endpoint) {
      EmberEndpointDescription const * d = endpoints->description;
      result->profileId                   = d->profileId;
      result->deviceId                    = d->deviceId;
      result->deviceVersion               = d->deviceVersion;
      result->inputClusterCount           = d->inputClusterCount;
      result->outputClusterCount          = d->outputClusterCount;
      return true;
    }
  }
  return false;
}

uint16_t emberGetEndpointCluster(uint8_t endpoint,
                                 EmberClusterListId listId,
                                 uint8_t listIndex)
{
  uint8_t i;
  EmberEndpoint *endpoints = emberEndpoints;
  for (i = 0; i < emberEndpointCount; i++, endpoints++) {
    if (endpoints->endpoint == endpoint) {
      switch (listId) {
        case EMBER_INPUT_CLUSTER_LIST:
          return endpoints->inputClusterList[listIndex];
        case EMBER_OUTPUT_CLUSTER_LIST:
          return endpoints->outputClusterList[listIndex];
        default: {
        }
      }
    }
  }
  return 0;
}

#endif // defined EMBER_APPLICATION_HAS_GET_ENDPOINT

// Inform the application that an orphan notification has been received.
// This is generally not useful for applications. It could be useful in
// testing and is included for this purpose.
#ifndef EMBER_APPLICATION_HAS_ORPHAN_NOTIFICATION_HANDLER
void emberOrphanNotificationHandler(EmberEUI64 longId)
{
  return;
}
#endif

#ifndef EMBER_APPLICATION_HAS_PAN_ID_CONFLICT_HANDLER
// This handler is called by the
//  stack to report the number of conflict reports exceeds
//  EMBER_PAN_ID_CONFLICT_REPORT_THRESHOLD within a period of 1 minute )
EmberStatus emberPanIdConflictHandler(int8_t conflictCount)
{
  return EMBER_SUCCESS;
}
#endif

#ifndef EMBER_APPLICATION_HAS_COUNTER_HANDLER

void emberCounterHandler(EmberCounterType type, EmberCounterInfo info)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_COUNTER_ROLLOVER_HANDLER
void emberCounterRolloverHandler(EmberCounterType type)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_STACK_TOKEN_CHANGED_HANDLER
void emberStackTokenChangedHandler(uint16_t tokenAddress)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_ID_CONFLICT_HANDLER
void emberIdConflictHandler(EmberNodeId conflictingId)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_MAC_PASSTHROUGH_FILTER_HANDLER
bool emberMacPassthroughFilterHandler(uint8_t *macHeader)
{
  return false;
}
#endif

#ifndef EMBER_APPLICATION_HAS_ZLL_ADDRESS_ASSIGNMENT_HANDLER
void emberZllAddressAssignmentHandler(const EmberZllAddressAssignment* addressInfo)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_ZLL_NETWORK_FOUND_HANDLER
void emberZllNetworkFoundHandler(const EmberZllNetwork* networkInfo,
                                 const EmberZllDeviceInfoRecord* deviceInfo)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_ZLL_SCAN_COMPLETE_HANDLER
void emberZllScanCompleteHandler(EmberStatus status)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_ZLL_TOUCH_LINK_TARGET_HANDLER
void emberZllTouchLinkTargetHandler(const EmberZllNetwork* networkInfo)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_CALCULATE_SMACS_HANDLER
void emberCalculateSmacsHandler(EmberStatus status,
                                EmberSmacData* initiatorSmac,
                                EmberSmacData* responderSmac)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_GENERATE_CBKE_KEYS_HANDLER
void emberGenerateCbkeKeysHandler(EmberStatus status,
                                  EmberPublicKeyData *ephemeralPublicKey)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_CALCULATE_SMACS_HANDLER_283K1
void emberCalculateSmacsHandler283k1(EmberStatus status,
                                     EmberSmacData *initiatorSmac,
                                     EmberSmacData *responderSmac)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_GENERATE_CBKE_KEYS_HANDLER_283K1
void emberGenerateCbkeKeysHandler283k1(EmberStatus status,
                                       EmberPublicKey283k1Data *ephemeralPublicKey)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_DSA_SIGN_HANDLER
void emberDsaSignHandler(EmberStatus status,
                         EmberMessageBuffer signedMessage)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_DSA_VERIFY_HANDLER
void emberDsaVerifyHandler(EmberStatus status)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_RTOS_IDLE_HANDLER
bool emberRtosIdleHandler(uint32_t *idleTimeMs)
{
  return false;
}
#endif

#ifndef EMBER_APPLICATION_HAS_RTOS_STACK_WAKEUP_ISR_HANDLER
void emberRtosStackWakeupIsrHandler(void)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_DUTY_CYCLE_HANDLER
void emberDutyCycleHandler(uint8_t channelPage,
                           uint8_t channel,
                           EmberDutyCycleState state)
{
}
#endif
#ifndef EMBER_APPLICATION_HAS_AF_COUNTER_HANDLER
void emberAfCounterHandler(EmberCounterType type, EmberCounterInfo info)
{
}
#endif

#ifndef EMBER_APPLICATION_HAS_BUFFER_MARKER
void emberMarkApplicationBuffers()
{
}
#endif

#ifndef EMBER_AF_PLUGIN_PACKET_HANDOFF
  #if defined(EMBER_CALLBACK_INCOMING_PACKET_FILTER) \
  || defined(EMBER_CALLBACK_OUTGOING_PACKET_FILTER)
    #error "Use of the Packet Filter Callbacks requires the Packet Handoff Plugin"
  #endif
EmberPacketAction emberPacketHandoffIncoming(EmberZigbeePacketType packetType,
                                             EmberMessageBuffer packetBuffer,
                                             uint8_t index,
                                             void *data)
{
  return EMBER_ACCEPT_PACKET;
}

EmberPacketAction emberPacketHandoffOutgoing(EmberZigbeePacketType packetType,
                                             EmberMessageBuffer packetBuffer,
                                             uint8_t index,
                                             void *data)
{
  return EMBER_ACCEPT_PACKET;
}
#endif
#endif /* __NCP_CONFIG__ */

// Stubs that apply to both SoC and NCP

// Temporary change: add stubs to compile NCP images and other apps that don't
// know about the Zigbee Event Logger plugin
#ifndef EMBER_AF_PLUGIN_ZIGBEE_EVENT_LOGGER
 #include "app/framework/plugin/zigbee-event-logger-stub/zigbee-event-logger-stub-gen.c"
#endif // !EMBER_AF_PLUGIN_ZIGBEE_EVENT_LOGGER
