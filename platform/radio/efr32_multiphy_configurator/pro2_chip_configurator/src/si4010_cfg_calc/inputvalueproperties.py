'''
Created on Nov 19, 2013

@author: sesuskic
'''

from common import Properties

class InputValueProperties(object):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        self._input_specs = {}
        properties = Properties()
        properties.load(open("owlinputs.properties"))
        for item in properties["inputs"].split(","):
            type = properties[item + ".type"]
            default = properties[item + ".default"]
            low = properties[item + ".min"]
            high = properties[item + ".max"]
            increment = properties[item + ".increment"]
            values = properties[item + ".values"]
            self._input_specs[item] = {"type":type,"default":default,"min":low,"max":high,"increment":increment, "values": values}
        
    def get_input_specs(self):
        return self._input_specs
        
if __name__ == "__main__":
    limits = InputValueProperties()
    for k, v in limits.get_input_specs().items():
        print(k, v)