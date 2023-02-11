# Warning: changing the default_options requires a new build directory and doing 'meson setup ...' again
project('paintown', 'cpp', default_options: ['cpp_std=c++11'])

includes = include_directories('src')
all_code = [
  'src/xmain.cpp',
  'src/main-menu.cpp',

  'src/r-tech1/configuration.cpp',
  'src/r-tech1/input/keyboard.cpp',
  'src/r-tech1/file-system.cpp',
  'src/r-tech1/debug.cpp',
  'src/r-tech1/funcs.cpp',
  'src/r-tech1/tokenreader.cpp',
  'src/r-tech1/token.cpp',
  'src/r-tech1/exceptions/load_exception.cpp',
  'src/r-tech1/exceptions/exception.cpp',
  'src/r-tech1/graphics/bitmap.cpp',
  'src/r-tech1/system.cpp',
  'src/r-tech1/version.cpp',
  'src/r-tech1/argument.cpp',
  'src/r-tech1/init.cpp',
  'src/r-tech1/input/input-manager.cpp',
  'src/r-tech1/sound/music.cpp',
  'src/r-tech1/font.cpp',
  'src/r-tech1/menu/menu.cpp',
  'src/r-tech1/menu/font-info.cpp',

  'src/factory/collector.cpp',
  'src/paintown-engine/game/mod.cpp',
  'src/paintown-engine/game/argument.cpp',

  'src/mugen/argument.cpp',
  ]

add_global_arguments('-DUSE_SDL=1', language: 'cpp')
add_global_arguments('-DDATA_PATH="data"', language: 'cpp')

sdl_dependency = dependency('sdl', required: true)
freetype_dependency = dependency('freetype2', required: true)

executable('paintown', all_code, include_directories: includes, dependencies: [sdl_dependency, freetype_dependency])
