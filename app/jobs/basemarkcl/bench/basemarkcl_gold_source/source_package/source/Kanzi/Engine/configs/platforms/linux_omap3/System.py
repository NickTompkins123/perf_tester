Import('build')
Import('define_component')
Import('component_root')
Import('component_name')

# === Component specific code starts ===
# Platform appends for linux. Required parameters: name

append = define_component(
    source_path = ['sources/system_layer/platforms/linux_egl/src',
                   'sources/system_layer/platforms/zoom2/src',
                   'sources/system_layer/environments/linux/src',
                   'sources/system_layer/environments/egl/src']
)

override = None

# === Component specific code ends ===

options  = {'append' : append, 'override':override}
build['platform_options'][build['platform']][component_name] = options

