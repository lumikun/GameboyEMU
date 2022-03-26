project('gameboyemu', 'c',
                      license : 'GPL3.0',
                      default_options : ['c_std=C11', 'optimization=2', 'warning_level=1', 'default_library=static'])

deps = [dependency('sdl2'), dependency('sdl2_ttf')]

progname = 'gbemu'

subdir('src')
executable(progname, sources, win_subsystem: 'windows',
             dependencies : deps, install : false)
