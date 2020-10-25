from pyradioconfig.parts.ocelot.calculators.calc_radio import CALC_Radio_ocelot

class Calc_Radio_Bobcat(CALC_Radio_ocelot):

    def calc_txtrimdreg_reg(self, model):
        self._reg_write(model.vars.RAC_TX_TX0DBMENBLEEDPREDRVREG, 1)
        #self._reg_write(model.vars.RAC_PATRIM3_TXTRIMDREGSLICES, 1)

    def calc_lnamix_reg(self, model):

        part_family = model.part_family.lower()

        #TODO: Enable once present in the reg model
        self._reg_write_default(model.vars.RAC_RX_LNAMIXLDOLOWCUR, part_family)
        #self._reg_write_default(model.vars.RAC_LNAMIXTRIM0_LNAMIXCAPSEL0, part_family)
        #self._reg_write_default(model.vars.RAC_LNAMIXTRIM1_LNAMIXCAPSEL1, part_family)

        self._reg_write(model.vars.RAC_LNAMIXTRIM4_LNAMIXRFPKDTHRESHSELLO, 2)
        self._reg_write(model.vars.RAC_LNAMIXTRIM4_LNAMIXRFPKDTHRESHSELHI, 4)
        self._reg_write(model.vars.RAC_PGACTRL_PGATHRPKDLOSEL, 1)
        self._reg_write(model.vars.RAC_PGACTRL_PGATHRPKDHISEL, 5)


