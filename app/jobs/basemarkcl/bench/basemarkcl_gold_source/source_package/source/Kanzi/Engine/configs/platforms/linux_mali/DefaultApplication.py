Import('build')
Import('define_component')
Import('component_root')
Import('component_name')

# === Component specific code starts ===

append = define_component(
    link_libraries = []
)
override = None

# === Component specific code ends ===

options  = {'append' : append, 'override':override}
build['platform_options'][build['platform']][component_name] = options

