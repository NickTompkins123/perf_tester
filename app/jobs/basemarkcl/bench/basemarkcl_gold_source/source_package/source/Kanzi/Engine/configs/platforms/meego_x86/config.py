
debuglog("Reading the Meego(x86) build definition...")


platform_attributes += ["meego-x86", "maemo5", "linux", "egl", "nokia_n900", "maemo5_egl"]
module("default").type = "exe"


    
# paths on the build system 

sdk_path = "/usr/sdk/meego/wetab-1.0"
sdk_gles_path = "/usr/sdk/meego/x86/gles-libs"

# Compiler and linker flags

module("default").env["CPPDEFINES"] += ["MAEMO", "PNG_NO_MMX_CODE", "__linux__", "SUPPORT_X11"]
module("default").env["CCFLAGS"] += ["-Wno-long-long"]

# Toolchain

module("default").env["CC"] = "LD_LIBRARY_PATH=" + sdk_path + "/usr/lib " + sdk_path + "/usr/bin/gcc"
module("default").env["AR"] = "LD_LIBRARY_PATH=" + sdk_path + "/usr/lib " + sdk_path + "/usr/bin/ar"
module("default").env["CXX"] = "LD_LIBRARY_PATH=" + sdk_path + "/usr/lib " + sdk_path + "/usr/bin/g++"

module("default").env["CPPPATH"] += [ sdk_path + "/usr/include" ]

module("default").env["LIBPATH"] += [ sdk_path + "/lib" ]
module("default").env["LIBPATH"] += [ sdk_path + "/usr/lib/gcc/i586-meego-linux/4.4.2" ]
module("default").env["LIBPATH"] += [ sdk_path + "/usr/lib" ]
module("default").env["LIBPATH"] += [ sdk_gles_path + "/lib" ]

module("default").env["LIBS"] += ["EGL", "Xau", "X11", "xcb", "Xext", "Xfixes"]

library("opengl_es_1_1").binaries += ["GLES_CM"]
library("opengl_es_2_0").binaries += ["GLESv2"]
