/***************************************************************************//**
 * @file
 * @brief Application interface.
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

extern void init_prop_app(void);
extern void proprietary_queue_post(prop_msg_t msg, RTOS_ERR *err);

#endif // APP_PROPRIETARY_H
