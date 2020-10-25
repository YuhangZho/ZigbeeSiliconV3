from pyradioconfig.parts.lynx.calculators.calc_dsa import CALC_DSA_lynx
import math

class CALC_DSA_ocelot(CALC_DSA_lynx):

    def calc_longrange_timeout_threshold(self, model):
        """
        For COHDSA, wait for this time out after prefilter detect before shutting demod down.

        Args:
            model:

        Returns:

        """

        demod_select = model.vars.demod_select.value
        bitrate_gross = model.vars.bitrate_gross.value
        preamble_pattern_len = model.vars.preamble_pattern_len.value
        demod_rate = model.vars.demod_rate_actual.value

        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            preamble_pattern_time_us = preamble_pattern_len * (1 / bitrate_gross) * 1e6

            # : timeout [us] = LRTIMEOUTTHD / demod_rate_MHz
            # : wait twice preamble pattern length
            demod_rate_MHz = demod_rate / 1e6
            lrtimeoutthd_reg = int(round(demod_rate_MHz * preamble_pattern_time_us * 2.0))
        else:
            lrtimeoutthd_reg = 0

        self._reg_write(model.vars.MODEM_LONGRANGE1_LRTIMEOUTTHD, lrtimeoutthd_reg)

    def calc_cohdsa_dynamic_threshold(self, model):
        demod_select = model.vars.demod_select.value
        chpwr_accumux_noise = model.vars.chpwraccu_noise.value
        base_frequency_hz = model.vars.base_frequency_hz.value

        if base_frequency_hz <= 500e6:
            static_cohdsa_threshold = 140
            baseline_dynamic_cohdsa_threshold = 140
        else:
            static_cohdsa_threshold = 130
            baseline_dynamic_cohdsa_threshold = 130

        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            self._reg_write(model.vars.MODEM_LONGRANGE6_LRSPIKETHD, static_cohdsa_threshold)
            self._reg_write(model.vars.MODEM_COH2_FIXEDCDTHFORIIR, baseline_dynamic_cohdsa_threshold)
            self._reg_write(model.vars.MODEM_LONGRANGE6_LRCHPWRSPIKETH, int(round(chpwr_accumux_noise+6)))
        else:
            self._reg_write(model.vars.MODEM_LONGRANGE6_LRCHPWRSPIKETH, 0)
            self._reg_write(model.vars.MODEM_LONGRANGE6_LRSPIKETHD, 0)
            self._reg_write(model.vars.MODEM_COH2_FIXEDCDTHFORIIR, 0)

    def calc_cohdsa_mode(self, model):
        demod_select = model.vars.demod_select.value
        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            enable_cohdsa = 1
        else:
            enable_cohdsa = 0
        self._reg_write(model.vars.MODEM_COH3_COHDSAEN, enable_cohdsa)

    def calc_cohdsa_dynamic_iir_filter_coefficient(self, model):
        demod_select = model.vars.demod_select.value
        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            # : Set to maximum filtering
            dyniircoef = 3
        else:
            dyniircoef = 0
        self._reg_write(model.vars.MODEM_COH3_DYNIIRCOEFOPTION, dyniircoef)

    def calc_cohdsa_check_peak_index_length(self, model):
        # : Disable check if difference in the indices of prefilter correlation peaks is less than dsapeakindlen
        self._reg_write(model.vars.MODEM_COH3_DSAPEAKCHKEN, 0)
        self._reg_write(model.vars.MODEM_COH3_DSAPEAKINDLEN, 0)

    def calc_cohdsa_addwndsize(self, model):
        demod_select = model.vars.demod_select.value
        target_osr = model.vars.target_osr.value
        dsss_len = model.vars.dsss_len_actual.value

        # : For coherent demod, advance timing window from DSA detection by half symbol
        # : This is assuming that DSA detection occured on 3rd or 4th preamble.
        # : For now, choosing to detect dsa on 3rd or 4th preamble so that the difference between peak and noise will be
        # : high. This may result in sensitivity degradation since the static DSA threshold will be set high.
        # : If DSA detection occurs on 1st or 2nd preamble, need to DELAY (negative) by 2 symbols
        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            wndsize = target_osr * dsss_len / 2.0
            #wndsize = math.pow(2,10) - (target_osr * 2.0 * dsss_len)
            #wndsize = math.pow(2, 10) - (target_osr * 3.0 * dsss_len)
        else:
            wndsize = 0

        wndsize = int(wndsize)
        self._reg_write(model.vars.MODEM_COH3_COHDSAADDWNDSIZE, wndsize)

    def calc_cohdsa_signal_select(self, model):
        demod_select = model.vars.demod_select.value

        # : For coherent demod, use 4 bits from 10 bit complex multiplier output for pre filter
        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            number_of_bits_from_prefilter = 4
        else:
            number_of_bits_from_prefilter = 0

        self._reg_write(model.vars.MODEM_COH3_CDSS, number_of_bits_from_prefilter)

