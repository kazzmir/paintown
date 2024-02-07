test_timing = ['timing.cpp']

test_extra_files = [
    '../../globals.cpp',
]

includes = include_directories('../..')

executable('test-timing', test_timing + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: [sdl_graphics_library, rtech1_library],
  link_args: test_link_args,
  )
