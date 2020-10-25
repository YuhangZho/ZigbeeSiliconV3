'''
Created on Aug 17, 2014

@author: sesuskic
'''

from common import CategoryInterface

__all__  = ["BandwidthTime"]

class BandwidthTime(CategoryInterface):
    # Each element in the list in the map is a value for MODEM_TX_FILTER_COEFF_[0:8]
    # property. Element 0 in this array corresponds to MODEM_TX_FILTER_COEFF_8, 
    # element 1 corresponds to MODEM_TX_FILTER_COEFF_7, and etc.
    BANDWIDTH_TIME_COEFFICIENTS = {0.30: [0x3F, 0x3D, 0x39, 0x32, 0x2A, 0x21, 0x19, 0x12, 0x0C],
                                   0.35: [0x48, 0x46, 0x3F, 0x35, 0x29, 0x1E, 0x15, 0x0D, 0x08],
                                   0.40: [0x52, 0x4F, 0x45, 0x37, 0x28, 0x1A, 0x10, 0x09, 0x04],
                                   0.45: [0x5D, 0x57, 0x49, 0x37, 0x25, 0x16, 0x0C, 0x05, 0x02],
                                   0.50: [0x67, 0x60, 0x4D, 0x36, 0x21, 0x11, 0x08, 0x03, 0x01],
                                   0.55: [0x71, 0x68, 0x50, 0x34, 0x1C, 0x0D, 0x05, 0x02, 0x00],
                                   0.60: [0x7B, 0x6F, 0x52, 0x31, 0x18, 0x0A, 0x03, 0x01, 0x00],
                                   0.65: [0x43, 0x3B, 0x29, 0x17, 0x0A, 0x03, 0x01, 0x00, 0x00],
                                   0.70: [0x48, 0x3F, 0x29, 0x14, 0x08, 0x02, 0x00, 0x00, 0x00],
                                   0.75: [0x4D, 0x42, 0x29, 0x12, 0x06, 0x01, 0x00, 0x00, 0x00],
                                   0.80: [0x52, 0x45, 0x28, 0x10, 0x04, 0x01, 0x00, 0x00, 0x00],
                                   0.85: [0x57, 0x47, 0x26, 0x0E, 0x03, 0x01, 0x00, 0x00, 0x00],
                                   0.90: [0x5C, 0x49, 0x25, 0x0C, 0x02, 0x00, 0x00, 0x00, 0x00],
                                   0.95: [0x62, 0x4C, 0x23, 0x0A, 0x02, 0x00, 0x00, 0x00, 0x00],
                                   1.00: [0x67, 0x4D, 0x21, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00]}
    BT_DEVIATION_MULTIPLIER_THRESHOLD = 0.65

    def __init__(self):
        super(BandwidthTime, self).__init__()
        self.reset()

    def reset(self):
        self.value = 0.50
        
    def set_option(self, option, value):
        real_value = super(BandwidthTime, self).set_option(option, value)
        if option == "value":
            if not(self.value in self.BANDWIDTH_TIME_COEFFICIENTS.keys()):
                raise KeyError("Invalid value for Bandwidth Time.")
        return real_value

    def get_configuration(self, cc):
        command_list = []
        if cc.get_option("BandwidthTime.enabled"):
            coeffs = self.BANDWIDTH_TIME_COEFFICIENTS[cc.get_option("BandwidthTime.value")]
            for idx in range(0, 9):
                command_list.append(cc.get_property("MODEM_TX_FILTER_COEFF_{}".format(8 - idx)).set_value(coeffs[idx]).configure())
        return command_list
        