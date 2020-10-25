/***************************************************************************//**
 * @file
 * @brief Draws the graphics on the display
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
#ifndef __GRAPHICS_H
#define __GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>
#include "glib.h"

#ifdef __cplusplus
extern "C" {
#endif

extern GLIB_Context_t glibContext;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/
void GRAPHICS_Init(void);
void GRAPHICS_ShowStatus(void);
void GRAPHICS_CreateString(char *string, int32_t value);

#ifdef __cplusplus
}
#endif

#endif /* __GRAHPHICS_H */
