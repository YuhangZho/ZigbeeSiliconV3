'''
Created on JUly 25, 2013

@author: shyang
'''

from common import InputInterface
from common import CategoryInterface

__all__ = ["Si4010ApiCalcInputs"]

default_inputs = [0,      # PaketType
                  # --------  Antenna Setup  --------
                  #Alpha    ApproxEfficiency    ReactanceChange(%)     ManualImpedanceEntry    AntennaRealZ    AntennaImagZ
                  4.2,     7.7,                40,                   False,                    100,            40,
                  # --------  Power Amplifier Setup  --------
                  #TotalPAPower(dBm)    PAType'                         CenterFrequency(MHZ)    NominalCap    ExternalDiffCap    Q_FactorOfExtCap
                  0,                    'Min PA & Max Radiated Power',  315.000,                30,           1,                 300,
                  # --------  Serializer Setup  --------
                  #DataRate(kbits/s)    Encoding      Modulation    FSK_Deviation(kHZ)    ManualRampRateEntry    TargetRampRate
                  48,                   'NRZ Only',   'FSK',        43.2,                 False,                  12
                  ]

class General(CategoryInterface):
    def __init__(self, input_data):
        super(General, self).__init__()
        self.Package = float(input_data) # 0 (0) or 1 (1)
        
class AntennaSetup(CategoryInterface):
    def __init__(self, input_data):
        super(AntennaSetup, self).__init__()
        self.Alpha                  = float(input_data[0])  # bLevel/deg C
        self.Approx_Efficiency      = float(input_data[1])  # 
        self.Reactance_change       = float(input_data[2])  # 
        self.Manual_Impedance_Entry = input_data[3]  # 
        self.Antenna_Real_Z         = float(input_data[4])  # Ohms
        self.Antenna_Imag_Z         = float(input_data[5])  # Ohms
        
class PowerAmplifierSetup(CategoryInterface):
    def __init__(self, input_data):
        super(PowerAmplifierSetup, self).__init__()
        self.Total_PA_Power        = float(input_data[0])  # dBm
        self.Drive_Type            = input_data[1]  # "Max Radiated Power" or "Min PA & Max Radiated Power"
        self.Center_Frequency      = float(input_data[2])  # MHz
        self.Nominal_Cap_Word      = float(input_data[3])  #
        self.External_Diff_Cap     = float(input_data[4])  # 
        self.Q_Factor_of_Ext_Cap   = float(input_data[5])  # 

class SerializerSetup(CategoryInterface):
    def __init__(self, input_data):
        super(SerializerSetup, self).__init__()
        self.Bit_Data_Rate          = float(input_data[0])  # 
        self.Encoding               = input_data[1]  # 
        self.Modulation             = input_data[2]  # 
        self.FSK_Deviation          = float(input_data[3])  # 
        self.Manual_Ramp_Rate_Entry = input_data[4]  #  "Yes"/"No"
        self.Target_Ramp_Rate       = float(input_data[5])  #

class Si4010ApiCalcInputs(InputInterface):
    '''
    classdocs
    '''
    def _init_from_input(self, input_data):
        self.General = General(input_data[0])  # Package Type
        # Antenna Setup
        self.AntennaSetup = AntennaSetup(input_data[1:7])
        # Power Amplifier Setup
        self.PowerAmplifierSetup = PowerAmplifierSetup(input_data[7:13])
        # Serializer Setup
        self.SerializerSetup = SerializerSetup(input_data[13:])

    def __init__(self, inputarray=None):
        if inputarray == None:
            input_data = default_inputs
        else:
            input_data = inputarray
        self._init_from_input(input_data)

    def dump(self):
        print("API_Package                             :",  self.API_Package                      )
        print("-------------  Antenna Setup -------------  ")
        print("API_AS_Alpha                            :",  self.API_AS_Alpha                     )
        print("API_AS_Approx_Efficiency                :",  self.API_AS_Approx_Efficiency         )
        print("API_AS_Reactance_change                 :",  self.API_AS_Reactance_change          )
        print("API_AS_Manual_Impedance_Entry           :",  self.API_AS_Manual_Impedance_Entry    )
        print("API_AS_Antenna_Real_Z                   :",  self.API_AS_Antenna_Real_Z            )
        print("API_AS_Antenna_Imag_Z                   :",  self.API_AS_Antenna_Imag_Z            )
        print("-------------  Power Amplifier Setup -------------  ")
        print("API_PAS_Total_PA_Power                  :",  self.API_PAS_Total_PA_Power           )
        print("API_PAS_Drive_Type                      :",  self.API_PAS_Drive_Type               )
        print("API_PAS_Center_Frequency                :",  self.API_PAS_Center_Frequency         )
        print("API_PAS_Nominal_Cap_Word                :",  self.API_PAS_Nominal_Cap_Word         )
        print("API_PAS_External_Diff_Cap               :",  self.API_PAS_External_Diff_Cap        )
        print("API_PAS_Q_Factor_of_Ext_Cap             :",  self.API_PAS_Q_Factor_of_Ext_Cap      )
        print("-------------  Serializer Setup -------------  ")
        print("API_SS_Bit_Data_Rate                    :",  self.API_SS_Bit_Data_Rate             )
        print("API_SS_Encoding                         :",  self.API_SS_Encoding                  )
        print("API_SS_FSK_Deviation                    :",  self.API_SS_FSK_Deviation             )
        print("API_SS_Manual_Ramp_Rate_Entry           :",  self.API_SS_Manual_Ramp_Rate_Entry    )
        print("API_SS_Target_Ramp_Rate                 :",  self.API_SS_Target_Ramp_Rate          )
