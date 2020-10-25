from pyradioconfig.calculator_model_framework.interfaces.iphy_filter import IPhyFilter


class PhyFilters_Sol(IPhyFilter):

    # Studio black list (files removed before Studio distribution)
    customer_phy_groups = ['Phys_Internal_Base_Testing',
                           'Phys_Internal_Base_Utility',
                        ]

    # Studio white list (these PHYs show in Studio as proprietary starting points)
    simplicity_studio_phy_groups = ['Phys_Studio_SUNFSK']

    # Special designation for simulation PHYs
    sim_tests_phy_groups = []

    # Special designation for non-functional PHYs
    non_functional_phy_groups = []
