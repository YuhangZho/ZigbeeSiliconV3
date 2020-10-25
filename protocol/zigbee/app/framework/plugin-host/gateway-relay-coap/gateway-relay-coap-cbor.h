/***************************************************************************//**
 * @file
 * @brief CBOR definitions for the Gateway Relay COAP plugin.
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

// CBOR API
EmberStatus emAfPluginGatewayRelayCoapCborReadInit(uint8_t *buffer,
                                                   uint16_t bufferLength);
EmberStatus emAfPluginGatewayRelayCoapCborGetUnsignedInteger(uint32_t *value,
                                                             uint8_t *size);
EmberStatus emAfPluginGatewayRelayCoapCborGetSignedInteger(int32_t *value,
                                                           uint8_t *size);
EmberStatus emAfPluginGatewayRelayCoapCborWriteInit(uint8_t *buffer,
                                                    uint16_t bufferLength);
EmberStatus emAfPluginGatewayRelayCoapCborGetByteStringDefiniteLength(uint8_t *buffer,
                                                                      uint16_t bufferLength);
EmberStatus emAfPluginGatewayRelayCoapCborGetBooleanValue(bool *value);

uint16_t emAfPluginGatewayRelayCoapCborCurrentWriteDataLength(void);
EmberStatus emAfPluginGatewayRelayCoapCborWriteInt8(int8_t signedData8);
EmberStatus emAfPluginGatewayRelayCoapCborWriteInt16(int16_t signedData16);
EmberStatus emAfPluginGatewayRelayCoapCborWriteInt32(int32_t signedData32);
EmberStatus emAfPluginGatewayRelayCoapCborWriteInt64(int64_t signedData64);
EmberStatus emAfPluginGatewayRelayCoapCborWriteUint8(uint8_t data8);
EmberStatus emAfPluginGatewayRelayCoapCborWriteUint16(uint16_t data16);
EmberStatus emAfPluginGatewayRelayCoapCborWriteUint32(uint32_t data32);
EmberStatus emAfPluginGatewayRelayCoapCborWriteUint64(uint64_t data64);
EmberStatus emAfPluginGatewayRelayCoapCborWriteMapTag(void);
EmberStatus emAfPluginGatewayRelayCoapCborWriteArrayTag(void);
EmberStatus emAfPluginGatewayRelayCoapCborWriteBreak(void);
EmberStatus emAfPluginGatewayRelayCoapCborWriteTrue(void);
EmberStatus emAfPluginGatewayRelayCoapCborWriteFalse(void);
EmberStatus emAfPluginGatewayRelayCoapCborWriteString(char *string);
EmberStatus emAfPluginGatewayRelayCoapCborWriteStringOfLength(const uint8_t *string,
                                                              uint16_t length,
                                                              bool isText);
EmberStatus emAfPluginGatewayRelayCoapCborWriteFloatHalf(uint16_t data16);
EmberStatus emAfPluginGatewayRelayCoapCborWriteFloatSingle(uint32_t data32);
EmberStatus emAfPluginGatewayRelayCoapCborWriteFloatDouble(uint64_t data64);
