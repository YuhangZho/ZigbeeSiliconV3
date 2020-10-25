'''
Created on Sept 01, 2013

@author: shyang
'''
from common import InputInterface 
from .default_inputs import DefaultInputs
from .base_inputs import Buttons, FixedPacket, Holtek, Misc, Packet, PacketSetup, Security, Timer, TopLevelSetup 

__all__ = ["Si4010CfgCalcInputs"]

class Si4010CfgCalcInputs(InputInterface):
    '''
    classdocs
    '''
    def _set(self, input_data):
        ## read in API params
        try:
            # -------------  Global features  -------------
            # -------------  Sleep Timer and Sample Rate  -------------
            self.timer = Timer(input_data[0])
            # --------  Top Level Setup   --------
            self.topLevelSetup = TopLevelSetup(input_data[1])
            # -------------  Buttons  -------------
            self.buttons = Buttons(input_data[2])
            # -------------  Packet(0/1/2/3)  -------------
            self.Packet0 = Packet(input_data[3])
            self.Packet1 = Packet(input_data[4])
            self.Packet2 = Packet(input_data[5])
            self.Packet3 = Packet(input_data[6])
            # --------  Packet Setup  --------
            self.packetSetup = PacketSetup(input_data[7])
            # --------  Fixed Packet  --------
            self.fixedPacket = FixedPacket(input_data[8])
            # --------  Misc Setup  --------
            self.misc = Misc(input_data[9])
            # Security
            self.security = Security(input_data[10])
            # Holtek emulation
            self.holtek = Holtek(input_data[11])
        except:
            raise RuntimeError('Invalid format!')
        self.check_inputs()

    def __init__(self, inputparams=None):
        '''
        Constructor
        '''
        if inputparams == None:
            inputarray = DefaultInputs.inputs
        else:
            inputarray = inputparams
        self._set(inputarray)

    def set(self, input_data):
        self._set(input_data)
        self.check_inputs()
    
    def check_inputs(self):
        self.timer.validate()
        self.topLevelSetup.validate()
        self.buttons.validate()
        self.Packet0.validate()
        self.Packet1.validate()
        self.Packet2.validate()
        self.Packet3.validate()
        self.packetSetup.validate()
        self.fixedPacket.validate()
        self.misc.validate()
        self.security.validate()
        self.holtek.validate()
        
    def dump(self):
        print("Input Parameters:                        ")
        self.topLevelSetup.dump()
        self.timer.dump()
        self.buttons.dump()
        self.Packet0.dump()
        self.Packet1.dump()
        self.Packet2.dump()
        self.Packet3.dump()
        self.packetSetup.dump()
        self.misc.dump()
        self.holtek.dump()
        self.security.dump()
        self.fixedPacket.dump()