'''
Created on Jan 23, 2014

@author: sesuskic
'''
from .propertygroupbasedoptions import PropertyGroupBasedOptions

__all__ = ["Global", "GlobalPlus"]

class Global(PropertyGroupBasedOptions):

    def __init__(self, radioApi):
        super(Global, self).__init__(radioApi, "global", ["GLOBAL_XO_TUNE", # this property is set elsewhere in chip configurator
                                                          "GLOBAL_LOW_BATT_THRESH", # the other three are not touched by .NET WDS RCA
                                                          "GLOBAL_WUT_CAL",
                                                          "GLOBAL_BUFCLK_CFG"])
    def get_configuration(self, cc):
        # Implementation of http://jira.silabs.com/browse/WDS-162
        cc.set_option("Global.GLOBAL_CONFIG__RESERVED", 1)
        return super(Global, self).get_configuration(cc)

class GlobalPlus(PropertyGroupBasedOptions):
    
    def __init__(self, radioApi):
        super(GlobalPlus, self).__init__(radioApi, "global", ["GLOBAL_XO_TUNE", # this property is set elsewhere in chip configurator
                                                              "GLOBAL_LOW_BATT_THRESH", # the other three are not touched by .NET WDS RCA
                                                              "GLOBAL_WUT_CAL"])
