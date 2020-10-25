/***************************************************************************//**
 * @file
 * @brief FreeRTOS configurations.
 ******************************************************************************/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#if !defined(__IAR_SYSTEMS_ASM__) // Skip em_assert.h in IAR assembler files
#include "em_assert.h"
#include "em_device.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/********************** Configuration of FreeRTOS ****************************/

/* Implement FreeRTOS configASSERT as emlib assert */
#define configASSERT(x)       EFM_ASSERT(x)

/* Modes of operations of operation system*/
#define configUSE_PREEMPTION       (1)

/* Energy saving modes */
#define configUSE_TICKLESS_IDLE    (1)

#define configTICK_RATE_HZ    (1024)

/* Definition used by Keil to replace default system clock source. */
#define configOVERRIDE_DEFAULT_TICK_CONFIGURATION (1)

/* Main functions*/
#define configMAX_PRIORITIES                      (8)
#define configMINIMAL_STACK_SIZE                  (( unsigned short ) 140)
#define configTOTAL_HEAP_SIZE                     (( size_t )(9000))
#define configMAX_TASK_NAME_LEN                   (10)
#define configUSE_TRACE_FACILITY                  (0)
#define configUSE_16_BIT_TICKS                    (0)
#define configIDLE_SHOULD_YIELD                   (0)
#define configUSE_MUTEXES                         (1)
#define configUSE_RECURSIVE_MUTEXES               (0)
#define configUSE_COUNTING_SEMAPHORES             (0)
#define configQUEUE_REGISTRY_SIZE                 (10)
#define configUSE_QUEUE_SETS                      (0)
#define configSUPPORT_STATIC_ALLOCATION           (1)
#define configSUPPORT_DYNAMIC_ALLOCATION          (1)

/* Hook function related definitions. */
#define configUSE_TICK_HOOK                       (0)
#define configCHECK_FOR_STACK_OVERFLOW            (0)
#define configUSE_MALLOC_FAILED_HOOK              (0)

/* Run time stats gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS             (0)

/* Co-routine related definitions. */
#define configUSE_CO_ROUTINES                     (0)
#define configMAX_CO_ROUTINE_PRIORITIES           (1)

/* Software timer related definitions. */
#define configUSE_TIMERS                          (1)
#define configTIMER_TASK_PRIORITY                 (configMAX_PRIORITIES - 1)   /* Highest priority */
#define configTIMER_QUEUE_LENGTH                  (10)
#define configTIMER_TASK_STACK_DEPTH              (configMINIMAL_STACK_SIZE)

/* Interrupt nesting behaviour configuration. */
#define configKERNEL_INTERRUPT_PRIORITY           (255)
#define configMAX_SYSCALL_INTERRUPT_PRIORITY      (96)   /* equivalent to 0x60, or priority 3. */

/* Optional functions - most linkers will remove unused functions anyway. */
#define INCLUDE_vTaskPrioritySet                  (1)
#define INCLUDE_uxTaskPriorityGet                 (1)
#define INCLUDE_vTaskDelete                       (1)
#define INCLUDE_vTaskSuspend                      (1)
#define INCLUDE_xResumeFromISR                    (1)
#define INCLUDE_vTaskDelayUntil                   (1)
#define INCLUDE_vTaskDelay                        (1)
#define INCLUDE_xTaskGetSchedulerState            (1)
#define INCLUDE_xTaskGetCurrentTaskHandle         (1)
#define INCLUDE_uxTaskGetStackHighWaterMark       (0)
#define INCLUDE_xTaskGetIdleTaskHandle            (0)
#define INCLUDE_xTimerGetTimerDaemonTaskHandle    (0)
#define INCLUDE_pcTaskGetTaskName                 (0)
#define INCLUDE_eTaskGetState                     (0)

/* Default value of CPU clock (RC)*/
#define configCPU_CLOCK_HZ                        (SystemCoreClockGet())

#define configUSE_IDLE_HOOK  (0)

/*-----------------------------------------------------------*/

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
 * standard names. */
#define vPortSVCHandler        SVC_Handler
#define xPortPendSVHandler     PendSV_Handler
#define xPortSysTickHandler    SysTick_Handler

#ifdef __cplusplus
}
#endif
#endif /* FREERTOS_CONFIG_H */
