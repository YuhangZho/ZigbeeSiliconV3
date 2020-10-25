from pyradioconfig.calculator_model_framework.interfaces.itarget import ITarget
from os.path import join

class Target_Sim_Sol(ITarget):

    _targetName = "Sim"
    _description = "Supports the wired FPGA and other targets of sim PHYs"
    _store_config_output = True
    _cfg_location = join('target_sim','sol')
    _tag = "SIM"

    def target_calculate(self, model):

        #Always use fixed length in sim results
        model.vars.frame_length_type.value_forced = model.vars.frame_length_type.var_enum.FIXED_LENGTH

        #PHY-specific rules (to be replaced globally once we figure out how calculate where they need to apply)
        if model.phy.name == 'PHY_Bluetooth_LE_1M_Viterbi_917M_noDSA':

            # template - put correct calls here
            self.FRC_DFLCTRL_DISABLE(model)
            self.FRC_FCD_DISABLE(model)
            self.MODEM_SHAPING_OVERRIDE(model)

        elif model.phy.name == 'PHY_Bluetooth_LE_2M_Viterbi_noDSA_fullrate':

            # template - put correct calls here
            self.FRC_DFLCTRL_DISABLE(model)
            self.FRC_FCD_DISABLE(model)
            self.MODEM_SHAPING_OVERRIDE(model)

        elif model.phy.name == 'PHY_Bluetooth_LE_2M_Viterbi':

            # template - put correct calls here
            self.FRC_DFLCTRL_DISABLE(model)
            self.FRC_FCD_DISABLE(model)
            self.MODEM_SHAPING_OVERRIDE(model)

        elif model.phy.name == 'PHY_Bluetooth_LE':

            # template - put correct calls here
            self.FRC_DFLCTRL_DISABLE(model)
            self.FRC_FCD_DISABLE(model)
            self.MODEM_SHAPING_OVERRIDE(model)

        elif model.phy.name == 'PHY_Bluetooth_LE_Viterbi_noDSA_fullrate':

            # template - put correct calls here
            self.FRC_DFLCTRL_DISABLE(model)
            self.FRC_FCD_DISABLE(model)
            self.MODEM_SHAPING_OVERRIDE(model)

        elif model.phy.name == 'PHY_Bluetooth_LE_Viterbi_noDSA':

            # template - put correct calls here
            self.FRC_DFLCTRL_DISABLE(model)
            self.FRC_FCD_DISABLE(model)
            self.MODEM_SHAPING_OVERRIDE(model)

        elif model.phy.name == 'PHY_Bluetooth_LE_Viterbi':

            # template - put correct calls here
            self.FRC_DFLCTRL_DISABLE(model)
            self.FRC_FCD_DISABLE(model)
            self.MODEM_SHAPING_OVERRIDE(model)

        elif model.phy.name == 'PHY_Bluetooth_LongRange_dsa_125kbps':

            # template - put correct calls here
            self.FRC_DFLCTRL_DISABLE(model)
            self.FRC_FCD_DISABLE(model)
            self.MODEM_SHAPING_OVERRIDE(model)
            self.FRC_LONGRANGE_OVERRIDE(model)

        elif model.phy.name == 'PHY_Bluetooth_LongRange_dsa_500kbps':

            # template - put correct calls here
            self.FRC_DFLCTRL_DISABLE(model)
            self.FRC_FCD_DISABLE(model)
            self.MODEM_SHAPING_OVERRIDE(model)
            self.FRC_LONGRANGE_OVERRIDE(model)

        elif model.phy.name == 'PHY_Bluetooth_LongRange_NOdsa_125kbps':

            # template - put correct calls here
            self.FRC_DFLCTRL_DISABLE(model)
            self.FRC_FCD_DISABLE(model)
            self.MODEM_SHAPING_OVERRIDE(model)
            self.FRC_LONGRANGE_OVERRIDE(model)

        elif model.phy.name == 'PHY_Bluetooth_LongRange_NOdsa_500kbps':

            # template - put correct calls here
            self.FRC_DFLCTRL_DISABLE(model)
            self.FRC_FCD_DISABLE(model)
            self.MODEM_SHAPING_OVERRIDE(model)
            self.FRC_LONGRANGE_OVERRIDE(model)

        elif model.phy.name == 'PHY_IEEE802154_2p4GHz_cohdsa_diversity':

            # template - put correct calls here
            self.FRC_DFLCTRL_DISABLE(model)
            self.FRC_FCD_DISABLE(model)
            # self.MODEM_SHAPING_OVERRIDE(model)

        elif model.phy.name == 'PHY_IEEE802154_2p4GHz_cohdsa':

            # template - put correct calls here
            self.FRC_DFLCTRL_DISABLE(model)
            self.FRC_FCD_DISABLE(model)
            # self.MODEM_SHAPING_OVERRIDE(model)

        elif model.phy.name == 'PHY_IEEE802154_2p4GHz_diversity':

            # template - put correct calls here
            self.FRC_DFLCTRL_DISABLE(model)
            self.FRC_FCD_DISABLE(model)
            # self.MODEM_SHAPING_OVERRIDE(model)

        elif model.phy.name == 'PHY_IEEE802154_2p4GHz':

            # template - put correct calls here
            self.FRC_DFLCTRL_DISABLE(model)
            self.FRC_FCD_DISABLE(model)
            # self.MODEM_SHAPING_OVERRIDE(model)

    ############################################################################

    def FRC_DFLCTRL_DISABLE(self, model):
        # Dynamic frame length is not currently supported in RTL simulations
        model.vars.FRC_DFLCTRL_DFLBITORDER.value_forced = 0
        model.vars.FRC_DFLCTRL_DFLBITS.value_forced = 0
        model.vars.FRC_DFLCTRL_DFLINCLUDECRC.value_forced = 0
        model.vars.FRC_DFLCTRL_DFLMODE.value_forced = 0
        model.vars.FRC_DFLCTRL_DFLOFFSET.value_forced = 0
        model.vars.FRC_DFLCTRL_DFLSHIFT.value_forced = 0
        model.vars.FRC_DFLCTRL_MINLENGTH.value_forced = 0

    def FRC_FCD_DISABLE(self, model):
        model.vars.FRC_CTRL_RXFCDMODE.value_forced = 0
        model.vars.FRC_CTRL_TXFCDMODE.value_forced = 0
        model.vars.FRC_FCD0_CALCCRC.value_forced = 0
        model.vars.FRC_FCD0_EXCLUDESUBFRAMEWCNT.value_forced = 0
        model.vars.FRC_FCD0_SKIPWHITE.value_forced = 0
        model.vars.FRC_FCD0_WORDS.value_forced = 255
        model.vars.FRC_FCD1_CALCCRC.value_forced = 0
        model.vars.FRC_FCD1_INCLUDECRC.value_forced = 0
        model.vars.FRC_FCD1_SKIPWHITE.value_forced = 0
        model.vars.FRC_FCD1_WORDS.value_forced = 255
        model.vars.FRC_FCD2_CALCCRC.value_forced = 0
        model.vars.FRC_FCD2_INCLUDECRC.value_forced = 0
        model.vars.FRC_FCD2_SKIPWHITE.value_forced = 0
        model.vars.FRC_FCD2_WORDS.value_forced = 255
        model.vars.FRC_FCD3_BUFFER.value_forced = 0
        model.vars.FRC_FCD3_CALCCRC.value_forced = 0
        model.vars.FRC_FCD3_INCLUDECRC.value_forced = 0
        model.vars.FRC_FCD3_SKIPWHITE.value_forced = 0
        model.vars.FRC_FCD3_WORDS.value_forced = 255

    def FRC_LONGRANGE_OVERRIDE(self, model):
        # needed for simulation
        model.vars.FRC_CTRL_RXFCDMODE.value_forced = 2
        model.vars.FRC_CTRL_TXFCDMODE.value_forced = 2
        model.vars.FRC_FCD0_CALCCRC.value_forced = 1
        model.vars.FRC_FCD0_WORDS.value_forced = 3
        # FIXME: Guner: Why?
        model.vars.MODEM_SYNC0_SYNC0.value_forced = 2391391958

    def MODEM_SHAPING_OVERRIDE(self, model):
        # always overwrite the calculator shaping filter coefficients with ideal Gaussian filter for BLE PHYs
        model.vars.MODEM_CTRL0_SHAPING.value_forced = 2
        model.vars.MODEM_SHAPING0_COEFF0.value_forced = 4
        model.vars.MODEM_SHAPING0_COEFF1.value_forced = 10
        model.vars.MODEM_SHAPING0_COEFF2.value_forced = 20
        model.vars.MODEM_SHAPING0_COEFF3.value_forced = 34
        model.vars.MODEM_SHAPING1_COEFF4.value_forced = 50
        model.vars.MODEM_SHAPING1_COEFF5.value_forced = 65
        model.vars.MODEM_SHAPING1_COEFF6.value_forced = 74
        model.vars.MODEM_SHAPING1_COEFF7.value_forced = 79
        model.vars.MODEM_SHAPING2_COEFF8.value_forced = 0
        model.vars.MODEM_SHAPING2_COEFF9.value_forced = 0
        model.vars.MODEM_SHAPING2_COEFF10.value_forced = 0
        model.vars.MODEM_SHAPING2_COEFF11.value_forced = 0
        model.vars.MODEM_SHAPING3_COEFF12.value_forced = 0
        model.vars.MODEM_SHAPING3_COEFF13.value_forced = 0
        model.vars.MODEM_SHAPING3_COEFF14.value_forced = 0
        model.vars.MODEM_SHAPING3_COEFF15.value_forced = 0
        model.vars.MODEM_SHAPING4_COEFF16.value_forced = 0
        model.vars.MODEM_SHAPING4_COEFF17.value_forced = 0
        model.vars.MODEM_SHAPING4_COEFF18.value_forced = 0
        model.vars.MODEM_SHAPING4_COEFF19.value_forced = 0
        model.vars.MODEM_SHAPING5_COEFF20.value_forced = 0
        model.vars.MODEM_SHAPING5_COEFF21.value_forced = 0
        model.vars.MODEM_SHAPING5_COEFF22.value_forced = 0
        model.vars.MODEM_SHAPING5_COEFF23.value_forced = 0
        model.vars.MODEM_SHAPING6_COEFF24.value_forced = 0
        model.vars.MODEM_SHAPING6_COEFF25.value_forced = 0
        model.vars.MODEM_SHAPING6_COEFF26.value_forced = 0
        model.vars.MODEM_SHAPING6_COEFF27.value_forced = 0
        model.vars.MODEM_SHAPING7_COEFF28.value_forced = 0
        model.vars.MODEM_SHAPING7_COEFF29.value_forced = 0
        model.vars.MODEM_SHAPING7_COEFF30.value_forced = 0
        model.vars.MODEM_SHAPING7_COEFF31.value_forced = 0
        model.vars.MODEM_SHAPING8_COEFF32.value_forced = 0
        model.vars.MODEM_SHAPING8_COEFF33.value_forced = 0
        model.vars.MODEM_SHAPING8_COEFF34.value_forced = 0
        model.vars.MODEM_SHAPING8_COEFF35.value_forced = 0
        model.vars.MODEM_SHAPING9_COEFF36.value_forced = 0
        model.vars.MODEM_SHAPING9_COEFF37.value_forced = 0
        model.vars.MODEM_SHAPING9_COEFF38.value_forced = 0
        model.vars.MODEM_SHAPING9_COEFF39.value_forced = 0
        model.vars.MODEM_SHAPING10_COEFF40.value_forced = 0
        model.vars.MODEM_SHAPING10_COEFF41.value_forced = 0
        model.vars.MODEM_SHAPING10_COEFF42.value_forced = 0
        model.vars.MODEM_SHAPING10_COEFF43.value_forced = 0
        model.vars.MODEM_SHAPING11_COEFF44.value_forced = 0
        model.vars.MODEM_SHAPING11_COEFF45.value_forced = 0
        model.vars.MODEM_SHAPING11_COEFF46.value_forced = 0
        model.vars.MODEM_SHAPING11_COEFF47.value_forced = 0

    def TX_MODE_1(self, model):
        # override Package/pyradioconfig/parts/ocelot/calculators/calc_synth.py output
        # some PHYs are having issue with the TX synth locking in RNM simulation

        model.vars.SYNTH_LPFCTRL2TX_CASELTX.value_forced = 1
        model.vars.SYNTH_LPFCTRL2TX_CAVALTX.value_forced = 8
        model.vars.SYNTH_LPFCTRL2TX_CZSELTX.value_forced = 1
        model.vars.SYNTH_LPFCTRL2TX_CZVALTX.value_forced = 70
        model.vars.SYNTH_LPFCTRL2TX_CFBSELTX.value_forced = 0
        model.vars.SYNTH_LPFCTRL2TX_LPFGNDSWENTX.value_forced = 0
        model.vars.SYNTH_LPFCTRL2TX_LPFINCAPTX.value_forced = 2
        model.vars.SYNTH_LPFCTRL2TX_MODESELTX.value_forced = 0
        model.vars.SYNTH_LPFCTRL1TX_OP1BWTX.value_forced = 5
        model.vars.SYNTH_LPFCTRL1TX_OP1COMPTX.value_forced = 13
        # model.vars.SYNTH_LPFCTRL2TX_CALCTX.value_forced = 16
        model.vars.SYNTH_LPFCTRL1TX_RFBVALTX.value_forced = 0
        model.vars.SYNTH_LPFCTRL1TX_RPVALTX.value_forced = 0
        model.vars.SYNTH_LPFCTRL1TX_RZVALTX.value_forced = 3
        model.vars.SYNTH_LPFCTRL2TX_LPFSWENTX.value_forced = 1
        # model.vars.RAC_SYTRIM0_SYCHPCURRTX.value_forced = 4
        model.vars.SYNTH_DSMCTRLTX_REQORDERTX.value_forced = 0
        model.vars.SYNTH_DSMCTRLTX_MASHORDERTX.value_forced = 0
        model.vars.SYNTH_DSMCTRLTX_DEMMODETX.value_forced = 1
        model.vars.SYNTH_DSMCTRLTX_LSBFORCETX.value_forced = 0
        model.vars.SYNTH_DSMCTRLTX_DSMMODETX.value_forced = 0
        model.vars.SYNTH_DSMCTRLTX_DITHERDACTX.value_forced = 3
        model.vars.SYNTH_DSMCTRLTX_DITHERDSMOUTPUTTX.value_forced = 3
        model.vars.SYNTH_DSMCTRLTX_DITHERDSMINPUTTX.value_forced = 1
        # model.vars.RAC_SYMMDCTRL_SYMMDMODETX.value_forced = 2
        model.vars.RAC_SYTRIM1_SYLODIVLDOTRIMNDIOTX.value_forced = 4
        model.vars.RAC_SYTRIM1_SYLODIVLDOTRIMCORETX.value_forced = 3
        model.vars.RAC_VCOCTRL_VCODETAMPLITUDETX.value_forced = 4
        model.vars.RAC_TX_SYCHPBIASTRIMBUFTX.value_forced = 1
        model.vars.RAC_TX_SYPFDCHPLPENTX.value_forced = 0
        model.vars.RAC_TX_SYPFDFPWENTX.value_forced = 0
        model.vars.RAC_SYEN_SYCHPLPENTX.value_forced = 0
        model.vars.RAC_SYTRIM0_SYCHPLEVPSRCTX.value_forced = 7
        model.vars.RAC_SYTRIM0_SYCHPSRCENTX.value_forced = 1
