#
# SConstruct script for building applications with Kanzi library
#   Mostly concerned with setting the right paths
#   Also compiler/linker directives and macro re-definitions possible
#
#   Platform: Nokia Maemo
#   Project: Kanzi Library + example applications
#

platform = "maemo5"                             # platform name defines the subdirectory from where to search for the component build scripts

project_root = "../../.."                       # path to root directory where output directory will be placed
engine_root = "../../../../../Kanzi/Engine"     # path to the root of Kanzi engine

common_utilities = engine_root + "/configs/platforms/common/common_utilities.py"    
execfile(common_utilities)                      # read in the functions that will be used for SCons configuration

platform_library_root = engine_root + "/libraries/platforms/linux-arm"   # These paths are actually from /Kanzi/Engine/configs/platforms/common (where common_utilities.py is)
common_library_root = engine_root + "/libraries/common"
  
sdk_library_root = "/scratchbox/users/default/targets/FREMANTLE_ARMEL/usr"   # Path to platform libraries on //serenity

compiler = "/scratchbox/compilers/cs2007q3-glibc2.5-arm7/bin/arm-none-linux-gnueabi-gcc"
cflags = "-DMAEMO"

archiver = "/scratchbox/compilers/cs2007q3-glibc2.5-arm7/bin/arm-none-linux-gnueabi-ar"
buildenv_include_path = "/scratchbox/users/default/targets/FREMANTLE_ARMEL/usr/include"

platform_libraries = [ "freetype", "IMGegl", "srv_um", "itype", "Xau", "X11", "Xdmcp" ]
platform_defines = [ "PNG_NO_MMX_CODE" ]
platform_exclude_strings = [ ".*itype.*" ]


#===========================================
# Define shared library paths for each configuration
#===========================================


config_es1 = linux_shared_library (
    library_path = [ sdk_library_root + "/lib" ],
    include_path = [ sdk_library_root + "/include" ]
)

shared_libraries_es1 = {
    "EGL"    : {"libname" : "EGL",      "config" : config_es1},
    "GL"     : {"libname" : "GLES_CM",  "config" : config_es1},
    "IMGegl" : {"libname" : "IMGegl",   "config" : config_es1},
    "srv_um" : {"libname" : "srv_um",   "config" : config_es1},
    "Xdmcp"  : {"libname" : "Xdmcp",    "config" : config_es1},
    "Xau"    : {"libname" : "Xau",      "config" : config_es1},
    "X11"    : {"libname" : "X11",      "config" : config_es1}
}


config_es2 = linux_shared_library (
    library_path = [ sdk_library_root + "/lib" ],
    include_path = [ sdk_library_root + "/include" ]
)

shared_libraries_es2 = {
    "EGL"    : {"libname" : "EGL",      "config" : config_es2},
    "GL"     : {"libname" : "GLESv2",   "config" : config_es2},
    "IMGegl" : {"libname" : "IMGegl",   "config" : config_es2},
    "srv_um" : {"libname" : "srv_um",   "config" : config_es2},
    "Xdmcp"  : {"libname" : "Xdmcp",    "config" : config_es2},
    "Xau"    : {"libname" : "Xau",      "config" : config_es2},
    "X11"    : {"libname" : "X11",      "config" : config_es2}
}


#===========================================
# Static library paths
#===========================================


libjpeg_library_config = external_static_library (
    library_path = [platform_library_root + "/libjpeg/lib"],
    include_path = [common_library_root + "/libjpeg/include"]
)
    
libpng_library_config = external_static_library (
    library_path = [platform_library_root + "/libpng/lib"],
    include_path = [
        common_library_root + "/libpng/include",
        common_library_root + "/zlib/include"
    ]
)

zlib_library_config = external_static_library (
    library_path = [platform_library_root + "/zlib/lib"],
    include_path = [common_library_root + "/zlib/include"]
)

itype_library_config = linux_shared_library(
    library_path = [platform_library_root + "/itype/lib"],
    include_path = [
        common_library_root + "/itype/include",
        platform_library_root + "/itype/include"
    ]
)

freetype_library_config = external_static_library (
    library_path = [platform_library_root + "/freetype/lib"],
    include_path = [common_library_root + "/freetype/include"]
)

lua_library_config = external_static_library (
    library_path = [platform_library_root + "/lua/lib"],
    include_path = [common_library_root + "/lua/include"]
)

tolua_library_config = external_static_library (
    library_path = [platform_library_root + "/lua/lib"],
    include_path = [common_library_root + "/lua/include"]
)

external_static_libraries_common = {
    "libjpeg" : {"libname" : "jpeg",    "config" : libjpeg_library_config},
    "libpng"  : {"libname" : "png",     "config" : libpng_library_config},
    "zlib"    : {"libname" : "z",       "config" : zlib_library_config},
    "freetype": {"libname" : "freetype","config" : freetype_library_config},
    "itype"   : {"libname" : "itype",   "config" : itype_library_config},
    "lua"     : {"libname" : "lua",     "config" : lua_library_config},
    "tolua"   : {"libname" : "tolua++", "config" : tolua_library_config}    
}


#===============================================
# Define platforms
#=============================================== 

available_build_types = {

    "release_es1" : build_configuration (
        cc_flags = "-O2 " + cflags,
        shared_libraries = [shared_libraries_es1],
        static_libraries = [external_static_libraries_common],
        cc = compiler,
        ar = archiver,
        export_path = True,
        cpppath = buildenv_include_path,
        defines = {"KZ_OPENGL_VERSION" : "KZ_OPENGL_ES_1_1"},
        use_rpath = False
    ),

    "debug_es1" : build_configuration (
        cc_flags = "-g " + cflags,
        shared_libraries = [shared_libraries_es1],
        static_libraries = [external_static_libraries_common],
        cc = compiler,
        ar = archiver,
        export_path = True,
        cpppath = buildenv_include_path,
        defines = {"KZ_OPENGL_VERSION" : "KZ_OPENGL_ES_1_1", "_DEBUG" : 1},
        use_rpath = False
    ),
        
    "release_es2" : build_configuration (
        cc_flags = "-O2 " + cflags,
        shared_libraries = [shared_libraries_es2],
        static_libraries = [external_static_libraries_common],
        cc = compiler,
        ar = archiver,
        export_path = True,
        cpppath = buildenv_include_path,
        defines = {"KZ_OPENGL_VERSION" : "KZ_OPENGL_ES_2_0"},
        use_rpath = False
    ),

    "debug_es2" : build_configuration (
        cc_flags = "-g " + cflags,
        shared_libraries = [shared_libraries_es2],
        static_libraries = [external_static_libraries_common],
        cc = compiler,
        ar = archiver,
        export_path = True,
        cpppath = buildenv_include_path,
        defines = {"KZ_OPENGL_VERSION" : "KZ_OPENGL_ES_2_0", "_DEBUG" : 1},
        use_rpath = False
    ),
        
    "default" : "debug_es1"
} 

build = define_build (
    build_types = available_build_types,
    platform = platform,
    build_root = project_root,
    global_utilities_path = common_utilities,
    platform_exclude_strings = platform_exclude_strings
)

platform_defaults = define_component()
try: platform_defaults["link_libraries"] = platform_libraries
except NameError: pass      # platform_libraries not defined, do nothing
try: platform_defaults["defines"] = platform_defines
except NameError: pass      # platform_defines not defined, do nothing    

build["platform_options"][build["platform"]]["platform_defaults"] = {"append" : platform_defaults}


#================================================
# Project components
#================================================

# Specify the targets that can be built with this scons script

build_component(build, component_root = engine_root, component_name = "System")
build_component(build, component_root = engine_root, component_name = "Core")
build_component(build, component_root = engine_root, component_name = "User")
build_component(build, component_root = engine_root, component_name = "LuaWrapper")
build_component(build, component_root = engine_root, component_name = "ApplicationFramework")

build_component(build, component_root = project_root, component_name = "binary_viewer")
