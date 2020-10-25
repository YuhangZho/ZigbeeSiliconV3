from pyradioconfig.parts.lynx.calculators.calc_misc import CALC_Misc_lynx
from pycalcmodel.core.variable import ModelVariableFormat

class CALC_Misc_ocelot(CALC_Misc_lynx):

    # def calc_misc(self, model):
    #     """
    #     These aren't really calculating right now.  Just using defaults or forced values.
    #
    #     Args:
    #         model (ModelRoot) : Data model to read and write variables from
    #     """
    def calc_misc_Panther(self, model):
        pass

    def calc_misc_Lynx(self, model):
        pass

    def calc_misc_Ocelot(self, model):
        self._reg_write(model.vars.MODEM_CF_ADCBITORDERI, 0)
        self._reg_write(model.vars.MODEM_CF_ADCBITORDERQ, 0)

        # Digital Gain Control
        self._reg_write(model.vars.MODEM_DIGIGAINCTRL_DEC0GAIN, 0)
        self._reg_write(model.vars.MODEM_DIGIGAINCTRL_DIGIGAINDOUBLE, 0)
        self._reg_write(model.vars.MODEM_DIGIGAINCTRL_DIGIGAINEN, 0)
        self._reg_write(model.vars.MODEM_DIGIGAINCTRL_DIGIGAINHALF, 0)
        self._reg_write(model.vars.MODEM_DIGIGAINCTRL_DIGIGAINSEL, 0)

        self._reg_write(model.vars.MODEM_CHFCTRL_FWSELCOEFF, 0)
        self._reg_write(model.vars.MODEM_CHFCTRL_FWSWCOEFFEN, 0)
        ## New registers
        # FIXME: how to calculate these?

        self._reg_write(model.vars.MODEM_CHFLATENCYCTRL_CHFLATENCY, 0)
        self._reg_write(model.vars.MODEM_DIGMIXCTRL_DIGMIXMODE, 1)
        self._reg_write(model.vars.MODEM_DIGMIXCTRL_DIGMIXFB, 1)
        self._reg_write(model.vars.MODEM_DCCOMPFILTINIT_DCCOMPINIT, 0)
        self._reg_write(model.vars.MODEM_DCCOMPFILTINIT_DCCOMPINITVALI, 0)
        self._reg_write(model.vars.MODEM_DCCOMPFILTINIT_DCCOMPINITVALQ, 0)


    def buildVariables(self, model):
        # : Build variables using base
        super().buildVariables(model)

        """ Coherent Demod Calculations """
        # : Noise floor of channel power accumulator used for coherent demod.
        self._addModelVariable(model, 'chpwraccu_noise', float, ModelVariableFormat.DECIMAL)

    def calc_misc(self, model):
        """
        These aren't really calculating right now.  Just using defaults or forced values.

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        part_family = model.part_family.lower()

        # Legacy Demod Registers
        # FIXME: calculate these

        self._reg_write(model.vars.MODEM_CTRL2_BRDIVA, 0)
        self._reg_write(model.vars.MODEM_CTRL2_BRDIVB, 0)
        self._reg_write(model.vars.MODEM_CTRL2_DEVMULA, 0)
        self._reg_write(model.vars.MODEM_CTRL2_DEVMULB, 0)
        self._reg_write(model.vars.MODEM_CTRL2_RATESELMODE, 0)
        self._reg_write(model.vars.MODEM_CTRL2_RXFRCDIS, 0)
        self._reg_write(model.vars.MODEM_CTRL2_SQITHRESH, 0)
        self._reg_write(model.vars.MODEM_CTRL2_TXPINMODE, 0)
        self._reg_write(model.vars.MODEM_CTRL4_ADCSATDENS, 0)
        self._reg_write(model.vars.MODEM_CTRL4_ADCSATLEVEL, 6)
        self._reg_write(model.vars.MODEM_CTRL4_OFFSETPHASESCALING, 0)
        self._reg_write(model.vars.MODEM_CTRL4_PHASECLICKFILT, 0)
        self._reg_write(model.vars.MODEM_CTRL4_PREDISTAVG, 0)
        self._reg_write(model.vars.MODEM_CTRL4_PREDISTDEB, 0)
        self._reg_write(model.vars.MODEM_CTRL4_PREDISTGAIN, 0)
        self._reg_write(model.vars.MODEM_CTRL4_PREDISTRST, 0)
        self._reg_write(model.vars.MODEM_CTRL4_SOFTDSSSMODE, 0)
        self._reg_write(model.vars.MODEM_CTRL5_BBSS, 0)
        self._reg_write(model.vars.MODEM_CTRL5_DEMODRAWDATASEL2, 0)
        self._reg_write(model.vars.MODEM_CTRL5_DETDEL, 0)
        self._reg_write(model.vars.MODEM_CTRL5_POEPER, 0)
        self._reg_write(model.vars.MODEM_CTRL5_RESYNCLIMIT, 0)
        self._reg_write(model.vars.MODEM_CTRL6_CODINGB, 0)
        self._reg_write(model.vars.MODEM_CTRL6_CPLXCORREN, 0)
        self._reg_write(model.vars.MODEM_CTRL6_DEMODRESTARTALL, 0)
        self._reg_write(model.vars.MODEM_CTRL6_DSSS3SYMBOLSYNCEN, 0)
        self._reg_write(model.vars.MODEM_CTRL6_PREBASES, 0)
        self._reg_write(model.vars.MODEM_CTRL6_RXRESTARTUPONRSSI, 0)
        self._reg_write(model.vars.MODEM_CTRL6_RXRESTARTUPONSHORTRSSI, 0)
        self._reg_write(model.vars.MODEM_CTRL6_TXDBPSKINV, 0)
        self._reg_write(model.vars.MODEM_CTRL6_TXDBPSKRAMPEN, 0)
        self._reg_write(model.vars.MODEM_ANARAMPCTRL_VMIDCTRL, 1)
        self._reg_write(model.vars.MODEM_ANARAMPCTRL_MUTEDLY, 0)
        self._reg_write(model.vars.MODEM_ETSCTRL_CAPTRIG, 0)
        self._reg_write(model.vars.MODEM_ETSCTRL_ETSLOC, 0)
        self._reg_write(model.vars.MODEM_ETSTIM_ETSCOUNTEREN, 0)
        self._reg_write(model.vars.MODEM_ETSTIM_ETSTIMVAL, 0)

        self._reg_write(model.vars.MODEM_OOKSHAPING_OOKSHAPINGEN, 0)
        self._reg_write(model.vars.MODEM_OOKSHAPING_OOKSHAPINGLUTSIZE, 0)
        self._reg_write(model.vars.MODEM_OOKSHAPING_OOKSHAPINGSTEP, 0)
        self._reg_write(model.vars.MODEM_PRE_DSSSPRE, 0)
        self._reg_write(model.vars.MODEM_PRE_PRESYMB4FSK, 0)
        self._reg_write(model.vars.MODEM_PRE_SYNCSYMB4FSK, 0)
        self._reg_write(model.vars.MODEM_TIMING_FASTRESYNC, 0)
        self._reg_write(model.vars.MODEM_TIMING_TIMSEQINVEN, 0)
        self._reg_write(model.vars.MODEM_TIMING_TIMSEQSYNC, 0)
        self._reg_write(model.vars.MODEM_TIMING_TSAGCDEL, 0)

        # Long Range registers
        # FIXME: calculate these
        self._reg_write(model.vars.MODEM_LONGRANGE1_LOGICBASEDLRDEMODGATE, 0)
        self._reg_write(model.vars.MODEM_LONGRANGE1_LOGICBASEDPUGATE, 0)
        self._reg_write(model.vars.MODEM_LONGRANGE1_LRSPIKETHADD, 0)
        self._reg_write(model.vars.MODEM_LONGRANGE1_LRSS, 0)
        self._reg_write(model.vars.MODEM_LRFRC_CI500, 1)
        self._reg_write(model.vars.MODEM_LRFRC_FRCACKTIMETHD, 0)
        self._reg_write(model.vars.MODEM_LRFRC_LRCORRMODE, 1)

        # DSA registers
        # FIXME: what do we need to calculate here?
        self._reg_write(model.vars.MODEM_DSACTRL_AGCBAUDEN, 0)
        self._reg_write(model.vars.MODEM_DSACTRL_AMPJUPTHD, 0)
        self._reg_write(model.vars.MODEM_DSACTRL_ARRTOLERTHD0, 2)
        self._reg_write(model.vars.MODEM_DSACTRL_ARRTOLERTHD1, 4)
        self._reg_write(model.vars.MODEM_DSACTRL_DSARSTON, 1)
        self._reg_write(model.vars.MODEM_DSACTRL_FREQAVGSYM, 1)
        self._reg_write(model.vars.MODEM_DSACTRL_GAINREDUCDLY, 0)
        self._reg_write(model.vars.MODEM_DSACTRL_LOWDUTY, 0)
        self._reg_write(model.vars.MODEM_DSACTRL_RESTORE, 0)
        self._reg_write(model.vars.MODEM_DSACTRL_SCHPRD, 0)
        self._reg_write(model.vars.MODEM_DSACTRL_TRANRSTDSA, 0)
        self._reg_write(model.vars.MODEM_DSATHD0_FDEVMAXTHD, 0x78)
        self._reg_write(model.vars.MODEM_DSATHD0_FDEVMINTHD, 12)
        self._reg_write(model.vars.MODEM_DSATHD0_SPIKETHD, 0x64)
        self._reg_write(model.vars.MODEM_DSATHD0_UNMODTHD, 4)
        self._reg_write(model.vars.MODEM_DSATHD1_AMPFLTBYP, 1)
        self._reg_write(model.vars.MODEM_DSATHD1_DSARSTCNT, 2)
        self._reg_write(model.vars.MODEM_DSATHD1_FREQLATDLY, 1)
        self._reg_write(model.vars.MODEM_DSATHD1_FREQSCALE, 0)
        self._reg_write(model.vars.MODEM_DSATHD1_POWABSTHD, 0x1388)
        self._reg_write(model.vars.MODEM_DSATHD1_POWRELTHD, 0)
        self._reg_write(model.vars.MODEM_DSATHD1_PWRDETDIS, 1)
        self._reg_write(model.vars.MODEM_DSATHD1_PWRFLTBYP, 1)
        self._reg_write(model.vars.MODEM_DSATHD1_RSSIJMPTHD, 6)
        self._reg_write(model.vars.MODEM_DSATHD2_FDADJTHD, 1)
        self._reg_write(model.vars.MODEM_DSATHD2_FREQESTTHD, 6)
        self._reg_write(model.vars.MODEM_DSATHD2_INTERFERDET, 6)
        self._reg_write(model.vars.MODEM_DSATHD2_JUMPDETEN, 1)
        self._reg_write(model.vars.MODEM_DSATHD2_PMDETFORCE, 0)
        self._reg_write(model.vars.MODEM_DSATHD2_PMDETPASSTHD, 6)
        self._reg_write(model.vars.MODEM_DSATHD2_POWABSTHDLOG, 100)
        self._reg_write(model.vars.MODEM_DSATHD3_FDEVMAXTHDLO, 120)
        self._reg_write(model.vars.MODEM_DSATHD3_FDEVMINTHDLO, 12)
        self._reg_write(model.vars.MODEM_DSATHD3_SPIKETHDLO, 100)
        self._reg_write(model.vars.MODEM_DSATHD3_UNMODTHDLO, 4)
        self._reg_write(model.vars.MODEM_DSATHD4_ARRTOLERTHD0LO, 2)
        self._reg_write(model.vars.MODEM_DSATHD4_ARRTOLERTHD1LO, 4)
        self._reg_write(model.vars.MODEM_DSATHD4_POWABSTHDLO, 5000)
        self._reg_write(model.vars.MODEM_DSATHD4_SWTHD, 0)

        # FIXME: check with Subrata on how to calculate these
        self._reg_write(model.vars.MODEM_PHDMODANTDIV_ANTWAIT, 20)
        self._reg_write(model.vars.MODEM_PHDMODANTDIV_SKIP2ANT, 1)
        self._reg_write(model.vars.MODEM_PHDMODANTDIV_SKIPCORRTHD, 100)
        self._reg_write(model.vars.MODEM_PHDMODANTDIV_SKIPRSSITHD, 0)
        self._reg_write(model.vars.MODEM_PHANTDECSION_RSSICORR0,1)
        self._reg_write(model.vars.MODEM_PHANTDECSION_RSSICORR1, 1)
        self._reg_write(model.vars.MODEM_PHANTDECSION_RSSICORR2, 1)
        self._reg_write(model.vars.MODEM_PHANTDECSION_RSSICORR3, 1)
        self._reg_write(model.vars.MODEM_PHANTDECSION_RSSIANDDIVTHD, 20)
        self._reg_write(model.vars.MODEM_PHANTDECSION_CORRANDDIVTHD, 100)

        # FIXME: figure out how these AGC registers need to be calculated
        self._reg_write(model.vars.AGC_RSSISTEPTHR_DEMODRESTARTPER, 0)
        self._reg_write(model.vars.AGC_RSSISTEPTHR_DEMODRESTARTTHR, 0)
        self._reg_write(model.vars.AGC_RSSISTEPTHR_NEGSTEPTHR, 0)
        self._reg_write(model.vars.AGC_RSSISTEPTHR_POSSTEPTHR, 0)
        self._reg_write(model.vars.AGC_RSSISTEPTHR_STEPPER, 0)

        # Antenna Diversity Registers
        # FIXME: check with Amey if we need to calculate these
        self._reg_write(model.vars.MODEM_ANTDIVCTRL_ADPRETHRESH, 0)
        self._reg_write(model.vars.MODEM_ANTDIVCTRL_ENADPRETHRESH, 0)
        self._reg_write(model.vars.MODEM_ANTSWCTRL1_TIMEPERIOD, 436906)
        self._reg_write(model.vars.MODEM_ANTSWCTRL_ANTCOUNT, 0)
        self._reg_write(model.vars.MODEM_ANTSWCTRL_ANTDFLTSEL, 0)
        self._reg_write(model.vars.MODEM_ANTSWCTRL_ANTSWENABLE, 0)
        self._reg_write(model.vars.MODEM_ANTSWCTRL_ANTSWTYPE, 0)
        self._reg_write(model.vars.MODEM_ANTSWCTRL_CFGANTPATTEN, 0)
        self._reg_write(model.vars.MODEM_ANTSWCTRL_EXTDSTOPPULSECNT, 30)
        self._reg_write(model.vars.MODEM_ANTSWEND_ANTSWENDTIM, 0)
        self._reg_write(model.vars.MODEM_ANTSWSTART_ANTSWSTARTTIM, 0)
        self._reg_write(model.vars.MODEM_CFGANTPATT_CFGANTPATTVAL, 0)
        #self._reg_write(model.vars.MODEM_CTRL3_ANTDIVMODE, 0)
        self._reg_write(model.vars.MODEM_CTRL3_ANTDIVREPEATDIS, 0)

        # Coherent Demod Registers
        #FIXME: Check with Yan/Per on how to calculate these
        self._reg_write(model.vars.MODEM_COH2_DSAPEAKCHPWRTH, 0)
        self._reg_write(model.vars.MODEM_COH3_COHDSADETDIS, 0)
        self._reg_write(model.vars.MODEM_COH3_DSAPEAKCHPWREN, 0)
        self._reg_write(model.vars.MODEM_COH3_LOGICBASEDCOHDEMODGATE, 0)
        self._reg_write(model.vars.MODEM_COH3_ONEPEAKQUALEN, 0)
        self._reg_write(model.vars.MODEM_COH3_PEAKCHKTIMOUT, 0)

        # Clock-gating register
        self._reg_write(model.vars.MODEM_AUTOCG_AUTOCGEN, 0) #We calculate MODEM_CGCLKSTOP_FORCEOFF in calculator instead
        self._reg_write(model.vars.FRC_AUTOCG_AUTOCGEN, 7)

        # Shaping filter coefficients
        #FIXME: check with Yan on how to calculate these
        self._reg_write(model.vars.MODEM_SHAPING10_COEFF40, 0)
        self._reg_write(model.vars.MODEM_SHAPING10_COEFF41, 0)
        self._reg_write(model.vars.MODEM_SHAPING10_COEFF42, 0)
        self._reg_write(model.vars.MODEM_SHAPING10_COEFF43, 0)
        self._reg_write(model.vars.MODEM_SHAPING11_COEFF44, 0)
        self._reg_write(model.vars.MODEM_SHAPING11_COEFF45, 0)
        self._reg_write(model.vars.MODEM_SHAPING11_COEFF46, 0)
        self._reg_write(model.vars.MODEM_SHAPING11_COEFF47, 0)
        self._reg_write(model.vars.MODEM_SHAPING12_COEFF48, 0)
        self._reg_write(model.vars.MODEM_SHAPING12_COEFF49, 0)
        self._reg_write(model.vars.MODEM_SHAPING12_COEFF50, 0)
        self._reg_write(model.vars.MODEM_SHAPING12_COEFF51, 0)
        self._reg_write(model.vars.MODEM_SHAPING13_COEFF52, 0)
        self._reg_write(model.vars.MODEM_SHAPING13_COEFF53, 0)
        self._reg_write(model.vars.MODEM_SHAPING13_COEFF54, 0)
        self._reg_write(model.vars.MODEM_SHAPING13_COEFF55, 0)
        self._reg_write(model.vars.MODEM_SHAPING14_COEFF56, 0)
        self._reg_write(model.vars.MODEM_SHAPING14_COEFF57, 0)
        self._reg_write(model.vars.MODEM_SHAPING14_COEFF58, 0)
        self._reg_write(model.vars.MODEM_SHAPING14_COEFF59, 0)
        self._reg_write(model.vars.MODEM_SHAPING15_COEFF60, 0)
        self._reg_write(model.vars.MODEM_SHAPING15_COEFF61, 0)
        self._reg_write(model.vars.MODEM_SHAPING15_COEFF62, 0)
        self._reg_write(model.vars.MODEM_SHAPING15_COEFF63, 0)
        self._reg_write(model.vars.MODEM_SHAPING2_COEFF10, 0)
        self._reg_write(model.vars.MODEM_SHAPING2_COEFF11, 0)
        self._reg_write(model.vars.MODEM_SHAPING2_COEFF9, 0)
        self._reg_write(model.vars.MODEM_SHAPING3_COEFF12, 0)
        self._reg_write(model.vars.MODEM_SHAPING3_COEFF13, 0)
        self._reg_write(model.vars.MODEM_SHAPING3_COEFF14, 0)
        self._reg_write(model.vars.MODEM_SHAPING3_COEFF15, 0)
        self._reg_write(model.vars.MODEM_SHAPING4_COEFF16, 0)
        self._reg_write(model.vars.MODEM_SHAPING4_COEFF17, 0)
        self._reg_write(model.vars.MODEM_SHAPING4_COEFF18, 0)
        self._reg_write(model.vars.MODEM_SHAPING4_COEFF19, 0)
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF20, 0)
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF21, 0)
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF22, 0)
        self._reg_write(model.vars.MODEM_SHAPING5_COEFF23, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF24, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF25, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF26, 0)
        self._reg_write(model.vars.MODEM_SHAPING6_COEFF27, 0)
        self._reg_write(model.vars.MODEM_SHAPING7_COEFF28, 0)
        self._reg_write(model.vars.MODEM_SHAPING7_COEFF29, 0)
        self._reg_write(model.vars.MODEM_SHAPING7_COEFF30, 0)
        self._reg_write(model.vars.MODEM_SHAPING7_COEFF31, 0)
        self._reg_write(model.vars.MODEM_SHAPING8_COEFF32, 0)
        self._reg_write(model.vars.MODEM_SHAPING8_COEFF33, 0)
        self._reg_write(model.vars.MODEM_SHAPING8_COEFF34, 0)
        self._reg_write(model.vars.MODEM_SHAPING8_COEFF35, 0)
        self._reg_write(model.vars.MODEM_SHAPING9_COEFF36, 0)
        self._reg_write(model.vars.MODEM_SHAPING9_COEFF37, 0)
        self._reg_write(model.vars.MODEM_SHAPING9_COEFF38, 0)
        self._reg_write(model.vars.MODEM_SHAPING9_COEFF39, 0)

        # Modem Registers with fixed value
        self._reg_write(model.vars.MODEM_AFC_AFCTXMODE, 0)
#        self._reg_write(model.vars.MODEM_AFC_AFCGEAR, 3)
        self._reg_write(model.vars.MODEM_CTRL0_DEMODRAWDATASEL, 0)
        self._reg_write(model.vars.MODEM_CTRL2_DMASEL, 0)
        self._reg_write(model.vars.MODEM_CTRL3_PRSDINEN, 0)
        self._reg_write(model.vars.MODEM_CTRL4_CLKUNDIVREQ, 0)
        self._reg_write(model.vars.MODEM_CTRL3_RAMTESTEN, 0)
        self._reg_write(model.vars.MODEM_DIRECTMODE_CLKWIDTH, 1)
        self._reg_write(model.vars.MODEM_DIRECTMODE_DMENABLE, 0)
        self._reg_write(model.vars.MODEM_DIRECTMODE_SYNCASYNC, 0)
        self._reg_write(model.vars.MODEM_DIRECTMODE_SYNCPREAM, 3)
        self._reg_write(model.vars.MODEM_PADEBUG_ENMANPACLKAMPCTRL, 0)
        self._reg_write(model.vars.MODEM_PADEBUG_ENMANPAPOWER, 0)
        self._reg_write(model.vars.MODEM_PADEBUG_ENMANPASELSLICE, 0)
        self._reg_write(model.vars.MODEM_PADEBUG_MANPACLKAMPCTRL, 0)
        self._reg_write(model.vars.MODEM_CTRL0_OOKASYNCPIN, 0)
        self._reg_write(model.vars.MODEM_CTRL0_DETDIS, 0)
        self._reg_write(model.vars.MODEM_CTRL0_DUALCORROPTDIS, 0)
        self._reg_write(model.vars.MODEM_CTRL0_FRAMEDETDEL, 0)
        self._reg_write(model.vars.MODEM_CTRL1_SYNC1INV, 0)

        # FRC Registers with fixed value
        self._reg_write(model.vars.FRC_BOICTRL_BOIBITPOS, 0)
        self._reg_write(model.vars.FRC_BOICTRL_BOIEN, 0)
        self._reg_write(model.vars.FRC_BOICTRL_BOIFIELDLOC, 0)
        self._reg_write(model.vars.FRC_BOICTRL_BOIMATCHVAL, 0)
        self._reg_write(model.vars.FRC_CTRL_LPMODEDIS, 0)
        self._reg_write(model.vars.FRC_CTRL_RATESELECT, 0)
        self._reg_write(model.vars.FRC_CTRL_WAITEOFEN, 0)
        self._reg_write(model.vars.FRC_DFLCTRL_DFLBOIOFFSET, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_DSLBITORDER, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_DSLBITS, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_DSLMINLENGTH, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_DSLMODE, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_DSLOFFSET, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_DSLSHIFT, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_RXSUPRECEPMODE, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_STORESUP, 0)
        self._reg_write(model.vars.FRC_DSLCTRL_SUPSHFFACTOR, 0)
        self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TRAILTXREPLEN, 0)
        self._reg_write(model.vars.FRC_TRAILTXDATACTRL_TXSUPPLENOVERIDE, 0)
        self._reg_write(model.vars.FRC_WCNTCMP3_SUPPLENFIELDLOC, 0)
        self._reg_write(model.vars.FRC_WCNTCMP4_SUPPLENGTH, 0)

        # Added new reg-fields related to 15.4 subG OQPSK phys
        self._reg_write(model.vars.MODEM_COH3_COHDSACMPLX, 0)
        self._reg_write(model.vars.MODEM_SYNCPROPERTIES_STATICSYNCTHRESH, 0)

        # Added new reg-fields related to Internal Long Range
        self._reg_write(model.vars.MODEM_PRE_PREWNDERRORS, 0)
        self._reg_write(model.vars.MODEM_CTRL3_TIMINGBASESGAIN, 0)

        #AGC default settings
        self._reg_write(model.vars.AGC_CTRL0_CFLOOPNFADJ, 0)
        self._reg_write(model.vars.AGC_CTRL6_DUALRFPKDDEC, 240296)
        self._reg_write(model.vars.AGC_DUALRFPKDTHD1_RFPKDHITHD0, 1)
        self._reg_write(model.vars.AGC_DUALRFPKDTHD1_RFPKDHITHD1, 40)
        self._reg_write(model.vars.AGC_DUALRFPKDTHD0_RFPKDLOWTHD0, 1)
        self._reg_write(model.vars.AGC_DUALRFPKDTHD0_RFPKDLOWTHD1, 10)
        self._reg_write(model.vars.AGC_CTRL6_ENDUALRFPKD, 1)

        self._reg_write(model.vars.MODEM_SQ_SQEN                , 0)
        self._reg_write(model.vars.MODEM_SQ_SQTIMOUT            , 0)
        self._reg_write(model.vars.MODEM_SQEXT_SQSTG2TIMOUT     , 0)
        self._reg_write(model.vars.MODEM_SQEXT_SQSTG3TIMOUT     , 0)

        # reg-fields to modify sync detection reset behavior PGOCELOT-5282
        self._reg_write(model.vars.MODEM_FRMSCHTIME_PMRSTSYCNEN, 0)
        self._reg_write(model.vars.MODEM_FRMSCHTIME_DSARSTSYCNEN, 0)

        #RAC settings
        self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENDRVN, part_family)
        self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENDRVP, part_family)
        self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENREG3, part_family)
        self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTENBYPASS40MHZ, part_family)
        self._reg_write_default(model.vars.RAC_CLKMULTEN0_CLKMULTREG3ADJV, part_family)
        self._reg_write_default(model.vars.RAC_CLKMULTEN1_CLKMULTDRVAMPSEL, part_family)
        self._reg_write_default(model.vars.RAC_IFADCTRIM0_IFADCSIDETONEAMP, part_family)
