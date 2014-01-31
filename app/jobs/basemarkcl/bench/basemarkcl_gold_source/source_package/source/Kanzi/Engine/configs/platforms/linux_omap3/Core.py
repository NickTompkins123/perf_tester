Import('build')
Import('define_component')
Import('component_root')
Import('component_name')

# === Component specific code starts ===
# Platform appends for linux. Required parameters: name

append = define_component(
        defines = [ 'PNG_NO_MMX_CODE' ]
)

override = None

# === Component specific code ends ===

options  = {'append' : append, 'override':override}
build['platform_options'][build['platform']][component_name] = options

