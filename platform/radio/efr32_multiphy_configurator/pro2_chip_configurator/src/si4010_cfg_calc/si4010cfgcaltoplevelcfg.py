'''
Created on Sept 02, 2013

@author: shyang
'''

__all__ = ["Si4010CfgCalcTopLevelCfg"]

class Si4010CfgCalcTopLevelCfg(object):
    '''
    classdocs
    '''
    def __init__(self, inputs):
        self.cfg = {'bConfigSetting' : -1}
        self.cfg_PQ_file = {}

        B = [0]*8
        B[0] = int(inputs.topLevelSetup.AltPAForPerodicTransmit)
        B[1] = int(inputs.topLevelSetup.EnPeriodicTransmit)*2
        # B2 Reserved(set to 0)
        B[3] = int(inputs.topLevelSetup.EnMultiCfgOTP)*8
        B[4] = int(inputs.topLevelSetup.EnStuckButton)*16
        B[5] = int(inputs.topLevelSetup.GP9DrivenLOWforADCMeasure)*32
        B[6] = int(inputs.topLevelSetup.EnThermostat)*64
        B[7] = int(inputs.topLevelSetup.EnTestMode)*128
        self.cfg['bConfigSetting'] = sum(B)


        self.cfg['bUserData'] = inputs.topLevelSetup.OTPcfgNum + inputs.topLevelSetup.DataInTwoBytesPayload*16
        self.cfg['bUserPayloadData0']    = inputs.topLevelSetup.UserPayloadData0
        self.cfg['bUserPayloadData1']    = inputs.topLevelSetup.UserPayloadData1
        
        self.cfg['iAdcOrTempThresholdH']  = inputs.topLevelSetup.AdcOrTempThresholdH
        self.cfg['iAdcOrTempThresholdL']  = inputs.topLevelSetup.AdcOrTempThresholdL

        # Now the range of the input was not checked, depends on the
        self.cfg['bAdcHoldOff']  =  inputs.topLevelSetup.AdcHoldOff//85 + (inputs.topLevelSetup.SmokeAlarmHoldOff//85)*16

    def get_cfg_data(self):
        return self.cfg


    def dump(self):
        print('    -------------  configuration data  -------------')
        for m in self.cfg:
            value = self.cfg[m]
            if type(value) == type(10):
                value = '0x' + format(self.cfg[m], 'X')
            print('        {}  =  {}'.format(m, value))
















































