/***************************************************************************//**
 * @file
 * @brief Circular Buffer API
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

#ifndef CIRCULAR_BUFF_H_
#define CIRCULAR_BUFF_H_

#include <stdbool.h>
#include <stdint.h>

/***************************************************************************//**
 * @addtogroup Circular_Buffer
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @defgroup Circular_Buffer_Config_Settings Circular Buffer configuration
 * @{
 * @brief Circular Buffer configuration setting macro definitions
 ******************************************************************************/

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/
typedef enum {
  cb_err_ok = 0,            /**< No error */
  cb_err_full,              /**< Buffer is full */
  cb_err_empty,             /**< Buffer is empty */
  cb_err_no_mem,            /**< No memory for buffer allocation */
  cb_err_too_much_data,     /**< To much data to be push into the circular buffer */
  cb_err_insuff_data,       /**< Insufficient amount of data to be pop */
}cb_err_code_t;

typedef struct {
  void *buffer;             /**< Data buffer */
  void *buffer_end;         /**< End of data buffer */
  size_t capacity;           /**< Maximum number of items in the buffer */
  size_t count;             /**< Number of items in the buffer */
  size_t item_size;         /**< Size of each item in the buffer */
  void *head;               /**< Pointer to head */
  void *tail;               /**< Pointer to tail */
} circular_buffer_t;

/** @} {end defgroup Circular_Buffer_Config_Settings} */

/***************************************************************************//**
 * @defgroup Circular_Buffer_Functions Circular Buffer Functions
 * @{
 * @brief Circular Buffer support functions
 ******************************************************************************/

cb_err_code_t cb_init(circular_buffer_t *cb, size_t capacity, size_t sz);
cb_err_code_t cb_push_buff(circular_buffer_t *cb, void *inBuff, size_t len);
cb_err_code_t cb_pop_buff(circular_buffer_t *cb, void *outBuff, size_t len);
void cb_free(circular_buffer_t *cb);

/** @} {end defgroup Circular_Buffer_Functions}*/

/** @} {end addtogroup Circular_Buffer} */

#endif /* CIRCULAR_BUFF_H_ */
