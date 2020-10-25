from pyradioconfig.calculator_model_framework.interfaces.iprofile import IProfile
from pyradioconfig.parts.common.utils.units_multiplier import UnitsMultiplier
from pyradioconfig.parts.common.profiles.ocelot_regs import build_modem_regs_ocelot
from pyradioconfig.parts.common.profiles.profile_modem import buildModemInfoOutputs, buildRailOutputs
from pyradioconfig.parts.common.profiles.profile_common import buildCrcOutputs, buildFecOutputs, buildFrameOutputs, \
    buildWhiteOutputs, build_ircal_sw_vars
from pycalcmodel.core.output import ModelOutput, ModelOutputType

class Profile_WiSUN_Ocelot(IProfile):

    phySunFskSfd_0_uncoded = 0b1001000001001110
    phySunFskSfd_1_uncoded = 0b0111101000001110
    phySunFskSfd_0_coded = 0b0110111101001110
    phySunFskSfd_1_coded = 0b0110001100101101

    def __init__(self):
        self._profileName = "WiSUN"
        self._readable_name = "WiSUN Profile"
        self._category = ""
        self._description = "Profile used for WiSUN PHYs"
        self._default = False
        self._activation_logic = ""
        self._family = "ocelot"

    def buildProfileModel(self, model):

        # Build profile object and append it to the model
        profile = self._makeProfile(model)

        # Build inputs
        self.build_required_profile_inputs(model, profile)
        self.build_optional_profile_inputs(model, profile)
        self.build_advanced_profile_inputs(model, profile)
        self.build_hidden_profile_inputs(model, profile)

        # Build outputs
        self.build_register_profile_outputs(model, profile)
        self.build_variable_profile_outputs(model, profile)
        self.build_info_profile_outputs(model, profile)

    def build_required_profile_inputs(self, model, profile):

        IProfile.make_required_input(profile, model.vars.base_frequency_hz, "operational_frequency",
                                     readable_name="Base Channel Frequency", value_limit_min=358000000,
                                     value_limit_max=956000000, units_multiplier=UnitsMultiplier.MEGA)
        IProfile.make_required_input(profile, model.vars.channel_spacing_hz, "operational_frequency",
                                     readable_name="Channel Spacing", value_limit_min=0,
                                     value_limit_max=10000000,
                                     units_multiplier=UnitsMultiplier.KILO)
        IProfile.make_required_input(profile, model.vars.xtal_frequency_hz, "crystal",
                                     readable_name="Crystal Frequency", value_limit_min=38000000,
                                     value_limit_max=40000000, units_multiplier=UnitsMultiplier.MEGA)
        IProfile.make_required_input(profile, model.vars.preamble_length, "preamble",
                                     readable_name="Preamble Length Total", value_limit_min=0, value_limit_max=2097151)
        IProfile.make_required_input(profile, model.vars.wisun_mode, "WiSUN",
                                     readable_name="WiSUN Mode")
        IProfile.make_required_input(profile, model.vars.crc_poly, 'crc', readable_name="CRC Polynomial")

    def build_optional_profile_inputs(self, model, profile):
        IProfile.make_optional_input(profile, model.vars.payload_white_en, 'frame_payload',
                                     readable_name="Payload Whitening Enable", default=True)
        IProfile.make_optional_input(profile, model.vars.max_tx_power_dbm, 'modem', default=17,
                                     readable_name="Max TX power in dBm", value_limit_min=-100, value_limit_max=100)
        IProfile.make_optional_input(profile, model.vars.shaping_filter, "modem", readable_name="Shaping Filter",
                                     default=model.vars.shaping_filter.var_enum.Gaussian)
        IProfile.make_optional_input(profile, model.vars.shaping_filter_param, "modem",
                                     readable_name="Shaping Filter Parameter (BT or R)", value_limit_min=0.3,
                                     value_limit_max=2.0, fractional_digits=2, default=2.0)
        IProfile.make_optional_input(profile, model.vars.fec_tx_enable, 'Channel_Coding',
                                     readable_name="Enable FEC on TX", default=model.vars.fec_tx_enable.var_enum.DISABLED)

    def build_advanced_profile_inputs(self, model, profile):
        #No advanced inputs for this profile
        pass

    def build_hidden_profile_inputs(self, model, profile):

        # Hidden inputs to allow for fixed frame length testing
        IProfile.make_hidden_input(profile, model.vars.frame_length_type, 'frame_general',
                                   readable_name="Frame Length Algorithm")
        IProfile.make_hidden_input(profile, model.vars.fixed_length_size, category='frame_fixed_length',
                                   readable_name="Fixed Payload Size", value_limit_min=0, value_limit_max=0x7fffffff)

        #Hidden inputs to allow for keeping absolute tolerance the same when testing at 915M
        IProfile.make_hidden_input(profile, model.vars.rx_xtal_error_ppm, category="general",
                                   readable_name="Set desired xtal tolerance on RX side", value_limit_min=0,
                                   value_limit_max=100)
        IProfile.make_hidden_input(profile, model.vars.tx_xtal_error_ppm, category="general",
                                   readable_name="Set desired xtal tolerance on TX side", value_limit_min=0,
                                   value_limit_max=100)
        IProfile.make_hidden_input(profile, model.vars.freq_offset_hz, 'Advanced',
                                   readable_name="Frequency Offset Compensation (AFC) Limit", value_limit_min=0,
                                   value_limit_max=500000, units_multiplier=UnitsMultiplier.KILO)

    def build_register_profile_outputs(self, model, profile):
        family = self._family
        build_modem_regs_ocelot(model, profile, family)
        buildFrameOutputs(model, profile, family)
        buildCrcOutputs(model, profile, family)
        buildWhiteOutputs(model, profile)
        buildFecOutputs(model, profile)

    def build_variable_profile_outputs(self, model, profile):
        buildRailOutputs(model, profile)
        build_ircal_sw_vars(model, profile)
        profile.outputs.append(ModelOutput(model.vars.ircal_power_level, '', ModelOutputType.RAIL_CONFIG,
                                           readable_name='IR cal power level (amplitude)'))

    def build_info_profile_outputs(self, model, profile):
        buildModemInfoOutputs(model, profile)

    def profile_calculate(self, model):
        self._fixed_wisun_vars(model)
        self._lookup_from_wisun_mode(model)
        self._lookup_from_crc_type(model)

    def _fixed_wisun_vars(self, model):
        # Calculate all of the fixed values (forces) unique to this profile

        #All spec references are to IEEE Std 802.15.4-2015 (shortened to 802154)

        # Variable Length and Endianness Inputs
        model.vars.frame_bitendian.value_forced = model.vars.frame_bitendian.var_enum.LSB_FIRST # 802154 17.2.2 (Reference Modulator Diagram)
        model.vars.frame_length_type.value_forced = model.vars.frame_length_type.var_enum.VARIABLE_LENGTH # 802154 20.2.2 (PHR Field Format)
        model.vars.var_length_bitendian.value_forced = model.vars.var_length_bitendian.var_enum.MSB_FIRST # 802154 20.2.2 (PHR Field Format)
        model.vars.var_length_byteendian.value_forced = model.vars.var_length_byteendian.var_enum.MSB_FIRST #Consistent with var_length_bitendian above
        model.vars.var_length_numbits.value_forced = 11 #Frame Length contained in bits 5-15 (11 total bits) of PHR
        model.vars.var_length_shift.value_forced = 0 #Since the length is stored MSB_FIRST, we don't need to shift as the MSB is in bit 15
        model.vars.var_length_maxlength.value_forced = 2047 #11-bit field for Frame Length
        model.vars.var_length_minlength.value_forced = 0
        model.vars.var_length_includecrc.value_forced = True #Length is the total number of octets contained in the PSDU
        model.vars.var_length_adjust.value_forced = 0

        # Header Inputs
        model.vars.header_calc_crc.value_forced = False #No CRC on the header itself
        model.vars.header_en.value_forced = True # 802154 20.2.2 (PHR Field Format)
        model.vars.header_size.value_forced = 2 #Header size is 16 bits
        model.vars.header_white_en.value_forced = False #When whitening is enabled it shall be applied only over the PSDU

        # FEC settings (note that there is a separate variable fec_tx_enable that determines if FEC is on/off in TX)
        model.vars.fec_en.value_forced = model.vars.fec_en.var_enum.FEC_154G_NRNSC_INTERLEAVING
        model.vars.dynamic_fec_enable.value_forced = model.vars.dynamic_fec_enable.var_enum.ENABLED

        # Whitening Inputs set consistent with 802154 17.2.3
        model.vars.white_poly.value_forced = model.vars.white_poly.var_enum.PN9_802154
        model.vars.white_seed.value_forced = 0xFF #This has been tested, need a 0 in position 8 due to EFR32 whitening architecture
        model.vars.white_output_bit.value_forced = 8

        # CRC Inputs set to match 802.15.4 settings in PHY_COMMON_FRAME_154
        model.vars.payload_crc_en.value_forced = True
        model.vars.crc_bit_endian.value_forced = model.vars.crc_bit_endian.var_enum.MSB_FIRST
        model.vars.crc_byte_endian.value_forced = model.vars.crc_byte_endian.var_enum.MSB_FIRST
        model.vars.crc_input_order.value_forced = model.vars.crc_input_order.var_enum.LSB_FIRST

        #Modulation type
        model.vars.modulation_type.value_forced = model.vars.modulation_type.var_enum.FSK2

        # Symbol mapping and encoding
        model.vars.diff_encoding_mode.value_forced = model.vars.diff_encoding_mode.var_enum.DISABLED
        model.vars.dsss_chipping_code.value_forced = 0
        model.vars.dsss_len.value_forced = 0
        model.vars.dsss_spreading_factor.value_forced = 0
        model.vars.fsk_symbol_map.value_forced = model.vars.fsk_symbol_map.var_enum.MAP0  # 802154 Table 20.8 and 20.9

        # Preamble and syncword definition
        model.vars.preamble_pattern.value_forced = 1 # 802154 20.2.1.1 (Preamble field)
        model.vars.preamble_pattern_len.value_forced = 2
        model.vars.syncword_0.value_forced = self.phySunFskSfd_0_uncoded
        model.vars.syncword_1.value_forced = self.phySunFskSfd_0_coded
        model.vars.syncword_length.value_forced = 16

        #Tolerance values
        model.vars.baudrate_tol_ppm.value_forced = 300
        model.vars.deviation_tol_ppm.value_forced = 300000
        model.vars.rx_xtal_error_ppm.value_forced = 20  # FAN EU Mode #1b, WiSUN 20140727-PHY-Profile Table 7 (assume RX=TX value)
        model.vars.tx_xtal_error_ppm.value_forced = 20  # FAN EU Mode #1b, WiSUN 20140727-PHY-Profile Table 7 (assume RX=TX value)

        # Variables that are optional inputs in the Base profile and need to be explicitly set
        model.vars.test_ber.value_forced = False
        model.vars.asynchronous_rx_enable.value_forced = False
        model.vars.manchester_mapping.value_forced = model.vars.manchester_mapping.var_enum.Default
        model.vars.syncword_tx_skip.value_forced = False
        model.vars.frame_coding.value_forced = model.vars.frame_coding.var_enum.NONE
        model.vars.fixed_length_size.value_forced = 1
        model.vars.frame_type_0_filter.value_forced = True
        model.vars.frame_type_0_length.value_forced = 0
        model.vars.frame_type_0_valid.value_forced = False
        model.vars.frame_type_1_filter.value_forced = True
        model.vars.frame_type_1_length.value_forced = 0
        model.vars.frame_type_1_valid.value_forced = False
        model.vars.frame_type_2_filter.value_forced = True
        model.vars.frame_type_2_length.value_forced = 0
        model.vars.frame_type_2_valid.value_forced = False
        model.vars.frame_type_3_filter.value_forced = True
        model.vars.frame_type_3_length.value_forced = 0
        model.vars.frame_type_3_valid.value_forced = False
        model.vars.frame_type_4_filter.value_forced = True
        model.vars.frame_type_4_length.value_forced = 0
        model.vars.frame_type_4_valid.value_forced = False
        model.vars.frame_type_5_filter.value_forced = True
        model.vars.frame_type_5_length.value_forced = 0
        model.vars.frame_type_5_valid.value_forced = False
        model.vars.frame_type_6_filter.value_forced = True
        model.vars.frame_type_6_length.value_forced = 0
        model.vars.frame_type_6_valid.value_forced = False
        model.vars.frame_type_7_filter.value_forced = True
        model.vars.frame_type_7_length.value_forced = 0
        model.vars.frame_type_7_valid.value_forced = False
        model.vars.frame_type_bits.value_forced = 3
        model.vars.frame_type_loc.value_forced = 0
        model.vars.frame_type_lsbit.value_forced = 0

    def _lookup_from_wisun_mode(self, model):
        # This function calculates some variables/registers based on the wisun_mode

        # Read the mode and fec_en from the profile inputs (not yet written to model vars)
        wisun_mode = model.profile.inputs.wisun_mode.var_value
        fec_tx_enable = (model.profile.inputs.fec_tx_enable.var_value == model.vars.fec_tx_enable.var_enum.ENABLED)

        # We need to modify the net bitrate (data rate) based on whether FEC is enabled or not
        fec_factor = 2 if fec_tx_enable else 1

        #Set basic parameters based on the mode
        if wisun_mode == model.vars.wisun_mode.var_enum.Mode1a:
            model.vars.bitrate.value_forced = 50000//fec_factor
            model.vars.deviation.value_forced = 12500

        elif wisun_mode == model.vars.wisun_mode.var_enum.Mode1b:
            model.vars.bitrate.value_forced = 50000//fec_factor
            model.vars.deviation.value_forced = 25000

        elif wisun_mode == model.vars.wisun_mode.var_enum.Mode2a:
            model.vars.bitrate.value_forced = 100000//fec_factor
            model.vars.deviation.value_forced = 25000

        elif wisun_mode == model.vars.wisun_mode.var_enum.Mode2b:
            model.vars.bitrate.value_forced = 100000//fec_factor
            model.vars.deviation.value_forced = 50000

        elif wisun_mode == model.vars.wisun_mode.var_enum.Mode3:
            model.vars.bitrate.value_forced = 150000//fec_factor
            model.vars.deviation.value_forced = 37500

        elif wisun_mode == model.vars.wisun_mode.var_enum.Mode4a:
            model.vars.bitrate.value_forced = 200000//fec_factor
            model.vars.deviation.value_forced = 50000

        elif wisun_mode == model.vars.wisun_mode.var_enum.Mode4b:
            model.vars.bitrate.value_forced = 200000//fec_factor
            model.vars.deviation.value_forced = 100000

        elif wisun_mode == model.vars.wisun_mode.var_enum.Mode5:
            model.vars.bitrate.value_forced = 300000//fec_factor
            model.vars.deviation.value_forced = 75000

    def _lookup_from_crc_type(self, model):
        #This function sets CRC parameters based on the CRC specified

        # Read the CRC type
        crc_poly = model.profile.inputs.crc_poly.var_value

        #Based on the CRC type, set the CRC seed, invert, and padding settings
        if crc_poly == model.vars.crc_poly.var_enum.ANSIX366_1979:
            crc_invert = True
            crc_pad_input = True
            crc_seed = 0xFFFFFFFF
        else:
            crc_invert = False
            crc_pad_input = False
            crc_seed = 0x00000000

        #Write the variables
        model.vars.crc_invert.value_forced = crc_invert
        model.vars.crc_pad_input.value_forced = crc_pad_input
        model.vars.crc_seed.value_forced = crc_seed

