# Warning: changing the default_options requires a new build directory and doing 'meson setup ...' again
project('paintown', ['c', 'cpp'], default_options: ['cpp_std=c++11'], version: '4.0')

build_tests = get_option('build_tests')

add_global_arguments('-DUSE_SDL2=1', language: 'cpp')
add_global_arguments('-DDATA_PATH="data"', language: 'cpp')
add_global_arguments('-g3', language: ['cpp', 'c'])
add_global_arguments(['-Wno-unused-variable', '-Wno-unused-function', '-Wno-unused-parameter', '-Wextra'], language: ['cpp', 'c'])

link_args = []
is_static = true
sdl_graphics_dependency = []
sdl_graphics_library = []

# Not supported in mingw
if not meson.is_cross_build()
  add_global_arguments(['-fsanitize=address'], language: ['cpp', 'c'])
  link_args = ['-fsanitize=address']
  is_static = false
  sdl_graphics_dependency = dependency('SDL2_gfx', required: true, static: is_static)
endif

includes = include_directories('src')
dumb_includes = include_directories('src/r-tech1/libs/dumb/include')
all_code = [
  'src/xmain.cpp',
  'src/main-menu.cpp',
  'src/factory/collector.cpp',
  'src/factory/font_render.cpp',
  'src/globals.cpp',
]

sdl_dependency = dependency('sdl2', required: true, static: is_static)
sdl_image_dependency = dependency('SDL2_image', required: true, static: is_static)
sdl_ttf_dependency = dependency('SDL2_ttf', required: true, version: '>=2.0.18', static: is_static)
sdl_mixer_dependency = dependency('SDL2_mixer', required: true, static: is_static)
#sdl_graphics_dependency = dependency('SDL2_gfx', required: true, static: is_static)
# freetype_dependency = dependency('freetype2', required: true)
# zlib static = true doesn't work here, have to put it in the built-in options
zlib_dependency = dependency('zlib', required: true)
vorbis_dependency = dependency('vorbis', required: false, static: is_static)
vorbis_file_dependency = dependency('vorbisfile', required: false, static: is_static)
ogg_dependency = dependency('ogg', required: false, static: is_static)
mpg123_dependency = dependency('libmpg123', required: false, static: is_static)

if vorbis_dependency.found() and vorbis_file_dependency.found() and ogg_dependency.found()
  add_global_arguments('-DHAVE_OGG=1', language: 'cpp')
endif

if mpg123_dependency.found()
  add_global_arguments('-DHAVE_MP3_MPG123=1', language: 'cpp')
endif

if meson.is_cross_build()
  subdir('.tmp/SDL_gfx')
endif

subdir('src/r-tech1')
subdir('src/mugen')
subdir('src/paintown-engine')
subdir('src/openbor')

test_dependencies = [sdl_dependency, sdl_image_dependency, sdl_ttf_dependency, sdl_mixer_dependency, sdl_graphics_dependency, zlib_dependency, vorbis_dependency, vorbis_file_dependency, ogg_dependency, mpg123_dependency]
test_linkages = [rtech1_library, mugen_library, paintown_library, openbor_library]
test_link_args = link_args

executable('paintown', all_code,
  include_directories: [includes, dumb_includes],
  dependencies: [sdl_dependency, sdl_image_dependency, sdl_ttf_dependency, sdl_mixer_dependency, sdl_graphics_dependency, zlib_dependency, vorbis_dependency, vorbis_file_dependency, ogg_dependency, mpg123_dependency],
  link_with: [sdl_graphics_library, rtech1_library, mugen_library, paintown_library, openbor_library],
  link_args: link_args,
  )

if build_tests == true
  subdir('src/test/paintown')
  subdir('src/test/graphics')
  subdir('src/test/audio')
  subdir('src/test/mugen')
  subdir('src/test/timing')
endif
