from pyradioconfig.parts.ocelot.calculators.calc_synth import CALC_Synth_ocelot

class Calc_Synth_Bobcat(CALC_Synth_ocelot):

    # overwrite this function from Ocelot to make VCODIV the default clock for ADC
    def calc_adc_clock_config(self, model):
        # This function calculates both the ADC mode (e.g. fullrate, halfrate, etc) as well as the ADC clock divider path

        # Load model values into local variables
        bandwidth_hz = model.vars.bandwidth_hz.value

        if (bandwidth_hz < 1.25e6):
            # 1/2 rate mode
            # Use the HFXO along with DPLL for the ADC clock
            adc_rate_mode = model.vars.adc_rate_mode.var_enum.HALFRATE
            adc_clock_mode = model.vars.adc_clock_mode.var_enum.VCODIV
        else:
            # Full rate mode
            # Use the divided down VCO for the ADC clock
            adc_rate_mode = model.vars.adc_rate_mode.var_enum.FULLRATE
            adc_clock_mode = model.vars.adc_clock_mode.var_enum.VCODIV

        # Load local variables back into model variables
        model.vars.adc_clock_mode.value = adc_clock_mode
        model.vars.adc_rate_mode.value = adc_rate_mode

    def calc_sylodivrloadcclk_reg(self, model):
        adc_rate_mode = model.vars.adc_rate_mode.value

        if adc_rate_mode == model.vars.adc_rate_mode.var_enum.HALFRATE:
            reg = 1
        else:
            reg = 0

        self._reg_write(model.vars.RAC_SYLOEN_SYLODIVRLOADCCLKSEL, reg)
