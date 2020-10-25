'''
Created on May 5, 2014

@author: sesuskic
'''
import ast
from .commands import Commands
from .commandset import CommandSet
from .command import Command, ArgumentStream
from .parameterset import ParameterSet, ParameterSetArray
from .field import Field
from .mapping import Mapping
from .properties import Properties
from .propertygroup import PropertyGroup
from .property import Property

class SiRadioApi(object):
    def __init__(self, params):
        self.part_name = params["part"]
        self.features = params["features"].split(",")
        self.modes = params["modes"].split(",")
        self.commands = Commands(params["part"] + "_commands", ast.literal_eval(params["commands"]))
        self._init_commands(params)
        self.properties = Properties(params["part"] + "_properties", ast.literal_eval(params["properties"]))
        self._init_properties(params)
        
    def _init_fields(self, params, paramSet, paramSetKey, paramSetInitList):
        fieldsKey = paramSetKey + ".fields"
        if fieldsKey in params:
            for field in params[fieldsKey].split(","):
                fieldKey = fieldsKey + "." + field
                f = Field(field, ast.literal_eval(params[fieldKey]))
                mappingsKey = fieldKey + ".mappings"
                for mapping in params[mappingsKey].split(","):
                    mappingKey = mappingsKey + "." + mapping
                    m = Mapping(ast.literal_eval(params[mappingKey]) + paramSetInitList)
                    f.add_mapping(m)
                paramSet.add_field(f)
    
    def _init_commands(self, params):
        for cs in params["command_sets"].split(","):
            commandSet = CommandSet(cs)
            for c in params[cs].split(","):
                cmdKey = cs + "." + c
                command = Command(c, ast.literal_eval(params[cmdKey]))
                for args in ["inputs", "outputs"]:
                    argsKey  = cmdKey + "." + args
                    argStream = ArgumentStream()
                    if args == "outputs" and c in ["WRITE_TX_FIFO", "EZCONFIG_ARRAY_WRITE"]:
                        # ugly hack
                        pass
                    else:
                        for  argStreamKey in params[argsKey].split(","):
                            paramSetKey = argsKey + "." + argStreamKey
                            paramSetArrayKey = paramSetKey + "." + "ParameterSetArray"
                            if paramSetArrayKey in params:
                                paramSetArrayInitList = ast.literal_eval(params[paramSetArrayKey])
                                paramSetArray = ParameterSetArray(argStreamKey, paramSetArrayInitList)
                                idx = 0
                                for element in range(paramSetArrayInitList[0], paramSetArrayInitList[1] + 1, paramSetArrayInitList[3]):
                                    paramSet = ParameterSet(argStreamKey + "_{:d}".format(idx), [element, element + paramSetArrayInitList[3] - 1])
                                    self._init_fields(params, paramSet, paramSetKey, [element, element + paramSetArrayInitList[3] - 1])
                                    paramSetArray.add_parameter_set(paramSet)
                                    for field in paramSet.field_list:
                                        for mapping in field.mappings:
                                            mapping.parameter_start_idx += idx
                                    idx += 1
                                argStream.add_parameter_set(paramSetArray)
                            else:
                                paramSet = ParameterSet(argStreamKey, ast.literal_eval(params[paramSetKey]))
                                self._init_fields(params, paramSet, paramSetKey, ast.literal_eval(params[paramSetKey]))
                                argStream.add_parameter_set(paramSet)
                        if args == "inputs":
                            command.add_inputs(argStream)
                        elif args =="outputs":
                            command.add_outputs(argStream)
                commandSet.add_command(command) 
            self.commands.add_command_set(commandSet)

    def _init_properties(self, params):
        for pg in params["property_groups"].split(","):
            propertyGroup = PropertyGroup(pg, ast.literal_eval(params[pg]))
            propGroupKey = pg + "." + "Properties"
            for p in params[propGroupKey].split(","):
                propKey = pg + "." + p
                prop = Property(p, ast.literal_eval(params[propKey]))
                propParamSetKey = propKey + "." + "ParameterSet"
                if propParamSetKey in params:
                    for paramSet in params[propParamSetKey].split(","):
                        paramSetKey = propParamSetKey + "." + paramSet
                        paramSet = ParameterSet(paramSet, ast.literal_eval(params[paramSetKey]))
                        self._init_fields(params, paramSet, paramSetKey, ast.literal_eval(params[paramSetKey]))
                        prop.add_parameter_set(paramSet)
                    propertyGroup.add_property(prop)
            self.properties.add_property_group(propertyGroup)