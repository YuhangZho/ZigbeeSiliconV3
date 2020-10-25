'''
Created on Jul 25, 2013

@author: sesuskic
'''
from si4440_modem_calc.pro2modemcalcinputs import Pro2ModemCalcInputs
from si4440_modem_calc.pro2plusmodemcalcinputs import Pro2PlusModemCalcInputs
from common import InputInterface
from .crystalcapbankoptions import CrystalCapBank, Pro2CrystalCapBank
from .powerupoptions import PowerUp, Pro2PowerUp
from .gpiopincfg import GpioPinCfg
from .packetconfig import Pkt, Sync, Preamble, Match
from .interruptcontrolconfiguration import InterruptControl
from .poweramplifier import PowerAmplifier
from .fastresponseregs import FastResponseRegs
from .bandwidthtimeconfig import BandwidthTime
from .modemfskconfig import ModemFsk4Map
from .globalconfig import Global, GlobalPlus
from .performiqcal import PerformIqCal
from .rssiconfig import RssiConfig
from .autorxfreqhopconfig import AutoRxFreqHop

__all__ = ["ChipInputs", "Pro2Inputs", "Pro2PlusInputs"]


class ChipInputs(InputInterface):
    def __init__(self, radioApi):
        self.PowerUp = PowerUp()
        self.GpioPinCfg = GpioPinCfg()
        self.CrystalCapBank = CrystalCapBank()
        self.FastResponseRegs = FastResponseRegs(radioApi)
        self.Pkt = Pkt(radioApi)
        self.Sync = Sync(radioApi)
        self.Preamble = Preamble(radioApi)
        self.Match = Match(radioApi)
        self.Modem = Pro2ModemCalcInputs()
        self.InterruptControl = InterruptControl(radioApi)
        if "PA_GRP" in radioApi.features:
            self.PowerAmplifier = PowerAmplifier(radioApi)
        if "RX_HOP_GRP" in radioApi.features:
            self.AutoRxFreqHop = AutoRxFreqHop(radioApi)
    
class Pro2Inputs(ChipInputs):
    def __init__(self, radioApi):
        super(Pro2Inputs, self).__init__(radioApi)
        self.PowerUp = Pro2PowerUp()
        self.CrystalCapBank = Pro2CrystalCapBank()
        if "TX" in radioApi.features:
            self.BandwidthTime = BandwidthTime()
        self.ModemFsk4Map = ModemFsk4Map()
        self.Global = Global(radioApi)
        if "IR_CAL" in radioApi.features:
            self.PerformIqCal = PerformIqCal()
        if "RX" in radioApi.features:
            self.RssiConfig = RssiConfig()
        
        
class Pro2PlusInputs(Pro2Inputs):
    def __init__(self, radioApi):
        super(Pro2PlusInputs, self).__init__(radioApi)
        # Disable this category for revC parts. The modem calculator
        # computes this the associated values for revC but not for revB.
        # If the client still wants to configure these directly, effectively
        # overriding what the modem calculator does, then the client needs
        # to enable the category.
        if hasattr(self, "RssiConfig"):
            self.RssiConfig.enabled = False
        self.Modem = Pro2PlusModemCalcInputs()
        self.Global = GlobalPlus(radioApi)
