from pyradioconfig.parts.ocelot.phys.Phys_Internal_Base_Customer_Sigfox import PHYS_Sigfox_Ocelot
from py_2_and_3_compatibility import *

class PHYS_Sigfox_Sol(PHYS_Sigfox_Ocelot):

    def PHY_Sigfox_868MHz_DBPSK_100bps(self, model, phy_name=None):
        pass

    def PHY_Sigfox_915MHz_DBPSK_600bps(self, model, phy_name=None):
        pass

    def PHY_Sigfox_915MHz_GFSK_600bps_WIP(self, model,phy_name=None):
        pass

    #def PHY_Sigfox_915MHz_GFSK_600bps_WIP(self, model,phy_name=None):
        '''
        Sigfox downlink (Rx):
        This PHY still needs to be improved to meet -126dBm 10% PER spec, current measurement is arount -124dBm.

        The Rx configuration
        has a special feature. As the base station measures the frequency on the uplink
        packet and adjusts the downlink frequency to it, there is no need to use any
        AFC algorithm in the receiver and the narrowest possible filter bandwidth can
        be configured. That said the specification is still quite challenging at -126
        dBm at 10% PER.
        '''

        '''
        phy = self._makePhy(model, model.profiles.Base, 'Sigfox 915MHz GFSK 600bps PHY')

        phy.profile_inputs.xtal_frequency_hz.value = 38400000
        phy.profile_inputs.base_frequency_hz.value = long(915000000)
        phy.profile_inputs.channel_spacing_hz.value = 0  # spacing is too narrow to use the spacing parameter
        phy.profile_inputs.baudrate_tol_ppm.value = 0

        phy.profile_inputs.bandwidth_hz.value = 2200
        # phy.profile_outputs.MODEM_MODINDEX_FREQGAINM.override = 5
        # phy.profile_outputs.MODEM_MODINDEX_FREQGAINE.override = 4
        # phy.profile_inputs.if_frequency_hz.value = 400000
    
        phy.profile_inputs.rx_xtal_error_ppm.value = 0
        phy.profile_inputs.tx_xtal_error_ppm.value = 0
        
        # 2GFSK
        # 600 bps data rate
        # Deviation = 800 Hz
        phy.profile_inputs.modulation_type.value = model.vars.modulation_type.var_enum.FSK2
        phy.profile_inputs.bitrate.value = 600
        phy.profile_inputs.deviation.value = 800
        
        # BT=1
        phy.profile_inputs.shaping_filter.value = model.vars.shaping_filter.var_enum.Gaussian
        phy.profile_inputs.shaping_filter_param.value = 1.0
        
        phy.profile_inputs.symbols_in_timing_window.value = 16
        phy.profile_inputs.agc_period.value = 0
        phy.profile_inputs.agc_speed.value = model.vars.agc_speed.var_enum.FAST
    
        # 5 bytes of 0x55 preamble
        phy.profile_inputs.preamble_pattern.value = 1  #
        phy.profile_inputs.preamble_pattern_len.value = 2  #
        phy.profile_inputs.preamble_length.value = 40  #
        
        # 13 bit sync word of b1001 0001 0011 1
        phy.profile_inputs.syncword_0.value = long(0x1227)  #
        phy.profile_inputs.syncword_1.value = long(0x0)  #
        phy.profile_inputs.syncword_length.value = 13  #
        
        # 16 byte payload
        
        # payload byte wise LSB first
        phy.profile_inputs.frame_bitendian.value = model.vars.frame_bitendian.var_enum.LSB_FIRST
        
        # Packet Inputs
        phy.profile_inputs.frame_length_type.value = model.vars.frame_length_type.var_enum.FIXED_LENGTH
        phy.profile_inputs.fixed_length_size.value = 16
        phy.profile_inputs.payload_white_en.value = False
        phy.profile_inputs.payload_crc_en.value = True
        
        # Variable length includes header
        phy.profile_inputs.header_en.value = False
        phy.profile_inputs.header_size.value = 0
        phy.profile_inputs.header_calc_crc.value = False
        phy.profile_inputs.header_white_en.value = False
        
        # CRC Inputs
        phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.CRC_16  # CRC polynomial:   CRC_16
        phy.profile_inputs.crc_seed.value = long(0x00000000)  # CRC seed:         0x0000
        phy.profile_inputs.crc_byte_endian.value = model.vars.crc_byte_endian.var_enum.MSB_FIRST  # CRC byte order:   MSB first
        phy.profile_inputs.crc_bit_endian.value = model.vars.crc_bit_endian.var_enum.LSB_FIRST  # CRC bit order:    LSB first
        phy.profile_inputs.crc_input_order.value = model.vars.crc_input_order.var_enum.LSB_FIRST  # CRC engine feed:  LSB first
        phy.profile_inputs.crc_pad_input.value = False
        phy.profile_inputs.crc_invert.value = False

	    # Misc frame configuration
        phy.profile_inputs.diff_encoding_mode.value = model.vars.diff_encoding_mode.var_enum.DISABLED
        phy.profile_inputs.dsss_chipping_code.value = long(0)
        phy.profile_inputs.dsss_len.value = 0
        phy.profile_inputs.dsss_spreading_factor.value = 0
        phy.profile_inputs.fsk_symbol_map.value = model.vars.fsk_symbol_map.var_enum.MAP0

        #phy.profile_inputs.timing_detection_threshold.value = 4
        phy.profile_inputs.timing_sample_threshold.value = 2
        phy.profile_inputs.freq_offset_hz.value = 0

        phy.profile_inputs.errors_in_timing_window.value = 0
        #phy.profile_inputs.agc_settling_delay.value = 24
        phy.profile_inputs.frequency_offset_period.value = 0

        # Force to Legacy demod until verified in Matlab on another demod
        phy.profile_inputs.demod_select.value = model.vars.demod_select.var_enum.LEGACY
        phy.profile_inputs.target_osr.value = 5

        phy.profile_outputs.FEFILT0_DIGIGAINCTRL_DEC0GAIN.override = 1
        phy.profile_outputs.FEFILT0_CF_DEC1GAIN.override = 2
        phy.profile_outputs.MODEM_CTRL3_TSAMPMODE.override = 0
        phy.profile_outputs.MODEM_CTRL2_DEVWEIGHTDIS.override = 1
        phy.profile_outputs.FEFILT0_CF_DEC2.override = 2

        # phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_ALWAYS_ON
        # phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.INTERNAL_LOCK_AT_PREAMBLE_DETECT
        phy.profile_inputs.frequency_comp_mode.value = model.vars.frequency_comp_mode.var_enum.DISABLED

        return phy
        '''
