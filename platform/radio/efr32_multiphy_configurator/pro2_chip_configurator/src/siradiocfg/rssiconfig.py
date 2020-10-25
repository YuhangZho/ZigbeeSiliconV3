'''
Created on Aug 26, 2014

@author: sesuskic
'''

from common import CategoryInterface

class RssiConfig(CategoryInterface):

    def __init__(self):
        super(RssiConfig, self).__init__()
        self.reset()
        
    def reset(self):
        self.Latch = 1
        self.Average = 0
        self.CheckThresholdAtAverage = False
        self.Threshold = 0xff
        self.JumpThreshold = 0x0c
    
    def get_configuration(self, cc):
        command_list = []
        if cc.get_option("RssiConfig.enabled"):
            command_list.append(cc.get_property("MODEM_RSSI_CONTROL")
                                .set_field("LATCH", cc.get_option("RssiConfig.Latch"))
                                .set_field("AVERAGE", cc.get_option("RssiConfig.Average"))
                                .set_field("CHECK_THRESH_AT_LATCH", cc.get_option("RssiConfig.CheckThresholdAtAverage"))
                                .configure())
            command_list.append(cc.get_property("MODEM_RSSI_THRESH")
                                .set_field("RSSI_THRESH", cc.get_option("RssiConfig.Threshold"))
                                .configure())
            command_list.append(cc.get_property("MODEM_RSSI_JUMP_THRESH")
                                .set_field("RSSIJMPTHD", cc.get_option("RssiConfig.JumpThreshold"))
                                .configure())
        return command_list
        
        
        