'''
Created on Feb 25, 2013

@author: sesuskic
'''

import os
from version_info import version_info

__all__ = ["generate"]

def value_format_string(x, api_file):
    if api_file in ['api', 'raw']:
        prefix = "0x"
    else:
        prefix = ""
    return prefix + "{:0" + "{:d}".format(((x.bit_length()+7)//8)*2) + "X}"
    #return prefix + "{:0" + "{:d}".format(max(2,((x.bit_length()+7)//8)*2)) + "X}"


def generate(outdir, inputfilebase, modem_calc, api_format, api_file):
    if api_file == 'api':
        file_suffix = '_API.tcl'
        set_literal = 'set '
        prop_name_format_string = '{:s}'
    elif api_file == 'wds':
        file_suffix = '_WDS.tcl'
        set_literal = "'SET_PROPERTY' "
        prop_name_format_string = "'{:s}'"
    elif api_file == 'raw':
        file_suffix = '_RAW.txt'
        set_literal = ""
        prop_name_format_string = '{:s}:'
    else:
        raise RuntimeError("Invalid api style input for file generation")
        
    setPropertyApiFile = os.path.join(outdir, inputfilebase + file_suffix)
    
    if api_format == 'legacy':
        api_list = modem_calc.get_api_list()
    elif api_format == 'object_api':
        # TODO: need to hide _object_api_list
        api_list = modem_calc.api_list._object_api_list
    else:
        raise RuntimeError("Requested invalid api format")

    ModTypeText = ['Carrier',  'OOK',  '2FSK',  '2GFSK', '4FSK', '4GFSK']

    # header info of RF, ModType, DR, Dev, RXBW
    outRF = 'RF frequency is {:4.2f} MHz\n'.format(modem_calc.inputs.API_fc/1e6)
    outMod = 'Mod type: ' + ModTypeText[int(modem_calc.inputs.API_modulation_type)] +',\t'
    if (modem_calc.inputs.API_modulation_type>1) : # FSK: show deviation
        outMod += 'DataRate: {:3.2f} ksps;  FreqDev: {:3.2f} kHz;  RX BW: {:3.2f} kHz \n'.format(modem_calc.inputs.API_Rsymb/1e3, modem_calc.inputs.API_Fdev/1e3, modem_calc.demodulator.Ch_Fil_Bw)
    elif (modem_calc.inputs.API_modulation_type==1) : # OOK: show RXBW
        outMod += 'DataRate: {:3.2f} ksps;  RX BW: {:3.2f} kHz \n'.format(modem_calc.inputs.API_Rsymb/1e3, modem_calc.demodulator.Ch_Fil_Bw)

    ## set property for modem APIs (ROM1)
    fid = open(setPropertyApiFile, 'w') # do not seperate TX & RX into 2 parts
    # svnRev replaced by python release #
    # fid.write('# Calculator svn revision {:d}\n\n'.format(modem_calc.svnRev)) 
    fid.write('# Calculator version {:s}\n\n'.format(version_info.get_version_string()))
    fid.write('################################################################# \n')
    fid.write('# ' + outRF)
    fid.write('# ' + outMod)
    fid.write('##################  General parameters \n')
    for k, v in iter(api_list.items()):
        fid.write(("{:s}"  + prop_name_format_string + " " + value_format_string(v, api_file) + "\n").format(set_literal, k, v))
    fid.write('################################################################# \n')
    fid.close()
