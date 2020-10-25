/***************************************************************************//**
 * @file
 * @brief Over-the-Air Device Firmware Update
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

#ifndef SL_OTA_DFU_H
#define SL_OTA_DFU_H

/**************************************************************************//**
 * Bluetooth stack event handler.
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_ota_dfu_on_event(sl_bt_msg_t *evt);

#endif // SL_OTA_DFU_H
