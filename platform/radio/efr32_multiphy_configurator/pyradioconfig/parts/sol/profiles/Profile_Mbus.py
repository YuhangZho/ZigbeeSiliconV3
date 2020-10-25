from pyradioconfig.parts.ocelot.profiles.Profile_Mbus import Profile_Mbus_Ocelot
from pyradioconfig.calculator_model_framework.interfaces.iprofile import IProfile

class Profile_Mbus_Sol(Profile_Mbus_Ocelot):

    def __init__(self):
        super().__init__()
        self._family = "sol"

    def buildProfileModel(self, model):
        # Start with the inherited profile
        profile = super(Profile_Mbus_Sol, self).buildProfileModel(model)

        # Add Ocelot-specific variables

        IProfile.make_optional_input(profile, model.vars.ofdm_option, 'modem', default=1,
                                     readable_name="OPTION number for OFDM Modem", value_limit_min=1, value_limit_max=4)

        return profile
