from pyradioconfig.parts.ocelot.calculators.calc_synth import CALC_Synth_ocelot

class Calc_Synth_Sol(CALC_Synth_ocelot):

    def calc_adc_rate_mode_reg(self, model):

        #This function handles writes to the registers impacting ADC clock mode and HFXO multiplier
        adc_rate_mode = model.vars.adc_rate_mode.value

        if(model.vars.adc_rate_mode.var_enum.FULLRATE == adc_rate_mode):
            self._reg_write(model.vars.RFFPLL0_RFFPLLCAL1_DIVXADCSEL, 0)
        elif (model.vars.adc_rate_mode.var_enum.HALFRATE == adc_rate_mode):            
            self._reg_write(model.vars.RFFPLL0_RFFPLLCAL1_DIVXADCSEL, 1)
        else:
            # Eigthrate
            self._reg_write(model.vars.RFFPLL0_RFFPLLCAL1_DIVXADCSEL, 1)

    def calc_adc_freq_actual(self,model):
        #This function calculates the actual ADC sample frequency and error based on the registers

        #Load model variables into local variables
        adc_clock_mode_actual = model.vars.adc_clock_mode_actual.value
        adc_vco_div_actual = model.vars.adc_vco_div_actual.value
        fxo = model.vars.xtal_frequency_hz.value
        fsynth = model.vars.rx_synth_freq_actual.value
        fadc_target = model.vars.adc_target_freq.value #The target frequency is used for computing error
        sysclkmult_divx_adc_sel = model.vars.RFFPLL0_RFFPLLCAL1_DIVXADCSEL.value
        rx_ifadc_en_xo_bypass = model.vars.RAC_IFADCTRIM1_IFADCENXOBYPASS.value

        if adc_clock_mode_actual == model.vars.adc_clock_mode.var_enum.HFXOMULT:
            #the ordering of the if statements is important - keep rx_ifadc_en_xo_bypass on top
            if 1 == rx_ifadc_en_xo_bypass:
                adc_freq_actual = fxo
            elif 0 == sysclkmult_divx_adc_sel:
                adc_freq_actual = fxo * 8
            else:
                adc_freq_actual = fxo * 4
        else:
            adc_freq_actual = int(fsynth / adc_vco_div_actual)

        # Compute the final ADC frequency percent error
        ferror = 100 * (fadc_target - adc_freq_actual) / float(fadc_target)

        #Load local variables back into model variables
        model.vars.adc_freq_actual.value = adc_freq_actual
        model.vars.adc_freq_error.value = ferror

    def calc_lo_side_regs(self, model):
        """
        calculate LOSIDE register in synth and matching one in modem

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        model.vars.lo_injection_side.value = model.vars.lo_injection_side.var_enum.HIGH_SIDE  # default to high-side

        lo_injection_side = model.vars.lo_injection_side.value

        if lo_injection_side == model.vars.lo_injection_side.var_enum.HIGH_SIDE:
            loside = 1
            digiqswapen = 1
            mixerconj = 0
        else:
            loside = 0
            digiqswapen = 0
            mixerconj = 1

        # Write the registers
        self._reg_write(model.vars.SYNTH_IFFREQ_LOSIDE, loside)
        self._reg_write(model.vars.FEFILT0_MIXCTRL_DIGIQSWAPEN, digiqswapen)
        self._reg_write(model.vars.FEFILT0_DIGMIXCTRL_MIXERCONJ, mixerconj)
