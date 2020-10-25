'''
Created on July 25, 2013

@author: shyang
'''

import math
from .si4010apicalcconstants import constants
from .si4010apicalcutilities import CalcUtils
from .msopimpr import MsopImpR
from .msopimpc import MsopImpC
from .soicimpr import SoicImpR
from .soicimpc import SoicImpC

__all__ = ["Si4010ApiCalcResCap"]

class Si4010ApiCalcResCap(object):
    '''
    This class is used to calculate the R1/C1/R2/C2 etc. in the interpolation table.
    The interpolation is used in sheets: "PA Impedance" and "Beta Calc"

    callerType:  The caller's name, can be:
       "BetaCalc"    means called by sheet "Beta Calc"
       "PaImpedance" and other strings means called by "MSOP PA Impedance" or "SOIC PA Impedance" 
    '''
    # prefix
    # SIT_ :  Shift Interpolation Table
    # ECC_ :  Ext Cap Calculations
    # CLMA_ : Cap Lookup for Manual Antenna Z Calcs
    SIT_Freq0       = -1
    SIT_Freq1       = -1
    SIT_Freq2       = -1

    ECC_Cp_diff     = -1
    ECC_Cs_diff     = -1
    ECC_Rs_ExtCap   = -1
    ECC_Rp_ExtCap   = -1
    CLMA_TargetC    = -1
    ManualFreq1CapTable = []
    ManualFreq2CapTable = []
    ManualxCapWordTable = []

    CLMA_Index1     = -1
    CLMA_Index2     = -1
    CLMA_CapWord    = -1
    SIT_CapWord     = -1
    SIT_R1          = -1
    SIT_R2          = -1
    SIT_C1          = -1
    SIT_C2          = -1

    SIT_InterpR     = -1
    SIT_InterpCZ    = -1
    TotalCap        = -1
    TotalRes        = -1
    C_ARRAY_1       = -1
    C_ARRAY_2       = -1
    R_ARRAY_1       = -1
    R_ARRAY_2       = -1



    def __init__ (self, inputs, Zp, callerType):
        '''
        Constructor
        '''
        self.inputs = inputs
        self.original_inputs = self.inputs
        self.choose_pa_const()
        self.evaluate(Zp, callerType)
        return


    def choose_pa_const(self):
        if self.inputs.General.Package == 1:
            self.C_ARRAY_1, self.C_ARRAY_2 = MsopImpC.MSOP_C_ONE, MsopImpC.MSOP_C_TWO
            self.R_ARRAY_1, self.R_ARRAY_2 = MsopImpR.MSOP_R_ONE, MsopImpR.MSOP_R_TWO
        else:
            self.C_ARRAY_1, self.C_ARRAY_2 = SoicImpC.SOIC_C_ONE, SoicImpC.SOIC_C_TWO
            self.R_ARRAY_1, self.R_ARRAY_2 = SoicImpR.SOIC_R_ONE, SoicImpR.SOIC_R_TWO


    def evaluate(self, Zp, callerType):
        self.SIT_Freq0 = self.inputs.PowerAmplifierSetup.Center_Frequency
        self.SIT_Freq1 = CalcUtils.freq1(self.inputs.PowerAmplifierSetup.Center_Frequency)
        self.SIT_Freq2 = CalcUtils.freq2(self.inputs.PowerAmplifierSetup.Center_Frequency)

        # Cs_diff / Cp_diff
        self.ECC_Cs_diff = self.inputs.PowerAmplifierSetup.External_Diff_Cap*0.000000000001
        self.ECC_Cp_diff = self.inputs.PowerAmplifierSetup.External_Diff_Cap*(self.inputs.PowerAmplifierSetup.Q_Factor_of_Ext_Cap)**2/(1 + (self.inputs.PowerAmplifierSetup.Q_Factor_of_Ext_Cap)**2)*0.000000000001
        # Rs_ExtCap / Rp_ExtCap
        if self.inputs.PowerAmplifierSetup.External_Diff_Cap > 0:
            self.ECC_Rs_ExtCap = 1/(self.inputs.PowerAmplifierSetup.External_Diff_Cap*self.inputs.PowerAmplifierSetup.Center_Frequency*1e6*2*math.pi)/self.inputs.PowerAmplifierSetup.Q_Factor_of_Ext_Cap/0.000000000001
        else:
            self.ECC_Rs_ExtCap = 0

        self.ECC_Rp_ExtCap = self.ECC_Rs_ExtCap*(1 + (self.inputs.PowerAmplifierSetup.Q_Factor_of_Ext_Cap)**2)

        self.CLMA_TargetC = 1/Zp/self.inputs.PowerAmplifierSetup.Center_Frequency/2/math.pi/1e6 - self.ECC_Cp_diff

        # Manual Antenna Z Calc Table
        self.ManualFreq1CapTable = self._genManuCapTable(self.C_ARRAY_1[CalcUtils.freqIndex(self.inputs.PowerAmplifierSetup.Center_Frequency)])
        self.ManualFreq2CapTable = self._genManuCapTable(self.C_ARRAY_2[CalcUtils.freqIndex(self.inputs.PowerAmplifierSetup.Center_Frequency)])
        self.ManualxCapWordTable = self._genManuCapTable(MsopImpR.MSOP_xCapWord)
        #print("Target C", self.CLMA_TargetC)
        #print("Freq1 ", self.ManualFreq1CapTable[:10])
        #print("Freq2 ", self.ManualFreq2CapTable[:10])

        if self.CLMA_TargetC < 0:
            self.CLMA_Index1 = 0
            self.CLMA_Index2 = 0
        else:
            self.CLMA_Index1 = self.ManualxCapWordTable[CalcUtils.lookupIndex(self.CLMA_TargetC, self.ManualFreq1CapTable)]
            self.CLMA_Index2 = self.ManualxCapWordTable[CalcUtils.lookupIndex(self.CLMA_TargetC, self.ManualFreq2CapTable)]

        self.CLMA_CapWord = int(round((self.CLMA_Index2 - self.CLMA_Index1)*1.0/(self.SIT_Freq2 - self.SIT_Freq1)*(self.inputs.PowerAmplifierSetup.Center_Frequency - self.SIT_Freq1) + self.CLMA_Index1 + 1e-5))  # In case of 0.5
        #IF(USR_IN_Ant_Manual_Impedance_Entry="No",USR_IN_PA_Nominal_Cap_Word,$AD$15)
        #IF(USR_IN_Ant_Manual_Impedance_Entry="No",USR_IN_PA_Nominal_Cap_Word,CapWord_Manu)
        if (self.inputs.AntennaSetup.Manual_Impedance_Entry == False):
            self.SIT_CapWord = self.inputs.PowerAmplifierSetup.Nominal_Cap_Word
        else:
            self.SIT_CapWord = self.CLMA_CapWord
        if callerType == "BetaCalc":
            ind1 = int(self.CLMA_Index1)
            ind2 = int(self.CLMA_Index2)
        else:
            ind1   = int(self.SIT_CapWord)
            ind2   = int(ind1)
        self.SIT_R1 = self.R_ARRAY_1[CalcUtils.freqIndex(self.inputs.PowerAmplifierSetup.Center_Frequency)][ind1]
        self.SIT_R2 = self.R_ARRAY_2[CalcUtils.freqIndex(self.inputs.PowerAmplifierSetup.Center_Frequency)][ind2]
        self.SIT_C1 = self.C_ARRAY_1[CalcUtils.freqIndex(self.inputs.PowerAmplifierSetup.Center_Frequency)][ind1]
        self.SIT_C2 = self.C_ARRAY_2[CalcUtils.freqIndex(self.inputs.PowerAmplifierSetup.Center_Frequency)][ind2]

        self.SIT_InterpR  = (self.SIT_R2 - self.SIT_R1)/(self.SIT_Freq2 - self.SIT_Freq1)*(self.inputs.PowerAmplifierSetup.Center_Frequency - self.SIT_Freq1) + self.SIT_R1
        self.SIT_InterpCZ = (self.SIT_C2 - self.SIT_C1)/(self.SIT_Freq2 - self.SIT_Freq1)*(self.inputs.PowerAmplifierSetup.Center_Frequency - self.SIT_Freq1) + self.SIT_C1

        if callerType == "BetaCalc":
            # -1/((Interp_Z_Pos+Cp_diff_Pos)*2*PI()*1000000*SIT_Freq0_Pos)
            self.TotalCap = -1/((self.SIT_InterpCZ + self.ECC_Cp_diff)*2*math.pi*1000000*self.SIT_Freq0)
        else:
            self.TotalCap = self.SIT_InterpCZ + self.ECC_Cp_diff
        if self.inputs.PowerAmplifierSetup.External_Diff_Cap > 0:
            self.TotalRes = 1/(1/self.SIT_InterpR + 1/self.ECC_Rp_ExtCap)
        else:
            self.TotalRes = self.SIT_InterpR


    def _genManuCapTable (self, FreqTabl):
        ManuCapTable = [0]
        for m in FreqTabl:
            ManuCapTable.append(m)

        return ManuCapTable


