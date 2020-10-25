from pyradioconfig.parts.ocelot.phys.Phys_RAIL_Base_Standard_ZWave import PHYS_ZWave_Ocelot
from pyradioconfig.parts.common.phys.phy_common import PHY_COMMON_FRAME_154
from py_2_and_3_compatibility import *

class PHYS_ZWave_Sol(PHYS_ZWave_Ocelot):

    #def PHY_ZWave_100kbps_912MHz_OQPSK(self, model, phy_name=None):
        #pass

    def ZWave_100kbps_912MHz_OQPSK(self, phy, model):

        # force adc_clock_mode to XO - for Ocelot this is already the default - only doing this for Bobcat
        model.vars.adc_clock_mode.value_forced = model.vars.adc_clock_mode.var_enum.HFXOMULT

        PHY_COMMON_FRAME_154(phy, model)
        # Override min length for 802.15.4E Seq# Suppression
        phy.profile_inputs.var_length_minlength.value = 4
        phy.profile_inputs.bandwidth_hz.value = 1000000
        phy.profile_inputs.base_frequency_hz.value = long(912000000)
        phy.profile_inputs.baudrate_tol_ppm.value = 0
        phy.profile_inputs.bitrate.value = 100000
        phy.profile_inputs.channel_spacing_hz.value = 5000000
        phy.profile_inputs.deviation.value = 200000
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.dsss_chipping_code.value = long(0xC8DD7892)
        phy.profile_inputs.dsss_len.value = 32
        phy.profile_inputs.dsss_spreading_factor.value = 8
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.DISABLED
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0
        #phy.profile_inputs.if_frequency_hz.value = 1370000
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.OQPSK
        phy.profile_inputs.number_of_timing_windows.value = 7
        phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_1500KHz
        phy.profile_inputs.pll_bandwidth_rx.value = model.vars.pll_bandwidth_rx.var_enum.BW_250KHz
        phy.profile_inputs.preamble_length.value = 320
        phy.profile_inputs.preamble_pattern.value = 0
        phy.profile_inputs.preamble_pattern_len.value = 4
        phy.profile_inputs.rssi_period.value = 3
        phy.profile_inputs.rx_xtal_error_ppm.value = 27
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Custom_OQPSK
        phy.profile_inputs.shaping_filter_param.value = 1.0
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.DSSS
        phy.profile_inputs.symbols_in_timing_window.value = 12
        phy.profile_inputs.syncword_0.value = long(0xe5)
        phy.profile_inputs.syncword_1.value = long(0x0)
        phy.profile_inputs.syncword_length.value = 8
        phy.profile_inputs.target_osr.value = 5
        phy.profile_inputs.timing_detection_threshold.value = 65
        phy.profile_inputs.timing_sample_threshold.value = 0
        phy.profile_inputs.tx_xtal_error_ppm.value = 27
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        phy.profile_inputs.target_osr.value = 5  # Calc SRC
        phy.profile_inputs.demod_select.value = model.vars.demod_select.var_enum.COHERENT

        phy.profile_outputs.AGC_LNABOOST_BOOSTLNA.override = 1
        phy.profile_outputs.AGC_LNABOOST_LNABWADJ.override = 0
        # phy.profile_outputs.AGC_CTRL0_PWRTARGET.override = 20
        # phy.profile_outputs.AGC_CTRL1_PWRPERIOD.override = 4
        phy.profile_outputs.FRC_AUTOCG_AUTOCGEN.override = 7
        phy.profile_outputs.MODEM_AFC_AFCRXCLR.override = 1
        phy.profile_outputs.MODEM_AFCADJLIM_AFCADJLIM.override = 2750
        phy.profile_outputs.MODEM_CGCLKSTOP_FORCEOFF.override = 0x1E00  # 9,10,11,12
        phy.profile_outputs.MODEM_COH0_COHCHPWRTH0.override = 33
        phy.profile_outputs.MODEM_COH0_COHCHPWRTH1.override = 40
        phy.profile_outputs.MODEM_COH0_COHCHPWRTH2.override = 100
        phy.profile_outputs.MODEM_COH0_COHDYNAMICBBSSEN.override = 1
        phy.profile_outputs.MODEM_COH0_COHDYNAMICPRETHRESH.override = 1
        phy.profile_outputs.MODEM_COH0_COHDYNAMICSYNCTHRESH.override = 1
        phy.profile_outputs.MODEM_COH1_SYNCTHRESH0.override = 20
        phy.profile_outputs.MODEM_COH1_SYNCTHRESH1.override = 23
        phy.profile_outputs.MODEM_COH1_SYNCTHRESH2.override = 26
        phy.profile_outputs.MODEM_COH2_DSAPEAKCHPWRTH.override = 200
        phy.profile_outputs.MODEM_COH2_FIXEDCDTHFORIIR.override = 105
        phy.profile_outputs.MODEM_COH2_SYNCTHRESHDELTA1.override = 2
        phy.profile_outputs.MODEM_COH2_SYNCTHRESHDELTA2.override = 4
        phy.profile_outputs.MODEM_COH3_CDSS.override = 4
        phy.profile_outputs.MODEM_COH3_COHDSAADDWNDSIZE.override = 80
        phy.profile_outputs.MODEM_COH3_COHDSAEN.override = 1
        phy.profile_outputs.MODEM_COH3_DSAPEAKINDLEN.override = 4
        phy.profile_outputs.MODEM_COH3_DYNIIRCOEFOPTION.override = 3
        phy.profile_outputs.MODEM_COH3_LOGICBASEDCOHDEMODGATE.override = 1
        phy.profile_outputs.MODEM_COH3_PEAKCHKTIMOUT.override = 18
        phy.profile_outputs.MODEM_CTRL1_PHASEDEMOD.override = 2
        phy.profile_outputs.MODEM_CTRL2_DATAFILTER.override = 7
        phy.profile_outputs.MODEM_CTRL2_SQITHRESH.override = 200
        phy.profile_outputs.MODEM_CTRL5_BBSS.override = 6
        phy.profile_outputs.MODEM_CTRL5_DSSSCTD.override = 1
        phy.profile_outputs.MODEM_CTRL5_FOEPREAVG.override = 7
        phy.profile_outputs.MODEM_CTRL5_LINCORR.override = 1
        phy.profile_outputs.MODEM_CTRL5_POEPER.override = 1
        phy.profile_outputs.MODEM_CTRL6_ARW.override = 1
        phy.profile_outputs.MODEM_CTRL6_DSSS3SYMBOLSYNCEN.override = 1
        phy.profile_outputs.MODEM_CTRL6_PSTIMABORT0.override = 1
        phy.profile_outputs.MODEM_CTRL6_PSTIMABORT1.override = 1
        phy.profile_outputs.MODEM_CTRL6_PSTIMABORT2.override = 1
        phy.profile_outputs.MODEM_CTRL6_RXBRCALCDIS.override = 1
        phy.profile_outputs.MODEM_CTRL6_TDREW.override = 64
        phy.profile_outputs.MODEM_CTRL6_TIMTHRESHGAIN.override = 2
        phy.profile_outputs.MODEM_DSACTRL_ARRTHD.override = 4  # Was missed
        phy.profile_outputs.MODEM_INTAFC_FOEPREAVG0.override = 1
        phy.profile_outputs.MODEM_INTAFC_FOEPREAVG1.override = 3
        phy.profile_outputs.MODEM_INTAFC_FOEPREAVG2.override = 5
        phy.profile_outputs.MODEM_INTAFC_FOEPREAVG3.override = 5
        phy.profile_outputs.MODEM_LONGRANGE1_AVGWIN.override = 4
        phy.profile_outputs.MODEM_LONGRANGE1_HYSVAL.override = 3
        phy.profile_outputs.MODEM_LONGRANGE1_LRTIMEOUTTHD.override = 320
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH1.override = 20
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH2.override = 26
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH3.override = 33
        phy.profile_outputs.MODEM_LONGRANGE2_LRCHPWRTH4.override = 40
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH5.override = 46
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH6.override = 52
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH7.override = 59
        phy.profile_outputs.MODEM_LONGRANGE3_LRCHPWRTH8.override = 66
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH1.override = 3
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH2.override = 4
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH3.override = 5
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRSH4.override = 5
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRTH10.override = 80
        phy.profile_outputs.MODEM_LONGRANGE4_LRCHPWRTH9.override = 73
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH10.override = 11
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH11.override = 12
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH5.override = 6
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH6.override = 7
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH7.override = 8
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH8.override = 9
        phy.profile_outputs.MODEM_LONGRANGE5_LRCHPWRSH9.override = 10
        phy.profile_outputs.MODEM_LONGRANGE6_LRCHPWRSPIKETH.override = 40
        phy.profile_outputs.MODEM_LONGRANGE6_LRSPIKETHD.override = 105
        phy.profile_outputs.MODEM_TIMING_TIMTHRESH.override = 35
        # LM phy.profile_outputs.MODEM_TXBR_TXBRDEN.override = 105 - Not used in Matlab
        # LM phy.profile_outputs.MODEM_TXBR_TXBRNUM.override = 252 - Not used in Matlab
        # phy.profile_outputs.RAC_PGACTRL_LNAMIXRFPKDTHRESHSEL.override = 2
        # phy.profile_outputs.RAC_PGACTRL_LNAMIXRFPKDTHRESHSEL.override = 2
        # LM phy.profile_outputs.RAC_PGACTRL_PGATHRPKDHISEL.override = 5
        # LM phy.profile_outputs.RAC_PGACTRL_PGATHRPKDLOSEL.override = 1
        # phy.profile_outputs.RAC_SYNTHCTRL_MMDPOWERBALANCEDISABLE.override = 1
        # LM phy.profile_outputs.RAC_SYNTHREGCTRL_MMDLDOVREFTRIM.override = 3
        # LM phy.profile_outputs.RAC_PGACTRL_PGAENLATCHI.override = 1 - Not used in Matlab
        # LM phy.profile_outputs.RAC_PGACTRL_PGAENLATCHQ.override = 1 - Not used in Matlab
        # LM phy.profile_outputs.RFCRC_CTRL_INPUTINV.override = 0 - Not used in Matlab
        # LM phy.profile_outputs.SYNTH_LPFCTRL1CAL_OP1BWCAL.override = 11
        # LM phy.profile_outputs.SYNTH_LPFCTRL1CAL_OP1COMPCAL.override = 14
        # LM phy.profile_outputs.SYNTH_LPFCTRL1CAL_RFBVALCAL.override = 0
        # LM phy.profile_outputs.SYNTH_LPFCTRL1CAL_RPVALCAL.override = 0
        # LM phy.profile_outputs.SYNTH_LPFCTRL1CAL_RZVALCAL.override = 9
        phy.profile_outputs.MODEM_CTRL2_SQITHRESH.override = 56

        # from meeting with Per: new registers needed
        # from meeting with Per: new registers needed
        phy.profile_outputs.MODEM_LONGRANGE1_PREFILTLEN.override = 3
        phy.profile_outputs.MODEM_PREFILTCOEFF_PREFILTCOEFF.override = long(0xF3CD231D)

        phy.profile_outputs.MODEM_COH0_COHCHPWRRESTART.override = 1

        return phy


    def PHY_ZWave_100kbps_912MHz_OQPSK_beam(self, model, phy_name=None):
        pass

    #def PHY_ZWave_100kbps_916MHz(self, model, phy_name=None):
        #pass

    def PHY_ZWave_100kbps_916MHz_DSA_Beam_Passthrough(self, model):
        pass

    def PHY_ZWave_100kbps_916MHz_DSA_Beam_Passthrough_lowside(self, model):
        pass

    def PHY_ZWave_100kbps_916MHz_lowside(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='PHY_ZWave_100kbps_916MHz_lowside', phy_name=phy_name)

        self.ZWave_100kbps_base(phy, model)
        #phy.profile_inputs.bandwidth_hz.value = 250000
        phy.profile_inputs.base_frequency_hz.value = long(916000000)
        phy.profile_inputs.lo_injection_side.value = model.vars.lo_injection_side.var_enum.LOW_SIDE
        phy.profile_outputs.FEFILT0_DIGMIXCTRL_MIXERCONJ.override = 1

        # Overrides for the SQ block
        phy.profile_outputs.MODEM_SQ_SQEN.override = 1
        # Timings based on Zwave_LR_timing_21.xlsx
        # Based on sequential scanning of R3, R2, R1, LR
        phy.profile_outputs.MODEM_SQ_SQTIMOUT.override = int(230 / 10) # Timeout before checking for timing detect. Units: T_b. Zwave_LR_timing_21.xlsx:I13
        phy.profile_outputs.MODEM_SQEXT_SQSTG2TIMOUT.override =  320 # Timeout after timing detect to check for preamble. Units T_b.
        phy.profile_outputs.MODEM_SQEXT_SQSTG3TIMOUT.override = 16 # Timeout after preamble to check for sync word. Units T_b. Cell B25

        # Disable TX side AFC adjustments.
        phy.profile_outputs.MODEM_AFCADJTX_AFCSCALEM.override = 0
        phy.profile_outputs.MODEM_AFCADJTX_AFCSCALEE.override = 0
        return phy

    def ZWave_40kbps_908MHz_viterbi_base(self, phy, model):
        self.ZWave_40kbps_base(phy, model)
        phy.profile_inputs.demod_select.value = model.vars.demod_select.var_enum.TRECS_SLICER
        phy.profile_inputs.base_frequency_hz.value = long(908400000)
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        phy.profile_inputs.symbols_in_timing_window.value = 12
        phy.profile_inputs.syncword_0.value = long(0xF0) # add 3 bytes from the preamble to make the syncword stronger

        # preamble/sync trades off preamble budget for syncword strength
        model.vars.trecs_pre_bits_to_syncword.value_forced = 8
        model.vars.preamsch_len.value_forced = 12  # set to timing window length to minimize detection time
        model.vars.trecs_effective_preamble_len.value_forced = 72
        model.vars.trecs_effective_syncword_len.value_forced = 16

        # remod path is used to widen the bandwidth for frequency offset requirement. OSR is moved to DEC2 and reduces
        # droop seen at DEC1, increasing the CHF f_s and decreasing the bwsel constraint
        phy.profile_inputs.bandwidth_hz.value = 200000
        #FIXME: causes SRC2 out of range
        #phy.profile_outputs.FEFILT0_CF_DEC2.override = 1
        # No frequency gain. Set so no saturation with up to 5*fdev. freqgain = freqscalem * 2^(2-freqscalee)
        # Must be able to handle +/- 85 kHz offset
        phy.profile_outputs.MODEM_MODINDEX_FREQGAINE.override = 2
        phy.profile_outputs.MODEM_MODINDEX_FREQGAINM.override = 1
        phy.profile_outputs.MODEM_VITERBIDEMOD_VITERBIKSI3.override = 17
        phy.profile_outputs.MODEM_TRECPMDET_PMMINCOSTTHD.override = 60
        phy.profile_outputs.MODEM_TRECPMDET_PMCOSTVALTHD.override = 3
        phy.profile_outputs.MODEM_REALTIMCFE_RTSCHWIN.override = 4

        # Disable TX side AFC adjustments.
        phy.profile_outputs.MODEM_AFCADJTX_AFCSCALEM.override = 0
        phy.profile_outputs.MODEM_AFCADJTX_AFCSCALEE.override = 0

    #def PHY_ZWave_100kbps_916MHz_viterbi(self, model, phy_name=None):
        #pass
    def PHY_ZWave_100kbps_916MHz(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='PHY_ZWave_100kbps_916MHz for Sol',
                            phy_name=phy_name)
        self.ZWave_100kbps_base(phy, model)
        phy.profile_inputs.xtal_frequency_hz.value = 38400000
        phy.profile_inputs.base_frequency_hz.value = long(916000000)  # temp solution, FIXME, it needs to be 916MHz

        # cfloopdel was 10, changed to improve timing search (first window is lost)
        # phy.profile_outputs.AGC_GAINSTEPLIM0_CFLOOPDEL.override = (10 + 35)  # was 10, then 37 passing,
        # now 33 (not good enough, some frames are lost, estimated a value between 40-47)

        # Overrides for the SQ block
        phy.profile_outputs.MODEM_SQ_SQTIMOUT.override = int(
            250 / 10)  # Timeout before checking for timing detect. Units: T_b. Zwave_LR_timing_21.xlsx:I13
        phy.profile_outputs.MODEM_SQEXT_SQSTG2TIMOUT.override = 320  # Timeout after timing detect to check for preamble. Units T_b.
        phy.profile_outputs.MODEM_SQEXT_SQSTG3TIMOUT.override = 16 + 16  # Timeout after preamble to check for sync word. Units T_b.

        # Reduce HICNTREGINON0 to speed up AGC when near upper threshold
        phy.profile_outputs.AGC_HICNTREGION0_HICNTREGION0.override = 10

        # Disable TX side AFC adjustments.
        #phy.profile_outputs.SYNTH_FREQ_FREQ.override = 36901809

        return phy

    def PHY_ZWave_100kbps_916MHz_viterbi_beam(self, model, phy_name=None):
        pass

    def PHY_ZWave_100kbps_916MHz_viterbi_lowside(self, model, phy_name=None):
        # low side version for Korea image rejection issue on channel 0
        phy = self.PHY_ZWave_100kbps_916MHz_viterbi(model, phy_name=phy_name)
        phy.profile_inputs.lo_injection_side.value = model.vars.lo_injection_side.var_enum.LOW_SIDE
        phy.profile_outputs.FEFILT0_DIGMIXCTRL_MIXERCONJ.override = 1
        return phy

    def PHY_ZWave_100kbps_916MHz_viterbi_lowside_beam(self, model, phy_name=None):
        pass

    #def PHY_ZWave_40kbps_908MHz(self, model, phy_name=None):
        #pass

    #def PHY_ZWave_40kbps_908MHz_viterbi(self, model, phy_name=None):
        #pass

    def PHY_ZWave_40kbps_908MHz_viterbi_beam(self, model, phy_name=None):
        pass

    #def PHY_ZWave_40kbps_9p6kbps_908MHz_legacy_conc(self, model, phy_name=None):
        #pass

    #def PHY_ZWave_40kbps_9p6kbps_908MHz_viterbi_conc(self, model, phy_name=None):
        #pass

    #def PHY_ZWave_9p6kbps_908MHz(self, model, phy_name=None):
        #pass

    def PHY_ZWave_9p6kbps_908MHz_bcr(self, model, phy_name=None):
        pass

    def PHY_ZWave_9p6kbps_908MHz_bcr_conc(self, model, phy_name=None):
        pass

    def PHY_ZWave_9p6kbps_908MHz_viterbi(self, model, phy_name=None):
        pass
