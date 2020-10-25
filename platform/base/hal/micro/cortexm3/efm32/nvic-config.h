/***************************************************************************//**
 * @file
 * @brief NVIC Config Header
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
#if !defined (CORTEXM3_EFR32)
  #include "em_device.h"
  #if defined (_SILICON_LABS_32B_SERIES_1_CONFIG_4)
    #define EFR32_SERIES1_CONFIG4_MICRO
  #elif defined (_SILICON_LABS_32B_SERIES_1_CONFIG_3)
    #define EFR32_SERIES1_CONFIG3_MICRO
  #elif defined (_SILICON_LABS_32B_SERIES_1_CONFIG_2)
    #define EFR32_SERIES1_CONFIG2_MICRO
  #elif defined (_SILICON_LABS_32B_SERIES_1_CONFIG_1)
    #define EFR32_SERIES1_CONFIG1_MICRO
  #elif defined (_SILICON_LABS_32B_SERIES_2_CONFIG_1)
    #define EFR32_SERIES2_CONFIG1_MICRO
  #elif defined (_SILICON_LABS_32B_SERIES_2_CONFIG_2)
    #define EFR32_SERIES2_CONFIG2_MICRO
  #endif
#endif

#if defined (EFR32_SERIES1_CONFIG4_MICRO)
  #include "efr32/nvic-config-series1-config4.h"
#elif defined (EFR32_SERIES1_CONFIG3_MICRO)
  #include "efr32/nvic-config-series1-config3.h"
#elif defined (EFR32_SERIES1_CONFIG2_MICRO)
  #include "efr32/nvic-config-series1-config2.h"
#elif defined (EFR32_SERIES2_CONFIG1_MICRO)
  #include "efr32/nvic-config-series2-config1.h"
#elif defined (EFR32_SERIES2_CONFIG2_MICRO)
  #include "efr32/nvic-config-series2-config2.h"
#else
  #include "efr32/nvic-config-series1-config1.h"
#endif
