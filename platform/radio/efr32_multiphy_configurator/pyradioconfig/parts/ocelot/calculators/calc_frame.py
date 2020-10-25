from pyradioconfig.parts.panther.calculators.calc_frame import CALC_Frame_panther

# This file inherits from Panther instead of Lynx because Ocelot does not need
# the workaround for PGLYNX-2670
class CALC_Frame_ocelot(CALC_Frame_panther):
    pass
