project('verlet-c', 'cpp',
  version: '0.1',
  meson_version: '>=1.1')

sdl3_dep = dependency('sdl3', fallback: ['sdl3', 'sdl2'])
tracy_dep = dependency('tracy', static: true )

if get_option('enable_tracy') and get_option('buildtype') != 'debugoptimized'
  warning('Profiling builds should set --buildtype=debugoptimized')
endif

executable('verlet-c', 'main.cpp',
  dependencies: [sdl3_dep, tracy_dep],
)
