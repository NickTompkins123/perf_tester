Import('build')
Import('define_component')
Import('component_root')
Import('component_name')

# === Component specific code starts ===
# Platform appends for linux. Required parameters: name

append = define_component(
    source_path = ['sources/system_layer/platforms/maemo5_egl/src',
                   'sources/system_layer/platforms/nokia_n900/src',
                   'sources/system_layer/environments/linux/src',
                   'sources/system_layer/environments/egl/src']
)

# === Component specific code ends ===

build['platform_options'][build['platform']][component_name] = {'append' : append}

