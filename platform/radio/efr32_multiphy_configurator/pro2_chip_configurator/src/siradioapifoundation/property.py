'''
Created on Sep 26, 2013

@author: sesuskic
'''
from .command import Command, ArgumentStream
from .parameterset import ParameterSet, ParameterSetArray
from .field import Field
from .mapping import Mapping

__all__ =['Property']

_PROPERTY_INFO_DISPLAY = """Property
name:   {name}
group:  {group.name}
value:  {parameter_set_list[0].value}
"""

class Property(object):

    def __init__(self, name, args):
        self.name = name
        self.index_start = args[0]
        self.index_end = args[1]
        self.default_value = args[2]
        self.parameter_set_list = []
        self.parameter_set_map = {}
        self.group_number = args[3]
        self._create_and_init_set_cmd()
        self._create_and_init_get_cmd()
        self._dirty = False
        
    def _create_set_get_common_inputs(self):
        inputs = ArgumentStream()
        cmdParamSet = ParameterSet('CMD', [0x00, 0x00])
        inputs.add_parameter_set(cmdParamSet)
        groupParamSet = ParameterSet('GROUP', [0x01, 0x01])
        groupField = Field('GROUP', [8, 0])
        groupMapping = Mapping([0x01, 7, 8, None, None]+[0x01, 0x01])
        groupField.add_mapping(groupMapping)
        groupParamSet.add_field(groupField)
        inputs.add_parameter_set(groupParamSet)
        numPropsParamSet = ParameterSet('NUM_PROPS', [0x02, 0x02])
        numPropsField = Field('NUM_PROPS', [8, 0])
        numPropsMapping = Mapping([0x02, 7, 8, None, None]+[0x02, 0x02])
        numPropsField.add_mapping(numPropsMapping)
        numPropsParamSet.add_field(numPropsField)
        inputs.add_parameter_set(numPropsParamSet)
        startPropParamSet = ParameterSet('START_PROP', [0x03, 0x03])
        startPropField = Field('START_PROP', [8, 0])
        startPropMapping = Mapping([0x03, 7, 8, None, None]+[0x03, 0x03])
        startPropField.add_mapping(startPropMapping)
        startPropParamSet.add_field(startPropField)
        inputs.add_parameter_set(startPropParamSet)
        return inputs

    def _create_set_get_common_outputs(self):
        outputs = ArgumentStream()
        ctsParamSet = ParameterSet('CTS', [0x00, 0x00])
        ctsField = Field('CTS', [8, 0])
        ctsMapping = Mapping([0x00, 7, 8, None, None]+[0x00, 0x00])
        ctsField.add_mapping(ctsMapping)
        ctsParamSet.add_field(ctsField)
        outputs.add_parameter_set(ctsParamSet)
        return outputs

    def _create_and_init_set_cmd(self):        
        '''
            Each property has its own Set Property command 
        '''
        self.set_cmd = Command('SET_PROPERTY', 0x11)
        self.set_cmd.property_name = self.name
        inputs = self._create_set_get_common_inputs()
        
        dataParamSetArray = ParameterSetArray('DATA', [4, 3 + self.index_end - self.index_start + 1, True, 1])
        for idx in range(4, 4 + self.index_end - self.index_start + 1):
            temp_param_set = ParameterSet('DATA', [idx, idx])
            temp_field = Field('DATA', [8, 0])
            temp_mapping = Mapping([idx, 7, 8, 7, 0]+[idx, idx])
            temp_field.add_mapping(temp_mapping)
            temp_param_set.add_field(temp_field)
            dataParamSetArray.add_parameter_set(temp_param_set)
        inputs.add_parameter_set(dataParamSetArray)
        self.set_cmd.add_inputs(inputs)
        self.set_cmd.set_field("GROUP", "GROUP", self.group_number)
        self.set_cmd.set_field("NUM_PROPS", "NUM_PROPS", self.index_end - self.index_start + 1)
        self.set_cmd.set_field("START_PROP", "START_PROP", self.index_start)
        
        outputs = self._create_set_get_common_outputs()
        self.set_cmd.add_outputs(outputs)

    def _create_and_init_get_cmd(self):
        '''
            Each property has its own Get Property command 
        '''
        self.get_cmd = Command('GET_PROPERTY', 0x12)
        self.get_cmd.property_name = self.name
        inputs = self._create_set_get_common_inputs()
        self.get_cmd.add_inputs(inputs)
        self.get_cmd.set_field("GROUP", "GROUP", self.group_number)
        self.get_cmd.set_field("NUM_PROPS", "NUM_PROPS", self.index_end - self.index_start + 1)
        self.get_cmd.set_field("START_PROP", "START_PROP", self.index_start)
        
        outputs = self._create_set_get_common_outputs()
        dataParamSetArray = ParameterSetArray('DATA', [1, self.index_end - self.index_start + 1, True, 1])
        for idx in range(1, self.index_end - self.index_start + 1 + 1):
            temp_param_set = ParameterSet('DATA', [idx, idx])
            temp_field = Field('DATA', [8, 0])
            temp_mapping = Mapping([idx, 7, 8, 7, 0]+[idx, idx])
            temp_field.add_mapping(temp_mapping)
            temp_param_set.add_field(temp_field)
            dataParamSetArray.add_parameter_set(temp_param_set)
        outputs.add_parameter_set(dataParamSetArray)
        self.get_cmd.add_outputs(outputs)
        
    def add_parameter_set(self, parameter_set):
        self.parameter_set_list.append(parameter_set)
        self.parameter_set_map[parameter_set.name] = parameter_set
        self.set_value(self.default_value)
        
    def get_field_map(self):
        return self.parameter_set_list[0].field_map
    
    def set_value(self, value):
        if value == self.default_value and self._dirty == False:
            pass
        elif value != self.parameter_set_list[0].get_value():
            self._dirty = True
        else:
            self._dirty = False
        self.parameter_set_list[0].set_value(value)
        return self
    
    def set_field(self, field_name, value):
        if value == self.parameter_set_list[0].get_field(field_name):
            self._dirty = False
        else:
            self._dirty = True
        self.parameter_set_list[0].set_field(field_name, value)
        return self
    
    def reset(self):
        self.set_value(self.default_value)
        return self
        
    def get_value(self):
        return self.parameter_set_list[0].get_value()
    
    def get_field(self, field_name):
        return self.parameter_set_list[0].get_field(field_name)
    
    def get_field_names(self):
        return self.parameter_set_list[0].get_field_names()
    
    def configure(self):
        for field in self.parameter_set_list[0].field_list:
            for mapping in field.mappings:
                mapping_value = ((field.get() & mapping.field_mask) >> mapping.field_shift) << mapping.parameter_set_lsb 
                parameter_set_mask = ((1 << mapping.fsize) - 1) << mapping.parameter_set_lsb
                set_cmd_param_set = self.set_cmd.inputs.get_parameter_set("DATA").parameter_set_list[mapping.relative_parameter_index]
                masked_out_value = set_cmd_param_set.get_value() & ~parameter_set_mask
                set_cmd_param_set.set_value(masked_out_value | mapping_value)
        return self.set_cmd
        
    def get_command(self):
        return self.get_cmd
    
    def fill_output(self, bytestream):
        self.parameter_set_list[0].set_value(bytestream[1:])
        self.get_command().fill_output(bytestream)
        return self

    def is_dirty(self):
        return self._dirty