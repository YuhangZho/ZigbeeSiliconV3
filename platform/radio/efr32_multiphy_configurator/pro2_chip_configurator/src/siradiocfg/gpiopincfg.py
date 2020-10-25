'''
Created on Aug 15, 2014

@author: sesuskic
'''

from common import CategoryInterface
 
__all__ = ["GpioPinCfg"]

class GpioPinCfg(CategoryInterface):
    def __init__(self):
        super(GpioPinCfg, self).__init__()
        self.reset()
        
    def reset(self):
        self.nirq_pull_ctl = 0
        self.nirq_mode = 0 
        self.sdo_pull_ctl = 0
        self.sdo_mode = 0 
        self.drv_strength = 0
        self.gpio_pull_ctl = [0,0,0,0]
        self.gpio_mode =  [0,0,0,0]
        
    def get_configuration(self, cc):
        if cc.get_option("GpioPinCfg.enabled"):
            return [cc.get_command("gpio_pin_cfg")
                    .set_field("GPIO", "PULL_CTL", cc.get_option("GpioPinCfg.gpio_pull_ctl"))
                    .set_field("GPIO", "GPIO_MODE", cc.get_option("GpioPinCfg.gpio_mode"))
                    .set_field("NIRQ", "PULL_CTL", cc.get_option("GpioPinCfg.nirq_pull_ctl"))
                    .set_field("NIRQ", "NIRQ_MODE", cc.get_option("GpioPinCfg.nirq_mode"))
                    .set_field("SDO", "PULL_CTL", cc.get_option("GpioPinCfg.sdo_pull_ctl"))
                    .set_field("SDO", "SDO_MODE", cc.get_option("GpioPinCfg.sdo_mode"))
                    .set_field("GEN_CONFIG", "DRV_STRENGTH", cc.get_option("GpioPinCfg.drv_strength"))]
        else:
            return []
        
