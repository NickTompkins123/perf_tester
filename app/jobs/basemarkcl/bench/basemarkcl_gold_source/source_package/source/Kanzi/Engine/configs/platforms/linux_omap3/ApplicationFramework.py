
Import('build')
Import('define_component')
Import('component_root')
Import('component_name')

#TODO: add only one Import, and one post-action

# === Component specific code starts ===

append = define_component(
    source_path = ['sources/application_framework/platforms/linux_generic/src'],
    include_path = ['sources/application_framework/platforms/linux_generic/src'],
    library_headers = ['ts', 'User']
)
override = None

# === Component specific code ends ===

options  = {'append' : append, 'override':override}
build['platform_options'][build['platform']][component_name] = options

