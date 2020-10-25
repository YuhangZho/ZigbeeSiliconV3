'''
Created on Sep 26, 2013

@author: sesuskic
'''

__all__ = ['CommandSet']

class CommandSet(object):
    ''' Container for command sets '''    
    def __init__(self, name):
        self.name = name
        self.command_list = []
        
    def add_command(self, command):
        self.command_list.append(command)