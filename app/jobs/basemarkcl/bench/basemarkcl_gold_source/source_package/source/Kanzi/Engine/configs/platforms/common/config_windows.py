
# Kanzi module definitions for the source distribution
# This file is called from build.py:~650


#---------------------------------------------------------#
#   Profile definitions
#---------------------------------------------------------#

# profile definitions are actually modules that will be inherited by 
#   the "default" module and thus by all modules

profile("ES1").env.Append(CPPDEFINES = "KZ_OPENGL_VERSION=KZ_OPENGL_ES_1_1")
profile("ES2").env.Append(CPPDEFINES = "KZ_OPENGL_VERSION=KZ_OPENGL_ES_2_0")
profile("noES").env.Append(CPPDEFINES = "KZ_OPENGL_VERSION=KZ_OPENGL_NONE")
profile("ES1").used_libraries += ["opengl_es_1_1"]
profile("ES2").used_libraries += ["opengl_es_2_0"]

# CCFLAGS goes to both C and C++
profile("debug").env.Append(CCFLAGS = "-g", CPPDEFINES = ["DEBUG", "_DEBUG"])    
profile("release").env.Append(CCFLAGS = "-O2", CPPDEFINES = ["RELEASE"])
profile("profile").env.Append(CCFLAGS = "-pg")#, LINKFLAGS = "-pg")
profile("profile").inherits += ["debug"]


#---------------------------------------------------------#
#   Some common definitions
#---------------------------------------------------------#

m = module("default")

# TODO: should be a profile?
#m.env.Append(CPPDEFINES = "ENABLE_LUA")

# if MSVC is would be used, add flags that are required by MSVC
if "msvc" in m.env["TOOLS"]:
    m.env["CCFLAGS"].append("/EHsc")
elif "mingw" in m.env["TOOLS"] or "gcc" in m.env["TOOLS"]:
    # spice up the compiler warnings
    # TODO: what if compiler isn't gcc? Test if SCons handles the flags correctly on other compilers.
    #m.env["CCFLAGS"] += "-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-overlength-strings -fno-strict-aliasing "
    m.env["CCFLAGS"] += ["-Wall", "-Wextra", "-Wno-unused-parameter", "-Wno-unused-variable", "-Wno-unused-function", "-Wno-overlength-strings", "-Wstrict-prototypes", "-Wmissing-prototypes", "-fno-strict-aliasing"]
    m.env["CFLAGS"] += ["-Wstrict-prototypes", "-Wmissing-prototypes"]
    #module("default").env["CCFLAGS"] += "-pedantic "

    # group the libs so that the order won't matter
    # NOTE: if the link step feels slow (shouldn't be), try the first alternative,
    #         or comment both out (two lines each)
    #m.env["LINKCOM"] = "$LINK $LINKFLAGS $_LIBDIRFLAGS $SOURCES -Wl,--start-group $_LIBFLAGS -Wl,--end-group -o $TARGET"
    #m.env["SHLINKCOM"] = "$SHLINK $SHLINKFLAGS $_LIBDIRFLAGS $SOURCES -Wl,--whole-archive -Wl,--start-group $_LIBFLAGS -Wl,--end-group -o $TARGET"
    m.env["LINKCOM"] = "$LINK $LINKFLAGS $_LIBDIRFLAGS -Wl,--start-group $SOURCES $_LIBFLAGS -Wl,--end-group -o $TARGET"
    #m.env["SHLINKCOM"] = "$SHLINK $SHLINKFLAGS $_LIBDIRFLAGS -Wl,--whole-archive -Wl,--start-group $SOURCES $_LIBFLAGS -Wl,--end-group -o $TARGET"
    m.env["SHLINKCOM"] = "$SHLINK $_LIBDIRFLAGS -Wl,--start-group $SOURCES $_LIBFLAGS -Wl,--end-group $SHLINKFLAGS -o $TARGET"
    # default LINKCOM: $LINK -o $TARGET $LINKFLAGS $SOURCES $_LIBDIRFLAGS $_LIBFLAGS
    # default SHLINKCOM: $SHLINK -o $TARGET $SHLINKFLAGS $SOURCES $_LIBDIRFLAGS $_LIBFLAGS


#---------------------------------------------------------#
#   Define Kanzi Engine modules
#---------------------------------------------------------#

debuglog("Reading the Kanzi Engine module definitions")

#if "binary_distribution" in used_profiles:
#    library("prebuilt_system").libpaths = ["..."]
#    library("prebuilt_system").include_dirs = ["..."]
#    library("prebuilt_system").libs = ["..."]
#else:

m = module("system")
m.type = "a"
m.root = engine_root + sep + "sources" + sep + "system_layer"
m.output_path = engine_root + sep + "output" + sep + platform + sep + profile_string + sep + "system"
m.install_dir = engine_root + sep + "lib" + sep + platform + sep + profile_string
#m.source_paths += [m.root + sep + "common" + sep + "src"]
#m.source_paths += [m.root + sep + "platforms" + sep + p + sep + "src" for p in platform_attributes]
#m.source_paths += [m.root + sep + "environments" + sep + p + sep + "src" for p in platform_attributes]
#m.source_paths = filter(exists, m.source_paths)
#m.include_paths += m.source_paths    
#m.library.include_paths += m.include_paths

m = module("core")
m.type = "a"
m.depends += ["system"]
m.used_libraries += ["freetype"]
m.used_libraries += ["itype"]      # TODO: add iff KZ_ITYPE_ENABLED
#m.exclude_strings += ["itype"]    # TODO: iff not KZ_ITYPE_ENABLED
m.used_libraries += ["libpng", "libjpeg", "zlib"]
m.root = engine_root + sep + "sources" + sep + "core_layer"
m.output_path = engine_root + sep + "output" + sep + platform + sep + profile_string + sep + "core"
m.install_dir = engine_root + sep + "lib" + sep + platform + sep + profile_string
#m.source_paths += [m.root + sep + "src"]
#m.include_paths += m.source_paths    
#m.library.include_paths += m.include_paths
#if "mingw" in m.env["TOOLS"] or "gcc" in m.env["TOOLS"]:
#    m.env["CCFLAGS"] += ["-ansi"]

m = module("user")
m.type = "a"
m.depends += ["system", "core"]
m.used_libraries += ["lua"]        # TODO: add iff ENABLE_LUA
#, "libjpeg"]        
m.root = engine_root + sep + "sources" + sep + "user_layer"
m.output_path = engine_root + sep + "output" + sep + platform + sep + profile_string + sep + "user"
m.install_dir = engine_root + sep + "lib" + sep + platform + sep + profile_string
#m.source_paths += [m.root + sep + "src"]
#m.include_paths += m.source_paths    
#m.library.include_paths += m.include_paths
#if "mingw" in m.env["TOOLS"] or "gcc" in m.env["TOOLS"]:
#    m.env["CCFLAGS"] += ["-ansi"]

m = module("applicationframework")
m.type = "a"
m.depends += ["system", "core", "user"]
#m.depends += ["luawrapper"]                     # TODO: add iff ENABLE_LUA
m.used_libraries += ["lua"]        # TODO: add iff ENABLE_LUA
m.root = engine_root + sep + "sources" + sep + "application_framework"    
m.output_path = engine_root + sep + "output" + sep + platform + sep + profile_string + sep + "applicationframework"
m.install_dir = engine_root + sep + "lib" + sep + platform + sep + profile_string
#m.source_paths += [m.root + sep + "common" + sep + "src"]
#m.source_paths += [m.root + sep + "platforms" + sep + p + sep + "src" for p in platform_attributes]
#m.source_paths = filter(exists, m.source_paths)
#m.include_paths += m.source_paths    
#m.library.include_paths += m.include_paths

#m = module("luawrapper")
#m.type = "a"
#m.depends += ["system", "core", "user"]       # circular dependencies, should be ignored. (TODO?)
#m.root = engine_root + sep + "sources" + sep + "lua_wrapper"    
#m.output_path = engine_root + sep + "output" + sep + platform + sep + profile_string + sep + "luawrapper"
#m.install_dir = engine_root + sep + "lib" + sep + platform + sep + profile_string
#m.used_libraries += ["lua"] 


del m
