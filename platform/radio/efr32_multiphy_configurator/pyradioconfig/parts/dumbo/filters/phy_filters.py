"""
Dumbo specific filters
"""
from pyradioconfig.calculator_model_framework.interfaces.iphy_filter import IPhyFilter


class PhyFilters(IPhyFilter):

    customer_phy_groups = [
                            'Phys_Acuity',
                            'Phys_ATnT',
                            'Phys_Bluetooth_LE',
                            'Phys_Deprecated',
                            'Phys_Diehl_Hydrometer',
                            'Phys_Essence',
                            'Phys_IEEE802154',
                            'Phys_imagotag',
                            'Phys_Landis_Gyr',
                            'Phys_lutron',
                            'Phys_Mbus_lab',
                            'Phys_OOK',
                            'Phys_RAIL',
                            'Phys_sim_tests',
                            'Phys_Utility',
                            'Phys_Velux',
                            'Phys_sim_tests',
                            'Phys_Internal',
                        ]

    sim_tests_phy_groups = ['Phys_sim_tests']

    simplicity_studio_phy_groups = ['Phys_Datasheet', 'Phys_Studio', 'Phys_connect', 'Phys_MBus_Studio' ]

    non_functional_phy_groups = ['Phys_ASK']
