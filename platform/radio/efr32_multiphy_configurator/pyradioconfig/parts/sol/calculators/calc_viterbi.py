from pyradioconfig.parts.ocelot.calculators.calc_viterbi import CALC_Viterbi_ocelot

class Calc_Viterbi_Sol(CALC_Viterbi_ocelot):

    def calc_swcoeffen_reg(self, model):

        afc1shot_en = model.vars.MODEM_AFC_AFCONESHOT.value

        reg = 1 if afc1shot_en else 0

        self._reg_write(model.vars.FEFILT0_CHFCTRL_SWCOEFFEN, reg)
        self._reg_write(model.vars.MODEM_VTCORRCFG1_KSI3SWENABLE, reg)

    #to be removed later FIXME: AW
    def calc_syncacqwin_reg(self, model):

        vtdemoden = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value
        synclen = model.vars.syncbits_actual.value

        syncacqwin = synclen // 4 - 1 if vtdemoden else 0


        self._reg_write(model.vars.MODEM_REALTIMCFE_SYNCACQWIN, syncacqwin)
