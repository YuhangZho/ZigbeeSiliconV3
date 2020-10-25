import sys
import argparse
import unittest
import os
import json
import stat
import warnings
import time
sys.path.insert(0,".")
import testPro2.SPIDut as SPIDut
import testPro2.pause as pause
import testPro2.TestDCP2 as TestDCP2

def configure_tests (suite, config_file) :
    if isinstance(suite,unittest.TestCase) :
        test_name = u'{0}.{1}.{2}'.format(suite.__module__,type(suite).__name__,suite._testMethodName)
        if test_name in config_file :
            for attr, value in config_file[test_name].items() :
                setattr(suite,attr,value)
    else :
        for tests in suite :
            configure_tests(tests, config_file) 

def dump_test_config (suite, config_file) :
    if isinstance(suite,unittest.TestCase) :
        test_name = u'{0}.{1}.{2}'.format(suite.__module__,type(suite).__name__,suite._testMethodName)
        if test_name not in config_file :
            config_file[test_name] = {"enabled" : False, "runs" : 0, "seed" : None}
            if hasattr(suite,'enabled') :
                config_file[test_name]["enabled"] = suite.enabled
            if hasattr(suite,'runs') :
                config_file[test_name]["runs"] = suite.runs
    else :
        for tests in suite :
            dump_test_config(tests, config_file) 

if __name__ == '__main__' :
    parser = argparse.ArgumentParser("Run the Python Testing Environment")
    parser.add_argument("--firmware-dir","-f",type=str)
    parser.add_argument("--batch-mode","-b",action='store_true',default=False)
    parser.add_argument("--log-dir","-l",type=str)
    parser.add_argument("--dump-config","-d",action='store_true',default=False)
    parser.add_argument("config_file",type=str)
    args = parser.parse_args()
    SPIDut.firmware_dir = args.firmware_dir
    pause.batch_mode = args.batch_mode
    if stat.S_ISREG(os.stat(args.config_file).st_mode) :
        with open(args.config_file) as f :
            config_file = json.load(f)
    if args.log_dir is not None and not os.path.isdir(args.log_dir) :
        os.makedirs(args.log_dir)
    TestDCP2.log_directory = args.log_dir
    loader = unittest.defaultTestLoader
    suite = loader.discover("fw_test")
    configure_tests(suite,config_file)
    dump_test_config(suite,config_file)
    with open(args.config_file, 'w') as f:
        f.write(json.dumps(config_file, sort_keys=True, indent=2,separators=(',',': ')))
    if not args.dump_config :
        unittest.TextTestRunner(verbosity=2,resultclass=TestDCP2.Results).run(suite)