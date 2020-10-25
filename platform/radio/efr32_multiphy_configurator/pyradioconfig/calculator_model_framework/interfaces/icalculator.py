from abc import ABCMeta, abstractmethod
import inspect
import types
from pyradioconfig.calculator_model_framework.Utils.Version import Version
from pycalcmodel.core.variable import ModelVariable
from pyradioconfig.calculator_model_framework.Utils.LogMgr import LogMgr

"""
Calculations interface file
"""
class ICalculator(object):

    _major = 0
    _minor = 0
    _patch = 0

    # Static variables
    actual_suffix = "_actual"

    """
    Returns profile version
    """
    @abstractmethod
    def getVersion(self):
        return Version(self._major, self._minor, self._patch)

    """
    Returns list of callable calc functions
    """
    @abstractmethod
    def getCalculationList(self):
        functionList = list()
        for functionName, functionPointer in inspect.getmembers(self, predicate=inspect.ismethod):
                if str(functionName).lower().startswith('calc_'):  # Check if the function name starts with "calc_"
                    if isinstance(functionPointer,types.MethodType):
                        #functionList.append(self.__class__.__name__ + "." + functionName)
                        functionList.append(functionPointer)
        return functionList

    """
    Populates a list of needed variables for this calculator
    """
    def buildVariables(self, modem_model):
        raise NotImplementedError('Call to abstract method getName()')

    """
    Adds variable to modem model
    """
    @abstractmethod
    def _addModelVariable(self, modem_model, name, var_type, format, desc=None, is_array=False,
                          forceable=True, units=None):
        var = ModelVariable(name, var_type, is_array, forceable=forceable)
        var.format = format

        if desc is not None:
            var.desc = desc

        if units is not None:
            var.units = units

        modem_model.vars.append(var)
        return var

    """
    Adds svd variable to modem model
    """
    @abstractmethod
    def _addModelRegister(self, modem_model, svd_mapping, var_type, format, desc=None, is_array=False,
                          forceable=True):
        name = svd_mapping.replace('.','_')
        var = ModelVariable(name, var_type, is_array, forceable=forceable)
        var.format = format

        if desc is not None:
            var.desc = desc

        var.svd_mapping = svd_mapping

        modem_model.vars.append(var)
        return var

        
    """
    Adds reverse path "actual" variable to modem model
    
    A helper function is used to define these so we can be assured of at least having
    a specific naming convention.  Today we're only identifying reverse path 
    variables via the "actual" in the name.  By using this function, we could also
    add other flags, like setting a "don't allow this variable to be forced" bit.
    """
    @abstractmethod
    def _addModelActual(self, modem_model, name, var_type, format, desc=None, is_array=False):
        name_actual = name + self.actual_suffix
        var = ModelVariable(name_actual, var_type, is_array, forceable=False)
        var.format = format

        if desc is not None:
            var.desc = desc

        modem_model.vars.append(var)
        return var


    """
    Calculator Helper function
    """
    def _reg_write(self, varname, value):
        varname.value = value

    def _reg_write_default(self, varname, part_family):
        varname.set_to_reset_val(part_family)

    def _reg_do_not_care(self, varname):
        varname.value_do_not_care = True

    def _reg_limit_write(self, varname, value, value_limit):
        if value > value_limit:
            value = value_limit
            LogMgr.Warning("WARNING: Limit applied when writing to %s" % varname.name)
        varname.value = value

    def _reg_sat_write(self, varname, value, part_family):
        bit_width = varname.get_bit_width(part_family)
        if value > (pow(2, bit_width) - 1):
            value = pow(2, bit_width) - 1
            LogMgr.Warning("WARNING: Saturation applied when writing to %s" % varname.name)
        varname.value = value

