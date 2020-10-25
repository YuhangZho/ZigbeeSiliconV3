from pyradioconfig.parts.lynx.calculators.calc_viterbi import CALC_Viterbi_lynx
from math import *
from py_2_and_3_compatibility import *
from pycalcmodel.core.variable import ModelVariableFormat

class CALC_Viterbi_ocelot(CALC_Viterbi_lynx):

    def buildVariables(self, model):

        #Call the inherited buildVariables
        super().buildVariables(model)

        #Add some variables
        self._addModelVariable(model,'trecs_pre_bits_to_syncword',int,ModelVariableFormat.DECIMAL,desc='Number of preamble bits to move to syncword with TRECS'	)
        self._addModelVariable(model, 'trecs_effective_preamble_len', int, ModelVariableFormat.DECIMAL,desc='TRECS preamble length minus bits shifted to syncword')
        self._addModelVariable(model, 'trecs_effective_syncword_len', int, ModelVariableFormat.DECIMAL,desc='TRECS syncword length plus bits shifted from preamble')

    # define constants
    MIN_COST_THD_FULL = 600 # threshold for min cost function when using full 32 bit search (sync or preamble)

    def calc_shift_trecs_pre_bits_to_syncword(self, model):
        #This method calculates how many bits to shift from premable to syncword when using TRECS
        #The goal is to strengthen correlation

        demod_select = model.vars.demod_select.value
        pream_len = model.vars.preamble_length.value
        syncword_len = model.vars.syncword_length.value
        delay_samp = model.vars.grpdelay_to_demod.value
        osr = model.vars.oversampling_rate_actual.value
        rtschmode_actual = model.vars.MODEM_REALTIMCFE_RTSCHMODE.value

        if demod_select == model.vars.demod_select.var_enum.TRECS_VITERBI or \
            demod_select == model.vars.demod_select.var_enum.TRECS_SLICER:

            target_cfe_len = 32 #The goal is to end up with a 32 bit window, length must end up as a mult of 4
            delay_symbols = int(ceil(delay_samp / osr)) # Calculate the group delay in symbols
            min_pre_len = 26 + delay_symbols #Need to leave at least 26 preamble bits after delay so that we can enable preamsch and AFC
            min_pre_no_preamsch_len = 8 #Need to leave 8 bits of preamble if preamsch is disabled

            if (syncword_len < target_cfe_len) and (rtschmode_actual == 0):
                #We want to shift some preamble bits to the syncword
                #Skip this if using RTSCHMODE=1 (hard slicing)
                cfe_len_shortage = target_cfe_len - syncword_len

                # Check if we have a long enough preamble to accommodate moving bits to the syncword while leaving
                # at least the minimum number of preamble bits
                if  pream_len >= cfe_len_shortage + min_pre_len:
                    trecs_pre_bits_to_syncword = cfe_len_shortage #Resulting CFE len is equal to target

                elif pream_len >= min_pre_len:
                    # If the preamble is not long enough, then only move enough bits to leave the min preamble bits
                    #If the preamble is exactly the minimum length then this won't move any bits
                    trecs_pre_bits_to_syncword = pream_len - min_pre_len

                elif pream_len > min_pre_no_preamsch_len:
                    #If we are going to turn off preamble search and AFC anyway, then just leave a minimal number of bits
                    #But make sure to not move more bits than the CFE length shortage
                    trecs_pre_bits_to_syncword = min(pream_len - min_pre_no_preamsch_len,cfe_len_shortage)

                else:
                    # The preamble is too short to be able to move any bits
                    trecs_pre_bits_to_syncword = 0

            else:
                #The syncword is already long enough to meet our target CFE length, or we are using RTSCHMODE=1
                trecs_pre_bits_to_syncword = 0

        else:
            #Not using TRECS
            trecs_pre_bits_to_syncword = 0

        # The resulting CFE length needs to be a multiple of 4, so if it's not then move less preamble bits
        if trecs_pre_bits_to_syncword > 0:

            cfe_len_expected = syncword_len + trecs_pre_bits_to_syncword
            correction_factor = cfe_len_expected % 4
            trecs_pre_bits_to_syncword = trecs_pre_bits_to_syncword - correction_factor

        model.vars.trecs_pre_bits_to_syncword.value = trecs_pre_bits_to_syncword
        model.vars.trecs_effective_preamble_len.value = pream_len - trecs_pre_bits_to_syncword
        model.vars.trecs_effective_syncword_len.value = syncword_len + trecs_pre_bits_to_syncword

    def calc_demod_expect_patt_value(self, model):

        demod_select = model.vars.demod_select.value
        syncword0 = model.vars.MODEM_SYNC0_SYNC0.value
        mapfsk = model.vars.MODEM_CTRL0_MAPFSK.value
        trecs_pre_bits_to_syncword = model.vars.trecs_pre_bits_to_syncword.value
        preamble_string = model.vars.preamble_string.value
        syncword_len = model.vars.syncword_length.value

        if demod_select == model.vars.demod_select.var_enum.TRECS_VITERBI or \
                demod_select == model.vars.demod_select.var_enum.TRECS_SLICER:

            syncword_str_part = '{:032b}'.format(syncword0)[-syncword_len:] #Read the rightmost characters

            #Need to check for zero because python treats -0 the same as 0 in terms of list slicing
            if trecs_pre_bits_to_syncword > 0:
                preamble_str_part = preamble_string[-trecs_pre_bits_to_syncword:]  # Read the rightmost characters
            else:
                preamble_str_part = ""

            effective_syncword_str = preamble_str_part+syncword_str_part[::-1]+'0'*32 #reverse syncword part only

            #HW will add head and tail for correlation computation
            viterbi_demod_expect_patt = int(effective_syncword_str[0:32],2)
            patt = viterbi_demod_expect_patt

            # if MAPFSK is 1 mapping is inverted so invert the expected pattern to match
            if mapfsk:
                patt ^= 0xFFFFFFFF
        else:
            # set to default reset value
            patt = long(0x123556B7)

        model.vars.viterbi_demod_expect_patt.value = patt

    def calc_demod_expect_patt_reg(self, model):

        viterbi_demod_expect_patt = model.vars.viterbi_demod_expect_patt.value

        self._reg_write(model.vars.MODEM_VTCORRCFG0_EXPECTPATT, viterbi_demod_expect_patt)

    def calc_demod_expect_patt_head_tail_value(self, model):

        demod_select = model.vars.demod_select.value
        preamble_pattern = model.vars.preamble_pattern.value
        mapfsk = model.vars.MODEM_CTRL0_MAPFSK.value

        if demod_select == model.vars.demod_select.var_enum.TRECS_VITERBI or \
            demod_select == model.vars.demod_select.var_enum.TRECS_SLICER:

            # Head (end of preamble) and tail bits after syncword (0)
            viterbi_demod_expect_patt_head_tail = int(preamble_pattern << 2)

            # if MAPFSK is 1 mapping is inverted so invert the expected pattern to match
            if mapfsk:
                viterbi_demod_expect_patt_head_tail ^= 0xF

            model.vars.viterbi_demod_expect_patt_head_tail.value = viterbi_demod_expect_patt_head_tail
        else:
            # set to default reset value
            model.vars.viterbi_demod_expect_patt_head_tail.value = 5

    def calc_demod_expect_patt_head_tail_reg(self, model):

        viterbi_demod_expect_patt_head_tail = model.vars.viterbi_demod_expect_patt_head_tail.value
        self._reg_write(model.vars.MODEM_VTCORRCFG1_EXPECTHT, viterbi_demod_expect_patt_head_tail)

    def calc_expsynclen_reg(self, model):
        """ set register EXPSYNCLEN that determines RAM pointer rollback from the last write
        after TRECS recovers timing from sync word. This needs to include 2 preamble bits. It may need
        additional bits depending on delay between last write and timing acquisition
        This worked for FSK2 for wmbus.
        """
        vtdemoden = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value
        syncacqwinsize = model.vars.syncacqwin_actual.value
        osr = model.vars.MODEM_TRECSCFG_TRECSOSR.value

        if vtdemoden:
            expsynclen = osr * (syncacqwinsize + 4) + 2
            if expsynclen > 511:
                expsynclen = 511
        else:
            expsynclen = 0

        self._reg_write(model.vars.MODEM_VTCORRCFG1_EXPSYNCLEN, int(expsynclen))

    def calc_syncacqwin_reg(self, model):

        vtdemoden = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value
        trecs_effective_syncword_len = model.vars.trecs_effective_syncword_len.value

        if vtdemoden:
            syncacqwin = trecs_effective_syncword_len // 4 - 1
        else:
            syncacqwin = 0

        self._reg_write(model.vars.MODEM_REALTIMCFE_SYNCACQWIN, syncacqwin)

    def calc_preamsch_len(self, model):

        preamble_len = model.vars.trecs_effective_preamble_len.value
        delay_samp = model.vars.grpdelay_to_demod.value
        osr = model.vars.oversampling_rate_actual.value
        vtdemoden = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value

        if vtdemoden:
            #Calculate how much shorter the timing window should be than the preamble
            preamble_len_after_delay = preamble_len - int(ceil(delay_samp / osr))

            # can only use 32 preamble bits
            if preamble_len_after_delay > 32:
                preamsch_len = 32

            # if preamble length is less than or equal to 24 don't use preamble search in TRECS
            elif preamble_len_after_delay <= 24:
                preamsch_len = 0

            else:
                preamsch_len = preamble_len_after_delay

        else:
            #Not using TRECS
            preamsch_len = 0

        model.vars.preamsch_len.value = preamsch_len

    def calc_preamsch_reg(self, model):

        #The preamsch len is the effective preamble length minus delay bits
        preamsch_len = model.vars.preamsch_len.value

        #If the preamble search length is set to 0 just do syncword detection
        if preamsch_len == 0:
            reg = 0
        else:
            reg = 1

        self._reg_write(model.vars.MODEM_TRECPMDET_PREAMSCH, reg)

    def calc_pmacqwin_reg(self, model):

        vtdemoden = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value
        preamsch_len = model.vars.preamsch_len.value

        if preamsch_len >= 4 and vtdemoden:
            if model.vars.MODEM_CTRL3_ANTDIVMODE.value == 5 :
                pmacqwin = 2
            else:
                pmacqwin = preamsch_len // 4 - 1
        else:
            pmacqwin = 0

        self._reg_write(model.vars.MODEM_TRECPMDET_PMACQUINGWIN, pmacqwin)

    def calc_pmacquingwin_actual(self, model):

        pmacquingwin = model.vars.MODEM_TRECPMDET_PMACQUINGWIN.value

        model.vars.pmacquingwin_actual.value = 4 * (pmacquingwin + 1)

    def calc_pmdetthd_reg(self, model):

        # we disable this feature for now. keep non-zero value
        self._reg_write(model.vars.MODEM_PHDMODCTRL_PMDETTHD, 8)

    def calc_pmoffset_reg(self, model):

        osr = model.vars.MODEM_TRECSCFG_TRECSOSR.value
        pmwinsize = model.vars.pmacquingwin_actual.value
        rtschmode = model.vars.MODEM_REALTIMCFE_RTSCHMODE.value
        baudrate =  model.vars.baudrate.value

        if (rtschmode == 1) and (baudrate > 90000):
            #Special case for dual syncword detection case where hard slicing on syncword is required
            #because frequency tolerance is more difficult when RTSCHMODE is 1
            pmoffset = osr*2 + 2
        else:
            # + 2 for processing delay. See expsynclen register description. These are used in the same way.
            pmoffset =  osr * pmwinsize + 2

        self._reg_write(model.vars.MODEM_TRECSCFG_PMOFFSET,  pmoffset)

    def calc_pmcostthd_reg(self, model):

        vtdemoden = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value
        preamsch_len = model.vars.preamsch_len.value
        pmacquingwin = model.vars.MODEM_TRECPMDET_PMACQUINGWIN.value
        freq_dev_min = model.vars.freq_dev_min.value
        freq_dev_max = model.vars.freq_dev_max.value
        deviation = model.vars.deviation.value

        #Calculate the cost threshold based on preamble detect window and deviation tolerance requirement
        if preamsch_len > 0 and vtdemoden:
            if ((freq_dev_min < 0.8*deviation) or (freq_dev_max > 1.2*deviation)):
                #High tolerance case
                reg = 750 - (7 - pmacquingwin) * 60
            else:
                reg = 500 - (7 - pmacquingwin) * 60
        else:
            reg = 0

        self._reg_write(model.vars.MODEM_TRECPMDET_PMMINCOSTTHD, reg)

    def calc_mincostthd_reg(self, model):

        vtdemoden = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value
        syncacqwin_reg = model.vars.MODEM_REALTIMCFE_SYNCACQWIN.value

        if vtdemoden:
            reg = self.MIN_COST_THD_FULL - (7 - syncacqwin_reg) * 60
        else:
            reg = 0

        self._reg_write(model.vars.MODEM_REALTIMCFE_MINCOSTTHD, reg)

    def calc_rtcfeen_reg(self, model):

        vtdemoden = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value

        reg = 1 if vtdemoden else 0

        self._reg_write(model.vars.MODEM_REALTIMCFE_RTCFEEN, reg)

    def calc_harddecision_reg(self, model):

        vtdemoden = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value
        mi = model.vars.modulation_index.value

        reg = 1 if vtdemoden and mi >= 1.0 else 0

        self._reg_write(model.vars.MODEM_VITERBIDEMOD_HARDDECISION, reg)

    def calc_vtdemoden_reg(self, model):
        demod_sel = model.vars.demod_select.value

        # enable viterbi demod when it is selected
        if demod_sel == model.vars.demod_select.var_enum.TRECS_VITERBI or demod_sel == model.vars.demod_select.var_enum.TRECS_SLICER:
            reg = 1
        else:
            reg = 0

        self._reg_write(model.vars.MODEM_VITERBIDEMOD_VTDEMODEN, reg)

    def calc_viterbi_misc_reg(self, model):

        part_family = model.part_family.lower()

        vtdemoden = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value

        self._reg_write(model.vars.MODEM_REALTIMCFE_SINEWEN, 0)
        self._reg_write(model.vars.MODEM_TRECPMDET_COSTHYST, 0)
        self._reg_write(model.vars.MODEM_VITERBIDEMOD_CORRCYCLE, 0)
        self._reg_write(model.vars.MODEM_VTBLETIMING_FLENOFF, 0)
        self._reg_write(model.vars.MODEM_VTTRACK_FREQBIAS, 0)
        self._reg_write(model.vars.MODEM_VTTRACK_TIMGEAR, 0)

        self._reg_write(model.vars.MODEM_PHDMODCTRL_BCRTRECSCONC, 0)
        self._reg_write(model.vars.MODEM_PHDMODCTRL_BCRLEGACYCONC, 0)
        self._reg_write(model.vars.MODEM_PHDMODCTRL_BCRDETECTOR, 0)
        #self._reg_write(model.vars.MODEM_VTCORRCFG1_VITERBIKSI3WB, 0)

        self._reg_write(model.vars.MODEM_REALTIMCFE_EXTENSCHBYP, 1) # Enable the extended search bypass as workaround for PGOCELOT-5342

        if vtdemoden:
            # these register fields have a fixed value for now
            self._reg_write(model.vars.MODEM_VITERBIDEMOD_CORRSTPSIZE, 4)
            self._reg_write(model.vars.MODEM_VTBLETIMING_DISDEMODOF, 1)
            self._reg_write(model.vars.MODEM_VTBLETIMING_TIMINGDELAY, 60)
            self._reg_write(model.vars.MODEM_VTBLETIMING_VTBLETIMINGSEL, 1)
            self._reg_write(model.vars.MODEM_VTTRACK_FREQTRACKMODE, 1)
            self._reg_write(model.vars.MODEM_VTTRACK_HIPWRTHD, 1)
            self._reg_write(model.vars.MODEM_VTTRACK_TIMTRACKTHD, 2)
            self._reg_write_default(model.vars.MODEM_VTTRACK_TIMEACQUTHD, part_family)
        else:
            self._reg_write(model.vars.MODEM_VITERBIDEMOD_CORRSTPSIZE, 0)
            self._reg_write(model.vars.MODEM_VTBLETIMING_DISDEMODOF, 0)
            self._reg_write(model.vars.MODEM_VTBLETIMING_TIMINGDELAY, 0)
            self._reg_write(model.vars.MODEM_VTBLETIMING_VTBLETIMINGSEL, 0)
            self._reg_write(model.vars.MODEM_VTTRACK_FREQTRACKMODE, 0)
            self._reg_write(model.vars.MODEM_VTTRACK_HIPWRTHD, 0)
            self._reg_write(model.vars.MODEM_VTTRACK_TIMTRACKTHD, 0)
            self._reg_do_not_care(model.vars.MODEM_VTTRACK_TIMEACQUTHD)

    def calc_realtimcfe_vtafcframe_reg(self, model):
        #This function calculates the REALTIMCFE_VTAFCFRAME reg field

        #Read in model vars
        vtdemoden = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value

        #Calculate the reg
        if vtdemoden:
            #Constantly update digmix based on freq offset estimates each trackingwin
            vtafcframe = 1
        else:
            vtafcframe = 0

        #Write the register
        self._reg_write(model.vars.MODEM_REALTIMCFE_VTAFCFRAME, vtafcframe)

    def calc_realtimcfe_trackingwin(self, model):
        #This function calculates the REALTIMCFE_TRACKINGWIN reg field

        #Read in model variables
        baudrate_tol_ppm = model.vars.baudrate_tol_ppm.value
        vtdemoden = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value

        if vtdemoden:
            #If the tol request is at least 5000ppm, then reduce the tracking win size
            if baudrate_tol_ppm >= 5000:
                trackingwin = 5
            else:
                trackingwin = 7
        else:
            trackingwin = 0

        #Write the reg
        self._reg_write(model.vars.MODEM_REALTIMCFE_TRACKINGWIN, trackingwin)

    # calculate based on Wentao's feedback
    def calc_rtschwin_reg(self, model):

        preamsch_len = model.vars.preamsch_len.value

        if preamsch_len <= 24:
            reg = 4
        else:
            reg = 5

        self._reg_write(model.vars.MODEM_REALTIMCFE_RTSCHWIN, reg)

    # calculate based on Wentao's feedback
    def calc_pmcostvalthd_reg(self, model):

        preamsch_len = model.vars.preamsch_len.value

        if preamsch_len <= 16:
            reg = 2
        elif preamsch_len <= 24:
            reg = 3
        else:
            reg = 4

        self._reg_write(model.vars.MODEM_TRECPMDET_PMCOSTVALTHD, reg)


    def calc_pmtimeoutsel_reg(self, model):

        preamsch_len = model.vars.preamsch_len.value
        preamble_len = model.vars.preamble_length.value
        sync_len = model.vars.syncbits_actual.value

        # first approximation time out should be total length of preamble and sync word
        # we add a margin of half the sync_len for extreme test cases
        total_len = int(preamble_len + sync_len * 1.5)

        # set closest setting possible based on total_len
        if total_len <= 16 or total_len == 0:
            reg = 0
        elif total_len <= 24:
            reg = 1
        elif total_len <= 32:
            reg = 2
        else:
            reg = 3

        if total_len > 65535:
            total_len = 65535

        self._reg_write(model.vars.MODEM_TRECPMDET_PMTIMEOUTSEL, reg)
        self._reg_write(model.vars.MODEM_FRMSCHTIME_FRMSCHTIME, total_len)

    # def calc_vtpmdetsel_reg(self, model):
    #     preamsch_len = model.vars.preamsch_len.value
    #
    #     # set to closes value to the preamble search window
    #     if preamsch_len > 0:
    #         reg = (preamsch_len - 8) // 4
    #         if reg > 3:
    #             reg = 3
    #     else:
    #         reg = 0
    #
    #     self._reg_write(model.vars.MODEM_TRECPMDET_VTPMDETSEL, reg)


    def calc_pmexpectpatt_reg(self, model):

        preamsch_len = model.vars.preamsch_len.value
        pre_str = model.vars.preamble_string.value
        pmwinsize = model.vars.pmacquingwin_actual.value
        mapfsk = model.vars.MODEM_CTRL0_MAPFSK.value
        demod_sel = model.vars.demod_select.value
        trecs_effective_preamble_len = model.vars.trecs_effective_preamble_len.value

        # Only calculate pmexpectpatt for TRECS or BCR (BCR reuses this reg)
        if demod_sel == model.vars.demod_select.var_enum.TRECS_VITERBI or demod_sel == model.vars.demod_select.var_enum.TRECS_SLICER or demod_sel == model.vars.demod_select.var_enum.BCR:

            effective_pre_str = pre_str[:trecs_effective_preamble_len] #This is the preamble once some bits are shifted to the syncword
            zero_filler_str = '0'*32 #Add 32 zeroes to the end to make sure we have a long enough string
            combined_str = effective_pre_str + zero_filler_str

            # if PM search is enabled set pattern to preamble string
            # then convert binary string to integer to write into register field
            reg = int(combined_str[0:32],2)
            # if MAPFSK is 1 mapping is inverted so invert the expected pattern to match
            if mapfsk:
                reg ^= 0xFFFFFFFF
        else:
            reg = 0

        self._reg_write(model.vars.MODEM_TRECPMPATT_PMEXPECTPATT, reg)

    def calc_synthafc_reg(self, model):
        afc1shot_en = model.vars.MODEM_AFC_AFCONESHOT.value

        # enable SYNTHAFC when AFC 1-shot is enabled
        reg = 1 if afc1shot_en else 0

        self._reg_write(model.vars.MODEM_VITERBIDEMOD_SYNTHAFC, reg)

    def calc_vtfreqlim_reg(self, model):
        vtdemoden = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value
        freq_offset_hz = model.vars.freq_offset_hz.value
        rf_freq_hz = model.vars.base_frequency_hz.value * 1.0
        baudrate = model.vars.baudrate.value

        #Frequency offset is never exactly zero. To be conservative we act like xtal tol is at least 1ppm for rx/tx.
        freq_offset_hz_min = (2) * rf_freq_hz / 1e6
        freq_offset_hz = max(freq_offset_hz_min, freq_offset_hz)

        # limit is actually offset/baudrate but we don't want this to have an effect on frequency offset
        # performance so we multiply result by 2 to have margin
        val = (20+(1.2*2 * 256 * freq_offset_hz / baudrate) )if vtdemoden else 0

        # make sure we can fit result into register
        reg = int(511 if val > 511 else val)

        self._reg_write(model.vars.MODEM_VTCORRCFG1_VTFRQLIM, reg)

    # def calc_synctimeoutsel_reg(self, model):
    #     vtdemoden = model.vars.MODEM_VITERBIDEMOD_VTDEMODEN.value
    #     synclen = model.vars.syncbits_actual.value
    #
    #     # In BLE we have 32 bit sync word and set this to 48 so about 1.5 x synclen
    #     # synclen up to 21 results in 32 or less synclen more than that we'll set the timeout to 48
    #     # since max synclen is 32 no need to set anything else
    #     reg = 1 if synclen > 21 and vtdemoden else 0
    #
    #     self._reg_write(model.vars.MODEM_VTTRACK_SYNCTIMEOUTSEL, reg)


    def calc_swcoeffen_reg(self, model):

        afc1shot_en = model.vars.MODEM_AFC_AFCONESHOT.value

        reg = 1 if afc1shot_en else 0

        self._reg_write(model.vars.MODEM_CHFCTRL_SWCOEFFEN, reg)
        self._reg_write(model.vars.MODEM_VTCORRCFG1_KSI3SWENABLE, reg)

    def calc_pmdeten_reg(self, model):

        # always disable for now
        self._reg_write(model.vars.MODEM_PHDMODCTRL_PMDETEN, 0)

    def calc_pmtimlosen_reg(self, model):

        # disable these for now
        self._reg_write(model.vars.MODEM_PHDMODCTRL_PMTIMLOSEN, 0)
        self._reg_write(model.vars.MODEM_PHDMODCTRL_PMTIMLOSTHD, 0)

    def calc_remodoutsel_reg(self, model):

        # always set to 1 for now. This feeds datafilter output into TRECS if remoden is 1.
        # note that phscale is still in the path even though the datafilter output is a frequency signal
        # if remodoutsel == 0 and (remoden == 1 and remoddwn == 0) -> input is datafilter output before TRECS DSA and DEC2 output after
        # if remodoutsel == 1 and (remoden == 1 and remoddwn == 0) -> input is taken from datafilter output
        # if remodoutsel == 2 and (remoden == 1 and remoddwn == 0) -> input is taken from DEC2 output
        # if remodoutsel == 3 -> input is from remodulated sliced signal before TRECS DSA and DEC2 output after
        # if remodoutsel < 3 and (remoden == 0 or remoddwn != 0) -> input is taken from remodulated sliced signal
        self._reg_write(model.vars.MODEM_PHDMODCTRL_REMODOUTSEL, 1)

    def calc_realtimcfe_rtschmode_reg(self, model):
        #This function calculates the RTSCHMODE register field for TRECS

        #Read in model variables
        dualsync = model.vars.syncword_dualsync.value
        demod_select = model.vars.demod_select.value

        #Calculate the register value based on whether we are using TRECS and whether dual syncword detect is enabled
        if demod_select == model.vars.demod_select.var_enum.TRECS_VITERBI or \
                demod_select == model.vars.demod_select.var_enum.TRECS_SLICER:

            #If dual syncword detection is enabled, then stop using CFE and hard slice syncword w TRECS
            if dualsync:
                rtschmode = 1
            else:
                rtschmode = 0
        else:
            rtschmode = 0

        #Write the register
        self._reg_write(model.vars.MODEM_REALTIMCFE_RTSCHMODE, rtschmode)
