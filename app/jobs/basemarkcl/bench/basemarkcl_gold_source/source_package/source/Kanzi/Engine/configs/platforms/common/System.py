Import("build")
Import("define_component")
Import("component_root")

# === Component specific code starts ===

component = define_component(
    name = "System",
    type = "library",
    source_path = ["sources/system_layer/common/src"],
    platform_libraries = ["m"],
    library_headers = ["GL", "EGL"]
)

# === Component specific code ends ===

component["root"] = component_root

Export("build")
Export("component")
build["env"].SConscript(build["component_build"])

