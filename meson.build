# Warning: changing the default_options requires a new build directory and doing 'meson setup ...' again
project('paintown', ['c', 'cpp'], default_options: ['cpp_std=c++11'])

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
  'src/r-tech1/sound/sound.cpp',
  'src/r-tech1/font.cpp',
  'src/r-tech1/menu/menu.cpp',
  'src/r-tech1/menu/font-info.cpp',
  'src/r-tech1/menu/menu_option.cpp',
  'src/r-tech1/menu/options.cpp',
  'src/r-tech1/menu/optionfactory.cpp',
  'src/r-tech1/directory.cpp',
  'src/r-tech1/thread.cpp',
  'src/r-tech1/utf.cpp',
  'src/r-tech1/system/init.cpp',

  'src/r-tech1/libs/sfl/sfldir.c',

  'src/r-tech1/libs/7z/7zAlloc.c',
  'src/r-tech1/libs/7z/7zBuf.c',
  'src/r-tech1/libs/7z/7zBuf2.c',
  'src/r-tech1/libs/7z/7zCrc.c',
  'src/r-tech1/libs/7z/7zCrcOpt.c',
  'src/r-tech1/libs/7z/7zDec.c',
  'src/r-tech1/libs/7z/7zIn.c',
  'src/r-tech1/libs/7z/CpuArch.c',
  'src/r-tech1/libs/7z/LzmaDec.c',
  'src/r-tech1/libs/7z/Lzma2Dec.c',
  'src/r-tech1/libs/7z/Bra.c',
  'src/r-tech1/libs/7z/Bra86.c',
  'src/r-tech1/libs/7z/Bcj2.c',
  'src/r-tech1/libs/7z/7zFile.c',
  'src/r-tech1/libs/7z/7zStream.c',

  'src/r-tech1/libs/zip/ioapi.c',
  'src/r-tech1/libs/zip/unzip.c',
  'src/r-tech1/libs/zip/zip.c',

  'src/factory/collector.cpp',
  'src/paintown-engine/game/mod.cpp',
  'src/paintown-engine/game/argument.cpp',
  'src/paintown-engine/game/options.cpp',

  'src/mugen/argument.cpp',
  'src/mugen/option-options.cpp',
  'src/mugen/exception.cpp',
  ]

add_global_arguments('-DUSE_SDL=1', language: 'cpp')
add_global_arguments('-DDATA_PATH="data"', language: 'cpp')

sdl_dependency = dependency('sdl', required: true)
freetype_dependency = dependency('freetype2', required: true)
zlib_dependency = dependency('zlib', required: true)

executable('paintown', all_code, include_directories: includes, dependencies: [sdl_dependency, freetype_dependency, zlib_dependency])
