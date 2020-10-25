'''
Created on Aug 29, 2014

@author: sesuskic
'''

from common import CategoryInterface
from .propertygroupbasedoptions import PropertyGroupBasedOptions

__all__ = ["AutoRxFreqHop"]

class AutoRxFreqHopProps(PropertyGroupBasedOptions):
    def __init__(self, radioApi):
        super(AutoRxFreqHopProps, self).__init__(radioApi, "rx_hop")
        
class RxHopPublicOptions(CategoryInterface):
    def __init__(self):
        super(RxHopPublicOptions, self).__init__()
        self.reset()
        
    def reset(self):
        self.Mode = 0
        self.RssiTimeout = 0x4
        self.Table = [0x01]
    
class AutoRxFreqHop(CategoryInterface):
    def __init__(self, radioApi):
        super(AutoRxFreqHop, self).__init__(radioApi)
        self.rxHopProps = AutoRxFreqHopProps(radioApi)
        self.publicOptions = RxHopPublicOptions()
        
    def get_options(self):
        return self.publicOptions.get_options()
    
    def get_configuration(self, cc):
        if self.enabled:
            return self.rxHopProps.get_configuration(cc)
        return []
    
    def set_option(self, option, value):
        real_value = self.publicOptions.set_option(option, value)
        if (option == "Mode"):
            self.rxHopProps.set_option("RX_HOP_CONTROL__HOP_EN", real_value)
        elif (option == "RssiTimeout"):
            self.rxHopProps.set_option("RX_HOP_CONTROL__RSSI_TIMEOUT", real_value)
        elif (option == "Table"):
            table_size = len(self.publicOptions.Table)
            self.rxHopProps.set_option("RX_HOP_TABLE_SIZE__RX_HOP_TABLE_SIZE", table_size)
            for idx in range(table_size):
                self.rxHopProps.set_option("RX_HOP_TABLE_ENTRY_{}__CHANNEL_NUM".format(idx), self.publicOptions.Table[idx])
            for idx in range(table_size, 64):
                self.rxHopProps.set_option("RX_HOP_TABLE_ENTRY_{}__CHANNEL_NUM".format(idx), 0xff)
        return real_value
    
    def reset(self):
        self.rxHopProps.reset()
        self.publicOptions.reset()
