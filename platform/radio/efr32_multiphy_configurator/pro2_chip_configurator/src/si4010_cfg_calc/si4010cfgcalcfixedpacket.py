'''
Created on Sept 07, 2013

@author: shyang
'''

__all__ = ["Si4010CfgCalcFixedPacket"]

class Si4010CfgCalcFixedPacket(object):
    '''
    classdocs
    '''

    def __init__(self, inputs):
        '''
        Constructor
        '''
        self.cfg = {}
        self.cfg_PQ_file = {}

        self.calc_packet(inputs)
        self.calc_PacketOveRide(inputs)

    def calc_packet(self, inputs):
        self.TotalButtons   = inputs.fixedPacket.FixedPacketButtons
        if not self.TotalButtons in [1, 2, 4, 8]:
            self.TotalButtons = 1
            inputs.fixedPacket.FixedPacketButtons = 1
            
        self.PacketLen = inputs.fixedPacket.FixedPacketLength
        if not self.PacketLen in range(4, 96//self.TotalButtons + 1):
            self.PacketLen = 4
            inputs.fixedPacket.FixedPacketLength = 4

        #TODO add something to show which buttons and data will be used


    def calc_PacketOveRide(self, inputs):
        self.PacketOverRide = inputs.fixedPacket.PacketOverRide
        self.cfg['bPacketOverRide[96]'] = inputs.fixedPacket.PacketOverRide
        # TODO restriction
        #if type is Security, then some fields are fixed :


    def get_cfg_data(self):
        return self.cfg


    def dump(self):
        print('    -------------  configuration data for TX C file  -------------')
        for m in self.cfg:
            print('        {}  =  {}'.format(m, self.cfg[m]))

        print('    -------------  configuration data for PQ C file  -------------')
        for m in self.cfg_PQ_file:
            print('        {}  =  {}'.format(m, self.cfg_PQ_file[m]))


