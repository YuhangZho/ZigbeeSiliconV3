'''
Created on Oct 23, 2013

@author: sesuskic
'''

from .sipro2 import SiPro2
from .inputs import Pro2PlusInputs
from si4440_modem_calc import Pro2PlusModemCalc, Pro2PlusLegacyModemCalc

__all__ = ["SiPro2Plus", "SiPro2PlusRevC01"]

class SiPro2Plus(SiPro2):
    def __init__(self, radioApi):
        super(SiPro2Plus, self).__init__(radioApi)

    def _create_inputs(self, radioApi):
        self.inputs = Pro2PlusInputs(radioApi)

    def _create_modem_calculator(self, modem_inputs):
        return Pro2PlusModemCalc(modem_inputs)
    
class SiPro2PlusRevC01(SiPro2Plus):
    def __init__(self, radioApi):
        super(SiPro2PlusRevC01, self).__init__(radioApi)

    def _create_modem_calculator(self, modem_inputs):
        return Pro2PlusLegacyModemCalc(modem_inputs)
    
