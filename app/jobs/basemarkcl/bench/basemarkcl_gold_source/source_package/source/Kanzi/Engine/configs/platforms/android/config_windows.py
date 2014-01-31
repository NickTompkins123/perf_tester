
debuglog("Reading the Android build definition...")

# try to find the ndk_path from environment variables
#   look for NDK_PATH and after that search PATH
from os import environ
ndk_path = None
if "NDK_PATH" in environ:
    ndk_path = environ["NDK_PATH"]    
if not ndk_path and "PATH" in environ:
    for p in environ["PATH"].split(";"):
        if "android-ndk" in p:
            ndk_path = p
            break
if not ndk_path:            
    ndk_path = "C:\\Program Files (x86)\\android-ndk-r5b"
    warning("NDK_PATH is not defined, assuming " + ndk_path + ". Please SET NDK_PATH=<your NDK path>")

platform_attributes += ["android", "linux-arm"]
platform_binary_name = "kanzi"
platform_default_type = "so"

m = module("default")
m.type = "so"
m.env["SHLIBPREFIX"] = "lib"
m.env["SHLIBSUFFIX"] = ".so"
m.env["LIBPREFIX"] = "lib"
m.env["LIBSUFFIX"] = ".a"

# mechanisms that are needed to make APK building happen without project-specific scripts

module("targets").install_dir = "libs/armeabi"
module("targets").profile("ES1").preactions += [
    "android update project -p . -t android-4"#,
    #"cp GLES_profiles/ES1/AndroidManifest.xml .",
    #"cp GLES_profiles/ES1/KanziView.java src/com/rightware/kanzi"
    #"ant use_es1"
]
module("targets").profile("ES2").preactions += [
    "android update project -p . -t android-7"#,
    #"cp GLES_profiles/ES2/AndroidManifest.xml .",
    #"cp GLES_profiles/ES2/KanziView.java src/com/rightware/kanzi"
    #"ant use_es2"
]
module("targets").profile("debug").postactions += ["ant debug"]
#module("targets").profile("profile").postactions += ["ant debug"]
module("targets").profile("release").postactions += ["ant release"]

# TODO: temporary hack, won't run ant (or Install?) if this file already exists
#       also ensure install dir exists
module("targets").preactions += [
    "if not exist libs\\armeabi mkdir libs\\armeabi", 
    "del /q libs\\armeabi\\*"
]    


# Compiler and linker flags

m.env["CPPDEFINES"] += ["ANDROID"]
#profile("release").env["CFLAGS"].remove("-O2") # TODO: should there be -O? in release? Not with -g anyway.

#pdb.set_trace()
# Toolchain

m.env["CC"] = m.env["SHCC"] = '"' + ndk_path + '\\toolchains\\arm-eabi-4.4.0\\prebuilt\\windows\\bin\\arm-eabi-gcc.exe"'
m.env["CXX"] = m.env["SHCXX"] = '"' + ndk_path + '\\toolchains\\arm-eabi-4.4.0\\prebuilt\\windows\\bin\\arm-eabi-g++.exe"'
m.env["AR"] = '"' + ndk_path + '\\toolchains\\arm-eabi-4.4.0\\prebuilt\\windows\\bin\\arm-eabi-ar.exe"'
m.env["RANLIB"] = '"' + ndk_path + '\\toolchains\\arm-eabi-4.4.0\\prebuilt\\windows\\bin\\arm-eabi-ranlib.exe"'

# "-g  Produce debugging information in the operating system's native format"
#   ndk-build does it, however, so it maybe should included? TODO: test
m.env.Append(CCFLAGS = ["-fpic", "-mthumb-interwork", "-fstack-protector", "-fno-short-enums", \
    "-Wno-psabi", "-Wno-long-long", "-Wno-comment", \
    "-D__ARM_ARCH_5__", "-D__ARM_ARCH_5T__", "-D__ARM_ARCH_5E__", "-D__ARM_ARCH_5TE__"
    #"-march=armv5te", "-mtune=xscale", "-msoft-float", "-mthumb" \
    #"-fomit-frame-pointer", "-fno-strict-aliasing", "-finline-limit=64"
    #"-g"
])
m.env.Append(CXXFLAGS = ["-fno-exceptions", "-fno-rtti"])

profile("release").env.Append(CCFLAGS = ["-ffunction-sections", "-funwind-tables"])

m.env.Append(SHLINKFLAGS = ["-nostdlib", "-Wl,--no-undefined", "-Wl,-z,noexecstack", "-Wl,-Bsymbolic", "-fno-short-enums"])

# OpenGL ES libraries
# Also select the target android device depending on the ES version choice
library("opengl_es_1_1").binaries += ["GLESv1_CM"]
library("opengl_es_1_1").libpaths += [ndk_path + "\\platforms\\android-4\\arch-arm\\usr\\lib"]
library("opengl_es_1_1").include_paths += [ndk_path + "\\platforms\\android-4\\arch-arm\\usr\\include"]
library("opengl_es_2_0").binaries += ["GLESv2"]
library("opengl_es_2_0").libpaths += [ndk_path + "\\platforms\\android-8\\arch-arm\\usr\\lib"]
library("opengl_es_2_0").include_paths += [ndk_path + "\\platforms\\android-8\\arch-arm\\usr\\include"]

m.env["LIBS"] += ["log", "c", "m", "dl"]
m.env["SHLINKFLAGS"] += [ndk_path + "\\toolchains\\arm-eabi-4.4.0\\prebuilt\\windows\\lib\\gcc\\arm-eabi\\4.4.0\\libgcc.a"]
#m.env["LIBPATH"] += ["/usr/sdk/android/android-ndk-r4b/build/prebuilt/linux-x86/arm-eabi-4.4.0/lib/gcc/arm-eabi/4.4.0"]
#m.env["LIBS"] += ["gcc"]


del m
