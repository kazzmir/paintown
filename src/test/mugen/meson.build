test_view = ['view.cpp']

test_extra_files = [
  '../../factory/font_render.cpp',
  '../../globals.cpp',
]

test_load = ['load.cpp']

includes = include_directories('../..')

executable('test-mugen-view', test_view + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: [sdl_graphics_library, rtech1_library, mugen_library],
  link_args: test_link_args,
  )

executable('test-mugen-load', test_load + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: [sdl_graphics_library, rtech1_library, mugen_library],
  link_args: test_link_args,
  )
