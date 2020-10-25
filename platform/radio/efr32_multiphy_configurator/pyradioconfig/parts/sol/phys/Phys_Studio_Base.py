from pyradioconfig.parts.ocelot.phys.Phys_Studio_Base import PHYS_Studio_Base_Ocelot

class PHYS_Studio_Base_Sol(PHYS_Studio_Base_Ocelot):

    def PHY_Datasheet_169M_2GFSK_38p4Kbps_20K(self, model, phy_name=None):
        pass

    def PHY_Datasheet_285M_2GFSK_2p4Kbps_1p2K(self, model, phy_name=None):
        pass

    def PHY_Datasheet_285M_2GFSK_500Kbps_125K(self, model, phy_name=None):
        pass

    def PHY_Datasheet_315M_2GFSK_2p4Kbps_1p2K(self, model, phy_name=None):
        pass

    def PHY_Datasheet_315M_2GFSK_500Kbps_125K(self, model, phy_name=None):
        pass

    def PHY_Datasheet_434M_4GFSK_9p6Kbps_0p8K(self, model, phy_name=None):
        pass

    def PHY_Datasheet_490M_2GFSK_100Kbps_50K(self, model, phy_name=None):
        pass

    def PHY_Datasheet_490M_2GFSK_10Kbps_25K_20ppm(self, model, phy_name=None):
        pass

    def PHY_Datasheet_868M_2GFSK_2p4Kbps_1p2K(self, model, phy_name=None):
        pass

    def PHY_Datasheet_868M_2GFSK_500Kbps_125K(self, model, phy_name=None):
        pass

    def PHY_Datasheet_915M_2GFSK_100Kbps_50K_antdiv(self, model, phy_name=None):
        pass

    def PHY_Datasheet_915M_2GFSK_100Kbps_50K(self, model, phy_name=None):
        pass

    def PHY_Datasheet_915M_2GFSK_200Kbps_100K_antdiv(self, model, phy_name=None):
        pass

    #def PHY_Datasheet_915M_2GFSK_200Kbps_100K(self, model, phy_name=None):
        #pass

    #def PHY_Datasheet_915M_2GFSK_300bps_300(self, model, phy_name=None):
        #pass

    #def PHY_Datasheet_915M_2GFSK_50Kbps_25K(self, model, phy_name=None):
        #pass

    # Enabling per request from Tom Voor, ToDo: determine if really needed for Sol
    def PHY_Datasheet_915M_2GFSK_600bps_300(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.Base, readable_name='915M 2GFSK 600bps 300K', phy_name=phy_name)

        # Start with the base function
        self.Studio_2GFSK_base(phy, model)

        # Add data-rate specific parameters
        phy.profile_inputs.bitrate.value = 600
        phy.profile_inputs.deviation.value = 300
        phy.profile_inputs.rx_xtal_error_ppm.value = 0
        phy.profile_inputs.tx_xtal_error_ppm.value = 0

        # Add band-specific parameters
        phy.profile_inputs.base_frequency_hz.value = 915000000

        return phy

    #def PHY_Datasheet_915M_4GFSK_400Kbps_33p3K(self, model, phy_name=None):
        #pass

    def PHY_Studio_169M_2GFSK_2p4Kbps_1p2K(self, model, phy_name=None):
        pass

    def PHY_Datasheet_169M_2GFSK_2p4Kbps_1p2K_ETSI(self, model, phy_name=None):
        pass

    def PHY_Studio_315M_2GFSK_38p4Kbps_20K(self, model, phy_name=None):
        pass

    def PHY_Studio_315M_OOK_40kbps(self, model, phy_name=None):
        pass

    def PHY_Studio_315M_OOK_4p8kbps(self, model, phy_name=None):
        pass

    def PHY_Studio_433M_OOK_4p8kbps(self, model, phy_name=None):
        pass

    def PHY_Studio_434M_2GFSK_100Kbps_50K(self, model, phy_name=None):
        pass

    def PHY_Studio_434M_2GFSK_2p4Kbps_1p2K(self, model, phy_name=None):
        pass

    def PHY_Studio_434M_2GFSK_50Kbps_25K(self, model, phy_name=None):
        pass

    def PHY_Studio_434M_4GFSK_50Kbps_8p33K(self, model, phy_name=None):
        pass

    def PHY_Studio_490M_2GFSK_10Kbps_25K_20ppm(self, model, phy_name=None):
        pass

    def PHY_Studio_490M_2GFSK_10Kbps_5K(self, model, phy_name=None):
        pass

    def PHY_Studio_490M_2GFSK_2p4Kbps_1p2K(self, model, phy_name=None):
        pass

    def PHY_Studio_490M_2GFSK_38p4Kbps_20K(self, model, phy_name=None):
        pass

    def PHY_Datasheet_868M_2GFSK_2p4Kbps_1p2K_ETSI(self, model, phy_name=None):
        pass

    def PHY_Studio_868M_2GFSK_38p4Kbps_20K(self, model, phy_name=None):
        pass

    def PHY_Studio_868M_2GFSK_600bps_800(self, model, phy_name=None):
        pass

    def PHY_Studio_868M_GMSK_500Kbps(self, model, phy_name=None):
        pass

    def PHY_Studio_915M_2GFSK_2Mbps_500K(self, model, phy_name=None):
        pass

    def PHY_Studio_915M_2GFSK_500Kbps_175K_mi0p7(self, model, phy_name=None):
        pass

    def PHY_Studio_915M_4GFSK_200Kbps_16p6K(self, model, phy_name=None):
        pass

    def PHY_Studio_915M_OOK_120kbps(self, model, phy_name=None):
        pass

    def PHY_Studio_915M_OOK_4p8kbps(self, model, phy_name=None):
        pass

