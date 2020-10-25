from pyradioconfig.calculator_model_framework.interfaces.icalculator import ICalculator
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from enum import Enum

class CALC_WiSUN_Ocelot(ICalculator):

    def buildVariables(self, model):
        var = self._addModelVariable(model, 'wisun_mode', Enum, ModelVariableFormat.DECIMAL, 'WiSUN Mode')
        member_data = [
            ['Mode1a', 0, '2FSK 50kbps mi=0.5'],
            ['Mode1b', 1, '2FSK 50kbps mi=1.0'],
            ['Mode2a', 2, '2FSK 100kbps mi=0.5'],
            ['Mode2b', 3, '2FSK 100kbps mi=1.0'],
            ['Mode3',  4, '2FSK 150kbps mi=0.5'],
            ['Mode4a', 5, '2FSK 200kbps mi=0.5'],
            ['Mode4b', 6, '2FSK 200kbps mi=1.0'],
            ['Mode5',  7, '2FSK 300kbps mi=0.5']
            ]
        var.var_enum = CreateModelVariableEnum(
            'WiSUNModeEnum',
            'List of supported WiSUN Modes',
            member_data)