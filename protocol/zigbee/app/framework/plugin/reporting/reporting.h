/***************************************************************************//**
 * @file
 * @brief Definitions for the Reporting plugin.
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
#ifndef REPORTING_H
#define REPORTING_H
#define NULL_INDEX 0xFFFF

#ifdef EMBER_AF_PLUGIN_REPORTING_ENABLE_EXPANDED_TABLE
  #include "platform/emdrv/nvm3/inc/nvm3.h"

  #define REPORTING_TABLE_MAX_RANGE 0x400
  #define REPORTING_TABLE_PLUGIN_SIZE (EMBER_AF_PLUGIN_REPORTING_EXPANDED_TABLE_SIZE)

  #define NVM3KEY_REPORTING_TABLE_EXPANDED (NVM3KEY_DOMAIN_ZIGBEE | 0x6000)
  #define REPORTING_TABLE_EXPANDED_MAX_KEY (NVM3KEY_REPORTING_TABLE_EXPANDED + REPORTING_TABLE_MAX_RANGE - 1)
#else // not expanded
  #define REPORTING_TABLE_MAX_RANGE 127
  #define REPORTING_TABLE_PLUGIN_SIZE (EMBER_AF_PLUGIN_REPORTING_TABLE_SIZE)
#endif

// The default reporting will generate a table that is mandatory
// but user may still allocate some table for adding more reporting over
// the air or by cli as part of reporting plugin.
#ifndef REPORT_TABLE_SIZE
#if defined EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE
#define REPORT_TABLE_SIZE (EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE + REPORTING_TABLE_PLUGIN_SIZE)
#else
#define REPORT_TABLE_SIZE (REPORTING_TABLE_PLUGIN_SIZE)
#endif
#endif

#if REPORT_TABLE_SIZE > REPORTING_TABLE_MAX_RANGE
 #ifndef EMBER_AF_PLUGIN_REPORTING_ENABLE_EXPANDED_TABLE
   #error "Reporting Table total size exceeds the maximum size.  Try enabling the expanded table option"
 #else
   #error "Reporting Table total size exceeds the maximum configuration"
 #endif
#endif

typedef struct {
  uint32_t lastReportTimeMs;
  EmberAfDifferenceType lastReportValue;
  bool reportableChange;
} EmAfPluginReportVolatileData;
extern EmAfPluginReportVolatileData emAfPluginReportVolatileData[];
EmberAfStatus emberAfPluginReportingConfigureReportedAttribute(const EmberAfPluginReportingEntry *newEntry);
void emAfPluginReportingGetEntry(uint16_t index, EmberAfPluginReportingEntry *result);
void emAfPluginReportingSetEntry(uint16_t index, EmberAfPluginReportingEntry *value);
uint16_t emAfPluginReportingAddEntry(EmberAfPluginReportingEntry* newEntry);
EmberStatus emAfPluginReportingRemoveEntry(uint16_t index);
bool emAfPluginReportingDoEntriesMatch(const EmberAfPluginReportingEntry* const entry1,
                                       const EmberAfPluginReportingEntry* const entry2);
uint16_t emAfPluginReportingConditionallyAddReportingEntry(EmberAfPluginReportingEntry* newEntry);
void emberAfPluginReportingLoadReportingConfigDefaults(void);
bool emberAfPluginReportingGetReportingConfigDefaults(EmberAfPluginReportingEntry *defaultConfiguration);

uint16_t emAfPluginReportingNumEntries(void);
uint16_t emAfPluginReportingAppendEntry(EmberAfPluginReportingEntry* newEntry);

#endif //REPORTING_H
