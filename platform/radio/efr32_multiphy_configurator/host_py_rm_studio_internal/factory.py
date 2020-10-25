
import os
import imp


_PKG_PATH = os.path.dirname(os.path.realpath(__file__))

__all__ = ['RM_Factory', 'RM_GetInfo',
           'RM_S1_PART_FAMILY_NAMES', 'RM_S2_PART_FAMILY_NAMES',
           'RM_ALL_PART_FAMILY_NAMES', 'rm_dynamic_import']

_RM_PART_FAMILY_MAP = {
    'DUMBO':   'efr32xg1',
    'JUMBO':   'efr32xg12',
    'NERIO':   'efr32xg13',
    'NIXI':    'efr32xg14',
    'PANTHER': 'efr32xg21',
    'LYNX':    'efr32xg22',
    'OCELOT':  'efr32xg23',
    'BOBCAT':  'efr32xg24',
    'SOL':     'efr32xg25',
}

RM_S1_PART_FAMILY_NAMES = [
    'DUMBO',
    'JUMBO',
    'NERIO',
    'NIXI',
]

RM_S2_PART_FAMILY_NAMES = [
    'PANTHER',
    'LYNX',
    'OCELOT',
    'BOBCAT',
    'SOL',
]

RM_ALL_PART_FAMILY_NAMES = sorted(_RM_PART_FAMILY_MAP.keys())


def get_mod_name(part_family_name):
    """Get the module name for full part.

    Args:
        part_family_name (str): A key in the RM_ALL_PART_FAMILY_NAMES.

    Returns:
        str: A value from RM_ALL_PART_FAMILY_NAMES.
    """
    try:
        mod_name = _RM_PART_FAMILY_MAP[part_family_name]
    except KeyError:
        raise KeyError("Invalid family name '{}'. "
                       "Available: {}".format(part_family_name,
                                              ','.join(RM_ALL_PART_FAMILY_NAMES)))
    return mod_name


def rm_dynamic_import(part_family_name, rev_name=None):
    """Return a device revision class for instantiation.

    This can be one of four types of register classes for a part revision:
        - An external manufacturer core regs revision (e.g. M33R0P4)
        - A Silicon Labs RTL revision (e.g. A0, A1)
        - A Silicon Labs device info revision (e.g. DI0, DI1)
        - A Silicon Labs PTE MFG revision (e.g. PTE0, PTE1)

    Args:
        part_family_name (str): A key in the RM_ALL_PART_FAMILY_NAMES.
        rev_name (str): A family specific revision string.

    Returns:
        Varies: Device revision class. The name is comprised of the
            the info class's val_die_name and the rev_name string
            (e.g. RM_Device_EFR32XG22X000F512IM32_RevA2).

    """
    rm_info = RM_GetInfo(part_family_name)
    if rev_name is None:
        rev_name = rm_info.rtl_revs[-1]
    elif rev_name not in rm_info.all_revs:
        raise ValueError("Invalid register revision '{}'. "
                         "Available revisions: {}".format(rev_name,
                                                          ','.join(rm_info.all_revs)))
    mod_name = os.path.basename(rm_info.pkg_path) + '.rev' + rev_name
    rev_path = os.path.join(rm_info.pkg_path, 'rev' + rev_name)
    rev_pkg = imp.load_module(mod_name, None, rev_path, ('', '', 5))

    return getattr(rev_pkg, 'RM_Device_' + rm_info.val_die_name + '_Rev' + rev_name)


def RM_Factory(part_family_name, rev_name=None):
    return rm_dynamic_import(part_family_name, rev_name)


def RM_GetInfo(part_family_name):
    mod_name = get_mod_name(part_family_name)
    die_path = os.path.join(_PKG_PATH, 'full', mod_name)
    die_pkg = imp.load_module(mod_name, None, die_path, ('', '', 5))
    factory_func = getattr(die_pkg.factory, 'RM_GetInfo')
    return factory_func()
