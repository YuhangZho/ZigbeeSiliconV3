/***************************************************************************//**
 * @file
 * @brief EZSP SPI Host user interface functions
 *
 * This includes command option parsing, trace and counter functions.
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

#include PLATFORM_HEADER
#include <string.h>
#ifndef WIN32
  #include <unistd.h>
  #include <time.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
#endif
#include <stdlib.h>
#include "stack/include/ember-types.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/ezsp-host/ezsp-host-common.h"
#include "app/ezsp-host/spi/spi-host.h"
#include "app/ezsp-host/ezsp-host-priv.h"
#include "app/ezsp-host/ezsp-host-io.h"
#include "app/ezsp-host/ezsp-host-queues.h"
#include "app/ezsp-host/ezsp-host-ui.h"
#include "app/util/gateway/backchannel.h"
#include "app/ezsp-host/spi/spi-host-ui.h"

//------------------------------------------------------------------------------
// Preprocessor definitions

#define ERR_LEN               128   // max length error message

#define txControl (txBuffer[0])     // more descriptive aliases
#define rxControl (rxBuffer[0])

static const char options[] = "h::t:v::";

bool ezspInternalProcessCommandOptions(int argc, char *argv[], char *errStr)
{
  int c;
  char port[SPI_PORT_LEN];
  char devport[SPI_PORT_LEN];
  uint8_t trace;
  uint8_t portnum;
  int optionCount = 0;

  while (true) {
    c = getopt(argc, argv, options);
    if (c == -1) {
      if (optind != argc ) {
        snprintf(errStr, ERR_LEN, "Invalid option %s.\n", argv[optind]);
      }
      break;
    }

    optionCount++;

    switch (c) {
      case 'h':
      case '?':
        snprintf(errStr, ERR_LEN, "\n");
        break;
      case 't':
        if (sscanf(optarg, "%hhu", &trace) != 1) {
          snprintf(errStr, ERR_LEN, "Invalid trace flag value %s.\n", optarg);
        } else {
          spiWriteConfig(traceFlags, trace);
        }
        break;
      case 'v':
        if (!backchannelSupported) {
          fprintf(stderr, "Error: Backchannel support not compiled into this application.\n");
          exit(1);
        }
        backchannelEnable = true;
        if (optarg) {
          int port = atoi(optarg);
          if (port == 0 || port > 65535) {
            snprintf(errStr, ERR_LEN, "Invalid virtual ISA port number '%d'.\n", port);
          }
          backchannelSerialPortOffset = port;
        }
        break;
      default:
        assert(1);
        break;
    }   // end of switch (c)
  } //end while
  return true;
}
