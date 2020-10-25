'''
Created on Sep 26, 2013

@author: sesuskic
'''

from .parameterset import ParameterSet, ParameterSetArray 

__all__ = ['Command', 'ArgumentStream']

class ArgumentStream(object):
    def __init__(self):
        self.parameter_set_list = []
        self.parameter_set_map = {}
        
    def add_parameter_set(self, parameter_set):
        self.parameter_set_list.append(parameter_set)
        self.parameter_set_map[parameter_set.name] = parameter_set
        
    def get_arguments(self):
        return self.parameter_set_list[1:]
    
    def get_parameter_set(self, parameter_set_name):
        return self.parameter_set_map[parameter_set_name]
        
class Command(object):
    ''' Encapsulation of Command entity from Sixxxx api object xml'''
    def __repr__(self):
        out_str = '[{}]'.format(','.join(['0x{:02x}'.format(b) for b in self.get_arg_stream_raw_bytes()]))
        return out_str
    
    def __init__(self, name, number):
        self.name = name
        self.number = int(number)
        self.inputs = None
        self.outputs = None
        self.property_name = None
        
    def _get_arg_stream_length(self, stream):
        arg_stream_length = 0
        for param_set in stream.parameter_set_list:
            arg_stream_length += param_set.get_arg_count()
        return arg_stream_length
    

    def _build_raw_bytes(self, param_set, raw_bytes):
        for field in param_set.field_list:
            for mapping in field.mappings:
                # TODO: get rid of long() cast with something smarter from
                # object API xml field_type attribute of field element
                raw_bytes[mapping.parameter_start_idx] |= (((int(field.get()) & mapping.field_mask) >> mapping.field_shift) << mapping.parameter_set_lsb)
        
    def _get_parameter_set_map(self, arguments):
        parameter_set_map = {}
        for param_set in arguments:
            field_map = {}
            for field_name in param_set.get_field_names():
                field_map[field_name] = param_set.get_field(field_name)
            parameter_set_map[param_set.name] = (param_set.get_value(), field_map)
        return parameter_set_map
    
    def add_inputs(self, inputs):
        self.inputs = inputs
    
    def add_outputs(self, outputs):
        self.outputs = outputs
    
    def get_input_parameter_set_map(self):
        return self._get_parameter_set_map(self.inputs.get_arguments())

    def compile_reply_stream(self):
        for param_set in self.outputs.responses:
            for field in param_set.field_list:
                self.reply_stream[field.name] = field.value
        return self.reply_stream

    def get_arg_stream_raw_bytes(self):
        raw_bytes = [0]*self._get_arg_stream_length(self.inputs)
        raw_bytes[0] = self.number
        for param_set in self.inputs.get_arguments():
            if isinstance(param_set, ParameterSet):
                self._build_raw_bytes(param_set, raw_bytes)
            elif isinstance(param_set, ParameterSetArray):
                for item in param_set.get_array():
                    self._build_raw_bytes(item, raw_bytes)
        return raw_bytes

    def set_field(self, param_set_name, field_name, value):
        current_value = self.get_field(param_set_name, field_name)
        if isinstance(current_value, list) and isinstance(value, int):
            self.inputs.parameter_set_map[param_set_name].set_field(field_name, len(current_value)*[value])
        elif isinstance(current_value, list) and isinstance(value, list):
            if self.inputs.parameter_set_map[param_set_name].size is not None:
                assert len(value) <= len(current_value)
                idx = 0
                for item in value:
                    if idx < len(current_value):
                        current_value[idx] = item
                    else:
                        current_value.append(item)
                    idx += 1
            else:
                current_value = value    
            self.inputs.parameter_set_map[param_set_name].set_field(field_name, (current_value))
        elif isinstance(current_value, int) and isinstance(value, int):
            self.inputs.parameter_set_map[param_set_name].field_map[field_name].set(value)
        else:
            raise RuntimeError("error in set_field_value")
        return self
    
    def get_field(self, param_set_name, field_name):
        param_set = self.inputs.parameter_set_map[param_set_name]
        return param_set.get_field(field_name)

    def reset(self) :
        for output_set, input_set in zip(self.outputs.parameter_set_list,
                                         self.inputs.parameter_set_list) :
            output_set.reset()
            input_set.reset()
        return self

    def fill_output(self, byte_stream):
        for param_set in self.outputs.parameter_set_list :
            if  param_set.size == None :
                # This case would also assume that the given param_set
                # is actually a ParameterSetArray. Add a sanity check
                assert isinstance(param_set, ParameterSetArray)
                param_set.set_value(byte_stream[param_set.index_start:])
            else :
                param_set.set_value(byte_stream[param_set.index_start:param_set.index_end+1])
        return self

    def get_input_argument_stream_length(self):
        return self._get_arg_stream_length(self.inputs)

    def get_output_argument_stream_length(self):
        return self._get_arg_stream_length(self.outputs)
    
    def get_input_arguments(self):
        return self._get_parameter_set_map(self.inputs.get_arguments())
    
    def get_output_arguments(self):
        return self._get_parameter_set_map(self.outputs.parameter_set_list)
