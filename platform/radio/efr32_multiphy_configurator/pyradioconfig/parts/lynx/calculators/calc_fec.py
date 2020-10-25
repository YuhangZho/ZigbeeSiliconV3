from pyradioconfig.parts.common.calculators.calc_fec import CALC_FEC
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from enum import Enum

class CALC_FEC_Lynx(CALC_FEC):

    def __init__(self):
        #Call init of parent class
        super().__init__()

    def _build_fec_var(self, model):
        # This function adds the fec_en enumerated variable. It is a separate method to allow overwriting in future parts.

        var = self._addModelVariable(model, 'fec_en', Enum, ModelVariableFormat.DECIMAL,
                                     'List of supported FEC Configurations')
        member_data = [
            ['NONE', 0, 'No FEC'],
            ['FEC_154G', 1, '(legacy not recommended) 15.4G FEC settings'],
            ['FEC_154G_K7', 2, '(legacy not recommended) 15.4G FEC settings with K=7'],
            ['FEC_154G_NRNSC_INTERLEAVING',  3, '15.4G FEC settings with NRNSC and interleaving'],
            ['FEC_154G_RSC_INTERLEAVING', 4, '15.4G FEC settings with RSC and interleaving'],
            ['FEC_154G_RSC_NO_INTERLEAVING', 5, '15.4G FEC settings with RSC and no interleaving'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'FECEnum',
            'List of supported FEC Configurations',
            member_data)

    def calc_fecctrl_convinv_reg(self, model):
        #This function calculates the FRC_FECCTRL_CONVINV reg field

        #Read in model variables
        fec_en = model.vars.fec_en.value

        #Calculate the output bits to invert based on the fec_en mode
        if fec_en == model.vars.fec_en.var_enum.FEC_154G_NRNSC_INTERLEAVING:
            convinv = 3 #Invert both bits
        else:
            convinv = 0

        #Write the register
        self._reg_write(model.vars.FRC_FECCTRL_CONVINV, convinv)

    def calc_feccrl_interleavemode_reg(self, model):
        # This function calculates the FRC_FECCTRL_INTERLEAVEMODE reg field

        # Read in model variables
        fec_en = model.vars.fec_en.value

        if (fec_en == model.vars.fec_en.var_enum.FEC_154G_NRNSC_INTERLEAVING) or \
                (fec_en == model.vars.fec_en.var_enum.FEC_154G_RSC_INTERLEAVING) or \
                (fec_en == model.vars.fec_en.var_enum.FEC_154G) or \
                (fec_en == model.vars.fec_en.var_enum.FEC_154G_K7):
            interleavemode = 1
        else:
            interleavemode = 0

        # Load value into register
        self._reg_write(model.vars.FRC_FECCTRL_INTERLEAVEMODE, interleavemode)

    def calc_convgenerator_recursive_reg(self, model):
        #This function calculates the FRC_CONVGENERATOR_RECURSIVE reg field

        #Read in model variables
        fec_en = model.vars.fec_en.value

        if (fec_en == model.vars.fec_en.var_enum.FEC_154G_RSC_INTERLEAVING) or (fec_en == model.vars.fec_en.var_enum.FEC_154G_RSC_NO_INTERLEAVING):
            recursive = 1
        else:
            recursive = 0

        # Load value into register
        self._reg_write(model.vars.FRC_CONVGENERATOR_RECURSIVE, recursive)