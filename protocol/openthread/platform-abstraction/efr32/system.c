/*
 *  Copyright (c) 2019, The OpenThread Authors.
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
 * @brief
 *   This file includes the platform-specific initializers.
 */

#include <assert.h>
#include <string.h>

#include "openthread-system.h"
#include <openthread/platform/uart.h>

#include "common/logging.hpp"

#include "em_chip.h"
#include "em_core.h"
#include "em_emu.h"
#include "em_system.h"
#include "rail.h"
#include "sl_mpu.h"
#include "sl_sleeptimer.h"
#if OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE
#include "sl_malloc.h"
#include "openthread/heap.h"
#endif

#include "openthread-core-efr32-config.h"
#include "platform-efr32.h"
#include "sl_system_init.h"

#define USE_EFR32_LOG                                                                   \
    ((OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_PLATFORM_DEFINED) || \
     (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_NCP_SPINEL))

otInstance *sInstance;

#ifndef SL_COMPONENT_CATALOG_PRESENT
__WEAK void sl_openthread_init(void)
{
    // Place holder for enabling Silabs specific features available only through Simplicity Studio
}
#endif // SL_COMPONENT_CATALOG_PRESENT

void otSysInit(int argc, char *argv[])
{
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
    sl_status_t status;

    __disable_irq();

#if OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE
    otHeapSetCAllocFree(sl_calloc, sl_free);
#endif
    
#if defined(FREE_RTOS)
  #define PRIGROUP_POSITION (2)  // PPPXX.XXX on EFR32 Series-1 and PPPPX.XXX on Series-2
#else
  #define PRIGROUP_POSITION (4)  // PPP.XXXXX on EFR32 Series-1 and PPP.SXXXX on Series-2
#endif

#undef FIXED_EXCEPTION
#define FIXED_EXCEPTION(vectorNumber, functionName, deviceIrqn, deviceIrqHandler)
#define EXCEPTION(vectorNumber, functionName, deviceIrqn, deviceIrqHandler, priorityLevel, subpriority) \
    NVIC_SetPriority(deviceIrqn, NVIC_EncodePriority(PRIGROUP_POSITION - 1, priorityLevel, subpriority));
#include "nvic-config.h"
#undef EXCEPTION

    NVIC_SetPriorityGrouping(PRIGROUP_POSITION - 1);
    sl_system_init();
    sl_openthread_init();

    status = sl_sleeptimer_init();
    assert(status == SL_STATUS_OK);

    __enable_irq();

#if USE_EFR32_LOG
    efr32LogInit();
#endif
    efr32RadioInit();
    efr32AlarmInit();
    efr32MiscInit();
}

bool otSysPseudoResetWasRequested(void)
{
    return false;
}

void otSysDeinit(void)
{
    efr32RadioDeinit();

#if USE_EFR32_LOG
    efr32LogDeinit();
#endif
}

void otSysProcessDrivers(otInstance *aInstance)
{
    sInstance = aInstance;

    // should sleep and wait for interrupts here

    efr32UartProcess();
    efr32RadioProcess(aInstance);
    efr32AlarmProcess(aInstance);
}

__WEAK void otSysEventSignalPending(void)
{
    // Intentionally empty
}
