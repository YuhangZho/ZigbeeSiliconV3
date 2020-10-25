/***************************************************************************//**
 * @file
 * @brief Heartbeat task configuration
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef HEARTBEAT_TASK_CONFIG_H
#define HEARTBEAT_TASK_CONFIG_H
// <<< Use Configuration Wizard in Context Menu >>>

// <h> HEARTBEAT task configuration

// <o HEARTBEAT_TASK_STACK_SIZE> Task stack size  <64 - 4096>
// <i> Default: 64
#define HEARTBEAT_TASK_STACK_SIZE      64

// <o HEARTBEAT_TASK_PRIO> Task priority
// <i> Default: 15
#define HEARTBEAT_TASK_PRIO      20

#endif
