'''
Created on Jan 30, 2013

@author: sesuskic
'''

import os
from argparse import ArgumentParser
from si4440_modem_calc import modemcalculatorfactory, CalcLog, Pro2CalcException    
from property_generator import generate
from version_info import version_info

def modem_calc_tb(testplan='pro2_test_case.txt', 
                  iqcal=False, 
                  fields=False, 
                  outdir='None', 
                  radio='pro2',
                  apiformat='legacy',
                  apifile='api'):

    if (not(outdir) or (outdir == 'None') or (outdir == '')):
        outdir = os.path.dirname(os.path.abspath(testplan))
    inputfilebase = os.path.splitext(os.path.basename(testplan))[0]
    calc_log_file = os.path.join(outdir, inputfilebase + '_calc_log.txt')
    calc_log = CalcLog(calc_log_file)
    calc_log.add_to_log('# radio:  {:s}\n\n'.format(radio))

    modem_calc = modemcalculatorfactory.create_modem_calculator(testplan, radio)
    modem_calc.calc_log = calc_log
    try:
        modem_calc.calculate()
    except Pro2CalcException as e:
        return
    calc_log.write_to_calc_log(modem_calc)
    generate(outdir, inputfilebase, modem_calc, apiformat, apifile)
    if iqcal:
        inputfilebase += '_iqcal' # prep for output filename
        modem_calc.calculate_iqcal()
        calc_log.write_to_calc_log(modem_calc)
        generate(outdir, inputfilebase, modem_calc, apiformat, apifile)
    if fields:
        print(modem_calc.get_data())
    return modem_calc.get_data() 

def main(argv=None):
    parser = ArgumentParser()
    parser.add_argument("inputfile", help="Specify file containing modem calculator inputs")
    parser.add_argument("--iqcal", action="store_true", help="Turn on IQ cal in the modem calculator")
    parser.add_argument("--fields", action="store_true", help="Print modem calculator output data")
    parser.add_argument("--outdir", help="Specify an existing directory to which the calculator will generate the output files")
    parser.add_argument("--radio", default='pro2', help="Valid options: pro2 (default), pro2plus (revC2), pro2plus_legacy (revC0 and revC1)")
    parser.add_argument("--apiformat", default='legacy', help="Valid options: legacy (default), object_api")
    parser.add_argument("--apifile", default='api', help="Valid options: api (default), wds, raw")
    parser.add_argument("--version", action="version", version=version_info.get_version_string())
    args = parser.parse_args()
    modem_calc_tb(args.inputfile, 
                  args.iqcal, 
                  args.fields, 
                  args.outdir, 
                  args.radio,
                  args.apiformat,
                  args.apifile)
    
if __name__ == '__main__':
    main()
    pass
