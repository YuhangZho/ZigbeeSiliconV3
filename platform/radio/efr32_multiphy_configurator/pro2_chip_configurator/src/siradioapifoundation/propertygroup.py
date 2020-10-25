'''
Created on Sep 26, 2013

@author: sesuskic
'''
__all__ = ['PropertyGroup']

class PropertyGroup(object):
    def __init__(self, name, group_num):
        self.name = name
        self.group_num = group_num
        self.property_list = []
        
    def add_property(self, prop):
        self.property_list.append(prop)
        
    def get_property_map(self):
        value_dict = {}
        for prop in self.property_list:
            value_dict[prop.name] = prop
        return value_dict
    
    def get_property_list(self):
        return self.property_list
