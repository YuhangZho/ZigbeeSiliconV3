'''
Created on Aug 17, 2014

@author: sesuskic
'''

from common import CategoryInterface

__all__  = ["ModemFsk4Map"]

class ModemFsk4Map(CategoryInterface):
    def __init__(self):
        super(ModemFsk4Map, self).__init__()
        self.reset()

    def reset(self):
        self.value = 0
        