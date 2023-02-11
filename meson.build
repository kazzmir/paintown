# Warning: changing the default_options requires a new build directory and doing 'meson setup ...' again
project('paintown', 'cpp', default_options: ['cpp_std=c++11'])

includes = include_directories('src')
all_code = ['src/xmain.cpp', 'src/main-menu.cpp']

add_global_arguments('-DUSE_SDL=1', language: 'cpp')
add_global_arguments('-DDATA_PATH="data"', language: 'cpp')

sdl_dependency = dependency('sdl', required: true)
freetype_dependency = dependency('freetype2', required: true)

executable('paintown', all_code, include_directories: includes, dependencies: [sdl_dependency, freetype_dependency])
