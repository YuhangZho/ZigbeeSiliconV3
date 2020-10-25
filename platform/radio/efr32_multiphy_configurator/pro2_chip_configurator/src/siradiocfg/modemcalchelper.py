'''
Created on Aug 28, 2014

@author: sesuskic
'''
from copy import copy

__all__ = ["ModemCalcHelper"]

class ModemCalcHelper(object):
    @ staticmethod
    def configureModem(cc, performIqCal = False):
        cc._restrict_modem_inputs()
        if performIqCal:
            modem_inputs = copy(cc.get_options("Modem"))
            cc.modem_calc.calculate_iqcal()
            for k,v in modem_inputs.items():
                cc.set_option(k, v)
        else:
            cc.modem_calc.calculate()
        return cc.modem_calc.get_api_list(cc.modem_radio_api)
