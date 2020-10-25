'''
Created on Sep 26, 2013

@author: sesuskic
'''

__all__ = ['Mapping']

class Mapping(object):
    def __init__(self, args):
        parameter_start_idx = args[0]
        parameter_bit_msb = args[1]
        fsize = args[2]
        split_msb = args[3]
        split_lsb = args[4]
        parameter_set_index_start = args[5]
        parameter_set_index_end = args[6]
        
        self.parameter_start_idx = parameter_start_idx
        self.parameter_bit_msb = parameter_bit_msb
        self.relative_parameter_index = self.parameter_start_idx - parameter_set_index_start
        self.fsize = fsize
        parameter_index_span = parameter_set_index_end - self.parameter_start_idx
        split_field = False
        if (split_msb != None and split_lsb != None):
            split_field = True
            self.split_msb = split_msb
            self.split_lsb = split_lsb
        else:
            self.split_msb = parameter_bit_msb
            self.split_lsb = self.parameter_bit_msb - self.fsize + 1

        self.parameter_set_lsb = (self.parameter_bit_msb - self.fsize + 1)
        self.parameter_set_mask = ((1 << self.fsize) - 1) <<  ((parameter_index_span * 8) + self.parameter_set_lsb)
        self.parameter_set_shift = (parameter_index_span * 8) + self.parameter_set_lsb

        if split_field:
            self.field_shift = self.split_lsb
            self.field_mask = ((1 << (self.split_msb - self.split_lsb + 1)) - 1) << self.field_shift
        else:
            self.field_shift = 0
            self.field_mask = (1 << (self.split_msb - self.split_lsb + 1)) - 1
