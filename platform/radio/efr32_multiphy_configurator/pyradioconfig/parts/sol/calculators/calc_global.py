from pyradioconfig.parts.ocelot.calculators.calc_global import CALC_Global_ocelot
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from enum import Enum

class Calc_Global_Sol(CALC_Global_ocelot):

    def buildVariables(self, model):

        # Build variables from the Ocelot calculations
        super().buildVariables(model)

        # new model variables for Sol
        self._addModelRegister(model, 'RFFPLL0.RFFPLLCAL1.DIVXADCSEL', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CF.DEC0'                  , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT0.CF.DEC1'                  , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT0.CF.DEC2'                  , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT0.CF.CFOSR'                 , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT0.CF.DEC1GAIN'              , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT0.CF.ADCBITORDERI', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CF.ADCBITORDERQ', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.SRCCHF.SRCRATIO2',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.SRCCHF.SRCENABLE2',          int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.SRCCHF.SRCDECEN2',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.SRCCHF.INTOSR',              int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DIGIGAINCTRL.DIGIGAINEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DIGIGAINCTRL.DIGIGAINSEL', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DIGIGAINCTRL.DIGIGAINDOUBLE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DIGIGAINCTRL.DIGIGAINHALF', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DIGIGAINCTRL.DEC0GAIN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE00.SET0COEFF0', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE00.SET0COEFF1', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE00.SET0COEFF2', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE01.SET0COEFF3', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE01.SET0COEFF4', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE02.SET0COEFF5', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE02.SET0COEFF6', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE03.SET0COEFF7', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE03.SET0COEFF8', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE04.SET0COEFF9', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE04.SET0COEFF10', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE05.SET0COEFF11', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE05.SET0COEFF12', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE06.SET0COEFF13', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE06.SET0COEFF14', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE10.SET1COEFF0', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE10.SET1COEFF1', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE10.SET1COEFF2', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE11.SET1COEFF3', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE11.SET1COEFF4', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE12.SET1COEFF5', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE12.SET1COEFF6', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE13.SET1COEFF7', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE13.SET1COEFF8', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE14.SET1COEFF9', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE14.SET1COEFF10', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE15.SET1COEFF11', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE15.SET1COEFF12', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE16.SET1COEFF13', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCOE16.SET1COEFF14', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCTRL.FWSWCOEFFEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCTRL.FWSELCOEFF', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFCTRL.SWCOEFFEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.CHFLATENCYCTRL.CHFLATENCY', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.MIXCTRL.DIGIQSWAPEN'      , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT0.DIGMIXCTRL.DIGMIXFREQ',      int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DIGMIXCTRL.DIGMIXMODE',      int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DIGMIXCTRL.MIXERCONJ', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DIGMIXCTRL.DIGMIXFB', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCGAINGEAREN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCGAINGEAR', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCGAINGEARSMPS', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCESTIEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCCOMPEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCRSTEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCCOMPFREEZE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCCOMPGEAR', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMP.DCLIMIT', int, ModelVariableFormat.HEX)        
        self._addModelRegister(model, 'FEFILT0.DCCOMPFILTINIT.DCCOMPINITVALI', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMPFILTINIT.DCCOMPINITVALQ', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT0.DCCOMPFILTINIT.DCCOMPINIT', int, ModelVariableFormat.HEX)

        self._addModelRegister(model, 'FEFILT1.CF.DEC0'                  , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT1.CF.DEC1'                  , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT1.CF.DEC2'                  , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT1.CF.CFOSR'                 , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT1.CF.DEC1GAIN'              , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT1.CF.ADCBITORDERI', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CF.ADCBITORDERQ', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.SRCCHF.SRCRATIO2',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.SRCCHF.SRCENABLE2',          int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.SRCCHF.SRCDECEN2',           int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.SRCCHF.INTOSR',              int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DIGIGAINCTRL.DIGIGAINEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DIGIGAINCTRL.DIGIGAINSEL', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DIGIGAINCTRL.DIGIGAINDOUBLE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DIGIGAINCTRL.DIGIGAINHALF', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DIGIGAINCTRL.DEC0GAIN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE00.SET0COEFF0', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE00.SET0COEFF1', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE00.SET0COEFF2', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE01.SET0COEFF3', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE01.SET0COEFF4', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE02.SET0COEFF5', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE02.SET0COEFF6', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE03.SET0COEFF7', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE03.SET0COEFF8', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE04.SET0COEFF9', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE04.SET0COEFF10', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE05.SET0COEFF11', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE05.SET0COEFF12', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE06.SET0COEFF13', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE06.SET0COEFF14', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE10.SET1COEFF0', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE10.SET1COEFF1', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE10.SET1COEFF2', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE11.SET1COEFF3', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE11.SET1COEFF4', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE12.SET1COEFF5', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE12.SET1COEFF6', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE13.SET1COEFF7', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE13.SET1COEFF8', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE14.SET1COEFF9', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE14.SET1COEFF10', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE15.SET1COEFF11', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE15.SET1COEFF12', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE16.SET1COEFF13', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCOE16.SET1COEFF14', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCTRL.FWSWCOEFFEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCTRL.FWSELCOEFF', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFCTRL.SWCOEFFEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.CHFLATENCYCTRL.CHFLATENCY', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.MIXCTRL.DIGIQSWAPEN'      , int, ModelVariableFormat.HEX )
        self._addModelRegister(model, 'FEFILT1.DIGMIXCTRL.DIGMIXFREQ',      int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DIGMIXCTRL.DIGMIXMODE',      int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DIGMIXCTRL.MIXERCONJ', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DIGMIXCTRL.DIGMIXFB', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCGAINGEAREN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCGAINGEAR', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCGAINGEARSMPS', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCESTIEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCCOMPEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCRSTEN', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCCOMPFREEZE', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCCOMPGEAR', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMP.DCLIMIT', int, ModelVariableFormat.HEX)        
        self._addModelRegister(model, 'FEFILT1.DCCOMPFILTINIT.DCCOMPINITVALI', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMPFILTINIT.DCCOMPINITVALQ', int, ModelVariableFormat.HEX)
        self._addModelRegister(model, 'FEFILT1.DCCOMPFILTINIT.DCCOMPINIT', int, ModelVariableFormat.HEX)

    def calc_family_name(self, model):
        model.vars.family.value = "sol"

    def _add_modulation_type_variable(self, model):

        #This method is called in the inherited Common calculations
        #Adding the method here overrides the modulation type for Sol

        var = self._addModelVariable(model, 'modulation_type', Enum, ModelVariableFormat.DECIMAL,
                                     'Defines the modulation type.')
        member_data = [
            ['FSK2', 0, 'Frequency Shift Keying on two frequencies'],
            ['FSK4', 1, 'Frequency Shift Keying on four frequencies'],
            ['BPSK', 2,
             'Binary Phase Shift Keying: the 2 symbols are represented by 0 or 180 degree phase shifts wrt the carrier'],
            ['DBPSK', 3,
             'Differential Binary Phase Shift Keying: the 2 symbols are represented by 0 or 180 degree phase shifts wrt the preceding symbol'],
            ['OOK', 4, 'On Off Keying: the 2 symbols are represented by the presence / absence of the carrier'],
            ['ASK', 5,
             'Amplitude Shift Keying: the 2 symbols are represented by two different power levels of the carrier'],
            ['MSK', 6,
             'Minimum Shift Keying: Special case of FSK2 where the phase shift in one symbol is +/- 90 degree'],
            ['OQPSK', 7,
             'Offset Quadrature Phase Shift Keying: 4 state phase modulation with 0, 90, 180 and 270 degrees wrt the carrier. Only +/-90 degree changes are allowed at any one transition that take place at twice the symbol rate.'],
            ['OFDM', 8, 'Orthogonal Frequency-Division Multiplexing'],
        ]
        var.var_enum = CreateModelVariableEnum(
            'ModModeEnum',
            'Defines the modulation type.',
            member_data)
