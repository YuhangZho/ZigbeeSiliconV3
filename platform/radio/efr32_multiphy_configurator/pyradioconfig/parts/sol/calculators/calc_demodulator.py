from pyradioconfig.parts.ocelot.calculators.calc_demodulator import CALC_Demodulator_ocelot
from pycalcmodel.core.variable import ModelVariableFormat, CreateModelVariableEnum
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException
from math import *
from enum import Enum
import numpy as np
import numpy.matlib
from scipy import signal as sp

class Calc_Demodulator_Sol(CALC_Demodulator_ocelot):

    def buildVariables(self, model):

        #Build variables from Ocelot
        super().buildVariables(model)

        self._addModelVariable(model, 'ofdm_option', int, ModelVariableFormat.DECIMAL, desc='Option number of OFDM PHYs')

        #New or modified variables
        model.vars.demod_select.var_enum = CreateModelVariableEnum(
            enum_name='DemodSelectEnum',
            enum_desc='Demod Selection',
            member_data=[
                ['LEGACY', 0, 'Legacy Demod'],
                ['COHERENT', 1, 'Coherent Demod'],
                ['TRECS_VITERBI', 2, 'TRecS + Viterbi Demod'],
                ['TRECS_SLICER', 3, 'TRecS + HD Demod'],
                ['BCR', 4, 'PRO2 BCR Demod'],
                ['LONGRANGE', 5, 'BLE Long Range Demod'],
                ['OFDM', 6, 'OFDM Soft Demod']
            ])

    def calc_init_advanced(self, model):
        pass



    def calc_mod_type_reg(self, model):
        #This function writes the modulation type register

        #Load model variables into local variables
        modformat = model.vars.modulation_type.value

        if modformat == model.vars.modulation_type.var_enum.FSK2:
            mod = 0
        elif modformat == model.vars.modulation_type.var_enum.FSK4:
            mod = 1
        elif modformat == model.vars.modulation_type.var_enum.BPSK:
            mod = 2
        elif modformat == model.vars.modulation_type.var_enum.DBPSK:
            mod = 3
        elif modformat == model.vars.modulation_type.var_enum.OQPSK:
            mod = 4
        elif modformat == model.vars.modulation_type.var_enum.MSK:
            mod = 5
        elif modformat == model.vars.modulation_type.var_enum.OOK or \
                modformat == model.vars.modulation_type.var_enum.ASK:
            mod = 6
        elif modformat == model.vars.modulation_type.var_enum.OFDM:
            mod = 0  # OFDM is not supported in the original demod - just put an arbitrary value in MODFORMAT
        else:
            raise CalculationException('ERROR: modulation method in input file not recognized')

        #Write register
        self._reg_write(model.vars.MODEM_CTRL0_MODFORMAT, mod)


    def calc_demod_sel(self, model):

        modtype = model.vars.modulation_type.value
        tol = model.vars.baudrate_tol_ppm.value
        mi = model.vars.modulation_index.value

        if model.vars.demod_select._value_forced != None:
            demod_select = model.vars.demod_select._value_forced
            [target_osr, dec0, dec1, min_osr, max_osr] = self.return_solution(model, demod_select)

        else:
            # choose demod_select based on modulation and demod priority
            if (modtype == model.vars.modulation_type.var_enum.OFDM):
                demod_select = model.vars.demod_select.var_enum.OFDM
                [target_osr, dec0, dec1, min_osr, max_osr] = self.return_osr_dec0_dec1(model, demod_select)

            elif (modtype == model.vars.modulation_type.var_enum.OOK) or \
                    (modtype==model.vars.modulation_type.var_enum.ASK):
                demod_select = model.vars.demod_select.var_enum.BCR
                [target_osr,dec0,dec1,min_osr,max_osr] = self.return_osr_dec0_dec1(model,demod_select)
                # TODO: Is there a case where osr < 7

            elif (modtype == model.vars.modulation_type.var_enum.OQPSK):
                demod_select = model.vars.demod_select.var_enum.LEGACY
                [target_osr,dec0,dec1,min_osr,max_osr] = self.return_osr_dec0_dec1(model, demod_select)

            elif (modtype == model.vars.modulation_type.var_enum.BPSK) or \
                     (modtype == model.vars.modulation_type.var_enum.DBPSK):
                demod_select = model.vars.demod_select.var_enum.LEGACY
                [target_osr,dec0,dec1,min_osr,max_osr] = self.return_osr_dec0_dec1(model, demod_select)

            elif (modtype == model.vars.modulation_type.var_enum.FSK4):

                if tol > 10000:
                    demod_select = model.vars.demod_select.var_enum.BCR
                    [target_osr,dec0,dec1,min_osr,max_osr] = self.return_osr_dec0_dec1(model, demod_select)
                else:
                    demod_select = model.vars.demod_select.var_enum.LEGACY
                    [target_osr,dec0,dec1,min_osr,max_osr] = self.return_osr_dec0_dec1(model, demod_select)

            elif (modtype == model.vars.modulation_type.var_enum.FSK2 or \
                    modtype == model.vars.modulation_type.var_enum.MSK):

                if tol > 10000:
                    demod_select = model.vars.demod_select.var_enum.BCR
                    [target_osr,dec0,dec1,min_osr,max_osr] = self.return_osr_dec0_dec1(model, demod_select)
                else:
                    if mi < 1.0:
                        demod_select_list = [model.vars.demod_select.var_enum.TRECS_VITERBI,
                                             model.vars.demod_select.var_enum.BCR,
                                             model.vars.demod_select.var_enum.LEGACY]
                    else:
                        demod_select_list = [model.vars.demod_select.var_enum.TRECS_SLICER,
                                             model.vars.demod_select.var_enum.BCR,
                                             model.vars.demod_select.var_enum.LEGACY]

                    # loop over demod list and see if we can find a solution
                    for demod_select in demod_select_list:
                        [target_osr, dec0, dec1, min_osr, max_osr] = self.return_solution( model, demod_select)

                        # stop at first solution
                        if target_osr != 0:
                            break

        if target_osr == 0:
            raise CalculationException('WARNING: target_osr=0 in calc_choose_demod()')

        model.vars.demod_select.value = demod_select
        model.vars.target_osr.value = int(target_osr)
        model.vars.targetmin_osr.value = int(min_osr)
        model.vars.targetmax_osr.value = int(max_osr)
        model.vars.dec0.value = int(dec0)
        model.vars.dec1.value = int(dec1)

    def calc_dec0_reg(self,model):
        #This function writes the register for dec0

        #Load model variables into local variables
        dec0_value = model.vars.dec0.value

        #Define a constant list for the (register data, value pairs)
        dec0_list = [(0, 3), (2, 4), (4, 8), (5, 5)]
        # Search for the value in the list
        for dec0_pair in dec0_list:
            if (dec0_pair[1]==dec0_value):
                dec0_reg = dec0_pair[0]

        #Write the registers
        self._reg_write(model.vars.FEFILT0_CF_DEC0, dec0_reg)

    def calc_dec1_reg(self, model):
        #This function writes the register for dec1

        #Load model variables into local variables
        dec1_value = model.vars.dec1.value

        #Dec1 register is simply one less than the value
        dec1_reg = dec1_value - 1

        #Write the registers
        self._reg_write(model.vars.FEFILT0_CF_DEC1, dec1_reg)

    def calc_dec0_actual(self,model):
        #This function calculates the actual dec0 based on the register value

        #Load model variables into local variables
        dec0_reg = model.vars.FEFILT0_CF_DEC0.value

        #Define a constant list for the (register data, value pairs)
        dec0_list = [(0, 3), (1, 4), (2, 4), (3, 8), (4, 8), (5, 5)]
        #Search for the value in the list
        for dec0_pair in dec0_list:
            if (dec0_pair[0]==dec0_reg):
                dec0_value = dec0_pair[1]

        #Load local variables back into model variables
        model.vars.dec0_actual.value = dec0_value

    def calc_dec1_actual(self, model):
        #This function calculates the actual dec1 based on the register value

        #Load model variables into local variables
        dec1_reg = model.vars.FEFILT0_CF_DEC1.value

        #Dec1 value is simply one more than the register setting
        dec1_value = dec1_reg + 1

        #Load local variables back into model variables
        model.vars.dec1_actual.value = dec1_value

    def calc_src2_dec2(self,model):
        #This function calculates dec2 and src2

        # FIXME: need to have an options for TRecS where DEC2 is bypassed DEC2=1
        #        unless the remod is enabled

        #Load model variables into local variables
        adc_freq = model.vars.adc_freq_actual.value
        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value
        baudrate = model.vars.baudrate.value #We don't know the actual baudrate yet
        target_osr = model.vars.target_osr.value #We don't know the actual OSR value yet
        demod_sel = model.vars.demod_select.value
        max_dec2 = model.vars.max_dec2.value
        min_dec2 = model.vars.min_dec2.value

        min_src2 = model.vars.min_src2.value  # min value for SRC2
        max_src2 = model.vars.max_src2.value  # max value for SRC2

        if model.vars.FEFILT0_CF_DEC2.value_forced != None and model.vars.FEFILT0_SRCCHF_SRCRATIO2.value_forced != None:
            model.vars.dec2.value = model.vars.FEFILT0_CF_DEC2.value + 1
            model.vars.src2_ratio.value = model.vars.FEFILT0_SRCCHF_SRCRATIO2.value / 16384.0
            return
        elif model.vars.FEFILT0_CF_DEC2.value_forced != None:
            model.vars.dec2.value = model.vars.FEFILT0_CF_DEC2.value + 1
            model.vars.src2_ratio.value = target_osr * 8.0 * dec0 * dec1 * model.vars.dec2.value * baudrate / adc_freq
            return
        elif model.vars.FEFILT0_SRCCHF_SRCRATIO2.value_forced != None:
            model.vars.src2_ratio.value = model.vars.FEFILT0_SRCCHF_SRCRATIO2.value / 16384.0
            model.vars.dec2.value = int(round(model.vars.src2_ratio.value * adc_freq / (8.0 * target_osr * dec0 * dec1 * baudrate)))
            return

        if (demod_sel==model.vars.demod_select.var_enum.BCR):
            # BCR demod, dec2 and src2 not enabled
            best_dec2 = 1
            best_src2 = 1.0

        else:
            # Legacy, Coherent, Trecs/Viterbi Demods
            #Calculate the OSR at the input to SRC2
            osr_src2_min = float(adc_freq) / (8 * dec0 * dec1 * baudrate) * min_src2
            osr_src2_max = float(adc_freq) / (8 * dec0 * dec1 * baudrate) * max_src2

            #Calculate dec2 to achieve close to the target OSR
            dec2_min = max(int( ceil(osr_src2_min / target_osr)),min_dec2)
            dec2_max = min(int(floor(osr_src2_max / target_osr)),max_dec2)

            target_src2 = 1.0
            best_error = 999
            for dec2 in range(dec2_min, dec2_max + 1):
                src2 = dec2 * (8 * dec0 * dec1 * baudrate) * target_osr / float(adc_freq)
                error = abs(target_src2 - src2)
                if best_error > error:
                    best_error = error
                    best_src2 = src2
                    best_dec2 = dec2

            # model.vars.brcalen.value is only needed in legacy demod case - in viterbi case it creates a circular
            # dependency which is why we don't read it in at the top of this function
            # if demod_sel == model.vars.demod_select.var_enum.LEGACY and model.vars.brcalen.value:
            #     # in legacy demod if BRCAL is enabled cannot use SRC2
            #     best_src2 = 1.0

        #Load local variables back into model variables
        model.vars.dec2.value = best_dec2
        model.vars.src2_ratio.value = best_src2

    def calc_src2_reg(self,model):
        #This function calculates the src2 register writes

        # Load model variables into local variables
        src2_value = model.vars.src2_ratio.value
        min_src2 = model.vars.min_src2.value  # min value for SRC2
        max_src2 = model.vars.max_src2.value  # max value for SRC2

        if model.vars.FEFILT0_SRCCHF_SRCRATIO2.value_forced != None:
            src2_forced =  model.vars.FEFILT0_SRCCHF_SRCRATIO2.value / 16384.0
            if (src2_forced > max_src2):
                max_src2 = src2_forced
            if (src2_forced < min_src2):
                min_src2 = src2_forced

        if (src2_value) >= min_src2 and (src2_value <= max_src2):
            src2_reg = int(round(16384/src2_value))
        else:
            raise CalculationException('WARNING: src2 value out of range in calc_src2_reg()')

        if (src2_reg != 16384):
            src2_en = 1
        else:
            src2_en = 0

        #Write to registers
        self._reg_write(model.vars.FEFILT0_SRCCHF_SRCRATIO2, src2_reg)
        self._reg_write(model.vars.FEFILT0_SRCCHF_SRCENABLE2, src2_en)

    def calc_dec2_reg(self,model):
        #This function calculates the dec2 register value

        #Load model variables into local variables
        dec2_value = model.vars.dec2.value

        #The dec2 register is one less than the decimation value
        dec2_reg = dec2_value - 1

        #Write to register
        self._reg_write(model.vars.FEFILT0_CF_DEC2, dec2_reg)

    def calc_src2_actual(self,model):
        #This function calculates the actual SRC2 ratio from the register value

        #Load model variables into local variables
        src2_reg = model.vars.FEFILT0_SRCCHF_SRCRATIO2.value
        src2_en_reg = model.vars.FEFILT0_SRCCHF_SRCENABLE2.value

        if src2_en_reg:
            #The src2 ratio is simply 16384 divided by the register value
            src2_ratio_actual = 16384.0 / src2_reg
        else:
            src2_ratio_actual = 1.0

        #Load local variables back into model variables
        model.vars.src2_ratio_actual.value = src2_ratio_actual

    def calc_dec2_actual(self,model):
        #This function calculates the actual dec2 ratio from the register value

        #Load model variables into local variables
        dec2_reg = model.vars.FEFILT0_CF_DEC2.value

        #The actual dec2 value is the dec2 register plus one
        dec2_actual = dec2_reg + 1

        #Load local variables back into model variables
        model.vars.dec2_actual.value = dec2_actual

    def calc_baudrate(self, model):
        # This function calculates baudrate based on the input bitrate and modulation/encoding settings

        # Load model variables into local variables
        mod_type = model.vars.modulation_type.value
        bitrate = model.vars.bitrate.value
        encoding = model.vars.symbol_encoding.value
        spreading_factor = model.vars.dsss_spreading_factor.value
        mbus_encoding = model.vars.mbus_symbol_encoding.value
        ofdm_opt = model.vars.ofdm_option.value
        fec_enabled = model.vars.fec_enabled.value

        # Based on modulation type calculate baudrate from bitrate
        if (mod_type == model.vars.modulation_type.var_enum.OQPSK) or \
                (mod_type == model.vars.modulation_type.var_enum.OOK) or \
                (mod_type == model.vars.modulation_type.var_enum.ASK) or \
                (mod_type == model.vars.modulation_type.var_enum.FSK2) or \
                (mod_type == model.vars.modulation_type.var_enum.BPSK) or \
                (mod_type == model.vars.modulation_type.var_enum.DBPSK) or \
                (mod_type == model.vars.modulation_type.var_enum.MSK):
            baudrate = bitrate
        elif (mod_type == model.vars.modulation_type.var_enum.FSK4):
            baudrate = bitrate / 2
        elif (mod_type == model.vars.modulation_type.var_enum.OFDM):
            if ofdm_opt == 1:
                baudrate = 4e6/3.0
            elif ofdm_opt == 2:
                baudrate = 4e6/6.0
            elif ofdm_opt == 3:
                baudrate = 4e6/12.0
            else:
                baudrate = 4e6/24.0
        else:
            raise CalculationException('ERROR: modulation type not supported in calc_baudrate()')

        # Adjust baudrate based on symbol encoding
        if (encoding == model.vars.symbol_encoding.var_enum.Manchester):
            baudrate *= 2
        if (encoding == model.vars.symbol_encoding.var_enum.DSSS):
            baudrate *= spreading_factor
        if (mbus_encoding == model.vars.mbus_symbol_encoding.var_enum.MBUS_3OF6):
            baudrate *= 1.5
        if fec_enabled:
            baudrate *= 2

        # Load local variables back into model variables
        model.vars.baudrate.value = int(round(baudrate))


    def calc_baudrate_actual(self,model):
        #This function calculates the actual baudrate based on register settings

        #Load model variables into local variables
        adc_freq = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        dec1_actual = model.vars.dec1_actual.value
        dec2_actual = model.vars.dec2_actual.value
        src2ratio_actual = model.vars.src2_ratio_actual.value
        subfrac_actual = model.vars.subfrac_actual.value
        rxbrfrac_actual = model.vars.rxbrfrac_actual.value
        dec = model.vars.MODEM_BCRDEMODOOK_RAWNDEC.value

        if subfrac_actual > 0:
            frac = subfrac_actual * pow(2, dec)
        else:
            frac = rxbrfrac_actual

        #Calculate actual baudrate once the ADC, decimator, SRC, and rxbr settings are kown
        baudrate_actual = (adc_freq * src2ratio_actual) / (dec0_actual * dec1_actual * dec2_actual * 8 * 2 * frac)

        #Load local variables back into model variables
        model.vars.rx_baud_rate_actual.value = baudrate_actual

    def calc_bwsel(self,model):

        #This function calculates the bwsel ratio that sets the channel bandwidth

        #Load model variables into local variables
        adc_freq = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        dec1_actual = model.vars.dec1_actual.value
        afc_run_mode = model.vars.afc_run_mode.value
        bandwidth = model.vars.bandwidth_hz.value #We don't know the actual channel bandwidth yet
        lock_bandwidth = model.vars.lock_bandwidth_hz.value  # maybe this cab be reduced further based on residual freq offset
        min_bwsel = model.vars.min_bwsel.value

        # This check includes all forces, and issues a warning for development team
        if (lock_bandwidth > bandwidth):
            print('  WARNING: lock_bandwidth > bandwidth_hz in calc_bwsel()')
            print('           Change forced values so that lock_bandwidth_hz <= bandwidth_hz')

        #Calculate the required BWSEL from the adc rate, decimators, and required bandwidth
        bwsel = float(bandwidth * 8 * dec0_actual * dec1_actual) / adc_freq
        lock_bwsel = float(lock_bandwidth * 8 * dec0_actual * dec1_actual) / adc_freq

        if (lock_bwsel < min_bwsel) and (afc_run_mode == model.vars.afc_run_mode.var_enum.ONE_SHOT):
            lock_bwsel = min_bwsel

        #Load local variables back into model variables
        model.vars.bwsel.value = bwsel
        model.vars.lock_bwsel.value = lock_bwsel

    def calc_chfilt_reg(self,model):

        #This function calculates the channel filter registers

        #Load model variables into local variables
        bwsel = model.vars.bwsel.value

        coeffs = self.return_coeffs(bwsel)

        bit_widths = [10,10,10,11,11,11,12,12,12,14,14,14,16,16,16]

        # replace negative numbers with 2s complement
        for i in range(15):
            if coeffs[i] < 0:
                coeffs[i] = coeffs[i] + 2**bit_widths[i]

        # Write registers
        self._reg_write(model.vars.FEFILT0_CHFCOE00_SET0COEFF0,  coeffs[0])
        self._reg_write(model.vars.FEFILT0_CHFCOE00_SET0COEFF1,  coeffs[1])
        self._reg_write(model.vars.FEFILT0_CHFCOE00_SET0COEFF2,  coeffs[2])
        self._reg_write(model.vars.FEFILT0_CHFCOE01_SET0COEFF3,  coeffs[3])
        self._reg_write(model.vars.FEFILT0_CHFCOE01_SET0COEFF4,  coeffs[4])
        self._reg_write(model.vars.FEFILT0_CHFCOE02_SET0COEFF5,  coeffs[5])
        self._reg_write(model.vars.FEFILT0_CHFCOE02_SET0COEFF6,  coeffs[6])
        self._reg_write(model.vars.FEFILT0_CHFCOE03_SET0COEFF7,  coeffs[7])
        self._reg_write(model.vars.FEFILT0_CHFCOE03_SET0COEFF8,  coeffs[8])
        self._reg_write(model.vars.FEFILT0_CHFCOE04_SET0COEFF9,  coeffs[9])
        self._reg_write(model.vars.FEFILT0_CHFCOE04_SET0COEFF10, coeffs[10])
        self._reg_write(model.vars.FEFILT0_CHFCOE05_SET0COEFF11, coeffs[11])
        self._reg_write(model.vars.FEFILT0_CHFCOE05_SET0COEFF12, coeffs[12])
        self._reg_write(model.vars.FEFILT0_CHFCOE06_SET0COEFF13, coeffs[13])
        self._reg_write(model.vars.FEFILT0_CHFCOE06_SET0COEFF14, coeffs[14])

        # Load model variables into local variables
        bwsel = model.vars.lock_bwsel.value
        coeffs = self.return_coeffs(bwsel)

        # replace negative numbers with 2s complement
        for i in range(15):
            if coeffs[i] < 0:
                coeffs[i] = coeffs[i] + 2**bit_widths[i]

        # TODO: calculate the second set separately
        self._reg_write(model.vars.FEFILT0_CHFCOE10_SET1COEFF0,  coeffs[0])
        self._reg_write(model.vars.FEFILT0_CHFCOE10_SET1COEFF1,  coeffs[1])
        self._reg_write(model.vars.FEFILT0_CHFCOE10_SET1COEFF2,  coeffs[2])
        self._reg_write(model.vars.FEFILT0_CHFCOE11_SET1COEFF3,  coeffs[3])
        self._reg_write(model.vars.FEFILT0_CHFCOE11_SET1COEFF4,  coeffs[4])
        self._reg_write(model.vars.FEFILT0_CHFCOE12_SET1COEFF5,  coeffs[5])
        self._reg_write(model.vars.FEFILT0_CHFCOE12_SET1COEFF6,  coeffs[6])
        self._reg_write(model.vars.FEFILT0_CHFCOE13_SET1COEFF7,  coeffs[7])
        self._reg_write(model.vars.FEFILT0_CHFCOE13_SET1COEFF8,  coeffs[8])
        self._reg_write(model.vars.FEFILT0_CHFCOE14_SET1COEFF9,  coeffs[9])
        self._reg_write(model.vars.FEFILT0_CHFCOE14_SET1COEFF10, coeffs[10])
        self._reg_write(model.vars.FEFILT0_CHFCOE15_SET1COEFF11, coeffs[11])
        self._reg_write(model.vars.FEFILT0_CHFCOE15_SET1COEFF12, coeffs[12])
        self._reg_write(model.vars.FEFILT0_CHFCOE16_SET1COEFF13, coeffs[13])
        self._reg_write(model.vars.FEFILT0_CHFCOE16_SET1COEFF14, coeffs[14])

    def return_coeffs(self, bwsel):

        # this table is generated with srw_model/models/channel_filters/gen_channel_filter_coeffs.m
        if bwsel < 0.205:
            coeffs = [30, 100, 200, 268, 201, -88, -581, -1088, -1264, -744, 660, 2792, 5128, 6951, 7639]
        elif bwsel < 0.215:
            coeffs = [29, 107, 239, 372, 391, 176, -304, -887, -1219, -877, 400, 2502, 4894, 6799, 7525]
        elif bwsel < 0.225:
            coeffs = [12, 77, 206, 373, 464, 325, -126, -775, -1256, -1075, 127, 2297, 4874, 6971, 7778]
        elif bwsel < 0.235:
            coeffs = [-1, 42, 160, 348, 510, 467, 72, -619, -1250, -1255, -159, 2066, 4836, 7146, 8045]
        elif bwsel < 0.245:
            coeffs = [-14, -5, 74, 257, 485, 569, 290, -397, -1178, -1416, -479, 1791, 4803, 7395, 8417]
        elif bwsel < 0.255:
            coeffs = [-22, -38, 8, 179, 448, 631, 455, -209, -1093, -1515, -718, 1570, 4764, 7576, 8697]
        elif bwsel < 0.265:
            coeffs = [-29, -72, -69, 73, 372, 658, 611, 3, -968, -1585, -954, 1337, 4722, 7779, 9010]
        elif bwsel < 0.275:
            coeffs = [-34, -104, -155, -65, 237, 623, 733, 229, -797, -1614, -1178, 1099, 4690, 8026, 9385]
        elif bwsel < 0.285:
            coeffs = [-39, -138, -255, -244, 35, 510, 800, 440, -610, -1628, -1403, 865, 4710, 8388, 9905]
        elif bwsel < 0.295:
            coeffs = [-30, -141, -307, -386, -170, 352, 801, 593, -478, -1716, -1722, 569, 4774, 8920, 10651]
        elif bwsel < 0.305:
            coeffs = [-14, -107, -294, -452, -329, 190, 787, 783, -223, -1644, -1940, 240, 4662, 9179, 11093]
        elif bwsel < 0.315:
            coeffs = [2, -61, -243, -466, -461, 1, 707, 925, 41, -1517, -2112, -87, 4513, 9395, 11493]
        elif bwsel < 0.325:
            coeffs = [17, -8, -163, -434, -556, -194, 576, 1013, 297, -1351, -2240, -407, 4338, 9570, 11851]
        elif bwsel < 0.335:
            coeffs = [30, 50, -55, -340, -586, -370, 398, 1031, 526, -1149, -2313, -707, 4128, 9676, 12132]
        elif bwsel < 0.345:
            coeffs = [41, 107, 69, -209, -573, -536, 183, 997, 733, -930, -2363, -1012, 3898, 9772, 12412]
        elif bwsel < 0.355:
            coeffs = [50, 163, 211, -17, -468, -629, -28, 904, 882, -723, -2397, -1316, 3631, 9808, 12627]
        elif bwsel < 0.365:
            coeffs = [45, 189, 329, 211, -245, -578, -161, 812, 1047, -385, -2220, -1498, 3232, 9442, 12324]
        elif bwsel < 0.375:
            coeffs = [29, 163, 345, 312, -126, -588, -327, 684, 1153, -145, -2161, -1728, 2996, 9496, 12562]
        elif bwsel < 0.385:
            coeffs = [11, 120, 335, 408, 33, -549, -498, 501, 1231, 133, -2059, -1974, 2716, 9556, 12843]
        elif bwsel < 0.395:
            coeffs = [-7, 65, 286, 460, 189, -462, -623, 306, 1259, 386, -1936, -2188, 2455, 9627, 13136]
        elif bwsel < 0.405:
            coeffs = [-26, -7, 191, 461, 340, -323, -704, 94, 1243, 637, -1784, -2395, 2186, 9727, 13490]
        elif bwsel < 0.415:
            coeffs = [-43, -80, 70, 410, 459, -156, -727, -99, 1197, 860, -1620, -2573, 1931, 9832, 13847]
        elif bwsel < 0.425:
            coeffs = [-61, -173, -113, 268, 526, 43, -691, -284, 1131, 1112, -1394, -2734, 1658, 9961, 14268]
        elif bwsel < 0.435:
            coeffs = [-68, -249, -307, 50, 473, 131, -735, -535, 1017, 1322, -1264, -3025, 1435, 10497, 15288]
        elif bwsel < 0.445:
            coeffs = [-50, -239, -383, -89, 458, 306, -645, -729, 838, 1505, -1001, -3166, 1111, 10603, 15732]
        elif bwsel < 0.455:
            coeffs = [-29, -203, -420, -229, 390, 451, -508, -877, 629, 1637, -729, -3264, 795, 10676, 16131]
        elif bwsel < 0.465:
            coeffs = [-2, -137, -413, -369, 263, 567, -322, -988, 378, 1731, -421, -3331, 448, 10717, 16524]
        elif bwsel < 0.475:
            coeffs = [25, -49, -341, -454, 109, 609, -136, -1028, 141, 1761, -141, -3344, 140, 10696, 16796]
        elif bwsel < 0.485:
            coeffs = [52, 61, -210, -493, -81, 587, 55, -1027, -114, 1747, 156, -3322, -188, 10638, 17045]
        elif bwsel < 0.495:
            coeffs = [83, 196, -7, -446, -254, 508, 211, -1017, -391, 1677, 430, -3295, -526, 10568, 17294]
        else:
            coeffs = [85, 274, 199, -265, -276, 473, 408, -853, -554, 1550, 686, -3080, -779, 10093, 16843]

        # Confirm Sum of the Magnitudes is in spec to not overflow the
        # filter accumulator
        try:
            assert sum([abs(i) for i in coeffs]) < 2**16
        except AssertionError:
            raise CalculationException('ERROR: Channel Filter Coefficients Sum of Magnitudes >= 2^16')

        return coeffs

    def calc_digmixfreq_reg(self,model):

        #This function calculates the digital mixer register

        #Load model variables into local variables
        fif = model.vars.if_frequency_hz_actual.value #IF frequency based on the actual SYNTH settings
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        # digital mixer frequency resolution, Hz/mixer ticks
        digmix_res = adc_freq_actual/((2**20) * 8.0 * dec0_actual)
        #FIXME: this function should be split into two one to calculate digmix_res_actual and one that calculates the register
        model.vars.digmix_res_actual.value = digmix_res
        #Calculate the register setting for digital mixer
        digmixfreq_reg = int(round(fif/digmix_res))

        # Write register
        self._reg_write(model.vars.FEFILT0_DIGMIXCTRL_DIGMIXFREQ, digmixfreq_reg)

    def calc_digmixfreq_actual(self,model):

        #This function calculates the actual digital mixer frequency based on the register

        #Load model variables into local variables
        digmixfreq_reg = model.vars.FEFILT0_DIGMIXCTRL_DIGMIXFREQ.value
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value

        #Calculate the actual mixer frequency
        digmixfreq_actual = int(digmixfreq_reg * model.vars.digmix_res_actual.value)

        #Load local variables back into model variables
        model.vars.digmixfreq_actual.value = digmixfreq_actual

    def calc_demod_rate_actual(self,model):
        #This function calculates the actual sample rate at the demod

        # Load model variables into local variables
        adc_freq_actual = model.vars.adc_freq_actual.value
        dec0_actual = model.vars.dec0_actual.value
        dec1_actual = model.vars.dec1_actual.value
        dec2_actual = model.vars.dec2_actual.value
        src2_actual = model.vars.src2_ratio_actual.value

        demod_rate_actual = int(adc_freq_actual * src2_actual / (8 * dec0_actual * dec1_actual * dec2_actual))

        #Load local variables back into model variables
        model.vars.demod_rate_actual.value = demod_rate_actual

    def calc_intosr_reg(self, model):
        # This function sets INTOSR register field
        osr = model.vars.oversampling_rate_actual.value

        if abs(round(osr) - osr) < 0.001:
            intosr = 1
        else:
            intosr = 0

        self._reg_write(model.vars.FEFILT0_SRCCHF_INTOSR, intosr)


    def calc_dec1gain_value(self, model):
        """calculate additional gain we want in the DEC1 decimator for very low bandwidth
        PHY settings.
        see register definition of DEC1GAIN in EFR32 Reference Manual (internal.pdf)

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        bw = model.vars.bandwidth_actual.value

        if bw < 500:
            dec1gain = 12
        elif bw < 2000:
            dec1gain = 6
        else:
            dec1gain = 0

        model.vars.dec1gain.value = dec1gain


    def calc_dec1gain_reg(self, model):
        """set DEC1GAIN register based on calculated value

        Args:
            model (ModelRoot) : Data model to read and write variables from
        """

        val = model.vars.dec1gain.value

        if val == 12:
            reg = 2
        elif val == 6:
            reg = 1
        else:
            reg = 0

        self._reg_write(model.vars.FEFILT0_CF_DEC1GAIN,  reg)



    def calc_datapath_delays(self, model):

        dec0 = model.vars.dec0_actual.value
        dec1 = model.vars.dec1_actual.value
        dec2 = model.vars.dec2_actual.value
        datafilter_taps = model.vars.datafilter_taps.value
        chflatency = model.vars.FEFILT0_CHFLATENCYCTRL_CHFLATENCY.value
        src2_actual = model.vars.src2_ratio_actual.value
        demod_sel = model.vars.demod_select.value
        remoden = model.vars.MODEM_PHDMODCTRL_REMODEN.value
        remoddwn = model.vars.MODEM_PHDMODCTRL_REMODDWN.value
        trecs_enabled = model.vars.trecs_enabled.value

        # need to flush out the entire delay line so delay is not group delay but number of taps
        # DEC8 delay: 22 taps
        del_dec8 = 22
        # DEC0 delay: 27 or 40 taps depending on decimation
        del_dec0 = 27.0 if dec0 == 3 or dec0 == 4 else 40
        # DC cancel filter group delay of 1, IRCAL delay of 1, no delay in dig mixer
        del_dc_ircal_digmix  = 2
        # DEC1 delay: 4 additional taps per decimation as this is 4th order CIC
        del_dec1 = (dec1 - 1) * 4.0 + 1
        # CHFLT delay: 29 taps minus the 6 taps for each increment in latency reduction field
        del_chflt = 29.0 - chflatency * 6.0
        # SRC delay: can be up to 2 samples
        del_src2 = 2
        # Digital gain and CORDIC do not introduce any delays
        del_digigain = 0
        del_cordic = 0
        # Differentiation delay of 1, frequency gain has no delay
        del_diff = 1
        # DEC2 delay: 1st or CIC so number of taps is the same as decimation
        del_dec2 = dec2
        # DATAFILT delay: number of taps
        del_data = datafilter_taps
        # remod operation delay
        # FIXME: verify the delay in this block
        del_remod = remoddwn

        del_adc_to_diff = (((del_dec8 / 8 + del_dec0) / dec0 + del_dc_ircal_digmix + del_dec1) / dec1 + del_chflt + del_src2 ) / src2_actual + \
            del_digigain + del_cordic + del_diff

        grpdel_mixer_to_diff = ( (del_dec1+1)/2 / dec1 + (del_chflt+1)/2 + del_src2) / src2_actual + del_digigain + del_cordic + del_diff

        if trecs_enabled:
            if remoden == 1 and remoddwn == 0: # demod at DEC2 output
                delay_adc_to_demod =  (del_adc_to_diff + del_dec2) / dec2  # delay at dec2 output in samples at that point
                grpdelay_to_demod = (grpdel_mixer_to_diff + (del_dec2+1)/2) / dec2  # delay at dec2 output in samples at that point
                delay_agc = delay_adc_to_demod * dec2 * src2_actual
            elif remoden == 1 and remoddwn > 1:
                delay_adc_to_demod = ((del_adc_to_diff + del_dec2) / dec2 + del_data + del_remod) / remoddwn
                grpdelay_to_demod = ((grpdel_mixer_to_diff + (del_dec2+1)/2) / dec2 + (del_data+1)/2 + (del_remod+1)/2) / remoddwn
                delay_agc = delay_adc_to_demod * dec2 * src2_actual * remoddwn
            else:
                delay_adc_to_demod = del_adc_to_diff
                grpdelay_to_demod = grpdel_mixer_to_diff
                delay_agc = del_adc_to_diff * src2_actual

        else:
            delay_adc_to_demod = (del_adc_to_diff + del_dec2) / dec2 + del_data
            grpdelay_to_demod = (grpdel_mixer_to_diff + (del_dec2+1)/2) / dec2 + (del_data+1)/2
            delay_agc = delay_adc_to_demod * dec2 * src2_actual

        model.vars.grpdelay_to_demod.value = int(ceil(grpdelay_to_demod))
        model.vars.delay_adc_to_demod.value = int(ceil(delay_adc_to_demod))
        model.vars.agc_settling_delay.value = int(ceil(delay_agc))

    def calc_dccomp_misc_reg(self, model):

        # always enable both DC offset estimation and compensation blocks
        self._reg_write(model.vars.FEFILT0_DCCOMP_DCCOMPEN, 1)
        self._reg_write(model.vars.FEFILT0_DCCOMP_DCESTIEN, 1)

        # don't reset at every packet
        self._reg_write(model.vars.FEFILT0_DCCOMP_DCRSTEN, 0)

        # always enable gear shifting option
        self._reg_write(model.vars.FEFILT0_DCCOMP_DCGAINGEAREN, 1)

        # when AGC gain change happens set the gear to fastest
        self._reg_write(model.vars.FEFILT0_DCCOMP_DCGAINGEAR, 7)

        # final gear setting after settling
        self._reg_write(model.vars.FEFILT0_DCCOMP_DCCOMPGEAR, 3)

        # limit max DC to 1V
        self._reg_write(model.vars.FEFILT0_DCCOMP_DCLIMIT, 0)

        # don't freeze state of DC comp filters
        self._reg_write(model.vars.FEFILT0_DCCOMP_DCCOMPFREEZE, 0)

        # time between gear shifts - set to fixed value for now
        self._reg_write(model.vars.FEFILT0_DCCOMP_DCGAINGEARSMPS, 40)

    def gen_frequency_signal(self, x, sf, cf, sfosr, model):
        # get parameters
        deviation = model.vars.deviation.value
        baudrate = model.vars.baudrate.value
        demodosr = round(model.vars.oversampling_rate_actual.value)
        src2 = model.vars.FEFILT0_SRCCHF_SRCRATIO2.value
        datafilter = model.vars.MODEM_CTRL2_DATAFILTER.value
        remoden = model.vars.MODEM_PHDMODCTRL_REMODEN.value
        remodoutsel = model.vars.MODEM_PHDMODCTRL_REMODOUTSEL.value
        rawfltsel = model.vars.MODEM_BCRDEMODCTRL_RAWFLTSEL.value
        demod_select = model.vars.demod_select.value
        rawgain = model.vars.MODEM_BCRDEMODOOK_RAWGAIN.value
        dec2 = model.vars.dec2_actual.value
        rawndec = model.vars.MODEM_BCRDEMODOOK_RAWNDEC.value

        remodpath = True if remoden or demod_select == model.vars.demod_select.var_enum.BCR else False

        if demod_select == model.vars.demod_select.var_enum.BCR:
            dec2 = 2 ** rawndec

        # scale shaping filter to desired amplitude OSR = 8
        sf = sf / np.sum(sf) * sfosr

        # pulse shape OSR = 8
        y = sp.lfilter(sf, 1, x)

        # apply deviation OSR = 8
        z = y * deviation

        # integrate to get phase after scaling by sampling rate at TX OSR = 8
        t = np.cumsum(z / (baudrate * sfosr))

        # modulate at baseband OSR = 8
        u = np.exp(1j * 2 * pi * t)

        # resample at channel filter rate (e.g. sfosr -> osr) OSR = chflt_osr * src2
        # FIXME: handle other remod paths here if we end up using them
        if remodpath:
            osr = demodosr * dec2
        else:
            osr = demodosr

        u2 = sp.resample_poly(u,osr*src2, sfosr*16384)

        # channel filter OSR = chflt_osr * src2
        v = sp.lfilter(cf, 1, u2)

        # src2 - resample to target OSR rate OSR = target_osr * dec2
        v2 = sp.resample_poly(v, 16384, src2)

        # CORDIC OSR = target_osr * dec2
        a = np.unwrap(np.angle(v2))

        # downsample by dec2 to get to target_osr if remod enabled
        if remodpath: #and remodoutsel == 1:
            # differentiate phase to frequency OSR = target_osr * dec2
            f1 = a[1:] - a[0:-1]
            #f = sp.resample_poly(f1, 1, dec2)

            # when downsampling pick the best phase that results in max eye opening as we are going to feed the samples
            # from here to the datafilter. Low value samples will bring the average soft decision to a lower value.
            best_min = 0
            for phase in range(dec2):
                f2 = sp.resample_poly(f1[round(len(f1)/4)+phase:], 1, dec2)
                min_val = min(abs(f2[3:-3]))
                if min_val >= best_min:
                    best_min = min_val
                    f = f2
        else:
            # differentiate phase to frequency OSR = target_osr * dec2
            f = a[osr:] - a[0:-osr]

        # optional decimation and filtering for remod paths
        if demod_select == model.vars.demod_select.var_enum.BCR:
            ma1 = self.get_ma1_filter(rawgain)
            g1 = sp.lfilter(ma1, 1, f)
            ma2 = self.get_ma2_filter(rawfltsel)
            g = sp.lfilter(ma2, 1, g1)
        elif remoden and (remodoutsel == 0 or remodoutsel == 1):
            df = self.get_data_filter(datafilter)
            g = sp.lfilter(df, 1, f)
        else:
            g = f

        # return frequency signal
        return g

    def return_dec0_list(self,if_frequency_hz,adc_freq):
        # The purpose of this function is determine the prioritized dec0 list from decimation options 3,4,8
        # Rules:
        # 1) DEC0=8 was only designed for adc_freq <= 40MHz
        # 2) DEC0 anti-aliasing rejection >60dB for DEC0=8 and 4

        first_null_d8 = float(adc_freq) / (8 * 8)
        ratio_d8 = float(if_frequency_hz) / first_null_d8

        first_null_d4 = float(adc_freq) / (8 * 4)
        ratio_d4 = float(if_frequency_hz) / first_null_d4

        if (ratio_d8 < 0.248) and (adc_freq <= 40e6):
            # 0.248 = (.125-.094)/.125 corresponds to >60dB attenuation on d0=8 response
            dec0_priority_list = [8,4,3,5]
        elif ratio_d4 < 0.27:
            # 0.27 = (.25-0.1825)/.25 corresponds to >60dB attenuation on d0=4 response
            dec0_priority_list = [4,3,5]
        else:
            dec0_priority_list = [3,4,5]

        return dec0_priority_list
