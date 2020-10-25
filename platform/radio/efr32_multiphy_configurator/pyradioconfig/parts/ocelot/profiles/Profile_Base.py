from pyradioconfig.calculator_model_framework.interfaces.iprofile import IProfile
from pyradioconfig.parts.common.profiles.Profile_Base import Profile_Base
from pyradioconfig.parts.common.utils.units_multiplier import UnitsMultiplier
from pyradioconfig.parts.common.profiles.dumbo_regs import *
from pyradioconfig.parts.common.profiles.jumbo_regs import *
from pyradioconfig.parts.common.profiles.nixi_regs import *
from pyradioconfig.parts.common.profiles.panther_regs import *
from pyradioconfig.parts.common.profiles.lynx_regs import *
from pyradioconfig.parts.common.profiles.ocelot_regs import *
from pyradioconfig.parts.common.profiles.profile_common import *
from pyradioconfig.parts.ocelot.profiles.profile_modem import *

class Profile_Base_Ocelot(Profile_Base):

    """
    Init internal variables
    """
    def __init__(self):
        super().__init__()
        self._description = "Profile used for most PHYs on EFR32xG23 parts"
        self._family = "ocelot"

    # def buildProfileModel(self, model):
    #     # All profile I/O for Ocelot should be defined here (no splitting up between files)
    #     # If this needs to be inherited, split up into multiple sub-methods and then override those as needed (e.g. to remove profile I/O or remap)
    #     # Do not add if/then statements based on part family
    #
    #     # TODO: Clean this up so that all of the profile I/O is bubbled up to the top level
    #
    #     return super(Profile_Base_Ocelot, self).buildProfileModel(model)

    def buildProfileModel(self, model):

        family = self._family

        # Build profile
        profile = self._makeProfile(model)

        IProfile.make_required_input(profile, model.vars.base_frequency_hz,      "operational_frequency", readable_name="Base Channel Frequency",     value_limit_min=long(100000000), value_limit_max=long(2480000000), units_multiplier=UnitsMultiplier.MEGA)
        IProfile.make_required_input(profile, model.vars.channel_spacing_hz,     "operational_frequency", readable_name="Channel Spacing",            value_limit_min=0,         value_limit_max=10000000, units_multiplier=UnitsMultiplier.KILO)

        IProfile.make_required_input(profile, model.vars.xtal_frequency_hz,      "crystal",  readable_name="Crystal Frequency",          value_limit_min=38000000,  value_limit_max=40000000, units_multiplier=UnitsMultiplier.MEGA)
        IProfile.make_required_input(profile, model.vars.rx_xtal_error_ppm,      "crystal",  readable_name="RX Crystal Accuracy",        value_limit_min=0,         value_limit_max=200)
        IProfile.make_required_input(profile, model.vars.tx_xtal_error_ppm,      "crystal",  readable_name="TX Crystal Accuracy",        value_limit_min=0,         value_limit_max=200)

        IProfile.make_required_input(profile, model.vars.syncword_0,             "syncword", readable_name="Sync Word 0",                value_limit_min=long(0),        value_limit_max=long(0xffffffff))
        IProfile.make_required_input(profile, model.vars.syncword_1,             "syncword", readable_name="Sync Word 1",                value_limit_min=long(0),        value_limit_max=long(0xffffffff))
        IProfile.make_optional_input(profile, model.vars.syncword_tx_skip,       "syncword", readable_name="Sync Word TX Skip",          default=False)
        IProfile.make_required_input(profile, model.vars.syncword_length,        "syncword", readable_name="Sync Word Length",           value_limit_min=0,         value_limit_max=32)

        IProfile.make_required_input(profile, model.vars.preamble_pattern_len,   "preamble", readable_name="Preamble Pattern Length",    value_limit_min=0,         value_limit_max=4)
        IProfile.make_required_input(profile, model.vars.preamble_length,        "preamble", readable_name="Preamble Length Total",      value_limit_min=0,         value_limit_max=2097151)
        IProfile.make_required_input(profile, model.vars.preamble_pattern,       "preamble", readable_name="Preamble Base Pattern",      value_limit_min=0,         value_limit_max=15)

        IProfile.make_required_input(profile, model.vars.modulation_type,        "modem",    readable_name="Modulation Type"             )
        IProfile.make_required_input(profile, model.vars.deviation,              "modem",    readable_name="Deviation",                  value_limit_min=0,         value_limit_max=1000000, units_multiplier=UnitsMultiplier.KILO)
        IProfile.make_required_input(profile, model.vars.bitrate,                "modem",    readable_name="Bitrate",                    value_limit_min=100,       value_limit_max=2000000, units_multiplier=UnitsMultiplier.KILO)
        IProfile.make_required_input(profile, model.vars.baudrate_tol_ppm,       "modem",    readable_name="Baudrate Tolerance",         value_limit_min=0,         value_limit_max=200000)
        IProfile.make_required_input(profile, model.vars.shaping_filter,         "modem",    readable_name="Shaping Filter"              )
        IProfile.make_required_input(profile, model.vars.fsk_symbol_map,         "modem",    readable_name="FSK symbol map"              )
        IProfile.make_required_input(profile, model.vars.shaping_filter_param,   "modem",    readable_name="Shaping Filter Parameter (BT or R)",   value_limit_min=0.3, value_limit_max=2.0, fractional_digits=2)
        IProfile.make_optional_input(profile, model.vars.asynchronous_rx_enable, "modem",    readable_name="Enable Asynchronous direct mode",    default=False )

        IProfile.make_required_input(profile, model.vars.diff_encoding_mode,     "symbol_coding",  readable_name="Differential Encoding Mode"  )
        IProfile.make_optional_input(profile, model.vars.symbol_encoding,        "symbol_coding",  readable_name="Symbol Encoding", default=model.vars.symbol_encoding.var_enum.NRZ)
        IProfile.make_optional_input(profile, model.vars.manchester_mapping,     "symbol_coding",  readable_name="Manchester Code Mapping", default=model.vars.manchester_mapping.var_enum.Default)
        IProfile.make_required_input(profile, model.vars.dsss_chipping_code,     "symbol_coding",  readable_name="DSSS Chipping Code Base",    value_limit_min=long(0),        value_limit_max=long(0xffffffff))
        IProfile.make_required_input(profile, model.vars.dsss_len,               "symbol_coding",  readable_name="DSSS Chipping Code Length",  value_limit_min=0,         value_limit_max=32)
        IProfile.make_required_input(profile, model.vars.dsss_spreading_factor,  "symbol_coding",  readable_name="DSSS Spreading Factor",      value_limit_min=0,         value_limit_max=100)

        IProfile.make_optional_input(profile, model.vars.test_ber,               "testing", readable_name="Reconfigure for BER testing",  default=False)

        # enable optional input for injection side
        IProfile.make_optional_input(profile, model.vars.lo_injection_side,      "operational_frequency", default=model.vars.lo_injection_side.var_enum.HIGH_SIDE, readable_name="Injection side")

        # 40nm Series 2 enable optional input for SRC range minimum
        IProfile.make_hidden_input(profile, model.vars.src1_range_available_minimum, "modem", readable_name="SRC range minimum",      value_limit_min=125,         value_limit_max=155)

        # Series 2: optional input to disallow DEC0 enums of DEC3/DEC8 (any width)
        # Not allowed to use bools per Attila, so int flag instead
        IProfile.make_hidden_input(profile, model.vars.input_decimation_filter_allow_dec3, "modem", readable_name="1=Allow input decimation filter decimate by 3 in cost function",      value_limit_min=0,         value_limit_max=1)
        IProfile.make_hidden_input(profile, model.vars.input_decimation_filter_allow_dec8, "modem", readable_name="1=Allow input decimation filter decimate by 8 in cost function",      value_limit_min=0,         value_limit_max=1)

        IProfile.make_hidden_input(profile, model.vars.demod_select, 'Advanced', readable_name="Demod Selection")
        IProfile.make_hidden_input(profile, model.vars.adc_clock_mode, "modem", readable_name="ADC Clock Mode (XO vs VCO)")
        IProfile.make_hidden_input(profile, model.vars.adc_rate_mode, 'Advanced', readable_name="ADC Rate Mode")

        # for concurrent demod PHYs we like to calculate BCR settings in addition to the chosen demod
        IProfile.make_hidden_input(profile, model.vars.bcr_demod_en, 'Advanced', readable_name="Force BCR demod calculation", value_limit_min=0, value_limit_max=1)

        IProfile.make_hidden_input(profile, model.vars.bcr_conc_baudrate,  "concurrent",readable_name="baudrate for BCR in concurrent mode", value_limit_min=0,value_limit_max=1000000)
        IProfile.make_hidden_input(profile, model.vars.bcr_conc_manchester, "concurrent",readable_name="Manchester code enable bit for BCR in concurrent mode",value_limit_min=0,value_limit_max=1)
        IProfile.make_hidden_input(profile, model.vars.bcr_conc_deviation, "concurrent",readable_name="deviation for BCR in concurrent mode",value_limit_min=0,value_limit_max=1000000)
        IProfile.make_hidden_input(profile, model.vars.synth_settling_mode, 'modem', readable_name="Synth Settling Mode")
        IProfile.make_optional_input(profile, model.vars.max_tx_power_dbm, 'modem', default=17, readable_name="Max TX power in dBm", value_limit_min=-100, value_limit_max=100)

        IProfile.make_optional_input(profile, model.vars.deviation_tol_ppm, 'modem', default=0,readable_name="Maximum deviation offset expected in ppm", value_limit_min=0,value_limit_max=500000)

        IProfile.make_linked_io(profile, model.vars.fec_tx_enable, 'Advanced', readable_name="Enable FEC on TX")
        IProfile.make_linked_io(profile, model.vars.dynamic_fec_enable, 'Advanced', readable_name="Enable dynamic FEC based on syncword")


        # Frame configuration Inputs
        buildFrameInputs(model, profile, family=family)
        buildCrcInputs(model, profile)
        buildWhiteInputs(model, profile)
        buildFecInputs(model, profile)

        # Modem Advanced Inputs
        buildModemAdvancedInputs(model, profile, family)

        # Informational Output
        buildModemInfoOutputs(model, profile)

        # RAIL Outputs
        buildRailOutputs(model, profile)

        # Output fields
        buildFrameOutputs(model, profile, family=family)
        buildCrcOutputs(model, profile, family)
        buildWhiteOutputs(model, profile)
        buildFecOutputs(model, profile)

        self.buildRegisterOutputs(model, profile, family)

        build_ircal_sw_vars(model, profile)

        return profile

    def buildRegisterOutputs(self, model, profile, family):
        build_modem_regs_ocelot(model, profile, family)
