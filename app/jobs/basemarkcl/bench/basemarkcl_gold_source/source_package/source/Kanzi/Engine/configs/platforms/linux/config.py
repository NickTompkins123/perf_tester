debuglog("Reading the Linux build definition...")
platform_attributes += ["linux", "linux-x86", "egl"]

# Compiler and linker flags

module("default").env["CCFLAGS"] += [ \
    "-Wno-long-long", "-m32"
]
