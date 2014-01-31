
debuglog("Reading the Maemo build definition...")


platform_attributes += ["maemo5", "linux-arm", "linux", "egl", "nokia_n900", "maemo5_egl"]
module("default").type = "exe"


# Compiler and linker flags

module("default").env["CPPDEFINES"] += ["MAEMO", "PNG_NO_MMX_CODE"]

module("default").env["CCFLAGS"] += [ \
    "-Wno-long-long"
    #"-march=armv5te -mtune=xscale -msoft-float -mthumb " \
    #"-fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 "
]

# Toolchain

    # scratchbox
#module("default").env["CPPPATH"] += [ "/targets/FREMANTLE_ARMEL/usr/include" ]
#module("default").env["LIBPATH"] += [ "/targets/FREMANTLE_ARMEL/usr/lib" ]
#module("default").env["LIBPATH"] += [ "/targets/FREMANTLE_ARMEL/usr/lib/gcc/arm-linux-gnueabi/4.2" ]
#module("default").env["LIBS"] += ["EGL", "GLESv2", "IMGegl", "srv_um", "Xdmcp", "Xau", "X11"]

    # serenity
# some of platform libs fail on Serenity because of the symlinks
#   e.g. libc.so -> /usr/lib/libc.so.6 -> BOOM  (should be Scratchbox's /usr/lib...)
#   fix: copied the armel platform's /usr/lib under Engine/libraries/platform_usr/lib
#           and changed the symlinks so that they don't point to absolute paths
#   fix2: changed symlinks and .so references
#module("default").used_libraries += ["platform_usr"]
module("default").env["CC"] = "/scratchbox/compilers/cs2007q3-glibc2.5-arm7/bin/arm-none-linux-gnueabi-gcc"
module("default").env["AR"] = "/scratchbox/compilers/cs2007q3-glibc2.5-arm7/bin/arm-none-linux-gnueabi-ar"
module("default").env["CXX"] = "/scratchbox/compilers/cs2007q3-glibc2.5-arm7/bin/arm-none-linux-gnueabi-g++"
module("default").env["CPPPATH"] += [ "/scratchbox/users/default/targets/FREMANTLE_ARMEL/usr/include" ]

module("default").env["LIBPATH"] += [ "/scratchbox/users/default/targets/FREMANTLE_ARMEL/usr/lib" ]
#module("default").env["LIBPATH"] += [ "/scratchbox/users/default/targets/FREMANTLE_ARMEL/usr/lib/gcc/arm-linux-gnueabi/4.2" ]
module("default").env["LIBS"] += ["EGL", "IMGegl", "srv_um", "Xdmcp", "Xau", "X11"]

library("opengl_es_1_1").binaries += ["GLES_CM"]
library("opengl_es_2_0").binaries += ["GLESv2"]