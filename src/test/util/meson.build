filesystem_test = ['file-system.cpp']

test_extra_files = [
    '../../globals.cpp',
]

includes = include_directories('../..')

executable('test-fs', filesystem_test + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: [rtech1_library],
  link_args: test_link_args,
  )