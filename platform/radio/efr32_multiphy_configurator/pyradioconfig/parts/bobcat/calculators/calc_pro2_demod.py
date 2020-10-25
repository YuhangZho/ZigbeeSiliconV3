from pyradioconfig.parts.ocelot.calculators.calc_pro2_demod import CALC_Pro2_Demod_Ocelot

class Calc_Pro2_Demod_Bobcat(CALC_Pro2_Demod_Ocelot):

    def calc_bcr_cfe_costthd(self,model):
        calculated_phscale = model.vars.MODEM_TRECPMDET_PHSCALE.value
        calculated_bcrpmacqwin = model.vars.MODEM_BCRDEMODPMEXP_BCRPMACQWIN.value

        if  model.vars.bcr_demod_en.value == 1:
            bcr_cfe_costthd = int(round((500 - (7 - calculated_bcrpmacqwin) * 60) / 2 ** calculated_phscale) * 1.5)
            self._reg_write(model.vars.MODEM_BCRDEMODPMEXP_BCRCFECOSTTHD, bcr_cfe_costthd)
        else:
            self._reg_do_not_care(model.vars.MODEM_BCRDEMODPMEXP_BCRCFECOSTTHD)