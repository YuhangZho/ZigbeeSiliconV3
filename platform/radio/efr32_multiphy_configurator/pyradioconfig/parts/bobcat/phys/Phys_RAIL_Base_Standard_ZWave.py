from pyradioconfig.parts.ocelot.phys.Phys_RAIL_Base_Standard_ZWave import PHYS_ZWave_Ocelot
from py_2_and_3_compatibility import *

class PHYS_ZWave_Bobcat(PHYS_ZWave_Ocelot):

    ### override inheritance from Ocelot
    # Base class for 9.6k R1 PHY for use in concurrent PHYs
    def ZWave_9p6kbps_908MHz_bcr_conc_base(self, phy, model):
        phy.profile_inputs.bcr_demod_en.value = 1
        phy.profile_inputs.bcr_conc_baudrate.value = 19200
        phy.profile_inputs.bcr_conc_manchester.value = 1
        phy.profile_inputs.bcr_conc_deviation.value = 20000

        # These overrides need to be converted to inputs. Need to be
        # careful of the concurrent portion.
        phy.profile_outputs.MODEM_BCRDEMODCTRL_BCRDEMODEN.override = 0

        phy.profile_outputs.MODEM_BCRDEMODRSSI_PRWOFFSET.override = 0
        phy.profile_outputs.MODEM_BCRDEMODCTRL_DETECTORSEL.override = 3
        phy.profile_outputs.MODEM_BCRDEMODARR0_PHSPIKETHD.override = 30
        phy.profile_outputs.MODEM_BCRDEMODARR0_SCHFRZEN.override = 0
        phy.profile_outputs.MODEM_BCRDEMODARR0_SCHPRDLO.override = 2  # after premable detection
        phy.profile_outputs.MODEM_BCRDEMODARR0_SCHPRDHI.override = 1  # before dsa detection

        phy.profile_outputs.MODEM_BCRDEMODARR1_BCRCFESRC.override = 1

        # These parameters are sample rate dependent and should be
        # overriden in the base 40k concurrent PHY where SRC2 is enabled
        # ideal eye opening is 7.7
        phy.profile_outputs.MODEM_BCRDEMODARR1_SYMBWMAX.override = 10
        phy.profile_outputs.MODEM_BCRDEMODARR1_SYMBWMIN.override = 6
        phy.profile_outputs.MODEM_BCRDEMODPMEXP_BCRPHSCALE.override = 0
        phy.profile_outputs.MODEM_BCRDEMODKSI_BCRKSI3.override = 35

        phy.profile_outputs.MODEM_BCRDEMODPMEXP_BCRCFECOSTTHD.override = 200

        phy.profile_outputs.MODEM_BCRDEMODCTRL_PMPATTERN.override = 3

        # Override for bitoverflow of phcomp_gain_4gfsk1
        phy.profile_outputs.MODEM_BCRDEMOD4FSK1_PHCOMP4FSK1.override = 0

        # Disable TX side AFC adjustments.
        phy.profile_outputs.MODEM_AFCADJTX_AFCSCALEM.override = 0
        phy.profile_outputs.MODEM_AFCADJTX_AFCSCALEE.override = 0

    ### override inheritance from Ocelot
    def ZWave_100kbps_base(self, phy, model):
        # force adc_clock_mode to XO - for Ocelot this is already the default - only doing this for Bobcat
        model.vars.adc_clock_mode.value_forced = model.vars.adc_clock_mode.var_enum.HFXOMULT

        # These values pass through to be consumed by RAIL API calls to enable RX scanning/hopping. Implemented as overrides to enable development. Long-term should be calculate-able.
        phy.profile_outputs.rx_ch_hopping_order_num.override = 0
        phy.profile_outputs.rx_ch_hopping_mode.override = model.vars.rx_ch_hopping_mode.var_enum.RAIL_RX_CHANNEL_HOPPING_MODE_TIMING_SENSE
        phy.profile_outputs.rx_ch_hopping_delay_usec.override = 270

        # These overrides are recommended only for Series-1 chips
        # phy.profile_outputs.MODEM_CTRL3_TSAMPDEL.override = 2
        # phy.profile_outputs.MODEM_CTRL5_DETDEL.override = 1
        # phy.profile_outputs.MODEM_CTRL2_DEVWEIGHTDIS.override = 1

        # phy.profile_inputs.agc_speed.value = model.vars.agc_speed.var_enum.FAST
        # phy.profile_inputs.agc_power_target.value = -8  # -24  # Targets ADC output power of -24dBm. Helps leave more head-room for blockers
        # phy.profile_outputs.AGC_CTRL0_MODE.override = 2  # LOCKFRAMEDET Gain is locked once a sync word is detected.

        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        phy.profile_inputs.rx_xtal_error_ppm.value = 27
        phy.profile_inputs.tx_xtal_error_ppm.value = 27
        phy.profile_inputs.deviation.value = 29000
        phy.profile_inputs.base_frequency_hz.value = long(869850000)
        phy.profile_inputs.bitrate.value = 100000
        phy.profile_inputs.dsss_chipping_code.value = long(0)
        phy.profile_inputs.dsss_len.value = 0
        phy.profile_inputs.dsss_spreading_factor.value = 0
        phy.profile_inputs.baudrate_tol_ppm.value = 0

        ### original settings for non-hopping PHY
        # phy.profile_inputs.timing_sample_threshold.value = 4
        # phy.profile_inputs.symbols_in_timing_window.value = 6
        # phy.profile_inputs.number_of_timing_windows.value = 2

        ### new settings optimized for hopping PHYs
        phy.profile_inputs.timing_sample_threshold.value = 11
        phy.profile_inputs.timing_detection_threshold.value = 10
        phy.profile_inputs.symbols_in_timing_window.value = 6
        phy.profile_inputs.number_of_timing_windows.value = 1

        phy.profile_inputs.agc_period.value = 0
        phy.profile_inputs.shaping_filter_param.value = 0.6
        phy.profile_inputs.preamble_pattern.value = 1
        phy.profile_inputs.preamble_pattern_len.value = 2
        phy.profile_inputs.preamble_length.value = 320
        phy.profile_inputs.syncword_0.value = long(240)
        phy.profile_inputs.syncword_1.value = long(0)
        phy.profile_inputs.syncword_length.value = 8
        phy.profile_inputs.syncword_tx_skip.value = False
        phy.profile_inputs.asynchronous_rx_enable.value = True
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP1
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK2
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON  # INTERNAL_LOCK_AT_PREAMBLE_DETECT
        # phy.profile_inputs.if_frequency_hz.value = 400000
        phy.profile_inputs.dsa_enable.value = False
        phy.profile_inputs.target_osr.value = 7
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian
        phy.profile_inputs.channel_spacing_hz.value = 0
        phy.profile_inputs.test_ber.value = False
        phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.CCITT_16
        phy.profile_inputs.crc_seed.value = long(0x1d0f)
        phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.MSB_FIRST
        phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.MSB_FIRST
        phy.profile_inputs.crc_pad_input.value = False
        phy.profile_inputs.crc_input_order.value = model.vars.crc_input_order.var_enum.MSB_FIRST
        phy.profile_inputs.crc_invert.value = False
        phy.profile_inputs.fec_en.value = model.vars.fec_en.var_enum.NONE
        phy.profile_inputs.frame_bitendian.value = model.vars.frame_bitendian.var_enum.MSB_FIRST
        phy.profile_inputs.frame_length_type.value = model.vars.frame_length_type.var_enum.VARIABLE_LENGTH
        phy.profile_inputs.payload_white_en.value = False
        phy.profile_inputs.payload_crc_en.value = True
        phy.profile_inputs.payload_addtrailtxdata_en.value = False
        # Absent from Ocelot, was used on Nerio
        # phy.profile_inputs.payload_excludesubframewcnt_en.value = False
        phy.profile_inputs.header_en.value = True
        phy.profile_inputs.header_size.value = 8
        phy.profile_inputs.header_calc_crc.value = True
        phy.profile_inputs.header_white_en.value = False
        phy.profile_inputs.header_addtrailtxdata_en.value = False
        phy.profile_inputs.header_excludesubframewcnt_en.value = False
        phy.profile_inputs.fixed_length_size.value = 16
        phy.profile_inputs.var_length_numbits.value = 8
        phy.profile_inputs.var_length_byteendian.value = model.vars.var_length_byteendian.var_enum.MSB_FIRST
        phy.profile_inputs.var_length_bitendian.value = model.vars.var_length_bitendian.var_enum.MSB_FIRST
        phy.profile_inputs.var_length_shift.value = 0
        phy.profile_inputs.var_length_minlength.value = 2  # 2
        phy.profile_inputs.var_length_maxlength.value = 170
        phy.profile_inputs.var_length_includecrc.value = True
        phy.profile_inputs.var_length_adjust.value = 8
        phy.profile_inputs.frame_type_loc.value = 0
        phy.profile_inputs.frame_type_bits.value = 0
        phy.profile_inputs.frame_type_lsbit.value = 0
        phy.profile_inputs.frame_type_0_length.value = 0
        phy.profile_inputs.frame_type_1_length.value = 0
        phy.profile_inputs.frame_type_2_length.value = 0
        phy.profile_inputs.frame_type_3_length.value = 0
        phy.profile_inputs.frame_type_4_length.value = 0
        phy.profile_inputs.frame_type_5_length.value = 0
        phy.profile_inputs.frame_type_6_length.value = 0
        phy.profile_inputs.frame_type_7_length.value = 0
        phy.profile_inputs.frame_type_0_valid.value = False
        phy.profile_inputs.frame_type_1_valid.value = False
        phy.profile_inputs.frame_type_2_valid.value = False
        phy.profile_inputs.frame_type_3_valid.value = False
        phy.profile_inputs.frame_type_4_valid.value = False
        phy.profile_inputs.frame_type_5_valid.value = False
        phy.profile_inputs.frame_type_6_valid.value = False
        phy.profile_inputs.frame_type_7_valid.value = False
        phy.profile_inputs.symbol_encoding.value = model.vars.symbol_encoding.var_enum.NRZ
        phy.profile_inputs.manchester_mapping.value = model.vars.manchester_mapping.var_enum.Default
        phy.profile_inputs.white_poly.value = model.vars.white_poly.var_enum.NONE
        phy.profile_inputs.white_seed.value = 0
        phy.profile_inputs.white_output_bit.value = 0
        phy.profile_inputs.frame_coding.value = model.vars.frame_coding.var_enum.NONE

        # Need to ensure the actual OSR is 7. At 39MHz xtal, the calculation alone will deliver either:
        # OSR = 6 when target input 7. This degrades 915MHz blocking performance 400kHz-10MHz in certain combinations of hardware (Z-Wave SIP)
        # OSR = 8 when target input is 8. This is not a valid config.
        # Instead of modifying calculations (which could affect many PHYs), simply override outputs which would be calculated
        # in parallel with with OSR.
        # phy.profile_inputs.target_osr.value = 7
        # phy.profile_inputs.agc_settling_delay.value = 36
        # phy.profile_outputs.MODEM_RXBR_RXBRNUM.override = 1
        # phy.profile_outputs.MODEM_SRCCHF_BWSEL.override = 0
        # phy.profile_outputs.MODEM_SRCCHF_SRCRATIO2.override = 933
        # phy.profile_outputs.MODEM_TIMING_OFFSUBDEN.override = 10
        # phy.profile_outputs.MODEM_TIMING_OFFSUBNUM.override = 13
        # phy.profile_outputs.MODEM_CTRL2_DATAFILTER.override = 5
        # phy.profile_outputs.MODEM_SRCCHF_BWSEL.override = 2

        phy.profile_inputs.pll_bandwidth_tx.value = model.vars.pll_bandwidth_tx.var_enum.BW_750KHz
        phy.profile_inputs.demod_select.value = model.vars.demod_select.var_enum.LEGACY

        phy.profile_inputs.antdivmode.value = model.vars.antdivmode.var_enum.DISABLE

        phy.profile_outputs.AGC_HICNTREGION0_HICNTREGION0.override = 10
        phy.profile_outputs.AGC_STEPDWN_STEPDWN0.override = 1
        phy.profile_outputs.AGC_STEPDWN_STEPDWN1.override = 2
        phy.profile_outputs.AGC_STEPDWN_STEPDWN2.override = 3
        phy.profile_outputs.AGC_STEPDWN_STEPDWN3.override = 4
        phy.profile_outputs.AGC_STEPDWN_STEPDWN4.override = 5
        phy.profile_outputs.AGC_STEPDWN_STEPDWN5.override = 6

        phy.profile_outputs.MODEM_AFC_AFCRXCLR.override = 1


    ### override inheritance from Ocelot
    def ZWave_100kbps_912MHz_OQPSK(self, phy, model):

        # force adc_clock_mode to XO - for Ocelot this is already the default - only doing this for Bobcat
        model.vars.adc_clock_mode.value_forced = model.vars.adc_clock_mode.var_enum.HFXOMULT

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
        # ZWave LR is intended to be 802.15.4 like in syncword, symbol endianness, except for a different DSSS code
        # syncword_0 = {b7b6b5b4b3b2b1b0} = 0b0111 1010 = 0x7A
        # SYNC0 = bitreverse(syncword_0) = 0x5E = {0101}{1110} = {b7b6b5b4}{b3b2b1b0} = {s1}{s0}
        # least significant symbol s0 is sent first
        phy.profile_inputs.syncword_0.value = long(0x7A) # Figure 12-3 - Format of SFD field, 802.15.4-2015.pdf
        phy.profile_inputs.syncword_1.value = long(0x0)
        phy.profile_inputs.syncword_length.value = 8
        phy.profile_inputs.tx_xtal_error_ppm.value = 27
        phy.profile_inputs.xtal_frequency_hz.value = 39000000
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
        phy.profile_outputs.MODEM_DIGIGAINCTRL_DIGIGAINEN.override = 1
        phy.profile_outputs.MODEM_DIGIGAINCTRL_DIGIGAINSEL.override = 20
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

        # Frame format for LR as per Z-Wave LR Draft SDS14639 2020-01-21
        phy.profile_inputs.frame_bitendian.value = model.vars.frame_bitendian.var_enum.LSB_FIRST

        phy.profile_inputs.payload_white_en.value = False
        phy.profile_inputs.payload_crc_en.value = True
        phy.profile_inputs.payload_addtrailtxdata_en.value = False

        # Header - FCD0 - must be before DFL definition
        # FCD0 / header is used for DFL extraction, it is not meant for MHR / Payload separation
        phy.profile_inputs.header_en.value = True
        phy.profile_inputs.header_size.value = 8 # Frame byte, 1 indexed
        phy.profile_inputs.header_calc_crc.value = True
        phy.profile_inputs.header_white_en.value = False
        phy.profile_inputs.header_addtrailtxdata_en.value = False
        phy.profile_inputs.header_excludesubframewcnt_en.value = False

        # dynamic frame length - section 6.1.1 - must be after FCD0 definition
        phy.profile_inputs.frame_length_type.value = model.vars.frame_length_type.var_enum.VARIABLE_LENGTH
        # WCNTCMPx need -1 to account for internal representation
        phy.profile_inputs.var_length_numbits.value = 8 # bitwidth

        phy.profile_inputs.var_length_minlength.value = 2 # 2 for crc length, calculator accounts for header length
        phy.profile_inputs.var_length_maxlength.value = 192 - phy.profile_inputs.header_size.value # 192 from LR spec
        phy.profile_inputs.var_length_shift.value = 0
        phy.profile_inputs.var_length_includecrc.value = True
        phy.profile_inputs.var_length_byteendian.value = model.vars.var_length_byteendian.var_enum.MSB_FIRST
        phy.profile_inputs.var_length_bitendian.value = model.vars.var_length_bitendian.var_enum.LSB_FIRST
        # DFLOFFSET calculation does not handle length bytes not at the end of header, as in Z-Wave
        # furthermore, does not account for signedness and register width. MCUW_RADIO_CFG-642, MCUW_RADIO_CFG-648
        # phy.profile_inputs.var_length_adjust.value = 8  # dfloffset, 4-bit, 2's complement
        # signed(15, 4b) = -1, which is what is needed to compare correctly against wcnt
        phy.profile_outputs.FRC_DFLCTRL_DFLOFFSET.override = 15

        # CRC
        phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.CCITT_16
        phy.profile_inputs.crc_seed.value = long(0x1d0f)
        phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.MSB_FIRST
        phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.LSB_FIRST
        phy.profile_inputs.crc_input_order.value = model.vars.crc_input_order.var_enum.MSB_FIRST
        phy.profile_inputs.crc_pad_input.value = False
        phy.profile_inputs.crc_invert.value = False

        # misc
        phy.profile_inputs.frame_type_loc.value = 0
        phy.profile_inputs.frame_type_bits.value = 0
        phy.profile_inputs.frame_type_lsbit.value = 0
        phy.profile_inputs.frame_type_0_length.value = 0
        phy.profile_inputs.frame_type_1_length.value = 0
        phy.profile_inputs.frame_type_2_length.value = 0
        phy.profile_inputs.frame_type_3_length.value = 0
        phy.profile_inputs.frame_type_4_length.value = 0
        phy.profile_inputs.frame_type_5_length.value = 0
        phy.profile_inputs.frame_type_6_length.value = 0
        phy.profile_inputs.frame_type_7_length.value = 0

        return phy

    def PHY_ZWave_100kbps_912MHz_OQPSK(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='PHY_ZWave_100kbps_912MHz_OQPSK for Bobcat', phy_name=phy_name)
        phy.profile_inputs.antdivmode.value = model.vars.antdivmode.var_enum.DISABLE
        self.ZWave_100kbps_912MHz_OQPSK(phy, model)

        # Forcing to VCODIV HALFRATE for Bobcat
        model.vars.adc_clock_mode.value_forced = model.vars.adc_clock_mode.var_enum.VCODIV
        model.vars.adc_rate_mode.value_forced = model.vars.adc_rate_mode.var_enum.HALFRATE

        # FIXME - added for enabling dynamic frame control
        phy.profile_inputs.var_length_numbits.value = 8
        phy.profile_inputs.var_length_includecrc.value = True
        phy.profile_outputs.FRC_WCNTCMP1_LENGTHFIELDLOC.override = 7

        # Overrides for the SQ block
        phy.profile_outputs.MODEM_SQ_SQEN.override = 1
        T_b = 1.25e-6
        phy.profile_outputs.MODEM_SQ_SQTIMOUT.override = int(250e-6 / T_b)  # Timeout before checking for timing detect. Units: T_b. Zwave_LR_timing_21.xlsx:I13
        phy.profile_outputs.MODEM_SQEXT_SQSTG2TIMOUT.override = int(3.2e-3 / T_b) # Timeout after timing detect to check for preamble. Units T_b.
        phy.profile_outputs.MODEM_SQEXT_SQSTG3TIMOUT.override = 16 * 8 # Timeout after preamble to check for sync word. Units T_b. Cell B25

        # Reduce HICNTREGINON0 to speed up AGC when near upper threshold
        phy.profile_outputs.AGC_HICNTREGION0_HICNTREGION0.override = 10
        phy.profile_outputs.AGC_STEPDWN_STEPDWN0.override = 1
        phy.profile_outputs.AGC_STEPDWN_STEPDWN1.override = 2
        phy.profile_outputs.AGC_STEPDWN_STEPDWN2.override = 3
        phy.profile_outputs.AGC_STEPDWN_STEPDWN3.override = 4
        phy.profile_outputs.AGC_STEPDWN_STEPDWN4.override = 5
        phy.profile_outputs.AGC_STEPDWN_STEPDWN5.override = 6

        # Increase threshold and delay to reduce false detections
        phy.profile_outputs.MODEM_COH3_DYNIIRCOEFOPTION.override = 1
        phy.profile_outputs.MODEM_COH2_FIXEDCDTHFORIIR.override = 140
        phy.profile_outputs.MODEM_LONGRANGE6_LRSPIKETHD.override = 140
        phy.profile_outputs.AGC_GAINSTEPLIM0_CFLOOPDEL.override = 127

        # Disable TX side AFC adjustments.
        phy.profile_outputs.MODEM_AFCADJTX_AFCSCALEM.override = 0
        phy.profile_outputs.MODEM_AFCADJTX_AFCSCALEE.override = 0

        return phy

    def PHY_ZWave_100kbps_912MHz_OQPSK_beam(self, model, phy_name=None):
        pass

    def PHY_ZWave_100kbps_916MHz(self, model, phy_name=None):
        pass

    def PHY_ZWave_100kbps_916MHz_DSA_Beam_Passthrough(self, model):
        pass

    def PHY_ZWave_100kbps_916MHz_DSA_Beam_Passthrough_lowside(self, model):
        pass

    def PHY_ZWave_100kbps_916MHz_lowside(self, model, phy_name=None):
        pass

    def PHY_ZWave_100kbps_916MHz_viterbi(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='PHY_ZWave_100kbps_916MHz_viterbi for Bobcat', phy_name=phy_name)
        self.ZWave_100kbps_base(phy, model)

        # Forcing to VCODIV HALFRATE for Bobcat
        model.vars.adc_clock_mode.value_forced = model.vars.adc_clock_mode.var_enum.VCODIV
        model.vars.adc_rate_mode.value_forced = model.vars.adc_rate_mode.var_enum.HALFRATE

        phy.profile_inputs.xtal_frequency_hz.value = 39000000
        # phy.profile_inputs.rx_xtal_error_ppm.value = 20
        # phy.profile_inputs.tx_xtal_error_ppm.value = 20

        phy.profile_inputs.demod_select.value = model.vars.demod_select.var_enum.TRECS_SLICER
        phy.profile_inputs.base_frequency_hz.value = long(916000000)

        phy.profile_inputs.symbols_in_timing_window.value = 12
        phy.profile_inputs.syncword_0.value = long(0x55F0)
        phy.profile_inputs.syncword_length.value = 16
        # sync word is 8 bits, rest are taken from the preamble.
        phy.profile_inputs.preamble_length.value = phy.profile_inputs.preamble_length.value -  phy.profile_inputs.syncword_length.value + 8
        phy.profile_inputs.symbols_in_timing_window.value = 12

        phy.profile_outputs.MODEM_TRECPMPATT_PMEXPECTPATT.override = long(2863311530)
        phy.profile_outputs.MODEM_TRECPMDET_PMMINCOSTTHD.override = 200

        phy.profile_outputs.MODEM_VTCORRCFG1_KSI3SWENABLE.override = 1
#        phy.profile_outputs.MODEM_VTCORRCFG1_VITERBIKSI3WB.override = 44
#        phy.profile_outputs.MODEM_VITERBIDEMOD_VITERBIKSI3.override = 42

        # forcing wider bw_acq to improve freq offset performance
        phy.profile_inputs.bandwidth_hz.value = 210000

        # Overrides for the SQ block
        phy.profile_outputs.MODEM_SQ_SQEN.override = 1
        phy.profile_outputs.MODEM_SQ_SQTIMOUT.override = int(250 / 10)  # Timeout before checking for timing detect. Units: T_b.
        phy.profile_outputs.MODEM_SQEXT_SQSTG2TIMOUT.override = phy.profile_inputs.preamble_length.value - (phy.profile_inputs.syncword_length.value - 8)
        phy.profile_outputs.MODEM_SQEXT_SQSTG3TIMOUT.override = phy.profile_inputs.syncword_length.value +16

        # Reduce HICNTREGINON0 to speed up AGC when near upper threshold
        phy.profile_outputs.AGC_HICNTREGION0_HICNTREGION0.override = 10

        # Disable TX side AFC adjustments.
        phy.profile_outputs.MODEM_AFCADJTX_AFCSCALEM.override = 0
        phy.profile_outputs.MODEM_AFCADJTX_AFCSCALEE.override = 0

        phy.profile_outputs.AGC_STEPDWN_STEPDWN0.override = 1
        phy.profile_outputs.AGC_STEPDWN_STEPDWN1.override = 2
        phy.profile_outputs.AGC_STEPDWN_STEPDWN2.override = 3
        phy.profile_outputs.AGC_STEPDWN_STEPDWN3.override = 4
        phy.profile_outputs.AGC_STEPDWN_STEPDWN4.override = 5
        phy.profile_outputs.AGC_STEPDWN_STEPDWN5.override = 6

        phy.profile_outputs.MODEM_PHDMODCTRL_REMODEN.override = 0
        phy.profile_outputs.MODEM_PHDMODCTRL_REMODDWN.override = 0

        phy.profile_outputs.MODEM_TRECPMDET_PMCOSTVALTHD.override = 2

        return phy

    def PHY_ZWave_100kbps_916MHz_viterbi_beam(self, model, phy_name=None):
        pass

    def PHY_ZWave_100kbps_916MHz_viterbi_lowside(self, model, phy_name=None):
        pass

    def PHY_ZWave_100kbps_916MHz_viterbi_lowside_beam(self, model, phy_name=None):
        pass

    def PHY_ZWave_40kbps_908MHz(self, model, phy_name=None):
        pass

    def PHY_ZWave_40kbps_908MHz_viterbi(self, model, phy_name=None):
        pass

    def PHY_ZWave_40kbps_908MHz_viterbi_beam(self, model, phy_name=None):
        pass

    def PHY_ZWave_40kbps_9p6kbps_908MHz_legacy_conc(self, model, phy_name=None):
        pass

    def PHY_ZWave_40kbps_9p6kbps_908MHz_viterbi_conc(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, 'PHY_ZWave_40kbps_9p6kbps_908MHz_viterbi_conc on Bobcat')
        self.ZWave_40kbps_908MHz_viterbi_base(phy, model)
        self.ZWave_9p6kbps_908MHz_bcr_conc_base(phy, model)

        # RAIL overrides the RF to the correct region frequency and concurrent frequency
        # This concurrent PHY from the calculator transmits and receives on the standard R2 RF.
        phy.profile_inputs.base_frequency_hz.value = long(908400000)

        # FULLRATE needed to find DFE configuration (allowing bwsel_max=0.5 is alternate for Halfrate)
        model.vars.adc_clock_mode.value_forced = model.vars.adc_clock_mode.var_enum.VCODIV
        model.vars.adc_rate_mode.value_forced = model.vars.adc_rate_mode.var_enum.FULLRATE

        phy.profile_outputs.MODEM_PHDMODCTRL_BCRTRECSCONC.override = 1
        phy.profile_outputs.MODEM_FRMSCHTIME_DSARSTSYCNEN.override = 1 # Needed for concurrent demod for PGOCELOT-5282

        # Overrides for the SQ block
        phy.profile_outputs.MODEM_SQ_SQEN.override = 1
        phy.profile_outputs.MODEM_SQ_SQTIMOUT.override = int(1800 / 25) # Timeout before checking for timing detect. Units: T_b.
        phy.profile_outputs.MODEM_SQEXT_SQSTG2TIMOUT.override = 80 # Timeout after timing detect to check for preamble. Units T_b.
        phy.profile_outputs.MODEM_SQEXT_SQSTG3TIMOUT.override = 16 # Timeout after preamble to check for sync word. Units T_b.
        phy.profile_outputs.MODEM_TRECPMDET_PMCOSTVALTHD.override = 2

        return phy

    def PHY_ZWave_9p6kbps_908MHz(self, model, phy_name=None):
        pass

    def PHY_ZWave_9p6kbps_908MHz_bcr(self, model, phy_name=None):
        pass

    def PHY_ZWave_9p6kbps_908MHz_bcr_conc(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, 'PHY_ZWave_40kbps_9p6kbps_908MHz_viterbi_conc on Bobcat')
        self.ZWave_40kbps_908MHz_viterbi_base(phy, model)
        self.ZWave_9p6kbps_908MHz_bcr_conc_base(phy, model)

        # RAIL overrides the RF to the correct region frequency and concurrent frequency
        # This concurrent PHY from the calculator transmits and receives on the standard R1 RF.
        phy.profile_inputs.base_frequency_hz.value = long(908420000)

        # FULLRATE needed to find DFE configuration (allowing bwsel_max=0.5 is alternate for Halfrate)
        model.vars.adc_clock_mode.value_forced = model.vars.adc_clock_mode.var_enum.VCODIV
        model.vars.adc_rate_mode.value_forced = model.vars.adc_rate_mode.var_enum.FULLRATE

        # overrides from PHY_ZWave_40kbps_9p6kbps_908MHz_viterbi_conc to convert to 9.6kbps
        phy.profile_inputs.preamble_pattern.value = 6
        phy.profile_inputs.preamble_pattern_len.value = 4
        phy.profile_inputs.preamble_length.value = 160
        phy.profile_outputs.MODEM_PHDMODCTRL_BCRTRECSCONC.override = 1
        phy.profile_outputs.MODEM_FRMSCHTIME_DSARSTSYCNEN.override = 1 # Needed for concurrent demod for PGOCELOT-5282
        phy.profile_outputs.MODEM_SQ_SQEN.override = 1
        phy.profile_outputs.MODEM_SQ_SQTIMOUT.override = int(1800 / 25) # Timeout before checking for timing detect. Units: T_b.
        phy.profile_outputs.MODEM_SQEXT_SQSTG2TIMOUT.override = 160 # Timeout before checking for preamble in chips
        phy.profile_outputs.MODEM_SQEXT_SQSTG3TIMOUT.override = 16 # Timeout after preamble to check for sync word. Units T_b.

        # for registers to get BCR demod to work properly
        phy.profile_outputs.MODEM_SRCCHF_SRCENABLE2.override = 0
        phy.profile_outputs.MODEM_MODINDEX_MODINDEXM.override = 26
        phy.profile_outputs.MODEM_TXBR_TXBRDEN.override = 128
        phy.profile_outputs.MODEM_TXBR_TXBRNUM.override = 32500
        phy.profile_outputs.MODEM_BCRDEMODCTRL_BBPMDETEN.override = 1
        phy.profile_outputs.MODEM_TRECPMPATT_PMEXPECTPATT.override = 0x33333333
        phy.profile_outputs.MODEM_SRCCHF_SRCENABLE2.override = 0
        phy.profile_outputs.MODEM_CTRL0_CODING.override = 1
        phy.profile_outputs.MODEM_CTRL1_SYNCBITS.override = 31
        phy.profile_outputs.MODEM_SYNC0_SYNC0.override = 0xAA556666
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0

        # force the following to keep register differences between PHY_ZWave_40kbps_9p6kbps_908MHz_viterbi_conc and
        # this phy at minimum. not required for proper function - add/remove/modify these as the other PHY changes
        # Diffing between the two concurrent PHYs should only have registers diffs in
        # {MODEM_BCRDEMODCTRL_BBPMDETEN, MODEM_CTRL0_CODING, MODEM_CTRL0_MAPFSK, MODEL_CTRL1_SYNCBITS, MODEM_FRMSCHTIME_FRMSCHTIME,
        # MODEM_PRE_{BASE,BASEBITS,TXBASES}, MODEM_SRCCHF.SRCENABLE2, MODEM_SYNC0_SYNC0, MODEM_TIMING_TIMINGBASES, MODEM_TRECPMPATT_PMEXPECTPATT, MODEM_TXBR, SYNTH_FREQ}
        phy.profile_outputs.AGC_CTRL7_SUBDEN.override = 0
        phy.profile_outputs.AGC_CTRL7_SUBINT.override = 0
        phy.profile_outputs.AGC_CTRL7_SUBNUM.override = 0
        phy.profile_outputs.AGC_CTRL7_SUBPERIOD.override = 0
        phy.profile_outputs.MODEM_AFC_AFCDEL.override = 5
        phy.profile_outputs.MODEM_MODINDEX_FREQGAINE.override = 1
        phy.profile_outputs.MODEM_MODINDEX_FREQGAINM.override = 1
        phy.profile_outputs.MODEM_PHDMODCTRL_REMODOSR.override = 6
        phy.profile_outputs.MODEM_RXBR_RXBRINT.override = 3
        phy.profile_outputs.MODEM_REALTIMCFE_MINCOSTTHD.override = 300
        phy.profile_outputs.MODEM_REALTIMCFE_SYNCACQWIN.override = 3
        phy.profile_outputs.MODEM_VTCORRCFG0_EXPECTPATT.override = 2853175295
        phy.profile_outputs.MODEM_VTCORRCFG1_EXPSYNCLEN.override = 142
        phy.profile_outputs.MODEM_VTCORRCFG1_EXPECTHT.override = 11

        phy.profile_outputs.MODEM_BCRCTRL0_CRSLOW.override = 2
        phy.profile_outputs.MODEM_BCRDEMODARR0_SCHPRDHI.override = 0

        phy.profile_outputs.MODEM_AFCADJRX_AFCSCALEE.override = 1
        phy.profile_outputs.MODEM_AFCADJRX_AFCSCALEM.override = 25
        phy.profile_outputs.MODEM_BCRDEMODCTRL_PREATH.override = 20

        return phy

    def PHY_ZWave_9p6kbps_908MHz_viterbi(self, model, phy_name=None):
        pass
