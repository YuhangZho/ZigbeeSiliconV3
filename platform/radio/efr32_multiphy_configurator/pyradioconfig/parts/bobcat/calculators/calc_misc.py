from pyradioconfig.parts.ocelot.calculators.calc_misc import CALC_Misc_ocelot

class Calc_Misc_Bobcat(CALC_Misc_ocelot):

    def calc_misc(self, model):

        #Call inherited calc_misc method
        super().calc_misc(model)

        #Add LongRange reg writes
        self._reg_write(model.vars.MODEM_LONGRANGE_LRBLE, 0)
        self._reg_write(model.vars.MODEM_LONGRANGE_LRBLEDSA, 0)
        self._reg_write(model.vars.MODEM_LONGRANGE_LRCORRSCHWIN, 0xA)
        self._reg_write(model.vars.MODEM_LONGRANGE_LRCORRTHD, 0x3E8)
        self._reg_write(model.vars.MODEM_LONGRANGE_LRDEC, 0)
        self._reg_write(model.vars.MODEM_LONGRANGE_LRTIMCORRTHD, 0x0FA)


