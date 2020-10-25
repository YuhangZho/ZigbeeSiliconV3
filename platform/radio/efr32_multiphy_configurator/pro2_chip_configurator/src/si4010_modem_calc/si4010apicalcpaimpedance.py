'''
Created on July 25, 2013

@author: shyang
'''
import math
from .si4010apicalcrescap import Si4010ApiCalcResCap



__all__ = ["Si4010ApiCalcPAImpedance"]

class Si4010ApiCalcPAImpedance(object):
    '''
    classdocs
    '''
    # prefix
    PackageType = -1
    PACapRes    = []
    CR_CI_Real_Z = -1   # CALCULATED RESULTES/Chip Impedance/Real_Z
    CR_CI_Imag_Z = -1   # CALCULATED RESULTES/Chip Impedance/Imag_Z
    API_vPa_Setup__wNominalCap  = -1

    def __init__(self, inputs):
        '''
        Constructor
        '''
        #def __init__ (self, In_PackageType,
        #              In_ManualImpedanceEntry,   In_AntennaReal,           In_AntennaImage,
        #              In_ExternalDiffCap,        In_QfactorOfExternalCap,  In_CenterFreq,
        #              In_NominalCapWord):

        self.inputs = inputs
        self.original_inputs = self.inputs
        self.genCapResTable()
        self.eval_api_paras()
        #self.dump() # TODO DEL WHEN RELEASE
        return

    def genCapResTable(self):

        #calc.PAImpedance(1, "No", 1000, 40, 1, 300, 434.2, 10)

        self.PackageType = self.inputs.General.Package

        self.PACapRes = Si4010ApiCalcResCap(self.inputs, self.inputs.AntennaSetup.Antenna_Imag_Z, "PaImpedance")


    def eval_api_paras(self):
        #IF(USR_IN_Package_Type=1,'MSOP PA Impedance'!Total Res,'SOIC PA Impedance'!Total Res)
        self.CR_CI_Real_Z = self.PACapRes.TotalRes
        #IF(USR_IN_Package_Type=1,
        #   -1/(2*PI()*'MSOP PA Impedance'!Total Cap*USR_IN_PA_Center_Frequency*1000000),
        #   -1/(2*PI()*'SOIC PA Impedance'!Total Cap*USR_IN_PA_Center_Frequency*1000000))
        self.CR_CI_Imag_Z = -1/(2*math.pi*self.PACapRes.TotalCap*self.inputs.PowerAmplifierSetup.Center_Frequency*1000000)
        #"IF(USR_IN_Ant_Manual_Impedance_Entry=""Yes"",
        #    IF(USR_IN_Package_Type="0",
        #        'SOIC PA Impedance'!CapWord_Manu,
        #    ELSE
        #        CapWord_Manu),
        #ELSE
        #    USR_IN_PA_Nominal_Cap_Word)"
        if self.inputs.AntennaSetup.Manual_Impedance_Entry == True:
            self.API_vPa_Setup__wNominalCap = self.PACapRes.CLMA_CapWord
        else:
            self.API_vPa_Setup__wNominalCap = self.inputs.PowerAmplifierSetup.Nominal_Cap_Word

            

    def dump (self):
        print("\n Dump information of Class:{}".format(self.__class__.__name__))
        print("    ------------ Package type ------------")
        print("    Package          :", self.PackageType)
        print("                     :"                   )
        print("    ------------ Shift Interpolation Table ------------")
        print("    SIT_Freq0        :", self.PACapRes.SIT_Freq0       )
        print("    SIT_Freq1        :", self.PACapRes.SIT_Freq1       )
        print("    SIT_Freq2        :", self.PACapRes.SIT_Freq2       )
        print("    SIT_R1           :", self.PACapRes.SIT_R1          )
        print("    SIT_R2           :", self.PACapRes.SIT_R2          )
        print("    SIT_C1           :", self.PACapRes.SIT_C1          )
        print("    SIT_C2           :", self.PACapRes.SIT_C2          )
        print("    SIT_CapWord      :", self.PACapRes.SIT_CapWord     )
        print("    SIT_InterpR      :", self.PACapRes.SIT_InterpR     )
        print("    SIT_InterpC      :", self.PACapRes.SIT_InterpCZ    )
        print("                     :"                                )
        print("    ------------ Cap Lookup for Manual Antenna Z Calcs ------------")
        print("    CLMA_TargetC     :", self.PACapRes.CLMA_TargetC    )
        print("    CLMA_Index1      :", self.PACapRes.CLMA_Index1     )
        print("    CLMA_Index2      :", self.PACapRes.CLMA_Index2     )
        print("    CLMA_CapWord     :", self.PACapRes.CLMA_CapWord    )
        print("                     :"                                )
        print("    ------------ Ext Cap Calculations ------------")
        print("    ECC_Cp_diff      :", self.PACapRes.ECC_Cp_diff     )
        print("    ECC_Cs_diff      :", self.PACapRes.ECC_Cs_diff     )
        print("    ECC_Rs_ExtCap    :", self.PACapRes.ECC_Rs_ExtCap   )
        print("    ECC_Rp_ExtCap    :", self.PACapRes.ECC_Rp_ExtCap   )
        print("                     :"                                )
        print("    ------------ Total ------------")
        print("    TotalCap    =    :", self.PACapRes.TotalCap        )
        print("    TotalRes    =    :", self.PACapRes.TotalRes        )
        print("                     :"                                )
        print("    ------------ Tables ------------")
        print("    ManualFreq1CapTable    :", self.PACapRes.ManualFreq1CapTable[:4])
        print("    ManualFreq2CapTable    :", self.PACapRes.ManualFreq2CapTable[:4])
        print("    ManualxCapWordTable    :", self.PACapRes.ManualxCapWordTable[:4])
        print("                     :"                                )
        print("    ------------ Calculator Result ------------")
        print("    ------------     Chip Impedance ------------")
        print("    CR_CI_Real_Z     :", self.CR_CI_Real_Z)
        print("    CR_CI_Imag_Z     :", self.CR_CI_Imag_Z)
        print("    ------------ API Parameters ------------")
        print("    API:vPa_Setup        :\n        API_vPa_Setup__wNominalCap:    {}".format(self.API_vPa_Setup__wNominalCap))



