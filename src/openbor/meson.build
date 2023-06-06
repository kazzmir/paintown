sources = [
  'animation.cpp',
  'data.cpp',
  'display-character.cpp',
  'mod.cpp',
  'pack-reader.cpp',
  'util.cpp',
]

includes = include_directories('..')
openbor_library = static_library('openbor', sources,
  include_directories: [includes],
  dependencies: [sdl_dependency, zlib_dependency],
  )
