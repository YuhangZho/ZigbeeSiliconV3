'''
Created on Feb 18, 2014

@author: sesuskic
'''

from common import ChipConfiguratorInterface
from .sipro2 import SiPro2
from .sipro2plus import SiPro2Plus, SiPro2PlusRevC01
from .siezradio2 import EzRadio2, EzRadio2Pro2Plus
from .siradioapifactory import SiRadioApiFactory

class ConfiguratorFacade(ChipConfiguratorInterface):

    def __init__(self):
        self._configurator = None

    
    def _create_radio_from_api(self, radio_api):
        if all(item in radio_api.features for item in ["EZCONFIG2", "REVB"]):
            return EzRadio2(radio_api)
        if all(item in radio_api.features for item in ["EZCONFIG2", "REVC"]):
            return EzRadio2Pro2Plus(radio_api)
        if (all(item in radio_api.features for item in ["REVC", "REVC0"]) or
            all(item in radio_api.features for item in ["REVC", "REVC1"])):
            return SiPro2PlusRevC01(radio_api)
        if all(item in radio_api.features for item in ["REVC", "REVC2"]):
            return SiPro2Plus(radio_api)
        return SiPro2(radio_api)        
    
    def _create_configurator(self, chipId):
        radio_api = SiRadioApiFactory.createRadioApi(chipId)
        return self._create_radio_from_api(radio_api)

    def instance(self, chipId):
        chipIdToUse = chipId
        if chipId == "si446x": # addresses legacy use of si446x in ISD
            chipIdToUse = "si4460"
        # let's assume that chipId will have <part_nam>_<rev>[_factory] format
        part_name_pieces = chipIdToUse.split("_")
        num_part_name_pieces = len(part_name_pieces)
        # If there are not 2 or 3 elements, need to assert
        assert(num_part_name_pieces in [1, 2, 3, 4])
        if num_part_name_pieces == 1:
            if chipId in ["si4467", "si4468"]:
                chipIdToUse += "_reva0a"
            elif chipId in ["si4469"]:
                chipIdToUse += "_reva1a"
            else:
                chipIdToUse += "_revb1b"
        # check is this object's _configurator was ever created
        if self._configurator is None:
            # if it was not, create it
            self._configurator = self._create_configurator(chipIdToUse)
        else:
            # the object already has a _configurator, but need to 
            # handle the case when requested chipId is si446x, and
            # the case when the existing _configurator does not match
            # the otherwise requested chipId
            if (self._configurator.get_name().startswith("si446") and
                "revb" in self._configurator.get_name() and
                chipId == "si446x"):
                pass
            else:
                if (self._configurator.get_name() != chipIdToUse):
                    self._configurator = None
                    self._configurator = self._create_configurator(chipIdToUse)
        return self._configurator
        