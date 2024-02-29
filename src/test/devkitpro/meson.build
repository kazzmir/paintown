
filesystem_test = ['file-system.cpp']
select_test = ['select-main.cpp']
animation_test = ['animation.cpp']

test_extra_files = [
    '../../globals.cpp',
  '../../factory/collector.cpp',
  '../../factory/font_render.cpp',
]

includes = include_directories('../..')

executable('test-fs', filesystem_test + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: test_linkages,
  link_args: test_link_args,
  )

executable('test-paintown-select', select_test + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: test_linkages,
  link_args: test_link_args,
  )

executable('test-animation', animation_test + test_extra_files,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: test_linkages,
  link_args: test_link_args,
  )