from pyradioconfig.parts.ocelot.calculators.calc_misc import CALC_Misc_ocelot

class Calc_Misc_Sol(CALC_Misc_ocelot):

    def calc_misc_Ocelot(self, model):
        pass

    def calc_misc_Sol(self, model):
        """
        These aren't really calculating right now.  Just using defaults or forced values.
       
        Args:
             model (ModelRoot) : Data model to read and write variables from
        """
        # FIXME: This was not calculated on Ocelot
        #self._reg_write(model.vars.FEFILT0_CF_CFOSR, 0)
        
        self._reg_write(model.vars.FEFILT0_CF_ADCBITORDERI, 0)
        self._reg_write(model.vars.FEFILT0_CF_ADCBITORDERQ, 0)
        
        # FIXME: This was not calculated on Ocelot
        #self._reg_write(model.vars.FEFILT0_SRCCHF_SRCDECEN2, 0)                
        
        # Digital Gain Control
        self._reg_write(model.vars.FEFILT0_DIGIGAINCTRL_DEC0GAIN, 0)
        self._reg_write(model.vars.FEFILT0_DIGIGAINCTRL_DIGIGAINDOUBLE, 0)
        self._reg_write(model.vars.FEFILT0_DIGIGAINCTRL_DIGIGAINEN, 0)
        self._reg_write(model.vars.FEFILT0_DIGIGAINCTRL_DIGIGAINHALF, 0)
        self._reg_write(model.vars.FEFILT0_DIGIGAINCTRL_DIGIGAINSEL, 0)

        ## New registers
        # FIXME: how to calculate these?
        self._reg_write(model.vars.FEFILT0_CHFCTRL_FWSELCOEFF, 0)
        self._reg_write(model.vars.FEFILT0_CHFCTRL_FWSWCOEFFEN, 0)
        self._reg_write(model.vars.FEFILT0_CHFLATENCYCTRL_CHFLATENCY, 0)

        self._reg_write(model.vars.FEFILT0_DIGMIXCTRL_DIGMIXMODE, 1)
        self._reg_write(model.vars.FEFILT0_DIGMIXCTRL_DIGMIXFB, 1)

        self._reg_write(model.vars.FEFILT0_DCCOMPFILTINIT_DCCOMPINIT, 0)
        self._reg_write(model.vars.FEFILT0_DCCOMPFILTINIT_DCCOMPINITVALI, 0)
        self._reg_write(model.vars.FEFILT0_DCCOMPFILTINIT_DCCOMPINITVALQ, 0)

