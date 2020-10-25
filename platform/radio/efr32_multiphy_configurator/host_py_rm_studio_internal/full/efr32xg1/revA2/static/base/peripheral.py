

__all__ = [ 'Base_RM_Peripheral' ]

from collections import OrderedDict
from .. interface import IRegMapPeripheral

class Base_RM_Peripheral(IRegMapPeripheral):

    def __init__(self, rmio, label, baseAddress, name, description):
        self.__dict__['zz_frozen'] = False
        self.zz_rmio = rmio
        self.zz_label = label
        self.zz_rdict = OrderedDict()
        self.baseAddress = baseAddress
        self.name = name
        self.description = description

    def __setattr__(self, name, value):
        if self.__dict__['zz_frozen']:
            if name not in self.zz_rdict:
                raise AttributeError("ERROR: Invalid register '{}'\n".format(name))
            else:
                raise AttributeError("ERROR: Unable to set '{}' to '{}'\n".format(name, value))
        else:
            self.__dict__[name] = value

    def __repr__(self):
        out = "{:#010x}  {}: {} ({} registers)\n".format(self.baseAddress, self.name,
                                                         self.description, len(self.zz_rdict))
        reg_list = []
        for key in self.zz_rdict:
            reg = self.zz_rdict[key]
            reg_list.append([reg.baseAddress + reg.addressOffset, "    {}\n".format(reg.getDesc())])
        for regaddr, line in sorted(reg_list):
            out += line
        return out

    def getRegisterNames(self):
        nameList = []
        for key in sorted(self.zz_rdict):
            nameList.append(self.zz_rdict[key].fullname)
        return nameList

    def getAddressNameMap(self, addrNameDict):
        for key in self.zz_rdict:
            reg = self.zz_rdict[key]
            addrNameDict[reg.baseAddress + reg.addressOffset] = reg.fullname

    def setAccessedFlags(self):
        for key in self.zz_rdict:
            self.zz_rdict[key].setAccessedFlag()

    def clearAccessedFlags(self):
        for key in self.zz_rdict:
            self.zz_rdict[key].clearAccessedFlag()

    def getAccessedRegisterNames(self):
        nameList = []
        for key in sorted(self.zz_rdict):
            if self.zz_rdict[key].getAccessedFlag():
                nameList.append(self.zz_rdict[key].fullname)
        return nameList

    def getAccessedFieldNames(self):
        nameList = []
        for key in sorted(self.zz_rdict):
            nameList.extend(self.zz_rdict[key].getAccessedFieldNames())
        return nameList

    def assignRegDefault(self):
        for key in self.zz_rdict:
            self.zz_rdict[key].assignRegDefault()

    def getExcludedDumpRegisterNames(self):
        nameList = []
        for key in sorted(self.zz_rdict):
            if not self.zz_rdict[key].dumpActive:
                nameList.append(self.zz_rdict[key].fullname)
        return nameList

    def includeInDump(self):
        for key in self.zz_rdict:
            self.zz_rdict[key].includeInDump()

    def excludeFromDump(self):
        for key in self.zz_rdict:
            self.zz_rdict[key].excludeFromDump()

    def buildRegFilterList(self, outFH, filterList):
        reg_list = []
        for key in self.zz_rdict:
            reg = self.zz_rdict[key]
            # only append to list if dump flag is set
            if reg.dumpActive:
                reg_list.append([reg.baseAddress + reg.addressOffset, reg])
        for regaddr, reg in sorted(reg_list):
            reg.buildRegFilterList(outFH, filterList)

    def dump(self, outFH, valueDict):
        """
        This dump is only called for the use case with no filtered register
        list. Use the dumpActive flag to determine if this register should
        be read and dumped to file.
        """
        reg_list = []
        for key in self.zz_rdict:
            reg = self.zz_rdict[key]
            # only append to list if dump flag is set
            if reg.dumpActive:
                reg_list.append([reg.baseAddress + reg.addressOffset, reg])
        for regaddr, reg in sorted(reg_list):
            reg.dump(outFH, valueDict)
