parse_test = ['parse.cpp']
token_test = ['test.cpp']

test_extra_files = [
    '../../globals.cpp',
]

includes = include_directories('../..')

executable('test-parse', parse_test + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: [sdl_graphics_library, rtech1_library],
  link_args: test_link_args,
  )

executable('test-token', token_test + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: [sdl_graphics_library, rtech1_library],
  link_args: test_link_args,
  )