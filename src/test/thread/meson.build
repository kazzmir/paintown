futures_test = ['futures.cpp']

includes = include_directories('../..')

executable('test-futures', futures_test,
  include_directories: [includes],
  dependencies: test_dependencies,
  link_with: [test_linkages, rtech1_library],
  link_args: test_link_args,
  )
