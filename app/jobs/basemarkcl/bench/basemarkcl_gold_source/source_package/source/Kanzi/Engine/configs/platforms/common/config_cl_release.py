
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
profile("noES").env.Append(CPPDEFINES = "KZ_OPENGL_PLATFORM=KZ_PLATFORM_NONE")
profile("GL21").env.Append(CPPDEFINES = "KZ_OPENGL_VERSION=KZ_OPENGL_2_1")
profile("GL21").env.Append(CPPDEFINES = "KZ_OPENGL_PLATFORM=KZ_PLATFORM_DESKTOP")
profile("ES1").used_libraries += ["opengl_es_1_1"]
profile("ES2").used_libraries += ["opengl_es_2_0"]
profile("GL21").used_libraries += ["opengl_2_1"]

# CCFLAGS goes to both C and C++
profile("debug").env.Append(CCFLAGS = "-g", CPPDEFINES = ["DEBUG", "_DEBUG"])    
profile("release").env.Append(CCFLAGS = "-O2", CPPDEFINES = ["RELEASE"])
profile("profile").env.Append(CCFLAGS = "-pg")#, LINKFLAGS = "-pg")
profile("profile").inherits += ["debug"]


#---------------------------------------------------------#
#   Some common definitions
#---------------------------------------------------------#

m = module("default")


# spice up the compiler warnings
# TODO: what if compiler isn't gcc? Test if SCons handles the flags correctly on other compilers.
#m.env["CCFLAGS"] += "-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-overlength-strings -fno-strict-aliasing "
m.env["CCFLAGS"] += ["-Wall", "-Wextra", "-Wno-unused-parameter", "-Wno-unused-variable", "-Wno-unused-function", "-Wno-overlength-strings"]
m.env["CFLAGS"] += ["-Wstrict-prototypes", "-Wmissing-prototypes"]
#module("default").env["CCFLAGS"] += "-pedantic "

# TODO: should be a profile?
#m.env.Append(CPPDEFINES = "ENABLE_LUA")

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

debuglog("Reading the Kanzi Engine module definitions; using Kanzi binary distribution")

# add the kanzi precompiled library path to all projects
module("default").env["LIBPATH"] += [engine_root + sep + "lib" + sep + platform + sep + profile_string]
module("default").used_libraries += ['freetype', 'zlib', 'lua', 'libjpeg', 'freetype', 'libpng']


header_root = engine_root + sep + "headers" + sep + "system"
path_candidates  = [header_root + sep + "common" + sep + "include"]
path_candidates += [header_root + sep + "platforms" + sep + p + sep + "include" for p in platform_attributes]
path_candidates += [header_root + sep + "environments" + sep + p + sep + "include" for p in platform_attributes]
library("system").include_paths = filter(exists, path_candidates)
library("system").binaries = ["system"]

library("core").include_paths = [engine_root + sep + "headers" + sep + "core" + sep + "include"]
library("core").binaries = ["core"]

library("user").include_paths = [engine_root + sep + "headers" + sep + "user" + sep + "include"]
library("user").binaries = ["user"]

header_root = engine_root + sep + "headers" + sep + "application_framework"
path_candidates  = [header_root + sep + "common" + sep + "include"]
path_candidates += [header_root + sep + "platforms" + sep + p + sep + "include" for p in platform_attributes]
library("applicationframework").include_paths = filter(exists, path_candidates)
library("applicationframework").binaries = ["applicationframework"]

library("luawrapper").include_paths = [engine_root + sep + "headers" + sep + "lua_wrapper" + sep + "include"]
library("luawrapper").binaries = ["luawrapper"]


del header_root
