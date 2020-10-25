""" CALC_Viterbi Package

Calculator functions are pulled by using their names.
Calculator functions must start with "calc_", if they are to be consumed by the framework.
    Or they should be rturned by overriding the function:
        def getCalculationList(self):
"""
from pyradioconfig.parts.jumbo.calculators.calc_viterbi import CALC_Viterbi


class CALC_Viterbi_nerio(CALC_Viterbi):

    """
    Init internal variables
    """
    def __init__(self):
        self._major = 1
        self._minor = 0
        self._patch = 0

