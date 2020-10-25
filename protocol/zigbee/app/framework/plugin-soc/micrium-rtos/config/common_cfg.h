/***************************************************************************//**
 * @file
 * @brief Common Configuration Template File
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                               MODULE
 *********************************************************************************************************
 *********************************************************************************************************
 */

#ifndef SILABS_COMMON_CFG_H
#define SILABS_COMMON_CFG_H

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                             INCLUDE FILES
 *********************************************************************************************************
 *********************************************************************************************************
 */

#include  <common/include/lib_def.h>

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                     MEMORY LIBRARY CONFIGURATION
 *********************************************************************************************************
 *********************************************************************************************************
 */

/*
 *********************************************************************************************************
 *                              MEMORY LIBRARY OPTIMIZATION CONFIGURATION
 *
 * Note(s) : (1) Configure LIB_MEM_CFG_STD_C_LIB_EN to enable/disable use of standard C lib (will include
 *               standard <string.h>) for Mem_Set()/memset(), Mem_Copy()/memcpy(), Mem_Move()/memmove(),
 *               Mem_Clr()/memset() and Mem_Cmp()/memcmp() functions.
 *
 *           (2) Configure LIB_MEM_CFG_MEM_COPY_OPTIMIZE_ASM_EN to enable/disable assembly-optimized
 *               memory copy function.
 *
 *           (3) LIB_MEM_CFG_STD_C_LIB_EN and LIB_MEM_CFG_MEM_COPY_OPTIMIZE_ASM_EN cannot be both enabled
 *               at the same time because we cannot know how the memory functions are called within
 *               standard C library.
 *
 *           (4) When using IAR, the implementation provided by the compiler is overridden to use either
 *               Micrium OS' C or assembly implementation (based on LIB_MEM_CFG_MEM_COPY_OPTIMIZE_ASM_EN).
 *               This is a workaround for a known issue with IAR's memcpy not working properly and doing
 *               unaligned accesses to memory. The IAR extensions are required for this workaround to work
 *               properly. It is possible to force the use of IAR's memcpy by #define'ing
 *               LIB_MEM_COPY_FNCT_PREFIX as nothing or by setting LIB_MEM_CFG_STD_C_LIB_EN to
 *               DEF_ENABLED. In those cases, the unaligned access issue would still be present.
 *********************************************************************************************************
 */

#define  LIB_MEM_CFG_STD_C_LIB_EN                           DEF_ENABLED

#define  LIB_MEM_CFG_MEM_COPY_OPTIMIZE_ASM_EN               DEF_DISABLED

/*
 *********************************************************************************************************
 *                                   MEMORY ALLOCATION CONFIGURATION
 *
 * Note(s) : (1) Configure LIB_MEM_CFG_DBG_INFO_EN to enable/disable memory allocation usage tracking
 *               that associates a name with each segment or dynamic pool allocated.
 *
 *           (2) Configure LIB_MEM_CFG_HEAP_SIZE with the desired size of heap memory (in octets).
 *
 *           (3) Configure LIB_MEM_CFG_HEAP_BASE_ADDR to specify a base address for heap memory :
 *
 *               (a) Heap initialized to specified application memory, if LIB_MEM_CFG_HEAP_BASE_ADDR
 *                   #define'd in 'common_cfg.h'; CANNOT #define to address 0x0.
 *
 *               (b) Heap declared to Mem_Heap[] in 'lib_mem.c', if LIB_MEM_CFG_HEAP_BASE_ADDR NOT
 *                   #define'd in 'common_cfg.h'.
 *********************************************************************************************************
 */

#define  LIB_MEM_CFG_DBG_INFO_EN                            DEF_DISABLED

#define  LIB_MEM_CFG_HEAP_SIZE                              0u

#define  LIB_MEM_CFG_HEAP_PADDING_ALIGN                     LIB_MEM_PADDING_ALIGN_NONE

#if 0                                                           /* Remove this to have heap alloc at specified addr.    */
#define  LIB_MEM_CFG_HEAP_BASE_ADDR       0x00000000            /* Configure heap memory base address (see Note #2b).   */
#endif

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                     STRING LIBRARY CONFIGURATION
 *********************************************************************************************************
 *********************************************************************************************************
 */

/*
 *********************************************************************************************************
 *                                 STRING FLOATING POINT CONFIGURATION
 *
 * Note(s) : (1) Configure LIB_STR_CFG_FP_EN to enable/disable floating point string function(s).
 *
 *           (2) Configure LIB_STR_CFG_FP_MAX_NBR_DIG_SIG to set the maximum number of significant digits
 *               to calculate and/or display for floating point string function(s). See also 'lib_str.h
 *               STRING FLOATING POINT DEFINES Note #1'.
 *********************************************************************************************************
 */

#define  LIB_STR_CFG_FP_EN                                  DEF_DISABLED

#define  LIB_STR_CFG_FP_MAX_NBR_DIG_SIG                     LIB_STR_FP_MAX_NBR_DIG_SIG_DFLT

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                        TERMINAL CONFIGURATION
 *********************************************************************************************************
 *********************************************************************************************************
 */

#define  TERMINAL_CFG_HISTORY_EN                            DEF_ENABLED

/*
 *********************************************************************************************************
 *********************************************************************************************************
 *                                             MODULE END
 *********************************************************************************************************
 *********************************************************************************************************
 */

#endif /* End of common_cfg.h module include.                  */
