# These are registers that should not be touched by radio configs, as they are handled by the RAIL API
PROTECTED_REGS = ['SYNTH.FREQ', 'SYNTH.CHCTRL',
                  'SYNTH.CHSP', 'FRC.BLOCKRAMADDR', 'FRC.CONVRAMADDR']

# These are fields of registers that should not be touched by radio configs, as they are handled by the RAIL API
PROTECTED_FIELDS = {
    'AGC.CTRL0': ['RSSISHIFT'],
    'FRC.RXCTRL': [
        'BUFRESTORERXABORTED',
        'BUFRESTOREFRAMEERROR',
        'BUFCLEAR',
        'TRACKABFRAME',
        'ACCEPTBLOCKERRORS',
        'ACCEPTCRCERRORS',
        'STORECRC'
    ],
    'HFXO0.TRIM': [
        'VTRREGTRIMANA',
        'VTRCORETRIMANA',
        'SHUNTLVLANA'
    ],
    'HFXO0.LOWPWRCTRL': ['SHUNTBIASANA'],
    'MODEM.IRCALCOEF': [
        'CIV',
        'CRV'
    ],
    'MODEM.IRCAL': [
        'MURSHF',
        'MUISHF'
    ],
    'RAC.AUXADCTRIM': ['AUXADCRCTUNE'],
    'RAC.PATRIM0': [
        'TX0DBMTRIMDUTYCYP',
        'TX0DBMTRIMDUTYCYN',
        'TX0DBMTRIMBIASP',
        'TX0DBMTRIMBIASN'
    ],
    'RAC.PATRIM1': [
        'TX0DBMTRIMREGVREF',
        'TX0DBMTRIMREGFB'
    ],
    'RAC.PATRIM2': [
        'TX6DBMTRIMDUTYCYP',
        'TX6DBMTRIMDUTYCYN',
        'TX6DBMTRIMBIASP',
        'TX6DBMTRIMBIASN'
    ],
    'RAC.PATRIM3': [
        'TX6DBMTRIMREGVREF',
        'TX6DBMTRIMREGFB',
        'TX6DBMTRIMPREDRVREGVREF',
        'TX6DBMTRIMPREDRVREGFB'
    ],
    'RAC.RFBIASCAL': [
        'RFBIASCALVREF',
        'RFBIASCALBIAS'
    ],
    'RAC.LNAMIXTRIM1': [
        'LNAMIXIBIASADJ',
        'LNAMIXRFPKDCALCM'
    ],
    'RAC.PRECTRL': ['PREREGTRIM'],
    'RAC.IFADCCAL': ['IFADCTUNERC'],
    'RAC.PGATRIM': ['PGACTUNE'],
    'RAC.AUXADCTRIM': [
        'AUXADCRCTUNE',
        'AUXADCLDOVREFTRIM'
    ],
	'RAC.SYEN': [
        'SYENVCOBIAS',
        'SYENMMDREG',
        'SYLODIVLDOBIASEN',
        'SYENCHPREG',
        'SYENCHPREPLICA',
        'SYENMMDREPLICA1',
        'SYENVCOPFET',
        'SYENVCOREG',
        'SYLODIVLDOEN',
        'SYCHPEN',
        'SYLODIVEN'
    ],
    'RFSENSE.TRIMBG': ['TRIMREF'],
    'RFSENSE.TRIMPTAT': [
        'TRIMCURR',
        'TRIMRES'
    ],
    'RFSENSE.TRIMDAC': [
        'TRIMDM',
        'CMPOSCAL',
        'TRIMCM'
    ],
    'SYNTH.LPFCTRL2RX': ['CALCRX'],
    'SYNTH.LPFCTRL2TX': ['CALCTX'],
    'SYNTH.VCDACCTRL': ['VCDACVAL'],
    'SYNTH.VCOGAIN': [
        'VCOKVCOARSE',
        'VCOKVFINE'
    ]
}

# Radio config action "enum"
RADIO_ACTION_WRITE = 0
RADIO_ACTION_AND = 1
RADIO_ACTION_XOR = 2
RADIO_ACTION_OR = 3
RADIO_ACTION_DELAY = 4
RADIO_ACTION_BITSET = 5
RADIO_ACTION_BITCLR = 6
RADIO_ACTION_WAITFORSET = 7
RADIO_ACTION_DONE = 0xFFFFFFFF
