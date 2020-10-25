'''
Created on August 25, 2013

@author: shyang
'''
import copy

__all__ = ["Si4010CfgCalcSleepTimerSampleRate"]

class Si4010CfgCalcSleepTimerSampleRate(object):
    '''
    classdocs
    '''
    data_table = [
        {'Sleep Interval'  : 3600.0/8192,   'wSleepTimerDivide'  :  8192  ,    'lSleepCycleMask'  :  0x00000300 ,    'wSleepCycleMask'  :  0x0001,    'bLoopIncrement'  :  1,    'bSleepSubSample' : 0x1F },
        {'Sleep Interval'  : 3600.0/4096,   'wSleepTimerDivide'  :  4096  ,    'lSleepCycleMask'  :  0x00000600 ,    'wSleepCycleMask'  :  0x0003,    'bLoopIncrement'  :  1,    'bSleepSubSample' : 0x0F },
        {'Sleep Interval'  : 3600.0/2048,   'wSleepTimerDivide'  :  2048  ,    'lSleepCycleMask'  :  0x00000C00 ,    'wSleepCycleMask'  :  0x0007,    'bLoopIncrement'  :  1,    'bSleepSubSample' : 0x07 },
        {'Sleep Interval'  : 3600.0/1024,   'wSleepTimerDivide'  :  1024  ,    'lSleepCycleMask'  :  0x00001800 ,    'wSleepCycleMask'  :  0x000F,    'bLoopIncrement'  :  1,    'bSleepSubSample' : 0x03 },
        {'Sleep Interval'  : 3600.0/512 ,   'wSleepTimerDivide'  :  512   ,    'lSleepCycleMask'  :  0x00003000 ,    'wSleepCycleMask'  :  0x001F,    'bLoopIncrement'  :  1,    'bSleepSubSample' : 0x01 },
        {'Sleep Interval'  : 3600.0/256 ,   'wSleepTimerDivide'  :  256   ,    'lSleepCycleMask'  :  0x00006000 ,    'wSleepCycleMask'  :  0x003F,    'bLoopIncrement'  :  1,    'bSleepSubSample' : 0x01 },
        {'Sleep Interval'  : 3600.0/128 ,   'wSleepTimerDivide'  :  128   ,    'lSleepCycleMask'  :  0x0000E000 ,    'wSleepCycleMask'  :  0x007F,    'bLoopIncrement'  :  2,    'bSleepSubSample' : 0x01 },
        {'Sleep Interval'  : 3600.0/64  ,   'wSleepTimerDivide'  :  64    ,    'lSleepCycleMask'  :  0x0001E000 ,    'wSleepCycleMask'  :  0x00FF,    'bLoopIncrement'  :  4,    'bSleepSubSample' : 0x01 },
        {'Sleep Interval'  : 3600.0/32  ,   'wSleepTimerDivide'  :  32    ,    'lSleepCycleMask'  :  0x0003E000 ,    'wSleepCycleMask'  :  0x01FF,    'bLoopIncrement'  :  8,    'bSleepSubSample' : 0x01 },
        {'Sleep Interval'  : 3600.0/16  ,   'wSleepTimerDivide'  :  16    ,    'lSleepCycleMask'  :  0x0007E000 ,    'wSleepCycleMask'  :  0x03FF,    'bLoopIncrement'  : 16,    'bSleepSubSample' : 0x01 },
        {'Sleep Interval'  : 3600.0/8   ,   'wSleepTimerDivide'  :  8     ,    'lSleepCycleMask'  :  0x000FE000 ,    'wSleepCycleMask'  :  0x07FF,    'bLoopIncrement'  : 32,    'bSleepSubSample' : 0x01 },
        {'Sleep Interval'  : 3600.0/4   ,   'wSleepTimerDivide'  :  4     ,    'lSleepCycleMask'  :  0x001FE000 ,    'wSleepCycleMask'  :  0x0FFF,    'bLoopIncrement'  : 64,    'bSleepSubSample' : 0x01 },
        {'Sleep Interval'  : 3600.0/2   ,   'wSleepTimerDivide'  :  2     ,    'lSleepCycleMask'  :  0x003FE000 ,    'wSleepCycleMask'  :  0x1FFF,    'bLoopIncrement'  :128,    'bSleepSubSample' : 0x01 },
        {'Sleep Interval'  : 3600.0/1   ,   'wSleepTimerDivide'  :  1     ,    'lSleepCycleMask'  :  0x007FE000 ,    'wSleepCycleMask'  :  0x3FFF,    'bLoopIncrement'  :  0,    'bSleepSubSample' : 0x01 },
        ]
    # Packet Qualifier Time Stamp Windows
    # RKE Mode
    
    PQ_TS_Windows_RKE = [
        # PrimaryWindow(32X)    Resync Limit(512X)     wResyncWindow(1.2*14Sec)
        [0x00003FFF,	        0x0003FFFF,        	   0x004B],
        [0x00003FFF,	        0x0003FFFF,        	   0x004B],
        [0x00003FFF,	        0x0003FFFF,        	   0x004B],
        [0x00003FFF,	        0x0003FFFF,        	   0x004B],
        [0x00003FFF,	        0x0003FFFF,        	   0x004B],
        [0x00003FFF,	        0x0003FFFF,        	   0x004B],
        [0x00003FFF,	        0x0003FFFF,        	   0x004B],
        [0x00003FFF,	        0x0003FFFF,        	   0x004B],
        [0x00003FFF,	        0x0003FFFF,        	   0x004B],
        [0x00007FFF,	        0x0007FFFF,        	   0x004B],
        [0x0000FFFF,	        0x000FFFFF,        	   0x004B],
        [0x0001FFFF,	        0x001FFFFF,        	   0x004B],
        [0x0003FFFF,	        0x003FFFFF,        	   0x004B],
        [0x0007FFFF,	        0x007FFFFF,        	   0x004B],
        ]
    # Periodic Mode
    PQ_TS_Windows_PeriodicMode = [
        # PrimaryWindow(32X)    Resync Limit(512X)     wResyncWindow(1.2*14Sec)
        [0x0029FFFF,        	0x0221FFFF,            0x004B],
        [0x0029FFFF,        	0x0221FFFF,            0x004B],
        [0x0029FFFF,        	0x0221FFFF,            0x004B],
        [0x0029FFFF,        	0x0221FFFF,            0x004B],
        [0x0029FFFF,        	0x0221FFFF,            0x004B],
        [0x0029FFFF,        	0x0221FFFF,            0x004B],
        [0x0029FFFF,        	0x0221FFFF,            0x0098],
        [0x0029FFFF,        	0x0221FFFF,            0x0132],
        [0x0029FFFF,        	0x0221FFFF,            0x0265],
        [0x0029FFFF,        	0x0221FFFF,            0x04CB],
        [0x0029FFFF,        	0x0221FFFF,            0x0998],
        [0x0029FFFF,        	0x0221FFFF,            0x1332],
        [0x0029FFFF,        	0x0221FFFF,            0x2665],
        [0x0029FFFF,        	0x0221FFFF,            0x4CCB],
        ]

    sleep_timer_info = {}  # The requested sleep timer information. Has more items than configuration data

    def __init__(self, inputs):
        self.sleep_timer_info = {}  # The requested sleep timer information. Has more items than configuration data
        self.cfg = {}
        self.cfg_PQ_file = {}
        pos = self.find_pos_in_table(inputs.timer.SampleInterval)
        if type(pos) != type(3):
            raise RuntimeError(pos)

        self.index_of_selected_slp_timer = pos
        self.sleep_timer_info = copy.deepcopy(self.data_table[pos])
        self.re_calc_SleepTimerDivide(inputs)
        self.calc_bSleepSubSample(inputs)
        self.calc_cfg()
        self.calc_calc_cfg_PQ_file(inputs)


    def re_calc_SleepTimerDivide(self, inputs):
        expected_interval = inputs.timer.SampleInterval

        #if abs(self.sleep_timer_info['Sleep Interval'] - expected_interval) > 1e-3: 
        # Calculator the divider anyway
        divider = int(round(3600.0/expected_interval + 1e-5, 0))
            
        # Update the divider and interval
        self.sleep_timer_info['wSleepTimerDivide']  = divider
        self.sleep_timer_info['Sleep Interval']     = 3600.0/divider  # Seconds
        # Update the input parameters
        inputs.timer.SampleInterval          = self.sleep_timer_info['Sleep Interval']


    def calc_bSleepSubSample(self, inputs):
        if inputs.timer.AutoTxRate == 0: # TX every sample
            self.sleep_timer_info['bSleepSubSample'] = 0
        elif inputs.timer.AutoTxRate < 0: # Skip all Only TX threshold crossing
            inputs.timer.AutoTxRate    = -1
            self.sleep_timer_info['bSleepSubSample'] = 0xFF
        elif inputs.timer.AutoTxRate < 14:
            inputs.timer.AutoTxRate    = 14
        elif inputs.timer.AutoTxRate > 3600:
            inputs.timer.AutoTxRate    = 3600


        # Reg-calcuate the acutal Tx Rate (in bSleepSubSample) if it is a real "Auto TX"
        if inputs.timer.AutoTxRate >= 14:
            SubSamples =  int(round(inputs.timer.AutoTxRate/self.sleep_timer_info['Sleep Interval']  + 1e-5, 0))
            if SubSamples > 0:
                SubSamples = SubSamples - 1

            if SubSamples < self.sleep_timer_info['bSleepSubSample']:
                self.sleep_timer_info['bSleepSubSample'] = SubSamples
            # Re-calcualte the actual Tx Rate
            inputs.timer.AutoTxRate = self.sleep_timer_info['Sleep Interval']*(1 + self.sleep_timer_info['bSleepSubSample'])



    def find_pos_in_table(self, interval):
        min_interval = self.data_table[0]['Sleep Interval']
        max_interval = self.data_table[len(self.data_table) -1]['Sleep Interval']

        if interval > max_interval or interval < min_interval:
            return "Interval is out of range"

        pos = 0
        for m in self.data_table:
            if m['Sleep Interval'] < interval:
                pos += 1
                continue
            else:
                break
        return pos

    def calc_cfg(self):
        self.cfg = copy.deepcopy(self.sleep_timer_info)
        del(self.cfg['Sleep Interval'])

    def calc_calc_cfg_PQ_file(self, inputs):
        indx = self.index_of_selected_slp_timer  # Index for choosing Packet Qualifier Time Stamp Window
        if (inputs.topLevelSetup.EnPeriodicTransmit == 0):
            vars = self.PQ_TS_Windows_RKE[indx]             # Use settings for RKE mode
        else:
            vars = self.PQ_TS_Windows_PeriodicMode[indx]    # Use settings for Periodic Mode
        self.cfg_PQ_file['lPrimaryWindow'] = vars[0]
        self.cfg_PQ_file['lResyncLimit']   = vars[1]
        self.cfg_PQ_file['wResyncWindow']  = vars[2]




    def get_table(self, index):
        return self.data_table[index]

    def get_cfg_data(self):
        return self.sleep_timer_info

    def dump(self):
        print('    -------------  configuration data  -------------')
        for m in self.sleep_timer_info:
            value = self.sleep_timer_info[m]
            if type(value) == type(10):
                value = '0x' + format(self.sleep_timer_info[m], 'X')
            print('        {}  =  {}'.format(m, value))
















































