from pyradioconfig.parts.lynx.calculators.calc_radio import CALC_Radio_lynx

#This file contains calculations related to the configuring the analog front end

class CALC_Radio_ocelot(CALC_Radio_lynx):

    def calc_lpfbwtx_reg(self, model):
        self._reg_write(model.vars.SYNTH_LPFCTRL2RX_VCMLVLRX, 0)

    def calc_lpfbwrx_reg(self, model):
        pass

    def calc_reg_vcodetamplitude(self, model):
        pass

    def calc_if_frequency_hz_value(self,model):
        #The IF frequency calculation is now in the calc_synth file for Ocelot
        pass

    def calc_txtrimdregbleed_reg(self, model):
        #This method calculates the value of RAC_PATRIM3_TXTRIMDREGBLEED

        base_frequency_hz = model.vars.base_frequency_hz.value
        modulation_type = model.vars.modulation_type.value

        if modulation_type == model.vars.modulation_type.var_enum.OOK:
            bleed = 2
        else:
            if base_frequency_hz > 450e6:
                bleed = 1
            else:
                bleed = 2

        self._reg_write(model.vars.RAC_PATRIM3_TXTRIMDREGBLEED, bleed)

    def calc_txtrimdregslices_reg(self, model):
        #This method calculates the value of RAC_PATRIM3_TXTRIMDREGSLICES

        base_frequency_hz = model.vars.base_frequency_hz.value
        max_tx_power_dbm = model.vars.max_tx_power_dbm.value

        if base_frequency_hz > 450e6:
            if max_tx_power_dbm >= 11:
                slices = 1
            else:
                slices = 0
        else:
            slices = 0

        self._reg_write(model.vars.RAC_PATRIM3_TXTRIMDREGSLICES, slices)

    def calc_lnamix_reg(self, model):

        part_family = model.part_family

        reg = model.vars.RAC_RX_LNAMIXLDOLOWCUR.get_reset_val(part_family)
        self._reg_write(model.vars.RAC_RX_LNAMIXLDOLOWCUR, reg)

        reg = model.vars.RAC_LNAMIXTRIM0_LNAMIXLNA0CAPSEL.get_reset_val(part_family)
        self._reg_write(model.vars.RAC_LNAMIXTRIM0_LNAMIXLNA0CAPSEL, reg)

        reg = model.vars.RAC_LNAMIXTRIM1_LNAMIXLNA1CAPSEL.get_reset_val(part_family)
        self._reg_write(model.vars.RAC_LNAMIXTRIM1_LNAMIXLNA1CAPSEL, reg)

        self._reg_write(model.vars.RAC_LNAMIXTRIM4_LNAMIXRFPKDTHRESHSELLO, 2)
        self._reg_write(model.vars.RAC_LNAMIXTRIM4_LNAMIXRFPKDTHRESHSELHI, 4)

        self._reg_write(model.vars.RAC_PGACTRL_PGATHRPKDLOSEL, 1)
        self._reg_write(model.vars.RAC_PGACTRL_PGATHRPKDHISEL, 5)




