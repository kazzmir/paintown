graphics_test = ['graphics.cpp']
lit_bitmap_test = ['lit-bitmap.cpp']
thread_test = ['thread.cpp']

test_extra_files = [
  '../../factory/font_render.cpp',
  '../../globals.cpp',
]

includes = include_directories('../..')

executable('test-graphics', graphics_test + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: [sdl_graphics_library, rtech1_library],
  link_args: test_link_args,
  )

executable('test-lit-bitmap', lit_bitmap_test + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: [sdl_graphics_library, rtech1_library],
  link_args: test_link_args,
  )

executable('test-thread', thread_test + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: [sdl_graphics_library, rtech1_library],
  link_args: test_link_args,
  )

