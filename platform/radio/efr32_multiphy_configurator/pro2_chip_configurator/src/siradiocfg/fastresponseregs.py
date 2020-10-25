'''
Created on Jun 6, 2014

@author: sesuskic
'''
from .propertygroupbasedoptions import PropertyGroupBasedOptions

__all__ = ["FastResponseRegs"]

class FastResponseRegs(PropertyGroupBasedOptions):

    def __init__(self, radioApi):
        super(FastResponseRegs, self).__init__(radioApi, "frr_ctl")
