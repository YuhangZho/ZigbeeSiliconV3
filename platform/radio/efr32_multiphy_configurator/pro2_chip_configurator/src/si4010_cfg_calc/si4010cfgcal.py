'''
Created on Sept 08, 2013

@author: shyang
'''
import time
import struct
import sys
from common import ChipConfiguratorInterface
from .si4010cfgcalcbuttons          import Si4010CfgCalcButtons
from .si4010cfgcalcfixedpacket      import Si4010CfgCalcFixedPacket
from .si4010cfgcalcholtekemulation  import Si4010CfgCalcHoltekEmulation
from .si4010cfgcalcinputs           import Si4010CfgCalcInputs
from .si4010cfgcalcmiscsetup        import Si4010CfgCalcMiscSetup
from .si4010cfgcalcpacket           import Si4010CfgCalcPacket
from .si4010cfgcalcpacketsetup      import Si4010CfgCalcPacketSetup
from .si4010cfgcalcsecurity         import Si4010CfgCalcSecurity
from .si4010cfgcalcsleeptimersamplerate import Si4010CfgCalcSleepTimerSampleRate
from .si4010cfgcaltoplevelcfg       import Si4010CfgCalcTopLevelCfg




__all__ = ["Si4010CfgCalc"]

class Si4010CfgCalc(ChipConfiguratorInterface):
    '''
    classdocs
    '''

    def __init__(self, inputs=None):
        '''
        Constructor
        '''
        super(Si4010CfgCalc,self).__init__()
        if inputs == None:
            self.inputs = Si4010CfgCalcInputs()
        else:
            self.inputs = inputs
        self.cfg_list = {}
        self.cfg_PQ_list = {}

    def calculate(self):
        self.top         = Si4010CfgCalcTopLevelCfg(self.inputs)
        self.timer       = Si4010CfgCalcSleepTimerSampleRate(self.inputs)
        self.Buttons     = Si4010CfgCalcButtons(self.inputs)
        self.MiscSetup   = Si4010CfgCalcMiscSetup(self.inputs)
        self.FixedPacket = Si4010CfgCalcFixedPacket(self.inputs)
        self.Security    = Si4010CfgCalcSecurity(self.inputs)
        self.HoltekEmu  = Si4010CfgCalcHoltekEmulation(self.inputs)

        self.Packet0     = Si4010CfgCalcPacket(self.inputs.Packet0, self.FixedPacket.PacketLen)
        self.Packet1     = Si4010CfgCalcPacket(self.inputs.Packet1, self.FixedPacket.PacketLen)
        self.Packet2     = Si4010CfgCalcPacket(self.inputs.Packet2, self.FixedPacket.PacketLen)
        self.Packet3     = Si4010CfgCalcPacket(self.inputs.Packet3, self.FixedPacket.PacketLen)
        PacketTransTimes = [self.Packet0.PacketTransTime, self.Packet1.PacketTransTime, self.Packet2.PacketTransTime, self.Packet3.PacketTransTime]
        self.PacketSetup = Si4010CfgCalcPacketSetup(self.inputs, PacketTransTimes)
        self.merge_configuration_data()
        self.merge_PQ_configuration_data()


    def merge_configuration_data(self):
        cfgs  = dict(self.top.cfg, **self.timer.cfg)
        cfgs  = dict(cfgs,    **self.Buttons.cfg)
        cfgs  = dict(cfgs,    **self.PacketSetup.cfg)
        cfgs  = dict(cfgs,    **self.MiscSetup.cfg)
        cfgs  = dict(cfgs,    **self.FixedPacket.cfg)
        cfgs  = dict(cfgs,    **self.Security.cfg)
        cfgs  = dict(cfgs,    **self.HoltekEmu.cfg)

        packets = {}
        for m in self.Packet0.cfg:
            packets[m] = [self.Packet0.cfg[m], self.Packet1.cfg[m], self.Packet2.cfg[m], self.Packet3.cfg[m]]

        cfgs  = dict(cfgs, **packets)

        self.cfg_list = cfgs

    def merge_PQ_configuration_data(self):
        cfgs = dict(self.timer.cfg_PQ_file, **self.Buttons.cfg_PQ_file)
        cfgs = dict(cfgs,                   **self.PacketSetup.cfg_PQ_file)
        cfgs = dict(cfgs,                   **self.Security.cfg_PQ_file)

        self.cfg_PQ_list = cfgs
    def gen_Hex_field(self, start_addr):
        H = []
        line_address = start_addr

        def int_hex(d, size):
            hex_str = ''
            for i in range(size):   # extract 1 byte
                hex_str += '%02X'%(d&0xFF)
                d = d >> 8
            return hex_str

        def float_hex4(f):
            # The type of struct.pack() in 3.X is bytes; in 2.x is str
            if sys.version.startswith('3.'):
                return ''.join(('%2.2X'%c) for c in struct.pack('>f', f))
            else:
                return ''.join(('%2.2X'%ord(c)) for c in struct.pack('>f', f))


        def gen_line(d_and_size, addr):
            line, length = '', 0

            for data, size in d_and_size:
                # In case of the memeber is not a list, convert it to a list for iterating
                if type(data) != type([]):
                    data = [data]

                for d in data:   # Handle every object
                    if type(d) == type(0):
                        line += int_hex(d, size)
                    else:  # type is float
                        line += float_hex4(d)
                    length += size
            if length > 16:
                raise RuntimeError('Too many bytes for a line in HEX file!')

            line = '%02X'%length + '%04X'%addr + '00' + line

            if len(line)%2 != 0:
                raise RuntimeError("Length {} of the string in hex file is not odd!".format(len(line)))
            checksum = 0x100 - (0xFF&sum(int(line[2*i : 2*i + 2], 16) for i in range(len(line)//2)))

            # append checksum
            line = ':' + line + '%02X'%checksum
            
            return [line, length]


        def pack_line(line_str, h, addr):
            h.append(line_str[0])
            return (addr + line_str[1]) # addre of data in next line

        # 16 bytes
        line_str = gen_line([[self.cfg_list['bButtonMask'],              1],   # 1  byte
                             [self.cfg_list['bValidButtons[23]'][ 0:15], 1]],  # 15 bytes
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line
        
        # 13 bytes
        line_str = gen_line([[self.cfg_list['bValidButtons[23]'][15:23], 1],   # 8 bytes
                             [self.cfg_list['bPushQualThresh'],          1],   # 1 byte
                             [self.cfg_list['wWaitForPush'],             2],   # 2 bytes
                             [self.cfg_list['wRepeatInterval'],          2]],  # 2 bytes
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line

        # 16 bytes
        line_str = gen_line([[self.cfg_list['bPacketBD[32]'][ 0:16],     1]],  # 16 bytes
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line
        
        # 16 bytes
        line_str = gen_line([[self.cfg_list['bPacketBD[32]'][16:32],     1]],  # 16 bytes
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line
        
        # 16 bytes
        line_str = gen_line([[self.cfg_list['bPacketTX[32]'][ 0:16],     1]],  # 16 bytes
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line

        # 16 bytes
        line_str = gen_line([[self.cfg_list['bPacketTX[32]'][16:32],     1]],  # 16 (1 byte X 16)
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line
        
        # 12 bytes
        line_str = gen_line([[self.cfg_list['bPacketsPerInterval'],      1],   # 1
                             [self.cfg_list['bPacketDelay'],             1],   # 1
                             [self.cfg_list['bMinButtonDuration'],       1],   # 1
                             [self.cfg_list['bAltMinButtonDuration'],    1],   # 1

                             [self.cfg_list['bMaxButtonDuration'],       1],   # 1
                             [self.cfg_list['bLEDOnTime'],               1],   # 1
                             [self.cfg_list['bLedIntensity'],            1],   # 1
                             [self.cfg_list['bBatThreshold'],            1],   # 1

                             [self.cfg_list['wBatNoTX'],                 2],   # 2
                             [self.cfg_list['iLowTempThreshold'],        2]],  # 2
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line

        # 15 bytes
        line_str = gen_line([[self.cfg_list['rAlternatePaSetup'][ 0: 2], 4],   # 8 (4 bytes  x 2)
                             [self.cfg_list['rAlternatePaSetup'][ 2: 4], 1],   # 2 (1 byte   x 2)
                             [self.cfg_list['rAlternatePaSetup'][ 3: 4], 2],   # 2
                             [self.cfg_list['bConfigSetting'],           1],   # 1
                             [self.cfg_list['bPTLedIntensity'],          1],   # 1
                             [self.cfg_list['bSleepSubSample'],          1]],  # 1
                             line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line

        # 13 bytes
        line_str = gen_line([[self.cfg_list['wSleepTimerDivide'],        2],   # 2
                             [self.cfg_list['lSleepCycleMask'],          4],   # 4
                             [self.cfg_list['wSleepCycleMask'],          2],   # 2

                             [self.cfg_list['bLoopIncrement'],           1],   # 1
                             [self.cfg_list['bPacketLength[4]'],         1]],  # 4 (1 byte X 4)
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line

        # 12 bytes
        line_str = gen_line([[self.cfg_list['bPreamble[4]'],             1],   # 4 (1 byte  X 4)
                             [self.cfg_list['wSyncWord[4]'],             2]],  # 8 (2 bytes X 4)
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line

        # 16 bytes
        line_str = gen_line([[self.cfg_list['fDesiredFreq[4]'],           4]],  # 16 (4 bytes X 4)
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line

        # 4 bytes
        line_str = gen_line([[self.cfg_list['bFskDev[4]'],                1]],   # 4 (1 bytes  X 4)
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line

        # 12 bytes
        line_str = gen_line([[self.cfg_list['rPaSetup[4]'][0][ 0: 2], 4],   # 8 (4 bytes  x 2)  
                             [self.cfg_list['rPaSetup[4]'][0][ 2: 4], 1],   # 2 (1 byte   x 2)  
                             [self.cfg_list['rPaSetup[4]'][0][ 3: 4], 2]],  # 2                 
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line

        # 12 bytes
        line_str = gen_line([[self.cfg_list['rPaSetup[4]'][1][ 0: 2], 4],   # 8 (4 bytes  x 2)  
                             [self.cfg_list['rPaSetup[4]'][1][ 2: 4], 1],   # 2 (1 byte   x 2)  
                             [self.cfg_list['rPaSetup[4]'][1][ 3: 4], 2]],  # 2                 
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line

        # 12 bytes
        line_str = gen_line([[self.cfg_list['rPaSetup[4]'][2][ 0: 2], 4],   # 8 (4 bytes  x 2)  
                             [self.cfg_list['rPaSetup[4]'][2][ 2: 4], 1],   # 2 (1 byte   x 2)  
                             [self.cfg_list['rPaSetup[4]'][2][ 3: 4], 2]],  # 2                 
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line

        # 12 bytes
        line_str = gen_line([[self.cfg_list['rPaSetup[4]'][3][ 0: 2], 4],   # 8 (4 bytes  x 2)  
                             [self.cfg_list['rPaSetup[4]'][3][ 2: 4], 1],   # 2 (1 byte   x 2)  
                             [self.cfg_list['rPaSetup[4]'][3][ 3: 4], 2]],  # 2                 
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line

        # 9 bytes
        print('rOdsSetup: ', self.cfg_list['rOdsSetup[4]'])
        line_str = gen_line([[self.cfg_list['rOdsSetup[4]'][0][ 0: 4], 1],   # 4 (1 byte  x 4)
                             [self.cfg_list['rOdsSetup[4]'][0][ 4: 5], 2],   # 2
                             [self.cfg_list['rOdsSetup[4]'][0][ 5: 8], 1]],  # 3 (1 byte  x 4)
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line
        # 9 bytes
        line_str = gen_line([[self.cfg_list['rOdsSetup[4]'][1][ 0: 4], 1],   # 4 (1 byte  x 4)
                             [self.cfg_list['rOdsSetup[4]'][1][ 4: 5], 2],   # 2
                             [self.cfg_list['rOdsSetup[4]'][1][ 5: 8], 1]],  # 3 (1 byte  x 4)
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line
        # 9 bytes
        line_str = gen_line([[self.cfg_list['rOdsSetup[4]'][2][ 0: 4], 1],   # 4 (1 byte  x 4)
                             [self.cfg_list['rOdsSetup[4]'][2][ 4: 5], 2],   # 2
                             [self.cfg_list['rOdsSetup[4]'][2][ 5: 8], 1]],  # 3 (1 byte  x 4)
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line
        # 9 bytes
        line_str = gen_line([[self.cfg_list['rOdsSetup[4]'][3][ 0: 4], 1],   # 4 (1 byte  x 4)
                             [self.cfg_list['rOdsSetup[4]'][3][ 4: 5], 2],   # 2
                             [self.cfg_list['rOdsSetup[4]'][3][ 5: 8], 1]],  # 3 (1 byte  x 4)
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line

        # 10 bytes
        line_str = gen_line([[self.cfg_list['bEncode[4]'],             1],   # 4 (1 byte  x 4)
                             [self.cfg_list['bUseExternalXo'],         1],   # 1
                             [self.cfg_list['rFCast_XoSetup'][0:1],    4],   # 4
                             [self.cfg_list['rFCast_XoSetup'][1:2],    1]],  # 1
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line

        # 16 bytes
        line_str = gen_line([[self.cfg_list['bOEM_Key[16]'],           1]],  # 16 (1 byte  x 16)
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line

        # 12 bytes
        line_str = gen_line([[self.cfg_list['lOEM_ID_Key'],            4],   # 4

                             [self.cfg_list['bUserData'],              1],   # 1
                             [self.cfg_list['bAdcHoldOff'],            1],   # 1
                             [self.cfg_list['iAdcOrTempThresholdH'],   2],   # 2

                             [self.cfg_list['iAdcOrTempThresholdL'],   2],   # 2
                             [self.cfg_list['bUserPayloadData0'],      1],   # 1
                             [self.cfg_list['bUserPayloadData1'],      1]],  # 1
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line
        
        # 16 bytes (0~15)
        line_str = gen_line([[self.cfg_list['bPacketOverRide[96]'][ 0: 16], 1]], # 16 (1 byte  x 16)
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line
        # 16 bytes (16~31)
        line_str = gen_line([[self.cfg_list['bPacketOverRide[96]'][16: 32], 1]], # 16 (1 byte  x 16)
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line
        # 16 bytes (32~47)
        line_str = gen_line([[self.cfg_list['bPacketOverRide[96]'][32: 48], 1]], # 16 (1 byte  x 16)
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line
        # 16 bytes (48~63)
        line_str = gen_line([[self.cfg_list['bPacketOverRide[96]'][48: 64], 1]], # 16 (1 byte  x 16)
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line
        # 16 bytes (64~79)
        line_str = gen_line([[self.cfg_list['bPacketOverRide[96]'][64: 80], 1]], # 16 (1 byte  x 16)
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line
        # 16 bytes (80~95)
        line_str = gen_line([[self.cfg_list['bPacketOverRide[96]'][80: 96], 1]], # 16 (1 byte  x 16)
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line

        # 16 bytes (0~15)
        line_str = gen_line([[self.cfg_list['bHoltekGpioMap[23]'][ 0:16], 1]],  # 16 (1 byte  x 16)
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line

        # 11 bytes
        line_str = gen_line([[self.cfg_list['bHoltekGpioMap[23]'][ 16:22], 1],  # 6 (1 byte  x 6)
                             [self.cfg_list['bHoltekDataMask'],            1],  # 1
                             [self.cfg_list['bReserve0'],                  1],  # 1

                             [self.cfg_list['bReserve1'],                  1],  # 1
                             [self.cfg_list['bReserve2'],                  1],  # 1
                             [self.cfg_list['bReserve3'],                  1]], # 1
                            line_address)
        line_address = pack_line(line_str, H, line_address)  # return value is addr for data in next line
        
        return H


    def gen_TX_C_File(self):
        C  = []
        C.append('/*------------------------------------------------------------------------------')
        C.append(' *                          Silicon Laboratories, Inc.')
        C.append(' *                               Copyright 2013')
        C.append(' *                          CONFIDENTIAL & PROPRIETARY')
        C.append(' *------------------------------------------------------------------------------')
        C.append(' *')
        C.append(' *    Author:            Silabs')
        C.append(' *    Date:              {}'.format(time.strftime('%m/%d/%Y %H:%M', time.localtime(time.time()))))
        C.append(' *    File Name (TX):    oem_config4_demo_data.c')
        C.append(' *    Created from       EZRay Configuration Calculator')
        C.append(' *')
        C.append(' *------------------------------------------------------------------------------')
        C.append(' *')
        C.append(' *    INCLUDES:')
        C.append(' */')
        C.append('#include \"oem_config.h\"')
        C.append(' ')
        C.append('tConfigData const code rOEMParamCode')
        C.append('= {')
        C.append('    //     BYTE    bButtonMask')
        C.append('    0x{}    ,   //  OR of used buttons'.format(format(self.cfg_list['bButtonMask'])))
        C.append('    //     bValidButtons[23]')
        C.append('    {')
        C.append('        ' + str(self.cfg_list['bValidButtons[23]']).strip('[]'))
        C.append('    },')

        C.append('    //     BYTE    bPushQualThresh        See AN370 pg 37, For Push Tracking Structure')
        C.append('    {}    , //    '.format(self.cfg_list['bPushQualThresh']))

        C.append('    //     WORD     wWaitForPush        See AN370 pg 37, For Push Tracking Structure')
        C.append('    ' + str(self.cfg_list['wWaitForPush']) +  ', //    Wait Time for Button Press before shutting down (ms)')

        C.append('    //     WORD     wRepeatInterval')
        C.append('    ' + str(self.cfg_list['wRepeatInterval']) + ',  //    ms')

        C.append('    //     BYTE    bPacketBD[32]')
        C.append('    {   ' + str(self.cfg_list['bPacketBD[32]'][ 0 :  4]).strip('[]') + ',')
        C.append('        ' + str(self.cfg_list['bPacketBD[32]'][ 4 :  8]).strip('[]') + ',')
        C.append('        ' + str(self.cfg_list['bPacketBD[32]'][ 8 : 12]).strip('[]') + ',')
        C.append('        ' + str(self.cfg_list['bPacketBD[32]'][12 : 16]).strip('[]') + ',')
        C.append('        ' + str(self.cfg_list['bPacketBD[32]'][16 : 20]).strip('[]') + ',')
        C.append('        ' + str(self.cfg_list['bPacketBD[32]'][20 : 24]).strip('[]') + ',')
        C.append('        ' + str(self.cfg_list['bPacketBD[32]'][24 : 28]).strip('[]') + ',')
        C.append('        ' + str(self.cfg_list['bPacketBD[32]'][28 : 32]).strip('[]'))
        C.append('    },')

        C.append('    //     BYTE    bPacketTX[32]')
        C.append('    {   ' + str(self.cfg_list['bPacketTX[32]'][ 0 :  4]).strip('[]') + ',')
        C.append('        ' + str(self.cfg_list['bPacketTX[32]'][ 4 :  8]).strip('[]') + ',')
        C.append('        ' + str(self.cfg_list['bPacketTX[32]'][ 8 : 12]).strip('[]') + ',')
        C.append('        ' + str(self.cfg_list['bPacketTX[32]'][12 : 16]).strip('[]') + ',')
        C.append('        ' + str(self.cfg_list['bPacketTX[32]'][16 : 20]).strip('[]') + ',')
        C.append('        ' + str(self.cfg_list['bPacketTX[32]'][20 : 24]).strip('[]') + ',')
        C.append('        ' + str(self.cfg_list['bPacketTX[32]'][24 : 28]).strip('[]') + ',')
        C.append('        ' + str(self.cfg_list['bPacketTX[32]'][28 : 32]).strip('[]'))
        C.append('    },')

        C.append('    //     BYTE    bPacketsPerInterval')
        C.append('    ' + str(self.cfg_list['bPacketsPerInterval']) + ',')

        C.append('    //     BYTE     bPacketDelay')
        C.append('    ' + str(self.cfg_list['bPacketDelay']) + ',')

        C.append('    //     BYTE    bMinButtonDuration')
        C.append('    ' + str(self.cfg_list['bMinButtonDuration']) + ',')

        C.append('    //     BYTE    bAltMinButtonDuration')
        C.append('    ' + str(self.cfg_list['bAltMinButtonDuration']) + ',')

        C.append('    //     BYTE      bMaxButtonDuration')
        C.append('    ' + str(self.cfg_list['bMaxButtonDuration']) + ',')

        C.append('    //     BYTE     bLEDOnTime')
        C.append('    ' + str(self.cfg_list['bLEDOnTime']) + ',')

        C.append('    //     BYTE     bLedIntensity')
        C.append('    ' + str(self.cfg_list['bLedIntensity']) + ',')

        C.append('    //     BYTE     bBatThreshold')
        C.append('    ' + str(self.cfg_list['bBatThreshold']) + ',')

        C.append('    //     WORD    wBatNoTX')
        C.append('    ' + str(self.cfg_list['wBatNoTX']) + ',')

        C.append('    //     int     iLowTempThreshold')
        C.append('    ' + str(self.cfg_list['iLowTempThreshold']) + ',')

        C.append('    //    tPa_Setup rAlternatePaSetup')
        C.append('    //    float         float        BYTE          BYTE           WORD')
        C.append('    //    fAlpha        fBeta        bLevel        bMaxDrv        wNominalCap')
        C.append('    {   ' + str(self.cfg_list['rAlternatePaSetup']).strip('[]') + ' },  //    ')

        C.append('    /*     BYTE     bConfigSetting')
        C.append('     *     B[0]  -  Alternate PA Setting  Periodic Transmission  :  ')
        C.append('     *     B[1]  -  Periodic Transmit                            :  ')
        C.append('     *     B[2]  -  Reserved to 0                                :  ')
        C.append('     *     B[3]  -  Multiple OTP configuration                   :  ')
        C.append('     *     B[4]  -  Stuck button                                 :  ')
        C.append('     *     B[5]  -  Drive to low during ADC sample               :  ')
        C.append('     *     B[6]  -  Thermostat                                   :  ')
        C.append('     *     B[7]  -  Test mode                                    :*/')
        # Not for reading, it's for parser, don't need biany format
        #C.append('0b'+format(self.cfg_list['bConfigSetting'], 'b') + ',')
        C.append('    '+str(self.cfg_list['bConfigSetting']) + ',')

        C.append('    //     BYTE     bPTLedIntensity')
        C.append('    ' + str(self.cfg_list['bPTLedIntensity']) + ',')

        C.append('    //     BYTE     bSleepSubSample')
        C.append('    ' + str(self.cfg_list['bSleepSubSample']) + ',')

        C.append('    //     WORD    wSleepTimerDivide')
        C.append('    ' + str(self.cfg_list['wSleepTimerDivide']) + ',')

        C.append('    //     LWORD    lSleepCycleMask')
        C.append('    ' + str(self.cfg_list['lSleepCycleMask']) + ',')

        C.append('    //     WORD    wSleepCycleMask')
        C.append('    ' + str(self.cfg_list['wSleepCycleMask']) + ',')

        C.append('    //     BYTE    bLoopIncrement')
        C.append('    ' + str(self.cfg_list['bLoopIncrement']) + ',')

        C.append('    //    BYTE bPacketLength[4]')
        C.append('    {')
        C.append('        ' + str(self.cfg_list['bPacketLength[4]']).strip('[]'))
        C.append('    },')

        C.append('    //    BYTE bPreamble[4];')
        C.append('    {')
        C.append('        ' + str(self.cfg_list['bPreamble[4]']).strip('[]'))
        C.append('    },')

        C.append('    //    WORD wSyncWord[4];')
        C.append('    {')
        C.append('        ' + str(self.cfg_list['wSyncWord[4]']).strip('[]'))
        C.append('    },')


        C.append('    //    float fDesiredFreq[4];')
        C.append('    {')
        C.append('        ' + str(self.cfg_list['fDesiredFreq[4]']).strip('[]'))
        C.append('    },')

        C.append('    //    BYTE bFskDev[4];')
        C.append('    {')
        C.append('        ' + str(self.cfg_list['bFskDev[4]']).strip('[]'))
        C.append('    },')

        C.append('    //    tPa_Setup rPaSetup[4];')
        C.append('    {')
        C.append('        //    float         float        BYTE          BYTE           WORD')
        C.append('        //    fAlpha        fBeta        bLevel        bMaxDrv        wNominalCap')
        C.append('        {   ' + str(self.cfg_list['rPaSetup[4]'][0]).strip('[]') + ' },  //    ')
        C.append('        {   ' + str(self.cfg_list['rPaSetup[4]'][1]).strip('[]') + ' },  //    ')
        C.append('        {   ' + str(self.cfg_list['rPaSetup[4]'][2]).strip('[]') + ' },  //    ')
        C.append('        {   ' + str(self.cfg_list['rPaSetup[4]'][3]).strip('[]') + ' },  //    ')
        C.append('    },')

        C.append('    // tOds_Setup rOdsSetup[4];')
        C.append('    {')
        C.append('        //    BYTE                   BYTE           BYTE             BYTE               WORD            BYTE              BYTE               BYTE')
        C.append('        //    bModulationType        bClkDiv        bEdgeRate        bGroupWidth        wBitRate        bLcWarmInt        bDivWarmInt        bPaWarmInt')
        C.append('        {   ' + str(self.cfg_list['rOdsSetup[4]'][0]).strip('[]') + ' },  //    ')
        C.append('        {   ' + str(self.cfg_list['rOdsSetup[4]'][1]).strip('[]') + ' },  //    ')
        C.append('        {   ' + str(self.cfg_list['rOdsSetup[4]'][2]).strip('[]') + ' },  //    ')
        C.append('        {   ' + str(self.cfg_list['rOdsSetup[4]'][3]).strip('[]') + ' },  //    ')
        C.append('    },')

        C.append('    //     BYTE  bEncode[4];')
        C.append('    {')
        C.append('        ' + str(self.cfg_list['bEncode[4]']).strip('[]'))
        C.append('    },')

        C.append('    //     BYTE    bUseExternalXo')
        C.append('    ' + str(self.cfg_list['bUseExternalXo']) + ',   //    Internally Generated Reference Clock')

        C.append('    // tFCast_XoSetup rFCast_XoSetup;')
        C.append('    {')
        C.append('        //    float        BYTE')
        C.append('        //    fXoFreq      bLowCap')
        C.append('        ' + str(self.cfg_list['rFCast_XoSetup']).strip('[]') + '//    Internally Generated Reference Clock')
        C.append('    },')

        C.append('    //     BYTE    bOEM_Key[16]')
        C.append('    {')
        C.append('        ' + str(self.cfg_list['bOEM_Key[16]']).strip('[]'))
        C.append('    },')
        C.append('    //     LWORD     lOEM_ID_Key')
        C.append('    ' + format(self.cfg_list['lOEM_ID_Key'], 'X') + ', //')

        C.append('    //     BYTE    bUserData')
        C.append('    ' + str(self.cfg_list['bUserData']) + ', //    ')

        C.append('    //     BYTE    bAdcHoldOff')
        C.append('    ' + str(self.cfg_list['bAdcHoldOff']) + ', //    ')

        C.append('    //     int    iAdcOrTempThresholdH')
        C.append('    ' + str(self.cfg_list['iAdcOrTempThresholdH']) + ', //    ')

        C.append('    //     BYTE    iAdcOrTempThresholdL')
        C.append('    ' + str(self.cfg_list['iAdcOrTempThresholdL']) + ', //    ')

        C.append('    //     BYTE    bUserPayloadData0')
        C.append('    ' + str(self.cfg_list['bUserPayloadData0']) + ', //    ')

        C.append('    //     BYTE    bUserPayloadData1')
        C.append('    ' + str(self.cfg_list['bUserPayloadData1']) + ', //    ')

        C.append('    // BYTE bPacketOverRide[96];')
        C.append('    {')
        C.append('        ' + str(self.cfg_list['bPacketOverRide[96]'][ 0 :  6]).strip('[]') + ',  // 0 - 5   ')
        C.append('        ' + str(self.cfg_list['bPacketOverRide[96]'][ 6 : 12]).strip('[]') + ',  // 6 - 11  ')
        C.append('        ' + str(self.cfg_list['bPacketOverRide[96]'][12 : 18]).strip('[]') + ',  // 12 - 17 ')
        C.append('        ' + str(self.cfg_list['bPacketOverRide[96]'][18 : 24]).strip('[]') + ',  // 18 - 23 ')
        C.append('        ' + str(self.cfg_list['bPacketOverRide[96]'][24 : 30]).strip('[]') + ',  // 24 - 29 ')
        C.append('        ' + str(self.cfg_list['bPacketOverRide[96]'][30 : 36]).strip('[]') + ',  // 30 - 35 ')
        C.append('        ' + str(self.cfg_list['bPacketOverRide[96]'][36 : 42]).strip('[]') + ',  // 36 - 41 ')
        C.append('        ' + str(self.cfg_list['bPacketOverRide[96]'][42 : 48]).strip('[]') + ',  // 42 - 47 ')
        C.append('        ' + str(self.cfg_list['bPacketOverRide[96]'][48 : 54]).strip('[]') + ',  // 48 - 53 ')
        C.append('        ' + str(self.cfg_list['bPacketOverRide[96]'][54 : 60]).strip('[]') + ',  // 54 - 59 ')
        C.append('        ' + str(self.cfg_list['bPacketOverRide[96]'][60 : 66]).strip('[]') + ',  // 60 - 65 ')
        C.append('        ' + str(self.cfg_list['bPacketOverRide[96]'][66 : 72]).strip('[]') + ',  // 66 - 71 ')
        C.append('        ' + str(self.cfg_list['bPacketOverRide[96]'][72 : 78]).strip('[]') + ',  // 72 - 77 ')
        C.append('        ' + str(self.cfg_list['bPacketOverRide[96]'][78 : 84]).strip('[]') + ',  // 78 - 83 ')
        C.append('        ' + str(self.cfg_list['bPacketOverRide[96]'][84 : 90]).strip('[]') + ',  // 84 - 89 ')
        C.append('        ' + str(self.cfg_list['bPacketOverRide[96]'][90 : 95]).strip('[]') + '   // 90 - 95 ')
        C.append('    },')

        C.append('    //     BYTE     bHoltekGpioMap[23]')
        C.append('    {  //  ')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][ 0], 'X') + ', //    bHoltekGpioMap[ 0]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][ 1], 'X') + ', //    bHoltekGpioMap[ 1]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][ 2], 'X') + ', //    bHoltekGpioMap[ 2]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][ 3], 'X') + ', //    bHoltekGpioMap[ 3]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][ 4], 'X') + ', //    bHoltekGpioMap[ 4]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][ 5], 'X') + ', //    bHoltekGpioMap[ 5]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][ 6], 'X') + ', //    bHoltekGpioMap[ 6]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][ 7], 'X') + ', //    bHoltekGpioMap[ 7]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][ 8], 'X') + ', //    bHoltekGpioMap[ 8]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][ 9], 'X') + ', //    bHoltekGpioMap[ 9]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][10], 'X') + ', //    bHoltekGpioMap[10]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][11], 'X') + ', //    bHoltekGpioMap[11]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][12], 'X') + ', //    bHoltekGpioMap[12]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][13], 'X') + ', //    bHoltekGpioMap[13]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][14], 'X') + ', //    bHoltekGpioMap[14]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][15], 'X') + ', //    bHoltekGpioMap[15]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][16], 'X') + ', //    bHoltekGpioMap[16]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][17], 'X') + ', //    bHoltekGpioMap[17]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][18], 'X') + ', //    bHoltekGpioMap[18]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][19], 'X') + ', //    bHoltekGpioMap[19]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][20], 'X') + ', //    bHoltekGpioMap[20]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][21], 'X') + ', //    bHoltekGpioMap[21]')
        C.append('        0x' + format(self.cfg_list['bHoltekGpioMap[23]'][22], 'X') + ', //    bHoltekGpioMap[22]')
        C.append('    },')

        C.append('    //     BYTE     bHoltekDataMask')
        C.append('    ' + str(self.cfg_list['bHoltekDataMask']) + ', //    ')

        C.append('    // BYTE bReserve0;')
        C.append('    0x' + format(self.cfg_list['bReserve0'], 'X') + ',')

        C.append('    // BYTE bReserve0;')
        C.append('    0x' + format(self.cfg_list['bReserve0'], 'X') + ',')

        C.append('    // BYTE bReserve0;')
        C.append('    0x' + format(self.cfg_list['bReserve0'], 'X') + ',')

        C.append('    // BYTE bReserve0;')
        C.append('    0x' + format(self.cfg_list['bReserve0'], 'X') + ',')
        C.append('    };')
        return C


    def gen_PQ_C_File(self):
        PQ = []
        PQ.append('/*------------------------------------------------------------------------------')
        PQ.append(' *                          Silicon Laboratories, Inc.')
        PQ.append(' *                               Copyright 2013')
        PQ.append(' *                          CONFIDENTIAL & PROPRIETARY')
        PQ.append(' *------------------------------------------------------------------------------')
        PQ.append(' *')
        PQ.append(' *      Author:            Silabs')
        PQ.append(' *      Date:              {}'.format(time.strftime('%m/%d/%Y %H:%M', time.localtime(time.time()))))
        PQ.append(' *      File Name (PQ):    QualifyPacketConfigData4.c')
        PQ.append(' *      Created from       EZRay Configuration Calculator')
        PQ.append(' *------------------------------------------------------------------------------')
        PQ.append(' *')
        PQ.append(' *      Description (PQ):  Configuration 4 data for Packet Qualification')
        PQ.append(' *')
        PQ.append(' *------------------------------------------------------------------------------')
        PQ.append(' *')
        PQ.append(' *    INCLUDES:')
        PQ.append(' */')
        PQ.append('{')
        PQ.append('    //     LWORD    lPrimaryWindow')
        PQ.append('    ' + str(self.cfg_PQ_list['lPrimaryWindow']) + ', //    PrimaryWindow (1 Week)')
        PQ.append('    //     LWORD    lResyncLimit')
        PQ.append('    ' + str(self.cfg_PQ_list['lResyncLimit']) + ', //  Resync Limit (13 Weeks)')
        PQ.append('    //     WORD    wResyncWindow')
        PQ.append('    ' + str(self.cfg_PQ_list['wResyncWindow']) + ', //  wResyncWindow (1.2 *Sleep Timer)')

        PQ.append('    //     BYTE    bValidButtons[23]')
        PQ.append('    {')
        PQ.append('        ' + str(self.cfg_list['bValidButtons[23]']).strip('[]'))
        PQ.append('    },')
        PQ.append('    //     BYTE    bPacketBD[32]            Rule: If value is zero then use previous non zero value')
        PQ.append('    {')
        PQ.append('        ' + str(self.cfg_list['bPacketBD[32]'][ 0 :  4]).strip('[]') + ',')
        PQ.append('        ' + str(self.cfg_list['bPacketBD[32]'][ 4 :  8]).strip('[]') + ',')
        PQ.append('        ' + str(self.cfg_list['bPacketBD[32]'][ 8 : 12]).strip('[]') + ',')
        PQ.append('        ' + str(self.cfg_list['bPacketBD[32]'][12 : 16]).strip('[]') + ',')
        PQ.append('        ' + str(self.cfg_list['bPacketBD[32]'][16 : 20]).strip('[]') + ',')
        PQ.append('        ' + str(self.cfg_list['bPacketBD[32]'][20 : 24]).strip('[]') + ',')
        PQ.append('        ' + str(self.cfg_list['bPacketBD[32]'][24 : 28]).strip('[]') + ',')
        PQ.append('        ' + str(self.cfg_list['bPacketBD[32]'][28 : 32]).strip('[]'))
        PQ.append('    },')
        PQ.append('    //     BYTE    bPacketTX[32]            Rule: If value is zero then use previous non zero value')
        PQ.append('    {')
        PQ.append('        ' + str(self.cfg_list['bPacketTX[32]'][ 0 :  4]).strip('[]') + ',')
        PQ.append('        ' + str(self.cfg_list['bPacketTX[32]'][ 4 :  8]).strip('[]') + ',')
        PQ.append('        ' + str(self.cfg_list['bPacketTX[32]'][ 8 : 12]).strip('[]') + ',')
        PQ.append('        ' + str(self.cfg_list['bPacketTX[32]'][12 : 16]).strip('[]') + ',')
        PQ.append('        ' + str(self.cfg_list['bPacketTX[32]'][16 : 20]).strip('[]') + ',')
        PQ.append('        ' + str(self.cfg_list['bPacketTX[32]'][20 : 24]).strip('[]') + ',')
        PQ.append('        ' + str(self.cfg_list['bPacketTX[32]'][24 : 28]).strip('[]') + ',')
        PQ.append('        ' + str(self.cfg_list['bPacketTX[32]'][28 : 32]).strip('[]'))
        PQ.append('    },')

        PQ.append('    //     BYTE    bOEM_Key[16]')
        PQ.append('    {')
        PQ.append('        ' + str(self.cfg_list['bOEM_Key[16]']).strip('[]'))
        PQ.append('    }')
        PQ.append('}')

        return PQ

    def get_cfg_list(self):
        return self.cfg_list

    def get_cfg_PQ_list(self):
        return self.cfg_PQ_list


    def dump(self):
        self.top.dump()
        self.timer.dump()
        self.Buttons.dump()
        self.PacketSetup.dump()
        self.MiscSetup.dump()
        self.FixedPacket.dump()
        self.Security.dump()
        self.HoltekEmu.dump()
        self.Packet0.dump()
        self.Packet1.dump()
        self.Packet2.dump()
        self.Packet3.dump()

    def get_options(self, category):
        options = super(Si4010CfgCalc, self).get_options(category)
        if category in ["Packet0", "Packet1", "Packet2","Packet3"]:
            del options[category+".modem"]
            for k, v in self.get_option(category+".modem").get_options().items():
                options[category+"."+"modem"+"."+k] = v
        return options 
    
    def set_option(self, option, value):
        if len(option.split(".")) == 2:
            super(Si4010CfgCalc, self).set_option(option, value)
        else:
            [cat, modem, opt] = option.split(".")
            self.get_option(cat+"."+modem).set_option(opt, value)
        return value
            
    def configure(self):
        self.calculate()
        file_lines = self.gen_TX_C_File()
        return {"tx_c_file": "\n".join(file_lines)}
        