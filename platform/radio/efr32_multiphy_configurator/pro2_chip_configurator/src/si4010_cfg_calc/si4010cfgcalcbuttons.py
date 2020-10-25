'''
Created on Sept 01, 2013

@author: shyang
'''
import copy

__all__ = ["Si4010CfgCalcButtons"]

class Si4010CfgCalcButtons(object):
    '''
    classdocs
    '''

    def __init__(self, inputs):
        '''
        Constructor
        '''
        self.cfg = {}
        self.cfg_PQ_file = {}

        self.PushQualThreshTime           = inputs.buttons.PushQualThreshTime
        self.WaitTimeBeforeShutDownMargin = inputs.buttons.WaitTimeBeforeShutDownMargin
        self.MultiButtonPressTimeDiff     = inputs.buttons.MultiButtonPressTimeDiff
        self.ExpectedChatterTimeofButtons = inputs.buttons.ExpectedChatterTimeofButtons
        self.calc_ValidButton(inputs)
        self.calc_bButtonMask()   # call this after self.ValidButtons is set
        self.calc_Times(inputs)


    def calc_ValidButton(self, inputs):
        # values for TX C file
        self.cfg['bValidButtons[23]']         = copy.deepcopy(inputs.buttons.ValidButtons)  # length must be 23
        self.cfg_PQ_file['bValidButtons[23]'] = copy.deepcopy(inputs.buttons.ValidButtons)
        self.ValidButtons                     = inputs.buttons.ValidButtons   # Read only
        # todo gen comments for the purpose of different keys

        # values for PQ C file
        keys = list(range(len(self.ValidButtons)))
        # Button[0]
        if self.ValidButtons[0] == 0:
            self.cfg_PQ_file['bValidButtons[23]'][0]  = self.ValidButtons[16]    # Associaton(1)
        else:
            self.cfg_PQ_file['bValidButtons[23]'][0]  = self.ValidButtons[0]
        keys.remove(0)

        # Button[14,15,16]
        self.cfg_PQ_file['bValidButtons[23]'][14] = self.ValidButtons[14]    # Associaton(1)
        self.cfg_PQ_file['bValidButtons[23]'][15] = self.ValidButtons[15]    # Associaton(2)
        self.cfg_PQ_file['bValidButtons[23]'][16] = self.ValidButtons[16]    # Associaton(3)
        keys.remove(14)
        keys.remove(15)        
        keys.remove(16)

        # Process the remaining keys
        for i in keys:
            if self.ValidButtons[i] == 0:
                self.cfg_PQ_file['bValidButtons[23]'][i] = self.ValidButtons[16]  # Why Associaton(3) ?
            else:
                self.cfg_PQ_file['bValidButtons[23]'][i] = self.ValidButtons[i]


    def calc_bButtonMask(self):
        mask = 0
        for k in self.ValidButtons:
            mask = mask | k
        self.ButtonMask = mask
        self.cfg['bButtonMask'] = mask

    def calc_Times(self, inputs):
        self.cfg['bPushQualThresh'] = self.PushQualThreshTime//5
        inputs.buttons.PushQualThreshTime   = self.cfg['bPushQualThresh']*5
        wWaitForPush                = inputs.buttons.ExpectedChatterTimeofButtons + inputs.buttons.PushQualThreshTime + inputs.buttons.MultiButtonPressTimeDiff + inputs.buttons.WaitTimeBeforeShutDownMargin
        if wWaitForPush > 2000:
            inputs.buttons.ExpectedChatterTimeofButtons = (2000 - inputs.buttons.PushQualThreshTime)//3
            inputs.buttons.PushQualThreshTime           = (2000 - inputs.buttons.PushQualThreshTime)//3
            inputs.buttons.MultiButtonPressTimeDiff     = (2000 - inputs.buttons.PushQualThreshTime)//3
            inputs.buttons.WaitTimeBeforeShutDownMargin = 2000 - (inputs.buttons.PushQualThreshTime + inputs.buttons.ExpectedChatterTimeofButtons + inputs.buttons.PushQualThreshTime + inputs.buttons.MultiButtonPressTimeDiff)
            self.cfg['wWaitForPush'] = 2000
        else:
            self.cfg['wWaitForPush'] = wWaitForPush

            
    def get_cfg_data(self):
        return self.cfg


    def dump(self):
        print('    -------------  configuration data for TX C file  -------------')
        for m in self.cfg:
            print('        {}  =  {}'.format(m, self.cfg[m]))

        print('    -------------  configuration data for PQ C file  -------------')
        for m in self.cfg_PQ_file:
            print('        {}  =  {}'.format(m, self.cfg_PQ_file[m]))


