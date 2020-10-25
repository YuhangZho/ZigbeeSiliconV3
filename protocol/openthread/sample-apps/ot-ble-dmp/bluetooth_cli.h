#include "sl_bt_api.h"

void bluetoothCliInit(void);

void bleCommand(uint8_t argc, char *argv[]);
void helpCommand(uint8_t argc, char *argv[]);
void getAddressCommand(uint8_t argc, char *argv[]);
void createAdvSetCommand(uint8_t argc, char *argv[]);
void setAdvTimingCommand(uint8_t argc, char *argv[]);
void setAdvRandomAddressCommand(uint8_t argc, char *argv[]);
void startAdvertisingCommand(uint8_t argc, char *argv[]);
void stopAdvertisingCommand(uint8_t argc, char *argv[]);
void startDiscoveryCommand(uint8_t argc, char *argv[]);
void setConnTimingCommand(uint8_t argc, char *argv[]);
void connOpenCommand(uint8_t argc, char *argv[]);
void connCloseCommand(uint8_t argc, char *argv[]);
void gattDiscoverPrimaryServicesCommand(uint8_t argc, char *argv[]);
void gattDiscoverCharacteristicsCommand(uint8_t argc, char *argv[]);
void gattWriteCharacteristicCommand(uint8_t argc, char *argv[]);
void setTxPowerCommand(uint8_t argc, char *argv[]);

void printBleAddress(bd_addr address);
