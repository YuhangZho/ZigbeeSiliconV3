'''
Created on Sep 26, 2013

@author: sesuskic
'''

__all__ = ['Field']

_FIELD_INFO_DISPLAY = """Field
name:            {name}
value:           {_value}
default value:   {default_value} 
size:            {fsize}
"""

class Field(object):
    
    def __repr__(self):
        out_str = _FIELD_INFO_DISPLAY.format(**self.__dict__) 
        return out_str

    def __init__(self, name, args):
        assert len(args) == 2, "Parameter list needs 2 elements"
        self.name = name
        self.fsize = args[0]
        self.default_value = args[1]
        self._value = args[1]
        self.mappings = []

    def add_mapping(self, mapping):
        self.mappings.append(mapping)
                
    def set(self, value):
        self._value = value
                
        
    def reset(self):
        self.set(self.default_value)

    def get(self):
        return self._value
    
