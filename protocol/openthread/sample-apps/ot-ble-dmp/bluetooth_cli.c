#include <openthread/cli.h>
#include "common/code_utils.hpp"
#include "sl_bt_api.h"
#include "bluetooth_cli.h"

static int Hex2Bin(const char *aHex, uint8_t *aBin, uint16_t aBinLength, bool aAllowTruncate);

otCliCommand userCommands[] = {{"ble", &bleCommand}};

otCliCommand bleCommands[] = {
    {"help", &helpCommand},			      
    {"get_address", &getAddressCommand},
    {"create_adv_set", &createAdvSetCommand},
    {"set_adv_timing", &setAdvTimingCommand},
    {"set_adv_random_address", &setAdvRandomAddressCommand},
    {"start_adv", &startAdvertisingCommand},
    {"stop_adv", &stopAdvertisingCommand},
    {"start_discovery", &startDiscoveryCommand},
    {"set_conn_timing", &setConnTimingCommand},
    {"conn_open", &connOpenCommand},
    {"conn_close", &connCloseCommand},
    {"gatt_discover_primary_services", &gattDiscoverPrimaryServicesCommand},
    {"gatt_discover_characteristics", &gattDiscoverCharacteristicsCommand},
    {"gatt_write_characteristic", &gattWriteCharacteristicCommand},
    {"set_tx_power", &setTxPowerCommand},
};

static otError ParseUnsignedLong(char *aString, unsigned long *aLong)
{
    char *endptr;
    *aLong = strtoul(aString, &endptr, 0);
    return (*endptr == '\0') ? OT_ERROR_NONE : OT_ERROR_PARSE;
}

void bleCommand(uint8_t argc, char *argv[]) {
    bool found = false;
    if (argc > 0) {
        for (size_t i = 0; i < OT_ARRAY_LENGTH(bleCommands); i++) {
            if (strcmp(argv[0], bleCommands[i].mName) == 0) {    
                (bleCommands[i].mCommand)(argc, argv);
		found = true;
		break;
	    }
        }
    }
    if (!found) {
	helpCommand(0, NULL);      
    }
}

void helpCommand(uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
    for (size_t i = 0; i < OT_ARRAY_LENGTH(bleCommands); i++) {
      otCliOutputFormat("%s\r\n", bleCommands[i].mName);
    }
}

void getAddressCommand(uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
    sl_status_t status;
    bd_addr address;
    uint8_t type;
    status = sl_bt_system_get_identity_address(&address, &type);

    if (status == SL_STATUS_OK) {
        otCliOutputFormat("BLE address: ");
        printBleAddress(address);
    } else {
        otCliOutputFormat("error: 0x%2x", status);
    }
    otCliOutputFormat("\r\n");
}

void createAdvSetCommand(uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);  
    sl_status_t status;
    uint8_t handle;

    status = sl_bt_advertiser_create_set(&handle);

    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success handle=%d\r\n", handle);
    } else {
        otCliOutputFormat("error: 0x%2x\r\n", status);
    }
}

void setAdvTimingCommand(uint8_t argc, char *argv[]) {
    otError error = OT_ERROR_NONE;
    sl_status_t status = SL_STATUS_OK;
    unsigned long handle, interval_min, interval_max, duration, max_events;

    VerifyOrExit(argc == 6, error = OT_ERROR_INVALID_ARGS);
    SuccessOrExit(error = ParseUnsignedLong(argv[1], &handle));
    SuccessOrExit(error = ParseUnsignedLong(argv[1], &interval_min));
    SuccessOrExit(error = ParseUnsignedLong(argv[1], &interval_max));
    SuccessOrExit(error = ParseUnsignedLong(argv[1], &duration));
    SuccessOrExit(error = ParseUnsignedLong(argv[1], &max_events));
    
    status = sl_bt_advertiser_set_timing(handle, interval_min, interval_max, duration, max_events);
    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success\r\n");
    } else {
        otCliOutputFormat("error: 0x%2x\r\n", status);
    }

exit:
    if (error != OT_ERROR_NONE) {
        otCliAppendResult(error);
    }
}

void setAdvRandomAddressCommand(uint8_t argc, char *argv[]) {
    otError error = OT_ERROR_NONE;
    sl_status_t status = SL_STATUS_OK;
    unsigned long handle;
    bd_addr unusedAddress;

    VerifyOrExit(argc == 2, error = OT_ERROR_INVALID_ARGS);
    SuccessOrExit(error = ParseUnsignedLong(argv[1], &handle));
    // TO DO: Check the third parameter.
    status = sl_bt_advertiser_set_random_address(handle, 2, unusedAddress, &unusedAddress);
    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success\r\n");
	    otCliOutputFormat("Random address: ");
	    printBleAddress(unusedAddress);
	    otCliOutputFormat("\r\n");
    } else {
        otCliOutputFormat("error: 0x%2x\r\n", status);
    }

exit:
    if (error != OT_ERROR_NONE) {
        otCliAppendResult(error);
    }
}

void startAdvertisingCommand(uint8_t argc, char *argv[]) {
    otError error = OT_ERROR_NONE;
    sl_status_t status = SL_STATUS_OK;
    unsigned long handle, discoverableMode, connectableMode;

    VerifyOrExit(argc == 4, error = OT_ERROR_INVALID_ARGS);
    SuccessOrExit(error = ParseUnsignedLong(argv[1], &handle));
    SuccessOrExit(error = ParseUnsignedLong(argv[2], &discoverableMode));
    SuccessOrExit(error = ParseUnsignedLong(argv[3], &connectableMode));

    status = sl_bt_advertiser_start(handle, discoverableMode, connectableMode);

    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success\r\n");
    } else {
        otCliOutputFormat("error: 0x%2x\r\n", status);
    }
      
exit:
    if (error != OT_ERROR_NONE) {
        otCliAppendResult(error);
    }
}

void stopAdvertisingCommand(uint8_t argc, char *argv[]) {
    otError error = OT_ERROR_NONE;
    sl_status_t status = SL_STATUS_OK;
    unsigned long handle;

    VerifyOrExit(argc == 2, error = OT_ERROR_INVALID_ARGS);
    SuccessOrExit(error = ParseUnsignedLong(argv[1], &handle));

    status = sl_bt_advertiser_stop(handle);

    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success\r\n");
    } else {
        otCliOutputFormat("error: 0x%2x\r\n", status);
    }
      
exit:
    if (error != OT_ERROR_NONE) {
        otCliAppendResult(error);
    }
}

void startDiscoveryCommand(uint8_t argc, char *argv[]) {
    otError error = OT_ERROR_NONE;
    sl_status_t status = SL_STATUS_OK; 
    unsigned long mode;

    VerifyOrExit(argc == 2, error = OT_ERROR_INVALID_ARGS);
    SuccessOrExit(error = ParseUnsignedLong(argv[1], &mode));

    status = sl_bt_scanner_start(1, mode); // 1=1M phy

    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success\r\n");
    } else {
        otCliOutputFormat("error: 0x%2x\r\n", status);
    }
      
exit:
    if (error != OT_ERROR_NONE) {
        otCliAppendResult(error);
    }
}

void setConnTimingCommand(uint8_t argc, char *argv[]) {
    otError error = OT_ERROR_NONE;
    sl_status_t status = SL_STATUS_OK; 
    unsigned long min_interval, max_interval, latency, timeout;

    VerifyOrExit(argc == 5, error = OT_ERROR_INVALID_ARGS);
    SuccessOrExit(error = ParseUnsignedLong(argv[1], &min_interval));
    SuccessOrExit(error = ParseUnsignedLong(argv[2], &max_interval));
    SuccessOrExit(error = ParseUnsignedLong(argv[3], &latency));
    SuccessOrExit(error = ParseUnsignedLong(argv[4], &timeout));
    
    status = sl_bt_connection_set_default_parameters(min_interval, max_interval, latency, timeout, 0, 0xffff);
    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success\r\n");
    } else {
        otCliOutputFormat("error: 0x%x\r\n", status);
    }

exit:
    if (error != OT_ERROR_NONE) {
        otCliAppendResult(error);
    }
}

void connOpenCommand(uint8_t argc, char *argv[]) {
    otError error = OT_ERROR_NONE;
    sl_status_t status = SL_STATUS_OK; 
    bd_addr address;
    uint8_t len = sizeof(address.addr);
    unsigned long addressType;
    uint8_t handle;

    VerifyOrExit(argc == 3, error = OT_ERROR_INVALID_ARGS);
    VerifyOrExit(Hex2Bin(argv[1], address.addr, len, false) >= 0, error = OT_ERROR_PARSE);
    SuccessOrExit(error = ParseUnsignedLong(argv[2], &addressType));    

    // Hex2Bin makes it little endian but it needs to be big endian
    for (size_t i = 0; i < len / 2; i++) {
      uint8_t temp = address.addr[len - i - 1];
      address.addr[len - i - 1] = address.addr[i];
      address.addr[i] = temp;
    }
    // TO DO: Check how is the handle returned?
    status = sl_bt_connection_open(address, addressType, 1, &handle); // 1=1M phy

    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success\r\n");
    } else {
        otCliOutputFormat("error: 0x%2x\r\n", status);
    }

exit:
    if (error != OT_ERROR_NONE) {
        otCliAppendResult(error);
    }
}

void connCloseCommand(uint8_t argc, char *argv[]) {
    otError error = OT_ERROR_NONE;
    sl_status_t status = SL_STATUS_OK; 
    unsigned long handle;

    VerifyOrExit(argc == 2, error = OT_ERROR_INVALID_ARGS);
    SuccessOrExit(error = ParseUnsignedLong(argv[1], &handle));
    
    status = sl_bt_connection_close(handle);

    if (status == SL_STATUS_OK) {
        otCliOutputFormat("success\r\n");
    } else {
        otCliOutputFormat("error: 0x%2x\r\n", status);
    }

exit:
    if (error != OT_ERROR_NONE) {
        otCliAppendResult(error);
    }
}

void gattDiscoverPrimaryServicesCommand(uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
}

void gattDiscoverCharacteristicsCommand(uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
}

void gattWriteCharacteristicCommand(uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
}

void setTxPowerCommand(uint8_t argc, char *argv[]) {
    OT_UNUSED_VARIABLE(argc);
    OT_UNUSED_VARIABLE(argv);
}

static int Hex2Bin(const char *aHex, uint8_t *aBin, uint16_t aBinLength, bool aAllowTruncate)
{
    size_t      hexLength = strlen(aHex);
    const char *hexEnd    = aHex + hexLength;
    uint8_t *   cur       = aBin;
    uint8_t     numChars  = hexLength & 1;
    uint8_t     byte      = 0;
    int         len       = 0;
    int         rval;

    if (!aAllowTruncate)
    {
        VerifyOrExit((hexLength + 1) / 2 <= aBinLength, rval = -1);
    }

    while (aHex < hexEnd)
    {
        if ('A' <= *aHex && *aHex <= 'F')
        {
            byte |= 10 + (*aHex - 'A');
        }
        else if ('a' <= *aHex && *aHex <= 'f')
        {
            byte |= 10 + (*aHex - 'a');
        }
        else if ('0' <= *aHex && *aHex <= '9')
        {
            byte |= *aHex - '0';
        }
        else
        {
            ExitNow(rval = -1);
        }

        aHex++;
        numChars++;

        if (numChars >= 2)
        {
            numChars = 0;
            *cur++   = byte;
            byte     = 0;
            len++;

            if (len == aBinLength)
            {
                ExitNow(rval = len);
            }
        }
        else
        {
            byte <<= 4;
        }
    }

    rval = len;

exit:
    return rval;
}

void printBleAddress(bd_addr address)
{
    for (size_t i = 0; i < 6; i++) {
        otCliOutputFormat("%02x", address.addr[5-i]);
    }
}

void bluetoothCliInit(void) {
    otCliSetUserCommands(userCommands, 1);
}
