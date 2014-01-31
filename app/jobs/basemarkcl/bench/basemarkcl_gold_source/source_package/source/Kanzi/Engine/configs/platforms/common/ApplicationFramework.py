Import("build")
Import("define_component")
Import("component_root")

#TODO: add only one Import, and one post-action

# === Component specific code starts ===

component = define_component(
    name = "ApplicationFramework",
    type = "library",
    source_path = ["sources/application_framework/common/src"],
    include_path = ["sources/application_framework/common/src"],
    library_headers = ["System", "Core", "User", "GL", "EGL"]
)

# === Component specific code ends ===

component["root"] = component_root

Export("build")
Export("component")
build["env"].SConscript(build["component_build"])

