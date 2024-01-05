test_view = ['view.cpp']

test_extra_files = [
  '../../factory/font_render.cpp',
  '../../globals.cpp',
]

includes = include_directories('../..')

executable('test-mugen-view', test_view + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: [rtech1_library, mugen_library],
  link_args: test_link_args,
  )
