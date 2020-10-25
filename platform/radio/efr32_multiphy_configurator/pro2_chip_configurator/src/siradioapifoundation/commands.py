'''
Created on Sep 26, 2013

@author: sesuskic
'''

__all__ = ['Commands']

class Commands(object):
    '''Container for command sets'''
    def __init__(self, name, args):
        assert len(args) == 4, "Parameter list needs 4 elements"
        self.name = name
        self.index_width = args[0]
        self.parameter_width = args[1]
        self.arg_max = args[2]
        self.reply_max = args[3]
        self.command_set_list = []
        
    def add_command_set(self, command_set):
        self.command_set_list.append(command_set)
    
    def get_command(self, name):
        for command_set in self.command_set_list:
            for command in command_set.command_list:
                if command.name == name.upper():
                    return command
        raise RuntimeError("Unknown command {}".format(name))
        
    def compile(self):
        arg_stream_list = []
        for cmd_set in self.command_set_list:
            for cmd in cmd_set.command_list:
                arg_stream_list.append(cmd.compile_arg_stream())
        return arg_stream_list
                

