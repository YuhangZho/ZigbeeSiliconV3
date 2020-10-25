'''
Created on Aug 5, 2013

@author: sesuskic
'''

from .siradio import SiRadio
from .modemcalchelper import ModemCalcHelper
from .inputs import ChipInputs
from .ezradio2config import EzRadio2Config
from .packetconfig import Pkt, Sync, Preamble, Match
from .siradioapifactory import SiRadioApiFactory
from .globalconfig import Global, GlobalPlus
from si4440_modem_calc import Pro2PlusModemCalc, Pro2PlusModemCalcInputs

class EzRadio2(SiRadio):
    def __init__(self, radioApi):
        super(EzRadio2, self).__init__(radioApi)
        self._create_ezconfig()
        
    def _create_ezconfig(self):
        self.ezconfig = EzRadio2Config()

    def _override_ezconfig_properties(self):
        # TODO: Figure out if there is a better way to key off of features in radio api 
        if "RX" in self.radio_api.features:
            # MODEM_RSSI_CONTROL and GLOBAL_CONFIG are set to these values in .NET WDS
            self.ezconfig.set_property("MODEM_RSSI_CONTROL", 0x2)
            self.ezconfig.set_property("GLOBAL_CONFIG", 0x21)
        if "TX" in self.radio_api.features:
            # set PA_MODE, PA_BIAS_CLKDUTY, and PA_TC to the values from .NET WDS
            self.ezconfig.set_property("PA_MODE", 0x18)
            self.ezconfig.set_property("PA_BIAS_CLKDUTY", 0xC0)
            self.ezconfig.set_property("PA_TC", 0x5D)
#         raise RuntimeError("Must implement in derived class")
    
    def _create_inputs(self, radioApi):
        self.inputs = ChipInputs(radioApi)
        self.inputs.Pkt = Pkt(self.modem_radio_api)
        self.inputs.Sync = Sync(self.modem_radio_api)
        self.inputs.Preamble = Preamble(self.modem_radio_api)
        self.inputs.Match = Match(self.modem_radio_api)
        self.inputs.Global = Global(self.modem_radio_api)

    def _restrict_modem_inputs(self):
        self.inputs.Modem.API_hi_pfm_div_mode = 0
        
    def _create_modem_radio_api(self):
        self.modem_radio_api = SiRadioApiFactory.createRadioApi("si4460_revb1b")
        
    def _get_ezconfig_properties(self):
        propertyList = ModemCalcHelper.configureModem(self)
        propertyList += self.inputs.Preamble.get_property_list()
        propertyList += self.inputs.Sync.get_property_list()
        propertyList += self.inputs.Pkt.get_property_list()
        propertyList += self.inputs.Match.get_property_list()
        propertyList += self.inputs.Global.get_property_list()
        if "PA_GRP" in self.radio_api.features:
            propertyList += self.inputs.PowerAmplifier.get_property_list()
        return propertyList
        
    def _configure_ezconfig(self, **kwargs):
        command_list = []
        propertyList = self._get_ezconfig_properties()
        self.ezconfig.set_properties(propertyList)
        self._override_ezconfig_properties()
        # The property list might get expanded to include additional configuration
        ezconfig_seed = None
        if kwargs:
            if "ezconfig_seed" in kwargs.keys():
                ezconfig_seed = kwargs["ezconfig_seed"]
        ezconfig_array = self.ezconfig.get_array(ezconfig_seed)
        command_list.append(self.get_command("EZCONFIG_ARRAY_WRITE")
                            .set_field("DATA", "DATA", ezconfig_array))
        command_list.append(self.get_command("EZCONFIG_CHECK")
                            .set_field("CHECKSUM", "CHECKSUM", self.ezconfig.get_crc()))
        return command_list
        
    def _configureImpl(self, **kwargs):
        self.command_list[:] = []
        self.command_list += self.inputs.PowerUp.get_configuration(self)
        self.command_list += self.inputs.InterruptControl.get_configuration(self)
        self.command_list += self.inputs.FastResponseRegs.get_configuration(self)
        self.command_list += self.inputs.CrystalCapBank.get_configuration(self)
        self.command_list += self._configure_ezconfig(**kwargs)
        self.command_list += self.inputs.GpioPinCfg.get_configuration(self)
        return self.get_raw_bytes(**kwargs)

class EzRadio2Pro2Plus(EzRadio2):
    def __init__(self, radioApi):
        super(EzRadio2Pro2Plus, self).__init__(radioApi)
        
    def _create_inputs(self, radioApi):
        super(EzRadio2Pro2Plus, self)._create_inputs(radioApi)
        self.inputs.Modem = Pro2PlusModemCalcInputs()
        self.inputs.Global = GlobalPlus(self.modem_radio_api)

    def _create_ezconfig(self):
        self.ezconfig = EzRadio2Config("pro2plus")

    def _create_modem_radio_api(self):
        self.modem_radio_api = SiRadioApiFactory.createRadioApi("si4460_revc2a")
        
    def _create_modem_calculator(self, modem_inputs):
        return Pro2PlusModemCalc(modem_inputs)
   
