'''
Created on Aug 15, 2014

@author: sesuskic
'''

from common import CategoryInterface
 
__all__ = ["CrystalCapBank", "Pro2CrystalCapBank"]

class CrystalCapBank(CategoryInterface):
    def __init__(self):
        super(CrystalCapBank, self).__init__()
        self.reset()
        
    def reset(self):
        self.value = 0x52

    def get_configuration(self, cc):
        return [cc.get_property("EZCONFIG_XO_TUNE").
                set_field("TUNE_VALUE", cc.get_option("CrystalCapBank.value"))
                .configure()]

class Pro2CrystalCapBank(CrystalCapBank):
    def __init__(self):
        super(Pro2CrystalCapBank, self).__init__()
        
    def get_configuration(self, cc):
        return [cc.get_property("GLOBAL_XO_TUNE")
                .set_field("TUNE_VALUE", cc.get_option("CrystalCapBank.value"))
                .configure()]
