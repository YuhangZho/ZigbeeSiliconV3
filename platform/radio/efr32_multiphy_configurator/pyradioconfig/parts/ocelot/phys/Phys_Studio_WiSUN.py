from pyradioconfig.calculator_model_framework.interfaces.iphy import IPhy


class PHYS_IEEE802154_WiSUN_Ocelot(IPhy):

    ### EU Region ###

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-166
    def PHY_IEEE802154_WISUN_868MHz_2GFSK_50kbps_1a_EU(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN, readable_name='WISUN 868MHz 2FSK 50kbps EU-1a mi=0.5', phy_name=phy_name)

        #Select the correct SUNFSK mode
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode1a

        #Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 863100000 # FAN EU Mode #1a, WiSUN 20140727-PHY-Profile Table 3
        phy.profile_inputs.channel_spacing_hz.value = 100000 # FAN EU Mode #1a, WiSUN 20140727-PHY-Profile Table 3
        phy.profile_inputs.preamble_length.value = 8 * 8  # FAN EU Mode #1a, WiSUN 20140727-PHY-Profile Table 6
        phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.ANSIX366_1979  # 802.15.4-2015, 7.2.10

        #Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-165
    def PHY_IEEE802154_WISUN_868MHz_2GFSK_100kbps_2a_EU(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN, readable_name='WISUN 868MHz 2FSK 100kbps EU-2a mi=0.5', phy_name=phy_name)

        # Select the correct SUNFSK mode
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode2a

        #Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 863100000 # FAN EU Mode #2a, WiSUN 20140727-PHY-Profile Table 3
        phy.profile_inputs.channel_spacing_hz.value = 200000 # FAN EU Mode #2a, WiSUN 20140727-PHY-Profile Table 3
        phy.profile_inputs.preamble_length.value = 8 * 8  # FAN EU Mode #2a, WiSUN 20140727-PHY-Profile Table 6
        phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.ANSIX366_1979  # 802.15.4-2015, 7.2.10

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy


    ### NA Region ###

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-168
    def PHY_IEEE802154_WISUN_915MHz_2GFSK_50kbps_1b_NA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN, readable_name='WISUN 915MHz 2FSK 50kbps NA-1b mi=1.0', phy_name=phy_name)

        # Select the correct SUNFSK mode
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode1b

        #Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 902200000 # FAN NA Mode #1b, WiSUN 20140727-PHY-Profile Table 3
        phy.profile_inputs.channel_spacing_hz.value = 200000 # FAN NA Mode #1b, WiSUN 20140727-PHY-Profile Table 3
        phy.profile_inputs.preamble_length.value = 8 * 8  # FAN NA Mode #1b, WiSUN 20140727-PHY-Profile Table 6
        phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.ANSIX366_1979  # 802.15.4-2015, 7.2.10

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-167
    def PHY_IEEE802154_WISUN_915MHz_2GFSK_150kbps_3_NA(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN, readable_name='WISUN 915MHz 2FSK 150kbps NA-3 mi=0.5', phy_name=phy_name)

        # Select the correct SUNFSK mode
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode3

        #Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 902400000 # FAN NA Mode #3, WiSUN 20140727-PHY-Profile Table 3
        phy.profile_inputs.channel_spacing_hz.value = 400000 # FAN NA Mode #3, WiSUN 20140727-PHY-Profile Table 3
        phy.profile_inputs.preamble_length.value = 12 * 8 # FAN NA Mode #3, WiSUN 20140727-PHY-Profile Table 6
        phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.ANSIX366_1979  # 802.15.4-2015, 7.2.10

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    ### JP Region ###

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-170
    def PHY_IEEE802154_WISUN_920MHz_2GFSK_50kbps_1b_JP(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN, readable_name='WISUN 920MHz 2FSK 50kbps JP-1b mi=1.0', phy_name=phy_name)

        # Select the correct SUNFSK mode
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode1b

        #Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 920600000 # Echonet JP Mode #1b, WiSUN 20140727-PHY-Profile Table 3
        phy.profile_inputs.channel_spacing_hz.value = 200000 # Echonet JP Mode #1b, WiSUN 20140727-PHY-Profile Table 3
        phy.profile_inputs.preamble_length.value = 8 * 8 # Echonet JP Mode #1b, WiSUN 20140727-PHY-Profile Table 5
        phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.CCITT_16 # 802.15.4-2015, 7.2.10

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-169
    def PHY_IEEE802154_WISUN_920MHz_2GFSK_100kbps_2b_JP(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN, readable_name='WISUN 920MHz 2FSK 100kbps JP-2b mi=1.0', phy_name=phy_name)

        # Select the correct SUNFSK mode
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode2b

        #Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 920900000 # Echonet JP Mode #2b, WiSUN 20140727-PHY-Profile Table 3
        phy.profile_inputs.channel_spacing_hz.value = 400000 # Echonet JP Mode #2b, WiSUN 20140727-PHY-Profile Table 3
        phy.profile_inputs.preamble_length.value = 15 * 8 # Echonet JP Mode #2b, WiSUN 20140727-PHY-Profile Table 5
        phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.CCITT_16 # 802.15.4-2015, 7.2.10

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    ### CN Region ###

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-1218
    def PHY_IEEE802154_WISUN_470MHz_2GFSK_50kbps_1b_CN(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN, readable_name='WISUN 470MHz 2FSK 50kbps CN-1b mi=1.0', phy_name=phy_name)

        # Select the correct SUNFSK mode
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode1b

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 470200000  # FAN CN Mode #1b, WiSUN 20140727-PHY-Profile Table 3
        phy.profile_inputs.channel_spacing_hz.value = 200000  # FAN CN Mode #1b, WiSUN 20140727-PHY-Profile Table 3
        phy.profile_inputs.preamble_length.value = 8 * 8  # FAN CN Mode #1b, WiSUN 20140727-PHY-Profile Table 5
        phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.ANSIX366_1979  # 802.15.4-2015, 7.2.10

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy

    # Owner: Casey Weltzin
    # JIRA Link: https://jira.silabs.com/browse/PGOCELOTVALTEST-1219
    def PHY_IEEE802154_WISUN_470MHz_2GFSK_100kbps_2a_CN(self, model, phy_name=None):
        phy = self._makePhy(model, model.profiles.WiSUN, readable_name='WISUN 470MHz 2FSK 100kbps CN-2a mi=0.5', phy_name=phy_name)

        # Select the correct SUNFSK mode
        phy.profile_inputs.wisun_mode.value = model.vars.wisun_mode.var_enum.Mode2a

        # Define WiSUN Profile / Region specific inputs
        phy.profile_inputs.base_frequency_hz.value = 470200000  # FAN CN Mode #2a, WiSUN 20140727-PHY-Profile Table 3
        phy.profile_inputs.channel_spacing_hz.value = 200000  # FAN CN Mode #2a, WiSUN 20140727-PHY-Profile Table 3
        phy.profile_inputs.preamble_length.value = 8 * 8  # FAN CN Mode #2a, WiSUN 20140727-PHY-Profile Table 5
        phy.profile_inputs.crc_poly.value = model.vars.crc_poly.var_enum.ANSIX366_1979  # 802.15.4-2015, 7.2.10

        # Default xtal frequency of 39MHz
        phy.profile_inputs.xtal_frequency_hz.value = 39000000

        return phy