'''
Created on July 25, 2013

@author: shyang
'''


__all__ = ["CalcUtils"]

class CalcUtils(object):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        return
    #
    # lookup function in excel
    @staticmethod
    def lookup(val, vector):
        Last = "Min value in Vector is bigger than val"
        for m in vector:
            if m > val:
                break
            else:
                Last = m

        return Last

    @staticmethod
    def lookupIndex(val, vector):
        '''
        Only do a approximate match
        Elements in vector must be sorted in a ascending order
        '''
        Last = "Min value in Vector is bigger than val"
        for m in vector:
            if m > val:
                break
            else:
                Last = m

        if Last in vector:
            pos = vector.index(Last)
        else:
            pos =  Last
        return pos


    @staticmethod
    def lookupLR(val, lookup_vector, result_vector):
        Last = "Min value in Vector is bigger than val"
        for m in lookup_vector:
            if m > val:
                break
            else:
                Last = m

        if Last in lookup_vector:
            result = result_vector[lookup_vector.index(Last)]
        else:
            result = Last

        return result

    # lookup min 

    @staticmethod
    def lookupMin(lookup_vector, result_vector):
        return (result_vector[lookup_vector.index(min(lookup_vector))])

    @staticmethod
    def freq1(freq):
        '''
        Find the Freq1 of a given frequency
        '''
        return CalcUtils.lookup(freq, [27,100,240,315,350,390,434,600,730,868,915,960])


    @staticmethod
    def freqIndex(freq):
        '''
        Find the index of a given frequency in the frequency table
        '''
        return [27,100,240,315,350,390,434,600,730,868,915,960].index(CalcUtils.freq1(freq))

    @staticmethod
    def freq2(freq):
        '''
        Find the Freq2 of a given frequency
        '''
        return CalcUtils.lookupLR(freq, 
                                  [27, 100,240,315,350,390,434,600,730,868,915,960],
                                  [100,240,315,350,390,434,600,730,868,915,960,960])


