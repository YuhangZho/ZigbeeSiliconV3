'''
Created on July 25, 2013

@author: shyang
'''

from collections import OrderedDict
from common import ChipConfiguratorInterface
from .si4010apicalcinputs import Si4010ApiCalcInputs
from .si4010apicalcfsklookup import Si4010ApiCalcFSKLookup
from .si4010apicalcpaimpedance import Si4010ApiCalcPAImpedance
from .si4010apicalcods       import Si4010ApiCalcODS
from .si4010apicalcpower     import Si4010ApiCalcPower
from .si4010apicalcbetacalc  import Si4010ApiCalcBetaCalc


__all__ = ["Si4010ApiCalcBetaCalc"]

class Si4010ApiCalc(ChipConfiguratorInterface):
    '''
    classdocs
    '''
    def __init__(self, inputs=None):
        '''
        Constructor
        '''
        super(Si4010ApiCalc,self).__init__()
        self.inputs = Si4010ApiCalcInputs(inputs)
        self._sync_categories_and_options()
        self._api_list = OrderedDict({})
        
    def configure(self, **kwargs):
        self.calculate()
        return self.get_api_list()
        
    def calculate(self):
        #print("\n\nGen a FSKLooup object:\n")
        self.FSKLookup = Si4010ApiCalcFSKLookup(self.inputs)

        #print("\n\nGen a PA Impedance object:\n")
        self.PAImpedance = Si4010ApiCalcPAImpedance(self.inputs)

        #print("\n\nGen a ODS object:\n")
        self.ODS = Si4010ApiCalcODS(self.inputs)

        #print("\n\nGen a Calc Power object:\n")
        self.CalcPower = Si4010ApiCalcPower(self.inputs, self.PAImpedance)

        #print("\n\nGen a Beta Calc object:\n")
        self.BetaCalc = Si4010ApiCalcBetaCalc(self.inputs, self.PAImpedance, self.CalcPower)

        self.si4010_api_list()

    def  get_api_list(self):
        return self._api_list



    def si4010_api_list(self):
        # vPa_Setup
        self._api_list['bMaxDrv']      = int(self.CalcPower.API_vPa_Setup__bMaxDrv)
        self._api_list['bLevel']       = int(self.CalcPower.API_vPa_Setup__bLevel)
        self._api_list['wNominalCap']  = int(self.PAImpedance.API_vPa_Setup__wNominalCap)
        self._api_list['fAlpha']       = self.inputs.AntennaSetup.Alpha
        self._api_list['fBeta']        = self.BetaCalc.API_vPa_Setup__fBeta
        # vFcast_Tune
        self._api_list['fiDesiredFreq'] = self.FSKLookup.API_vFCast_Tune__fiDesiredFreq
        # vFCast_FskAdj
        self._api_list['biFskDev'] = int(self.FSKLookup.API_vFCast_FskAdj_biFskDev)
        # vODs_Setup
        self._api_list['bModulationType'] = int(self.ODS.API_vOds_Setup__bModulationType)
        self._api_list['bClkDiv']         = int(self.ODS.API_vOds_Setup__bClkDiv)
        self._api_list['bEdgeRate']       = int(self.ODS.API_vOds_Setup__bEdgeRate)
        self._api_list['bGroupWidth']     = int(self.ODS.API_vOds_Setup__bGroupWidth)
        self._api_list['wBitRate']        = int(self.ODS.API_vOds_Setup__wBitRate)
        self._api_list['bLcWarmInt']      = int(self.ODS.API_vOds_Setup__bLcWarmInt)
        self._api_list['bDivWarmInt']     = int(self.ODS.API_vOds_Setup__bDivWarmInt)
        self._api_list['bPaWarmInt']      = int(self.ODS.API_vOds_Setup__bPaWarmInt)


