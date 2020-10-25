'''
Created on July 25, 2013

@author: shyang
'''

import math
from .si4010apicalcutilities import CalcUtils
from .si4010apicalcrescap import Si4010ApiCalcResCap


__all__ = ["Si4010ApiCalcBetaCalc"]

class Si4010ApiCalcBetaCalc(object):
    '''
    classdocs
    '''
    # Cal PA first, then CalPower
    # TODO When evulate < Max the floating number is not accuarc
    # bc = calc.BetaCalc("Yes", 1000, 40, 15, 434.2, 10, 1, 300, pa, cp)
    TargetFreq       = -1
    InterpFreq       = -1

    NomAntennaZp     = -1
    NomAntennaRp     = -1

    NomAntennaZs     = -1
    NomAntennaRs     = -1

    PosShiftInZs     = -1
    NegShiftInZs     = -1

    NomQp            = -1
    NomQs            = -1
    Qs_Pos           = -1
    Qs_Neg           = -1
    Rp_Pos           = -1
    Rp_Neg           = -1
    Zp_Pos           = -1
    Zp_Neg           = -1
    shift            = -1


    Beta_Pos_CapRes = []
    Beta_Neg_CapRes = []
    DiffPkgParR_Pos = -1
    DiffPkgParR_Neg = -1
    DiffParAntR_Pos = -1
    DiffParAntR_Neg = -1
    fBeta_Pos       = -1
    fBeta_Neg       = -1
    API_vPa_Setup__fBeta   = -1

    def __init__ (self, inputs,  PAObj, CalcPowerObj):
        '''
        Constructor
        '''
        self.inputs = inputs
        self.original_inputs = self.inputs
        self.calc(PAObj, CalcPowerObj)
        #self.dump() # TODO DEL WHEN RELEASE
        return



    def calc(self, PAObj, CalcPowerObj):
        self.TargetFreq    = self.inputs.PowerAmplifierSetup.Center_Frequency
        self.InterpFreq    = CalcUtils.freq2(self.inputs.PowerAmplifierSetup.Center_Frequency)
        self.NomAntennaZp  = CalcPowerObj.CR_AT_Imag_Z
        self.NomAntennaRp  = CalcPowerObj.CR_AT_Real_Z
        self.shift         = self.inputs.AntennaSetup.Reactance_change
        # Nom Antenna Zp/Nom Antenna Rp
        self.NomQp         = self.NomAntennaZp/self.NomAntennaRp
        # Nom Antenna Zp/(1+Nom Qp^2)
        self.NomAntennaZs  = self.NomAntennaZp/(1 + self.NomQp**2)
        # Nom Qp^2*Nom Antenna Rp/(1+Nom Qp^2)
        self.NomAntennaRs  = self.NomQp**2*self.NomAntennaRp/(1 + self.NomQp**2)
        # Nom Antenna Zs/Nom Antenna Rs
        self.NomQs         = self.NomAntennaZs/self.NomAntennaRs
        # Nom Antenna Zs*(1+% Shift/100)
        self.PosShiftInZs  = self.NomAntennaZs*(1 + self.shift/100.0)
        # Nom Antenna Zs*(1-percent_Shift/100)
        self.NegShiftInZs  = self.NomAntennaZs*(1 - self.shift/100.0)
        # Pos Shift in Zs/Nom Antenna Rs
        self.Qs_Pos = self.PosShiftInZs/self.NomAntennaRs
        # Neg Shift in Zs/Nom Antenna Rs
        self.Qs_Neg = self.NegShiftInZs/self.NomAntennaRs
        # Nom Antenna Rs*(1+Qs (Pos shift)^2)
        self.Rp_Pos = self.NomAntennaRs*(1 + self.Qs_Pos**2)
        # Nom Antenna Rs*(1+Qs (Neg shift)^2)
        self.Rp_Neg = self.NomAntennaRs*(1 + self.Qs_Neg**2)
        # Pos Shift in Zs*(1+Qs (Pos shift)^2)/Qs (Pos shift)^2
        self.Zp_Pos = self.PosShiftInZs*(1 + self.Qs_Pos**2)/self.Qs_Pos**2
        # Neg Shift in Zs*(1+Qs (Neg shift)^2)/Qs (Neg shift)^2
        self.Zp_Neg = self.NegShiftInZs*(1 + self.Qs_Neg**2)/self.Qs_Neg**2
      
        self.Beta_Pos_CapRes = Si4010ApiCalcResCap(self.inputs, self.Zp_Pos, "BetaCalc")
        self.Beta_Neg_CapRes = Si4010ApiCalcResCap(self.inputs, self.Zp_Neg, "BetaCalc")

        self.DiffPkgParR_Pos = self.Beta_Pos_CapRes.TotalRes
        self.DiffPkgParR_Neg = self.Beta_Neg_CapRes.TotalRes
        self.DiffParAntR_Pos = self.Rp_Pos
        self.DiffParAntR_Neg = self.Rp_Neg

        #IF(CALCULATOR!$K$9-$D$26=0,0,10*LOG10((($F$15/($F$15+$F$16))^2*$F$16)/((CALCULATOR!$G$9/(CALCULATOR!$G$9+CALCULATOR!$G$16))^2*CALCULATOR!$G$16))/0.253/(CALCULATOR!$K$9-$D$26))
        #
        #"IF(wNominalCap-POS_CapWord=0,
        #    0,
        #ELSE
        #  10*LOG10(
        #             ((Diff Pkg Par R (Rp_pkg)_POS/(Diff Pkg Par R (Rp_pkg)_POS+Diff Par Ant R (Rp_ant)_POS))^2*Diff Par Ant R (Rp_ant)_POS)  
        #              / 
        #             ((CR_CI_Real_Z/(CR_CI_Real_Z+CR_AT_Real_Z))^2*CR_AT_Real_Z)
        #           )
        #     /0.253/(wNominalCap-POS_CapWord)
        # x = (Diff Pkg Par R (Rp_pkg)_POS/(Diff Pkg Par R (Rp_pkg)_POS+Diff Par Ant R (Rp_ant)_POS))^2*Diff Par Ant R (Rp_ant)_POS)
        # y = ((CR_CI_Real_Z/(CR_CI_Real_Z+CR_AT_Real_Z))^2*CR_AT_Real_Z)
        # z = (wNominalCap-POS_CapWord)
        # 10*LOG10((x/y)/0.253/z

        if PAObj.API_vPa_Setup__wNominalCap == self.Beta_Pos_CapRes.CLMA_CapWord:
            self.fBeta_Pos = 0
        else:
            x = (self.DiffPkgParR_Pos/(self.DiffPkgParR_Pos + self.DiffParAntR_Pos))**2*self.DiffParAntR_Pos
            y = ((PAObj.CR_CI_Real_Z/(PAObj.CR_CI_Real_Z + CalcPowerObj.CR_AT_Real_Z))**2*CalcPowerObj.CR_AT_Real_Z)
            z = (PAObj.API_vPa_Setup__wNominalCap - self.Beta_Pos_CapRes.CLMA_CapWord)
            self.fBeta_Pos = 10*math.log(x/y, 10)/0.253/z

        if PAObj.API_vPa_Setup__wNominalCap == self.Beta_Neg_CapRes.CLMA_CapWord:
            self.fBeta_Neg = 0
        else:
            x = (self.DiffPkgParR_Neg/(self.DiffPkgParR_Neg + self.DiffParAntR_Neg))**2*self.DiffParAntR_Neg
            y = ((PAObj.CR_CI_Real_Z/(PAObj.CR_CI_Real_Z + CalcPowerObj.CR_AT_Real_Z))**2*CalcPowerObj.CR_AT_Real_Z)
            z = (PAObj.API_vPa_Setup__wNominalCap - self.Beta_Neg_CapRes.CLMA_CapWord)
            self.fBeta_Neg = 10*math.log(x/y, 10)/0.253/z

        # ('Beta Calc'!fBeta_Pos+'Beta Calc'!fBeta_Neg)/2
        self.API_vPa_Setup__fBeta = (self.fBeta_Neg + self.fBeta_Pos)/2.0

    def dump(self):
        print("        TargetFreq                       :", self.TargetFreq       )
        print("        InterpFreq                       :", self.InterpFreq       )
        print("                                         :",                  )
        print("        NomAntennaZp                     :", self.NomAntennaZp     )
        print("        NomAntennaRp                     :", self.NomAntennaRp     )
        print("                                         :",                  )
        print("        NomAntennaZs                     :", self.NomAntennaZs     )
        print("        NomAntennaRs                     :", self.NomAntennaRs     )
        print("                                         :",                  )
        print("        PosShiftInZs                     :", self.PosShiftInZs     )
        print("        NegShiftInZs                     :", self.NegShiftInZs     )
        print("                                         :",                  )
        print("        NomQp                            :", self.NomQp            )
        print("        NomQs                            :", self.NomQs            )
        print("        Qs_Pos                           :", self.Qs_Pos           )
        print("        Qs_Neg                           :", self.Qs_Neg           )
        print("        Rp_Pos                           :", self.Rp_Pos           )
        print("        Rp_Neg                           :", self.Rp_Neg           )
        print("        Zp_Pos                           :", self.Zp_Pos           )
        print("        Zp_Neg                           :", self.Zp_Neg           )
        print("        shift                            :", self.shift            )
        print("                                         :",                  )

        print("    ResCap Table:  Pos and Neg   :")
        print("    ---------------------------------------------")
        for CapResT in [self.Beta_Pos_CapRes, self.Beta_Neg_CapRes]:
            print("        SIT_Freq0        :", CapResT.SIT_Freq0       )
            print("        SIT_Freq1        :", CapResT.SIT_Freq1       )
            print("        SIT_Freq2        :", CapResT.SIT_Freq2       )
            print("                         :"                                )
            print("        ECC_Cp_diff      :", CapResT.ECC_Cp_diff     )
            print("        ECC_Cs_diff      :", CapResT.ECC_Cs_diff     )
            print("        ECC_Rs_ExtCap    :", CapResT.ECC_Rs_ExtCap   )
            print("        ECC_Rp_ExtCap    :", CapResT.ECC_Rp_ExtCap   )
            print("        CLMA_TargetC     :", CapResT.CLMA_TargetC    )
            print("        ManualFreq1CapTable    :", CapResT.ManualFreq1CapTable[:4])
            print("        ManualFreq2CapTable    :", CapResT.ManualFreq2CapTable[:4])
            print("        ManualxCapWordTable    :", CapResT.ManualxCapWordTable[:4])
            print("                         :"                                )
            print("        CLMA_Index1      :", CapResT.CLMA_Index1     )
            print("        CLMA_Index2      :", CapResT.CLMA_Index2     )
            print("        CLMA_CapWord     :", CapResT.CLMA_CapWord    )
            print("        SIT_CapWord      :", CapResT.SIT_CapWord     )
            print("        SIT_R1           :", CapResT.SIT_R1          )
            print("        SIT_R2           :", CapResT.SIT_R2          )
            print("        SIT_C1           :", CapResT.SIT_C1          )
            print("        SIT_C2           :", CapResT.SIT_C2          )
            print("                         :"                                )
            print("        SIT_InterpR      :", CapResT.SIT_InterpR     )
            print("        SIT_InterpC      :", CapResT.SIT_InterpCZ    )
            print("        TotalCap    =    :", CapResT.TotalCap        )
            print("        TotalRes    =    :", CapResT.TotalRes        )
            print("        ---------------------------------------------")


        print("    DiffPkgParR_Pos                  :", self.DiffPkgParR_Pos  )
        print("    DiffPkgParR_Neg                  :", self.DiffPkgParR_Neg  )
        print("    DiffParAntR_Pos                  :", self.DiffParAntR_Pos  )
        print("    DiffParAntR_Neg                  :", self.DiffParAntR_Neg  )
        print("    fBeta_Pos                        :", self.fBeta_Pos        )
        print("    fBeta_Neg                        :", self.fBeta_Neg        )
        print("    ------------ API Parameters ------------")
        print("    API:vPa_Setup        :")
        print("            API_vPa_Setup__bMaxDrv:    {}".format(self.API_vPa_Setup__fBeta))

        
