'''
Created on August 25, 2013

@author: shyang
'''
import math

__all__ = ["Si4010CfgCalcPacketSetup"]

class Si4010CfgCalcPacketSetup(object):
    '''
    classdocs
    '''
    StartupTime = 10 #ms
    # TODO check the if the buttons are valid
    # TODO check the validity of the threshold
    def __init__(self, inputs, PacketTransTimes):
        self.cfg = {}
        self.cfg_PQ_file = {}
        self.update_bPacket(inputs.packetSetup.PacketBD, inputs.packetSetup.PacketTX)
        self.calc_PacketsPerInterval(inputs)
        self.calc_latency(inputs, PacketTransTimes)
        self.calc_interval_table(inputs)


    def calc_PacketsPerInterval(self, inputs):
        self.PacketsPerInterval = (inputs.packetSetup.Packet0Enabled + \
                                   inputs.packetSetup.Packet1Enabled*2 + \
                                   inputs.packetSetup.Packet2Enabled*4 + \
                                   inputs.packetSetup.Packet3Enabled*8)<<4
        self.cfg['bPacketsPerInterval'] = self.PacketsPerInterval

    def calc_latency(self, inputs, PacketTransTimes):
        # Packet Latency (ms)
        self.Packet0Latency = PacketTransTimes[0] if inputs.packetSetup.Packet0Enabled else 0
        self.Packet1Latency = PacketTransTimes[1] if inputs.packetSetup.Packet1Enabled else 0
        self.Packet2Latency = PacketTransTimes[2] if inputs.packetSetup.Packet2Enabled else 0
        self.Packet3Latency = PacketTransTimes[3] if inputs.packetSetup.Packet3Enabled else 0

        # LED On Time(ms)
        self.LEDonTime  = inputs.packetSetup.LEDonTime
        self.cfg['bLEDOnTime'] = int(round(inputs.packetSetup.LEDonTime/5.0 + 1e-8, 0))
        
        # Additional Delay between packets in same interval(ms)
        self.AddiProgLatency = inputs.packetSetup.AddiDelayBetweenPack
        self.cfg['bPacketDelay'] = int(round(inputs.packetSetup.AddiDelayBetweenPack/5.0 + 1e-8, 0))

        self.calc_MinRepeatIntervalforUniformSpacing(inputs)

       
        # Calculate the value of 'wRepeatInterval'
        if inputs.packetSetup.RepeatInterval == 0:
            self.RepeatIntervalWoTX = math.ceil(self.MinRepeatIntervalforUniformSpacing/10.0 - 1e-5)*10
        else:
            self.RepeatIntervalWoTX = inputs.packetSetup.RepeatInterval
        self.cfg['wRepeatInterval'] =  self.RepeatIntervalWoTX

        # Repeat Interval with TX (ms)
        if self.RepeatIntervalWoTX > self.MinRepeatIntervalforUniformSpacing:
            self.RepeatIntervalWithTX = self.RepeatIntervalWoTX 
        else:
            self.RepeatIntervalWithTX = self.MinRepeatIntervalforUniformSpacing


    def calc_MinRepeatIntervalforUniformSpacing(self, inputs):
        # Min Repeat Interval for Uniform Spacing(ms)
        
        t = self.LEDonTime

        # Packet 0
        if inputs.packetSetup.Packet0Enabled:
            t += self.StartupTime      # start time
            t += self.Packet0Latency   # Packet0 Latency

        # Packet 1
        if inputs.packetSetup.Packet1Enabled and inputs.packetSetup.Packet0Enabled:
            t += self.AddiProgLatency  # Additional Programmed Latency
        if inputs.packetSetup.Packet1Enabled:
            t += self.StartupTime      # start time again
            t += self.Packet1Latency   # Packet1 Latency

        # Packet 2
        if inputs.packetSetup.Packet2Enabled and (inputs.packetSetup.Packet1Enabled or inputs.packetSetup.Packet0Enabled):
            t += self.AddiProgLatency  # Additional Programmed Latency
        if inputs.packetSetup.Packet2Enabled:
            t += self.StartupTime      # start time again
            t += self.Packet2Latency   # Packet2 Latency

        # Packet 3
        if inputs.packetSetup.Packet3Enabled and (inputs.packetSetup.Packet2Enabled or inputs.packetSetup.Packet1Enabled or inputs.packetSetup.Packet0Enabled):
            t += self.AddiProgLatency  # Additional Programmed Latency
        if inputs.packetSetup.Packet3Enabled:
            t += self.StartupTime      # start time again
            t += self.Packet3Latency   # Packet3 Latency

        self.MinRepeatIntervalforUniformSpacing = math.ceil(t/5.0 - 1e-5)*5


    def calc_interval_table(self, inputs):
        # Table of Interval/Start/End
        self.IntervalTable      = list(range(1, 257))
        self.IntervalStartTable = [0]*256
        self.IntervalEndTable   = [0]*256
        for i in range(0, 256):
            if (i + 1) in inputs.packetSetup.PacketBD:  # Enabled
                t = self.RepeatIntervalWithTX
            else:
                t = self.MinRepeatIntervalforUniformSpacing

            self.IntervalEndTable[i]   = t if i == 0 else (t + self.IntervalEndTable[i - 1])
            self.IntervalStartTable[i] = 0 if i == 0 else self.IntervalEndTable[i-1]


        # Maximum Button Duration before Shutdown or EEPROM erase(if enabled)(Seconds)
        self.MaxButtonDuration = inputs.packetSetup.MaxButtonDuration
        delta = [abs(m - self.MaxButtonDuration*1000) for m in self.IntervalEndTable]  # TODO change it to match
        self.cfg['bMaxButtonDuration'] = self.IntervalTable[delta.index(min(delta))]

        # Mininum # of TX's per button press
        self.MinNumOfTXsPerPress = inputs.packetSetup.MinNumOfTXsPerPress
        self.cfg['bMinButtonDuration'] = self.PacketBD[self.MinNumOfTXsPerPress - 1]

        # Mininum # TX's after a battery swap or ADC/Temp crossing threshold
        self.MinNumOfTXsaftBatterySwap = inputs.packetSetup.MinNumOfTXsaftBatterySwap
        self.cfg['bAltMinButtonDuration'] = self.PacketBD[self.MinNumOfTXsaftBatterySwap - 1]

    def update_bPacket(self, PacketBD, PacketTX):
        # bPacketBD[32]
        self.PacketBD             = PacketBD
        self.cfg['bPacketBD[32]'] = PacketBD
        # bPacketTX[32]
        self.PacketTX             = PacketTX
        self.cfg['bPacketTX[32]'] = PacketTX

        # PacketBD for PQ file
        self.PacketBD_PQ_file = list(range(32))
        indexes = list(range(1, 32))
        self.PacketBD_PQ_file[0] = PacketBD[0]

        for i in indexes:
            if PacketBD[i] != 0:
                self.PacketBD_PQ_file[i] = PacketBD[i]
            else:
                self.PacketBD_PQ_file[i] = self.PacketBD_PQ_file[i - 1]
        self.cfg_PQ_file['bPacketBD[32]'] = self.PacketBD_PQ_file

        # PacketTX for PQ file
        self.PacketTX_PQ_file = list(range(32))
        self.PacketTX_PQ_file[0] = PacketTX[0]

        indexes = list(range(1, 32))
        for i in indexes:
            if PacketTX[i] != 0:
                self.PacketTX_PQ_file[i] = PacketTX[i]
            else:
                self.PacketTX_PQ_file[i] = self.PacketTX_PQ_file[i - 1]
        self.cfg_PQ_file['bPacketTX[32]'] = self.PacketTX_PQ_file
                
                
    def get_cfg_data(self):
        return self.cfg

    def dump(self):
        print('    -------------  configuration data for TX C file  -------------')
        for m in self.cfg:
            print('        {}  =  {}'.format(m, self.cfg[m]))

        print('    -------------  configuration data for PQ C file  -------------')
        for m in self.cfg_PQ_file:
            print('        {}  =  {}'.format(m, self.cfg_PQ_file[m]))


















































