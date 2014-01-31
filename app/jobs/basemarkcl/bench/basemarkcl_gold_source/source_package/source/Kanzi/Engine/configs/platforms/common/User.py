Import("build")
Import("define_component")
Import("component_root")


# === Component specific code starts ===

component = define_component(
    name = "User",
    type = "library",
    source_path = ["sources/user_layer/src"],
    include_path = ["sources/user_layer/src"],
    library_headers = ["Core", "System", "GL", "lua"]
)

# === Component specific code ends ===

component["root"] = component_root

Export("build")
Export("component")
build["env"].SConscript(build["component_build"])

