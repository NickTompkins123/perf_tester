"""
This script is used to build the Kanzi components by the Continuous
integration server and split logs to component specific modules
"""

import os
import shutil
import sys
import subprocess

def write_file_to(file, target_path):
    try:
        os.makedirs(target_path)
    except:
        pass
    target_file = open(os.path.join(target_path, "log.txt"), "w")
    target_file.write(file)
    #shutil.copyfileobj(file, target_file) 

LogPath = "Logs"

possible_builds = ['debug', 'release']
possible_configs = ['es1', 'es2']
possible_modules = ['System', 'Core', 'User', 'Tests_generic','Tests_es1','Tests_es2']

usage_string = "Usage: python run_scons_and_split_logs.py <build_type> <config> <module> ... <module> where <build_type> is one of %s and config is one of %s and one or more modules must come from %s"%(str(possible_builds),str(possible_configs), str(possible_modules))

def killme():
    print usage_string
    sys.exit(2)

def slice_list_for_option(key_list, scanned_list):
    for k in key_list:
        if k in scanned_list:
            return k
    print "Error: required option was not found (one of ", str(key_list), ")"
    return None

def slice_list_for_options(key_list, scanned_list):
    out_list = set(key_list) & set(scanned_list)
    if not out_list:
        print "Error: none of required options was not found (one of ", str(key_list), ")"
        return None
    return out_list

option_arguments = sys.argv

# Get config type
config_type = slice_list_for_option(possible_configs, option_arguments)
if not config_type:
    killme()
# Get build type
build_type = slice_list_for_option(possible_builds, option_arguments)
if not build_type:
    killme()
# Get modules
modules = slice_list_for_options(possible_modules, option_arguments)
if not modules:
    killme()

# Build component libraries and tests and
# write logs to component libraries

config_string = build_type + '_' + config_type

for moduleName in modules:
    #cmd = "scons " + config_string + " " + moduleName + " 2>&1 | cat > " +  outfile
    cmd = "scons " + config_string + " " + moduleName 
    #print cmd
    scons_shell = subprocess.Popen(cmd, shell = True ,
                                    stderr = subprocess.STDOUT,
                                    stdout=subprocess.PIPE)

    stdout = scons_shell.communicate()[0]
    
    #print outfile to console
    print stdout
    
    # Write stdout to file for error parsing purposes
    logfilepath = os.path.join(LogPath,moduleName)
    write_file_to(stdout, logfilepath)
    
    retval = scons_shell.returncode
    if retval is not 0:
        print "Build aborted on abnormal error in module " + moduleName
        sys.exit(retval)
    

