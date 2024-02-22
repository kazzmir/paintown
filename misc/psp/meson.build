# Warning: changing the default_options requires a new build directory and doing 'meson setup ...' again
project('sdl2_ttf', ['c', 'cpp'], default_options: ['cpp_std=c++11'], version: '4.0')

build_tests = get_option('build_tests')
release = get_option('release')

add_global_arguments('-DUSE_SDL2=1', language: 'cpp')
add_global_arguments('-DDATA_PATH="data"', language: 'cpp')
add_global_arguments('-g3', language: ['cpp', 'c'])
add_global_arguments(['-Wextra', '-Wno-unused-variable', '-Wno-unused-function', '-Wno-unused-parameter'], language: ['cpp', 'c'])

is_static = true
executable_name = 'paintown'

sdl_dependency = dependency('sdl2', required: true, static: is_static)
sdl_ttf_dependency = dependency('SDL2_ttf', required: true, version: '>=2.0.18', static: is_static)

src_files = ['sdl2-ttf.cpp']
includes = []

# Add PSP-specific compiler and linker flags
eboot_bin = executable(executable_name, src_files,
  include_directories: [includes],
  dependencies: [ sdl_dependency, sdl_ttf_dependency ],
  link_with: [],
  link_args: []
  )
