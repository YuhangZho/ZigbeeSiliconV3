'''
Created on Oct 9, 2013

@author: sesuskic
'''

from si4010_modem_calc import Si4010ApiCalcInputs
from common import CategoryInterface

class InputBase(CategoryInterface):
    def __init__(self, input_data):
        raise RuntimeError("Not allowed to create an instance of this class")
    
    def dump(self):
        raise RuntimeError("Must implement in derived class")
    
    def set(self, input_data):
        self._set(input_data)
        self.validate()
        
    def _set(self, input_data):
        raise RuntimeError("Must implement in derived class")

    def check_range(self, para, name, range_list, range, mod):
        min, max = range[0], range[1]
        
        '''
        Possible input type:
             Type               Comment
          1. String
          2. Mix type           Value in list
          3. Integer/float      In a range
          4. Integer            In a range and  the_value%X = 0
        '''
        # Check range 
        in_range = False
        if para in range_list:
            in_range = True
        if (max != None) and (min != None):
            if (para < max) and (para > min):
                in_range = True
       
        if not in_range:
            raise RuntimeError("Input value:<{}> for variable:<{}> is out of range!".format(para, name))
        else:
            if ((mod != None) and (para%mod != 0)):
                raise RuntimeError("Input value:<{}> for variable:<{}> is not times of <{}>!".format(para, name, mod))

class Timer(InputBase):
    def __init__(self, input_data):
        self._set(input_data)

    def _set(self, input_data):
        # -------------  Sleep Timer and Sample Rate  -------------
        # Periodic Transmition
        self.SampleInterval                = input_data[0] # Second
        # -1 Tx threshold crossing
        # 0  Tx every sample
        # others are actual Tx rate (seconds)
        self.AutoTxRate                    = input_data[1] # Second

    def validate(self):
        self.check_range(self.SampleInterval,   'SampleInterval',      [0.439, 3600],       [0.439, 3600], None)
        self.check_range(self.AutoTxRate,       'AutoTxRate',          [],                 [-2, 3601], 1)

    def dump(self):
        print("-------------  Timer Settings -------------  ")
        print("SleepTimer Sample Rate:  {}  Seconds".format(self.SampleInterval))
        print("ADC/Temp Samples to skip TX every {}".format(self.AutoTxRate))
        print("Auto TX Rate                      {}".format(self.AutoTxRate))

class TopLevelSetup(InputBase):
    def __init__(self, input_data):
        self._set(input_data)

    def _set(self, input_data):
            # --------  Top Levle Setup   --------
            # (Enable Periodic Transmissions) bConfigSetting.B[1]
            self.EnPeriodicTransmit            = input_data[0]
            # (Use Alternate PA settings for Periodic Transmissions) bConfigSetting.B[0]
            self.AltPAForPerodicTransmit       = input_data[1]
            self.EnMultiCfgOTP                 = input_data[2]
            # Only one can be True. Cannot Enable Stuck Button at the same time
            self.EnStuckButton                 = input_data[3]
            self.EnThermostat                  = input_data[4]
            self.GP9DrivenLOWforADCMeasure     = input_data[5]
            self.EnTestMode                    = input_data[6]
            # -------------  User data  -------------
            # Follow upper nibble definition in the table of bUserData
            self.DataInTwoBytesPayload         = input_data[7]   # Two Data Bytes in Payload
            # Follow lower nibble definition in the table of bUserData
            # 0~7.  8~0xF is reserved
            self.OTPcfgNum                     = input_data[8]   # OTP Configuration number
            self.UserPayloadData0              = input_data[9]  # User input from UI
            self.UserPayloadData1              = input_data[10]  # User input from UI
            self.AdcOrTempThresholdH           = input_data[11]   # User input from UI
            self.AdcOrTempThresholdL           = input_data[12]    # User input from UI
            # -------------  ADC Enable to Conversion Hold Off Duration  -------------
            # follow the table of bAdcHoldOff
            self.AdcHoldOff                    = input_data[13]    # us  255~1275 mutiple of 85
            self.SmokeAlarmHoldOff             = input_data[14]   # us  255~1275 mutiple of 85

    def validate(self):
            self.check_range(self.EnPeriodicTransmit, 'EnPeriodicTransmit', [1, 0],            [None, None], None)
            self.check_range(self.AltPAForPerodicTransmit, 'AltPAForPerodicTransmit',  [1, 0],  [None, None], None)
            self.check_range(self.EnMultiCfgOTP,    'EnMultiCfgOTP',       [1, 0],             [None, None], None)
            self.check_range(self.EnStuckButton,    'EnStuckButton',       [1, 0],             [None, None], None)
            self.check_range(self.EnThermostat,     'EnThermostat',        [1, 0],             [None, None], None)
            self.check_range(self.GP9DrivenLOWforADCMeasure, 'GP9DrivenLOWforADCMeasure',  [1, 0], [None, None], None)
            self.check_range(self.EnTestMode,       'EnTestMode',          [1, 0],             [None, None], None)
    
            self.check_range(self.DataInTwoBytesPayload, 'DataInTwoBytesPayload', [],               [-1, 16], 1)
            self.check_range(self.OTPcfgNum,        'OTPcfgNum',       [],    [-1, 9],  1)
            self.check_range(self.UserPayloadData0, 'UserPayloadData0', [],  [-1, 256], 1)
            self.check_range(self.UserPayloadData1, 'UserPayloadData1', [],  [-1, 256], 1)
    
            self.check_range(self.AdcOrTempThresholdH, 'AdcOrTempThresholdH', [], [-32767, 32767], 1)
            self.check_range(self.AdcOrTempThresholdL, 'AdcOrTempThresholdL', [], [-32767, 32767], 1)
            
            self.check_range(self.SmokeAlarmHoldOff, 'SmokeAlarmHoldOff',  [], [254, 1276], 85)
            self.check_range(self.AdcHoldOff,        'AdcHoldOff',         [], [254, 1276], 85)

    def dump(self):
        print("PeriodicTransmit:                 {}".format(self.EnPeriodicTransmit))
        print("AltPAForPerodicTransmit:          {}".format(self.AltPAForPerodicTransmit))
        print("EnMultiCfgOTP:                    {}".format(self.EnMultiCfgOTP))
        print("EnStuckButton:                    {}".format(self.EnStuckButton))
        print("EnThermostat:                     {}".format(self.EnThermostat))
        print("GP9DrivenLOWforADCMeasure:        {}".format(self.GP9DrivenLOWforADCMeasure))
        print("EnTestMode:                       {}".format(self.EnTestMode))

        # -------------  User data  -------------
        print("DataInTwoBytesPayload:          0x{}".format(format(self.DataInTwoBytesPayload, 'X')))
        print("OTPcfgNum:                        {}".format(self.OTPcfgNum))
        print("UserPayloadData0:              0x{}".format(format(self.UserPayloadData0, 'X')))
        print("UserPayloadData1:              0x{}".format(format(self.UserPayloadData1, 'X')))
        print("AdcOrTempThresholdH:            0x{}".format(format(self.AdcOrTempThresholdH, 'X')))
        print("AdcOrTempThresholdL:            0x{}".format(format(self.AdcOrTempThresholdL, 'X')))
        # -------------  ADC Enable to Conversion Hold Off Duration  -------------
        print("AdcHoldOff:                     0x{}".format(format(self.AdcHoldOff, 'X')))
        print("SmokeAlarmHoldOff:              0x{}".format(format(self.SmokeAlarmHoldOff, 'X')))

class Buttons(InputBase):
    def __init__(self, input_data):
        self._set(input_data)

    def _set(self, input_data):
        # The length must be 23. It's the actual value of different key setting, 0 = Not used
        self.ValidButtons                  = input_data[0] # bValidButtons[23]
        # The sum of below three values should be less than 2000ms
        self.PushQualThreshTime            = input_data[1]     # ms Button Qualification Time(multiple of 5ms)
        self.ExpectedChatterTimeofButtons  = input_data[2]     # ms
        self.MultiButtonPressTimeDiff      = input_data[3]     # ms
        self.WaitTimeBeforeShutDownMargin  = input_data[4]     # ms

        if (min(self.ValidButtons)) < 0 or  (max(self.ValidButtons) > 0xFF):
            raise RuntimeError("Valid Button value is out of range!")
        if (len(self.ValidButtons) != 23):
            raise RuntimeError("Length of ValidButtons list must be 23!")

    def validate(self):
        self.check_range(self.PushQualThreshTime, 'PushQualThreshTime', [], [9, 1276], 5)
        self.check_range(self.ExpectedChatterTimeofButtons, 'ExpectedChatterTimeofButtons', [], [-1, 1001], 1)
        self.check_range(self.MultiButtonPressTimeDiff,     'MultiButtonPressTimeDiff', [], [-1, 1000], 1)
        self.check_range(self.WaitTimeBeforeShutDownMargin, 'WaitTimeBeforeShutDownMargin', [], [-1, 1000], 1)
        
    def dump(self):
        # -------------  Buttons  -------------
        print("ValidButtons:                     {}".format(self.ValidButtons))
        print("PushQualThreshTime:               {}".format(self.PushQualThreshTime))
        print("ExpectedChatterTimeofButtons:     {}".format(self.ExpectedChatterTimeofButtons))
        print("MultiButtonPressTimeDiff:         {}".format(self.MultiButtonPressTimeDiff))
        print("WaitTimeBeforeShutDownMargin:     {}".format(self.WaitTimeBeforeShutDownMargin))

class Packet(InputBase):
    def __init__(self, input_data):
        self.set(input_data)

    def _set(self, input_data):
        self.DataFormat = input_data["DataFormat"]
        self.PremLenForFixedOrSecu = input_data["PremLenForFixedOrSecu"]
        self.Preamble = input_data["Preamble"]
        self.SyncWord = input_data["SyncWord"]
        self.PacketLength = input_data["PacketLength"]
        self.modem = Si4010ApiCalcInputs(input_data["modem"])
        
    def validate(self):
        self.check_range(self.DataFormat,            'DataFormat',            ['Simple', 'Security', 'Fixed Packet', 'Holtek Emulation'], [None, None], None)
        self.check_range(self.PremLenForFixedOrSecu, 'PremLenForFixedOrSecu', [],     [-1, 84], 1) # TODO further check in calculator
        self.check_range(self.SyncWord,              'SyncWord',              [0, 0xFFFF], [0, 0xFFFF], 1)
        self.check_range(self.Preamble,              'Preamble',              [0, 0xFF],   [0, 0xFF],   1)

    def dump(self):
        print("DataFormat:                       {}".format(self.DataFormat))
        print("PremLenForFixedOrSecu             {}".format(self.PremLenForFixedOrSecu))
        print("Preamble                          {}".format(self.Preamble))
        print("SyncWord                          {}".format(self.SyncWord))
        print("PacketLength                      {}".format(self.PacketLength))
        print("modem                             {}".format(self.modem.dump()))

class PacketSetup(InputBase):
    def __init__(self, input_data):
        self._set(input_data)

    def _set(self, input_data):
        self.Packet0Enabled  =  input_data[0]
        self.Packet1Enabled  =  input_data[1]
        self.Packet2Enabled  =  input_data[2]
        self.Packet3Enabled  =  input_data[3]
        self.LEDonTime       =  input_data[4]
        self.AddiDelayBetweenPack      = input_data[5]     # multiple of 5ms
        self.RepeatInterval            =  input_data[6]  # ms
        self.MaxButtonDuration         = input_data[7]    # second
        self.MinNumOfTXsPerPress       = input_data[8]     # # of TX's
        self.MinNumOfTXsaftBatterySwap = input_data[9]     # # of TX's
        self.PacketBD = input_data[10]
        self.PacketTX = input_data[11]

    def validate(self):
        self.check_range(self.RepeatInterval,      'RepeatInterval',  [],  [-1, 0xFFFF + 1], 1)
        self.check_range(self.MinNumOfTXsPerPress, 'MinNumOfTXsPerPress', [], [0, self.MaxButtonDuration], 1)
        self.check_range(self.MinNumOfTXsaftBatterySwap, 'MinNumOfTXsaftBatterySwap', [], [0, 255], 1)
        #self.check_range(self.MaxButtonDuration,   'MaxButtonDuration',  [], [max(self.PacketBD), 255], 1)  # TODO enable this
        self.check_range(self.Packet0Enabled,      'Packet0Enabled',     [0, 1], [None, None],  None)
        self.check_range(self.Packet1Enabled,      'Packet1Enabled',     [0, 1], [None, None],  None)
        self.check_range(self.Packet2Enabled,      'Packet2Enabled',     [0, 1], [None, None],  None)
        self.check_range(self.Packet3Enabled,      'Packet3Enabled',     [0, 1], [None, None],  None)
        self.check_range(self.LEDonTime,           'LEDonTime',          [], [-1, 256], 5)
        self.check_range(self.AddiDelayBetweenPack, 'AddiDelayBetweenPack', [], [-1, 256], 5)
        if self.PacketBD[0] == 0:
            raise RuntimeError("PacketBD[0] cannot be 0!")
        if len(self.PacketBD) != 32:
            raise RuntimeError("Length of PacketBD must be 32")
        if self.PacketTX[0] == 0:
            raise RuntimeError("PacketTX[0] cannot be 0!")
        if len(self.PacketTX) != 32:
            raise RuntimeError("Length of PacketTX must be 32")
        
    def dump(self):
        # --------  Packet Setup  --------
        print("Packet0Enabled:                   {}".format(self.Packet0Enabled))
        print("Packet1Enabled:                   {}".format(self.Packet1Enabled))
        print("Packet2Enabled:                   {}".format(self.Packet2Enabled))
        print("Packet3Enabled:                   {}".format(self.Packet3Enabled))
        print("LEDonTime:                        {}".format(self.LEDonTime))
        print("AddiDelayBetweenPack:             {}".format(self.AddiDelayBetweenPack))
        print("RepeatInterval:                   {}".format(self.RepeatInterval))
        print("MaxButtonDuration:                {}".format(self.MaxButtonDuration))
        print("MinNumOfTXsPerPress:              {}".format(self.MinNumOfTXsPerPress))
        print("MinNumOfTXsaftBatterySwap:        {}".format(self.MinNumOfTXsaftBatterySwap))
        print("PacketBD:                         {}".format(self.PacketBD))
        print("PacketTX:                         {}".format(self.PacketTX))

class FixedPacket(InputBase):
    def __init__(self, input_data):        
        self._set(input_data)

    def _set(self, input_data):
        # Restrictions
        #   Buttons             Length
        #   1                   4~96
        #   2                   4~48
        #   4                   4~24
        #   8                   4~12
        self.FixedPacketButtons  = input_data[0]  # Buttons of Fixed Packet
        self.FixedPacketLength   = input_data[1] # Length of  Fixed Packet
        self.PacketOverRide      = input_data[2] # TODO restriction
        
    def validate(self):
        pass

    def dump(self):
        # --------  Fixed Packet  --------
        print("FixedPacketButtons:               {}".format(self.FixedPacketButtons))
        print("FixedPacketLength:                {}".format(self.FixedPacketLength))
        print("PacketOverRide:                   {}".format(self.PacketOverRide))
class Misc(InputBase):
    def __init__(self, input_data):
        self._set(input_data)

    def _set(self, input_data):
        # Drop list
        self.LEDIntensity          = input_data[0]  # 'Off' , '0.37 mA' , '0.60 mA' , '0.97 mA'
        self.PeriodicLEDIntensity  = input_data[1]  # 'Off' , '0.37 mA' , '0.60 mA' , '0.97 mA'
        self.BattThreshToDisLED    = input_data[2]  # 1500~3600
        self.BattThreshToDisTrans  = input_data[3]  # 1500~3600
        self.TempThreshToUsAlterPA = input_data[4]  # -50.0~150.0
        self.AlterPAOutPower       = input_data[5]  # dBm
        self.AlterPAType           = input_data[6]  #'Min PA & Max Radiated Power'  # or 'Max Radiated Power'
        self.RefClockType          = input_data[7]  #'Internal Ref Clock' , 'External Crystal' , 'External Si501'
        self.ExtCrystalFreq        = input_data[8]  #10000000~13000000(10MHz~13MHz)
        self.ExtCrystalLowCap      = input_data[9]  # 0 / 1

    def validate(self):
        self.check_range(self.LEDIntensity,         'LEDIntensity',         ['Off', '0.37 mA', '0.60 mA', '0.97 mA'], [None, None], None)
        self.check_range(self.PeriodicLEDIntensity, 'PeriodicLEDIntensity', ['Off', '0.37 mA', '0.60 mA', '0.97 mA'], [None, None], None)
        self.check_range(self.BattThreshToDisLED,   'BattThreshToDisLED',   [], [1499, 3601], 1)
        self.check_range(self.BattThreshToDisTrans, 'BattThreshToDisTrans', [], [1499, 3601], 1)
        
        self.check_range(self.TempThreshToUsAlterPA,'TempThreshToUsAlterPA',[], [-501, 1501], 1)
        self.check_range(self.AlterPAOutPower,      'AlterPAOutPower',      [], [-1e-8, 13+1e-8], None)
        self.check_range(self.AlterPAType,          'AlterPAType',          ['Min PA & Max Radiated Power' , 'Max Radiated Power'], [None, None], None)
        self.check_range(self.RefClockType,         'RefClockType',         ['Internal Ref Clock' , 'External Crystal' , 'External Si501'], [None, None], None)
        self.check_range(self.ExtCrystalFreq,       'ExtCrystalFreq',       [], [10e6-1, 13e6+1], 1)
        self.check_range(self.ExtCrystalLowCap,     'ExtCrystalLowCap',     [0, 1], [None, None], None)
        
    def dump(self):
        # --------  Misc Setup  --------
        print("LEDIntensity:                     {}".format(self.LEDIntensity))
        print("PeriodicLEDIntensity:             {}".format(self.PeriodicLEDIntensity))
        print("BattThreshToDisLED:               {}".format(self.BattThreshToDisLED))
        print("BattThreshToDisTrans:             {}".format(self.BattThreshToDisTrans))
        print("TempThreshToUsAlterPA:            {}".format(self.TempThreshToUsAlterPA))
        print("AlterPAOutPower:                  {}".format(self.AlterPAOutPower))
        print("AlterPAType:                      {}".format(self.AlterPAType))
        print("RefClockType:                     {}".format(self.RefClockType))
        print("ExtCrystalFreq:                   {}".format(self.ExtCrystalFreq))
        print("ExtCrystalLowCap:                 {}".format(self.ExtCrystalLowCap))

class Security(InputBase):
    def __init__(self, input_data):
        self._set(input_data)

    def _set(self, input_data):
        self.OEM_Key               = input_data[0]         # 16 elements
        self.OEM_ID_Key            = input_data[1]

#         self.check_range(self.OEM_ID_Key,           'OEM_ID_Key',           [], [None, None], None)
#         self.check_range(self.OEM_Key,              'OEM_Key',              [[]], [None, None], None)

    def validate(self):
        pass
    
    def dump(self):
        # Security
        print("OEM_Key:                          {}".format(self.OEM_Key))
        print("IOEM_ID_Key:                      {}".format(self.OEM_ID_Key))

class Holtek(InputBase):
    def __init__(self, input_data):
        self._set(input_data)

    def _set(self, input_data):
        self.HoltekEmuDataBits     = input_data[0]          # Data bits  (0~7)
        self.HoltekEmuMappingType  = input_data[1]   # 'Brazil', 'Direct', 'Custom'
        self.HoltekEmuCustomGpioMappingTable = input_data[2] # Correspoding to bValidButtons[23]

    def validate(self):
        self.check_range(self.HoltekEmuDataBits,    'HoltekEmuDataBits',    [], [-1, 8], 1)
        self.check_range(self.HoltekEmuMappingType, 'HoltekEmuMappingType', ['Brazil', 'Direct', 'Custom'], [None, None], None)

        if (min(self.HoltekEmuCustomGpioMappingTable)) < 0 or  (max(self.HoltekEmuCustomGpioMappingTable) > 0xFF):
            raise RuntimeError("Value in \"Holtek Emulation Custom Gpio Mapping Table\" is out of range!")
        if (len(self.HoltekEmuCustomGpioMappingTable) != 23):
            raise RuntimeError("Length of \"Holtek Emulation Custom Gpio Mapping Table\" list must be 23!")

    def dump(self):
        # Holtek emulation
        print("HoltekEmuDataBits:                {}".format(self.HoltekEmuDataBits))
        print("HoltekEmuMappingType:             {}".format(self.HoltekEmuMappingType))
        print("HoltekEmuCustomGpioMappingTable:  {}".format(self.HoltekEmuCustomGpioMappingTable))
