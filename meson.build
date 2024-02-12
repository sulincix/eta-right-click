project('eta-right-click', 'c')

# Declare dependency on libevdev
libevdev_dep = dependency('libevdev')

files = [
    'src/main.c',
    'src/event.c',
    'src/evdev.c',
    'src/input.c',
    'src/epoch_time.c',
]

if get_option('debug')
    add_project_arguments('-DDEBUG', language: 'c')
endif

executable('main', files,
    dependencies: [libevdev_dep],
    include_directories: include_directories('include'),
    install: 'true'
)
