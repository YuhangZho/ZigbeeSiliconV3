
import os
from host_py_rm_studio_internal import rm_dynamic_import

_PKG_PATH = os.path.dirname(os.path.realpath(__file__))


class RM_EFR32XG1_Info(object):
    short_name = 'efr32xg1'
    val_die_name = 'EFR32XG1XFULL'
    arm_core_revs = []
    rtl_revs = ['A2', 'A3']
    di_revs = []
    pte_revs = []
    pkg_path = _PKG_PATH
    all_revs = arm_core_revs + rtl_revs + di_revs + pte_revs


def RM_Factory(rev_name):
    return rm_dynamic_import('DUMBO', rev_name)


def RM_GetInfo():
    return RM_EFR32XG1_Info