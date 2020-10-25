/***************************************************************************//**
 * @file
 * @brief Parse header file
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

#ifndef PARSE_H_
#define PARSE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

/***********************************************************************************************//**
 * \defgroup par Parse Code
 * \brief Parse application arguments implementation
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup Parse
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup par
 * @{
 **************************************************************************************************/

/***************************************************************************************************
 * Public Macros and Definitions
 **************************************************************************************************/
#define STR_UART_PORT_SIZE (50)

/***************************************************************************************************
 * Public Function Declarations
 **************************************************************************************************/

/***********************************************************************************************//**
 *  \brief  Parse application arguments.
 *  \param[in]  argc  number of arguments
 *  \param[in]  argv  arguments array
 **************************************************************************************************/
void PAR_parse(int argc, char **argv);
void help(void);

/** @} (end addtogroup par) */
/** @} (end addtogroup Parse) */

#ifdef __cplusplus
};
#endif

#endif /* PARSE_H_ */
