
# Kanzi module definitions for the binary distribution


#---------------------------------------------------------#
#   Profile definitions
#---------------------------------------------------------#

# profile definitions are actually modules that will be inherited by 
#   the "default" module and thus by all modules

profile("ES1").env.Append(CPPDEFINES = "KZ_OPENGL_VERSION=KZ_OPENGL_ES_1_1")
profile("ES2").env.Append(CPPDEFINES = "KZ_OPENGL_VERSION=KZ_OPENGL_ES_2_0")
profile("ES1").used_libraries += ["opengl_es_1_1"]
profile("ES2").used_libraries += ["opengl_es_2_0"]

profile("debug").env.Append(CFLAGS = "-g", CPPDEFINES = ["DEBUG", "_DEBUG"])    
profile("release").env.Append(CFLAGS = "-O2", CPPDEFINES = ["RELEASE"])


#---------------------------------------------------------#
#   Some common definitions
#---------------------------------------------------------#

m = module("default")

# spice up the compiler warnings
m.env["CFLAGS"] += "-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-overlength-strings -Wstrict-prototypes -Wmissing-prototypes -fno-strict-aliasing "

m.env.Append(CPPDEFINES = "ENABLE_LUA")

# group the libs so that the order won't matter
m.env["LINKCOM"] = "$LINK $_LIBDIRFLAGS -Wl,--start-group $SOURCES $_LIBFLAGS -Wl,--end-group $LINKFLAGS -o $TARGET"
m.env["SHLINKCOM"] = "$SHLINK $_LIBDIRFLAGS -Wl,--start-group $SOURCES $_LIBFLAGS -Wl,--end-group $SHLINKFLAGS -o $TARGET"


#---------------------------------------------------------#
#   Define Kanzi Engine modules
#---------------------------------------------------------#

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
