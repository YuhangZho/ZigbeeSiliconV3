'''
Created on Oct 9, 2013

@author: sesuskic
'''

class DefaultInputs(object):
    # -------------  Packet(0/1/2/3)  -------------
    packet_0 = { #self.Packet0 = {
                    # --------  Data Format  --------
                               'DataFormat'             : 'Security', # 'Simple', 'Security', 'Fixed Packet', 'Holtek Emulation'
                               # --------  Preamble/Sync word  --------
                               'PremLenForFixedOrSecu'  :  5,         # Validity Check will be done by calculator
                               'Preamble'               :  0xAA,      # one byte
                               'SyncWord'               :  0x2DD4,    # two bytes
                               # --------  Packet  --------
                               'PacketLength'           :  30,
                               # --------  Parameter for modem calcuator  --------
                               'modem' : [0,      # PaketType
                                          # --------  Antenna Setup  --------
                                          #Alpha    ApproxEfficiency    ReactanceChange(%)     ManualImpedanceEntry    AntennaRealZ    AntennaImagZ
                                          4.2,     7.7,                40,                   False,                    100,            40,
                                          # --------  Power Amplifier Setup  --------
                                          #TotalPAPower(dBm)    PAType'                         CenterFrequency(MHZ)    NominalCap    ExternalDiffCap    Q_FactorOfExtCap
                                          0,                    'Min PA & Max Radiated Power',  315.000,                30,           1,                 300,
                                          # --------  Serializer Setup  --------
                                          #DataRate(kbits/s)    Encoding      Modulation    FSK_Deviation(kHZ)    ManualRampRateEntry    TargetRampRate
                                          48,                   'NRZ Only',   'FSK',        43.2,                 False,                  12
                                          ]
                               }
    packet_1 = { #self.Packet1 = {
                               # --------  Data Format  --------
                               'DataFormat'             : 'Security', # 'Simple', 'Security', 'Fixed Packet', 'Holtek Emulation'
                               # --------  Preamble/Sync word  --------
                               'PremLenForFixedOrSecu'  :  5,         # Validity Check will be done by calculator
                               'Preamble'               :  0xAA,      # one byte
                               'SyncWord'               :  0x2DD4,    # two bytes
                               # --------  Packet  --------
                               'PacketLength'           :  30,
                               # --------  Parameter for modem calcuator  --------
                               'modem' : [0,      # PaketType
                                          # --------  Antenna Setup  --------
                                          #Alpha    ApproxEfficiency    ReactanceChange(%)     ManualImpedanceEntry    AntennaRealZ    AntennaImagZ
                                          4.2,     7.7,                40,                   False,                    100,            40,
                                          # --------  Power Amplifier Setup  --------
                                          #TotalPAPower(dBm)    PAType'                         CenterFrequency(MHZ)    NominalCap    ExternalDiffCap    Q_FactorOfExtCap
                                          0,                    'Min PA & Max Radiated Power',  315.000,                30,           1,                 300,
                                          # --------  Serializer Setup  --------
                                          #DataRate(kbits/s)    Encoding      Modulation    FSK_Deviation(kHZ)    ManualRampRateEntry    TargetRampRate
                                          48,                   'NRZ Only',   'FSK',        43.2,                 False,                  12
                                          ]
                               }
    
    
    packet_2 = { #self.Packet2 = {
                               # --------  Data Format  --------
                               'DataFormat'             : 'Security', # 'Simple', 'Security', 'Fixed Packet', 'Holtek Emulation'
                               # --------  Preamble/Sync word  --------
                               'PremLenForFixedOrSecu'  :  5,         # Validity Check will be done by calculator
                               'Preamble'               :  0xAA,      # one byte
                               'SyncWord'               :  0x2DD4,    # two bytes
                               # --------  Packet  --------
                               'PacketLength'           :  30,
                               # --------  Parameter for modem calcuator  --------
                               'modem' : [0,      # PaketType
                                          # --------  Antenna Setup  --------
                                          #Alpha    ApproxEfficiency    ReactanceChange(%)     ManualImpedanceEntry    AntennaRealZ    AntennaImagZ
                                          4.2,     7.7,                40,                   False,                    100,            40,
                                          # --------  Power Amplifier Setup  --------
                                          #TotalPAPower(dBm)    PAType'                         CenterFrequency(MHZ)    NominalCap    ExternalDiffCap    Q_FactorOfExtCap
                                          0,                    'Min PA & Max Radiated Power',  315.000,                30,           1,                 300,
                                          # --------  Serializer Setup  --------
                                          #DataRate(kbits/s)    Encoding      Modulation    FSK_Deviation(kHZ)    ManualRampRateEntry    TargetRampRate
                                          48,                   'NRZ Only',   'FSK',        43.2,                 False,                  12
                                          ]
                               }
    
    
    packet_3 = { #self.Packet3 = {
                               # --------  Data Format  --------
                               'DataFormat'             : 'Security', # 'Simple', 'Security', 'Fixed Packet', 'Holtek Emulation'
                               # --------  Preamble/Sync word  --------
                               'PremLenForFixedOrSecu'  :  5,         # Validity Check will be done by calculator
                               'Preamble'               :  0xAA,      # one byte
                               'SyncWord'               :  0x2DD4,    # two bytes
                               # --------  Packet  --------
                               'PacketLength'           :  30,
                               # --------  Parameter for modem calcuator  --------
                               'modem' : [0,      # PaketType
                                          # --------  Antenna Setup  --------
                                          #Alpha    ApproxEfficiency    ReactanceChange(%)     ManualImpedanceEntry    AntennaRealZ    AntennaImagZ
                                          4.2,     7.7,                40,                   False,                    100,            40,
                                          # --------  Power Amplifier Setup  --------
                                          #TotalPAPower(dBm)    PAType'                         CenterFrequency(MHZ)    NominalCap    ExternalDiffCap    Q_FactorOfExtCap
                                          0,                    'Min PA & Max Radiated Power',  315.000,                30,           1,                 300,
                                          # --------  Serializer Setup  --------
                                          #DataRate(kbits/s)    Encoding      Modulation    FSK_Deviation(kHZ)    ManualRampRateEntry    TargetRampRate
                                          48,                   'NRZ Only',   'FSK',        43.2,                 False,                  12
                                          ]
                               }
    
    packet_HoltekHP6P20 = { #self.Packet3 = {
                                          # --------  Data Format  --------
                                          'DataFormat'             : 'Holtek Emulation', # 'Simple', 'Security', 'Fixed Packet', 'Holtek Emulation'
                                          # --------  Preamble/Sync word  --------
                                          'PremLenForFixedOrSecu'  :  5,         # Validity Check will be done by calculator
                                          'Preamble'               :  0xAA,      # For holtek emulation. This is useless
                                          'SyncWord'               :  0x2DD4,    # For holtek emulation. This is useless
                                          # --------  Packet  --------
                                          'PacketLength'           :  96,
                                          # --------  Parameter for modem calcuator  --------
                                          'modem' : [0,      # PaketType
                                                     # --------  Antenna Setup  --------
                                                     #Alpha    ApproxEfficiency    ReactanceChange(%)     ManualImpedanceEntry    AntennaRealZ    AntennaImagZ
                                                     0   ,     7.7,                40,                   False,                    100,            40,
                                                     # --------  Power Amplifier Setup  --------
                                                     #TotalPAPower(dBm)    PAType'                         CenterFrequency(MHZ)    NominalCap    ExternalDiffCap    Q_FactorOfExtCap
                                                     0,                    'Min PA & Max Radiated Power',  433.920,                30,           1,                 300,
                                                     # --------  Serializer Setup  --------
                                                     #DataRate(kbits/s)    Encoding      Modulation    FSK_Deviation(kHZ)    ManualRampRateEntry    TargetRampRate
                                                     2.141,                   'NRZ Only',   'OOK',        43.2,                 False,                  12
                                                     ]
                                          }

    # -------------  Global features  -------------
    # --------  Sleep Timer and Sample Interval   --------
    timer = [# SampleInterval
             7,
             # AutoTxRate
             # -1 Tx threshold crossing
             # 0  Tx every sample
             # others actual Tx rate (seconds)
             40]
    # --------  Top Levle Setup   --------
    top_level = [ # (Enable Periodic Transmissions) bConfigSetting.B[1]
                  #self.EnPeriodicTransmit            = True
                  True,
                  # (Use Alternate PA settings for Periodic Transmissions) bConfigSetting.B[0]
                  #self.AltPAForPerodicTransmit       = True
                  True,
                  #self.EnMultiCfgOTP                 = False
                  False,
                  # Only one can be True. Cannot Enable Stuck Button at the same time
                  #self.EnStuckButton                 = False
                  #self.EnThermostat                  = False
                  False, False,
                  #self.GP9DrivenLOWforADCMeasure     = True
                  True,
                  #self.EnTestMode                    = True
                  True,
                  # -------------  User data  -------------
                  # Follow upper nibble definition in the table of bUserData
                  #self.DataInTwoBytesPayload         = 0x5   # Two Data Bytes in Payload
                  0x5,
                  # Follow lower nibble definition in the table of bUserData
                  # 0~7.  8~0xF is reserved
                  #self.OTPcfgNum                     = 0x1   # OTP Configuration number
                  0x1,
                  #self.UserPayloadData0              = 0x55  # User input from UI
                  0x55,
                  #self.UserPayloadData1              = 0xAA  # User input from UI
                  0xAA,
                  #self.AdcOrTempThresholdH           = 250   # User input from UI
                  250,
                  #self.AdcOrTempThresholdL           = 10    # User input from UI
                  10,
                  # -------------  ADC Enable to Conversion Hold Off Duration  -------------
                  # follow the table of bAdcHoldOff
                  #self.AdcHoldOff                    = 255    # us  255~1275 mutiple of 85
                  255,
                  #self.SmokeAlarmHoldOff             = 255   # us  255~1275 mutiple of 85
                  255]
    # -------------  Buttons  -------------
    buttons = [# The length must be 23. It's the actual value of different key setting, 0 = Not used
               #self.ValidButtons                  = [8]*23 # bValidButtons[23]
               [8]*23,
               # The sum of below three values should be less than 2000ms
               #self.PushQualThreshTime            = 30     # ms Button Qualification Time(multiple of 5ms)
               30,
               #self.ExpectedChatterTimeofButtons  = 20     # ms
               20,
               #self.MultiButtonPressTimeDiff      = 20     # ms
               20,
               #self.WaitTimeBeforeShutDownMargin  = 10     # ms
               10]
    # --------  Packet Setup  --------
    packet_setup = [ #self.Packet0Enabled  =  True
                     True,
                     #self.Packet1Enabled  =  True
                     True,
                     #self.Packet2Enabled  =  True
                     True,
                     #self.Packet3Enabled  =  True
                     True,
                     #self.LEDonTime       =  20
                     20,
                     #self.AddiDelayBetweenPack      = 0     # multiple of 5ms
                     0,
                     #self.RepeatInterval            =  190  # ms
                     190,
                     #self.MaxButtonDuration         = 10    # second
                     10,
                     #self.MinNumOfTXsPerPress       = 3     # # of TX's
                     3,
                     #self.MinNumOfTXsaftBatterySwap = 6     # # of TX's
                     6,
                     #self.PacketBD = 
                     [1, 2, 3, 4, 21, 22, 23, 24, 41, 42, 43, 44, 61, 62, 63, 64, 81, 82, 83, 84, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
                     #self.PacketTX = 
                     ([8]*3 + [4]*20 + [2]*9)
                     ]
    # --------  Fixed Packet  --------
    fixed_packet = [ # Restrictions
                     #   Buttons             Length
                     #   1                   4~96
                     #   2                   4~48
                     #   4                   4~24
                     #   8                   4~12
                     #self.FixedPacketButtons  = 4  # Buttons of Fixed Packet
                     4,
                     #self.FixedPacketLength   = 20 # Length of  Fixed Packet
                     20,
                     #self.PacketOverRide      = [0]*96 # TODO restriction
                     [0]*96
                     ]
    # --------  Misc Setup  --------
    misc_setup = [ # Drop list
                   #self.LEDIntensity          = '0.37 mA'  # 'Off' , '0.37 mA' , '0.60 mA' , '0.97 mA'
                   '0.37 mA',
                   #self.PeriodicLEDIntensity  = '0.37 mA'  # 'Off' , '0.37 mA' , '0.60 mA' , '0.97 mA'
                   '0.37 mA',
                   #self.BattThreshToDisLED    = 2300       # 1500~3600
                   2300,
                   #self.BattThreshToDisTrans  = 1800       # 1500~3600
                   1800,
                   #self.TempThreshToUsAlterPA = -500       # -500~1500
                   -500,
                   #self.AlterPAOutPower       = 0          # dBm
                   0,
                   #self.AlterPAType           = 'Min PA & Max Radiated Power'  # or 'Max Radiated Power'
                   'Min PA & Max Radiated Power',
                   #self.RefClockType          = 'Internal Ref Clock' # 'Internal Ref Clock' , 'External Crystal' , 'External Si501'
                   'Internal Ref Clock',
                   #self.ExtCrystalFreq        = 10000000   #10000000~13000000(10MHz~13MHz)
                   10000000,
                   #self.ExtCrystalLowCap      = 0          # 0 / 1
                   0
                   ]
    # Security
    security =  [ #self.OEM_Key[16]          = []         # [] or 16 elements
                  [],
                  #self.OEM_ID_Key           = None        # [] or a integer
                  0]
    # Holtek emulation
    holtek_emu =  [ #self.HoltekEmuDataBits     = 4          # Data bits  (0~7)
                    4,
                    #self.HoltekEmuMappingType  = 'Brazil'   # 'Brazil', 'Direct', 'Custom'
                    'Brazil',
                    #self.HoltekEmuCustomGpioMappingTable = [0x08]*23 # Correspoding to bValidButtons[23]
                    [0x08]*23
                    ]

    inputs = []
    inputs.append(timer)
    inputs.append(top_level)
    inputs.append(buttons)
    inputs.append(packet_0)
    inputs.append(packet_1)
    inputs.append(packet_2)
    inputs.append(packet_3)
    inputs.append(packet_setup)
    inputs.append(fixed_packet)
    inputs.append(misc_setup)
    inputs.append(security)
    inputs.append(holtek_emu)
    
    def __init__(self):
        pass
        