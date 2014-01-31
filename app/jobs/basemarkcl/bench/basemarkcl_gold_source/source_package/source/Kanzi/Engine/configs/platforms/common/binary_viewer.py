Import("build", "define_component", "component_root")


component = define_component (
    name = "binary_viewer",
    type = "program",
    source_path = ["applications/binary_viewer/src"],
    include_path = ["applications/binary_viewer/src"],
    library_headers = ["ApplicationFramework", "System", "Core", "User", "GL", "EGL", "libjpeg", "libpng", "zlib", "itype", "LuaWrapper"],
    link_libraries = ["ApplicationFramework", "System", "Core", "User", "GL", "EGL", "libjpeg", "libpng", "zlib", "itype", "LuaWrapper"]
)


component["root"] = component_root
Export("build", "component")
build["env"].SConscript(build["component_build"])