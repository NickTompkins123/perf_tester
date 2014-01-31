Import("build")
Import("define_component")
Import("component_root")
Import("component_name")

# === Component specific code starts ===
# Platform appends for android. Required parameters: name

append = define_component(
    source_path = ["sources/system_layer/environments/android/src"],
    link_libraries = [ ]
)

override = None

# === Component specific code ends ===

options  = {"append" : append, "override":override}
build["platform_options"][build["platform"]][component_name] = options

