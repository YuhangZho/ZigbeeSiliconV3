'''
Created on August 25, 2013

@author: shyang
'''

__all__ = ["Si4010CfgCalcHoltekEmulation"]

class Si4010CfgCalcHoltekEmulation(object):
    '''
    classdocs
    '''
    AddrDataBit = [
        [0, 0xFF, '0 Data Bits, and 24 Address Bits'],
        [1, 0x7F, '1 Data Bits, and 23 Address Bits'],
        [2, 0x3F, '2 Data Bits, and 22 Address Bits'],
        [3, 0x1F, '3 Data Bits, and 21 Address Bits'],
        [4, 0x0F, '4 Data Bits, and 20 Address Bits'],
        [5, 0x07, '5 Data Bits, and 19 Address Bits'],
        [6, 0x03, '6 Data Bits, and 18 Address Bits'],
        [7, 0x01, '7 Data Bits, and 17 Address Bits'],
        ]

    BrazilMapping   = {'1'  : 0x00,
                       '4'  : 0x40,
                       '8'  : 0xC0,
                       '16' : 0x80 }

    def __init__(self, inputs):
        self.cfg = {}
        self.cfg_PQ_file = {}
        self.DataBits = inputs.holtek.HoltekEmuDataBits
        self.cfg['bHoltekDataMask'] = self.AddrDataBit[self.DataBits][1]
        self.MappingType = inputs.holtek.HoltekEmuMappingType
        self.HoltekEmuCustomGpioMappingTable = inputs.holtek.HoltekEmuCustomGpioMappingTable
        self.calc_GpioMap(inputs)


    def calc_GpioMap(self, inputs):
        if self.MappingType == 'Custom':
            self.GpioMapping = inputs.holtek.HoltekEmuCustomGpioMappingTable
        elif self.MappingType == 'Direct':
            self.GpioMapping = inputs.buttons.ValidButtons
        else:  # Brazil
            self.GpioMapping = [0]*len(inputs.buttons.ValidButtons)
            for i in range(0, len(inputs.buttons.ValidButtons)):
                button = str(inputs.buttons.ValidButtons[i])
                if button in self.BrazilMapping.keys():
                    self.GpioMapping[i] = self.BrazilMapping[button]
                else:
                    self.GpioMapping[i] = 0xFF


        self.cfg['bHoltekGpioMap[23]'] = self.GpioMapping




    #

    def get_cfg_data(self):
        return self.cfg


    def dump(self):
        print('    -------------  configuration data for TX C file  -------------')
        for m in self.cfg:
            print('        {}  =  {}'.format(m, self.cfg[m]))











































