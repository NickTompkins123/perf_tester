#=============================================================================
#   This is the lowest level of the Kanzi build scripts invoked from component
#   specific configuration files.
#
# All projects using this script must conform to the same hierarchy
# of build configurations where the platform specific build script reside in
#
#   <project_dir>/configs/platforms/<platform_name>
#
# and this script resides in 
#
#   Kanzi/Engine/configs/platforms/common
#
#=============================================================================


import os
import os.path
import shutil
import glob
import string
import subprocess
import re               # "exclude string" matching

Import("build")
Import("component")

source_to_build_map = {}
build_to_source_map = {}

def map_sourcedir(env, component, build_dir, dir):
    """ Generate mapping form source to build dir and apply options """
    rootdir = component["root"]
    #print "== Compose path '%s' + '%s')"%(rootdir, dir)
    srcdir = os.path.join(rootdir, dir)
    if not os.path.exists(srcdir):
        print "Error: path '%s' does not exist for component '%s'"%(srcdir, component["name"])
        Exit(2)
    blddir = os.path.join(build_dir, dir)
    env.BuildDir(blddir, srcdir, duplicate=0)
    source_to_build_map[srcdir] = blddir
    build_to_source_map[blddir] = srcdir
    return srcdir

def handle_build_and_source_directories(build, component):
    env = build["env"]
    build_dir = os.path.join(build["output"], component["name"])
    source_dir = []
    set_common_dir = []

    for dir in component["source_directory"]:
        source_dir.append(map_sourcedir(env, component, build_dir, dir))
  
    for dir in component["set_common_source_path"]:
        set_common_dir.append(map_sourcedir(env, component, build_dir, dir))

    return (build_dir, source_dir, set_common_dir)

def remap_path(original_head, new_head, path):
    """ Strip original head from path, replace with new head"""
    return string.replace(path, original_head, new_head)

def find_files(source_dir, endswith):
    endlength = len(endswith)
    source_files = []
    # print "Find file in ", source_dir, " that end with " , endswith

    for path, dirs, files in os.walk(source_dir):
        all_files = [os.path.abspath(os.path.join(path,file)) for file in files]
        for file in all_files:
            #print "    check file " , file
            if endswith == file[-endlength:]:
                #print "  accept"
                source_files.append(file)
    return source_files


def filter_library_for_bin_name(lib, build):
    name_map = build["lib_internal_to_resource_name_map"]
    if lib in name_map.keys():
        return name_map[lib]
    else:
        return lib
        

def get_env_option_set(build, component):
    """ Collect options for component libraries, library directories,
        include path, defines and linker options for shared libraries """

    include_paths = set() # include paths and library headers
    library_paths = set()
    linked_libraries = set()
    defines = set()
    shared_libraries = set()

    # Set common directory
    include_paths = include_paths.union(component["mapped_set_common_dir"])

    # Library headers
    for lib in component["library_headers"]:
        try:
            libref = build["libraries"][lib]
            include_paths = include_paths.union(libref["CPPPATH"])
        except:
            print "Note: library headers for " + lib + " required by component " + component["name"] + " were not found"  
            pass 
    
    # Linked libraries
    if component["link_libraries"]:
        for lib in component["link_libraries"]:
            linked_libraries.add(filter_library_for_bin_name(lib, build))
            try:
                libref = build["libraries"][lib]
                library_paths = library_paths.union(libref["LIBPATH"])
                # Check if lib is static or shared lib
                # Linking with shared libraries possibly needs to modify compiler options
                if libref["shared_data"]:
                    shared_libraries.add(libref["shared_data"])
            except:
                print "Note: library binary reference for " + lib + " required by component " + component["name"] + " was not found"  

    # Linked platform libraries (math library etc.)
    linked_libraries = linked_libraries.union(component["platform_libraries"])

    # Component include paths
    if component["include_path"]:
        for path in component["include_path"]:
            # either map to build root or use as absolute path
            # if first char is "#" remove the char and use the
            # rest of the path as an absolute 
            if path[0] is not "#":
                path = os.path.join(component["root"], path)
            else:
                path = path[1:]
            include_paths.add(path)

    # Defines
    defines = defines.union(component["defines"])

    option_set = {}
    option_set["include_paths"] = include_paths# include paths and library headers
    option_set["library_paths"] = library_paths
    option_set["linked_libraries"] = linked_libraries
    option_set["defines"] = defines
    option_set["shared_libraries"] = shared_libraries

    return option_set

def apply_option_set(env, option_set):
    """Append parameters to environment from option set"""
    env.Append(CPPPATH = list(option_set["include_paths"]))
    env.Append(LIBPATH = list(option_set["library_paths"]))
    env.Append(LIBS = list(option_set["linked_libraries"]))
    env.Append(CPPDEFINES = list(option_set["defines"]))

    # The few lines below where we use a tuple version of the 
    # shared library data must be kept in sync with the 
    # shared library definition files in common_utilities.py
    if option_set["shared_libraries"]:
        for shared_ref in option_set["shared_libraries"]:
            apply = shared_ref[0]
            apply(env, shared_ref)

def apply_build_options_to_env(env, build, component):
    option_set = get_env_option_set(build, component)
    apply_option_set(env, option_set)

def clone_env_from_component(env, build, component, build_dir, source_dir = None):

    """Clone a new environment and add configurations from component data"""

    new_env = env.Clone()
    new_env.Append(LIBPATH=[build_dir])
    if source_dir:
        new_env.Append(CPPPATH=source_dir)

    apply_build_options_to_env(new_env, build, component)
    return new_env

def compose_target_name(build_dir, component, number_of_build = 0):
    name = component["name"]
    
    if number_of_build > 0:
        name = "%s_%05i"%(name, number_of_build)

    target = os.path.join(build_dir,name)
    return target

def compose_lib_ref(build_dir, source_dir):
    if type(build_dir) is not list:
        build_dir = [build_dir]
    if type(source_dir) is not list:
        source_dir = [source_dir]
    return {"LIBPATH":build_dir, "CPPPATH":source_dir, "shared_data":None}

def build_library(build, component, files, build_dir, source_dir):
    if "env" not in component.keys():
        component["env"] = clone_env_from_component(build["env"], build, component, build_dir, source_dir)
    lib_env = component["env"] 
    target = compose_target_name(build_dir, component)
    component["buildaction"] = lib_env.Library(target, files)
    lib_env.Alias([component["name"], "all"], component["buildaction"])
    # Add references to build
    lib_ref = compose_lib_ref(build_dir, source_dir)
    build["libraries"][component["name"]] = lib_ref


def build_shared_library(build, component, files, build_dir, source_dir):
    if "env" not in component.keys():
        component["env"] = clone_env_from_component(build["env"], build, component, build_dir, source_dir)
    lib_env = component["env"] 
    target = compose_target_name(build_dir, component)
    library = lib_env.SharedLibrary(target, files)
    component["buildaction"] = lib_env.Install(os.path.join(build_dir, "bin"), library)
    lib_env.Alias([component["name"], "all"], [library, component["buildaction"]])
    lib_env["_LIBFLAGS"] = "-Wl,--start-group " + lib_env["_LIBFLAGS"] + " -Wl,--end-group"
    build["libraries"][component["name"]] = compose_lib_ref(build_dir, source_dir)
    return target
    
    
def build_program(build, component, files, build_dir, number_of_build = 0):
    if "env" not in component.keys():
        component["env"] = clone_env_from_component(build["env"], build, component, build_dir)
    prog_env = component["env"] 
    target = compose_target_name(build_dir, component, number_of_build)
    compile_action = prog_env.Program(target, files)
    component["buildaction"] = prog_env.Install(os.path.join(build_dir, "bin"), compile_action)
    prog_env.Alias([component["name"], "all"], [compile_action, component["buildaction"]])
    prog_env["_LIBFLAGS"] = "-Wl,--start-group " + prog_env["_LIBFLAGS"] + " -Wl,--end-group"
    return target


    
#===============================#
# MAIN PART                     #
# Script execution entry point  #
#===============================#


# Handle platform-specific option appends and overrides
def parse_platform_options(name):
    all_options = build["platform_options"][build["platform"]]
    if name not in all_options.keys():
        return
        
    options = all_options[name]
    
    if "override" in options and options["override"]:
        for key, value in options["override"].iteritems():
            if value:
                component[key] = value
                
    if "append" in options and options["append"]:
        for key, value in options["append"].iteritems():
            if value and type(value) is list:
                component[key].extend(value)
                
# First do the default option appends and overrides specified in SConstruct file
parse_platform_options("platform_defaults")

# Then do the appends and overrides specified in the project-specific py file
parse_platform_options(component["name"])



# Map build dir and compose absolute paths to source directories
# build_dir is path to the build root, while source_dir is a list of absolute paths
# to directories for source for this buildable
(build_dir, source_dir, set_common_dir) = handle_build_and_source_directories(build, component)

component["mapped_set_common_dir"] = set_common_dir

#print "Define component '%s' of type '%s'"%(component['name'], component['type'])
#print "Mapped source to build dirs:"
#for key, value in source_to_build_map.iteritems():
#    print "  %s:%s"%(key, value)

#import pdb
#pdb.set_trace()

# 1)  Find all files in source dir/dirs, remap their name into build_dir
mirrored_files = []
for src_dir in source_dir:
    #print "  Scanning directory '%s'"%(src_dir)
    original_files = find_files(src_dir, "c") + find_files(src_dir, "cpp")    
    build_address = source_to_build_map[src_dir]
    mirrored_files.extend([remap_path(src_dir, build_address, file) for file in original_files])

# 1b) Remove excluded files
for regexp in component["exclude_strings"] + build["platform_exclude_strings"]:
    mirrored_files = [f for f in mirrored_files if not re.match(regexp, f)]
    
# 2)  Declare build actions
if component["type"] is "library":
    build_library(build, component, mirrored_files, build_dir, source_dir)
elif component["type"] is "shared_library":
    build_shared_library(build, component, mirrored_files, build_dir, source_dir)
elif component["type"] is "program":
    build_program(build, component, mirrored_files, build_dir)

# 3a) Declare pre-build action
if "preaction" in build and "buildaction" in component:
    for action in build["preaction"]:
        component["env"].AddPreAction(component["buildaction"], action)      

# 3b) Declare post-build action
if "postaction" in build and "buildaction" in component:
    for action in build["postaction"]:
        component["env"].AddPostAction(component["buildaction"], action)      

  