project('gameboyemu', 'c',
                      license : 'GPL3.0',
                      default_options : ['c_std=c11', 'optimization=2', 'warning_level=1', 'default_library=static'])

sdl2wrap = subproject('sdl2')
sdl2ttfwrap = subproject('sdl2_ttf')
#deps = [dependency('sdl2', fallback : ['SDL2', 'sdl2wrap']), dependency('sdl2_ttf', fallback : ['sdl2-ttf', 'sdl2ttfwrap'])]
#deps = [dependency('sdl2wrap'), dependency('sdl2ttfwrap')]
deps = [dependency('sdl2'), dependency('sdl2_ttf')]

progname = 'gbemu'

subdir('src')
executable(progname, sources, win_subsystem: 'windows',
             dependencies : deps, install : false)
