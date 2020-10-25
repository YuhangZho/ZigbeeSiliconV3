from pyradioconfig.parts.common.calculators.calc_frame_detect import CALC_Frame_Detect
from py_2_and_3_compatibility import *

class CALC_Frame_Detect_Ocelot(CALC_Frame_Detect):

    def calc_txsync_reg(self, model):
        #This function calculates what syncword to transmit when using dualsync

        #Read in model variables
        dualsync = model.vars.syncword_dualsync.value
        fec_enabled = model.vars.fec_enabled.value

        #Use sync0 for TX unless using WiSUN with FEC
        if model.profile.name.lower() == 'wisun' and dualsync:
            if fec_enabled:
                txsync = 1 #syncword 1 is always programmed with the coded value
            else:
                txsync = 0
        else:
            txsync = 0

        #Write the register
        self._reg_write(model.vars.MODEM_CTRL1_TXSYNC, txsync)

    def calc_syncerrors_reg(self, model):
        #This function calulates the SYNCERRORS field

        #Read in model variables
        demod_select = model.vars.demod_select.value
        rtschmode_actual = model.vars.MODEM_REALTIMCFE_RTSCHMODE.value

        #Allow 1 sync error if using TRECS and RTSCHMODE = 1 (hard slicing instead of CFE)
        if demod_select == model.vars.demod_select.var_enum.TRECS_VITERBI or \
                demod_select == model.vars.demod_select.var_enum.TRECS_SLICER:
            if rtschmode_actual == 1:
                syncerrors = 1
            else:
                syncerrors = 0
        else:
            syncerrors = 0

        #Write the register
        self._reg_write(model.vars.MODEM_CTRL1_SYNCERRORS, syncerrors)

    def calc_timthresh_value(self, model):
        #This function calculates the timing threshold

        # Load model values into local variables
        mod_type = model.vars.modulation_type.value
        demod_rate_actual = model.vars.demod_rate_actual.value
        deviation = model.vars.deviation.value
        freq_gain_actual = model.vars.freq_gain_actual.value
        timing_window_actual = model.vars.timing_window_actual.value
        timthresh_gain_actual = model.vars.timing_detection_threshold_gain_actual.value
        symbol_enconding_type = model.vars.symbol_encoding.value
        dsss_len = model.vars.dsss_len.value
        dsss_sf = model.vars.dsss_spreading_factor.value

        if (mod_type == model.vars.modulation_type.var_enum.OOK):
            timthresh = 0
        elif (mod_type == model.vars.modulation_type.var_enum.OQPSK):
            # : semi-empirical calculation based on Ocelot OQPSK DSSS investigation
            # : For DSSS, timing window is always 1 symbol long. Number of bits used in correlation is
            # : 1 symbol x DSSS Length = # of bits
            # : The noise in the correlation therefore depends on DSSS length. The actual noise level is
            # : based on dsss length times scale factor
            # : https://jira.silabs.com/browse/MCUW_RADIO_CFG-1212
            if (symbol_enconding_type == model.vars.symbol_encoding.var_enum.DSSS):
                # : noise scale factor. Value is based on RTL simulation and generating a histogram of max_corr.
                # : Factor is set such that threshold is at 99 percentile of max_corr
                noise_scale_factor = 110
                nominal_decision = noise_scale_factor * math.log2(dsss_len)
                timthresh = int(math.ceil(nominal_decision / timthresh_gain_actual))
            else:
                timthresh = 60
        else:
            nominal_decision = 2.0 * deviation / demod_rate_actual * 128 * freq_gain_actual
            # FIXME: arbitrary scaling of 3 here. Should this be function of the sensitivity?
            timthresh = int(round(timing_window_actual * nominal_decision / timthresh_gain_actual / 3))

        # Load local variables back into model variables
        model.vars.timing_detection_threshold.value = timthresh


    def calc_timthresh_gain_actual(self,model):
        #This function calculates the actual timing threshold gain based on the register value

        #Load model values into local variables
        timthresh_gain_reg =  model.vars.MODEM_CTRL6_TIMTHRESHGAIN.value

        timthresh_gain_actual = 2**(timthresh_gain_reg+3)

        #Load local variables back into model variables
        model.vars.timing_detection_threshold_gain_actual.value = timthresh_gain_actual

    def calc_timbases_val(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        preamblebits = model.vars.preamble_length.value
        modformat = model.vars.modulation_type.value
        basebits = model.vars.preamble_pattern_len_actual.value
        encoding = model.vars.symbol_encoding.value
        vtdemoden = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value
        demod_select = model.vars.demod_select.value
        preamschlen = model.vars.preamsch_len.value
        baudrate_tol_ppm = model.vars.baudrate_tol_ppm.value
        agc_settling_delay = model.vars.agc_settling_delay.value
        osr = model.vars.oversampling_rate_actual.value

        if model.vars.asynchronous_rx_enable.value is True:
            # When asynchronous direct mode is enabled set to max
            timingbases = 15

        else:
            # Not asynchronous mode

            if encoding == model.vars.symbol_encoding.var_enum.DSSS:
                # Unique timing window settings are required for PHYs that use DSSS

                # : For coherent demod, timing base of 3 seems to work regardless of preamble length
                if demod_select == model.vars.demod_select.var_enum.COHERENT:
                    # : OQPSK uses 3 symbols for timing detect if coherent demod is enabled
                    if modformat == model.vars.modulation_type.var_enum.OQPSK:
                        timingbases = 3
                    # : BPSK uses 8 symbols for timing detect
                    elif modformat == model.vars.modulation_type.var_enum.DBPSK:
                        timingbases = 8
                    else: # modulation format is not supported by COHERENT demod. Follow DSSS default
                        timingbases = 1
                else:
                    # for DSSS set to 1
                    timingbases = 1

            else:
                # Not DSSS

                if vtdemoden:
                    # When using TRECS, use the calculated preamble search length
                    timingbases = preamschlen//basebits

                else:
                    #Default Legacy Demod calculation

                    #If cfloopdel is set correctly, then the first timing window after the cfloopdel period will be valid
                    cfloopdel_symbols = int(round(agc_settling_delay/osr))
                    remaining_pre_symbols = preamblebits - cfloopdel_symbols

                    if remaining_pre_symbols >=4 or modformat == model.vars.modulation_type.var_enum.FSK4:

                        if baudrate_tol_ppm >= 1000:
                            #Maximum timing window size is 8 to allow for more frequent resynchronization
                            max_timingbases = 8//basebits
                        else:
                            #Maximum timing window size is 16
                            max_timingbases = 16//basebits

                        #Use fixed window if we can make it 4 bits or larger
                        timingbases = min(remaining_pre_symbols//basebits,max_timingbases)

                    else:
                        #Use sliding window (FDM0)
                        timingbases = 0

        # Calculate the final timing window size and write to model variable
        timing_window_size = int(timingbases * basebits)
        model.vars.symbols_in_timing_window.value = timing_window_size


    def calc_timbases_reg(self, model):
        """

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        timingwindow = model.vars.symbols_in_timing_window.value * 1.0
        basebits = model.vars.preamble_pattern_len_actual.value

        if model.vars.ber_force_fdm0.value == True:
            timingbases = 0
        else:
            timingbases = int(math.ceil(timingwindow / basebits))

        if timingbases > 15:
            timingbases = 15

        self._reg_write(model.vars.MODEM_TIMING_TIMINGBASES,  timingbases)

    def calc_addtimseq_val(self, model):
        """
        calculate additional timing sequences to detect given preamble length
        the equation used to calcualte ADDTIMSEQ is derived emprically and might need
        tweaking as we have more PHY providing additional data

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        preamble_pattern_bits = model.vars.preamble_pattern_len_actual.value
        preamblebits = model.vars.preamble_length.value * 1.0
        timingbases = model.vars.timingbases_actual.value
        timingwindow = model.vars.timing_window_actual.value
        mod_format = model.vars.modulation_type.value
        demod_select = model.vars.demod_select.value
        symbol_enconding_type = model.vars.symbol_encoding.value

        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            # Process first aligned window 6 times for preamble search for coherent demod based on Ocelot measurements.
            addtimseq = 6
        elif mod_format == model.vars.modulation_type.var_enum.OQPSK \
                and symbol_enconding_type == model.vars.symbol_encoding.var_enum.DSSS:
            # : determine number of symbols in preamble
            num_sym_in_preamble = preamblebits / preamble_pattern_bits
            # : Timing window for DSSS is always 1 symbol. Allow number of timing windows to detect to be up to
            # : quarter of preamble. The final ratio between preamble length and number of timing windows may need
            # : to be tweaked based on additional investigations.
            addtimseq = int(round(num_sym_in_preamble / 4.0))-1
        elif mod_format == model.vars.modulation_type.var_enum.OOK:
            # Always use 1 timing window for OOK
            addtimseq = 0
        else:
            if timingbases > 1:
                # Figure out how many timing windows fit in the preamble. Assume one is throwaway.
                addtimseq = math.floor(preamblebits / timingwindow) - 2
            else:
                addtimseq = 0

        # saturate addtimseq to fit into 4 bits
        if addtimseq > 15:
            addtimseq = 15

        if addtimseq < 0:
            addtimseq = 0

        model.vars.number_of_timing_windows.value = int(addtimseq) + 1

    def calc_tsamplim_val(self, model):

        modformat = model.vars.modulation_type.value
        preamblebits = model.vars.preamble_length.value

        if model.vars.asynchronous_rx_enable.value is True:
            # for asynchronous direct mode we don't want the demod to change states so
            # keep the thresholds at the upper limit
            th = 65535
        else:
            if modformat == model.vars.modulation_type.var_enum.OOK or \
                modformat == model.vars.modulation_type.var_enum.ASK:
                # for amplitude modulated signal we need to turn off TSAMPMODE as enabling it
                # switches the slicer level from FREQOFFESTLIM to TSAMPLIM which we don't want.
                th = 0
            else:
                if preamblebits >= 32:
                    th = 0  # [MCUW_RADIO_CFG-1077] If we have at least 32 preamble bits then correlation is robust
                else:
                    # nominal threshold of 10 seems to work well for most PHYs
                    th = 10

        model.vars.timing_sample_threshold.value = th

    def calc_preerrors_val(self, model):
        """
        calculate PREERRORS field

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        # FIXME: consider adding +1 to errors when AFC is enbabled - seems to work better

        dssslen = model.vars.dsss_len_actual.value
        in_2fsk_opt_scope = model.vars.in_2fsk_opt_scope.value
        baudrate = model.vars.baudrate.value
        demod_select = model.vars.demod_select.value

        if dssslen == 0:
            if in_2fsk_opt_scope and baudrate > 1900000:
                preerrors = 1
            else:
                preerrors = 0
        else:
            preerrors = dssslen / 2.0

        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            # : For coherent demod, set to maximum value in order to disable this feature.
            preerrors = 15

        # make sure we fit into 4 bits
        if preerrors > 15:
            preerrors = 15

        model.vars.errors_in_timing_window.value = int(round(preerrors))

    def calc_allow_received_window(self, model):
        demod_select = model.vars.demod_select.value

        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            # : Always allow received windows for coherent demod
            self._reg_write(model.vars.MODEM_CTRL6_ARW, 1)
        else:
            # : allow received windows when window size is less than half of RAM size
            self._reg_write(model.vars.MODEM_CTRL6_ARW, 0)

    def calc_baud_rewind_after_timing_detect(self, model):
        demod_select = model.vars.demod_select.value
        dsss_sf = model.vars.dsss_spreading_factor.value
        mod_format = model.vars.modulation_type.value

        if demod_select == model.vars.demod_select.var_enum.COHERENT and \
                mod_format == model.vars.modulation_type.var_enum.OQPSK:
            # : For coherent oqpsk, rewind AFC window by 2 symbols from the initial timing window
            number_of_symbols_to_rewind = 2
            number_of_bauds_to_rewind = int(4 * dsss_sf * number_of_symbols_to_rewind)
        else:
            number_of_bauds_to_rewind = 0

        self._reg_write(model.vars.MODEM_CTRL6_TDREW, number_of_bauds_to_rewind)

    def calc_dsss_payload_correlation_detection_mode(self, model):
        demod_select = model.vars.demod_select.value

        if demod_select == model.vars.demod_select.var_enum.COHERENT:
            # : When this bit is set, correlation threshold is only used until preamble is detected
            # : after preamble detection, only detected symbol is used to qualify a valid preamble.
            self._reg_write(model.vars.MODEM_CTRL5_DSSSCTD, 1)
        else:
            self._reg_write(model.vars.MODEM_CTRL5_DSSSCTD, 0)


    def calc_dynamic_timing_thresholds(self, model):
        demod_select = model.vars.demod_select.value
        mod_format = model.vars.modulation_type.value
        bbss_transition_threhold_1 = model.vars.MODEM_LONGRANGE2_LRCHPWRTH1.value
        bbss_transition_threhold_2 = model.vars.MODEM_LONGRANGE2_LRCHPWRTH2.value
        bbss_transition_threhold_3 = model.vars.MODEM_LONGRANGE2_LRCHPWRTH3.value

        average_transition_threhold = int(round((bbss_transition_threhold_1+bbss_transition_threhold_2)/2.0))

        if hasattr(model.profiles, 'Long_Range'):
            is_long_range = model.profile == model.profiles.Long_Range
        else:
            is_long_range = False

        """ Set dynamic threshold modes """
        if demod_select == model.vars.demod_select.var_enum.COHERENT and \
                        mod_format == model.vars.modulation_type.var_enum.OQPSK:
            """ Set timing threshold gain """
            self._reg_write(model.vars.MODEM_CTRL6_TIMTHRESHGAIN, 2)

            """ Enable dynamic preamble and sync thresholds """
            # : Enable dynamic preamble threshold
            self._reg_write(model.vars.MODEM_COH0_COHDYNAMICPRETHRESH, 1)
            # : Set dynamic preamble threshold to 1x sync threshold
            self._reg_write(model.vars.MODEM_COH0_COHDYNAMICPRETHRESHSEL, 0)

            # : Disable static sync threshold and enable dynamic sync threshold
            self._reg_write(model.vars.MODEM_SYNCPROPERTIES_STATICSYNCTHRESHEN, 0)
            self._reg_write(model.vars.MODEM_COH0_COHDYNAMICSYNCTHRESH, 1)

            """ Enforce qualifications for valid preamble detect """
            self._reg_write(model.vars.MODEM_CTRL5_LINCORR, 1)
            self._reg_write(model.vars.MODEM_CTRL6_PSTIMABORT0, 1)
            self._reg_write(model.vars.MODEM_CTRL6_PSTIMABORT1, 1)
            self._reg_write(model.vars.MODEM_CTRL6_PSTIMABORT2, 1)
            self._reg_write(model.vars.MODEM_CTRL6_PSTIMABORT3, 0)
        else:
            self._reg_write(model.vars.MODEM_CTRL6_TIMTHRESHGAIN, 0)
            self._reg_write(model.vars.MODEM_COH0_COHDYNAMICPRETHRESH, 0)
            self._reg_write(model.vars.MODEM_COH0_COHDYNAMICPRETHRESHSEL, 0)
            self._reg_write(model.vars.MODEM_SYNCPROPERTIES_STATICSYNCTHRESHEN, 0)
            self._reg_write(model.vars.MODEM_COH0_COHDYNAMICSYNCTHRESH, 0)
            self._reg_write(model.vars.MODEM_CTRL5_LINCORR, 0)
            self._reg_write(model.vars.MODEM_CTRL6_PSTIMABORT0, 0)
            self._reg_write(model.vars.MODEM_CTRL6_PSTIMABORT1, 0)
            self._reg_write(model.vars.MODEM_CTRL6_PSTIMABORT2, 0)
            self._reg_write(model.vars.MODEM_CTRL6_PSTIMABORT3, 0)

        if demod_select == model.vars.demod_select.var_enum.COHERENT and is_long_range:
            self._reg_write(model.vars.MODEM_CTRL6_TIMTHRESHGAIN, 2)
            """ Where to begin new region in terms of channel power """
            self._reg_write(model.vars.MODEM_COH0_COHCHPWRTH0, average_transition_threhold)
            self._reg_write(model.vars.MODEM_COH0_COHCHPWRTH1, bbss_transition_threhold_2)
            self._reg_write(model.vars.MODEM_COH0_COHCHPWRTH2, bbss_transition_threhold_3)

            """ Starting sync threshold for each region """
            self._reg_write(model.vars.MODEM_COH1_SYNCTHRESH0, 27)
            self._reg_write(model.vars.MODEM_COH1_SYNCTHRESH1, 30)
            self._reg_write(model.vars.MODEM_COH1_SYNCTHRESH2, 33)
            self._reg_write(model.vars.MODEM_COH1_SYNCTHRESH3, 80)

            """ Slopes of each region """
            self._reg_write(model.vars.MODEM_COH2_SYNCTHRESHDELTA0, 0)
            self._reg_write(model.vars.MODEM_COH2_SYNCTHRESHDELTA1, 2)
            self._reg_write(model.vars.MODEM_COH2_SYNCTHRESHDELTA2, 4)
            self._reg_write(model.vars.MODEM_COH2_SYNCTHRESHDELTA3, 0)
        else:
            self._reg_write(model.vars.MODEM_COH0_COHCHPWRTH0, 0)
            self._reg_write(model.vars.MODEM_COH0_COHCHPWRTH1, 0)
            self._reg_write(model.vars.MODEM_COH0_COHCHPWRTH2, 0)
            self._reg_write(model.vars.MODEM_COH1_SYNCTHRESH0, 0)
            self._reg_write(model.vars.MODEM_COH1_SYNCTHRESH1, 0)
            self._reg_write(model.vars.MODEM_COH1_SYNCTHRESH2, 0)
            self._reg_write(model.vars.MODEM_COH1_SYNCTHRESH3, 0)
            self._reg_write(model.vars.MODEM_COH2_SYNCTHRESHDELTA0, 0)
            self._reg_write(model.vars.MODEM_COH2_SYNCTHRESHDELTA1, 0)
            self._reg_write(model.vars.MODEM_COH2_SYNCTHRESHDELTA2, 0)
            self._reg_write(model.vars.MODEM_COH2_SYNCTHRESHDELTA3, 0)

