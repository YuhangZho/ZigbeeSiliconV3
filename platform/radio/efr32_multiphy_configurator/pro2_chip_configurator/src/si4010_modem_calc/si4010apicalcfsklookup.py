'''
Created on July 25, 2013

@author: shyang
'''
from .si4010apicalcconstants  import constants


# TODO DEL when release
#from imp import reload
#reload(Si4010ApiCalcInputs)
#reload(si4010apicalcconstants)



__all__ = ["Si4010ApiCalcFSKLookup"]

class Si4010ApiCalcFSKLookup(object):
    '''
    classdocs
    '''
    Desired_FSK_Code = 0
    Projected_FSK_Deviation = 0
    Error_Table = []

    # Internal vars, for debug
    min_error       = -1
    min_error_FSK_Code = -1  

    CR_SC_Expected_Deviation   = -1
    # API command parameter generated in this class
    API_vFCast_FskAdj_biFskDev = -1
    API_vFCast_Tune__fiDesiredFreq  = -1

    def __init__(self, inputs):
        '''
        Constructor
        '''
        self.inputs = inputs
        self.original_inputs = self.inputs
        self.init_tables()
        self.evaluate()
        self.eval_api_paras()
        #self.dump(4) # TODO DEL WHEN RELEASE
        return

    def init_tables(self):
        self.Error_Table = []
        for m in constants.FSKLookup_PPMShift:
            error = abs(self.inputs.SerializerSetup.FSK_Deviation*2.0/self.inputs.PowerAmplifierSetup.Center_Frequency*1000 - m)

            self.Error_Table.append(error)


    def evaluate(self):
        self.min_error = min(self.Error_Table)
        self.min_error_FSK_Code = self.Error_Table.index(self.min_error)

        # Desired FSK Code and Projected FSK Deviation
        if self.min_error_FSK_Code >= 104:  # HARD CODE
            self.Desired_FSK_Code       = 104
            self.Projected_FSK_Deviation = constants.FSKLookup_PPMShift[104]  # TODO why this element
        else:
            self.Desired_FSK_Code       = self.min_error_FSK_Code
            self.Projected_FSK_Deviation = constants.FSKLookup_PPMShift[self.Desired_FSK_Code]

    def eval_api_paras(self):
        #IF(USR_IN_SS_Modulation="FSK",IF('FSK Lookup'!Desired_FSK_Code>104,104,'FSK Lookup'!Desired_FSK_Code),0)
        #"IF(USR_IN_SS_Modulation=""FSK"",
        #    IF('FSK Lookup'!Desired_FSK_Code>104,
        #        104,
        #    ELSE
        #         'FSK Lookup'!Desired_FSK_Code),
        #ELSE
        #    0)"
        #
        # Desired_FSK_Code has been restirct to <= 104
        self.API_vFCast_FskAdj_biFskDev = self.Desired_FSK_Code if self.inputs.SerializerSetup.Modulation == 'FSK' else 0
        # -------  Expected_Deviation  ----------
        #=IF(CALCULATOR!$C$32="FSK",'FSK Lookup'!$E$2*CALCULATOR!$C$23/1000/2,"N/A")
        #=IF(USR_IN_SS_Modulation="FSK",'FSK Lookup'!$E$2*USR_IN_PA_Center_Frequency/1000/2,"N/A")
        #=IF(USR_IN_SS_Modulation="FSK",'FSK Lookup'!Projected_FSK_Deviation*USR_IN_PA_Center_Frequency/1000/2,"N/A")
        # -------  fiDesiredFreq  ----------
        #=IF($C$32="OOK",$C$23*1000000,$C$23*1000000+$H$36*1000)
        #=IF(CALCULATOR!$C$32="OOK",CALCULATOR!$C$23*1000000,CALCULATOR!$C$23*1000000+CALCULATOR!$H$36*1000)
        #=IF(USR_IN_SS_Modulation="OOK",USR_IN_PA_Center_Frequency*1000000,USR_IN_PA_Center_Frequency*1000000+CR_SC_ExpectedDeviation*1000)

        self.API_vFCast_Tune__fiDesiredFreq = self.inputs.PowerAmplifierSetup.Center_Frequency*1e6
        if self.inputs.SerializerSetup.Modulation == "FSK":
            self.CR_SC_Expected_Deviation =  self.Projected_FSK_Deviation*self.inputs.PowerAmplifierSetup.Center_Frequency/1000/2
            self.API_vFCast_Tune__fiDesiredFreq = self.API_vFCast_Tune__fiDesiredFreq + self.CR_SC_Expected_Deviation*1e3
        else: #OOK
            self.CR_SC_Expected_Deviation = "N/A"



    def dump (self, l = 65535):
        l = min(l, len(self.Error_Table))
        print("\n Dump information of Class:{}".format(self.__class__.__name__))
        print("    ------------ Internal Variables ------------")
        print("    Min error                :",                    self.min_error)
        print("    Min error in Table       :",                    self.min_error_FSK_Code)
        print("    ------------ Tables ------------")
        print("    Error Table 0 to {}      :\n{}\n".format(l - 1, self.Error_Table[:l]))
        print("    ------------ Variables ------------")
        print("    Desired FSK Code         :",        self.Desired_FSK_Code)
        print("    Projected FSK Deviation  :",        self.Projected_FSK_Deviation)
        print("    ------------ Calculator Result ------------")
        print("    ------------     Chip Impedance ------------")
        print("    CR_SC_Expected Deviation     :", self.CR_SC_Expected_Deviation)
        print("    ------------ API Parameters ------------")
        print("    API:vFCast_FskAdj        :\n        biFskDev:      {}".format(self.API_vFCast_FskAdj_biFskDev))
        print("    API:vFCast_Tuen          :\n        fiDesiredFreq: {}".format(self.API_vFCast_Tune__fiDesiredFreq))


