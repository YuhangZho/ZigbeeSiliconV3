/***************************************************************************//**
 * @file
 * @brief Main entrypoint when using Micrium RTOS.
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
#ifdef EMBER_AF_NCP
#include "app/framework/include/af-ncp.h"
#else // !EMBER_AF_NCP
#include "app/framework/include/af.h"
#endif // EMBER_AF_NCP

#include <kernel/include/os.h>
#include <kernel/include/os_trace.h>

#include "micrium-rtos-zigbee.h"
#include "protocol/bluetooth/inc/sl_bt_stack_init.h"

#if defined(EMBER_AF_PLUGIN_BLE)
#include EMBER_AF_API_BLE
#endif // EMBER_AF_PLUGIN_BLE

#if defined(EMBER_AF_PLUGIN_MBEDTLS_MULTIPROTOCOL)
#include "sl_mbedtls.h"
#endif

#include "sl_bt_rtos_adaptation.h"
#include "sl_bt_mbedtls_context.h"
#include "sl_bluetooth.h"

#ifdef EMBER_AF_NCP
extern int emberAfMain(MAIN_FUNCTION_PARAMETERS);
#endif // EMBER_AF_NCP
//------------------------------------------------------------------------------
// Tasks variables and defines

#define BLE_LINK_LAYER_TASK_PRIORITY            4
#define BLE_STACK_TASK_PRIORITY                 5
#define ZIGBEE_STACK_TASK_PRIORITY              6

static OS_TCB zigbeeTaskControlBlock;
static CPU_STK zigbeeTaskStack[EMBER_AF_PLUGIN_MICRIUM_RTOS_ZIGBEE_STACK_SIZE];

#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1)
static OS_TCB applicationTask1ControlBlock;
static CPU_STK applicationTask1Stack[EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1_STACK_SIZE];
#endif // EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1

#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2)
static OS_TCB applicationTask2ControlBlock;
static CPU_STK applicationTask2Stack[EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2_STACK_SIZE];
#endif // EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2

#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3)
static OS_TCB applicationTask3ControlBlock;
static CPU_STK applicationTask3Stack[EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3_STACK_SIZE];
#endif // EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3

//------------------------------------------------------------------------------
// Event flags and semaphores

OS_FLAG_GRP zigbeeTaskEventFlags;

//------------------------------------------------------------------------------
// Forward and external declarations

static void zigbeeTask(void *p_arg);
static void initMicriumCpu(void);

extern void App_OS_SetAllHooks(void);
//------------------------------------------------------------------------------
// Main

void emAfPluginMicriumRtosSleepInit(void);

int main(MAIN_FUNCTION_PARAMETERS)
{
  RTOS_ERR err;

  CPU_Init();
  halInit();
  initMicriumCpu();
#ifndef EMBER_AF_NCP
  // Not exist on NCP.
  emberAfMainInit();
#endif
  // Initialize tokens in zigbee before we call emberAfMain because we need to
  // upgrade any potential SimEEv2 data to NVM3 before we call gecko_stack_init
  // (a bluetooth call). If we don't, the bluetooth call only initializes NVM3
  // and wipes any SimEEv2 data present (it does not handle upgrading). The
  // second call of halStackInitTokens in the zigbee stack will be a no-op
  if (EMBER_SUCCESS != halStackInitTokens()) {
    assert(false);
  }

  OS_TRACE_INIT();
  OSInit(&err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);

  App_OS_SetAllHooks();

  emAfPluginMicriumRtosSleepInit();

  // Create ZigBee task.
  // Note, this is gonna create the rest of the tasks down the road.
  OSTaskCreate(&zigbeeTaskControlBlock,
               "Zigbee Stack",
               zigbeeTask,
               NULL,
               ZIGBEE_STACK_TASK_PRIORITY,
               &zigbeeTaskStack[0],
               EMBER_AF_PLUGIN_MICRIUM_RTOS_ZIGBEE_STACK_SIZE / 10,
               EMBER_AF_PLUGIN_MICRIUM_RTOS_ZIGBEE_STACK_SIZE,
               0, // Not receiving messages
               0, // Default time quanta
               NULL, // No TCB extensions
               OS_OPT_TASK_STK_CLR | OS_OPT_TASK_STK_CHK,
               &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);

  OSFlagCreate(&zigbeeTaskEventFlags,
               "Zigbee Task Event Flag Group",
               (OS_FLAGS)0,
               &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);

  // Start the OS
  OSStart(&err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);
}

//------------------------------------------------------------------------------
// Internal APIs

// This can be called from ISR.
void emAfPluginMicriumRtosWakeUpZigbeeStackTask(void)
{
  RTOS_ERR err;

  OSFlagPost(&zigbeeTaskEventFlags,
             ZIGBEE_TASK_YIELD,
             (OS_OPT)OS_OPT_POST_FLAG_SET,
             &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);
}

//------------------------------------------------------------------------------
// Implemented callbacks

bool emberRtosIdleHandler(uint32_t *idleTimeMs)
{
  return emberAfPluginIdleSleepRtosCallback(idleTimeMs, false);
}

void emberRtosStackWakeupIsrHandler(void)
{
  emAfPluginMicriumRtosWakeUpZigbeeStackTask();
}

//------------------------------------------------------------------------------
// Static functions

static void zigbeeTask(void *p_arg)
{
#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1) || defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2) || defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3)
  RTOS_ERR err;
#endif //#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1) || defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2) || defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3)

#if defined(EMBER_AF_PLUGIN_MBEDTLS_MULTIPROTOCOL)
  // Call sl_mbedtls_init to initialize Silicon Labs specifics required for
  // proper mbedTLS operation, e.g. the SE Manager and redirection of mbedTLS's
  // threading subsystem to Micrium OS.
  sl_mbedtls_init();
#endif

#if defined(EMBER_AF_PLUGIN_BLE)
  sl_bt_rtos_init();
#endif // EMBER_AF_PLUGIN_BLE

#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1)
  emberAfPluginMicriumRtosAppTask1InitCallback();

  // Create Application Task 1.
  OSTaskCreate(&applicationTask1ControlBlock,
               "Application (1)",
               emberAfPluginMicriumRtosAppTask1MainLoopCallback,
               NULL,
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1_PRIORITY,
               &applicationTask1Stack[0],
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1_STACK_SIZE / 10,
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1_STACK_SIZE,
               0, // Not receiving messages
               0, // Default time quanta
               NULL, // No TCB extensions
               OS_OPT_TASK_STK_CLR | OS_OPT_TASK_STK_CHK,
               &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);

#endif // EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1

#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2)
  emberAfPluginMicriumRtosAppTask2InitCallback();

  // Create Application Task 2.
  OSTaskCreate(&applicationTask2ControlBlock,
               "Application (2)",
               emberAfPluginMicriumRtosAppTask2MainLoopCallback,
               NULL,
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2_PRIORITY,
               &applicationTask2Stack[0],
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2_STACK_SIZE / 10,
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2_STACK_SIZE,
               0, // Not receiving messages
               0, // Default time quanta
               NULL, // No TCB extensions
               OS_OPT_TASK_STK_CLR | OS_OPT_TASK_STK_CHK,
               &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);

#endif // EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK2

#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3)
  emberAfPluginMicriumRtosAppTask3InitCallback();

  // Create Application Task 3.
  OSTaskCreate(&applicationTask3ControlBlock,
               "Application (3)",
               emberAfPluginMicriumRtosAppTask3MainLoopCallback,
               NULL,
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3_PRIORITY,
               &applicationTask3Stack[0],
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3_STACK_SIZE / 10,
               EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3_STACK_SIZE,
               0, // Not receiving messages
               0, // Default time quanta
               NULL, // No TCB extensions
               OS_OPT_TASK_STK_CLR | OS_OPT_TASK_STK_CHK,
               &err);
  assert(RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE);

#endif // EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK3

  emberAfMain();
}

// Register interrupt vectors with the OS
static void initMicriumCpu(void)
{
  // Radio Interrupts can optionally be configured non-kernel aware at this point
}
