'''
Created on July 25, 2013

@author: shyang
'''

import os
#import math
from si4010_modem_calc     import si4010apicalcfsklookup
from si4010_modem_calc     import si4010apicalcinputs
from si4010_modem_calc     import si4010apicalcpaimpedance
from si4010_modem_calc     import si4010apicalcods
from si4010_modem_calc     import si4010apicalcpower
from si4010_modem_calc     import si4010apicalcbetacalc
from si4010_modem_calc     import si4010apicalc

# TODO DEL when release
from imp import reload
reload(si4010apicalcfsklookup)
reload(si4010apicalcinputs)
reload(si4010apicalcpaimpedance)
reload(si4010apicalcods)
reload(si4010apicalcpower)
reload(si4010apicalcbetacalc)
reload(si4010apicalc)


def main(argv=None):
    '''
    print("Generate a group of input for test:\n")
    inputs = genInputs(0)

    print("\n\nGen a FSKLooup object:\n")
    FSKLookup = si4010apicalcfsklookup.Si4010ApiCalcFSKLookup(inputs)

    print("\n\nGen a PA Impedance object:\n")
    PAImpedance = si4010apicalcpaimpedance.Si4010ApiCalcPAImpedance(inputs)

    print("\n\nGen a ODS object:\n")
    ODS = si4010apicalcods.Si4010ApiCalcODS(inputs)

    print("\n\nGen a Calc Power object:\n")
    CalcPower = si4010apicalcpower.Si4010ApiCalcPower(inputs, PAImpedance)

    print("\n\nGen a Beta Calc object:\n")
    BetaCalc = si4010apicalcbetacalc.Si4010ApiCalcBetaCalc(inputs, PAImpedance, CalcPower)

    print("\n\nCalc with the API:")
    calc = si4010apicalc.Si4010ApiCalc(genInputs(1))
    print("\n\n\nAPI paras from calculator:", calc.get_api_list())
    '''
    smoke_test()



def genInputs(i):
    in_paras_0 = [0, 1, 10, 41, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 1, 100, 10.00, "NRZ Only", "FSK", 59, False, 12]
    in_paras_1 = [1, 2, 11, 42, False, 1000, 40, 5, "Min PA & Max Radiated Power", 99, 11, 2, 150, 18.00, "NRZ+4b5b", "FSK", 59, False, 12]

    result_api_0 = [0, 75, 10, 75, -0.0132, 27003724, 104, 1, 5, 0, 7, 400, 0, 5, 4]
    result_api_1 = [0, 43, 11, 43, 0.5406, 99013656, 104, 1, 5, 0, 4, 178, 0, 5, 4]

    result_api_paras = [result_api_0, result_api_1]



    paras = [in_paras_0, in_paras_1, ]
    inputs = si4010apicalcinputs.Si4010ApiCalcInputs(paras[i])
    return inputs


in_para_0 = [0, 1, 7.7, 22.5, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 1, 100, 10.00, "NRZ Only", "FSK", 59, False, 12]
in_para_1 = [1, 2, 7.7, 42, False, 1000, 40, 5, "Min PA & Max Radiated Power", 99, 11, 2, 150, 18.00, "NRZ+4b5b", "FSK", 59, False, 12]
in_para_2 = [0, 3, 12, 43, True, 1000, 40, 6, "Min PA & Max Radiated Power", 101, 12, 3, 200, 24.00, "Manchester", "FSK", 59, False, 12]
in_para_3 = [0, 4, 7.7, 44, False, 1000, 40, 7, "Min PA & Max Radiated Power", 269, 13, 4, 250, 35.00, "NRZ+4b5b", "FSK", 59, False, 12]
in_para_4 = [0, 5, 3, 32, False, 1000, 40, 8, "Max Radiated Power", 271, 14, 5, 300, 5.00, "Manchester", "OOK", 59, True, 3]
in_para_5 = [0, 6, 7.7, 46, False, 1000, 40, 9, "Min PA & Max Radiated Power", 353, 15, 6, 320, 4.00, "Manchester", "OOK", 59, False, 12]
in_para_6 = [0, 7, 7.7, 47, False, 1000, 40, 10, "Max Radiated Power", 436, 16, 7, 340, 3.00, "NRZ+4b5b", "OOK", 59, True, 12]
in_para_7 = [0, 8, 15, 40, False, 1000, 40, 11, "Max Radiated Power", 27, 10, 0, 300, 2.00, "NRZ Only", "OOK", 59, True, 12]
in_para_8 = [0, 3, 7.7, 40, True, 1000, 40, 12, "Max Radiated Power", 99, 10, 0, 300, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_9 = [0, 2, 7.7, 43, False, 1000, 40, 4, "Max Radiated Power", 101, 10, 0, 300, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_10 = [0, 10, 19, 40, True, 700, 312, 4, "Max Radiated Power", 100, 10, 0, 300, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_11 = [0, 9, 7.7, 40, False, 1000, 40, 15, "Min PA & Max Radiated Power", 269.5, 10, 0, 300, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_12 = [0, 7, 7.7, 40, False, 1000, 40, 15, "Min PA & Max Radiated Power", 272.125, 10, 0, 300, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_13 = [0, 4.2, 4, 40, False, 1000, 40, 15, "Min PA & Max Radiated Power", 27, 10, 2, 300, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_14 = [0, 4.2, 7.7, 40, False, 1000, 40, 15, "Min PA & Max Radiated Power", 433, 10, 0, 300, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_15 = [0, 4.2, 7.7, 40, True, 500, 500, 15, "Min PA & Max Radiated Power", 27, 10, 3, 20, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_16 = [0, 4.2, 3, 40, True, 500, 500, 15, "Min PA & Max Radiated Power", 434, 3, 0, 300, 23.00, "NRZ Only", "FSK", 59, True, 12]
in_para_17 = [0, 4.2, 2, 40, True, 500, 500, 15, "Min PA & Max Radiated Power", 27, 10, 8, 300, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_18 = [0, 4.2, 7.7, 40, True, 500, 500, 15, "Min PA & Max Radiated Power", 860, 4, 0, 40, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_19 = [0, 4.2, 7.7, 40, False, 1000, 40, 15, "Min PA & Max Radiated Power", 27, 10, 1, 300, 48.00, "NRZ Only", "FSK", 59, True, 12]
in_para_20 = [0, 4.2, 7.7, 40, False, 1000, 40, 15, "Min PA & Max Radiated Power", 861.3, 10, 0, 60, 50.00, "NRZ Only", "FSK", 40, True, 12]
in_para_21 = [0, 4.2, 7.7, 40, False, 1000, 40, 15, "Min PA & Max Radiated Power", 27, 9, 0, 300, 3.00, "NRZ Only", "FSK", 59, True, 12]
in_para_22 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Min PA & Max Radiated Power", 899, 10, 3, 92, 50.00, "NRZ+4b5b", "FSK", 15, True, 12]
in_para_23 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Min PA & Max Radiated Power", 27, 10, 0, 300, 19.00, "NRZ+4b5b", "FSK", 59, True, 12]
in_para_24 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Min PA & Max Radiated Power", 27, 1, 10, 300, 50.00, "NRZ+4b5b", "FSK", 18, True, 12]
in_para_25 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 595, 7, 0, 38, 50.00, "NRZ+4b5b", "FSK", 59, True, 12]
in_para_26 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 0, 300, 23.00, "NRZ+4b5b", "FSK", 66, True, 12]
in_para_27 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 390, 8, 0, 483, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_28 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 3, 3, 50.00, "Manchester", "FSK", 40, True, 12]
in_para_29 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 0, 300, 25.00, "Manchester", "FSK", 59, True, 12]
in_para_30 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 4, 300, 50.00, "Manchester", "FSK", 32, True, 12]
in_para_31 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 0, 20, 36.00, "Manchester", "FSK", 59, True, 12]
in_para_32 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 0, 300, 50.00, "Manchester", "FSK", 35, True, 12]
in_para_33 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 0, 300, 37.00, "Manchester", "FSK", 59, True, 12]
in_para_34 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 0, 300, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_35 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 0, 300, 50.00, "NRZ Only", "FSK", 55, True, 12]
in_para_36 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 0, 300, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_37 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 0, 300, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_38 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 0, 300, 50.00, "NRZ Only", "FSK", 19, True, 12]
in_para_39 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 0, 300, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_40 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 0, 300, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_41 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 0, 300, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_42 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 0, 300, 50.00, "NRZ Only", "FSK", 59, True, 12]
in_para_43 = [0, 4.2, 7.7, 40, False, 1000, 40, 4, "Max Radiated Power", 27, 10, 0, 300, 50.00, "NRZ Only", "FSK", 59, True, 12]


result_api_0 = [0, 75, 10, 1, -0.0087, 27003724, 104, 1, 5, 0, 7, 400, 0, 5, 4]
result_api_1 = [0, 43, 11, 2, 0.5406, 99013656, 104, 1, 5, 0, 4, 178, 0, 5, 4]
result_api_2 = [0, 59, 511, 3, 0.0000, 101013932, 104, 1, 5, 0, 7, 83, 0, 5, 4]
result_api_3 = [0, 60, 13, 4, 0.4483, 269037106, 104, 1, 5, 0, 4, 91, 0, 5, 4]
result_api_4 = [1, 77, 14, 5, 0.3489, 271000000, 0, 0, 2, 2, 7, 800, 0, 10, 8]
result_api_5 = [0, 76, 15, 6, 0.4104, 353000000, 0, 0, 5, 0, 7, 500, 0, 5, 4]
result_api_6 = [1, 75, 16, 7, 0.4050, 436000000, 0, 0, 7, 3, 4, 800, 6, 4, 3]
result_api_7 = [1, 77, 10, 8, -0.0267, 27000000, 0, 0, 7, 3, 7, 1500, 6, 4, 3]
result_api_8 = [0, 67, 511, 3, 0.0000, 99013656, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_9 = [0, 67, 10, 2, -0.3065, 101013932, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_10 = [0, 57, 159, 10, 0.1524, 100013794, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_11 = [1, 77, 10, 9, 0.6617, 269537175, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_12 = [1, 77, 10, 7, 0.6589, 272162537, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_13 = [1, 77, 10, 4.2, -0.0808, 27003724, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_14 = [1, 77, 10, 4.2, 0.6426, 433058925, 102, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_15 = [1, 76, 375, 4.2, 0.0472, 27003724, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_16 = [1, 76, 0, 4.2, 0.0000, 434059061, 102, 1, 7, 3, 7, 130, 0, 4, 3]
result_api_17 = [1, 76, 84, 4.2, 0.0494, 27003724, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_18 = [1, 72, 0, 4.2, 0.0000, 860059336, 37, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_19 = [1, 77, 10, 4.2, -0.0516, 27003724, 104, 1, 7, 3, 7, 63, 0, 4, 3]
result_api_20 = [1, 77, 10, 4.2, 0.6248, 861339768, 21, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_21 = [1, 77, 9, 4.2, -0.0304, 27003724, 104, 1, 7, 3, 7, 1000, 6, 4, 3]
result_api_22 = [0, 43, 10, 4.2, 0.2330, 899016058, 7, 1, 7, 3, 4, 48, 0, 4, 3]
result_api_23 = [0, 35, 10, 4.2, -0.0057, 27003724, 104, 1, 7, 3, 4, 126, 0, 4, 3]
result_api_24 = [0, 35, 1, 4.2, 5.2273, 27003724, 104, 1, 7, 3, 4, 48, 0, 4, 3]
result_api_25 = [1, 68, 7, 4.2, 0.9660, 595058953, 59, 1, 7, 3, 4, 48, 0, 4, 3]
result_api_26 = [0, 75, 10, 4.2, -0.0071, 27003724, 104, 1, 7, 3, 4, 104, 0, 4, 3]
result_api_27 = [1, 70, 8, 4.2, 0.2535, 390053797, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_28 = [1, 72, 10, 4.2, -0.0076, 27003724, 104, 1, 7, 3, 7, 30, 0, 4, 3]
result_api_29 = [0, 75, 10, 4.2, -0.0071, 27003724, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_30 = [0, 75, 10, 4.2, -0.0407, 27003724, 104, 1, 7, 3, 7, 30, 0, 4, 3]
result_api_31 = [0, 75, 10, 4.2, -0.0071, 27003724, 104, 1, 7, 3, 7, 42, 0, 4, 3]
result_api_32 = [0, 75, 10, 4.2, -0.0071, 27003724, 104, 1, 7, 3, 7, 30, 0, 4, 3]
result_api_33 = [0, 75, 10, 4.2, -0.0071, 27003724, 104, 1, 7, 3, 7, 41, 0, 4, 3]
result_api_34 = [0, 75, 10, 4.2, -0.0071, 27003724, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_35 = [0, 75, 10, 4.2, -0.0071, 27003724, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_36 = [0, 75, 10, 4.2, -0.0071, 27003724, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_37 = [0, 75, 10, 4.2, -0.0071, 27003724, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_38 = [0, 75, 10, 4.2, -0.0071, 27003724, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_39 = [0, 75, 10, 4.2, -0.0071, 27003724, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_40 = [0, 75, 10, 4.2, -0.0071, 27003724, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_41 = [0, 75, 10, 4.2, -0.0071, 27003724, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_42 = [0, 75, 10, 4.2, -0.0071, 27003724, 104, 1, 7, 3, 7, 60, 0, 4, 3]
result_api_43 = [0, 75, 10, 4.2, -0.0071, 27003724, 104, 1, 7, 3, 7, 60, 0, 4, 3]


TEST_CASES_IN     = [in_para_0, in_para_1, in_para_2, in_para_3, in_para_4, in_para_5, in_para_6, in_para_7, in_para_8, in_para_9, in_para_10, in_para_11, in_para_12, in_para_13, in_para_14, in_para_15, in_para_16, in_para_17, in_para_18, in_para_19, in_para_20, in_para_21, in_para_22, in_para_23, in_para_24, in_para_25, in_para_26, in_para_27, in_para_28, in_para_29, in_para_30, in_para_31, in_para_32, in_para_33, in_para_34, in_para_35, in_para_36, in_para_37, in_para_38, in_para_39, in_para_40, in_para_41, in_para_42, in_para_43,  ]
TEST_CASES_RESULT = [result_api_0, result_api_1, result_api_2, result_api_3, result_api_4, result_api_5, result_api_6, result_api_7, result_api_8, result_api_9, result_api_10, result_api_11, result_api_12, result_api_13, result_api_14, result_api_15, result_api_16, result_api_17, result_api_18, result_api_19, result_api_20, result_api_21, result_api_22, result_api_23, result_api_24, result_api_25, result_api_26, result_api_27, result_api_28, result_api_29, result_api_30, result_api_31, result_api_32, result_api_33, result_api_34, result_api_35, result_api_36, result_api_37, result_api_38, result_api_39, result_api_40, result_api_41, result_api_42, result_api_43,  ]


def smoke_test():
    i = 0
    for in_array, outp in zip(TEST_CASES_IN, TEST_CASES_RESULT):
        print("Case:{} ".format(i))
        calc   = si4010apicalc.Si4010ApiCalc(in_array)
        calc.calculate()
        calc_result = calc.get_api_list()
        
        checkResult(calc_result, outp, in_array)
        i = i + 1


def run_test_case(i):
    print("\n\nTo run test case:", i)
    in_array = TEST_CASES_IN[i]
    outp = TEST_CASES_RESULT[i]

    print("\n\n  Inputs:", in_array)
    inputs = si4010apicalcinputs.Si4010ApiCalcInputs(in_array)
    calc   = si4010apicalc.Si4010ApiCalc(inputs)
    calc_result = calc.get_api_list()

    checkResult(calc_result, outp, in_array)
    return calc

def checkResult(calc_results, expected, in_array):

    # vPa_Setup
    api_items = ['bMaxDrv',        'bLevel',
                 'wNominalCap',    'fAlpha',
                 'fBeta',
                 'fiDesiredFreq', 
                 'biFskDev',
                 # vODs_Setup
                 'bModulationType', 'bClkDiv',
                 'bEdgeRate',       'bGroupWidth',
                 'wBitRate',        'bLcWarmInt',
                 'bDivWarmInt',     'bPaWarmInt']

    #print("\nChecking calculation results:")
    #print("\n\n  Result:", calc_results)
    #print("\n\n  Expected:", expected)
    i = 0
    all_passed = True
    for m in api_items:
        if calc_results[m] != expected[i]:
            if m == 'fBeta':
                calc_results[m] = round(calc_results[m], 4)
            error = 0
            if type(expected[i]) == type(1.0) or type(calc_results[m] == type(1.0)):
                error =  abs((calc_results[m] - expected[i])/(calc_results[m] + 1e-3))
            else:
                error = 100
            if error > 1e-3:
                print("Input parameters:{}".format(in_array))
                print("\n    !!!! Item:{} expected:{} but get:{}. Error rate: {}\n".format(m, expected[i], calc_results[m], format(error, '.3%')))
                all_passed = False
        i = i + 1
    if all_passed:
        print("Pass! All results are matched")

if __name__ == '__main__':
    main()
    pass

