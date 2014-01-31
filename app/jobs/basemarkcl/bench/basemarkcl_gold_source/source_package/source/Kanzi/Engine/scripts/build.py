#
# Design notes / issues / thoughts:
#
#   Contents, or the plot of the script (updated ?.10.10)
#       Definitions of classes: KzModule, KzLibrary
#           TODO: use KzModule for profiles
#            TODO: move out of this file
#        Utils
#            TODO: move out of this file
#       Init: automation, magic values
#       Parse command line overrides and profiles
#       Set / find the build directories (configs, lib, sources)
#       Apply profiles
#       Load the platform-specific configuration (config.py)
#       Load Kanzi Engine module definitions (config.py)
#       Find library directories
#       Load project-specific build / module definitions (config.py)
#       Complete the module definitions
#       Copy the inherited settings
#       Fetch the sources from source_paths
#       Invoke SConstruct for the modules
#           build target_modules and their dependencies
#
#   Currently this file is executed in the project's config folder (not Kanzi Engine's)
#       using only absolute paths everywhere is very much recommended; things may change in the future
#       user input may be relative to different places, but even then there should be a
#           guessing mechanism to find out the root (see e.g. install_dir guessing, line ~940)
#
#   Override order:
#       Project-specifics
#           command line
#           the SConstruct file
#       Module-specifics
#           module definition files
#               this is actually orthogonal to the first ones. Most things have
#               already been done with cmdline args when module defs are read
#               TODO: check if module- and project-specifics indeed can be made independent
#           profiles
#           autodiscovery from directory structure
#           defaults
#
#   "Source file paths" at KzModule.root/src are added as include paths during automagic search
#
#   Both library and module collections should tolerate mixed strings and class instances
#       at least in all structures that are (partly) input by user
#           e.g. used_libraries, depends
#
#   collections' names are in plural form:
#       used_ available_ required_ libraries modules profiles
#
#   dependencies vs. "inheritance"
#       why inheritance: default/common module settings
#           now in module("default")
#
#   error handling etc. messaging
#       error() - for fatal run-time show-stoppers, will Exit(1)
#       warning() - for non-fatal run-time maybe-show-stoppers, hint where to look for when the thing doesn't work (i.e. will build, might be misconfigured), will continue execution, shown in release build
#       debuglog() - for your information, not shown in release build
#       raise Exception()s - for misspellings etc. in the config code, NON-run-time
#       please don't use print directly!
#
#   there's a special module("default")
#       settings common to all modules
#       platform-specific data
#       TODO: Decide: could be explicitly "special" then
#           OTOH: many things work the same way as with other modules
#
#   "reserved names":
#       modules: default, targets, dummymodule, ES1, ES2, debug, release, all
#       libraries: platform_libs
#
#   profile names & parsing
#       non-case-sensitive
#           compare  p.lower() == q.lower()
#           always store internally the one from available_profiles
#
#   KzLibrary.binaries can be either whole filenames ("libmeh.so") or just libnames ("meh")
#
#   KzModule.inherits can contain both strings and KzModules
#       primarily, KzModules (that is, profile("name")) should be used
#       strings are fetched from the global available_profiles
#       this makes it possible to have KzModule.available_profiles too
#
#   TODO: handle module structure vs. single-module project
#       Done, still needs testing on different projects
#       Also maybe some "keyword design", or config.py syntax design
#       TODO?: autodiscovery of modules at project_root/<module_dir_names>/*module*
#
#   TODO: handle building just some modules
#       Essentially, handle supplying target module name on command line, the way it's supposed
#       Is building a single Kanzi lib necessary? How should they depend? (.a doesn't really "depend")
#
#   TODO: handle building just Engine modules (engine_root == project_root?)
#       Done, needs testing
#
#   TODO: eliminate global variables
#       at least those that don't have direct relevance to the whole project
#       or del them after use so that they won't spill
#
#   TODO: exclude_strings, test
#

# TODO: move classes to separate file(s)
#---------------------------------------------------------#
#   KzModule class
#---------------------------------------------------------#

# module definitions will be
#   in a dict with their name as key
#   new-style classes with relevant properties as members
#   used through a function interface that handles creating and initing

class KzModule(object):
    # TODO: better documentation; autogenerated members; lib member(s)
    # TODO: update and keep up to date!
    """
    KzModule class: Kanzi module definition.
    
    Has following member variables and defaults:
        name                    # module name
        type = None             # one of ["a", "so", "exe", None]
        inherits = ["default"]  # "supermodules" or "parent modules" whose settings are copied here
        depends = []            # (immediate) dependencies; modules that need to be built before this
        used_libraries = []     # (immediate) dependencies; pre-built libraries used in this module
        env                     # environment used when building module
        include_paths = []

        output_path
        binary_name = ""        # the name of the ready-built file (plus extensions, defaults to name)
        postactions = []        # shell commands to be executed after the build
       
        root = ""               # module root; walk this tree while fetching (source) files
        source_paths = []       # source paths to walk when fetching files            
        exclude_regexps = []    # regular expressions that rule out SOURCE FILES during search
        
        sources = []            # list of source files to be compiled; will be found automatically, but can be extended / overridden
    """
    
    def __init__(m, arg, is_profile=False):
        """
        Contructor for module definitions.
        
        If the argument is a string, use it as the name. 
        If the argument is a KzLibrary, just copy its contents.        
        """
            
        # TODO: Define the defaults ~compatibly with old, see old_scons_data_structures.txt
        # TODO: remove reference to project_root, do the output_path construction elsewhere
        # TODO: search module root
        if isinstance(arg, str):
            m.name = arg                # module name
            m.type = None               # one of ["a", "so", "exe", None]
            m.dummy = False             # True on modules that aren't going to produce any output files

            m.inherits = ["default"]    # "supermodules" or "parent modules" whose settings are copied here
            m.depends = []              # (immediate) dependencies; modules that need to be built before this
            m.used_libraries = []       # (immediate) dependencies; pre-built libraries used in this module
            m.available_profiles = {}   # module-specific profile specifications (additional, not overriding)
            
            e = Environment()
            
            # if we're on Windows, replace the Environment with gcc for Android builds
            if "win32" in e["PLATFORM"]: # and platform in ["android"]:
                e = Environment(TOOLS = ["cc", "c++", "gcc", "ar", "gnulink"])
                #e = Environment(TOOLS = ["default", "gnulink", "gcc", "g++", "gfortran", "gas", "ar", "filesystem", "m4", "lex", "yacc", "rpcgen", "jar", "javac", "javah", "rmic", "dvipdf", "gs", "tar", "zip"])
            
            e["LIBS"] = []
            e["LIBPATH"] = []
            e["CPPPATH"] = []
            e["CPPDEFINES"] = []
            m.env = e                   # environment used when building module            
            m.binary_name = m.name      # the name of the ready-built file (plus extensions and what-not, defaults to m.name)
            m.install_dir = ""          # the directory where the final binary will be copied to (or Installed)
            m.preactions = []           # shell commands to be executed before the build
            m.postactions = []          # shell commands to be executed after the build
            
            m.root = ""                 # module root; walk this tree while fetching (source) files            
            m.exclude_regexps = []      # regular expressions that rule out SOURCE FILES during search

            # These will be found automatically, but can be extended / overridden
            # TODO: how to delete/exclude files from sources
            # problem: the configs are read before file search (in order to get the root paths)
            # solution: exclude_regexps
            m.source_paths = []         # source paths to walk when fetching files
            m.include_paths = []
            m.sources = []              # list of source files to be compiled            
            m.output_path = ""          # defaults to  output_root + sep + m.name
            m.search_for_sources = True
            
            # Will be generated by walking the dependency tree
            m.link_libraries = []       # (transient) dependencies; binaries needed for linking
            
            # In case the module is a library, it should also include the info needed to use it.
            # Will be automatically generated when a library module is built
            #m.library = KzLibrary(m.name)
        
        # if the argument is a KzLibrary, just copy its contents
        # TODO: test and verify that this works (especially env._dict)
        elif isinstance(arg, KzModule):
            m.__dict__.update(arg.__dict__)
        else:
            raise TypeError("Tried to create module('" + str(arg) + "'). Module names must be strings.")
    
        # if the module has been marked as profile definition, don't inherit things
        if is_profile:
            m.dummy = True
            m.inherits = []             # profiles won't inherit the "default"
            m.search_for_sources = False
            
    
    # these members won't be inherited
    private_members = [
        "inherits", "dummy",            # inheritance info (obviously, that'd mess things up)
        "available_profiles",           # also handled through KzModule inheritance internally 
        "env",                          # will be separately handled ("deep-copied")
        "name", "binary_name",          # unique identifiers and other things that don't make sense in other modules
        "output_path", "sources", "link_libraries",   # (generated) stuff that isn't supposed to be input in config files
        "root"                          # TODO: should this be here?
        "sources", "search_for_sources"
    ]

    def __str__(m):
        return "module('" + m.name + "')"
        
    def profile(m, name):
        """ Accessor function to module-specific profiles; handles profile creation.
            Profile names are case insensitive.
        """
        name = name.lower()
        if name not in m.available_profiles:        
            m.available_profiles[name] = KzModule(name, is_profile = True)
        return m.available_profiles[name]        
    
    # TODO: never used, how come? (recursive build_module sufficient for everything? If so, remove this)
    def dependencies(m):
        """ Calculate the set of dependencies in the dependency tree, also check for circularity
        
            Arguments:  KzModule m      the module to be processed
            Returns:    list            of the dependencies, sub-dependencies etc.
        """
        # Helper function that keeps track on which modules have been counted (Work-In-Progress, wip)
        def depclos(m, clos, wip):
            if m in wip:
                # TODO: better diagnostic; print the whole circle
                response = "Circular dependency detected when processing module " + m.name
                error(response)
            wip += [m]
            if m in clos:
                return []
            clos += [m]
            for dep in m.depends:
                depclos(available_modules[dep], clos)
            wip.remove(m)    
            return clos        
        return depclos(m, [], [])

    def parents(self, init = True, parent_list = []):
        """ Generate the parents in the inheritance tree e.g. to find properties to be added to self from them.
            Usage:  for p in m.parents()
        """        
        if init:                    # at the beginning, empty the parent_list
            parent_list[:] = [self]
        par = [available_modules[p] for p in self.inherits if isinstance(p, str) and available_modules[p] not in parent_list]
        par += [p for p in self.inherits if isinstance(p, KzModule) and p not in parent_list]
        if par:                     # tree node, recurse
            for p in par:
                if p not in parent_list:
                    parent_list += [p]
                    for pp in p.parents(init = False):
                        yield pp        # pass on the grandparents
                    yield p             # and finally the parent

        
def module(name):
    "Accessor function to modules; handles module creation."
    if isinstance(name, KzModule):
        return name
    if name not in available_modules:        
        available_modules[name] = KzModule(name)
    return available_modules[name]

available_modules = {}    
target_modules = []

# "default" module will be inherited by all built modules
module("default").dummy = True

# "targets" module will be inherited by all build targets
module("targets").dummy = True

# "dummymodule" will not be touched; it is there to see which values have been changed from defaults
module("dummymodule").dummy = True


# TODO: add new profiles also to available_profiles, update used_profiles from cmdline targets    
def profile(name):
    """ Accessor function to profiles; handles corresponding module creation.
        Profile names are case insensitive. """
    if isinstance(name, KzModule):
        return name
    name = name.lower()
    if name not in available_modules:        
        available_modules[name] = KzModule(name, is_profile = True)
    return available_modules[name]

# Build profiles, common to all modules; these will be picked from the command line
# TODO: think about the profile system; callbacks? flexibility? required capabilities?
# TODO: find a better place (a file, profiles.py?)
# TODO: update available_profiles when profile() is called
available_profiles = ["ES1", "ES2", "noES", "GL21","Debug", "Release", "Profile"]
required_profiles = [ ["ES1", "ES2", "noES", "GL21"], ["Debug", "Release", "Profile"] ]    # CNF, that is, one from each group must be selected
used_profiles = []

    
#---------------------------------------------------------#
#   KzLibrary class
#---------------------------------------------------------#

class KzLibrary(object):
    """
    KzLibrary class: Kanzi library definition.
    
    Has following member variables and defaults:
        name
        include_paths = []
        libpaths = []
        binaries = []    
    """
    def __init__(self, arg=None):
        """
        Contructor for library definitions.
        
        If the argument is a string, use it as the name. 
        If the argument is a KzLibrary, just copy its contents.
        """        
        
        if arg is None:
            raise Exception("KzLibrary needs a name for construction")
        
        # if the argument is a string, use it as the name
        if isinstance(arg, str):
            self.name = arg
            self.include_paths = []
            self.libpaths = []
            self.binaries = []
        
        # if the argument is a KzLibrary, just copy its contents
        if isinstance(arg, KzLibrary):
            self.__dict__.update(arg.__dict__)
            
    def __str__(m):
        return "library('" + m.name + "')"
        
    def is_valid(self, prebuilt = True):        
        "Check if the library definition looks legit (paths exist etc.) and ready for use"
        try:
            # Check that name exists
            self.name
            
            # Check that include paths exist
            for p in self.include_paths:
                if not exists(p):
                    #return False
                    raise Exception("Include path " + p + " in library " + self.name + " wasn't found")
                    
            # Check that binaries exist in libpaths
            if prebuilt:
                unchecked_binaries = list(self.binaries)
                for p in self.libpaths:
                    for b in list(unchecked_binaries):
                        candidates = [p + sep + "lib" + b + "." + ext for ext in library_extensions]
                        candidates += [p + sep + b]                        
                        if filter(exists, candidates):
                            if b in unchecked_binaries:
                                unchecked_binaries.remove(b)
                            else:
                                warning("Library binary " + b + " was found in two locations.")
                if unchecked_binaries:
                    response = "Following binaries listed for library " + self.name + " were not found: "
                    response += unchecked_binaries[0]
                    for lib in unchecked_binaries[1:]:
                        response += ", " + lib.name
                    raise Exception(response)
        except:
            # TODO: Consider letting the exception through, modify the call to is_valid
            #   maybe issue a warning?
            #   alternatively replace raise with return False            
            return False
        return True
            
            
def library(name):
    "Accessor function to libraries; handles KzLibrary creation."
    if isinstance(name, KzLibrary):
        return name        
    if name not in available_libraries:
        available_libraries[name] = KzLibrary(name)    
    return available_libraries[name]

available_libraries = {}    

    
#---------------------------------------------------------#
#   Utils
#---------------------------------------------------------#

# using sets would remove duplicates but also mess up the order
def unique(original_list):
    """Remove duplicates from a list without changing the order.
       Only keep the first occurrence"""
    tmplist = []
    seen = {}
    # return [seen.setdefault(e,e) for e in original_list if e not in seen]
    for p in original_list:
        if p not in seen:
            seen[p] = 1
            tmplist.append(p)
    return tmplist

# Windows fix for "The command line is too long" problem
if module("default").env["PLATFORM"] == "win32":

    # modified from SCons/Platform/win32.py
    from SCons.Platform.win32 import exec_spawn
    def my_spawn(sh, escape, cmd, args, spawnenv):
    
        # add environment paths to spawnenv
        from os import environ
        spawnenv["PATH"] += environ["PATH"]
        if "JAVA_HOME" not in spawnenv:
            if "JAVA_HOME" in environ:
                spawnenv["JAVA_HOME"] = environ["JAVA_HOME"]
            else:
                # JAVA_HOME not found; just have a guess
                warning("JAVA_HOME not set, please  SET JAVA_HOME=<your JDK path>")
                spawnenv["JAVA_HOME"] = "C:\\Program Files\\Java\\jdk1.6.0_22"

        debugbreak(8)
        
        # spaces in "Program Files (x86)" cause the executable to be split
        #   in args array => also cmd is faulty
        # fix: find the actual executable by searching for ".exe"
        cmd = ""
        i = 0
        for arg in args:
            cmd += arg + " "
            i += 1
            if ".exe" in arg: break        
        # cmd is now the cmd_line up the .exe, OR the whole command-line if no .exe found
            
        # split ar lines into smaller chunks    
        if "ar.exe" in cmd:            
            # next, find the archive name (last "constant part" of the SCons-generated ar-line)
            for arg in args[i:]:
                cmd += arg + " "
                i += 1
                if ".a" in arg: break
            
            # execute the ar, 10 files at a time
            #import pdb; pdb.set_trace()
            while i < len(args) - 10: 
                cmd_line = escape(cmd + " ".join(args[i:i+10]))
                debuglog(cmd_line)
                exec_spawn([sh, '/C', cmd_line], spawnenv)
                i += 10
            cmd_line = escape(cmd + " ".join(args[i:]))
            debuglog(cmd_line)
            ret = exec_spawn([sh, '/C', cmd_line], spawnenv)
            
        #elif "?" in cmd:  # TODO: other commands need splitting?
        else:
            ret = exec_spawn([sh, '/C', escape(" ".join(args))], spawnenv)
            #if ret > 0:
            #    pdb.set_trace()
        
        if not sh:
            sys.stderr.write("scons: Could not find command interpreter, is it in your PATH?\n")
            ret = 127
            
        return ret

    module("default").env["SPAWN"] = my_spawn
    module("default").env["ARCOMSTR"] = "Archiving $TARGET"      # blank the default one and show custom commands
    #env["PSPAWN"] = my_spawn


#---------------------------------------------------------#
#   Initialization
#   (Auto)magic values
#---------------------------------------------------------#

from os.path import exists, abspath, split
from os import listdir, walk, getcwd, sep
from re import search
from SCons.Util import CLVar
import re
#import sys

usage_help_text = """
Usage:   scons [project name] [profile] ... [parameter=value] ...

Example: scons kzb_player es2 release
Example: scons Scene_manipulation es1 debug verbose=2

Mandatory profile specifications:
    release / debug / profile
    ES1 / ES2 / noES

Supported parameters: (notice: no whitespace around equals sign!)
    engine=[Kanzi Engine directory]    
    project=[project directory]    
    platform=[target platform]    
    name=[project name]    
    verbose=[verbosity level]   
        if verbose => 2, enter debugging session on error or warning
        entering 'c' continues execution, Ctrl+C breaks        
    configfile=[filename], defaults to just "config.py"
        config.py is still executed if no [filename] is found
"""

# Automation, notes:
#
# Configuration parameters that affect the build process, preferably ordered from the most common to the more obscure.
#
# Since they're "just variables", they can be changed in config.py -files.
#   This will affect only the steps that come after, that is:
#       * Complete the module definitions
#       * Copy the inherited settings
#       * Fetch the sources from source_paths
#   If changes are needed for earlier steps, the change should be done in SConstruct file or command-line,
#       and some changes are required in build.py:
#         * check if a variable is already set
#         * add a relevant command-line parse
#
# Project root detection:
#   currently: source_dir_names are sought from successive parent directories
#       "sources" is thus needed there
#   TODO: better ideas?

# magic values to help automagics

source_dir_names = ["src", "source", "sources"]     # in module root, e.g. core_layer/*src*/core/util
library_dir_names = ["lib", "library", "libraries"] # in project root, e.g. Application/*libraries*/common
module_dir_names = ["sources", "module", "modules"] # in project root, e.g. engine/*sources*/core_layer
header_dir_names = ["headers", "include", "includes"] # in bin_dist module or project root, e.g. engine/*headers*/core_layer
excluded_dir_names = [".svn"]                       # directories that shouldn't be touched

source_extensions = ["c", "cpp"]
header_extensions = ["h"]
executable_extensions = ["exe"]                     # this is more a KzModule.type tag than an actual file extension
shared_library_extensions = ["so", "dll"]
static_library_extensions = ["a", "lib"]
library_extensions = shared_library_extensions + static_library_extensions

platform_attributes = []
# TODO, check/test: shouldn't platform_default_type come from module("default")?
platform_default_type = "exe"                       # default output type on the platform (e.g. Android: so)
#platform_library_dirs = [] # perhaps not needed at all. More exotic libs/libpaths can be added "manually" to m.env["LIBS"/"LIBPATH"]

# config file names, tried in listed order while searching for config files
#   can be (front-)appended from the SConstruct(2.) and command-line(1.)
config_file_names = ["config.py"]
if "win32" in module("default").env["PLATFORM"]:
    config_file_names = ["config_windows.py"] + config_file_names

# this is specifically there so that the custom application side of a Kanzi app can be
#   placed in a directory named like this, and so that config.py can still be nicely
#   automated
bad_project_names = ["application"]                 # names we don't want to our projects from autoguesser

remove_duplicate_sources = True


#---------------------------------------------------------#
#   Parse command line arguments and overrides
#---------------------------------------------------------#

# engine directory from command line (currently done already in SConstruct, no harm for redoing?)
if ARGUMENTS.get("engine"):
    engine_root = ARGUMENTS.get("engine")

# root directory for the project
if ARGUMENTS.get("project"):
    project_root = ARGUMENTS.get("project")

# target platform      
if ARGUMENTS.get("platform"):           
    platform = ARGUMENTS.get("platform")

# name of the project (binary name etc.)    
if ARGUMENTS.get("name"):               
    project_name = ARGUMENTS.get("name")
    
# output root directory. Final binaries will be placed there by default
if ARGUMENTS.get("output"):
    output_root = ARGUMENTS.get("output")

# script verbosity level (currently 0..2, TODO: keep the usage_help_text up-to-date about this)
verbosity = 0
if ARGUMENTS.get("verbose"):
    try:
        verbosity = int(ARGUMENTS.get("verbose"))
    except:
        pass

# Name of the config file that is executed to define the build
# Append from the SConstruct file (2nd priority)
try:    
    config_file_names = [config_file_name] + config_file_names
    del config_file_name
except NameError:
    pass
# Append from the command line (1st priority)    
if ARGUMENTS.get("configfile"):
    config_file_names = [ARGUMENTS.get("configfile")] + config_file_names
    

# different behaviour for debuglog, warning and error when debugging
if verbosity <= 0:          # default: show only errors, warnings and SCons messages
    def debuglog(*args):
        pass
    def error(*args, **kwargs):
        print("Error: " + ("".join([str(x) for x in args])))
        if "show_usage" in kwargs and kwargs["show_usage"]:
            print usage_help_text
        Exit(1)
    def warning(*args):
        print("Warning: " + ("".join([str(x) for x in args])))
    def debugbreak(*args):
        pass
else:                       
    # verbosity == 1: show debuglog, no pdb
    # verbosity > 1: debug info, start pdb on error or warning    
    import pdb, sys
    def debuglog(*args):
        if len(args) == 0:
            return
        if isinstance(args[0], int):
            if int(args[0]) <= verbosity:
                print("".join([str(x) for x in args[1:]]))
        else:
            print("".join([str(x) for x in args]))
    def error(*args, **kwargs):
        debuglog("Error at " + sys._getframe(1).f_code.co_filename + ":" + str(sys._getframe(1).f_lineno) + " :  " + ("".join([str(x) for x in args])))
        if "show_usage" in kwargs and kwargs["show_usage"]:
            print usage_help_text        
        if verbosity > 1:
            pdb.set_trace()
        Exit(1)
    def warning(*args):
        debuglog("Warning at " + sys._getframe(1).f_code.co_filename + ":" + str(sys._getframe(1).f_lineno) + " :  " + ("".join([str(x) for x in args])))
        if verbosity > 1:
            pdb.set_trace()
    def debugbreak(*args):
        if len(args) == 0 or not isinstance(args[0], int) or args[0] <= verbosity:
            pdb.set_trace()
        
   
#---------------------------------------------------------#
#   Parse profiles from command line
#---------------------------------------------------------#
    
# profiles to be used in build (debug/release, ES1/ES2)    
for p in available_profiles:
    tmp = [arg for arg in COMMAND_LINE_TARGETS if p.lower() == arg.lower()]
    if tmp:
        used_profiles += [p]
        Alias(tmp[0], None)      # this effectively removes the profile declaration from BUILD_TARGETS
debuglog("Command-line args: ", COMMAND_LINE_TARGETS, "  Profiles: ", used_profiles)        

# generate a string from the profile info (used in output pathname, e.g.)
if len(used_profiles) > 0:            
    profile_string = "_".join(used_profiles)
else:
    profile_string = "undefined_profile"

# check that the necessary profile definitions were given
# this means that from each profile_set there must be at least one member in used_profiles
for profile_set in required_profiles:
    if type(profile_set) is not list or len(profile_set) == 0:
        debuglog("Invalid profile requirement: " + profile_set)
        continue
    # handle separately the (rare) case of single requirement
    if len(profile_set) == 1:
        if not profile_set[0] in used_profiles:
            error("You need to specify " + profile_set[0] + " on the command line.", show_usage = 1)
    else:        
        # if no p is both in used_profiles and in profile_set:
        if not [p for p in used_profiles if p in profile_set]:
            response = "You need to specify either " + profile_set[0]
            for p in profile_set[1:-1]:
                response += ", " + p
            response += " or " + profile_set[-1] + " on the command line."
            error(response, show_usage = 1)

    
#---------------------------------------------------------#
#   Find project_root, platform and project_name (if not specified)
#   Set the build config directories
#---------------------------------------------------------#

# engine_root is probably sought before invoking this file; how else to find this file?
#   search it if engine_root wasn't set
try:
    engine_root
except:    
    def find_kanzi_root():
        from os.path import exists, split  # function encapsulation keeps the imports out of the global scope
        from os import getcwd
        path = getcwd()
        current_dir = "."
        while current_dir:
            candidate = path + sep + "Kanzi" + sep + "Engine"
            if exists(candidate):
                return candidate
            path, current_dir = split(path)      # go one level up
        error("Kanzi Engine root directory not found! Please set it manually in the SConstruct file.")        
    engine_root = find_kanzi_root()

# convert path to absolute and remove the trailing /
if engine_root[-1] is sep:
    engine_root = engine_root[:-1]
engine_root = abspath(engine_root)


# check if project root is given in SConstruct or command line
# 
try:
    project_root                    
except:
    # if not, try to find it automatically
    # assumption1: this script will reside under project_root/configs
    # assumption2: all project_roots (with config subdir) have some source_dir_names subdir
    # assumption3: non-project_roots on the path don't have config subdir
    # TODO: less wild assumptions
    
    # find the last occurrence of config
    path = getcwd()
    i = path.rfind("config")
        
    # check that there's a source sub-directory in the same directory
    if i >= 0:
        for dir in source_dir_names:
            if exists(path[0:i-1] + sep + dir):
                project_root = path[0:i-1]
                break
    del path, i
try:
    project_root
except:
    error("Could not locate the project root. "
          "You may supply it on command line:", show_usage = 1)
    Exit(1)

# convert path to absolute and remove the trailing /
project_root = abspath(project_root)
if project_root[-1] is sep:
    project_root = project_root[:-1]
  
# check if platform is given in SConstruct or command line
try:
    platform
except:
    # if not, try to find it automatically
    # Assumption: this script will reside in directory named after the platform    
    # platform name is checked later to be meaningful
    platform = split(getcwd())[1]

# check if project name is given in SConstruct or command line
try:
    project_name
except:
    # if not, use project root directory name as default
    #   unless the name is bad (e.g. "application") => go one directory up
    #   TODO: make this magic guess more universal
    path, project_name = split(project_root)
    if project_name.lower() in [x.lower() for x in bad_project_names]:
        project_name = split(path)[1]
    
# check if output directory is given in SConstruct or command line    
try:
    output_root
except:
    output_root = project_root + sep + "output" + sep + platform + sep + profile_string

# Build config directories, common to all modules in the project
    # TODO: done in SConstruct currently. Natural place would be here. But how else invoke this file? (PATH for example)
    # otoh: resetting doesn't really matter
common_config_dir = engine_root + sep + "configs" + sep + "platforms" + sep + "common"  
platform_config_dir = engine_root + sep + "configs" + sep + "platforms" + sep + platform
project_config_dir = project_root + sep + "configs" + sep + "platforms" + sep + "common"
project_platform_config_dir = project_root + sep + "configs" + sep + "platforms" + sep + platform


#-------------------------------------------------------------------#
#   Load Engine module definitions and common Kanzi settings
#   Load the platform-specific configuration
#   Apply profiles
#-------------------------------------------------------------------#    

# TODO: Kanzi module and common definitions should come before the platform-specific ones
# problem: modules have paths that depend on platform_attributes
# solution1: string replace, {PLATFORM} -> platform_attributes
# solution2: split kanzi module defs and common defs
    
# Platform-independent module definitions
found = False
for conf in config_file_names:
    if not found and exists(common_config_dir + sep + conf):
        #exec(compile(open(conf).read(), conf, 'exec'), local_vars, global_vars)    # Python 3
        execfile(common_config_dir + sep + conf)                                    # Python 2
        found = True        
if not found:
    error("Couldn't find Engine config " + config_file_names[0] + " at " + common_config_dir)

# If platform_config_dir doesn't exist, the platform string is likely nonsense
if not exists(platform_config_dir):
    error("Cannot find the build definition directory for " + platform + " platform in Kanzi Engine configs")

# Platform-specific build definitions (under Engine, common to all projects)
found = False
for conf in config_file_names:
    if not found and exists(platform_config_dir + sep + conf):
        #exec(compile(open(conf).read(), conf, 'exec'), local_vars, global_vars)    # Python 3
        execfile(platform_config_dir + sep + conf)                                  # Python 2
        found = True        
if not found:
    error("Cannot find " + config_file_names[0] + " at " + platform_config_dir,
          ". It's likely that platform '" + platform + "' isn't valid or that "
          "config file names are incorrectly defined.")
del found          

# Apply the profiles 
# This only connects them to default module, they can be changed later and
#   the changes will be inherited to all modules
for p in used_profiles:
    if p.lower() in available_modules:
        #debuglog("Profile " + p, module(p).__dict__)
        module("default").inherits += [p.lower()]
    else:
        warning("Profile " + p + " was marked to be used but wasn't defined")    

        
#-------------------------------------------------------------------#
#   Find library directories, common to all modules in the project
#       Libraries required to build Kanzi (under Engine/libraries)
#       Project-specific libraries, e.g. PhysicsDemo: Bullet physics library
#       Kanzi modules' paths NOT included
#           (they should be under Engine/output/platform/profile/module once built)
#-------------------------------------------------------------------#    

# Library directories to be searched for binaries and headers
# Order matters! First platform-specific, then common; first project, then engine.
library_dirs_tmp = []
library_dirs_tmp += [project_root + sep + libdir + sep + "platforms" + sep + p for libdir in library_dir_names for p in platform_attributes]
library_dirs_tmp += [project_root + sep + libdir + sep + "common" for libdir in library_dir_names]
library_dirs_tmp += [engine_root + sep + "libraries" + sep + "platforms" + sep + p for p in platform_attributes]
library_dirs_tmp += [engine_root + sep + "libraries" + sep + "common"]

# remove duplicates and non-existant directories, preserve order
library_dirs = []
for dir in library_dirs_tmp:
    if dir not in library_dirs and exists(dir):
        library_dirs += [dir]
        
if len(library_dirs) == 0:
    debuglog("Library directories not found")
    #Exit(1)

# TODO: Decide
# Current implementation: just check the "right" places for libraries and includes
# Also possible: walk the directory tree, or at least search subdirectories in case
#                   either "lib" or "include" wasn't found right away
#   Risk: unwanted files breaking the build (OTOH: this problem is local, won't likely pre-empt anything useful)
#
# Check library directories and parse the found library binary/include files
#   to the dict of available_libraries.
# Don't append to existing library definitions (only binary)
#   same lib has several overlapping binaries => link fails
#   this means that if library binary is set in platforms, it won't be appended
#   also, platform_attributes have a priority; the first that is encountered 
#     will be picked, the rest are ignored
#   includes need to be appended so that platform-specific things are included
#     e.g. iType: platforms/p/itype/include/fs_port.h
debuglog("\nScanning directories for libraries")
for dir in library_dirs:
    for lib in listdir(dir):

        # if there are directories named lib, include, etc., we've hit the root of a library
        # search that directory for binary files
        # and don't continue further into the tree
        path = dir + sep + lib + sep + "lib"        
        if exists(path) and not library(lib).libpaths:  # don't auto-append to existing ones, TODO: member variable to control this? KzLibrary.append_happy
            debuglog("  ", path)
            library(lib).libpaths += [path]         # TODO: unnecessary addition can be harmful?
            for file in [f for f in listdir(path) if f.startswith("lib") and f.split(".")[-1] in library_extensions]:
                debuglog("    ", file)                                
                library(lib).binaries += [file]
        # TODO, decide: if nothing found, walk the directory tree
        #                 and/or include the files in  dir + sep + lib?

        # search a directory for header files
        # add the directory to CPPPATH even if no .h file present
        path = dir + sep + lib + sep + "include"
        if exists(path): # and not library(lib).include_paths:  # DO auto-append to existing ones
            debuglog("  ", path)
            library(lib).include_paths += [path]    
            
debuglog("Library paths searched: ", library_dirs)
debuglog("Libs found: ", available_libraries.keys())
del library_dirs, library_dirs_tmp

   
#---------------------------------------------------------#
#   Load project-specific build / module definitions
#---------------------------------------------------------#

debuglog("\nLoading project-specific build configs...")

# TODO: keep the variables in config files from polluting global namespace
#         at the same time, config files should have access to them (with getters/setters?)
#   possible fix: try variables and del them
         
# if we're building just the Engine, don't run the config.py (second time)
if project_root == engine_root:
    target_modules += ["applicationframework"]   # builds all Engine modules
else:

    # TODO: In order to make config files simple,
    #           read some variables set in config file in their proper places afterwards
    #       Especially for single-module projects the config files should be mostly
    #           configurations like  project_name = "project"  etc.
    # TODO: Specify the "keywords" or variable names, implement checking 
    #target = None
    #sources = []
    #libraries = []
    #include_dirs = []
    
    # Implemented:
    #output_type
    #root
    #    
    
    # Actually used / tested:
    #...
    
    # "initialize" some simple-config variable names so that they can be used with +=
    exclude = []
    libs = []
    used_libraries = []

    # remember modules count so that afterwards it can be checked if new modules were defined
    num_modules_before = len(available_modules)

    # Project configs
    found = False
    for conf in config_file_names:
        if not found and exists(project_config_dir + sep + conf):
            #exec(compile(open(conf).read(), conf, 'exec'), local_vars, global_vars)    # Python 3
            execfile(project_config_dir + sep + conf)                                   # Python 2
            found = True        
    if not found:
        debuglog("Couldn't find project config " + project_config_dir + sep + config_file_names[0])
        
    # Platform-specific project configs
    found = False
    for conf in config_file_names:
        if not found and exists(project_platform_config_dir + sep + conf):
            #exec(compile(open(conf).read(), conf, 'exec'), local_vars, global_vars)    # Python 3
            execfile(project_platform_config_dir + sep + conf)                                   # Python 2
            found = True        
    if not found:
        debuglog("Couldn't find platform-specific project config " + project_platform_config_dir + sep + config_file_names[0])


    # No new module definitions => single-module project
    # Set up with default settings as found in Internal projects
    #   TODO?: could just as well be done properly in an autogenerated config.py
    #   TODO?: smarter way to check if new modules have been defined
    if len(available_modules) == num_modules_before:        

        # TODO: No new module definitions => should look for modules if module_dir_names exist?
        #   currently: directly assumes a single-module build
        #   problem: module_dir_names and source_dir_names may overlap,
        #            still must be able to tell if project is multi-module or
        #              if m.root = project_root (single-module project)
        #            need to check more throughly if the module is valid;
        #            doesn't likely matter much though if unnecessary modules are "found";
        #              they probably won't be accidentally depended from the target
        
        # TODO: finish variable parsing refactor
        # parse also many-module projects from "default"
        def parse_variable(obj, attr, variable, default):
            try:
                value = eval(variable)
                obj.__setattr__(attr, value)
                return value
            except NameError:
                pass
            return None
            
            
        #def parse_append(obj, attr
            
#        m = module(project_name)    
#        if parse_variable(m, "sources", "sources"):
#            del sources
#            module(project_name).search_for_sources = False
            
        
        # Read from config file: sources -> m.sources
        try:
            module(project_name).sources = sources
            del sources
            module(project_name).search_for_sources = False
        except NameError:
            pass
        
        # Read from config file: root -> m.root
        try:
            #if [f for f in listdir(project_root) if f in source_dir_names]:
            module(project_name).root = root
            del root
        except NameError:
            module(project_name).root = project_root
        
        # Read from config file: binary_name -> m.binary_name
        try:
            module(project_name).binary_name = binary_name
            del binary_name
        except NameError:
            try:
                module(project_name).binary_name = platform_binary_name
            except NameError:    
                module(project_name).binary_name = module(project_name).name

        # Read from config file: defines -> env["CPPDEFINES"]
        try:
            module(project_name).env["CPPDEFINES"] += defines
            del defines
        except NameError:
            pass
            
        # Read from config file: compiler_flags -> env["CCFLAGS"]
        try:
            module(project_name).env["CCFLAGS"] += compiler_flags
            del compiler_flags
        except NameError:
            pass

        # Read from config file: exclude -> exclude_regexps
        try:
            if isinstance(exclude, str):
                exclude = [exclude]
            module(project_name).exclude_regexps += list(exclude)
            del exclude
        except NameError:   # exclude not defined
            pass
        except TypeError, ex:   # exclude defined in some funny non-iterable way
            error("'exclude' configuration parameters should be comma-separated strings. ", ex)
            
        # Read from config file: type -> m.type
        try:
            module(project_name).type = output_type
            del output_type
        except NameError:
            # TODO, check/test: shouldn't platform_default_type come from module("default") or module("targets")?
            module(project_name).type = platform_default_type        
            pass
            
        try:
            module(project_name).depends += depends
            del depends
        except NameError:
            # TODO: should appframework be here? Anything else?        
            module(project_name).depends += ["system", "core", "user", "applicationframework"] #, "luawrapper"]

        try:
            if isinstance(libs, str):
                libs = [libs]
            for lib in libs:
                # platform-specific libraries etc. could be "just added as is" => create KzLibrary wrapper
                if lib not in available_libraries:
                    library(lib).binaries = [lib]
                
            module(project_name).used_libraries += libs    
            del libs
        except NameError:
            #module(project_name).used_libraries += [ ? ]
            pass

            
        # TODO: read variables set in the project config file(s)
        #   This allows the config file to be written more cleanly
        #sources
        #libraries
        #include_dir
        
        # TODO: include automagically (only) those libraries that were defined under project_root/libraries
        #module(project_name).used_libraries = available_libraries          
        
        # Read from config file: install_dir -> m.install_dir
        # Default to project_root/bin; on Android, default to libs/armeabi
        try:
            module(project_name).install_dir = install_dir
            del install_dir
        except NameError:
            # module("targets").install_dir is set for Android
            if not module(project_name).install_dir and \
              not module("default").install_dir and \
              not module("targets").install_dir and \
              exists(project_root + sep + "bin"):
                module(project_name).install_dir = project_root + sep + "bin"
            
        target_modules += [project_name]

    # TODO: also process the "target_modules" given on command line    
    for m in target_modules:
        module(m).inherits += ["targets"]
        
    # Complete the module definitions
    #   Fill in compulsory details in modules if missing
    debuglog("\nModule inheritances")
    for m in available_modules.itervalues():

        # add the module-specific profiles as inheritances to the module
        # Apply the profiles 
        # This only connects them to default module, they can be changed later and
        #   the changes will be inherited to all modules
        for p in used_profiles:
            if p.lower() in [ap.lower() for ap in m.available_profiles]:
                #pdb.set_trace()
                debuglog("  Profile ", p, " was defined in ", m)
                m.inherits += [m.profile(p)]
    
        debuglog("  Parents of ", m)
        for p in m.parents():
            debuglog("   ", p)
            
        # Set individual modules' directories
        if not m.dummy:
            if not m.root:            
                for candidate in [project_root + sep + p + sep + m.name for p in module_dir_names]:
                    if exists(candidate):
                        m.root = candidate
                        break
            if not m.output_path:
                m.output_path = output_root + sep + m.name
            if m.install_dir and not exists(m.install_dir):
                if exists(m.root + sep + m.install_dir):
                    m.install_dir = m.root + sep + m.install_dir
                elif exists(project_root + sep + m.install_dir):
                    m.install_dir = project_root + sep + m.install_dir
                elif exists(project_platform_config_dir + sep + m.install_dir):
                    m.install_dir = project_platform_config_dir + sep + m.install_dir
                else:
                    m.install_dir = ""
    
                    
    del num_modules_before    
        
#---------------------------------------------------------#
#   Copy the inherited settings
#      typically from "default" and profiles
#---------------------------------------------------------#

def update_member(d, key, value, dummy):
    "Update a key in dict d with value according to KzModule inheritance rules"

    # TODO: is this correct? Check/test
    # consequence: cannot "override" something with nothing
    #   OTOH: dummyvalues should be exactly nothing so this shouldn't be an issue
    if not value:
        return
        
    if key not in d:
        d[key] = value
    elif d[key] != value:        
        # Append to lists and SCons command-line variables (typically paths, libs, defines)
        if isinstance(value, list) or isinstance(value, CLVar):
            debuglog("  ", key, ": ", d[key], " += ", value)        
            d[key] += value
        elif isinstance(value, set) or isinstance(value, dict):
            debuglog("  ", key, ": ", d[key], " += ", value)        
            d[key].update(value)            
        # Replace strings and functions (typically tool executables, command lines)
        #   ONLY if they haven't been changed into m
        # Also replace unknown SCons configuration variables and numbers
        #   TODO: try to find out append syntax for relevant SCons classes
        elif isinstance(value, str) or isinstance(value, type(update_member)) \
          or isinstance(value, int) or isinstance(value, float) \
          or isinstance(value, bool) or "SCons" in str(value.__class__):
            if d[key] == dummy[key]:
                debuglog("  ", key, ": ", d[key], " <= ", value)        
                d[key] = value
        # Other possibilities: SCons classes and callbacks
        # TODO: how to handle? Probably replace (builders, flag generators, actions)
        # OTOH: good to raise the exception for now since they're not needed currently
        #           also, more complex functionality (that can be anticipated) should
        #           be implemented as a "feature" in KzModule and handled in this script
        else:
            warning("Couldn't handle argument of type ", value.__class__)
        #    raise Exception("Module inheritance doesn't support other types than strings and lists")

# Copy things from m.parents() to m
#   Append to lists
#   Replace strings ONLY IF they haven't been defined previously in m
# TODO: this doesn't really handle well the overlapping settings from several parents
#         should be some kind of breadth-first search instead of depth-first?
for m in available_modules.itervalues():
    if not m.dummy:
        for p in m.parents():
        
            # Update the SCons environment
            debuglog("\nChanges in SCons environment copied '" + p.name + "' -> '" + m.name + "':")        
            for key, value in p.env._dict.iteritems():
                update_member(m.env._dict, key, value, module("dummymodule").env._dict)
            
            # Update the rest of the member variables
            debuglog("\nChanges in member variables copied '" + p.name + "' -> '" + m.name + "':")
            for key, value in p.__dict__.iteritems():
                if key not in KzModule.private_members:
                    update_member(m.__dict__, key, value, module("dummymodule").__dict__)


del update_member

#---------------------------------------------------------#
#   Complete the source_paths list and find source files
#---------------------------------------------------------#

# List of found source files with the path relative to module source path root
#     This ensures that only the first-found file of some name is included
sources_with_path = []

# For each module:
#   check if the module root is under project_root
#   add m.source_paths
#   add sources automagically to the m.sources
for m in available_modules.itervalues():

    # if root was't found while completing the module definition,
    #   the module is a dummy
    # or maybe user doesn't want source file autodiscovery...
    if "root" not in m.__dict__ or not m.root:
        continue
        
    # Find the source_paths under the module
    # NOTE: Order matters! Priority: project > platform > common
        # e.g. PhysicsDemo/sources/Demo/*src*
    m.source_paths += [m.root + sep + src for src in source_dir_names]      
        # e.g. sources/application_framework/*platforms/qnx*/src/application
    m.source_paths += [m.root + sep + "platforms" + sep + p + sep + "src"
                        for p in platform_attributes] 
        # e.g. sources/system_layer/*environments/qnx*/src/system/input
    m.source_paths += [m.root + sep + "environments" + sep + p + sep + "src" 
                        for p in platform_attributes] 
        # e.g. sources/application_framework/*common*/src/application
    m.source_paths += [m.root + sep + "common" + sep + "src"]
        # filter out the trash (older trash goes at the same) and remove duplicates
    m.source_paths = unique(filter(exists, m.source_paths))
    
    if not m.source_paths:
        warning("No source paths were found under ", m)
    debuglog("\nSource paths for ", m)
    debuglog("  Searching source files starting from ", m.root)

    # Fetch the sources from source_paths
    if m.search_for_sources:
        for dir in m.source_paths:        
            debuglog("    ", dir)
            for root, dirs, files in walk(dir):
                #dirs = filter(lambda d: d not in excluded_dir_names, dirs)
                dirs = [d for d in dirs if d not in excluded_dir_names]
                for file in [root + sep + f for f in files 
                                for ext in source_extensions 
                                if f.endswith("." + ext)]:
                    rel = file.replace(dir, "")    # get the part without module source path root
                    if remove_duplicate_sources and rel in sources_with_path:
                        debuglog("    > " + rel + " has already been previously added, skipping.")
                    elif [1 for regexp in m.exclude_regexps if re.search(regexp, rel)]:
                        debuglog("    > " + rel + " is marked to be excluded, skipping.")
                    else:
                        debuglog(2, "      " + rel)
                        m.sources.append(file)
                        sources_with_path.append(rel)

    # Prepend the source path if necessary
    for src in list(m.sources):
        if not exists(src):
            full_src = [path + sep + src for path in m.source_paths if exists(path + sep + src)]
            if full_src:
                m.sources.append(full_src[0])
                            
    m.sources = unique(filter(exists, m.sources))
    debuglog("  %d source files" % len(m.sources))
    if not m.sources:
        warning("No source files were found for " + str(m))

    # Add the source paths as include paths    
    m.include_paths += m.source_paths

        
    # Find separate include/header paths under the module, if any
        # e.g. Engine/tests/cppunit_tests/*include*
    m.include_paths += [m.root + sep + inc for inc in header_dir_names]   
    m.include_paths = filter(exists, m.include_paths)
    
                    
#---------------------------------------------------------#
#   Invoke SConstruct for the modules
#---------------------------------------------------------#

def build_module(m, wip_modules = [], built_modules = []):
    """
    Builds the given KzModule. Checks the configs and recursively builds the dependencies.
    
    arguments:      KzModule m      the module to be built
                    | OR str m      name of the module to be built
    
    returns:        set<KzLibrary>  of dependencies    
    """
    #wip_modules         # Work-In-Progress modules
    #built_modules       # KzModules that have been built
    
    # Handle the "function overload" by converting the string argument into a KzModule
    if isinstance(m, str):
        if m in available_modules:
            m = module(m)       # convert str -> KzModule
        else:
            error("module('" + m + "') isn't defined.")
    if not isinstance(m, KzModule):
        raise TypeError("build_module requires a str (module name) or a KzModule")

    # If already built once, just ignore the call
    if m in built_modules:
        return built_modules
    
    # Check for circularity, i.e. if this module depends on itself (likely through other modules)
    if m in wip_modules:
        # TODO: better diagnostic; print the whole circle
        response = "Circular dependency (or modules with same name) detected when processing " + str(m)
        error(response)        
    wip_modules += [m]
        
    debuglog("Setting up ", m, "...")
        
    # Check that all depended modules are built, build recursively if needed
    # Collect the link_libraries so that dependencies' library binaries are also taken to the link step
    for dep in m.depends:
        if isinstance(dep, str):
            if dep in available_modules:
                dep = module(dep)   # convert str -> KzModule
            elif dep in available_libraries:
                debuglog("module('" + dep + "') marked as dependency in ", m, " wasn't defined. "
                         "Assuming library('" + dep + "') was meant instead.") 
                m.used_libraries += [dep]
            else:    
                error("Couldn't find module('" + dep + "') required by ", m, ". \n"
                      "Consider adding the definition to the build config file " + project_config_dir + sep + config_file_names[0] + "\n")
                
        if isinstance(dep, KzModule):
            # Build recursively and check for success
            build_module(dep)            
            if dep not in built_modules:
                error(dep, " not found after issuing a build; check settings")
                
            if dep.type in library_extensions:
                if "library" in dep.__dict__ and dep.library.is_valid(prebuilt = False):
                    m.env.Depends(m.name, dep.name)     
                    m.used_libraries += [dep.library]
                    m.link_libraries += dep.link_libraries
                    #m.link_libraries = unique(m.link_libraries + [dep.library] + dep.link_libraries)
                    #m.include_paths = unique(m.include_paths + dep.library.include_paths)
                else:
                    warning(dep, " marked as a dependency despite not having a valid library definition.\n"
                            "Consider revising the build config file" + project_config_dir + sep + config_file_names[0] + "\n")  # TODO: better error message
                    #raise Exception(dep, " is invalid or not a library module")
            else:        
                warning(dep, " marked as a dependency despite being of type " + dep.type)

    # Check that all pre-built library dependencies are available, add info to the module
    for lib in m.used_libraries:
        if isinstance(lib, str):
            if lib in available_libraries:
                lib = library(lib)      # convert str -> KzLibrary
            else:
                error("Couldn't find library('" + lib + "') required by ", m, ". \n"
                      "Consider adding the definition manually to one of the "
                      "following build config files:\n"
                      "  " + project_config_dir + sep + config_file_names[0] + " "
                      "if the library is project-specific, or\n"
                      "  " + platform_config_dir + sep + config_file_names[0] + " "
                      "if the library is platform-specific")            
        if isinstance(lib, KzLibrary): #and if library(lib).is_valid():
            m.link_libraries = unique(m.link_libraries + [lib])
            m.include_paths = unique(m.include_paths + lib.include_paths)
            
    # Finally, pass the module definition to SCons
    # Modules with  type == None  or without source file aren't meant to 
    #   produce anything to SCons. They may have dependencies, though, 
    #   and that's why the previous steps were necessary
    if not m.dummy and m.type and m.sources:            
            
        # add the dependencies to build environment
        # header paths are taken from the immediate dependencies, they're needed only when compiling
        #for lib in _libs:
        #    for path in lib.include_paths:
        #        m.env.AppendUnique(CPPPATH = path)            
                
        # library binaries (and thus libpaths) are needed throughout the dependency tree, on the link step
        for lib in unique(m.link_libraries):        
            for binary in unique(lib.binaries):
                # parse filename -> libname if necessary
                if binary.startswith("lib") and "." in binary and binary.split(".", 1)[1] in library_extensions:
                    # Linux libname
                    binary = binary[3:].split(".", 1)[0]
                    m.env.AppendUnique(LIBS = binary)
                elif "." in binary and binary.split(".", 1)[1] in library_extensions:
                    # Windows libs might not have the lib prefix
                    binary = binary.split(".", 1)[0]
                    m.env.AppendUnique(LIBS = binary)                    
                else:
                    # if binary contains ".", it's likely a filename,
                    #   otherwise it would be a libname as it is
                    if "." in binary:
                        warning("Binary " + binary + " was found in library('"
                                "" + lib.name + "').binaries, however it's not "
                                "of form libXYZ.a (or it has an unknown library "
                                "file extension) so it might not work correctly")
                    m.env.AppendUnique(LIBS = binary)
                    
            for libpath in unique(lib.libpaths):
                m.env.AppendUnique(LIBPATH = libpath)

        # add module-specific include paths to environment
        # especially useful for platform includes in module("default")    
        for p in unique(m.include_paths):
            m.env.AppendUnique(CPPPATH = p)
        
        # set the output directory
        # note: src_dir means in SCons the directory that is the "source" of the build, not source files necessarily
        m.env.VariantDir(m.output_path, m.root, duplicate = 0)
        sources_in_output_path = [ src.replace(m.root, m.output_path) for src in m.sources ]

        # TODO: handle in some smart way (also in m.env.VariantDir!)
        # e.g.: /my/src -> output_path/my/src
        if [1 for src in sources_in_output_path if not src.startswith(m.output_path)]:
            error("File " + file + " isn't located under module root " + m.root)

        
        # pass the build command to SCons
        if m.type in shared_library_extensions:
            debugbreak(5)
            m.target = m.env.SharedLibrary(target = m.output_path + sep + m.binary_name, 
                                           source = sources_in_output_path)        
            debuglog("Dynamic library target: " + str(m.target))
        elif m.type in static_library_extensions:
            debugbreak(6)
            m.target = m.env.StaticLibrary(target = m.output_path + sep + m.binary_name, 
                                           source = sources_in_output_path)
            debuglog("Static library target: " + str(m.target))
        elif m.type in executable_extensions:
            debugbreak(5)
            m.target = m.env.Program(target = m.output_path + sep + m.binary_name, 
                                     source = sources_in_output_path)
            debuglog("Executable target: " + str(m.target))
        else:
            raise Exception("Module " + m.name + " has unsupported type "
                            "specification: " + m.type + "\n"
                            "See the build config file " + project_config_dir + sep + config_file_names[0] + "\n")

        # execute the preactions
        #  e.g. Android: update project (could be done in postaction but order is important here)
        #           (order: this is coupled with ES version, packaging with deb/rel; this has to come first)
        for action in m.preactions:
            m.env.AddPreAction(m.target, action)
            debuglog("  Preaction: " + action)     

        # If an installation location for a file has been defined, use that instead
        debuglog(4, "" + str(m) + ", install_dir " + m.install_dir)
        if m.install_dir:
            #pdb.set_trace()
            m.target = m.env.Install(m.install_dir, source = m.target)
            debuglog("  %s is to be installed to %s." % (m, m.target[0]))
        
        # execute the postactions 
        #  e.g. Android: packaging
        for action in m.postactions:
            #pdb.set_trace()
            act = m.env.AddPostAction(m.target, action)
            debuglog("  Postaction: %s (%s) " % (action, str(act[0])))
        
        m.env.AddPostAction(m.target, "echo Done building %s " % m.target[0])
        debuglog(4, "Configuring %s" % m.target[0])
            
        # Add an Alias so that SCons can associate the build target with the module name
        m.env.Alias([m.name, "all"], m.target)
                            
        # Add the library to available_libraries
        if m.type in library_extensions:
            m.library = KzLibrary(m.name)
            # TODO: include paths for compiling a library and using it are two
            # separate concepts (we don't want prebuilt libraries' include paths to go along)
            m.library.include_paths += m.source_paths #m.include_paths
            m.library.binaries += [split(m.target[0].abspath)[1]]
            m.library.libpaths += [split(m.target[0].abspath)[0]]    
    else:
        debuglog("Nothing to build in ", m)
    
    # We're done with m, remove it from WIP list
    built_modules += [m]
    if m in wip_modules:
        wip_modules.remove(m)
    
    debuglog(m, " done.")
    return built_modules

    
# Tell SCons to build the target module(s) and dependencies

used_modules = []          # modules to be included in the build, i.e. target modules and dependencies
for m in target_modules:
    used_modules += build_module(m)

# This is a bit hackish. Essentially we have overridden the SCons' idea of reading targets from command line
# The extra arguments that were not consumed by profile specification parsing should still be valid targets
# TODO: doesn't work with current scons version, FIX
#if COMMAND_LINE_TARGETS:
#    Depends(COMMAND_LINE_TARGETS[0], [m.target for m in used_modules])
#else:
#    Default([m.target for m in used_modules])

del build_module, used_modules

