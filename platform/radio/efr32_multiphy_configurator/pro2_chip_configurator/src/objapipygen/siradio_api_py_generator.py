'''
Created on May 14, 2013

@author: sesuskic
'''
import sys
import os
import argparse
from lxml import etree

__all__ = ['SiRadioApiPyGen']

#===============================================================================
# XPath and Attribute Constants 
#===============================================================================
_CCB_XPATH_PART_GROUP = "/root/part_groups/part_group"
_CCB_XPATH_COMMAND_SET = "command_sets/command_set"
_CCB_XPATH_COMMAND_SETS = "command_sets"
_CCB_XPATH_COMMAND = "command"
_CCB_XPATH_NUMBER = "number"
_CCB_XPATH_FIELD = "field"
_CCB_FIELD_SIZE = "fsize"
_CCB_XPATH_PROPERTY_GROUP = "property_groups/property_group"
_CCB_XPATH_PRPOERTY_GROUPS = "property_groups"
_CCB_XPATH_PROPERTY = "property"
_CCB_XPATH_PROPERTY_ARRAY = "property_array"
_CCB_XPATH_PROPERTY_ACTIVATION = "property_activation"
_CCB_XPATH_FEATURE = "features/feature"
_CCB_XPATH_MODE = "modes/mode"
_CCB_XPATH_PARAMETER_SET = "{}/parameter_set"
_CCB_XPATH_PARAMETER_SET_ARRAY = "{}/parameter_set_array"
_CCB_XPATH_MAPPING = "parameter_set_mapping/mapping"
_CCB_XPATH_PROPERTY_PARAMETER_SET = "property_activation/parameter_set"
#------------------------------------------------------------------------------ 
_CCB_ATTR_VISIBILTY_LOGIC = "visibility_logic"
_CCB_ATTR_ACTIVATION_LOGIC = "activation_logic"
_CCB_ATTR_VAR_NAME = "var_name"
_CCB_ATTR_NAME = "name"
_CCB_ATTR_ARRAY_INDEX = "array_index"
_CCB_ATTR_INDEX_START = "index_start"
_CCB_ATTR_INDEX_END = "index_end"
_CCB_ATTR_PARAM_START_INDEX = "parameter_start_index"
_CCB_ATTR_PARAM_BIT_MSB = "parameter_bit_msb"
_CCB_ATTR_FSIZE = "fsize"
_CCB_ATTR_SPLIT_MSB = "split_msb"
_CCB_ATTR_SPLIT_LSB = "split_lsb"
_CCB_ATTR_GROUP_NUM = "group_num"
_CCB_ATTR_DEFAULT = "default"
_CCB_ATTR_RELEASE_DETAIL="release_detail"
#===============================================================================
# CLass that uses various functions to create and generate code
#===============================================================================
class SiRadioApiPyGen(object):
    
    def __init__(self, root, args):
        self.args = args
        self.root = root
        self.visibility = args.visibility
        self.command_sets = self.root.xpath(_CCB_XPATH_COMMAND_SET)
        self.property_groups = self.root.xpath(_CCB_XPATH_PROPERTY_GROUP)
        self.cmd_idx_width = self.root.xpath(_CCB_XPATH_COMMAND_SETS)[0].get("command_index_width")
        self.cmd_param_width = self.root.xpath(_CCB_XPATH_COMMAND_SETS)[0].get("command_parameter_width")
        self.cmd_arg_max = self.root.xpath(_CCB_XPATH_COMMAND_SETS)[0].get("command_argument_max")
        self.cmd_reply_max = self.root.xpath(_CCB_XPATH_COMMAND_SETS)[0].get("command_reply_max")
        self.prop_idx_width = self.root.xpath(_CCB_XPATH_PRPOERTY_GROUPS)[0].get("property_index_width")
        self.prop_param_width = self.root.xpath(_CCB_XPATH_PRPOERTY_GROUPS)[0].get("property_group_width")
        self.prop_grp_width = self.root.xpath(_CCB_XPATH_PRPOERTY_GROUPS)[0].get("property_parameter_width")
        self._assign_var_dicts_defaults()
        self.radio_api_data = []

    #===========================================================================
    # Private methods
    #===========================================================================
    def _assign_var_dicts_defaults(self):        
        """
        Apply a default value to all feature/mode name values and equivalent
        feature/mode eval variables.
        """
        self.all_feature_vars = {} # feature readable_name values 
        self.all_mode_vars = {}    # mode readable_name values
        self.eval_vars = {}        # feature and mode var_name values
        default_value = False
        # set feature keys for all features
        for feature in self.root.xpath("/root/filter_variables/features/feature"):
            # name
            readable_name = feature.get('name')
            assert readable_name not in self.all_feature_vars, \
                "ERROR: Duplicate feature readable_name {0}".format(readable_name)
            self.all_feature_vars[readable_name] = default_value 
            # var_name
            var_name = feature.get('var_name')
            assert var_name not in self.eval_vars, \
                "ERROR: Duplicate feature var_name {0}".format(var_name)
            self.eval_vars[var_name] = default_value 
            
        # set mode keys for all modes
        for mode in self.root.xpath("/root/filter_variables/modes/mode"):
            # name
            readable_name = mode.get('name')
            assert readable_name not in self.all_mode_vars, \
                "ERROR: Duplicate mode readable_name {0}".format(readable_name)
            self.all_mode_vars[readable_name] = default_value
            # var_name
            var_name = mode.get('var_name')
            assert var_name not in self.eval_vars, \
                "ERROR: Duplicate mode var_name {0}".format(var_name)
            self.eval_vars[var_name] = default_value 

    def _assign_var_dicts_for_part(self, part):
        """
        For this part instance, set the feature/mode name variables and 
        feature/mode eval variables to True and all other feature/mode 
        variables to False.
        """
        self._assign_var_dicts_defaults()        

        # all features in this part
        for feature in part.xpath(_CCB_XPATH_FEATURE):
            readable_name = feature.get('name')
            self.all_feature_vars[readable_name] = True
            var_name = feature.get('var_name')
            self.eval_vars[var_name] = True
        
        # all modes in this part
        for mode in part.xpath(_CCB_XPATH_MODE):
            readable_name = mode.get('name')
            self.all_mode_vars[readable_name] = True
            var_name = mode.get('var_name')
            self.eval_vars[var_name] = True
    
    def _evaluate_activation_logic(self, activation_logic):
        """
        Return True if ignore_activation_logic is set or the activation_logic 
        argument is empty; otherwise, evaluate the activation_logic string 
        and return the result.
        """
        # return true for empty string
        if len(activation_logic) == 0:
            return True
        eval_str = activation_logic
        # switch to Python boolean operators
        for (op, py_op) in (('!', ' not '), ('&&', ' and '), ('||', ' or ')):
            eval_str = eval_str.replace(op, py_op)
        # TODO: guard against malicious code in eval() call?
        result = eval(eval_str, self.eval_vars)
        return result

    def _write_to_file(self, path, lines):
        with open(path, 'w') as f:
            f.write(lines)
            f.write("\n")
        
    def _underscore_them(self, name_list):
        return "_".join(name_list)
    
    def _make_name_valid(self, name):
        """
        This method turned is necessary because some API string literals start
        with a numeric character. Given that he code generation could use this name
        as the name of an instance object and given that an instance object name
        cannot start with a numeric character, the code generator prepends a 
        ``M_`` to the literal that comes from the Object API xml. 
        """
        new_name = name
        if name[:1].isdigit():
            new_name = "M_" + name
        return new_name
        
    def _is_visible(self, item):
        if self.visibility == 'public':
            return item.get(_CCB_ATTR_VISIBILTY_LOGIC) in ['', 'access_PUBLIC']
        elif self.visibility == 'factory':
            return item.get(_CCB_ATTR_VISIBILTY_LOGIC) in ['', 'access_PUBLIC', 'access_FACTORY']
        else:
            raise RuntimeError("Invalid value for visibility", self.visibility)
    
    def _is_activated(self, item):
        return self._evaluate_activation_logic(item.get(_CCB_ATTR_ACTIVATION_LOGIC))
    
    def _is_activated_and_public(self, item):
        return (self._is_visible(item) and self._is_activated(item))
        
    def _is_command_set_empty(self, command_set):
        empty = True
        for command in command_set.xpath(_CCB_XPATH_COMMAND):
            if self._is_activated_and_public(command):
                empty = False
                break
        return empty
    
    def _is_parameter_set_empty(self, parameter_set):
        empty = True
        number_of_fields = 0
        for field in parameter_set.xpath(_CCB_XPATH_FIELD):
            number_of_fields = 1
            if self._is_activated_and_public(field):
                empty = False
                break
        if number_of_fields == 0:
            # This check is used to differentiate those cases where
            # the parameter set has no fields, e.g. CMD
            empty = False
        return empty
    
    def _is_property_activated_and_public(self, prop):
        prop_is_activated = False
        for prop_activation in prop.xpath(_CCB_XPATH_PROPERTY_ACTIVATION):
            if self._is_activated(prop_activation):
                prop_is_activated = True
                break
        return (self._is_visible(prop) and prop_is_activated)
    
    def _is_property_group_empty(self, property_group):
        empty = True
        for prop in property_group.xpath(_CCB_XPATH_PROPERTY):
            if self._is_property_activated_and_public(prop):
                empty = False
                break
        if empty:
            for prop_array in property_group.xpath(_CCB_XPATH_PROPERTY_ARRAY):
                for prop in prop_array.xpath(_CCB_XPATH_PROPERTY):
                    if self._is_property_activated_and_public(prop):
                        empty = False
                        break
        return empty

    def _get_filter_vars_list(self, part):
        part_filter_vars_list = ['']
        for part_feature in part.xpath(_CCB_XPATH_FEATURE):
            part_filter_vars_list.append(part_feature.get(_CCB_ATTR_VAR_NAME))
        for part_modes in part.xpath(_CCB_XPATH_MODE):
            part_filter_vars_list.append(part_modes.get(_CCB_ATTR_VAR_NAME))
        return part_filter_vars_list                     
        
    def _part_name(self, part):
        return (part.get(_CCB_ATTR_NAME) + "_" + part.get(_CCB_ATTR_RELEASE_DETAIL)).lower()
    
    def _generate_py_class(self, part):
        self._generate_class(part)
        
    def _generate_class(self, part):
        self.radio_api_data = []
        part_name = self._make_name_valid(self._part_name(part)).lower()
        if self.visibility == "factory":
            part_name += "_factory"
        print("Processing '{0}'...".format(part_name))

        self._assign_var_dicts_for_part(part)

        self.radio_api_data.append("part={}".format(part_name))
        features = []
        for k, v in self.all_feature_vars.items():
            if v:
                features.append(k)
        self.radio_api_data.append("features={}".format(",".join(sorted(features))))
        modes = []
        for k, v in self.all_mode_vars.items():
            if v:
                modes.append(k)
        self.radio_api_data.append("modes={}".format(",".join(sorted(modes))))
        self._generate_init(part_name)
        self._generate_command_set_blocks()
        self._generate_property_group_blocks()
        
        self._write_to_file(os.path.join(self.args.out_dir, "{}.properties".format(part_name)), "\n".join(self.radio_api_data))
        print("Done with '{0}'...".format(part_name))
        
    def _generate_init(self, part_name):
        self.radio_api_data.append("commands=[{},{},{},{}]".format(self.cmd_idx_width, self.cmd_param_width, self.cmd_arg_max, self.cmd_reply_max))
        self.radio_api_data.append("properties=[{},{},{},{}]".format(self.prop_idx_width, self.prop_param_width, self.prop_grp_width, 0))
        
    def _generate_command_set_blocks(self):
        command_sets = []
        for cmd_set in self.command_sets:
            if self._is_command_set_empty(cmd_set) == False:
                cmd_set_name = self._make_name_valid(cmd_set.get(_CCB_ATTR_NAME))
                command_sets.append(cmd_set_name)
        self.radio_api_data.append("command_sets={}".format(",".join(command_sets)))
        for cmd_set in self.command_sets:
            if self._is_command_set_empty(cmd_set) == False:
                cmd_set_name = self._make_name_valid(cmd_set.get(_CCB_ATTR_NAME))
                commands = []
                for cmd_item in cmd_set.xpath(_CCB_XPATH_COMMAND):
                    if self._is_activated_and_public(cmd_item):
                        cmd_name = self._make_name_valid(cmd_item.get(_CCB_ATTR_NAME))
                        commands.append(cmd_name)
                self.radio_api_data.append("{}={}".format(cmd_set_name, ",".join(commands)))
        for cmd_set in self.command_sets:
            if self._is_command_set_empty(cmd_set) == False:
                cmd_set_name = self._make_name_valid(cmd_set.get(_CCB_ATTR_NAME))
                self._generate_command_blocks(cmd_set, cmd_set_name)
    
    def _generate_command_blocks(self, cmd_set, cmd_set_name):        
        for cmd_item in cmd_set.xpath(_CCB_XPATH_COMMAND):
            cmd_name = self._make_name_valid(cmd_item.get(_CCB_ATTR_NAME))
            cmd_val = cmd_item.get(_CCB_XPATH_NUMBER)
            if self._is_activated_and_public(cmd_item):
                self.radio_api_data.append("{}.{}={}".format(cmd_set_name, cmd_name, cmd_val))
                for xpath_prefix in ["inputs", "outputs"]:
                    arguments = []
                    for param_set in cmd_item.xpath(_CCB_XPATH_PARAMETER_SET.format(xpath_prefix)):
                        param_set_name = self._make_name_valid(param_set.get(_CCB_ATTR_NAME))
                        if self._is_parameter_set_empty(param_set):
                            param_idx_start = param_set.get(_CCB_ATTR_INDEX_START)
                            need_reserved_param_set = False
                            for field in param_set.xpath(_CCB_XPATH_FIELD):
                                if self._is_activated(field) == False:
                                    # need to add RESERVED parameter_set
                                    need_reserved_param_set = True
                                    break
                            if need_reserved_param_set:
                                param_set_name = "RESERVED_{}".format(int(param_idx_start, 0))
                            else:
                                continue
                        arguments.append(param_set_name)
                    for param_set_array in cmd_item.xpath(_CCB_XPATH_PARAMETER_SET_ARRAY.format(xpath_prefix)):
                        arguments.append(param_set_array.get(_CCB_ATTR_NAME))
                    if arguments:
                        self.radio_api_data.append("{}.{}.{}={}".format(cmd_set_name, cmd_name, xpath_prefix, ",".join(arguments)))
                    self._generate_parameter_set_blocks(cmd_item.xpath(_CCB_XPATH_PARAMETER_SET.format(xpath_prefix)),
                                                        cmd_set_name, cmd_name, xpath_prefix)
                    for param_set_array in cmd_item.xpath(_CCB_XPATH_PARAMETER_SET_ARRAY.format(xpath_prefix)):
                        self._generate_parameter_set_array_block(param_set_array, cmd_set_name, cmd_name, xpath_prefix)
                        self._generate_parameter_set_blocks(param_set_array,
                                                            cmd_set_name, cmd_name, xpath_prefix)
    
    def _generate_parameter_set_array_block(self, param_set_array, cmd_set_name, cmd_name, param_set_array_type):
        fixed_size = (False, True)[param_set_array.get("len_type") == "static"]
        item_byte_len = param_set_array.get("item_byte_len")
        param_set_array_name = param_set_array.get(_CCB_ATTR_NAME)
        index_start = param_set_array.get(_CCB_ATTR_INDEX_START)
        index_end = param_set_array.get(_CCB_ATTR_INDEX_END)
        self.radio_api_data.append("{}.{}.{}.{}.ParameterSetArray=[{},{},{},{}]".format(cmd_set_name, cmd_name, param_set_array_type, param_set_array_name,
                                                                                     index_start, index_end, fixed_size, item_byte_len))

    def _generate_parameter_set_blocks(self, parameter_set_collection, cmd_set_name, cmd_name, param_set_type="inputs", param_set_name_or_none = None):
        for param_set in parameter_set_collection:
            array_idx = param_set.get(_CCB_ATTR_ARRAY_INDEX)
            param_set_name = self._make_name_valid(param_set.get(_CCB_ATTR_NAME))
            if (param_set_name_or_none):
                # TODO: hacky way of handling per-mode activation
                param_set_name = param_set_name_or_none
            param_idx_start = param_set.get(_CCB_ATTR_INDEX_START)
            param_idx_end = param_set.get(_CCB_ATTR_INDEX_END)
            if self._is_parameter_set_empty(param_set):
                need_reserved_param_set = False
                for field in param_set.xpath(_CCB_XPATH_FIELD):
                    if self._is_activated(field) == False:
                        # need to add RESERVED parameter_set
                        need_reserved_param_set = True
                        break
                if need_reserved_param_set:
                    param_set_name = "RESERVED_{}".format(int(param_idx_start, 0))
                else:
                    continue
            if array_idx is None:
                self.radio_api_data.append("{}.{}.{}.{}=[{},{}]".format(cmd_set_name, cmd_name, param_set_type, param_set_name, param_idx_start, param_idx_end))
            field_list = []
            for field in param_set.xpath(_CCB_XPATH_FIELD):
                if self._is_activated_and_public(field):
                    field_name = self._make_name_valid(field.get(_CCB_ATTR_NAME))
                    field_list.append(field_name)
                    field_size = field.get(_CCB_FIELD_SIZE)
                    field_default = field.get("value_default")
                    self.radio_api_data.append("{}.{}.{}.{}.fields.{}=[{},{}]".format(cmd_set_name, cmd_name, param_set_type, param_set_name, field_name,
                                                                                      field_size, (0, field_default)[field_default != ""]))
                    idx = 0
                    mapping_list = []
                    for mapping in field.xpath(_CCB_XPATH_MAPPING):
                        self.radio_api_data.append("{}.{}.{}.{}.fields.{}.mappings.{}=[{},{},{},{},{}]".format(cmd_set_name,
                                                                                                               cmd_name,
                                                                                                               param_set_type,
                                                                                                               param_set_name,
                                                                                                               field_name,
                                                                                                               idx,
                                                                                                               mapping.get(_CCB_ATTR_PARAM_START_INDEX),
                                                                                                               mapping.get(_CCB_ATTR_PARAM_BIT_MSB),
                                                                                                               mapping.get(_CCB_ATTR_FSIZE),
                                                                                                               mapping.get(_CCB_ATTR_SPLIT_MSB),
                                                                                                               mapping.get(_CCB_ATTR_SPLIT_LSB)))
                        mapping_list.append("{}".format(idx))
                        idx += 1
                    self.radio_api_data.append("{}.{}.{}.{}.fields.{}.mappings={}".format(cmd_set_name, cmd_name, param_set_type, param_set_name, field_name,
                                                                                          ",".join(mapping_list)))
            if field_list:
                self.radio_api_data.append("{}.{}.{}.{}.fields={}".format(cmd_set_name, cmd_name, param_set_type, param_set_name, ",".join(field_list)))
            if array_idx:
                break
            if param_set_name_or_none:
                break

    def _generate_property_group_blocks(self):
        prop_group_list = []
        for prop_group in self.property_groups:
            prop_group_name = self._make_name_valid(prop_group.get(_CCB_ATTR_NAME))
            if self._is_property_group_empty(prop_group) == False:
                prop_group_list.append(prop_group_name)
        self.radio_api_data.append("property_groups={}".format(",".join(prop_group_list)))
        for prop_group in self.property_groups:
            prop_group_name = self._make_name_valid(prop_group.get(_CCB_ATTR_NAME))
            prop_group_number = prop_group.get(_CCB_ATTR_GROUP_NUM)
            if self._is_property_group_empty(prop_group) == False:
                self.radio_api_data.append("{}={}".format(prop_group_name, prop_group_number))
                prop_list = []
                self._generate_call_to_init_property(prop_group_name, prop_group.xpath(_CCB_XPATH_PROPERTY), prop_list)
                for prop_array in prop_group.xpath(_CCB_XPATH_PROPERTY_ARRAY):
                    self._generate_call_to_init_property(prop_group_name, prop_array.xpath(_CCB_XPATH_PROPERTY), prop_list)
                self.radio_api_data.append("{}.Properties={}".format(prop_group_name, ",".join(prop_list)))
        for prop_group in self.property_groups:
            if self._is_property_group_empty(prop_group) == False:
                prop_group_name = self._make_name_valid(prop_group.get(_CCB_ATTR_NAME))
                prop_group_number = prop_group.get(_CCB_ATTR_GROUP_NUM)
                self._generate_property_blocks(prop_group_name, prop_group.xpath(_CCB_XPATH_PROPERTY), prop_group_number)
                for prop_array in prop_group.xpath(_CCB_XPATH_PROPERTY_ARRAY):
                    self._generate_property_blocks(prop_group_name, prop_array.xpath(_CCB_XPATH_PROPERTY), prop_group_number)
    
    def _generate_call_to_init_property(self, prop_group_name, property_collection, prop_list):
        for prop in property_collection:
            if self._is_property_activated_and_public(prop):
                prop_name = self._make_prop_name(prop)
                prop_list.append(prop_name)
        
    def _generate_property_blocks(self, prop_group_name, property_collection, prop_group_number):
        for prop in property_collection:
            if self._is_property_activated_and_public(prop):
                prop_name = self._make_prop_name(prop)
                prop_index_start = prop.get(_CCB_ATTR_INDEX_START)
                prop_index_end =  prop.get(_CCB_ATTR_INDEX_END)
                prop_default_value = prop.xpath(_CCB_XPATH_PROPERTY_ACTIVATION)[0].get(_CCB_ATTR_DEFAULT)
                self.radio_api_data.append("{}.{}=[{},{},{},{}]".format(prop_group_name, 
                                                                        prop_name, 
                                                                        prop_index_start, 
                                                                        prop_index_end, 
                                                                        prop_default_value, 
                                                                        prop_group_number))
                param_set_name_or_none = None
                for param_set in prop.xpath(_CCB_XPATH_PROPERTY_PARAMETER_SET):
                    param_set_name = self._make_name_valid(param_set.get(_CCB_ATTR_NAME))
                    if param_set_name == "SIMPLE_PROP_ACT" or param_set_name == "COMPLEX_PROP_ACT":
                        # TODO: hacky way of handling per-mode activation
                        param_set_name = "_".join(prop_name.split("_")[1:])
                        param_set_name_or_none = param_set_name
                    if self._is_parameter_set_empty(param_set):
                        param_idx_start = param_set.get(_CCB_ATTR_INDEX_START)
                        need_reserved_param_set = False
                        for field in param_set.xpath(_CCB_XPATH_FIELD):
                            if self._is_activated(field) == False:
                                # need to add RESERVED parameter_set
                                need_reserved_param_set = True
                                break
                        if need_reserved_param_set:
                            param_set_name = "RESERVED_{}".format(int(param_idx_start, 0))
                        else:
                            continue
                    self.radio_api_data.append("{}.{}.ParameterSet={}".format(prop_group_name, prop_name, param_set_name))
                    break
#                 TODO: Figure out a way to deal with mode-specific parameter sets. For now,
#                       using just one doesn't really affect the chip configurator as it behaves.
#                       It basically does not do anything based on the mode, but it still operates
#                       with the superset of data for part over all modes, and know about only one default.  
                self._generate_parameter_set_blocks(prop.xpath(_CCB_XPATH_PROPERTY_PARAMETER_SET),
                                                    prop_group_name,
                                                    prop_name,
                                                    "ParameterSet",
                                                    param_set_name_or_none)
                
    
    def _make_prop_name(self, prop):
        prop_name = self._make_name_valid(prop.get(_CCB_ATTR_NAME))
        prop_idx = prop.get(_CCB_ATTR_ARRAY_INDEX)
        if not(prop_idx == None):
            #===========================================================
            # The array_index attribute indicates that the property
            # belongs to a property array, and it's the index at which
            # this property resides in the array.
            # The array_index attribute is a hex number string. The 
            # code generator uses the formatted decimal value to append
            # the index of the property to the property name.
            #===========================================================
            prop_name = self._underscore_them([prop_name, "{}".format(int(prop_idx,0))])
        return prop_name
    
    #===========================================================================
    # Public methods 
    #===========================================================================
    def generate_package(self):
        for partgroup in self.root.xpath(_CCB_XPATH_PART_GROUP):
            for part in partgroup.xpath("part"):
                self._generate_py_class(part)
                
def main():
    parser = argparse.ArgumentParser(description="Generate Chip Configurator Python source from APIXML object instance")
    parser.add_argument('xml_filename', type=str, help='The XML file')
    parser.add_argument('out_dir', type=str, help='The output folder to store Python source')
    # TODO: Add optional or required argument for schema path
    parser.add_argument('--schema', type=str, help='The schema file for validation')
    parser.add_argument('--visibility', default='public', type=str, help='public|factory. Default is public')
    args = parser.parse_args()
    if not os.path.exists(args.out_dir):
        os.mkdir(args.out_dir)
    if not os.path.exists(args.xml_filename):
        sys.stderr.write("FATAL ERROR: Unable to open '%s'\n" % \
                         args.xml_filename)
        sys.exit(1)
    print("Processing '{}' into source in '{}'...\n".format(args.xml_filename,
                                                            os.path.abspath(args.out_dir)))
    root = etree.parse(args.xml_filename)
    # TODO: Add schema validation
    if args.schema:
        xmlschema_doc = etree.parse(args.schema)
        xmlschema = etree.XMLSchema(xmlschema_doc)
        xmlschema.assertValid(xmlschema_doc)
    ccb = SiRadioApiPyGen(root, args)
    ccb.generate_package()
    print("Done!")
        
if __name__ == '__main__':
    main()