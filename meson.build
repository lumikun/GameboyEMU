project('gameboyemu', 'c',
                      license : 'GPL3.0',
                      default_options : ['c_std=gnu11', 'buildtype=debug', 'warning_level=3'],
                      meson_version : '>=0.49.0')

deps = [dependency('SDL2')]

progname = 'gbemu'

inc = include_directories('include')

subdir('include')
subdir('src')

debugbuild = '0'

if get_option('buildtype') == 'debug'
   debugbuild = '1'
elif get_option('buildtype') == 'debugoptimized'
     debugbuild = '1'
endif
