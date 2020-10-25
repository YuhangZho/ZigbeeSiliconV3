'''
Created on August 25, 2013

@author: shyang
'''
import math
import copy
import si4010_modem_calc

__all__ = ["Si4010CfgCalcPacket"]

class Si4010CfgCalcPacket(object):
    '''
    classdocs
    '''
    index_ofs_Encode     = 0
    index_ofs_DataLenth  = 1
    index_ofs_PreMin     = 2
    index_ofs_PreMax     = 3
    index_ofs_Preamble   = 4
    index_ofs_SyncWord   = 5
    index_ofs_GroupWidth = 6
    #                  Type                      Encode,	DataLength,		PreMin,		PreMax,		Preamble,		SyncWord,		GroupWidth
    DataFormatTable = {'Security'          :    [3, 		25, 			5,			71,			0xAA,			0x2DD4,			0],
                       'Simple'            :    [0x23, 		13, 			5,			83,			0xAA,			0x2DD4,			0],
                       'Fixed Packet'      :    [0x13, 		12, 			0,			 0,			0x00,			0x0000,			5],
                       'Holtek Emulation'  :   	[0x40, 		96, 			0,			 0,			0x00,			0x0000,			5],
                       }

    def __init__(self, packet_inputs, fixed_packet_len):
        self.cfg = {}
        self.cfg_PQ_file = {}
        # Must calc data format first
        self.calc_bEncode(packet_inputs)        

        # --------  Preamble/Sync word  --------
        self.calc_PreambleSynWord(packet_inputs, fixed_packet_len)

        # --------  Packet  --------
        self.calc_PacketLength(packet_inputs)

        # --------  Modem and PA settings  --------
        self.api_settings = self.calc_modem_PA(packet_inputs.modem)
        self.calc_modem_cfg_values()
        bit_rate = packet_inputs.modem.API_SS_Bit_Data_Rate
        #if self.api_settings['bGroupWidth'] == 5:   # No 5 in the modem calc TODO
        if packet_inputs.DataFormat == 'Holtek Emulation':
            bit_rate = bit_rate*8/(5 + 1)

        self.PacketTransTime = packet_inputs.PacketLength*8/bit_rate

        # Repeat Interval
        # TODO use the pre defined bGroupWidth?


    def calc_PreambleSynWord(self, packet_inputs, fixed_packet_len):
        DataFormat = packet_inputs.DataFormat
        
        if DataFormat == 'Fixed Packet':  # Override the packet length if necessary
            self.DataFormatTable[DataFormat][self.index_ofs_DataLenth] = fixed_packet_len

        # Preamble and Sync word
        if DataFormat == 'Security' or DataFormat == 'Simple' or DataFormat == 'Holtek Emulation':
            self.DataFormatTable[DataFormat][self.index_ofs_Preamble] = packet_inputs.Preamble
            self.DataFormatTable[DataFormat][self.index_ofs_SyncWord] = packet_inputs.SyncWord

        self.cfg['bPreamble[4]'] = self.DataFormatTable[DataFormat][self.index_ofs_Preamble]
        self.cfg['wSyncWord[4]'] = self.DataFormatTable[DataFormat][self.index_ofs_SyncWord]

        # GroupWidth
        self.cfg['bGroupWidth[4]'] = self.DataFormatTable[DataFormat][self.index_ofs_GroupWidth]
        
    def calc_PacketLength(self, packet_inputs):
        DataFormat = packet_inputs.DataFormat
        # Preamble length for fixed or security
        pream_len = packet_inputs.PremLenForFixedOrSecu
        min_len   = self.DataFormatTable[DataFormat][self.index_ofs_PreMin]
        max_len   = self.DataFormatTable[DataFormat][self.index_ofs_PreMax]
        if pream_len < min_len:
            packet_inputs.PremLenForFixedOrSecu = min_len
        elif pream_len > max_len:
            packet_inputs.PremLenForFixedOrSecu = max_len

        self.cfg['bPacketLength[4]'] = packet_inputs.PremLenForFixedOrSecu + 12

    def calc_bEncode(self, packet_inputs):
        DataFormat = packet_inputs.DataFormat
        if not DataFormat in self.DataFormatTable.keys():
            DataFormat = 'Simple'        # Reset to 'Simple' type
            packet_inputs.DataFormat = DataFormat    # Update the input parameter
        self.bEncode = self.DataFormatTable[DataFormat][self.index_ofs_Encode]
        self.cfg['bEncode[4]'] = self.bEncode


    def calc_modem_PA(self, modem_paras):
        inputs = copy.deepcopy(modem_paras)
        modem  = si4010_modem_calc.Si4010ApiCalc(inputs)
        modem.calculate()
        # TODO check bGroupWidth

        return modem.get_api_list() # api_settings

    def calc_modem_cfg_values(self):
        self.cfg['fDesiredFreq[4]'] = self.api_settings['fiDesiredFreq']
        self.cfg['bFskDev[4]']      = self.api_settings['biFskDev']
        self.cfg['rPaSetup[4]']     = [self.api_settings['fAlpha'], self.api_settings['fBeta'], self.api_settings['bLevel'], self.api_settings['bMaxDrv'], self.api_settings['wNominalCap']]
        self.cfg['rOdsSetup[4]']    = [self.api_settings['bModulationType'], self.api_settings['bClkDiv'], self.api_settings['bEdgeRate'], self.api_settings['bGroupWidth'], self.api_settings['wBitRate'], self.api_settings['bLcWarmInt'], self.api_settings['bDivWarmInt'], self.api_settings['bPaWarmInt']]
        


    def get_cfg_data(self):
        return self.cfg
            

    def dump(self):
        print('    -------------  configuration data for TX C file  -------------')
        for m in self.cfg:
            if m == 'wSyncWord' or m == 'bPreamble':
                value = '0x' + format(self.cfg[m], 'X')
            else:
                value = self.cfg[m]
            print('        {}  =  {}'.format(m, value))













































