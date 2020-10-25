from pyradioconfig.parts.ocelot.phys.Phys_Studio_WiSUN import PHYS_IEEE802154_WiSUN_Ocelot

class PHYS_IEEE802154_WiSUN_Sol(PHYS_IEEE802154_WiSUN_Ocelot):

    def PHY_IEEE802154_WISUN_915MHz_2GFSK_200kbps_4a_NA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN, readable_name='WISUN 915MHz 2GFSK 200kbps NA-4a mi=0.5',
                            phy_name=phy_name)

        # Select the correct SUNFSK mode
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode4a

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 902400000  # FAN NA Mode #4, WiSUN 20150727-PHY-Profile Table 3
        phy.profile_inputs.channel_spacing_hz.value = 400000  # FAN NA Mode #3, WiSUN 20150727-PHY-Profile Table 3
        phy.profile_inputs.preamble_length.value = 12 * 8  # FAN NA Mode #4a, not specified in Table 6
        phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.ANSIX366_1979  # 802.15.4-2015, 7.2.10

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    def PHY_IEEE802154_WISUN_920MHz_2GFSK_200kbps_4b_JP(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN, readable_name='WISUN 920MHz 2GFSK 200kbps JP-4b mi=1.0',
                            phy_name=phy_name)

        # Select the correct SUNFSK mode
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode4b

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 920800000  # Echonet JP Mode #4b, WiSUN 20150727-PHY-Profile Table 3
        phy.profile_inputs.channel_spacing_hz.value = 600000  # Echonet JP Mode #4b, WiSUN 20150727-PHY-Profile Table 3
        phy.profile_inputs.preamble_length.value = 15 * 8  # Echonet JP Mode #4b, not specified in Table 5 (used 2b)
        phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.CCITT_16  # 802.15.4-2015, 7.2.10

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    def PHY_IEEE802154_WISUN_915MHz_2GFSK_300kbps_5_NA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN, readable_name='WISUN 915MHz 2GFSK 300kbps NA-5 mi=0.5',
                            phy_name=phy_name)

        # Select the correct SUNFSK mode
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode5

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 902600000  # FAN NA Mode #5, WiSUN 20150727-PHY-Profile Table 3
        phy.profile_inputs.channel_spacing_hz.value = 600000  # FAN NA Mode #5, WiSUN 20150727-PHY-Profile Table 3
        phy.profile_inputs.preamble_length.value = 24 * 8  # FAN NA Mode #5, WiSUN 20150727-PHY-Profile Table 6
        phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.ANSIX366_1979  # 802.15.4-2015, 7.2.10

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy