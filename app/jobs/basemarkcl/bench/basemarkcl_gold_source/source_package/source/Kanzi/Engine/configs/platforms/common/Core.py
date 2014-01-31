Import("build", "define_component", "component_root")


# === Component specific code starts ===

component = define_component(
    name = "Core",
    type = "library",
    source_path = ["sources/core_layer/src"],
    include_path = ["sources/core_layer/src"],
    library_headers = ["System", "GL", "libjpeg", "libpng", "zlib", "freetype", "itype"]
)

# === Component specific code ends ===


component["root"] = component_root

Export("build", "component")
build["env"].SConscript(build["component_build"])

