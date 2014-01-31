#
# Utility functions to initialize confguration structures
#

import os
import os.path
import string

# Show the components that are built as well as paths and stuff
AddOption("--verbose", dest = "verbose", nargs = 0, help = "verbose build info")
if (GetOption("verbose") is None):
    debug_messages = False
        # don't show scons-generated command lines
    #SetOption("quiet", 1)  # won't work... -s, --silent or --quiet would hide the build commands
else:
    debug_messages = True
  

#==========================================
# Utils
#==========================================

def add_key(component, name, var):
    """
    Add a key to a component
    Encapsulates a key value in a list in case it isn't a list
    """
    if type(var) is list:
        component[name] = var
    else:
        component[name] = [var]


#==========================================
# Library path configuration
#==========================================

def linux_shared_library_link_options_no_rpath(env, lib_tuple):
    sharedLibraryPath = lib_tuple[1]

def linux_shared_library_link_options(env, lib_tuple):
    sharedLibraryPath = lib_tuple[1]
    if (debug_messages):
        print "   ## DBG ## sharedLibraryPath:", sharedLibraryPath
    env.Append(LINKFLAGS = "-z origin")
    #env["RPATH"] = env.Literal(sharedLibraryPath)
    env.Append(RPATH = env.Literal(sharedLibraryPath))

def shared_library_to_tuple(lib):
    lib_path_string = os.path.abspath(lib["library_path"][0])
    if (debug_messages):
        print " ** DBG ** " , lib_path_string
    return (lib["apply"], lib_path_string)
    
def linux_shared_library(library_path, include_path):
    # We want shared library options to be hashable
    shared_library = {}

    add_key(shared_library, "library_path", library_path)
    add_key(shared_library, "include_path", include_path)
    shared_library["apply"] = linux_shared_library_link_options

    return shared_library
    
def external_static_library(library_path, include_path):
    static_library = {}
    add_key(static_library, "library_path", library_path)
    add_key(static_library, "include_path", include_path)
    return static_library


#==========================================
# Component configuration options
#==========================================

def define_component(name = "", type = "", source_path = [], link_libraries = [], platform_libraries = [], include_path = [], set_common_source_path = [], set_common_source_files = [], library_headers = [],
defines = [], exclude_strings = []):
    """
    Defines a Kanzi component (e.g. Core)
    
    include_path: include paths for component. Prefix path with '#' to declare that
    the path is not relative to the component root. 
    ie. path:= '#/some/dir' will use /some/dir as include path while 
    path := '/some/dir will use <component_root>/some/dir 

    set_common_source_path: is active only with the type 'program_set' and defines a group of source
    files linked with every program in the set

    install_path optionally installs all build results (libraries and programs) to this path under
    the build dir
    """

    component = {}
    component["name"] = name # name
    if type and type not in ["library", "program", "shared_library", "test_set"]:
        print  "Invalid type '", type, "' for component '", name, "' (possible options are 'program', 'library', 'test_set')"
    component["type"] = type # type: library|program|program_set

    add_key(component, "source_directory", source_path)
    add_key(component, "link_libraries", link_libraries)
    add_key(component, "library_headers", library_headers)
    add_key(component, "platform_libraries", platform_libraries)
    add_key(component, "include_path", include_path)
    add_key(component, "set_common_source_path", set_common_source_path)
    add_key(component, "set_common_source_files", set_common_source_files)
    add_key(component, "defines", defines)
    add_key(component, "exclude_strings", exclude_strings)

    if (debug_messages):
        print "Component %s = {"%(component["name"])
        for key, value in component.iteritems():
            if key is not "name":
                print "    %s: %s,"%(key,value)
        print "}"
                
    return component

def compose_platform_script_name(build, component_root,component_name):
    component_script_name = component_name + ".py"
    script_path = os.path.join(
            component_root,
            "configs",
            "platforms",
            build["platform"],
            component_script_name
        )
    
    return script_path

def compose_script_name(build, component_root,component_name):
    component_script_name = component_name + ".py"
    script_path = os.path.join(
            component_root,
            "configs",
            "platforms",
            "common",
            component_script_name
        )
    
    return script_path


#====================================================
# Build configurations for different platforms, or "build types"
#   e.g. release_es2
#====================================================

def build_configuration(cc_flags = [], ar_flags = [], ld_flags=[], shared_libraries = {}, static_libraries={}, cc = "gcc", cxx = "g++", defines = [],
    ar = "", cpppath = [], cppdefines = [], export_path = False, use_rpath= True):
    "Sets up library, header and source paths for a Kanzi build configuration"
    
    config = {}
    if type(cc_flags) is not list:
        cc_flags = [cc_flags]

    if type(ar_flags) is not list:
        ar_flags = [ar_flags]
        
    if type(ld_flags) is not list:
        ld_flags = [ld_flags]

    default_cc_flags = [] 
# string.split("-pedantic -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-overlength-strings -Wstrict-prototypes 
# -Wmissing-prototypes -fno-strict-aliasing")
    default_ld_flags = []
    
    for df in default_cc_flags:
        if df not in cc_flags:
            cc_flags.append(df)

    for df in default_ld_flags:
        if df not in ld_flags:
            ld_flags.append(df)

    config["cxx"] = cxx
    config["cc"] = cc
    config["ar"] = ar
    
    add_key(config, "cc_flags", cc_flags)
    add_key(config, "ld_flags", ld_flags)
    add_key(config, "ar_flags", ar_flags)
    add_key(config, "cpppath", cpppath)
    add_key(config, "cppdefines", defines)

    # Add references to shared libraries
    all_shared_libraries = {}
    if type(shared_libraries) is list:
        for lib in shared_libraries:
            for key, value in lib.iteritems():
                all_shared_libraries[key] = value
    elif type(shared_libraries) is dict:
        all_shared_libraries = shared_libraries
    else:
        raise Exception("Wrong type for parameter 'shared_libraries' (not list or dictionary)")
    config["shared_libraries"] = all_shared_libraries

    # Filter the link-option apply function
    if not use_rpath:
        for shared_lib in config["shared_libraries"].values():
            shared_lib["config"]["apply"] = linux_shared_library_link_options_no_rpath 

    # Add references to static libraries
    all_static_libraries = {}
    if type(static_libraries) is list:
        for lib in static_libraries:
            for key, value in lib.iteritems():
                all_static_libraries[key] = value
    elif type(shared_libraries) is dict:
   #elif type(shared_libraries) is dict:            # used to be like this; inconsistent, probably typo
        all_static_libraries = static_libraries
    else:
        raise Exception("Wrong type for parameter 'shared_libraries' (not list or dictionary)")
    config["external_static_libraries"] = all_static_libraries

    config["export_path"] = export_path
    config["use_rpath"] = use_rpath
    return config

    
#====================================================
# Define build: choose the target conf (build type, e.g. release_es2) and set up (apply) the SCons environment
#==================================================== 
    
def read_options_from_input(available_build_types):
    "Gets a build type (build target conf) named on the command line, e.g. release_es2 (several named => undef. behaviour, chooses one)"

    build_type = None
    for build_flag in available_build_types:
        if build_flag in COMMAND_LINE_TARGETS:
            build_type = build_flag

    if not build_type:
        build_type = "default" 

    if not build_type:
        print("Usage: scons <options> <build-type> <project> \n" +
              "   <options>: -v (verbose) \n"
              "<build-type>: " + str(available_build_types).strip("[]"))             
        Exit(2)

    return build_type
    
def define_build(build_types, platform, build_root, global_utilities_path, preaction = [], postaction = [], platform_exclude_strings = []):
    """
    Sets up the SCons environment
    
    Reads the build type (build target conf) named on the command line, e.g. release_es2 (several named => undef. behaviour, chooses one); uses read_options_from_input
    
    Sets up paths
    
    Sets up Environment()
    
    Returns a dictionary with the configuration data
    """

    # Init config
    build = {}
    build["lib_internal_to_resource_name_map"] = {} # internal name: library name map
    build["libraries"] = {} # Each library name used as a key is also an alias
    build["programs"] = {} # Each program name used as a key is also an alias

    (common_util_path, tail) = os.path.split(os.path.abspath(global_utilities_path)) 
    build["common"] = common_util_path
    build["component_build"] = os.path.join(common_util_path, "component_build.py")

    build["platform"] = platform
    build["platform_options"] = {platform:{}}
    build["preaction"] = preaction
    build["postaction"] = postaction
    build["platform_exclude_strings"] = platform_exclude_strings
    
    
    # Get path to project root
    build["root"] = os.path.abspath(build_root)

    # Apply options from build configuration and create environment as a value in build
    build_type = read_options_from_input(build_types.keys())
    if build_type == "default":
        build_type = build_types["default"]
    
    # Output is in the build root
    build["output"] = os.path.join(build["root"], "output", build["platform"], build_type)

    config = build_types[build_type]

    # Get environment, get common build options from config
    if config["export_path"]:
        build["env"] = Environment(ENV=os.environ)
    else:
        build["env"] = Environment()
  
    env = build["env"]

    build["config_name"] = build_type
    if (debug_messages):
        print " ** build config name set to", build_type

    def set_if_defined(name_str, env_name):
        if config[name_str]:
            env[env_name] = config[name_str]

    set_if_defined("cc", "CC")
    set_if_defined("ar", "AR")
    set_if_defined("cxx", "CXX")

    for a in config["ar_flags"]:
        env.Append(ARFLAGS = a)
    for c in config["cc_flags"]:
        env.Append(CCFLAGS = c)
    for l in config["ld_flags"]:
        env.Append(LINKFLAGS = l)
    if config["cpppath"]:
        env.Append(CPPPATH = config["cpppath"])
    for c in config["cppdefines"]:
        env.Append(CPPDEFINES = c)
    
    Alias(build_type, None)     # this effectively removes the build_type from BUILD_TARGETS

    # Add shared library options to build
    if config["shared_libraries"]:
        for lib_name, lib_config  in config["shared_libraries"].iteritems():
            if (debug_messages):
                print " ** DBG ** lib_name:", lib_name
            lib_property = lib_config["config"]
            lib_bin_name = lib_config["libname"]
            build["lib_internal_to_resource_name_map"][lib_name] = lib_bin_name
            shared_lib_tuple = shared_library_to_tuple(lib_property)
            library_path = lib_property["library_path"]
            include_path = lib_property["include_path"]

            if type(library_path) is not list:
                library_path = [library_path]
            if type(include_path) is not list:
                include_path = [include_path]

            build["libraries"][lib_name] = {"LIBPATH":library_path, 
                                            "CPPPATH":include_path, 
                                            "shared_data": shared_lib_tuple}

    # Add external static library options to build
    if config["external_static_libraries"]:
        for lib_name, lib_config  in config["external_static_libraries"].iteritems():
            lib_property = lib_config["config"]
            lib_bin_name = lib_config["libname"]
            build["lib_internal_to_resource_name_map"][lib_name] = lib_bin_name

            library_path = lib_property["library_path"]
            include_path = lib_property["include_path"]

            if type(library_path) is not list:
                library_path = [library_path]
            if type(include_path) is not list:
                include_path = [include_path]

            build["libraries"][lib_name] = {"LIBPATH":library_path, 
                                            "CPPPATH":include_path, 
                                            "shared_data": None}
                                            
    return build
    
def build_component(build, component_root, component_name):
    """
    Declare components (programs and libraries)
    
    component_root := path that contains the platform specific config file 
                        <component_root>/<component_name>.py
    """

    component_root = os.path.abspath(component_root)
    env = build["env"]

    script_path = compose_script_name(build, component_root, component_name)
    platform_script_path = compose_platform_script_name(build, component_root, component_name)
    if (debug_messages):
        print "# Common script path:   %s" % (script_path)
        print "# Platform script path: %s" % (platform_script_path)

    Export("build")
    Export("define_component")
    Export("component_root")
    Export("component_name")
    
    # First load platform specific overrides (if there are some)
    if os.path.exists(platform_script_path):
        env.SConscript(platform_script_path)
    
    # Build component
    if os.path.exists(script_path):
        env.SConscript(script_path)

