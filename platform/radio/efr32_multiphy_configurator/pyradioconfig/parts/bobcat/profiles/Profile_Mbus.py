from pyradioconfig.parts.common.profiles.bobcat_regs import *
from pyradioconfig.parts.common.profiles.profile_common import *
from pyradioconfig.parts.ocelot.profiles.profile_modem import *

from pyradioconfig.calculator_model_framework.interfaces.iprofile import IProfile
from pyradioconfig.parts.common.utils.units_multiplier import UnitsMultiplier

from pyradioconfig.parts.ocelot.profiles.profile_mbus_modes import *

from pyradioconfig.parts.ocelot.profiles.Profile_Mbus import Profile_Mbus_Ocelot
from py_2_and_3_compatibility import *

class Profile_Mbus_Bobcat(Profile_Mbus_Ocelot):

    """
    Init internal variables
    """
    def __init__(self):
        self._family = "bobcat"
        self._profileName = "Mbus"
        self._readable_name = "Mbus Profile"
        self._category = ""
        self._description = "Profile used for Mbus phys"
        self._default = False
        self._activation_logic = ""


    """
    Builds inputs, forced, outputs into modem model
    """
    def buildProfileModel(self, model):

        family = self._family

        # Build profile
        profile = self._makeProfile(model)

        IProfile.make_required_input(profile, model.vars.mbus_mode,            category="general", readable_name="Mbus Mode")
        IProfile.make_required_input(profile, model.vars.mbus_frame_format,    category="general", readable_name="Mbus Frame Format")
        IProfile.make_required_input(profile, model.vars.mbus_symbol_encoding, category="general", readable_name="Symbol Encoding")
        IProfile.make_required_input(profile, model.vars.syncword_dualsync,    category="general", readable_name="Enable Dual Syncword Detection")

        IProfile.make_required_input(profile, model.vars.base_frequency_hz,    category="operational_frequency", readable_name="Base Channel Frequency",  value_limit_min=long(100000000), value_limit_max=long(2480000000), units_multiplier=UnitsMultiplier.MEGA)
        IProfile.make_required_input(profile, model.vars.channel_spacing_hz,   category="operational_frequency", readable_name="Channel Spacing",         value_limit_min=0,          value_limit_max=10000000,    units_multiplier=UnitsMultiplier.KILO)
        IProfile.make_required_input(profile, model.vars.xtal_frequency_hz, "crystal",
                                     readable_name="Crystal Frequency", value_limit_min=38000000,
                                     value_limit_max=40000000, units_multiplier=UnitsMultiplier.MEGA)

        # Hidden test inputs
        IProfile.make_hidden_input(profile, model.vars.fixed_length_size, category='frame_fixed_length', readable_name="Fixed Payload Size", value_limit_min=0, value_limit_max=0x7fffffff)
        IProfile.make_hidden_input(profile, model.vars.payload_crc_en,    category='frame_payload',      readable_name="Insert/Check CRC after payload")

        IProfile.make_hidden_input(profile, model.vars.crc_poly,        category='crc', readable_name="CRC Polynomial")
        #IProfile.make_hidden_input(profile, model.vars.crc_seed,        category='crc', readable_name="CRC Seed")
        IProfile.make_hidden_input(profile, model.vars.crc_byte_endian, category='crc', readable_name="CRC Byte Endian")
        IProfile.make_hidden_input(profile, model.vars.crc_bit_endian,  category='crc', readable_name="CRC Output Bit Endian")
        IProfile.make_hidden_input(profile, model.vars.crc_pad_input,   category='crc', readable_name="CRC Input Padding")
        IProfile.make_hidden_input(profile, model.vars.crc_input_order, category='crc', readable_name="CRC Input Bit Endian")
        IProfile.make_hidden_input(profile, model.vars.crc_invert,      category='crc', readable_name="CRC Invert")

        IProfile.make_optional_input(profile, model.vars.test_ber,      category="testing", readable_name="Reconfigure for BER testing", default=False)
        IProfile.make_hidden_input(profile, model.vars.target_osr,      category="general", readable_name="Set desired OSR", value_limit_min=3, value_limit_max=9)
        IProfile.make_hidden_input(profile, model.vars.bitrate,       category="modem",   readable_name="Bitrate", value_limit_min=100, value_limit_max=2000000,units_multiplier=UnitsMultiplier.KILO)

        #These can not be hidden because they are used in Studio PHYs. These can not be optional inputs because of the way the variables are forced in the Mbus code.
        IProfile.make_linked_io(profile, model.vars.rx_xtal_error_ppm, category="general", readable_name="Set desired xtal tolerance on RX side", value_limit_min=0, value_limit_max=100)
        IProfile.make_linked_io(profile, model.vars.tx_xtal_error_ppm, category="general", readable_name="Set desired xtal tolerance on TX side", value_limit_min=0, value_limit_max=100)

        IProfile.make_hidden_input(profile, model.vars.demod_select, 'Advanced', readable_name="Demod Selection")

        # TODO:  Change this to MSB_FIRst in the lab phys, then remove this input and move it to the forced section below
        IProfile.make_hidden_input(profile, model.vars.frame_bitendian, category='frame_general', readable_name="Frame Bit Endian")

        IProfile.make_hidden_input(profile, model.vars.synth_settling_mode, 'modem', readable_name="Synth Settling Mode")
        IProfile.make_optional_input(profile, model.vars.max_tx_power_dbm, 'modem', default=17, readable_name="Max TX power in dBm", value_limit_min=-100, value_limit_max=100)
        IProfile.make_optional_input(profile, model.vars.deviation_tol_ppm, 'modem', default=0, readable_name="Maximum deviation offset expected in ppm",value_limit_min=0, value_limit_max=500000)


        # Informational output
        buildModemInfoOutputs(model, profile)

        # RAIL Outputs
        buildRailOutputs(model, profile)

        # Profile does not include advanced inputs, but we do need an output for IR cal power level for rail_scripts to consume
        # So add that here. It will default to 0 (which triggers to change to IR cal).
        profile.outputs.append(ModelOutput(model.vars.ircal_power_level, '', ModelOutputType.RAIL_CONFIG, readable_name='IR cal power level (amplitude)'))

        # Output fields
        buildFrameOutputs(model, profile, family=family)
        buildCrcOutputs(model, profile, family)
        buildWhiteOutputs(model, profile)
        buildFecOutputs(model, profile)

        build_modem_regs_bobcat(model, profile, family = family)

        build_ircal_sw_vars(model, profile)

        return profile
