/***************************************************************************//**
 * @file
 * @brief Read Attributes Response handling for Gateway Relay CoAP plugin
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

// From coap-server/coap-server.h
#if !defined(COAP_RESPONSE_OK)
#define COAP_RESPONSE_OK                  200
#endif
#if !defined(COAP_RESPONSE_INTERNAL_ERROR)
#define COAP_RESPONSE_INTERNAL_ERROR      500
#endif

void emAfPluginGatewayRelayCoapCreateCborDataFromReadAttributeResponse(
  const uint8_t *buffer,
  uint8_t length,
  uint8_t *returnBuffer,
  uint8_t returnLength,
  void (*setUpErrorCodeFunction)(uint16_t coapStatusCode)
  );
