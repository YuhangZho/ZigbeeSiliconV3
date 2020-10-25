/***************************************************************************//**
 * @file
 * @brief Propriatery application interface.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef APP_PROPRIETARY_H
#define APP_PROPRIETARY_H

#include "os.h"

// Dummy OS event flag
#define APP_PROPRIETARY_EVENT_FLAG        ((OS_FLAGS)0x01)

// OS event to communicate with the proprietary task
extern OS_FLAG_GRP app_proprietary_event_flags;

/**************************************************************************//**
 * Proprietary application init.
 *****************************************************************************/
void app_proprietary_init(void);

#endif // APP_PROPRIETARY_H
