'''
Created on Oct 14, 2013

@author: sesuskic
'''

# TODO: need to figure out if the radio properties referenced in this module
# actually work for all Pro2/Pro2+ radios, or do I need something like the 
# modem_radio_api approach
from .propertygroupbasedoptions import PropertyGroupBasedOptions

class Preamble(PropertyGroupBasedOptions):
    def __init__(self, radioApi):
        super(Preamble, self).__init__(radioApi, "preamble")
        
class Sync(PropertyGroupBasedOptions):
    def __init__(self, radioApi):
        super(Sync, self).__init__(radioApi, "sync")
        
class Pkt(PropertyGroupBasedOptions):
    def __init__(self, radioApi):
        super(Pkt, self).__init__(radioApi, "pkt")
        
class Match(PropertyGroupBasedOptions):
    def __init__(self, radioApi):
        super(Match, self).__init__(radioApi, "match")
