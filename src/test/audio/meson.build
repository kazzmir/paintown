test_music = ['music.cpp']
test_player = ['test.cpp'] #all formats
test_wav = ['wav.cpp']

test_extra_files = [
  '../../r-tech1/sound/music-player.cpp',
  '../../r-tech1/sound/music-renderer.cpp',
  '../../r-tech1/debug.cpp',
  '../../r-tech1/utf.cpp',
  '../../r-tech1/sound/audio.cpp',
  '../../r-tech1/exceptions/exception.cpp',
  '../../r-tech1/token.cpp', 
  '../../r-tech1/file-system.cpp',
  '../../r-tech1/directory.cpp', 
  '../../r-tech1/thread.cpp',
  '../../r-tech1/system.cpp',
  '../../globals.cpp',
]

includes = include_directories('../..')

executable('test-music', test_music + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: [rtech1_library],
  link_args: test_link_args,
  )

executable('test-player', test_player + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: [rtech1_library],
  link_args: test_link_args,
  )

# SDL
#executable('test-wav', test_wav + test_extra_files,
#  include_directories: [includes],
#  dependencies: test_dependencies,
#  link_with: [rtech1_library],
#  link_args: test_link_args,
#  )