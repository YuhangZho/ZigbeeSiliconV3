'''
Created on Sept 01, 2013

@author: shyang
'''
import si4010_modem_calc
import copy

__all__ = ["Si4010CfgCalcMiscSetup"]

class Si4010CfgCalcMiscSetup(object):
    '''
    classdocs
    '''
    LEDIntensityTypes = ['Off' , '0.37 mA' , '0.60 mA' , '0.97 mA']
    ExtCrystalPara    = [1, 10000000, 0]
    RefClockTypeTable = {'Internal Ref Clock' : [0, 10000000, 0],
                         'External Crystal'   : ExtCrystalPara,
                         'External Si501'     : [2, 10000000, 1]}

    def __init__(self, inputs):
        '''
        Constructor
        '''
        self.cfg = {}
        self.cfg_PQ_file = {}

        # LED
        if not inputs.misc.LEDIntensity in self.LEDIntensityTypes:
            inputs.misc.LEDIntensity = self.LEDIntensityTypes[1]
        self.LEDIntensity = inputs.misc.LEDIntensity
        self.cfg['bLedIntensity'] = self.LEDIntensityTypes.index(self.LEDIntensity)

        if not inputs.misc.PeriodicLEDIntensity in self.LEDIntensityTypes:
            inputs.misc.PeriodicLEDIntensity = self.LEDIntensityTypes[1]  # TODO ?
        self.PeriodicLEDIntensity = inputs.misc.PeriodicLEDIntensity
        self.cfg['bPTLedIntensity'] = self.LEDIntensityTypes.index(self.PeriodicLEDIntensity)


        # Battery
        # TODO check validation
        if inputs.misc.BattThreshToDisLED < 1500:
            inputs.misc.BattThreshToDisLED = 1500
        if inputs.misc.BattThreshToDisLED > 3600:
            inputs.misc.BattThreshToDisLED = 3600
        self.BatThresholdToDisLED = int(round((inputs.misc.BattThreshToDisLED   - 1500 + 1e-5)/10, 0))
        self.cfg['bBatThreshold'] = self.BatThresholdToDisLED
        # Battery for No TX threshold
        if inputs.misc.BattThreshToDisTrans < 1500:
            inputs.misc.BattThreshToDisTrans = 1500
        if inputs.misc.BattThreshToDisTrans > 3600:
            inputs.misc.BattThreshToDisTrans = 3600
        self.BattThreshToDisTrans = inputs.misc.BattThreshToDisTrans
        self.cfg['wBatNoTX']      = self.BattThreshToDisTrans


        # TemperatureThreshold to Use Alternate PA settings
        self.LowTempThreshold = inputs.misc.TempThreshToUsAlterPA
        self.cfg['iLowTempThreshold'] = self.LowTempThreshold
        self.calc_alterPA(inputs)


        # Reference Clock
        if not inputs.misc.RefClockType in self.RefClockTypeTable.keys():
            inputs.misc.RefClockType = list(RefClockTypeTable.keys())[0]
        # TODO Restriction
        self.ExtCrystalPara[1] = inputs.misc.ExtCrystalFreq
        self.ExtCrystalPara[2] = inputs.misc.ExtCrystalLowCap
        self.RefClockType      = inputs.misc.RefClockType

        xo = self.RefClockTypeTable[self.RefClockType]
        self.cfg['bUseExternalXo'], self.cfg['rFCast_XoSetup'] = xo[0], [xo[1], xo[2]]

        self.gen_reserved_bytes()


    # TODO use what parameters?
    def calc_alterPA(self, inputs):
        #paras  =   [0,      # PaketType
        #            # --------  Antenna Setup  --------
        #            #Alpha    ApproxEfficiency    ReactanceChange(%)     ManualImpedanceEntry    AntennaRealZ    AntennaImagZ
        #            4.2,     7.7,                40,                   'No',                    100,            40,
        #            # --------  Power Amplifier Setup  --------
        #            #TotalPAPower(dBm)    PAType'                         CenterFrequency(MHZ)    NominalCap    ExternalDiffCap    Q_FactorOfExtCap
        #            0,                    'Min PA & Max Radiated Power',  315.000,                30,           1,                 300,
        #            # --------  Serializer Setup  --------
        #            #DataRate(kbits/s)    Encoding      Modulation    FSK_Deviation(kHZ)    ManualRampRateEntry    TargetRampRate
        #            48,                   'NRZ Only',   'FSK',        43.2,                 'No',                  12]

        #self.API_PAS_Total_PA_Power        = float(input_data[7])  # dBm
        #self.API_PAS_Drive_Type            = input_data[8]  # "Max Radiated Power" or "Min PA & Max Radiated Power"
        inputs.Packet0.modem.API_PAS_Total_PA_Power = inputs.misc.AlterPAOutPower
        inputs.Packet0.modem.API_PAS_Drive_Type = inputs.misc.AlterPAType

        # TODO change other parameters to packet1~3?
        modem_inputs = copy.deepcopy(inputs.Packet0.modem)
        modem  = si4010_modem_calc.Si4010ApiCalc(modem_inputs)
        modem.calculate()

        self.api_settings = modem.get_api_list() # api_settings
        self.cfg['rAlternatePaSetup']     = [self.api_settings['fAlpha'], self.api_settings['fBeta'], self.api_settings['bLevel'], self.api_settings['bMaxDrv'], self.api_settings['wNominalCap']]


    def gen_reserved_bytes(self):
        self.cfg['bReserve0'] = 0
        self.cfg['bReserve1'] = 0
        self.cfg['bReserve2'] = 0
        self.cfg['bReserve3'] = 0


    def get_cfg_data(self):
        return self.cfg

    def dump(self):
        print('    -------------  configuration data for TX C file  -------------')
        for m in self.cfg:
            print('        {}  =  {}'.format(m, self.cfg[m]))

        print('    -------------  configuration data for PQ C file  -------------')
        for m in self.cfg_PQ_file:
            print('        {}  =  {}'.format(m, self.cfg_PQ_file[m]))


