
debuglog("Reading the Android build definition...")
default = module("default") # TODO: move to build.py
target = module("targets")  # TODO: move to build.py

platform_attributes += ["android", "linux-arm"]
platform_binary_name = "kanzi"
default.type = "so"
platform_default_type = "so"        # TODO: redesign / remove / fix before release

if "ES1" in used_profiles:
    target_device = "android-4"
    es_version = "ES1"
else:
    target_device = "android-8"
    es_version = "ES2"

    
# paths on the build system 

# try to find the ndk_path from environment variables
#   look for NDK_PATH and after that search PATH
from os import environ
ndk_path = None
if "NDK_PATH" in environ:
    ndk_path = environ["NDK_PATH"]    
if not ndk_path and "PATH" in environ:
    for p in environ["PATH"].split(":"):
        if "android-ndk" in p:
            ndk_path = p
            break
if not ndk_path:            
    ndk_path = "/usr/sdk/android/android-ndk-r5b"
    warning("NDK_PATH is not defined, assuming " + ndk_path + ". Please export NDK_PATH=<your NDK path>")
    
sdk_tool_path = ndk_path + "/toolchains/arm-eabi-4.4.0/prebuilt/linux-x86"
#sdk_tool_path = ndk_path_r4b + "/build/prebuilt/linux-x86/arm-eabi-4.4.0"

sdk_platform_path = ndk_path + "/platforms/" + target_device + "/arch-arm"
#sdk_platform_path = ndk_path_r4b + "/build/platforms/" + target_device + "/arch-arm"


# Toolchain

default.env["CC"] = default.env["SHCC"] = sdk_tool_path + "/bin/arm-eabi-gcc"
default.env["CXX"] = default.env["SHCXX"] = sdk_tool_path + "/bin/arm-eabi-g++"
default.env["AR"] = sdk_tool_path + "/bin/arm-eabi-ar"


# Compiler and linker flags, libraries etc.

default.env["CPPDEFINES"] += ["ANDROID"]
#profile("release").env["CFLAGS"].remove("-O2") # TODO: should there be -O? in release? Not with -g anyway.

default.env["CPPPATH"] += [sdk_platform_path + "/usr/include"]
default.env["LIBPATH"] += [sdk_platform_path + "/usr/lib"]
default.env["LIBS"] += ["log", "c", "m", "dl"]
library("opengl_es_1_1").binaries += ["GLESv1_CM"]
library("opengl_es_2_0").binaries += ["GLESv2"]

# "-g  Produce debugging information in the operating system's native format"
#   ndk-build does it, however, so it maybe should included? TODO: test
default.env["CCFLAGS"] += [
    "-fpic", "-mthumb-interwork", "-fstack-protector", "-fno-short-enums", \
    "-Wno-psabi", "-Wno-long-long", "-Wno-comment", \
    "-D__ARM_ARCH_5__", "-D__ARM_ARCH_5T__", "-D__ARM_ARCH_5E__", "-D__ARM_ARCH_5TE__"
    #"-march=armv5te", "-mtune=xscale", "-msoft-float", "-mthumb" \
    #"-fomit-frame-pointer", "-fno-strict-aliasing", "-finline-limit=64"
    #"-g"
]
default.env["CXXFLAGS"] += ["-fno-exceptions", "-fno-rtti"]
profile("release").env["CCFLAGS"] += ["-ffunction-sections", "-funwind-tables"]

default.env["SHLINKFLAGS"] += [
    "-nostdlib",
    "-Wl,--no-undefined", "-Wl,-z,noexecstack", "-Wl,-Bsymbolic", "-fno-short-enums", 
    sdk_tool_path + "/lib/gcc/arm-eabi/4.4.0/libgcc.a"    
    # "-Wl,-rpath-link=" + sdk_platform_path + "/usr/lib",
    # sdk_platform_path + "/usr/lib/libc.so",
    # sdk_platform_path + "/usr/lib/libstdc++.so",
    # sdk_platform_path + "/usr/lib/libm.so",
    # sdk_platform_path + "/lib/gcc/arm-eabi/4.4.0/libgcc.a",
]

# Android NDK headers make -ansi flip out, so remove them from source builds
try:
    if "core" in available_modules and "user" in available_modules:
        module("core").env["CCFLAGS"].remove("-ansi")
        module("user").env["CCFLAGS"].remove("-ansi")
except ValueError:
    pass

    
# mechanisms that are needed to make APK building happen without project-specific scripts

target.install_dir = "libs/armeabi"
target.preactions += [
    #"android update project -p . -t " + target_device
    "/usr/sdk/android/android-sdk-linux_86/tools/android update project -p . -t " + target_device
    #"cp GLES_profiles/" + es_version + "/AndroidManifest.xml .",
    #"cp GLES_profiles/" + es_version + "/KanziView.java src/com/rightware/kanzi"
    #"ant use_es1"
]
target.profile("debug").postactions += ["ant debug"]
target.profile("profile").postactions += ["ant debug"]
target.profile("release").postactions += ["ant release"]

# TODO: temporary hack, won't run ant (or Install?) if this file already exists
#       also ensure install dir exists
# Bad stuff: may slow down Hudson, for example; forces relinkage of .so (not really that bad)
from os import system
debuglog("Cleaning the .so file")
system("mkdir -p libs/armeabi")
system("rm -f libs/armeabi/*.so")


del default
del target
