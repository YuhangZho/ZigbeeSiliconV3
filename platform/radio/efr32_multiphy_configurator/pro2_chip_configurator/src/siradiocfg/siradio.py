'''
Created on Sep 24, 2013

@author: sesuskic
'''
import os
from copy import copy
from common import Properties
from collections import OrderedDict
from si4440_modem_calc import Pro2ModemCalc
from common import ChipConfiguratorInterface

__all__ = ["SiRadio"]

class SiRadio(ChipConfiguratorInterface):
    def __init__(self, radio_api):
        super(SiRadio,self).__init__()
        self.command_list = []
        self.name = radio_api.part_name
        self.radio_api = radio_api
        self._create_modem_radio_api()
        self._create_inputs(self.radio_api)
        self._sync_categories_and_options()
        self._profiles = self._initialize_default_profile()
        self._load_profiles()
        self.dirty_props_only = False
        self.modem_calc = self._create_modem_calculator(self.inputs.Modem)
    
    def _create_inputs(self, radioApi):
        raise RuntimeError("Method needs to be implemented in derived class")

    def _restrict_modem_inputs(self):
        raise RuntimeError("Method needs to be implemented in derived class")
        
    def _initialize_default_profile(self):
        defaults = copy(self.options)
        return {"default" : defaults}
        
    def _load_profiles(self):
        if len(self._profiles) == 1:
            # There is an implied default profile, and it's the data with which the chip configurator
            # categories, and associated inputs are initialized.
            myPath = os.path.abspath(os.path.dirname(__file__))
            props = Properties()
            propFilePath = myPath + "/../profiles/"
            for item in ["owl-security"]:
                with open(propFilePath + item + ".properties") as f:
                    props.load(f)
                    data = {}
                    for k, v in props.getPropertyDict().items():
                        data[k] = eval(v)
                    self._profiles[item] = data.copy()
    
    def _create_modem_radio_api(self):
        self.modem_radio_api = self.radio_api
    
    def _create_modem_calculator(self, modem_inputs):
        return Pro2ModemCalc(modem_inputs)
    
    def _add_to_raw_data_dict(self, raw_data_dict, key, value):
#         if key in raw_data_dict.keys():
#             print("INTERNAL WARNING: {} already exists".format(key))
#             if raw_data_dict[key] != value:
#                 print("   And specifying new values!")
        raw_data_dict[key] = value
    
    def configure(self, **kwargs):
        configData = self._configureImpl(**kwargs)
        configData.update(self._get_modem_calc_info_data())
        return configData
    
    def _configureImpl(self, **kwargs):
        raise RuntimeError("Method needs to be implemented in derived class")
    
    def get_config_data(self):
        config_data = [] 
        for cmd in self.command_list:
            config_data.append((cmd.name, cmd.get_input_arguments()))
        return config_data
    
    def get_raw_bytes(self, **kwargs):
        split_set_property = False
        if kwargs:
            if "split_set_property" in kwargs.keys():
                split_set_property = True
        raw_data_dict = OrderedDict({})
        for cmd in self.command_list:
            raw_bytes = cmd.get_arg_stream_raw_bytes()
            cmd_key = "api_raw:" + cmd.name
            if cmd.name == "SET_PROPERTY" or cmd.name == "GET_PROPERTY":
                cmd_key = "api_raw:" + "{}_{}".format(cmd.name, cmd.property_name)
            if cmd.number == 0x66:
                if len(raw_bytes[1:]) < 128:
                    self._add_to_raw_data_dict(raw_data_dict, "{}".format(cmd_key), bytearray(raw_bytes))
                else:
                    split_limit = 106
                    num_split_cmds = int(len(raw_bytes[1:])/split_limit)
                    if len(raw_bytes[1:])%split_limit > 0:
                        num_split_cmds += 1
                    for idx in range(0, num_split_cmds):
                        newCmd = [0x66]
                        newCmd += (raw_bytes[1:])[idx*split_limit:(idx+1)*split_limit]
                        self._add_to_raw_data_dict(raw_data_dict, "{}_{}".format(cmd_key, idx), bytearray(newCmd))
                        if idx != num_split_cmds-1:
                            self._add_to_raw_data_dict(raw_data_dict, "{}_NOP_{}".format(cmd_key, idx), bytearray([0x00]))
            else:
                if (split_set_property and 
                    cmd.name == "SET_PROPERTY" 
                    and len(raw_bytes) > 16):
                    data = raw_bytes[4:]
                    start_prop = raw_bytes[3]
                    prop_group = raw_bytes[1]
                    num_split_cmds = int(len(data)/12)
                    if len(data)%12 > 0:
                        num_split_cmds += 1
                    for idx in range(0, num_split_cmds):
                        num_props = 12
                        if idx == num_split_cmds-1 :
                            num_props = len(data)-idx*12
#                         print("{}_{}".format(cmd_key, idx), [cmd.number, prop_group, num_props, start_prop+idx*12] + data[idx*12:idx*12+num_props])
                        self._add_to_raw_data_dict(raw_data_dict, "{}_{}".format(cmd_key, idx),
                                                   bytearray([cmd.number, prop_group, num_props, start_prop+idx*12] + data[idx*12:idx*12+num_props]))
                else:
                    self._add_to_raw_data_dict(raw_data_dict, "{}".format(cmd_key), bytearray(raw_bytes))
        return raw_data_dict
        
    def get_field_value(self, name):
        return self.radio_api.properties.all_properties_map[name].get()
    
    def get_property_raw_bytes(self, name):
        return self.radio_api.properties.all_properties_map[name].configure().get_arg_stream_raw_bytes()
    
    def get_name(self):
        return self.name
    
    def get_command(self, name):
        return self.radio_api.commands.get_command(name)
    
    def get_property(self, name):
        return self.radio_api.properties.get_property(name)
    
    def get_property_group(self, name):
        return self.radio_api.get_property_group(name)
    
    def set_profile(self, name):
        profile_name = name
        for category in self.get_categories():
            self.get_category(category).reset()
        # if requested profile is empty string, that means use default for this chip.
        if not(name in ["", "custom", "default"]):
            for k, v in self._profiles[profile_name].items():
                self.set_option(k, v)
        self._sync_categories_and_options()
        
    
    def get_profile_ids(self):
        return self._profiles.keys()
    
    def get_profile_data(self, profile):
        return self._profiles[profile]
    
    def add_profile(self, profile, data):
        # Existing profile will get new data
        self._profiles[profile] = data.copy()
        self.set_profile(profile)
        
    def dirty_props_only(self, value):
        self.dirty_props_only = value
        
    def _get_modem_calc_info_data(self):
        modem_calc_info_data = OrderedDict({})
        for k, v in self.get_options("Modem").items():
            if k == "Modem.enabled":
                continue
            key = k.split("Modem.")[1]
            if key.startswith("API_"):
                key = key.split("API_")[1]
            modem_calc_info_data["modem_input:" + key] = v
        modem_data_prefix = "modem_data:"
        modem_calc_info_data[modem_data_prefix + "if_freq"] = self.modem_calc.if_freq
        modem_calc_info_data[modem_data_prefix + "wb_filter"] = self.modem_calc.demodulator.filter_k1
        modem_calc_info_data[modem_data_prefix + "wb_filter_bw"] = self.modem_calc.demodulator.RX_CH_BW_k1
        modem_calc_info_data[modem_data_prefix + "nb_filter"] = self.modem_calc.demodulator.filter_k2
        modem_calc_info_data[modem_data_prefix + "nb_filter_bw"] = self.modem_calc.demodulator.RX_CH_BW_k2
        return modem_calc_info_data
