'''
Created on Apr 7, 2014

@author: sesuskic
'''

from .propertygroupbasedoptions import PropertyGroupBasedOptions

__all__ = ["PowerAmplifier"]

class PowerAmplifier(PropertyGroupBasedOptions):

    def __init__(self, radioApi):
        super(PowerAmplifier, self).__init__(radioApi, "pa", ["PA_TC"])