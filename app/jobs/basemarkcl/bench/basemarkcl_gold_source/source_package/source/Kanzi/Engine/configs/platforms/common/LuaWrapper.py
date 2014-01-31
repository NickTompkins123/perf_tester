Import("build")
Import("define_component")
Import("component_root")


# === Component specific code starts ===

component = define_component(
    name = "LuaWrapper",
    type = "library",
    source_path = ["sources/lua_wrapper/src"],
    include_path = ["sources/lua_wrapper/src"],
    library_headers = ["User", "Core", "System", "lua", "tolua"]
)

# === Component specific code ends ===

component["root"] = component_root

Export("build")
Export("component")
build["env"].SConscript(build["component_build"])

