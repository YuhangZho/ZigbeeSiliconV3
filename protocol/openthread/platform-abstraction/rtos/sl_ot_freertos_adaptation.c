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

/*
 * This file is derived from openthread-freertos.c in the ot-rtos github repo:
 * https://github.com/openthread/ot-rtos
 */

#include "sl_ot_freertos_adaptation.h"

#include <assert.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <openthread-core-config.h>
#include <openthread-system.h>
#include <openthread/cli.h>
#include <openthread/diag.h>
#include <openthread/tasklet.h>

#include <mbedtls/platform.h>

static TaskHandle_t      sMainTask     = NULL;
static SemaphoreHandle_t sExternalLock = NULL;
static otInstance *      sInstance     = NULL;

static void mainloop(void *aContext)
{
    otInstance *instance = (otInstance *)aContext;

    otSysProcessDrivers(instance);
    xSemaphoreTake(sExternalLock, portMAX_DELAY);
    while (!otSysPseudoResetWasRequested())
    {
        otTaskletsProcess(instance);
        xSemaphoreGive(sExternalLock);
        if (!otTaskletsArePending(instance)) {
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }
        xSemaphoreTake(sExternalLock, portMAX_DELAY);
        otSysProcessDrivers(instance);
        sl_ot_rtos_application_tick();
    }

    otInstanceFinalize(instance);
    vTaskDelete(NULL);
}

void otrTaskNotifyGive()
{
    if (sMainTask != NULL)
    {
        xTaskNotifyGive(sMainTask);
    }
}

void otrTaskNotifyGiveFromISR()
{
    BaseType_t taskWoken;

    if (sMainTask != NULL)
    {
        vTaskNotifyGiveFromISR(sMainTask, &taskWoken);
        if (taskWoken == pdTRUE)
        {
            taskYIELD();
        }
    }
}

void otTaskletsSignalPending(otInstance *aInstance)
{
    (void)aInstance;
    otrTaskNotifyGive();
}

void otrInit(int argc, char *argv[])
{
    otSysInit(argc, argv);

    sInstance = otInstanceInitSingle();
    assert(sInstance);

    sExternalLock = xSemaphoreCreateMutex();
    assert(sExternalLock != NULL);

    // Moved from otrStart()
    xTaskCreate(mainloop, "ot", 4096, sInstance, 2, &sMainTask);
    OTR_PORT_ENABLE_SLEEP();

    sl_ot_rtos_application_init();
}

void otrStart(void)
{
    vTaskStartScheduler();
}

void otrLock(void)
{
    if (xTaskGetCurrentTaskHandle() != sMainTask)
    {
        xSemaphoreTake(sExternalLock, portMAX_DELAY);
    }
}

void otrUnlock(void)
{
    if (xTaskGetCurrentTaskHandle() != sMainTask)
    {
        xSemaphoreGive(sExternalLock);
    }
}

void otSysEventSignalPending(void)
{
    if (otrPortIsInsideInterrupt())
    {
        otrTaskNotifyGiveFromISR();
    }
    else
    {
        otrTaskNotifyGive();
    }
}

otInstance *otrGetInstance()
{
    return sInstance;
}

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t ** ppxIdleTaskStackBuffer,
                                   uint32_t *     pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t  uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t ** ppxTimerTaskStackBuffer,
                                    uint32_t *     pulTimerTaskStackSize)
{
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t  uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

__WEAK void sl_ot_rtos_application_init(void)
{
}

__WEAK void sl_ot_rtos_application_tick(void)
{
}
