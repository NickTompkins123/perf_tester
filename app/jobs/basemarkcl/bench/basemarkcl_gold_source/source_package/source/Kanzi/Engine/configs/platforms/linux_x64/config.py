
debuglog("Reading the Meego(x86) build definition...")


#platform_attributes += ["meego-x86", "maemo5", "linux", "egl", "nokia_n900", "maemo5_egl"]
platform_attributes += ["linux-x64", "maemo5", "linux", "glx", "linux_x86", "meego_glx"]
module("default").type = "exe"


    
# paths on the build system 

#sdk_path = "/usr/sdk/meego/wetab-1.0"
sdk_path = ""
#sdk_path = "/usr/sdk/meego/meego-ivi-1.1"
#sdk_gles_path = "/usr/sdk/meego/x86/gles-libs"
#sdk_gles_path = "/usr/sdk/meego/x86/gles-libs-nvidia"
sdk_gles_path = ""

# Compiler and linker flags

module("default").env["CPPDEFINES"] += ["MAEMO", "GL_GLEXT_PROTOTYPES"]
module("default").env["CCFLAGS"] += ["-Wno-long-long", "-std=gnu99", "-Wall", "-Wextra"]

# Toolchain

module("default").env["CC"] = "LD_LIBRARY_PATH=" + sdk_path + "/usr/lib " + sdk_path + "/usr/bin/gcc"
module("default").env["AR"] = "LD_LIBRARY_PATH=" + sdk_path + "/usr/lib " + sdk_path + "/usr/bin/ar"
module("default").env["CXX"] = "LD_LIBRARY_PATH=" + sdk_path + "/usr/lib " + sdk_path + "/usr/bin/g++"

module("default").env["CPPPATH"] += [ sdk_path + "/usr/include" ]

module("default").env["LIBPATH"] += [ sdk_path + "/lib" ]
#module("default").env["LIBPATH"] += [ sdk_path + "/usr/lib/gcc/i586-meego-linux/4.4.2" ]
#module("default").env["LIBPATH"] += [ sdk_path + "/usr/lib/gcc/i586-meego-linux/4.5.0" ]
module("default").env["LIBPATH"] += [ sdk_path + "/usr/lib" ]
module("default").env["LIBPATH"] += [ sdk_gles_path + "/lib" ]


#module("default").env["LIBS"] += ["z", "png", "m", "pthread", "Xau", "X11", "xcb", "Xext", "Xfixes", "jpeg", "freetype"]
module("default").env["LIBS"] += ["z", "png", "m", "jpeg", "freetype"]

library("opengl_es_1_1").binaries += ["GLES_CM"]
library("opengl_es_2_0").binaries += ["GLESv2"]
library("opengl_2_1").binaries += ["GL"]

