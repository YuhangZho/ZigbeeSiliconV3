'''
Created on Oct 17, 2013

@author: sesuskic
'''


import random
from collections import OrderedDict
from .siradioapifactory import SiRadioApiFactory
from .ezradio2configprops import EzRadio2ConfigProps
from .ezradio2pro2plusconfigprops import EzRadio2Pro2PlusConfigProps

__all__ = ['EzRadio2Config']

class EzRadio2Config(object):
    obf_key = [0xb5, 0x0e, 0x9c, 0x82]
    poly_coef = 0x8005
    
    def __init__(self, radio = "pro2"):
        if radio == "pro2" :
            self.radio_api = SiRadioApiFactory.createRadioApi("si4460_revb1b_factory")
            self.property_names = EzRadio2ConfigProps.property_names
        elif radio == "pro2plus":
            self.radio_api = SiRadioApiFactory.createRadioApi("si4460_revc2a_factory")
            self.property_names = EzRadio2Pro2PlusConfigProps.property_names
        else:
            raise RuntimeError("Invalid radio parameter to ezconfig2config: {}".format(radio))
        self.crc = 0xffff
        self.property_list = OrderedDict({})
        for name in self.property_names:
            self.property_list[name] = self.radio_api.properties.get_property(name)

    def _get_raw_property_bytes(self, ezconfig_seed = None):
        if ezconfig_seed is None:
            first_element = int(random.uniform(0, 255))
        else:
            first_element = ezconfig_seed
        raw_property_bytes = [first_element]
        for prop in self.property_list.values():
            if not(prop == None):
                # calling configure() on each command to get the SET_COMMANDS,
                # then take the DATA part of the argument stream starting at index 4
                args = prop.configure().get_arg_stream_raw_bytes()
                raw_property_bytes += args[4:]
            else:
                raw_property_bytes.append(0)
        # Ugly hack to work around change in radio API definition for rev B. The MODEM_AGC_CONTROL,
        # which corresponds to element at index 59 of ezconfigarray used to have ADC_GAIN_COR_EN field,
        # and it's hard-coded to 1 in modem calculator algorithm.
        raw_property_bytes[59] += 0x2
        return raw_property_bytes
    
    def _process_crc(self, data):
        self.crc ^= (data << 8) & 0xffff
        for idx in range(0,8):
            if (self.crc & 0x8000) != 0x0000:
                self.crc = (self.crc << 1) & 0xffff
                self.crc ^= self.poly_coef
            else:
                self.crc = (self.crc << 1) & 0xffff
    
    def set_properties(self, property_list):
        for prop in property_list:
            try:
                self.property_list[prop.name].set_value(prop.get_value())
            except KeyError:
                # Caller passed in Property that's not in the ezconfig array.
                # Just ignore that.
                pass
    
    def set_property(self, name, value):
        self.property_list[name].set_value(value)
        
    def get_property(self, name):
        return self.property_list[name]
    
    def get_array(self, ezconfig_seed = None):
        self.crc = 0xffff
        processed_array = []
        idx = 0
        for item in self._get_raw_property_bytes(ezconfig_seed):
            processed_array.append(item ^ (self.crc & 0xff) ^ self.obf_key[idx % 4])
            self._process_crc(processed_array[idx])
            idx += 1
        return processed_array
    
    def get_crc(self):
        return self.crc
        
