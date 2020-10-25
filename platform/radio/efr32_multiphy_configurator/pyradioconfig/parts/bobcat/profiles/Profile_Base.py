from pyradioconfig.parts.ocelot.profiles.Profile_Base import Profile_Base_Ocelot
from pyradioconfig.parts.common.profiles.bobcat_regs import build_modem_regs_bobcat

class Profile_Base_Bobcat(Profile_Base_Ocelot):

    def __init__(self):
        super().__init__()
        self._description = "Profile used for most PHYs"
        self._family = "bobcat"

    def buildRegisterOutputs(self, model, profile, family):
        build_modem_regs_bobcat(model, profile, family)