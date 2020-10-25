/***************************************************************************//**
 * @file
 * @brief SPI abstraction used by memory lcd display
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

#include "sl_memlcd_spi.h"
#include "em_cmu.h"

sl_status_t sli_memlcd_spi_init(sli_memlcd_spi_handle_t *handle, int baudrate, USART_ClockMode_TypeDef mode)
{
  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;
  USART_TypeDef *usart = handle->usart;

  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(handle->clock, true);

  GPIO_PinModeSet((GPIO_Port_TypeDef)handle->clk_port, handle->clk_pin, gpioModePushPull, 0);
  GPIO_PinModeSet((GPIO_Port_TypeDef)handle->mosi_port, handle->mosi_pin, gpioModePushPull, 0);

  init.baudrate = baudrate;
  init.clockMode = mode;

  USART_InitSync(usart, &init);

#if defined(_SILICON_LABS_32B_SERIES_0)
  usart->ROUTE = (USART_ROUTE_CLKPEN | USART_ROUTE_TXPEN)
                 | (handle->loc << _USART_ROUTE_LOCATION_SHIFT);
#elif defined(_SILICON_LABS_32B_SERIES_1)
  usart->ROUTELOC0 = (handle->mosi_loc << _USART_ROUTELOC0_TXLOC_SHIFT)
                     | (handle->clk_loc << _USART_ROUTELOC0_CLKLOC_SHIFT);
  usart->ROUTEPEN = USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_CLKPEN;
#elif defined(_SILICON_LABS_32B_SERIES_2)
  int usart_index = USART_NUM(usart);
  GPIO->USARTROUTE[usart_index].TXROUTE = (handle->mosi_port << _GPIO_USART_TXROUTE_PORT_SHIFT)
                                          | (handle->mosi_pin << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[usart_index].CLKROUTE = (handle->clk_port << _GPIO_USART_CLKROUTE_PORT_SHIFT)
                                           | (handle->clk_pin << _GPIO_USART_CLKROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[usart_index].ROUTEEN = GPIO_USART_ROUTEEN_TXPEN | GPIO_USART_ROUTEEN_CLKPEN;
#endif

  return SL_STATUS_OK;
}

sl_status_t sli_memlcd_spi_shutdown(sli_memlcd_spi_handle_t *handle)
{
  USART_Enable(handle->usart, usartDisable);
  CMU_ClockEnable(handle->clock, false);
  return SL_STATUS_OK;
}

sl_status_t sli_memlcd_spi_tx(sli_memlcd_spi_handle_t *handle, const void *data, unsigned len)
{
  const char *buffer = data;
  USART_TypeDef *usart = handle->usart;

  for (unsigned i=0; i<len; i++) {
    USART_Tx(usart, buffer[i]);
  }

  /* Note that at this point all the data is loaded into the fifo, this does
   * not necessarily mean that the transfer is complete. */
  return SL_STATUS_OK;
}

void sli_memlcd_spi_wait(sli_memlcd_spi_handle_t *handle)
{
  USART_TypeDef *usart = handle->usart;

  /* Wait for all transfers to finish */
  while (!(usart->STATUS & USART_STATUS_TXC))
    ;
}
