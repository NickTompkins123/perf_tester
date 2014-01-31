# Path to Kanzi/Engine/configs/platforms/common/common_utilities.py
common_utilities = "../common/common_utilities.py"
execfile(common_utilities)


#===========================================
# Define shared library options
#===========================================


libjpeg_library_config = external_static_library(
        library_path =  ['../../../libraries/platforms/linux-arm/libjpeg/lib'],
        include_path =  ['../../../libraries/common/libjpeg/include']
        )

libpng_library_config = external_static_library(
        library_path =  ['../../../libraries/platforms/linux-arm/libpng/lib'],
        include_path =  ['../../../libraries/common/libpng/include',
                         '../../../libraries/common/zlib/include']
        )

zlib_library_config = external_static_library(
        library_path =  ['../../../libraries/platforms/linux-arm/zlib/lib'],
        include_path =  ['../../../libraries/common/zlib/include']
        )

itype_library_config = linux_shared_library(
    library_path = ['../../../libraries/platforms/linux-arm/itype/lib'],
    include_path = ['../../../libraries/common/itype/include','../../../libraries/platforms/linux-arm/itype/include']
)

external_static_libraries_common = {
    'libjpeg' : {'libname': 'jpeg', 'config':libjpeg_library_config},
    'libpng' : {'libname': 'png', 'config':libpng_library_config},
    'zlib' : {'libname': 'z', 'config':zlib_library_config},
    'itype'   : {'libname' : 'itype','config' : itype_library_config}
}


khronos_library_config_es1 = linux_shared_library(
        library_path =  ['/scratchbox/users/default/targets/FREMANTLE_ARMEL/usr/lib'],
        include_path =  ['/scratchbox/users/default/targets/FREMANTLE_ARMEL/usr/include']
        )

shared_libraries_es1 = {
    'EGL' : {'libname':'EGL','config':khronos_library_config_es1},
    'GL' : {'libname':'GLES_CM', 'config':khronos_library_config_es1},
    'IMGegl' : {'libname':'IMGegl', 'config':khronos_library_config_es1},
    'srv_um' : {'libname':'srv_um', 'config':khronos_library_config_es1}
}


khronos_library_config_es2 = linux_shared_library(
        library_path =  ['/scratchbox/users/default/targets/FREMANTLE_ARMEL/usr/lib'],
        include_path =  ['/scratchbox/users/default/targets/FREMANTLE_ARMEL/usr/include']
        )

shared_libraries_es2 = {
    'EGL' : {'libname': 'EGL', 'config':khronos_library_config_es2},
    'GL' : {'libname':'GLESv2', 'config':khronos_library_config_es2},
    'IMGegl' : {'libname':'IMGegl', 'config':khronos_library_config_es2},
    'srv_um' : {'libname':'srv_um', 'config':khronos_library_config_es2},
    'X11' : {'libname':'X11', 'config':khronos_library_config_es2}
}

#===============================================
# Define platform
#=============================================== 

available_build_types = {
    'release_es1': build_configuration(
        cc_flags = '-O2',
        shared_libraries = [shared_libraries_es1],
        static_libraries = [external_static_libraries_common],
        cc = '/scratchbox/compilers/cs2007q3-glibc2.5-arm7/bin/arm-none-linux-gnueabi-gcc',
        ar = '/scratchbox/compilers/cs2007q3-glibc2.5-arm7/bin/arm-none-linux-gnueabi-ar',
        export_path = True,
        cpppath = '/scratchbox/users/default/targets/FREMANTLE_ARMEL/usr/include',
        defines = {'KZ_OPENGL_VERSION':'KZ_OPENGL_ES_1_1'},
        use_rpath = False
        ),

    'debug_es1': build_configuration(
        cc_flags = '-g',
        shared_libraries = [shared_libraries_es1],
        static_libraries = [external_static_libraries_common],
        cc = '/scratchbox/compilers/cs2007q3-glibc2.5-arm7/bin/arm-none-linux-gnueabi-gcc',
        ar = '/scratchbox/compilers/cs2007q3-glibc2.5-arm7/bin/arm-none-linux-gnueabi-ar',
        export_path = True,
        cpppath = '/scratchbox/users/default/targets/FREMANTLE_ARMEL/usr/include',
        defines = {'KZ_OPENGL_VERSION':'KZ_OPENGL_ES_1_1'},
        use_rpath = False
        ),
    'release_es2': build_configuration(
        cc_flags = '-O2',
        shared_libraries = [shared_libraries_es2],
        static_libraries = [external_static_libraries_common],
        cc = '/scratchbox/compilers/cs2007q3-glibc2.5-arm7/bin/arm-none-linux-gnueabi-gcc',
        ar = '/scratchbox/compilers/cs2007q3-glibc2.5-arm7/bin/arm-none-linux-gnueabi-ar',
        export_path = True,
        cpppath = '/scratchbox/users/default/targets/FREMANTLE_ARMEL/usr/include',
        defines = {'KZ_OPENGL_VERSION':'KZ_OPENGL_ES_2_0'},
        use_rpath = False
        ),

    'debug_es2': build_configuration(
        cc_flags = '-g',
        shared_libraries = [shared_libraries_es2],
        static_libraries = [external_static_libraries_common],
        cc = '/scratchbox/compilers/cs2007q3-glibc2.5-arm7/bin/arm-none-linux-gnueabi-gcc',
        ar = '/scratchbox/compilers/cs2007q3-glibc2.5-arm7/bin/arm-none-linux-gnueabi-ar',
        export_path = True,
        cpppath = '/scratchbox/users/default/targets/FREMANTLE_ARMEL/usr/include',
        defines = {'KZ_OPENGL_VERSION':'KZ_OPENGL_ES_2_0'},
        use_rpath = False
        ),
    'default': 'debug_es1'
} 


output_directory = "../../../" # path to root directory where output will be placed

platform = 'maemo5' # platform name defines the subdirectory from where to search for the
                         # component build scripts

build = define_build(build_types = available_build_types, platform = platform, build_root = output_directory, global_utilities_path = common_utilities)


#================================================
# Project components
#================================================


component_root = "../../../" # path to component project directory
#TODO: explain
build_component(build, component_root = component_root, component_name = "System")
build_component(build, component_root = component_root, component_name = "Core")
build_component(build, component_root = component_root, component_name = "User")
build_component(build, component_root = component_root, component_name = "unit_tests_es1")
build_component(build, component_root = component_root, component_name = "unit_tests_es2")
build_component(build, component_root = component_root, component_name = "unit_tests_generic")
build_component(build, component_root = component_root, component_name = "ApplicationFramework")
build_component(build, component_root = component_root, component_name = "DefaultApplication")
