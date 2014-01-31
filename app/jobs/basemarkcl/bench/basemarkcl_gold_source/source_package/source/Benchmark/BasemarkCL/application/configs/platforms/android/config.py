binary_name = 'kanzi'
#profile("release").env["CCFLAGS"] = "-O3"
#profile("debug").env["CPPDEFINES"] = ["DEBUG"]
#defines = ["KZ_ITYPE_ENABLED"]

module("default").env["CPPPATH"] += [ "../../.." + "/libraries/common/ffmpeg/include" ]


# TODO: use this when we have OpenCL libs
#module("default").env["LIBS"] += ["OpenCL", "avcodec", "avformat", "swscale", "avutil"]

module("default").env["LIBPATH"] += ["../../.." + "/libraries/platforms/android/ffmpeg/lib"]
module("default").env["LIBS"] += ["avcodec", "avformat", "swscale", "avutil"]
