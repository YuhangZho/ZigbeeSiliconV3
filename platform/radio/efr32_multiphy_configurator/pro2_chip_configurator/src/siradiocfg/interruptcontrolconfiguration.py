'''
Created on Jan 23, 2014

@author: sesuskic
'''
from .propertygroupbasedoptions import PropertyGroupBasedOptions

__all__ = ["InterruptControl"]

class InterruptControl(PropertyGroupBasedOptions):

    def __init__(self, radioApi):
        super(InterruptControl, self).__init__(radioApi, "int_ctl")
