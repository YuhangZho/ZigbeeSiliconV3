'''
Created on May 8, 2014

@author: sesuskic
'''
import os
from siradioapifoundation import SiRadioApi
from common import Properties

class SiRadioApiFactory(object):

    @staticmethod
    def createRadioApi(radioApiName):
        myPath = os.path.abspath(os.path.dirname(__file__))
        dataDirName = "siradioapi"
        if "_factory" in radioApiName:
            dataDirName += "_factory"
        props = Properties()
        propFile = myPath + "/../" + dataDirName + "/" + radioApiName + ".properties"
        with open(propFile) as f:
            props.load(f)
        return SiRadioApi(props.getPropertyDict())
        