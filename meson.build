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
    install_dir:  '/usr/bin',
    install: 'true'
)


if get_option('service') == 'systemd'
    install_data('data/eta-right-click.systemd',
        install_dir: '/lib/systemd/system/',
        rename: 'eta-right-click.service')
elif get_option('service') == 'openrc'
    install_data('data/eta-right-click.openrc',
        install_dir: '/etc/init.d/',
        rename: 'eta-right-click')
endif