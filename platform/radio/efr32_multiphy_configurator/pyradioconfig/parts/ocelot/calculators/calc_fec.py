from pyradioconfig.parts.lynx.calculators.calc_fec import CALC_FEC_Lynx
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from enum import Enum

class CALC_FEC_Ocelot(CALC_FEC_Lynx):

    def __init__(self):
        #Call init of parent class
        super().__init__()

    def buildVariables(self, model):
        #First build all of the variables from the parent class
        super().buildVariables(model)

        #Now add variables
        var = self._addModelVariable(model, 'fec_tx_enable', Enum, ModelVariableFormat.DECIMAL, 'FEC TX enable')
        member_data = [
            ['DISABLED', 0, 'FEC TX Disabled'],
            ['ENABLED', 1, 'FEC TX Enabled'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'FECTxEnableEnum',
            'FEC TX Enable/Disable Selection',
            member_data)

        var = self._addModelVariable(model, 'dynamic_fec_enable', Enum, ModelVariableFormat.DECIMAL, 'Enable dynamic FEC based on syncword')
        member_data = [
            ['DISABLED', 0, 'Dynamic FEC Disabled'],
            ['ENABLED', 1, 'Dynamic FEC Enabled'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'DynamicFecEnum',
            'Dynamic FEC Enable/Disable Selection',
            member_data)

    def _build_fec_var(self, model):
        # This function adds the fec_en enumerated variable. It is a separate method to allow overwriting in future parts.

        var = self._addModelVariable(model, 'fec_en', Enum, ModelVariableFormat.DECIMAL,
                                     'List of supported FEC Configurations')
        member_data = [
            ['NONE', 0, 'No FEC'],
            ['FEC_154G_NRNSC_INTERLEAVING',  1, '15.4G FEC settings with NRNSC and interleaving'],
            ['FEC_154G_RSC_INTERLEAVING', 2, '15.4G FEC settings with RSC and interleaving'],
            ['FEC_154G_RSC_NO_INTERLEAVING', 3, '15.4G FEC settings with RSC and no interleaving'],
            ['FEC_K7_INTERLEAVING', 4, 'FEC settings with K=7 with interleaving']
        ]
        var.var_enum = CreateModelVariableEnum(
            'FECEnum',
            'List of supported FEC Configurations',
            member_data)

    def calc_fec(self, model):
        """calc_fec

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        model.vars.fec_en.value = model.vars.fec_en.var_enum.NONE  # Calculate a default value for Profiles where this is an advanced input

        # Always initialize FEC regs to these values
        self._calc_init(model)

        # IF FEC is enabled then write the 802.15.4g base values
        if model.vars.fec_en.value != model.vars.fec_en.var_enum.NONE:
            self._FEC_154G_Base(model)

            # : FEC_154G_Base uses K=4 by default. Override code to K=7 case.
            if model.vars.fec_en.value == model.vars.fec_en.var_enum.FEC_K7_INTERLEAVING:
                self._reg_write(model.vars.FRC_CONVGENERATOR_GENERATOR0, 0x6D)
                self._reg_write(model.vars.FRC_CONVGENERATOR_GENERATOR1, 0x4F)

        model.vars.fec_enabled.value = int(model.vars.FRC_FECCTRL_CONVMODE.value != 0)


    def _calc_init(self, model):
        # Need to override this method in order to instead set CONVMODE elsewhere

        self._reg_write(model.vars.FRC_FECCTRL_CONVTRACEBACKDISABLE, 0)
        self._reg_write(model.vars.FRC_FECCTRL_INTERLEAVEFIRSTINDEX, 0)
        self._reg_write(model.vars.FRC_FECCTRL_CONVBUSLOCK, 0)
        self._reg_write(model.vars.FRC_FECCTRL_CONVSUBFRAMETERMINATE, 0)
        self._reg_write(model.vars.FRC_FECCTRL_SINGLEBLOCK, 0)
        self._reg_write(model.vars.FRC_FECCTRL_FORCE2FSK, 0)
        self._reg_write(model.vars.FRC_FECCTRL_CONVHARDERROR, 0)

        self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATA, 0)
        self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATACNT, 0)
        self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATAFORCE, 0)

        self._reg_write(model.vars.FRC_CONVGENERATOR_GENERATOR0, 0)
        self._reg_write(model.vars.FRC_CONVGENERATOR_GENERATOR1, 0)
        self._reg_write(model.vars.FRC_CONVGENERATOR_NONSYSTEMATIC, 0)

        self._reg_write(model.vars.FRC_PUNCTCTRL_PUNCT0, 0)
        self._reg_write(model.vars.FRC_PUNCTCTRL_PUNCT1, 0)

    def _FEC_154G_Base(self, model):
        #Need to override this method in order to instead set CONVMODE elsewhere

        self._reg_write(model.vars.FRC_FECCTRL_CONVTRACEBACKDISABLE, 0)
        self._reg_write(model.vars.FRC_FECCTRL_INTERLEAVEFIRSTINDEX, 0)
        self._reg_write(model.vars.FRC_FECCTRL_CONVBUSLOCK, 0)
        self._reg_write(model.vars.FRC_FECCTRL_CONVSUBFRAMETERMINATE, 0)
        self._reg_write(model.vars.FRC_FECCTRL_SINGLEBLOCK, 0)
        self._reg_write(model.vars.FRC_FECCTRL_FORCE2FSK, 0)
        self._reg_write(model.vars.FRC_FECCTRL_CONVHARDERROR, 0)

        self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATA, 0x0B)
        self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATACNT, 0)
        self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXDATAFORCE, 0)

        self._reg_write(model.vars.FRC_CONVGENERATOR_GENERATOR0, 0x0F)
        self._reg_write(model.vars.FRC_CONVGENERATOR_GENERATOR1, 0x0D)
        self._reg_write(model.vars.FRC_CONVGENERATOR_NONSYSTEMATIC, 0)

        self._reg_write(model.vars.FRC_PUNCTCTRL_PUNCT0, 1)
        self._reg_write(model.vars.FRC_PUNCTCTRL_PUNCT1, 1)

    def calc_feccrl_interleavemode_reg(self, model):
        # This function calculates the FRC_FECCTRL_INTERLEAVEMODE reg field

        # Read in model variables
        fec_en = model.vars.fec_en.value
        fec_enabled = model.vars.fec_enabled.value
        dynamic_fec_enable = (model.vars.dynamic_fec_enable.value == model.vars.dynamic_fec_enable.var_enum.ENABLED)

        if dynamic_fec_enable:
            if fec_enabled:
                if (fec_en == model.vars.fec_en.var_enum.FEC_154G_NRNSC_INTERLEAVING) or \
                (fec_en == model.vars.fec_en.var_enum.FEC_154G_RSC_INTERLEAVING):
                    #We need to turn on interleaving to TX properly
                    interleavemode = 1
                else:
                    #We don't want to turn on interleaving but still want to turn on the buffer for RX
                    interleavemode = 2
            else:
                #We don't want to TX with interleaving so just turn on the buffer for RX
                interleavemode = 2
        else:
            #Not dynamic FEC
            if fec_enabled:
                if (fec_en == model.vars.fec_en.var_enum.FEC_154G_NRNSC_INTERLEAVING) or \
                        (fec_en == model.vars.fec_en.var_enum.FEC_154G_RSC_INTERLEAVING) or \
                        (fec_en == model.vars.fec_en.var_enum.FEC_K7_INTERLEAVING):
                    #We need to turn on interleaving to TX properly
                    interleavemode = 1
                else:
                    interleavemode = 0
            else:
                interleavemode = 0
        # Load value into register
        self._reg_write(model.vars.FRC_FECCTRL_INTERLEAVEMODE, interleavemode)

    def calc_convdecodemode_reg(self, model):
        # This function calculates the CONVDECODEMODE (hard vs soft decision symbols)

        # Assign inputs to variables
        fec_en = model.vars.fec_en.value
        fec_enabled = model.vars.fec_enabled.value
        dsss_bits_per_symbol = model.vars.dsss_bits_per_symbol.value
        demod_select = model.vars.demod_select.value

        if fec_enabled or (fec_en != model.vars.fec_en.var_enum.NONE):
            convdecodemode = 1 # hard decision (default in most cases)
        else:
            convdecodemode = 0

        # Write the register
        self._reg_write(model.vars.FRC_FECCTRL_CONVDECODEMODE, convdecodemode)

    def calc_fec_tx_enable(self, model):
        # This function enables or disables FEC in TX
        # It is used so that we can configure FEC type with fec_en but still disable on TX if desired

        #Read in model variables
        fec_en = model.vars.fec_en.value

        #By default enable FEC TX whenever a customer selects a fec_en setting that is not NONE
        if fec_en != model.vars.fec_en.var_enum.NONE:
            fec_tx_enable = model.vars.fec_tx_enable.var_enum.ENABLED
        else:
            fec_tx_enable = model.vars.fec_tx_enable.var_enum.DISABLED

        #Write the variable
        model.vars.fec_tx_enable.value = fec_tx_enable

    def calc_fecctrl_convmode_reg(self, model):
        #This function calculates the CONVMODE field

        #Read in model variables
        fec_tx_enable = (model.vars.fec_tx_enable.value == model.vars.fec_tx_enable.var_enum.ENABLED)

        #Calculate the register
        if fec_tx_enable:
            convmode = 1
        else:
            convmode = 0

        #Set the register
        self._reg_write(model.vars.FRC_FECCTRL_CONVMODE, convmode)

    def calc_dynamic_fec_enable(self, model):
        #This function calculates a default value for dynamic_fec_enable

        model.vars.dynamic_fec_enable.value = model.vars.dynamic_fec_enable.var_enum.DISABLED