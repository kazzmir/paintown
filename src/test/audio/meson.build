test_music = ['music.cpp']
test_player = ['test.cpp'] #all formats
test_wav = ['wav.cpp']

test_extra_files = [
    '../../globals.cpp',
]

includes = include_directories('../..')

executable('test-music', test_music + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: [sdl_graphics_library, rtech1_library],
  link_args: test_link_args,
  )

executable('test-player', test_player + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: [sdl_graphics_library, rtech1_library],
  link_args: test_link_args,
  )

# SDL
executable('test-wav', test_wav + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: [sdl_graphics_library, rtech1_library],
  link_args: test_link_args,
  )
