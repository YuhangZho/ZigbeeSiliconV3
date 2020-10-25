'''
Created on Aug 15, 2014

@author: sesuskic
'''

from common import CategoryInterface
 
__all__ = ["PowerUp", "Pro2PowerUp"]

def get_power_up_command(cc):
    return (cc.get_command("power_up")
            .set_field("BOOT_OPTIONS", "PATCH", cc.get_option("PowerUp.patch"))
            .set_field("BOOT_OPTIONS", "FUNC", cc.get_option("PowerUp.boot_mode"))
            .set_field("XO_FREQ", "XO_FREQ", int(cc.get_option("Modem.API_freq_xo"))))

class PowerUp(CategoryInterface):
    def __init__(self):
        super(PowerUp, self).__init__()
        self.reset()
        
    def reset(self):
        self.patch = 0 # copy image from OTP and boot
        self.boot_mode = 1 # boot in main mode.

    def get_configuration(self, cc):
        return [get_power_up_command(cc)]

class Pro2PowerUp(PowerUp):
    def __init__(self):
        super(Pro2PowerUp, self).__init__()
        self.use_external_tcxo = 0
        
    def set_option(self, option, value):
        real_value = 0
        getattr(self, option)
        if option == "use_external_tcxo":
            if value in [True, "TRUE", "True","true", "1", 1]:
                self.use_external_tcxo = 1
            elif value in [False, "FALSE", "False","false", "0", 0]:
                self.use_external_tcxo = 0
            real_value = self.use_external_tcxo
        else:
            real_value = super(Pro2PowerUp, self).set_option(option, value)
        return real_value
            
    def reset(self):
        super(Pro2PowerUp, self).reset()
        self.use_external_tcxo = 0
        
    def get_configuration(self, cc):
        return [get_power_up_command(cc)
                .set_field("XTAL_OPTIONS", "TCXO", int(cc.get_option("PowerUp.use_external_tcxo")))]
