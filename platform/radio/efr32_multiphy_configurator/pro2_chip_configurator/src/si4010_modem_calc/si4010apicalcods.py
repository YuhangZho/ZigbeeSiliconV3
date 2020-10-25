'''
Created on July 25, 2013

@author: shyang
'''

import math
from .si4010apicalcconstants import constants
from .si4010apicalcutilities import CalcUtils
from .rampdata import RampData


__all__ = ["Si4010ApiCalcODS"]

class Si4010ApiCalcODS(object):
    '''
    classdocs
    '''
    ErrorTable       = []
    RampRateTable    = []
    TargetBitRate    = -1
    TargetSymbolRate = -1
    DesiredRampTime  = -1
    ods_ck_div      = -1
    ods_edge_time    = -1
    ods_group_width  = -1

    ActualSymbolRate = -1

    ods_warm_pa      = -1
    ods_warm_div     = -1
    ods_warm_osc     = -1
    # Name from API setting in sheet CALCULATOR
    API_vOds_Setup__bModulationType  = -1
    API_vOds_Setup__bClkDiv          = -1
    API_vOds_Setup__bEdgeRate        = -1
    API_vOds_Setup__bGroupWidth      = -1
    API_vOds_Setup__wBitRate         = -1
    API_vOds_Setup__bLcWarmInt       = -1
    API_vOds_Setup__bDivWarmInt      = -1
    API_vOds_Setup__bPaWarmInt       = -1


    def __init__(self, inputs):
        '''
        Constructor
        '''
        self.inputs = inputs
        self.original_inputs = self.inputs

        self.calc()
        self.eval_result_in_calc()
        self.eval_api_paras()
        #self.dump() # TODO DEL WHEN RELEASE
        return

    def calc(self):
        self.ErrorTable    = []
        self.RampRateTable = []

        self.TargetBitRate = self.inputs.SerializerSetup.Bit_Data_Rate
        # Target Symbol Rate(Ksym/s)
        if self.inputs.SerializerSetup.Encoding == 'Manchester':
            self.TargetSymbolRate = 2*self.inputs.SerializerSetup.Bit_Data_Rate
        elif self.inputs.SerializerSetup.Encoding == "NRZ+4b5b":
            self.TargetSymbolRate = 1.25*self.inputs.SerializerSetup.Bit_Data_Rate
        elif self.inputs.SerializerSetup.Encoding == "Other":  #TODO Other?
            self.TargetSymbolRate = 1*self.inputs.SerializerSetup.Bit_Data_Rate # TODO Menus!$C$5*targetbps
        else:
            self.TargetSymbolRate = self.inputs.SerializerSetup.Bit_Data_Rate

        # Desired Ramp Time
        #"IF(AND(USR_IN_SS_Manual_Ramp_Rate_Entry=""No"",USR_IN_SS_Modulation=""OOK""),
        #    IF(USR_IN_SS_Encoding=""Manchester"",
        #        VLOOKUP(USR_IN_SS_Bit_Data_Rate,'Ramp Data'!$A$3:$B$78,2),
        #    ELSE
        #        IF(USR_IN_SS_Encoding=""NRZ+4b5b"",
        #            VLOOKUP(USR_IN_SS_Bit_Data_Rate*1.25,'Ramp Data'!$D$3:$E$242,2),
        #        ELSE
        #            VLOOKUP(USR_IN_SS_Bit_Data_Rate,'Ramp Data'!$D$3:$E$242,2))),
        #ELSE
        #    IF(AND(USR_IN_SS_Manual_Ramp_Rate_Entry=""No"",USR_IN_SS_Modulation=""FSK""),
        #        IF(Target Symbol Rate (Ksym/s)<1,
        #            8,
        #        ELSE
        #            2),
        #    ELSE
        #        USR_IN_SS_Target_Ramp_Rate))"

        if self.inputs.SerializerSetup.Manual_Ramp_Rate_Entry == False and self.inputs.SerializerSetup.Modulation == "OOK":
            if self.inputs.SerializerSetup.Encoding == "Manchester":
                self.DesiredRampTime = CalcUtils.lookupLR(self.inputs.SerializerSetup.Bit_Data_Rate, RampData.RampData_Manch_DataRate, RampData.RampData_Manch_RampRate)
            else:
                if self.inputs.SerializerSetup.Encoding == "NRZ+4b5b":
                    self.DesiredRampTime = CalcUtils.lookupLR(self.inputs.SerializerSetup.Bit_Data_Rate*1.25, RampData.RampData_NRZ_DataRate, RampData.RampData_NRZ_RampRate)
                else:
                    self.DesiredRampTime = CalcUtils.lookupLR(self.inputs.SerializerSetup.Bit_Data_Rate,      RampData.RampData_NRZ_DataRate, RampData.RampData_NRZ_RampRate)
        else:
            if self.inputs.SerializerSetup.Manual_Ramp_Rate_Entry == False and self.inputs.SerializerSetup.Modulation == "FSK":
                if self.TargetSymbolRate < 1:
                    self.DesiredRampTime = 8
                else:
                    self.DesiredRampTime = 2
            else:
                self.DesiredRampTime = self.inputs.SerializerSetup.Target_Ramp_Rate

        # Generate RampRate and Error Tables   
        for i in range(len(constants.ODS_CkDiv)):
            RampRate = 1/24e6*(1 + constants.ODS_CkDiv[i])*(1 + constants.ODS_OdsEdgeTime[i])*1000000*8
            self.RampRateTable.append(RampRate)
            Error = abs(RampRate - self.DesiredRampTime)
            self.ErrorTable.append(Error)

        # ods_ck_div
        self.ods_ck_div   = CalcUtils.lookupMin(self.ErrorTable, constants.ODS_CkDiv)
        self.ods_edge_time = CalcUtils.lookupMin(self.ErrorTable, constants.ODS_OdsEdgeTime)

        # ods_group_width
        if self.inputs.SerializerSetup.Encoding == "NRZ+4b5b":
            self.ods_group_width = 4
        else:
            if self.inputs.SerializerSetup.Encoding == "Manchester":
                self.ods_group_width = 7
            else:
                if self.inputs.SerializerSetup.Encoding == "NRZ Only":
                    self.ods_group_width = 7
                else:
                    #if API_SS_Encoding == "Other":
                    self.ods_group_width = 7 # TODO Menus!$B$5


    def eval_result_in_calc(self):
        # Warm-up Targets
        # HARD CODE. TODO 
        PAWarmupTime   = 0.000001
        DivWarmupTime  = 0.000005
        OSCWarmupTime  = 0.000125

        # ActualSymbolRate
        if ((self.inputs.SerializerSetup.Encoding == "Manchester" and self.inputs.SerializerSetup.Modulation == "FSK" and self.inputs.SerializerSetup.Bit_Data_Rate > 62.5) or 
            (                                                self.inputs.SerializerSetup.Modulation == "FSK" and self.inputs.SerializerSetup.Bit_Data_Rate > 100)  or
            (self.inputs.SerializerSetup.Encoding == "NRZ+4b5b"   and self.inputs.SerializerSetup.Modulation == "OOK" and self.inputs.SerializerSetup.Bit_Data_Rate > 50)   or
            (self.inputs.SerializerSetup.Encoding == "NRZ Only"   and self.inputs.SerializerSetup.Modulation == "OOK" and self.inputs.SerializerSetup.Bit_Data_Rate > 50)   or
            (self.inputs.SerializerSetup.Encoding == "Manchester" and self.inputs.SerializerSetup.Modulation == "OOK" and self.inputs.SerializerSetup.Bit_Data_Rate > 25)   or 
            (round(24e6/self.TargetSymbolRate/1000/(1 + self.ods_ck_div)) > 23767)
            ):
            self.ActualSymbolRate = "Target Bit Rate Impossible"
        else:
            self.ActualSymbolRate = 24e6/round(24e6/self.TargetSymbolRate/1000/(1 + self.ods_ck_div))/1000/(self.ods_ck_div + 1)

        #
        # TODO handle IF(ActualSymbolRate="Target Bit Rate Impossible",
        #  "Calculation Error",
        self.ods_warm_pa  = math.ceil(PAWarmupTime  /(self.ods_ck_div + 1)*24000000 - 1e-5)
        self.ods_warm_div = math.ceil(DivWarmupTime /(self.ods_ck_div + 1)*24000000/4 - 1e-5)

        # ods_warm_osc
        #"IF(ActualSymbolRate=""Target Bit Rate Impossible"",
        #    ""Calculation Error"",
        #else
        #    IF(0.95/ActualSymbolRate/1000<=OSC Warmup Time,
        #        0,
        #    else
        #        CEILING(OSC_Warmup_Time/(ods_clk_div+1)*24000000/64,1)))"
        if self.ActualSymbolRate == "Target Bit Rate Impossible":
            self.ods_warm_osc = "Calculation Error"
        else:
            if (0.95/self.ActualSymbolRate/1000 <= OSCWarmupTime):
                self.ods_warm_osc = 0
            else:
                self.ods_warm_osc = math.ceil(OSCWarmupTime/(self.ods_ck_div + 1)*24e6/64 - 1e-5)

    def eval_api_paras(self):
        # IF(USR_IN_SS_Modulation="FSK",1,0)
        self.API_vOds_Setup__bModulationType = 1 if self.inputs.SerializerSetup.Modulation == "FSK" else 0
        self.API_vOds_Setup__bClkDiv = self.ods_ck_div        # $A$11
        self.API_vOds_Setup__bEdgeRate = self.ods_edge_time   # $A$13
        self.API_vOds_Setup__bGroupWidth = self.ods_group_width #$A$15
        # wBitRate
        if (round(24e6/self.TargetSymbolRate/1000/(1 + self.ods_ck_div)) > 32767 or self.ActualSymbolRate == "Target Bit Rate Impossible"):
            self.API_vOds_Setup__wBitRate = "Target Bit Rate Impossible"
        else:
            # In case of 62.5
            self.API_vOds_Setup__wBitRate = round(24e6/self.TargetSymbolRate/1000/(1 + self.ods_ck_div) + 1e-5)

        self.API_vOds_Setup__bLcWarmInt  = self.ods_warm_osc # $A$27
        self.API_vOds_Setup__bDivWarmInt = self.ods_warm_div # $A$25
        self.API_vOds_Setup__bPaWarmInt  = self.ods_warm_pa  # $A$23


    def getCalcResult(self):
        # return value
        return(self.ods_edge_time, # bEdgeRate
            self.API_vOds_Setup__wBitRate,         # wBitRate
            self.ods_warm_div,     # bDivWarmInt
            self.ods_ck_div,       # bClkDiv
            self.ods_group_width,  # bGroupWidth
            self.ods_warm_osc,     # bLcWarmInt
            self.ods_warm_pa       # bPaWarmInt
            )

    def getActualSymbolRate(self):
        return self.ActualSymbolRate

    def dump(self):
        print("\n Dump information of Class:{}".format(self.__class__.__name__))
        print("    ------------ Variables ------------")
        print("    TargetBitRate    :", self.TargetBitRate         )
        print("    ActualSymbolRate :", self.TargetSymbolRate      )
        print("    DesiredRampTime  :", self.DesiredRampTime       )
        print("                     :"                             )
        print("    ------------ ODS ------------")
        print("    ods_ck_div       :", self.ods_ck_div            )
        print("    ods_edge_time    :", self.ods_edge_time         )
        print("    ods_group_width  :", self.ods_group_width       )
        print("                     :"                             )
        print("    ods_warm_pa      :", self.ods_warm_pa           )
        print("    ods_warm_div     :", self.ods_warm_div          )
        print("    ods_warm_osc     :", self.ods_warm_osc          )
        print("                     :"                             )
        print("    ------------ Tables ------------")
        print("    ErrorTable       :", self.ErrorTable[:4])
        print("    RampRateTable    :", self.RampRateTable[:4])
        print("                     :"                             )
        print("    ------------ Calculator Result ------------")
        print("    ------------ API Parameters ------------")
        print("    API:vOds_Setup   :")
        print("         bModulationType: {}".format(self.API_vOds_Setup__bModulationType))
        print("         bClkDiv:         {}".format(self.API_vOds_Setup__bClkDiv))
        print("         bEdgeRate:       {}".format(self.API_vOds_Setup__bEdgeRate))
        print("         bGroupWidth:     {}".format(self.API_vOds_Setup__bGroupWidth))
        print("         wBitRate:        {}".format(self.API_vOds_Setup__wBitRate))
        print("         bLcWarmInt:      {}".format(self.API_vOds_Setup__bLcWarmInt))
        print("         bDivWarmInt:     {}".format(self.API_vOds_Setup__bDivWarmInt))
        print("         bPaWarmInt:      {}".format(self.API_vOds_Setup__bPaWarmInt))


