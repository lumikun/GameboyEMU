project('gameboyemu', 'c',
                      license : 'GPL3.0',
                      default_options : ['c_std=c11', 'optimization=2', 'warning_level=1', 'default_library=static'])

deps = [dependency('sdl2'), dependency('sdl2_ttf')]
# Clunky to get deps, but as long as you have SDL2 and SDL2_ttf libs it should compile and work on Mac or *nix

progname = 'gbemu'

subdir('src')
executable(progname, sources, win_subsystem: 'windows',
             dependencies : deps, install : false)
