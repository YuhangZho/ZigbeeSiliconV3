'''
Created on July 25, 2013

@author: shyang
'''
import copy
import os
import sys
import si4010_cfg_calc

packet_para_template_HoltekHP6P20 = { #self.Packet3 = {
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
                                                 0   ,     7.7,                40,                   'No',                    100,            40,
                                                 # --------  Power Amplifier Setup  --------
                                                 #TotalPAPower(dBm)    PAType'                         CenterFrequency(MHZ)    NominalCap    ExternalDiffCap    Q_FactorOfExtCap
                                                 0,                    'Min PA & Max Radiated Power',  433.920,                30,           1,                 300,
                                                 # --------  Serializer Setup  --------
                                                 #DataRate(kbits/s)    Encoding      Modulation    FSK_Deviation(kHZ)    ManualRampRateEntry    TargetRampRate
                                                 2.141,                   'NRZ Only',   'OOK',        43.2,                 'No',                  12
                                                 ]
                                      }

# --------  Test cases definitions  --------
# --------  Sleep Timer  --------
TestCases_SleepTimer = [
    { 'in'  : {'EnPeriodicTransmit'  : 0,
               'AutoTxRate'          : -1,     # -1, Tx threshold crossing
               'SampleInterval'      : 3600.0/8192,
               },
      'out' : {'wSleepTimerDivide'   : 8192,
               'lSleepCycleMask'     : 0x00000300,
               'wSleepCycleMask'     : 0x0001,
               'bLoopIncrement'      : 1,
               'bSleepSubSample'     : 0xFF,  # Reset to 0xFF
               }
      # TODO check C_comments
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'AutoTxRate'          : -1,     # -1, Tx threshold crossing
               'SampleInterval'      : 3600.0/8192,
               },
      'out' : {'wSleepTimerDivide'   : 8192,
               'lSleepCycleMask'     : 0x00000300,
               'wSleepCycleMask'     : 0x0001,
               'bLoopIncrement'      : 1,
               'bSleepSubSample'     : 0xFF,  # Reset to 0xFF
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'AutoTxRate'          : -1,     # -1, Tx threshold crossing
               'SampleInterval'      : 3600.0/2,
               },
      'out' : {'wSleepTimerDivide'   : 2,
               'lSleepCycleMask'     : 0x003FE000,
               'wSleepCycleMask'     : 0x1FFF,
               'bLoopIncrement'      : 128,
               'bSleepSubSample'     : 0xFF,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'AutoTxRate'          : 0,     # -1, Tx threshold crossing
               'SampleInterval'      : 3600.0/8192,
               },
      'out' : {'wSleepTimerDivide'   : 8192,
               'lSleepCycleMask'     : 0x00000300,
               'wSleepCycleMask'     : 0x0001,
               'bLoopIncrement'      : 1,
               'bSleepSubSample'     : 0,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'AutoTxRate'          : 14,     # -1, Tx threshold crossing
               'SampleInterval'      : 3600.0/8192,
               },
      'out' : {'wSleepTimerDivide'   : 8192,
               'lSleepCycleMask'     : 0x00000300,
               'wSleepCycleMask'     : 0x0001,
               'bLoopIncrement'      : 1,
               'bSleepSubSample'     : 31,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'AutoTxRate'          : 14,     # -1, Tx threshold crossing
               'SampleInterval'      : 3600.0/4096,
               },
      'out' : {'wSleepTimerDivide'   : 4096,
               'lSleepCycleMask'     : 0x00000600,
               'wSleepCycleMask'     : 0x0003,
               'bLoopIncrement'      : 1,
               'bSleepSubSample'     : 15,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'AutoTxRate'          : 14,     # -1, Tx threshold crossing
               'SampleInterval'      : 3600.0/2048,
               },
      'out' : {'wSleepTimerDivide'   : 2048,
               'lSleepCycleMask'     : 0x00000C00,
               'wSleepCycleMask'     : 0x0007,
               'bLoopIncrement'      : 1,
               'bSleepSubSample'     : 7,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'AutoTxRate'          : 14,     # -1, Tx threshold crossing
               'SampleInterval'      : 3600.0/1024,
               },
      'out' : {'wSleepTimerDivide'   : 1024,
               'lSleepCycleMask'     : 0x00001800,
               'wSleepCycleMask'     : 0x000F,
               'bLoopIncrement'      : 1,
               'bSleepSubSample'     : 3,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'AutoTxRate'          : 14,     # -1, Tx threshold crossing
               'SampleInterval'      : 3600.0/512,
               },
      'out' : {'wSleepTimerDivide'   : 512,
               'lSleepCycleMask'     : 0x00003000,
               'wSleepCycleMask'     : 0x001F,
               'bLoopIncrement'      : 1,
               'bSleepSubSample'     : 1,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'AutoTxRate'          : 14,     # -1, Tx threshold crossing
               'SampleInterval'      : 3600.0/256,
               },
      'out' : {'wSleepTimerDivide'   : 256,
               'lSleepCycleMask'     : 0x00006000,
               'wSleepCycleMask'     : 0x003F,
               'bLoopIncrement'      : 1,
               'bSleepSubSample'     : 0,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'AutoTxRate'          : 14,     # -1, Tx threshold crossing
               'SampleInterval'      : 3600.0/128,
               },
      'out' : {'wSleepTimerDivide'   : 128,
               'lSleepCycleMask'     : 0x0000E000,
               'wSleepCycleMask'     : 0x007F,
               'bLoopIncrement'      : 2,
               'bSleepSubSample'     : 0,
               }
      },
]

# --------  Top level setting  --------
TestCases_TopLevel = [
    #     ----  bConfigSetting  ----
    { 'in'  : {'EnTestMode'                 : 1, # B7
               'EnThermostat'               : 1, # B6
               'GP9DrivenLOWforADCMeasure'  : 1, # B5
               'EnStuckButton'              : 1, # B4
      
               'EnMultiCfgOTP'              : 1, # B3
               # B2 Reserved to 0
               'EnPeriodicTransmit'         : 1, # B1
               'AltPAForPerodicTransmit'         : 1, # B0
               },
      'out' : {'bConfigSetting' : 0xFB}
      },
    { 'in'  : {'EnTestMode'                 : 0, # B7
               'EnThermostat'               : 1, # B6
               'GP9DrivenLOWforADCMeasure'  : 1, # B5
               'EnStuckButton'              : 1, # B4

               'EnMultiCfgOTP'              : 1, # B3
               # B2 Reserved to 0
               'EnPeriodicTransmit'         : 1, # B1
               'AltPAForPerodicTransmit'         : 1, # B0
               },
      'out' : {'bConfigSetting' : 0x7B}
      },
    { 'in'  : {'EnTestMode'                 : 1, # B7
               'EnThermostat'               : 0, # B6
               'GP9DrivenLOWforADCMeasure'  : 1, # B5
               'EnStuckButton'              : 1, # B4

               'EnMultiCfgOTP'              : 1, # B3
               # B2 Reserved to 0
               'EnPeriodicTransmit'         : 1, # B1
               'AltPAForPerodicTransmit'         : 1, # B0
               },
      'out' : {'bConfigSetting' : 0xBB}
      },
    { 'in'  : {'EnTestMode'                 : 1, # B7
               'EnThermostat'               : 1, # B6
               'GP9DrivenLOWforADCMeasure'  : 0, # B5
               'EnStuckButton'              : 1, # B4

               'EnMultiCfgOTP'              : 1, # B3
               # B2 Reserved to 0
               'EnPeriodicTransmit'         : 1, # B1
               'AltPAForPerodicTransmit'         : 1, # B0
               },
      'out' : {'bConfigSetting' : 0xDB}
      },
    { 'in'  : {'EnTestMode'                 : 1, # B7
               'EnThermostat'               : 1, # B6
               'GP9DrivenLOWforADCMeasure'  : 1, # B5
               'EnStuckButton'              : 0, # B4

               'EnMultiCfgOTP'              : 1, # B3
               # B2 Reserved to 0
               'EnPeriodicTransmit'         : 1, # B1
               'AltPAForPerodicTransmit'         : 1, # B0
               },
      'out' : {'bConfigSetting' : 0xEB}
      },
    { 'in'  : {'EnTestMode'                 : 1, # B7
               'EnThermostat'               : 1, # B6
               'GP9DrivenLOWforADCMeasure'  : 1, # B5
               'EnStuckButton'              : 1, # B4

               'EnMultiCfgOTP'              : 0, # B3
               # B2 Reserved to 0
               'EnPeriodicTransmit'         : 1, # B1
               'AltPAForPerodicTransmit'         : 1, # B0
               },
      'out' : {'bConfigSetting' : 0xF3}
      },
    { 'in'  : {'EnTestMode'                 : 1, # B7
               'EnThermostat'               : 1, # B6
               'GP9DrivenLOWforADCMeasure'  : 1, # B5
               'EnStuckButton'              : 1, # B4

               'EnMultiCfgOTP'              : 1, # B3
               # B2 Reserved to 0
               'EnPeriodicTransmit'         : 0, # B1
               'AltPAForPerodicTransmit'         : 1, # B0
               },
      'out' : {'bConfigSetting' : 0xF9}
      },
    { 'in'  : {'EnTestMode'                 : 1, # B7
               'EnThermostat'               : 1, # B6
               'GP9DrivenLOWforADCMeasure'  : 1, # B5
               'EnStuckButton'              : 1, # B4

               'EnMultiCfgOTP'              : 1, # B3
               # B2 Reserved to 0
               'EnPeriodicTransmit'         : 1, # B1
               'AltPAForPerodicTransmit'         : 0, # B0
               },
      'out' : {'bConfigSetting' : 0xFA}
      },
    #     ---- bUserData ----
    { 'in'  : {'OTPcfgNum'              : 0,
               'DataInTwoBytesPayload'  : 0,
               },
      'out' : {'bUserData'              : 0},
      },
    { 'in'  : {'OTPcfgNum'              : 1,
               'DataInTwoBytesPayload'  : 3,
               },
      'out' : {'bUserData'              : 0x31},
      },

    #     ---- User Payload data ----
    { 'in'  : {'UserPayloadData0'  : 0x32,
               'UserPayloadData1'  : 0x23,
               },
      'out' : {'bUserPayloadData0' : 0x32,
               'bUserPayloadData1' : 0x23,
               }
      },
    #     ---- AdcOrTempThreshold ----
    { 'in'  : {'DataInTwoBytesPayload' : 1,    # Temperature
               'AdcOrTempThresholdH'   : 352,
               'AdcOrTempThresholdL'   : 854,
               },
      'out' : {'iAdcOrTempThresholdH' : 352,
               'iAdcOrTempThresholdL' : 854,
               }
      },
    { 'in'  : {'DataInTwoBytesPayload' : 1,    # Temperature
               'AdcOrTempThresholdH'   : -204,
               'AdcOrTempThresholdL'   : -401,
               },
      'out' : {'iAdcOrTempThresholdH'  : -204,
               'iAdcOrTempThresholdL'  : -401,
               }
      },
    { 'in'  : {'DataInTwoBytesPayload' : 2,    # Temperature
               'AdcOrTempThresholdH'   : -297,
               'AdcOrTempThresholdL'   : -413,
               },
      'out' : {'iAdcOrTempThresholdH'  : -297,
               'iAdcOrTempThresholdL'  : -413,
               }
      },
    #     ---- SmokeAlarmHoldOff ----
    { 'in'  : {'AdcHoldOff'          : 3*85,
               'SmokeAlarmHoldOff'   : 4*85},
      'out' : {'bAdcHoldOff'         : 0x43}
      },
    { 'in'  : {'AdcHoldOff'          : 4*85,
               'SmokeAlarmHoldOff'   : 7*85},
      'out' : {'bAdcHoldOff'         : 0x74}
      },
    { 'in'  : {'AdcHoldOff'          : 10*85,
               'SmokeAlarmHoldOff'   : 11*85},
      'out' : {'bAdcHoldOff'         : 0xBA}
      },
    { 'in'  : {'AdcHoldOff'          : 15*85,
               'SmokeAlarmHoldOff'   : 14*85},
      'out' : {'bAdcHoldOff'         : 0xEF}
      },
    { 'in'  : {'AdcHoldOff'          : 11*85,
               'SmokeAlarmHoldOff'   : 7*85},
      'out' : {'bAdcHoldOff'         : 0x7B}
      },
    ]

# --------  Buttons  --------
TestCases_Buttons = [
    { 'in'  : {'ValidButtons'  : [0x08, 0x04, 0x0C, 0x01,
                                  0x08, 0x04, 0x0C, 0x01,
                                  0x08, 0x04, 0x0C, 0x01,
                                  0x08, 0x04, 0x0C, 0x01,

                                  0x08, 0x04, 0x0C, 0x01,
                                  0x08, 0x04, 0x0C,
                                  ]},
      'out' : {'bValidButtons[23]'  : [0x08, 0x04, 0x0C, 0x01,
                                       0x08, 0x04, 0x0C, 0x01,
                                       0x08, 0x04, 0x0C, 0x01,
                                       0x08, 0x04, 0x0C, 0x01,
                                       
                                       0x08, 0x04, 0x0C, 0x01,
                                       0x08, 0x04, 0x0C,
                                       ]},
      },
    { 'in'  : {'PushQualThreshTime'             : 10,   # 10~1275ms, step 5
               'ExpectedChatterTimeofButtons'   : 20,
               'MultiButtonPressTimeDiff'       : 20,
               'WaitTimeBeforeShutDownMargin'   : 20},
      'out' : {'bPushQualThresh'         : 2,
               'wWaitForPush'            : 70},
      },
    { 'in'  : {'PushQualThreshTime'             : 20,   # 10~1275ms, step 5
               'ExpectedChatterTimeofButtons'   : 20,
               'MultiButtonPressTimeDiff'       : 20,
               'WaitTimeBeforeShutDownMargin'   : 20},
      'out' : {'bPushQualThresh'         : 4,
               'wWaitForPush'            : 80},
      },
    { 'in'  : {'PushQualThreshTime'             : 120,   # 10~1275ms, step 5
               'ExpectedChatterTimeofButtons'   : 100,
               'MultiButtonPressTimeDiff'       : 200,
               'WaitTimeBeforeShutDownMargin'   : 200},
      'out' : {'bPushQualThresh'         : 24,
               'wWaitForPush'            : 620},
      },
    { 'in'  : {'PushQualThreshTime'             : 300,   # 10~1275ms, step 5
               'ExpectedChatterTimeofButtons'   : 900,
               'MultiButtonPressTimeDiff'       : 900,
               'WaitTimeBeforeShutDownMargin'   : 900},
      'out' : {'bPushQualThresh'         : 60,
               'wWaitForPush'            : 2000},  # TODO test this invalid config
      },
    ]

# --------  Packet  --------
Holteck_HP6P20_case_1 = copy.deepcopy(packet_para_template_HoltekHP6P20)
#Packet_2p4bps['modem'][13] = 1.2  # 13th, 2.141 bps
# --------  wRepeatInterval  --------
wRepeatInterval_test_case_1 = copy.deepcopy(packet_para_template_HoltekHP6P20)



TestCases_PacketSetup = [
    # LED On Time
    # Additional Delay between packets in same interval
    # Repeat Interval
    # Maximum Button Duration before Shutdown or EEPROM erase(if enabled)(Seconds)
    # Mininum # TX's after a battery swap or ADC/Temp crossing threshold
    { 'in'  : {'Packet3Enabled'  : 1,  'Packet2Enabled'  : 1,  'Packet1Enabled'  : 1,  'Packet0Enabled'  : 1, },
      'out' : {'bPacketsPerInterval' : 0xF0}
      },
    { 'in'  : {'Packet3Enabled'  : 1,  'Packet2Enabled'  : 1,  'Packet1Enabled'  : 1,  'Packet0Enabled'  : 0,  },
      'out' : {'bPacketsPerInterval' : 0xE0}
      },
    { 'in'  : {'Packet3Enabled'  : 1,  'Packet2Enabled'  : 1,  'Packet1Enabled'  : 0,  'Packet0Enabled'  : 1,  },
      'out' : {'bPacketsPerInterval' : 0xD0}
      },
    { 'in'  : {'Packet3Enabled'  : 1,  'Packet2Enabled'  : 0,  'Packet1Enabled'  : 1,  'Packet0Enabled'  : 1,  },
      'out' : {'bPacketsPerInterval' : 0xB0}
      },
    { 'in'  : {'Packet3Enabled'  : 0,  'Packet2Enabled'  : 1,  'Packet1Enabled'  : 1,  'Packet0Enabled'  : 1,  },
      'out' : {'bPacketsPerInterval' : 0x70}
      },
    { 'in'  : {'Packet3Enabled'  : 1,  'Packet2Enabled'  : 0,  'Packet1Enabled'  : 0,  'Packet0Enabled'  : 1,  },
      'out' : {'bPacketsPerInterval' : 0x90}
      },
    # bLEDOnTime
    { 'in'  : {'LEDonTime'  : 5},
      'out' : {'bLEDOnTime' : 1}   # input/5
      },
    # bPacketDelay
    { 'in'  : {'AddiDelayBetweenPack' : 20},
      'out' : {'bPacketDelay' : 4}    # input/5
      },
    # wRepeatInterval
    { 'in'  : {'RepeatInterval'      : 100}, # if not 0, the out should be the same value
      'out' : {'wRepeatInterval'     : 100}
      },
    #             (0) enabled
    { 'in'  : {
        'Packet3Enabled'   : 0,  'Packet2Enabled'   : 0,  'Packet1Enabled'   : 0,  'Packet0Enabled'   : 1,
        'RepeatInterval'   : 0,  'LEDonTime'        : 20, 'AddiDelayBetweenPack'      : 20,

        'Packet0' : Holteck_HP6P20_case_1,  'Packet1'  : Holteck_HP6P20_case_1, 'Packet2'  : Holteck_HP6P20_case_1,  'Packet3' : Holteck_HP6P20_case_1,
               },
      'out' : {'wRepeatInterval'  : 300}
      },
    #             (1) enabled
    { 'in'  : {
        'Packet3Enabled'   : 0,  'Packet2Enabled'   : 0,  'Packet1Enabled'   : 1,  'Packet0Enabled'   : 0,
        'RepeatInterval'   : 0,  'LEDonTime'        : 20, 'AddiDelayBetweenPack'      : 20,

        'Packet0' : Holteck_HP6P20_case_1,  'Packet1'  : Holteck_HP6P20_case_1, 'Packet2'  : Holteck_HP6P20_case_1,  'Packet3' : Holteck_HP6P20_case_1,
               },
      'out' : {'wRepeatInterval'  : 300}
      },
    #             (2) enabled
    { 'in'  : {
        'Packet3Enabled'   : 0,  'Packet2Enabled'   : 1,  'Packet1Enabled'   : 0,  'Packet0Enabled'   : 0,
        'RepeatInterval'   : 0,  'LEDonTime'        : 20, 'AddiDelayBetweenPack'      : 20,

        'Packet0' : Holteck_HP6P20_case_1,  'Packet1'  : Holteck_HP6P20_case_1, 'Packet2'  : Holteck_HP6P20_case_1,  'Packet3' : Holteck_HP6P20_case_1,
               },
      'out' : {'wRepeatInterval'  : 300}
      },
    #             (3) enabled
    { 'in'  : {
        'Packet3Enabled'   : 1,  'Packet2Enabled'   : 0,  'Packet1Enabled'   : 0,  'Packet0Enabled'   : 0,
        'RepeatInterval'   : 0,  'LEDonTime'        : 20, 'AddiDelayBetweenPack'      : 20,

        'Packet0' : Holteck_HP6P20_case_1,  'Packet1'  : Holteck_HP6P20_case_1, 'Packet2'  : Holteck_HP6P20_case_1,  'Packet3' : Holteck_HP6P20_case_1,
               },
      'out' : {'wRepeatInterval'  : 300}
      },
    #             (1, 0) enabled
    { 'in'  : {
        'Packet3Enabled'   : 0,  'Packet2Enabled'   : 0,  'Packet1Enabled'   : 1,  'Packet0Enabled'   : 1,
        'RepeatInterval'   : 0,  'LEDonTime'        : 20, 'AddiDelayBetweenPack'      : 20,

        'Packet0' : Holteck_HP6P20_case_1,  'Packet1'  : Holteck_HP6P20_case_1, 'Packet2'  : Holteck_HP6P20_case_1,  'Packet3' : Holteck_HP6P20_case_1,
               },
      'out' : {'wRepeatInterval'  : 600}
      },
    #             (3,2) enabled
    { 'in'  : {
        'Packet3Enabled'   : 1,  'Packet2Enabled'   : 1,  'Packet1Enabled'   : 0,  'Packet0Enabled'   : 0,
        'RepeatInterval'   : 0,  'LEDonTime'        : 20, 'AddiDelayBetweenPack'      : 20,

        'Packet0' : Holteck_HP6P20_case_1,  'Packet1'  : Holteck_HP6P20_case_1, 'Packet2'  : Holteck_HP6P20_case_1,  'Packet3' : Holteck_HP6P20_case_1,
        },
      'out' : {'wRepeatInterval'  : 600}
      },
    #             (3,0) enabled
    { 'in'  : {
        'Packet3Enabled'   : 1,  'Packet2Enabled'   : 0,  'Packet1Enabled'   : 0,  'Packet0Enabled'   : 1,
        'RepeatInterval'   : 0,  'LEDonTime'        : 20, 'AddiDelayBetweenPack'      : 20,

        'Packet0' : Holteck_HP6P20_case_1,  'Packet1'  : Holteck_HP6P20_case_1, 'Packet2'  : Holteck_HP6P20_case_1,  'Packet3' : Holteck_HP6P20_case_1,
        },
      'out' : {'wRepeatInterval'  : 600}
      },
    #             (1,2) enabled
    { 'in'  : {
        'Packet3Enabled'   : 0,  'Packet2Enabled'   : 1,  'Packet1Enabled'   : 1,  'Packet0Enabled'   : 0,
        'RepeatInterval'   : 0,  'LEDonTime'        : 20, 'AddiDelayBetweenPack'      : 20,

        'Packet0' : Holteck_HP6P20_case_1,  'Packet1'  : Holteck_HP6P20_case_1, 'Packet2'  : Holteck_HP6P20_case_1,  'Packet3' : Holteck_HP6P20_case_1,
        },
      'out' : {'wRepeatInterval'  : 600}
      },
    #             (2,0) enabled
    { 'in'  : {
        'Packet3Enabled'   : 0,  'Packet2Enabled'   : 1,  'Packet1Enabled'   : 0,  'Packet0Enabled'   : 1,
        'RepeatInterval'   : 0,  'LEDonTime'        : 20, 'AddiDelayBetweenPack'      : 20,

        'Packet0' : Holteck_HP6P20_case_1,  'Packet1'  : Holteck_HP6P20_case_1, 'Packet2'  : Holteck_HP6P20_case_1,  'Packet3' : Holteck_HP6P20_case_1,
        },
      'out' : {'wRepeatInterval'  : 600}
      },
    #             (2,1,0) enabled
    { 'in'  : {
        'Packet3Enabled'   : 0,  'Packet2Enabled'   : 1,  'Packet1Enabled'   : 1,  'Packet0Enabled'   : 1,
        'RepeatInterval'   : 0,  'LEDonTime'        : 20, 'AddiDelayBetweenPack'      : 20,

        'Packet0' : Holteck_HP6P20_case_1,  'Packet1'  : Holteck_HP6P20_case_1, 'Packet2'  : Holteck_HP6P20_case_1,  'Packet3' : Holteck_HP6P20_case_1,
        },
      'out' : {'wRepeatInterval'  : 900}
      },
    #             (2,1,0) enabled
    { 'in'  : {
        'Packet3Enabled'   : 0,  'Packet2Enabled'   : 1,  'Packet1Enabled'   : 1,  'Packet0Enabled'   : 1,
        'RepeatInterval'   : 0,  'LEDonTime'        : 20, 'AddiDelayBetweenPack'      : 20,

        'Packet0' : Holteck_HP6P20_case_1,  'Packet1'  : Holteck_HP6P20_case_1, 'Packet2'  : Holteck_HP6P20_case_1,  'Packet3' : Holteck_HP6P20_case_1,
        },
      'out' : {'wRepeatInterval'  : 900}
      },
    #             (3,1,0) enabled
    { 'in'  : {
        'Packet3Enabled'   : 1,  'Packet2Enabled'   : 0,  'Packet1Enabled'   : 1,  'Packet0Enabled'   : 1,
        'RepeatInterval'   : 0,  'LEDonTime'        : 20, 'AddiDelayBetweenPack'      : 20,

        'Packet0' : Holteck_HP6P20_case_1,  'Packet1'  : Holteck_HP6P20_case_1, 'Packet2'  : Holteck_HP6P20_case_1,  'Packet3' : Holteck_HP6P20_case_1,
        },
      'out' : {'wRepeatInterval'  : 900}
      },
    #             (3,2,0) enabled
    { 'in'  : {
        'Packet3Enabled'   : 1,  'Packet2Enabled'   : 0,  'Packet1Enabled'   : 1,  'Packet0Enabled'   : 1,
        'RepeatInterval'   : 0,  'LEDonTime'        : 20, 'AddiDelayBetweenPack'      : 20,

        'Packet0' : Holteck_HP6P20_case_1,  'Packet1'  : Holteck_HP6P20_case_1, 'Packet2'  : Holteck_HP6P20_case_1,  'Packet3' : Holteck_HP6P20_case_1,
        },
      'out' : {'wRepeatInterval'  : 900}
      },
    #             (3,2,1) enabled
    { 'in'  : {
        'Packet3Enabled'   : 1,  'Packet2Enabled'   : 0,  'Packet1Enabled'   : 1,  'Packet0Enabled'   : 1,
        'RepeatInterval'   : 0,  'LEDonTime'        : 20, 'AddiDelayBetweenPack'      : 20,

        'Packet0' : Holteck_HP6P20_case_1,  'Packet1'  : Holteck_HP6P20_case_1, 'Packet2'  : Holteck_HP6P20_case_1,  'Packet3' : Holteck_HP6P20_case_1,
        },
      'out' : {'wRepeatInterval'  : 900}
      },
    #               Four packages enabled
    { 'in'  : {
        'Packet3Enabled'   : 1,  'Packet2Enabled'   : 1,  'Packet1Enabled'   : 1,  'Packet0Enabled'   : 1,
        'RepeatInterval'   : 0,  'LEDonTime'        : 20, 'AddiDelayBetweenPack'      : 20,

        'Packet0' : Holteck_HP6P20_case_1,  'Packet1'  : Holteck_HP6P20_case_1, 'Packet2'  : Holteck_HP6P20_case_1,  'Packet3' : Holteck_HP6P20_case_1,
        },
      'out' : {'wRepeatInterval'  : 1200}
      },
    # bMaxButtonDuration
    #     Enable 2 packets
    { 'in'  : {'Packet3Enabled'   : 0, 
               'Packet2Enabled'   : 0,
               'Packet1Enabled'   : 1,
               'Packet0Enabled'   : 1,
               'RepeatInterval'   : 0,
               'LEDonTime'        : 20,
               'AddiDelayBetweenPack'      : 20,

               'Packet0'          : Holteck_HP6P20_case_1,
               'Packet1'          : Holteck_HP6P20_case_1,
               'Packet2'          : Holteck_HP6P20_case_1,
               'Packet3'          : Holteck_HP6P20_case_1,

               'MaxButtonDuration'   : 85,
               'PacketBD'            : [1,  2,  3,  4,       21, 22, 23, 24,         41, 42, 43, 44,    61, 62, 63, 64,
                                           81, 82, 83, 84,        0,  0,  0,  0,          0,  0,  0,  0,     0,  0,  0,  0],},
      'out' : {'bMaxButtonDuration'  : 142}
      },
    #            Enable 3 packets
    { 'in'  : {'Packet3Enabled'   : 1, 
               'Packet2Enabled'   : 0,
               'Packet1Enabled'   : 1,
               'Packet0Enabled'   : 1,
               'RepeatInterval'   : 0,
               'LEDonTime'        : 20,
               'AddiDelayBetweenPack'      : 20,

               'Packet0'          : Holteck_HP6P20_case_1,
               'Packet1'          : Holteck_HP6P20_case_1,
               'Packet2'          : Holteck_HP6P20_case_1,
               'Packet3'          : Holteck_HP6P20_case_1,

               'MaxButtonDuration'   : 85,
               'PacketBD'            : [1,  2,  3,  4,       21, 22, 23, 24,         41, 42, 43, 44,    61, 62, 63, 64,
                                           81, 82, 83, 84,        0,  0,  0,  0,          0,  0,  0,  0,     0,  0,  0,  0],},
      'out' : {'bMaxButtonDuration'  : 95}
      },


    { 'in'  : {'MaxButtonDuration'   : 14,
               'MinNumOfTXsPerPress'    : 12,
               'PacketBD'               : [1,  2,  3,  4,       21, 22, 23, 24,         41, 42, 43, 44,    61, 62, 63, 64,
                                           81, 82, 83, 84,        0,  0,  0,  0,          0,  0,  0,  0,     0,  0,  0,  0]},
      'out' : {'bMinButtonDuration'     : 44}
      },
    { 'in'  : {'MinNumOfTXsaftBatterySwap'   : 9,
               'PacketBD'                    : [1,  2,  3,  4,       21, 22, 23, 24,         41, 42, 43, 44,    61, 62, 63, 64,
                                                81, 82, 83, 84,        0,  0,  0,  0,          0,  0,  0,  0,     0,  0,  0,  0]},
      'out' : {'bAltMinButtonDuration'       : 41}
      },
    { 'in'  : {'MinNumOfTXsaftBatterySwap'   : 20,
               'PacketBD'                    : [1,  2,  3,  4,       21, 22, 23, 24,         41, 42, 43, 44,    61, 62, 63, 64,
                                                81, 82, 83, 84,        0,  0,  0,  0,          0,  0,  0,  0,     0,  0,  0,  0]},
      'out' : {'bAltMinButtonDuration'       : 84}
      },
    { 'in'  : {'PacketBD'      : list(range(1, 33))},
      'out' : {'bPacketBD[32]' : list(range(1, 33))}
      },
    { 'in'  : {'PacketBD'      : list(range(2, 66, 2))},
      'out' : {'bPacketBD[32]' : list(range(2, 66, 2))}
      },
    ]

# --------  Fixed Packet  --------
# The three parameters were use to show what data will be used.
# They don't change the config data
TestCases_FixedSetup = [
    { 'in'  : {'PacketOverRide'      : list(range(96))},
      'out' : {'bPacketOverRide[96]' : list(range(96))}
      },
    { 'in'  : {'FixedPacketButtons'  : 1,
               'FixedPacketLength'   : 4,  # 1 Button, len [4, 96]
               'PacketOverRide'      : list(range(96))},
      'out' : {'bPacketOverRide[96]' : list(range(96))}
      },
]



# --------  Misc Setup  --------
TestCases_MiscSetup = [
    # ---------  LED Intensity  --------
    { 'in'  : {'LEDIntensity'  : 'Off'},
      'out' : {'bLedIntensity' : 0}
      },
    { 'in'  : {'LEDIntensity'  : '0.37 mA'},
      'out' : {'bLedIntensity' : 1}
      },
    { 'in'  : {'LEDIntensity'  : '0.60 mA'},
      'out' : {'bLedIntensity' : 2,}
      },
    { 'in'  : {'LEDIntensity'  : '0.97 mA'},
      'out' : {'bLedIntensity' : 3}
      },
    # Will be killed by the validation of input
    #{ 'in'       : {'LEDIntensity'  : '1.97 mA'},  # Invalid input, should be corrected
    #  'cor_in'   : {'LEDIntensity'  : '1.97 mA'},  # Invalid input, should be corrected
    #  'out'      : {'bLedIntensity' : 1},
    #  'UI_msg'   : {'LEDIntensity'  : 'INVALID'},
    #  },
    # --------  bPTLedIntensity  --------
    { 'in'  : {'PeriodicLEDIntensity'  : 'Off'},
      'out' : {'bPTLedIntensity' : 0,}
      },
    { 'in'  : {'PeriodicLEDIntensity'  : '0.37 mA'},
      'out' : {'bPTLedIntensity' : 1,}
      },
    { 'in'  : {'PeriodicLEDIntensity'  : '0.60 mA'},
      'out' : {'bPTLedIntensity' : 2,}
      },
    { 'in'  : {'PeriodicLEDIntensity'  : '0.97 mA'},
      'out' : {'bPTLedIntensity' : 3,}
      },
    # Will be killed by the validation of input
    #{ 'in'  : {'PeriodicLEDIntensity'  : '1 mA'},
    #  'out' : {'bPTLedIntensity' : 1,}
    #  },
    # --------  bBatThreshold  --------
    # Will be killed by the validation of input
    #{ 'in'  : {'BattThreshToDisLED'  : 1300},
    #  'out' : {'bBatThreshold'       : 0,}
    #  },
    { 'in'  : {'BattThreshToDisLED'  : 1500},
      'out' : {'bBatThreshold'       : 0,}
      },
    { 'in'  : {'BattThreshToDisLED'  : 2300},
      'out' : {'bBatThreshold'       : 80,}
      },
    { 'in'  : {'BattThreshToDisLED'  : 3600},
      'out' : {'bBatThreshold'       : 210,}
      },
    # Will be killed by the validation of input
    #{ 'in'  : {'BattThreshToDisLED'  : 3800},
    #  'out' : {'bBatThreshold'       : 210,}
    #  },
    # --------  bBatThreshold  --------
    # Will be killed by the validation of input
    #{ 'in'  : {'BattThreshToDisTrans'  : 1300},
    #  'out' : {'wBatNoTX'       : 0,}
    #  },
    { 'in'  : {'BattThreshToDisTrans'  : 1500},
      'out' : {'wBatNoTX'       : 1500,}
      },
    { 'in'  : {'BattThreshToDisTrans'  : 2300},
      'out' : {'wBatNoTX'       : 2300,}
      },
    { 'in'  : {'BattThreshToDisTrans'  : 2314},
      'out' : {'wBatNoTX'       : 2314,}
      },
    # Will be killed by the validation of input
    #{ 'in'  : {'BattThreshToDisTrans'  : 2314.99},
    #  'out' : {'wBatNoTX'       : 81,}
    #  },
    { 'in'  : {'BattThreshToDisTrans'  : 2315},
      'out' : {'wBatNoTX'       : 2315,}
      },
    { 'in'  : {'BattThreshToDisTrans'  : 3600},
      'out' : {'wBatNoTX'       : 3600,}
      },
    # Will be killed by the validation of input
    #{ 'in'  : {'BattThreshToDisTrans'  : 3800},
    #  'out' : {'wBatNoTX'       : 210,}
    #  },
    # --------  iLowTempThreshold  --------
    { 'in'  : {'TempThreshToUsAlterPA'   : -500},
      'out' : {'iLowTempThreshold'       : -500,}
      },
    { 'in'  : {'TempThreshToUsAlterPA'   : -312},
      'out' : {'iLowTempThreshold'       : -312,}
      },
    { 'in'  : {'TempThreshToUsAlterPA'   : -334},
      'out' : {'iLowTempThreshold'       : -334,}
      },
    { 'in'  : {'TempThreshToUsAlterPA'   : 434},
      'out' : {'iLowTempThreshold'       : 434,}
      },
    # --------  Alternate PA  --------
        # TODO
    # --------  Reference Clock  --------
    { 'in'  : {'RefClockType'      : 'Internal Ref Clock',
               'ExtCrystalFreq'    : 13000000, # doesn't care
               'ExtCrystalLowCap'  : 1,}, # doesn't care
      'out' : {'bUseExternalXo'    : 0,
               'rFCast_XoSetup'    : [10000000, 0]},
      },
    { 'in'  : {'RefClockType'      : 'External Crystal',
               'ExtCrystalFreq'    : 10000000, # doesn't care
               'ExtCrystalLowCap'  : 1,}, # doesn't care
      'out' : {'bUseExternalXo'    : 1,
               'rFCast_XoSetup'    : [10000000, 1]},
      },
    { 'in'  : {'RefClockType'      : 'External Crystal',
               'ExtCrystalFreq'    : 10000000, # doesn't care
               'ExtCrystalLowCap'  : 1}, # doesn't care
      'out' : {'bUseExternalXo'    : 1,
               'rFCast_XoSetup'    : [10000000, 1]},
      },
    { 'in'  : {'RefClockType'      : 'External Crystal',
               'ExtCrystalFreq'    : 10000000, # doesn't care
               'ExtCrystalLowCap'  : 1,}, # doesn't care
      'out' : {'bUseExternalXo'    : 1,
               'rFCast_XoSetup'    : [10000000, 1]},
      },
    { 'in'  : {'RefClockType'      : 'External Si501',
               'ExtCrystalFreq'    : 12000000, # doesn't care
               'ExtCrystalLowCap'  : 1,}, # doesn't care
      'out' : {'bUseExternalXo'    : 2,
               'rFCast_XoSetup'    : [10000000, 1]},
      },
    ]








# --------  Security  --------
TestCases_Security = [
    { 'in'  : {'OTPcfgNum'    : 0,
               'OEM_Key'      : []},
      'out' : {'bOEM_Key[16]' : [0x63, 0xB4, 0x92, 0xCD, 0x42, 0x20, 0x03, 0xBC, 0x73, 0x29, 0x09, 0xBB, 0xFF, 0x6A, 0xDC, 0x6D] }
      },
    { 'in'  : {'OTPcfgNum'    : 1,
               'OEM_Key'      : []},
      'out' : {'bOEM_Key[16]' : [0x63, 0x4B, 0x92, 0xCD, 0x42, 0x20, 0x03, 0xBC, 0x73, 0x29, 0x09, 0xBB, 0xFF, 0x6A, 0xDC, 0x6D] }
      },
    { 'in'  : {'OTPcfgNum'    : 2,
               'OEM_Key'      : []},
      'out' : {'bOEM_Key[16]' : [0x63, 0xB4, 0x29, 0xCD, 0x42, 0x20, 0x03, 0xBC, 0x73, 0x29, 0x09, 0xBB, 0xFF, 0x6A, 0xDC, 0x6D], } #2
      },
    { 'in'  : {'OTPcfgNum'    : 3,
               'OEM_Key'      : []},
      'out' : {'bOEM_Key[16]' : [0x63, 0xB4, 0x92, 0xDC, 0x42, 0x20, 0x03, 0xBC, 0x73, 0x29, 0x09, 0xBB, 0xFF, 0x6A, 0xDC, 0x6D], } #3
      },
    { 'in'  : {'OTPcfgNum'    : 4,
               'OEM_Key'      : []},
      'out' : {'bOEM_Key[16]' : [0x63, 0xB4, 0x92, 0xCD, 0x24, 0x20, 0x03, 0xBC, 0x73, 0x29, 0x09, 0xBB, 0xFF, 0x6A, 0xDC, 0x6D], } #4
      },
    { 'in'  : {'OTPcfgNum'    : 5,
               'OEM_Key'      : []},
      'out' : {'bOEM_Key[16]' : [0x63, 0xB4, 0x92, 0xCD, 0x42, 0x02, 0x03, 0xBC, 0x73, 0x29, 0x09, 0xBB, 0xFF, 0x6A, 0xDC, 0x6D], } #5
      },
    { 'in'  : {'OTPcfgNum'    : 6,
               'OEM_Key'      : []},
      'out' : {'bOEM_Key[16]' : [0x63, 0xB4, 0x92, 0xCD, 0x42, 0x20, 0x30, 0xBC, 0x73, 0x29, 0x09, 0xBB, 0xFF, 0x6A, 0xDC, 0x6D], } #6
      },
    { 'in'  : {'OTPcfgNum'    : 7,
               'OEM_Key'      : []},
      'out' : {'bOEM_Key[16]' : [0x63, 0xB4, 0x92, 0xCD, 0x42, 0x20, 0x03, 0xCB, 0x73, 0x29, 0x09, 0xBB, 0xFF, 0x6A, 0xDC, 0x6D], } #7
      },
    { 'in'  : {'OTPcfgNum'    : 1,
               'OEM_ID_Key'   : None},
      'out' : {'lOEM_ID_Key'  : 0x00000000}
      },
    { 'in'  : {'OTPcfgNum'    : 2,
               'OEM_ID_Key'   : None},
      'out' : {'lOEM_ID_Key'  : 0x00000000}
      },
    { 'in'  : {'OTPcfgNum'    : 3,
               'OEM_ID_Key'   : None},
      'out' : {'lOEM_ID_Key'  : 0x00000000}
      },
    { 'in'  : {'OTPcfgNum'    : 4,
               'OEM_ID_Key'   : None},
      'out' : {'lOEM_ID_Key'  : 0x00000000}
      },
    { 'in'  : {'OTPcfgNum'    : 5,
               'OEM_ID_Key'   : None},
      'out' : {'lOEM_ID_Key'  : 0x00000000}
      },
    { 'in'  : {'OTPcfgNum'    : 6,
               'OEM_ID_Key'   : None},
      'out' : {'lOEM_ID_Key'  : 0x00000000}
      },
    { 'in'  : {'OTPcfgNum'    : 7,
               'OEM_ID_Key'   : None},
      'out' : {'lOEM_ID_Key'  : 0x00000000}
      },
    ]


# --------  Holteck emulation  --------
TestCases_HolteckEmulation = [
    { 'in'  : {'HoltekEmuDataBits': 0},
      'out' : {'bHoltekDataMask'  : 0xFF}
      },
    { 'in'  : {'HoltekEmuDataBits': 1},
      'out' : {'bHoltekDataMask'  : 0x7F}
      },
    { 'in'  : {'HoltekEmuDataBits': 2},
      'out' : {'bHoltekDataMask'  : 0x3F}
      },
    { 'in'  : {'HoltekEmuDataBits': 3},
      'out' : {'bHoltekDataMask'  : 0x1F}
      },
    { 'in'  : {'HoltekEmuDataBits': 4},
      'out' : {'bHoltekDataMask'  : 0x0F}
      },
    { 'in'  : {'HoltekEmuDataBits': 5},
      'out' : {'bHoltekDataMask'  : 0x07}
      },
    { 'in'  : {'HoltekEmuDataBits': 6},
      'out' : {'bHoltekDataMask'  : 0x03}
      },
    { 'in'  : {'HoltekEmuDataBits': 7},
      'out' : {'bHoltekDataMask'  : 0x01}
      },
    { 'in'  : {'HoltekEmuMappingType'             : 'Brazil',
               'ValidButtons'                     : [1]*4 + [4]*4    + [8]*4 +    [0x10]*4 + [0]*4    + [0xFF]*3,
               'HoltekEmuCustomGpioMappingTable'  : [0]*23},  # doens't care
      'out' : {'bHoltekGpioMap[23]'               : [0]*4 + [0x40]*4 + [0xC0]*4 + [0x80]*4 + [0xFF]*4 + [0xFF]*3}
      },
    { 'in'  : {'HoltekEmuMappingType'             : 'Direct',
               'ValidButtons'                     : list(range(23)),
               'HoltekEmuCustomGpioMappingTable'  : [0]*23},  # doens't care
      'out' : {'bHoltekGpioMap[23]'               : list(range(23))}
      },
    { 'in'  : {'HoltekEmuMappingType'             : 'Custom',
               'HoltekEmuCustomGpioMappingTable'  : list(range(23))},
      'out' : {'bHoltekGpioMap[23]'               : list(range(23))}
      },

    ]


TestCases_PQ_cfgs = [
    # Periodic Transmit disabled
    { 'in'  : {'EnPeriodicTransmit'  : 0,
               'SampleInterval'      : 3600.0/8192,
               },
      'out' : {'lPrimaryWindow'   : 0x00003FFF,
               'lResyncLimit'     : 0x0003FFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    # Periodic Transmit disabled
    { 'in'  : {'EnPeriodicTransmit'  : 0,
               'SampleInterval'      : 3600.0/4096,
               },
      'out' : {'lPrimaryWindow'   : 0x00003FFF,
               'lResyncLimit'     : 0x0003FFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    # Periodic Transmit disabled
    { 'in'  : {'EnPeriodicTransmit'  : 0,
               'SampleInterval'      : 3600.0/2048,
               },
      'out' : {'lPrimaryWindow'   : 0x00003FFF,
               'lResyncLimit'     : 0x0003FFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    # Periodic Transmit disabled
    { 'in'  : {'EnPeriodicTransmit'  : 0,
               'SampleInterval'      : 3600.0/1024,
               },
      'out' : {'lPrimaryWindow'   : 0x00003FFF,
               'lResyncLimit'     : 0x0003FFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    # Periodic Transmit disabled
    { 'in'  : {'EnPeriodicTransmit'  : 0,
               'SampleInterval'      : 3600.0/512,
               },
      'out' : {'lPrimaryWindow'   : 0x00003FFF,
               'lResyncLimit'     : 0x0003FFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    # Periodic Transmit disabled
    { 'in'  : {'EnPeriodicTransmit'  : 0,
               'SampleInterval'      : 3600.0/256,
               },
      'out' : {'lPrimaryWindow'   : 0x00003FFF,
               'lResyncLimit'     : 0x0003FFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    # Periodic Transmit disabled
    { 'in'  : {'EnPeriodicTransmit'  : 0,
               'SampleInterval'      : 3600.0/128,
               },
      'out' : {'lPrimaryWindow'   : 0x00003FFF,
               'lResyncLimit'     : 0x0003FFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    # Periodic Transmit disabled
    { 'in'  : {'EnPeriodicTransmit'  : 0,
               'SampleInterval'      : 3600.0/64,
               },
      'out' : {'lPrimaryWindow'   : 0x00003FFF,
               'lResyncLimit'     : 0x0003FFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    # Periodic Transmit disabled
    { 'in'  : {'EnPeriodicTransmit'  : 0,
               'SampleInterval'      : 3600.0/32,
               },
      'out' : {'lPrimaryWindow'   : 0x00003FFF,
               'lResyncLimit'     : 0x0003FFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    # Periodic Transmit disabled
    { 'in'  : {'EnPeriodicTransmit'  : 0,
               'SampleInterval'      : 3600.0/16,
               },
      'out' : {'lPrimaryWindow'   : 0x00007FFF,
               'lResyncLimit'     : 0x0007FFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    # Periodic Transmit disabled
    { 'in'  : {'EnPeriodicTransmit'  : 0,
               'SampleInterval'      : 3600.0/8,
               },
      'out' : {'lPrimaryWindow'   : 0x0000FFFF,
               'lResyncLimit'     : 0x000FFFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    # Periodic Transmit disabled
    { 'in'  : {'EnPeriodicTransmit'  : 0,
               'SampleInterval'      : 3600.0/4,
               },
      'out' : {'lPrimaryWindow'   : 0x0001FFFF,
               'lResyncLimit'     : 0x001FFFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    # Periodic Transmit disabled
    { 'in'  : {'EnPeriodicTransmit'  : 0,
               'SampleInterval'      : 3600.0/2,
               },
      'out' : {'lPrimaryWindow'   : 0x0003FFFF,
               'lResyncLimit'     : 0x003FFFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    # Periodic Transmit disabled
    { 'in'  : {'EnPeriodicTransmit'  : 0,
               'SampleInterval'      : 3600.0/1,
               },
      'out' : {'lPrimaryWindow'   : 0x0007FFFF,
               'lResyncLimit'     : 0x007FFFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    # Periodic Transmit Enabled
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'SampleInterval'      : 3600.0/8192,
               },
      'out' : {'lPrimaryWindow'   : 0x0029FFFF,
               'lResyncLimit'     : 0x0221FFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    # Periodic Transmit Enabled
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'SampleInterval'      : 3600.0/4096,
               },
      'out' : {'lPrimaryWindow'   : 0x0029FFFF,
               'lResyncLimit'     : 0x0221FFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'SampleInterval'      : 3600.0/2048,
               },
      'out' : {'lPrimaryWindow'   : 0x0029FFFF,
               'lResyncLimit'     : 0x0221FFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'SampleInterval'      : 3600.0/1024,
               },
      'out' : {'lPrimaryWindow'   : 0x0029FFFF,
               'lResyncLimit'     : 0x0221FFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'SampleInterval'      : 3600.0/512,
               },
      'out' : {'lPrimaryWindow'   : 0x0029FFFF,
               'lResyncLimit'     : 0x0221FFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'SampleInterval'      : 3600.0/256,
               },
      'out' : {'lPrimaryWindow'   : 0x0029FFFF,
               'lResyncLimit'     : 0x0221FFFF,
               'wResyncWindow'    : 0x04B,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'SampleInterval'      : 3600.0/128,
               },
      'out' : {'lPrimaryWindow'   : 0x0029FFFF,
               'lResyncLimit'     : 0x0221FFFF,
               'wResyncWindow'    : 0x098,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'SampleInterval'      : 3600.0/64,
               },
      'out' : {'lPrimaryWindow'   : 0x0029FFFF,
               'lResyncLimit'     : 0x0221FFFF,
               'wResyncWindow'    : 0x0132,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'SampleInterval'      : 3600.0/32,
               },
      'out' : {'lPrimaryWindow'   : 0x0029FFFF,
               'lResyncLimit'     : 0x0221FFFF,
               'wResyncWindow'    : 0x0265,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'SampleInterval'      : 3600.0/16,
               },
      'out' : {'lPrimaryWindow'   : 0x0029FFFF,
               'lResyncLimit'     : 0x0221FFFF,
               'wResyncWindow'    : 0x04CB,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'SampleInterval'      : 3600.0/8
               },
      'out' : {'lPrimaryWindow'   : 0x0029FFFF,
               'lResyncLimit'     : 0x0221FFFF,
               'wResyncWindow'    : 0x0998,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'SampleInterval'      : 3600.0/4,
               },
      'out' : {'lPrimaryWindow'   : 0x0029FFFF,
               'lResyncLimit'     : 0x0221FFFF,
               'wResyncWindow'    : 0x01332,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'SampleInterval'      : 3600.0/2,
               },
      'out' : {'lPrimaryWindow'   : 0x0029FFFF,
               'lResyncLimit'     : 0x0221FFFF,
               'wResyncWindow'    : 0x02665,
               }
      },
    { 'in'  : {'EnPeriodicTransmit'  : 1,
               'SampleInterval'      : 3600.0/1,
               },
      'out' : {'lPrimaryWindow'   : 0x0029FFFF,
               'lResyncLimit'     : 0x0221FFFF,
               'wResyncWindow'    : 0x04CCB,
               }
      },

    # PacketBD
    { 'in'  : {'PacketBD'       : [1,  2,  3,  4,        21, 22, 23, 24,         41, 42, 43, 44,    61, 62, 63, 64,
                                   81, 82, 83, 84,        0,  0,  0,  0,          0,  0,  0,  0,     0,  0,  0,  0],
               },
      'out' : {'bPacketBD[32]'  : [1,  2,  3,  4,        21, 22, 23, 24,         41, 42, 43, 44,    61, 62, 63, 64,
                                   81, 82, 83, 84,       84, 84, 84, 84,         84, 84, 84, 84,    84, 84, 84, 84],
               }
      },
    { 'in'  : {'PacketBD'       : [1,  3,  0,  0,        21, 24,  0,  0,         41, 42, 43, 44,    61, 62, 63, 64,
                                   81, 82, 83, 84,        0,  0,  0,  0,          0,  0,  0,  0,     0,  0,  0,  0],
               },
      'out' : {'bPacketBD[32]'  : [1,  3,  3,  3,        21, 24, 24, 24,         41, 42, 43, 44,    61, 62, 63, 64,
                                   81, 82, 83, 84,       84, 84, 84, 84,         84, 84, 84, 84,    84, 84, 84, 84],
               }
      },
    ]





# --------  Global varialbes  --------                
confg_var_list   = ["bButtonMask",
                    "bValidButtons[23]",
                    "bPushQualThresh",
                    "wWaitForPush",
                    "wRepeatInterval",
                    "bPacketBD[32]",
                    "bPacketTX[32]",
                    "bPacketsPerInterval",
                    "bPacketDelay",
                    "bMinButtonDuration",
                    "bAltMinButtonDuration",
                    "bMaxButtonDuration",
                    "bLEDOnTime",
                    "bLedIntensity",
                    "bBatThreshold",
                    "wBatNoTX",
                    "iLowTempThreshold",
                    "rAlternatePaSetup",
                    "bConfigSetting",
                    "bPTLedIntensity",
                    "bSleepSubSample",
                    "wSleepTimerDivide",
                    "lSleepCycleMask",
                    "wSleepCycleMask",
                    "bLoopIncrement",
                    "bPacketLength[4]",
                    "bPreamble[4]",
                    "wSyncWord[4]",
                    "fDesiredFreq[4]",
                    "bFskDev[4]",
                    "rPaSetup[4]",
                    "rOdsSetup[4]",
                    "bEncode[4]",
                    "bUseExternalXo",
                    "rFCast_XoSetup",
                    "bOEM_Key[16]",
                    "lOEM_ID_Key",
                    "bUserData",
                    "bAdcHoldOff",
                    "iAdcOrTempThresholdH",
                    "iAdcOrTempThresholdL",
                    "bUserPayloadData0",
                    "bUserPayloadData1",
                    "bPacketOverRide[96]",
                    "bHoltekGpioMap[23]",
                    "bHoltekDataMask",
                    "bReserve0",
                    "bReserve1",
                    "bReserve2",
                    "bReserve3",
                    ]
#-----------------------------
# Test Cases

def run_test_cases(cases, PQ_cfgs = False):
    print("----> ----> To run {} test cases  :".format(len(cases)))
    def get_inputs(overrided_inputs):
        inputs = si4010_cfg_calc.Si4010CfgCalcInputs()
        for member in overrided_inputs:
            if member in ["Packet0", "Packet1", "Packet2", "Packet3"]:
                # the test knows that inputs.Packet[0:3] have method set()
                inputs.__dict__[member].set(overrided_inputs[member])
            else:
                for obj in inputs.__dict__.values():
                    if hasattr(obj, member):
                        setattr(obj, member, overrided_inputs[member])
                    continue
        inputs.check_inputs()
        return inputs

    def check_outputs(expected_outputs, cfg_calc_cfg_list, cfg_calc_corrected_inputs):
        errors = []
        for m in expected_outputs:
            if expected_outputs[m] != cfg_calc_cfg_list[m]:
                errors.append([' Configuration value: ' + m, expected_outputs[m], cfg_calc_cfg_list[m]])

        return errors

    i = 0
    for c in cases:
        #print(i, 'th test case')
        i += 1
        inputs     = get_inputs(c['in'])
        ori_inputs = copy.deepcopy(inputs)
        cfg_calc  = si4010_cfg_calc.Si4010CfgCalc(inputs)
        cfg_calc.calculate()
        cfgs    = cfg_calc.get_cfg_list()
        cfgs_PQ = cfg_calc.get_cfg_PQ_list()
        if PQ_cfgs == False:
            errors = check_outputs(c['out'], cfgs, inputs)
        else:
            errors = check_outputs(c['out'], cfgs_PQ, inputs)

        if errors:
            print("\nxxxxxxxxxxxxxxxx       Failed    xxxxxxxxxxxxxxx\n")
            for e in errors:
                print('{} expected:< {} >, got: < {} >. Test config: < {} >'.format(e[0], e[1], e[2], c))
        #else:
        #    print("        Pass!")


def print_varlist(cfgs):
    for k in confg_var_list:
        print(k, '\n        ==  ', cfgs[k])


def smoke_test():
    inputs = si4010_cfg_calc.Si4010CfgCalcInputs()
    cfg_calc = si4010_cfg_calc.Si4010CfgCalc(inputs)
    cfg_calc.calculate()
    cfgs_dict    = cfg_calc.get_cfg_list()
    cfgs_PQ_dict = cfg_calc.get_cfg_PQ_list()
    print(cfgs_PQ_dict)
    #print_varlist(cfgs_dict)

    print('\n\n TX C Codes:')
    for line in cfg_calc.gen_TX_C_File():
        print(line)

    print('\n\n PQ C Codes:')
    for line in cfg_calc.gen_PQ_C_File():
        print(line)

    run_test_cases(TestCases_TopLevel)
    run_test_cases(TestCases_SleepTimer)
    run_test_cases(TestCases_Buttons)
    run_test_cases(TestCases_MiscSetup)
    run_test_cases(TestCases_FixedSetup)
    #run_test_cases(TestCases_PacketSetup)
    run_test_cases(TestCases_Security)
    run_test_cases(TestCases_HolteckEmulation)
    run_test_cases(TestCases_PacketSetup)
#     # --------  Test cases for different input parameters  --------
#     run_test_cases(TestCases_PacketSetup)
    run_test_cases(TestCases_PQ_cfgs, PQ_cfgs = True)

def main(argv=None):
    smoke_test()

if __name__ == '__main__':
    main()
    pass



