from pyradioconfig.parts.ocelot.calculators.calc_modulator import CALC_Modulator_Ocelot
from pyradioconfig.calculator_model_framework.Utils.CustomExceptions import CalculationException

class Calc_Modulator_Sol(CALC_Modulator_Ocelot):

    def calc_modindex_value(self, model):
        #Overriding this function due to variable name change

        fxo = model.vars.xtal_frequency_hz.value * 1.0
        modformat = model.vars.modulation_type.value
        freq_dev_hz = model.vars.deviation.value * 1.0
        synth_res = model.vars.synth_res_actual.value
        shaping_filter_gain = model.vars.shaping_filter_gain_actual.value
        interpolation_gain = model.vars.interpolation_gain_actual.value

        if modformat == model.vars.modulation_type.var_enum.FSK2 or \
           modformat == model.vars.modulation_type.var_enum.FSK4:
            modindex = freq_dev_hz * 16.0 / (synth_res * shaping_filter_gain * interpolation_gain)

        elif modformat == model.vars.modulation_type.var_enum.OQPSK or \
             modformat == model.vars.modulation_type.var_enum.MSK:
            modindex = fxo / (synth_res * 2 * shaping_filter_gain * interpolation_gain)

        elif modformat == model.vars.modulation_type.var_enum.BPSK or \
             modformat == model.vars.modulation_type.var_enum.OOK or \
             modformat == model.vars.modulation_type.var_enum.DBPSK or \
             modformat == model.vars.modulation_type.var_enum.ASK:
            modindex = 150.0 * 16 / (shaping_filter_gain * interpolation_gain)

        elif modformat == model.vars.modulation_type.var_enum.OFDM:
            modindex = 0.0 # don't care in OFDM
        else:
            raise CalculationException("ERROR: %s modulation not yet supported!" % modformat)
            return

        model.vars.modindex.value = modindex