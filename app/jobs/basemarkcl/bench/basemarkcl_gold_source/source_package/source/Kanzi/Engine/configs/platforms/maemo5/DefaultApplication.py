Import('build')
Import('define_component')
Import('component_root')
Import('component_name')

# === Component specific code starts ===

append = define_component(
    link_libraries = ['IMGegl', 'srv_um', 'itype', 'Xau', 'X11', 'Xdmcp']
)

# === Component specific code ends ===

options  = {'append' : append}
build['platform_options'][build['platform']][component_name] = options

