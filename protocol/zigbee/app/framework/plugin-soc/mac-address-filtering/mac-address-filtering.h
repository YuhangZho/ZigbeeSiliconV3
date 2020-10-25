/***************************************************************************//**
 * @file
 * @brief MAC Address Filtering - Public APIs
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

void emberAfPluginMacAddressFilteringInitCallback(void);

void emberAfPluginMacAddressFilteringInitCallback(void);

void emberAfPluginMacAddressFilteringPrintConfigCommand(void);

void emberAfPluginMacAddressFilteringFilterNoAddressCommand(void);

void emberAfPluginMacAddressFilteringAddShortAddressCommand(void);

void emberAfPluginMacAddressFilteringSetShortAddressListType(void);

void emberAfPluginMacAddressFilteringSetLongAddressListType(void);

void emberAfPluginMacAddressFilteringAddLongAddressCommand(void);

void emberAfPluginMacAddressFilteringClearShortAddressList(void);

void emberAfPluginMacAddressFilteringClearLongAddressList(void);

void emberAfPluginMacAddressFilteringAddPanIdCommand(void);

void emberAfPluginMacAddressFilteringPanIdDeleteEntry(void);

void emberAfPluginMacAddressFilteringClearPanIdList(void);

void emberAfPluginMacAddressFilteringSetPanIdListType(void);

void emberAfPluginMacAddressFilteringReset(void);

void emberAfPluginMacAddressFilteringClearAll(void);

void emberAfPluginMacAddressFilteringShortAddressDeleteEntry(void);

void emberAfPluginMacAddressFilteringLongAddressDeleteEntry(void);

EmberStatus emberAfPluginMacAddressFilteringGetStatsForShortAddress(uint8_t index,
                                                                    uint32_t* matchCountForIndexPtr);
void emberAfPluginMacAddressFilteringGetStats(boolean shortMode,
                                              uint32_t* allowedPacketCountPtr,
                                              uint32_t* droppedPacketCountPtr,
                                              uint32_t* totalPacketCountPtr);
