project('gameboyemu', 'c',
                      license : 'GPL3.0',
                      default_options : ['c_std=gnu11', 'optimization=2', 'warning_level=3'],
                      meson_version : '>=0.49.0')

deps = [dependency('sdl2')]

progname = 'gbemu'

subdir('src')
executable(progname, sources, dependencies : deps, install : false)