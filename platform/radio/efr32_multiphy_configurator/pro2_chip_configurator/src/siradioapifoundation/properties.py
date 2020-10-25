'''
Created on Sep 26, 2013

@author: sesuskic
'''
__all__ = ['Properties']

class Properties(object):
    def __init__(self, name, args):
        self.name = name
        self.index_width = args[0]
        self.grp_width = args[2]
        self.parameter_width = args[1]
        self.property_group_list = []
        self.all_properties_map = {}
        
    def add_property_group(self, property_group):
        self.property_group_list.append(property_group)
        
    def get_property(self, name):
        for pg in self.property_group_list:
            if name in pg.get_property_map():
                return pg.get_property_map()[name.upper()]
        return None
    
    def get_group_property_list(self, group_name):
        for prop_group in self.property_group_list:
            if prop_group.name == group_name.upper():
                return prop_group.get_property_list()
        return []
        
