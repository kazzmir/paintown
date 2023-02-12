# Warning: changing the default_options requires a new build directory and doing 'meson setup ...' again
project('paintown', ['c', 'cpp'], default_options: ['cpp_std=c++11'])

add_global_arguments('-DUSE_SDL=1', language: 'cpp')
add_global_arguments('-DDATA_PATH="data"', language: 'cpp')
add_global_arguments('-g3', language: ['cpp', 'c'])

includes = include_directories('src')
dumb_includes = include_directories('src/r-tech1/libs/dumb/include')
all_code = [
  'src/xmain.cpp',
  'src/main-menu.cpp',
  'src/factory/collector.cpp',
  'src/factory/font_render.cpp',
  'src/globals.cpp',
]

sdl_dependency = dependency('sdl', required: true)
freetype_dependency = dependency('freetype2', required: true)
zlib_dependency = dependency('zlib', required: true)

subdir('src/r-tech1')
subdir('src/mugen')
subdir('src/paintown-engine')
subdir('src/openbor')

executable('paintown', all_code,
  include_directories: [includes, dumb_includes],
  dependencies: [sdl_dependency, freetype_dependency, zlib_dependency],
  link_with: [rtech1_library, mugen_library, paintown_library, openbor_library],
  )
