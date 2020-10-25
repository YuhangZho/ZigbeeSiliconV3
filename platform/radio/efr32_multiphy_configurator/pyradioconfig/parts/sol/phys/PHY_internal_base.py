from pyradioconfig.parts.lynx.phys.PHY_internal_base import Phy_Internal_Base_Lynx

class Phy_Internal_Base_Sol(Phy_Internal_Base_Lynx):
    """
        Common Internal Phy functions live here
        """

    # For now, inherit all from Lynx
    def __init__(self):

        super().__init__()

        self.agc_fastmode_reg_exclude_list = [
            'RAC_PGACTRL_LNAMIXRFPKDTHRESHSEL',
            'AGC_AGCPERIOD_MAXHICNTTHD',
            'AGC_AGCPERIOD_PERIODHI',
            'AGC_AGCPERIOD_PERIODLO',
            'AGC_AGCPERIOD_SETTLETIMEIF',
            'AGC_AGCPERIOD_SETTLETIMERF',
            'AGC_HICNTREGION_HICNTREGION0',
            'AGC_HICNTREGION_HICNTREGION1',
            'AGC_HICNTREGION_HICNTREGION2',
            'AGC_HICNTREGION_HICNTREGION3',
            'AGC_HICNTREGION_HICNTREGION4',
            'AGC_GAINSTEPLIM_CFLOOPDEL',
            'AGC_GAINSTEPLIM_CFLOOPSTEPMAX',
            'AGC_GAINSTEPLIM_HYST',
            'AGC_GAINSTEPLIM_MAXPWRVAR',
            'AGC_GAINSTEPLIM_TRANRSTAGC',
            'AGC_GAINSTEPLIM_LNAINDEXMAX',
            'AGC_GAINSTEPLIM_PGAINDEXMAX',
            'AGC_GAINSTEPLIM_PNINDEXMAX',
            'AGC_PNRFATT0_LNAMIXRFATT3',
            'AGC_PNRFATT0_LNAMIXRFATT4',
            'AGC_PNRFATT0_LNAMIXRFATT5',
            'AGC_PNRFATT1_LNAMIXRFATT6',
            'AGC_PNRFATT1_LNAMIXRFATT7',
            'AGC_PNRFATT1_LNAMIXRFATT8',
            'AGC_PNRFATT1_LNAMIXRFATT9',
            'AGC_PNRFATT1_LNAMIXRFATT10',
            'AGC_PNRFATT2_LNAMIXRFATT11',
            'AGC_PNRFATT2_LNAMIXRFATT12',
            'AGC_PNRFATT2_LNAMIXRFATT13',
            'AGC_PNRFATT2_LNAMIXRFATT14',
            'AGC_PNRFATT3_LNAMIXRFATT15',
            'AGC_PNRFATT3_LNAMIXRFATT16',
            'AGC_PNRFATT3_LNAMIXRFATT17',
            'AGC_GAINRANGE_BOOSTLNA',
            'AGC_GAINRANGE_LNABWADJ',
            'AGC_CTRL6_SEQPNUPALLOW',
            'AGC_CTRL6_SEQRFPKDEN',
            'AGC_CTRL1_SUBDEN',
            'AGC_CTRL1_SUBINT',
            'AGC_CTRL1_SUBNUM',
            'AGC_CTRL1_SUBPERIOD'
        ]
