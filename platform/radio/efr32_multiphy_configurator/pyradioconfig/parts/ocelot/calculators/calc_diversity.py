from pyradioconfig.parts.common.calculators.calc_diversity import CALC_Diversity
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from enum import Enum
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException

class CALC_Diversity_Ocelot(CALC_Diversity):

    def buildVariables(self, model):
        """Populates a list of needed variables for this calculator

        Args:
            model (ModelRoot) : Builds the variables specific to this calculator
        """

        # Input variables

        # Internal variables
        # Must exposing the forced selection of a antenna 0 only as DISABLE enum
        var = self._addModelVariable(model, 'antdivmode', Enum, ModelVariableFormat.DECIMAL, 'Antenna diversity mode')
        member_data = [
            ['DISABLE', 0, 'Antenna 0 used'],
            ['ANTENNA1', 1, 'Antenna 1 is used'],
            ['ANTSELFIRST', 2, 'Select-First algorithm'],
            ['ANTSELCORR', 3, 'Select-Best algorithm based on correlation'],
            ['ANTSELRSSI', 4, 'Select-Best algorithm based on RSSI value'],
            ['PHDEMODANTDIV', 5, 'Select PHASE Demod ANT-DIV algorithm'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'AntDivModeEnum',
            'List of supported antenna diversity mode',
            member_data)

        # Bools not allowed as advanced inputs due to GUI constraint. Using enum instead
        var = self._addModelVariable(model, 'antdivrepeatdis', Enum, ModelVariableFormat.DECIMAL,
                                     'Repeated measurement of first antenna when Select-Best algorithm is used')
        member_data = [
            ['REPEATFIRST', 0, 'Enable repeated measurement of first antenna'],
            ['NOREPEATFIRST', 1, 'Disable repeated measurement of first antenna'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'AntDivRepeatDisModeEnum',
            'Enable or disable repeated measurement of first antenna',
            member_data)

        # Output software variables for RAIL to consume
        self._addModelVariable(model, 'div_antdivmode', int, ModelVariableFormat.DECIMAL, 'Antenna diversity mode')
        self._addModelVariable(model, 'div_antdivrepeatdis', int, ModelVariableFormat.DECIMAL,
                               'Repeated measurement of first antenna when Select-Best algorithm is used')

        # Calculation variable for reset period
        if model.part_family.lower() in ["jumbo", "nerio", "nixi"]:
            self._addModelVariable(model, 'div_demod_reset_period_hemi_usec', int, ModelVariableFormat.DECIMAL,
                                   'Sequencer FW issues a reset to demod at this interval. Used in antenna diversity.')

    def calc_diversity_values(self, model):
        """apply inputs to antenna diversity output software variables for RAIL to consume
        TODO: consider preamble length--should be long enough to allow the diversity search algorithm to make a proper antenna selection
        Not handled here: MODEM_ROUTEPEN: ANT0PEN, ANT1PEN to enable these pins, and MODEM_ROUTELOC1 to route to GPIO

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # default value 0 ANTENNA0 Antenna 0 (ANT0=1, ANT1=0) is used. This is not exposed as one of the enums, so use 0 here.
        # default ANTDIVREPEATDIS 0
        model.vars.antdivmode.value = model.vars.antdivmode.var_enum.DISABLE
        model.vars.antdivrepeatdis.value = model.vars.antdivrepeatdis.var_enum.REPEATFIRST

        # # unless set otherwise by advanced inputs
        antdivmode = model.vars.antdivmode.value
        antdivrepeatdis = model.vars.antdivrepeatdis.value


        model.vars.div_antdivmode.value = int(antdivmode)
        model.vars.div_antdivrepeatdis.value = int(antdivrepeatdis)

    def calc_antdivmode_reg(self, model):

        antdivmode = model.vars.antdivmode.value

        if antdivmode == model.vars.antdivmode.var_enum.DISABLE:
            reg = 0
        elif antdivmode == model.vars.antdivmode.var_enum.ANTENNA1:
            reg = 1
        elif antdivmode == model.vars.antdivmode.var_enum.ANTSELFIRST:
            reg = 2
        elif antdivmode == model.vars.antdivmode.var_enum.ANTSELCORR:
            reg = 3
        elif antdivmode == model.vars.antdivmode.var_enum.ANTSELRSSI:
            reg = 4
        elif antdivmode == model.vars.antdivmode.var_enum.PHDEMODANTDIV:
            reg = 5

        self._reg_write(model.vars.MODEM_CTRL3_ANTDIVMODE, reg)
