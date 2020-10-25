from pyradioconfig.parts.ocelot.profiles.Profile_WiSUN import Profile_WiSUN_Ocelot

class Profile_WiSUN_Sol(Profile_WiSUN_Ocelot):

    def __init__(self):
        self._profileName = "WiSUN"
        self._readable_name = "WiSUN Profile"
        self._category = ""
        self._description = "Profile used for WiSUN PHYs"
        self._default = False
        self._activation_logic = ""
        self._family = "sol"

    def _fixed_wisun_vars(self, model):

        #Use the fixed wisun vars from Ocelot
        super()._fixed_wisun_vars(model)

        #Add the ofdm_option variable
        model.vars.ofdm_option.value_forced = 0
