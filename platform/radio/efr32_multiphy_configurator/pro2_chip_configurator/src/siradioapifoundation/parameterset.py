'''
Created on Sep 26, 2013

@author: sesuskic
'''

import copy
import sys

__all__ = ['ParameterSet', 'ParameterSetArray']

def isPy3():
    return sys.version >'3'

def isInt(value):
    # 'long' is not defined in Python 3.x
    if isPy3():
        return isinstance(value, int)
    return isinstance(value, int) or isinstance(value, long)

class ParameterSet(object):

    def __init__(self, name, args):
        assert len(args) == 2, "Parameter list needs 2 elements"
        self.name = name
        self.index_start = args[0]
        self.index_end = args[1]
        self.size = self.index_end - self.index_start + 1
        self.field_list = []
        self.field_map = {}
        self.dirty = False
        self.value = 0
#         args.parameter_set_list.append(self)
#         args.parameter_set_map[name] = self
#         self.args = args
#         if hasattr(self.args, "default_value"):
#             self.value = self.args.default_value
#         else:
#             self.value = 0      

    def add_field(self, field):
        self.field_list.append(field)
        self.field_map[field.name] = field
        
    def set_field(self, field_name, value):
        self.field_map[field_name].set(value)
        if self.size == 1:
            for field in self.field_list:
                for mapping in field.mappings:
                    mapping_value = ((field.get() & mapping.field_mask) >> mapping.field_shift) << mapping.parameter_set_lsb 
                    parameter_set_mask = ((1 << mapping.fsize) - 1) << mapping.parameter_set_lsb
                    masked_out_value = self.value & ~parameter_set_mask
                    self.value = masked_out_value | mapping_value
        else:
            all_ones = (1 << self.value.bit_length()) - 1
            for mapping in self.field_map[field_name].mappings:
                mapping_value_mask = (~mapping.parameter_set_mask) & all_ones
                self.value = self.value & mapping_value_mask
            for mapping in self.field_map[field_name].mappings:
                mapping_value = ((value & mapping.field_mask) >> mapping.field_shift) << mapping.parameter_set_lsb
                shifted_mapping_value = mapping_value << mapping.parameter_set_shift
                self.value = self.value | shifted_mapping_value
        return self

    def get_field(self, field_name) :
        return self.field_map[field_name].get()

    def set_value(self, value):
        if isInt(value):
            self.value = value
            for field in self.field_list :
                new_value = 0
                for mapping in field.mappings:
                    new_value |= (((self.value & mapping.parameter_set_mask) >> mapping.parameter_set_shift)  << mapping.field_shift)
                field.set(new_value)
        elif isinstance(value, list):
            assert(self.size == len(value))
            new_value = 0
            reversed_bytes = value[::-1]
            for item in reversed_bytes:
                new_value += (item << (reversed_bytes.index(item) * 8))
            self.set_value(new_value)
        return self

    def get_field_names(self):
        return [field.name for field in self.field_list]
        
    def get_arg_count(self):
        return self.size

    def get_value(self):
        return self.value

    def reset (self) :
        for field in self.field_list :
            field.reset()
        return self

class ParameterSetArray(object):
    def __init__(self, name, args):
        assert len(args) == 4, "Parameter list needs 4 elements"
        self.name = name
        self.index_start = args[0]
        self.index_end = args[1]
        if args[2] == True:
            self.size = self.index_end - self.index_start + 1
        else:
            self.size = None
        self.parameter_set_list = []
        self.parameter_set_map = {} # not used
        self.parameter = None
        
    def add_parameter_set(self, parameter_set):
        self.parameter_set_list.append(parameter_set)
        self.parameter_set_map[parameter_set.name] = parameter_set
        
    def get_array(self):
        return self.parameter_set_list
    
    def set_field(self, field_name, value_array):
        if self.size is not None:
            if len(value_array) != len(self.parameter_set_list):
                raise RuntimeError("Invalid value array size")
            idx = 0
            for value in value_array:
                self.parameter_set_list[idx].field_map[field_name].set(value)
                idx += 1
        else:
            if self.parameter is None:
                self.parameter = copy.deepcopy(self.parameter_set_list[0])
            param_start_idx = self.parameter.field_list[0].mappings[0].parameter_start_idx
            self.parameter_set_list[:] = [] 
            idx = 0
            for value in value_array:
                self.parameter_set_list.append(copy.deepcopy(self.parameter))
                self.parameter_set_list[idx].field_map[field_name].set(value)
                self.parameter_set_list[idx].field_map[field_name].mappings[0].parameter_start_idx = param_start_idx + idx
                idx += 1

    def set_value(self, byte_stream):
        step_size = self.parameter_set_list[0].get_arg_count()
        cur_index = 0
        if self.size is not None :
            for param_set in self.parameter_set_list :
                param_set.set_value(byte_stream[cur_index:cur_index + step_size])
                cur_index += step_size
        else:
            if self.parameter is None:
                self.parameter = copy.deepcopy(self.parameter_set_list[0])
            param_start_idx = self.parameter.field_list[0].mappings[0].parameter_start_idx
            self.parameter_set_list[:] = [] 
            idx = 0
            for value in byte_stream: 
                self.parameter_set_list.append(copy.deepcopy(self.parameter))
                self.parameter_set_list[idx].set_value([value])
                self.parameter_set_list[idx].field_list[0].mappings[0].parameter_start_idx = param_start_idx + idx
                idx += 1
            
    def get_field(self, field_name):
        return [param.get_field(field_name) for param in self.parameter_set_list]
    
    def get_field_names(self):
        # Elements in a ParameterSetArray have the same fields
        return self.parameter_set_list[0].field_map.keys()

    def get_arg_count(self):
        return len(self.parameter_set_list)

    def get_value(self) :
        return [param.get_value() for param in self.parameter_set_list]

    def reset (self) :
        for param_set in self.parameter_set_list :
            param_set.reset()

    