'''
Created on Jul 8, 2014

@author: sesuskic
'''

from common import CategoryInterface
 
__all__ = ["PropertyGroupBasedOptions"]

class PropertyGroupBasedOptions(CategoryInterface):
    def __init__(self, radioApi, property_group_name, property_exclude_list = []):
        super(PropertyGroupBasedOptions, self).__init__()
        # self.prop_list can be empty, which means that the no property exists in the given
        # property group. This is not a strange case at all.
        self.prop_list = []
        for p in radioApi.properties.get_group_property_list(property_group_name):
            if not(p.name in property_exclude_list):
                self.prop_list.append(p)
        self.options = {"enabled": True}
        for prop in self.prop_list:
            setattr(self, prop.name, prop.get_value())
            self.refresh(prop)
        if len(self.prop_list) == 0:
            # If no prop_list is empty, then disable this category.
            self.enabled = False
            self.options["enabled"] = False
        
    def refresh(self, prop):
        for field_name in prop.get_field_names():
            setattr(self, prop.name + "__" + field_name, prop.get_field(field_name))
            self.options[prop.name + "__" + field_name] = prop.get_field(field_name)
        
    def get_options(self):
        return self.options
    
    def set_option(self, name, value):
#         print("DEBUG: setting option {} to {}".format(name, value))
#         if len(self.options) > 1:
        real_value = None
        try:
            real_value = super(PropertyGroupBasedOptions, self).set_option(name, value) 
        except AttributeError:
            return real_value
        if "__" in name or name == "enabled":
            self.options[name] = getattr(self, name)
            if name == "enabled":
                self.enabled = value
            else:
                [prop_name, field_name] = name.split("__")
                prop = self.get_property(prop_name)
                prop.set_field(field_name, self.options[name])
                setattr(self, prop_name, prop.get_value())
        else:
            self.set_property(name, real_value)
            real_value = self.options
        return real_value
     
    def get_configuration(self, cc):
        command_list = []
        if self.enabled:
            for prop in self.prop_list:
                propSetCmd = prop.configure()
                if prop.is_dirty() or not(cc.dirty_props_only):
                    command_list.append(propSetCmd)
#         print("DEBUG", commands)
        return command_list
    
    def get_property_list(self):
        return self.prop_list
    
    def set_property(self, name, value):
        for prop in self.prop_list:
            if prop.name == name:
#                 print("DEBUG: setting property {} to {}".format(name, value))
                prop.set_value(value)
                self.refresh(prop)
                return
        
    def get_property(self, name):
        for prop in self.prop_list:
            if prop.name == name:
                return prop
            
    def reset(self):
        for prop in self.prop_list:
            prop.reset()
            self.refresh(prop)
        