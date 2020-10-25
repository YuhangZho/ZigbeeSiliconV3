from pyradioconfig.parts.common.profiles.Profile_Sigfox_TX import Profile_Sigfox_TX, IProfile
from pyradioconfig.parts.ocelot.profiles.profile_modem import *

class Profile_Sigfox_TX_Ocelot(Profile_Sigfox_TX):

    def __init__(self):
        super(self.__class__, self).__init__()
        self._family = "ocelot"

    def buildProfileModel(self, model):
        # Start with the inherited profile
        profile = super(Profile_Sigfox_TX_Ocelot, self).buildProfileModel(model)

        # Add Ocelot-specific variables
        IProfile.make_hidden_input(profile, model.vars.demod_select, 'Advanced', readable_name="Demod Selection")
        IProfile.make_hidden_input(profile, model.vars.synth_settling_mode, 'modem', readable_name="Synth Settling Mode")
        IProfile.make_optional_input(profile, model.vars.max_tx_power_dbm, 'modem', default=17, readable_name="Max TX power in dBm", value_limit_min=-100, value_limit_max=100)

        return profile


















