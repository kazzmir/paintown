
select_test = [
  'select-main.cpp'
]

animation_test = ['animation.cpp']

test_extra_files = [
  '../../factory/collector.cpp',
  '../../factory/font_render.cpp',
  '../../globals.cpp',
]

includes = include_directories('../..')

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
