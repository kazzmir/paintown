#!/bin/bash

TEMP_DIR=".tmp"
BUILD_DIR="build-psp"

mkdir -p ${BUILD_DIR}/cross-env

#rm -rf ${TEMP_DIR}/*

function download () {
    NAME="$1"
    URL="$2"
    FOLDER="$3"
    IGNORE_BINS_DIR="$4"

    if [ ! -d "${TEMP_DIR}/${FOLDER}" ]; then
        echo "Downloading ${NAME}..."
        mkdir -p ${TEMP_DIR}/${FOLDER}
        wget -q -O- "${URL}" | tar -xz -C "${TEMP_DIR}/${FOLDER}" --strip-components=1
    else
        echo "${NAME} already downloaded..."
    fi

    if [ ! -z "${IGNORE_BINS_DIR}" ] && [ "${IGNORE_BINS_DIR}" -eq "1" ]; then
        return
    fi

    # if [ -d "${TEMP_DIR}/${FOLDER}/x86_64-w64-psp32" ]; then
    #     cp -R ${TEMP_DIR}/${FOLDER}/x86_64-w64-psp32/* ${BUILD_DIR}/cross-env
    # fi
}

function git_clone () {
    NAME="$1"
    URL="$2"
    FOLDER="$3"

    if [ ! -d "${TEMP_DIR}/${FOLDER}" ]; then
        echo "Checking out ${NAME}..."
        git clone ${URL} -- ${TEMP_DIR}/${FOLDER}
    else
        echo "${NAME} already downloaded..."
        #cd ${TEMP_DIR}/${FOLDER}
        #git pull
        #cd ../..
    fi
}

# download "SDL" "https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-2.28.5.tar.gz" "SDL"
# download "SDL_image" "https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.2/SDL2_image-2.8.2.tar.gz" "SDL_image"
# download "SDL_ttf" "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.2/SDL2_ttf-2.20.2.tar.gz" "SDL_ttf"
# download "SDL_mixer" "https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.6.3/SDL2_mixer-2.6.3.tar.gz" "SDL_mixer"
#download "SDL_gfx" "https://github.com/mupfdev/SDL2_gfx/archive/refs/tags/1.0.6.tar.gz" "SDL_gfx"

# SDL_gfx library, make static and include in build
git_clone "SDL_gfx" "https://github.com/ferzkopp/SDL2_gfx.git" "SDL_gfx"
cp ${TEMP_DIR}/SDL_gfx/*.h ${BUILD_DIR}/cross-env/include/SDL2
cat >${TEMP_DIR}/SDL_gfx/meson.build <<'EOT'
sources = [
  'SDL2_framerate.c',
  'SDL2_gfxPrimitives.c',  
  'SDL2_imageFilter.c',  
  'SDL2_rotozoom.c',
]

#includes = include_directories('../../build-psp/cross-env/include/SDL2')


sdl_graphics_library = static_library('SDL2_gfx', sources,
  include_directories: [includes],
  dependencies: [sdl_dependency, zlib_dependency],
  cpp_args : []
  )
EOT

# yaml-cpp library
git_clone "yaml-cpp" "https://github.com/jbeder/yaml-cpp.git" "yaml-cpp"
mkdir -p ${BUILD_DIR}/cross-env/include/yaml-cpp/{contrib,node/detail}
cp ${TEMP_DIR}/yaml-cpp/include/yaml-cpp/*.h ${BUILD_DIR}/cross-env/include/yaml-cpp
cp ${TEMP_DIR}/yaml-cpp/include/yaml-cpp/contrib/*.h ${BUILD_DIR}/cross-env/include/yaml-cpp/contrib
cp ${TEMP_DIR}/yaml-cpp/include/yaml-cpp/node/*.h ${BUILD_DIR}/cross-env/include/yaml-cpp/node
cp ${TEMP_DIR}/yaml-cpp/include/yaml-cpp/node/detail/*.h ${BUILD_DIR}/cross-env/include/yaml-cpp/node/detail
cat >${TEMP_DIR}/yaml-cpp/meson.build <<'EOT'
# Borrowed from here: https://github.com/jbeder/yaml-cpp/pull/699/files
#project('yaml-cpp', 'cpp',
#  version : '0.8.0',
#  meson_version : '>=0.46.0',
#  license : 'MIT')

sources = files([
  'src/binary.cpp',
  'src/convert.cpp',
  'src/depthguard.cpp',
  'src/directives.cpp',
  'src/emit.cpp',
  'src/emitfromevents.cpp',
  'src/emitter.cpp',
  'src/emitterstate.cpp',
  'src/emitterutils.cpp',
  'src/exceptions.cpp',
  'src/exp.cpp',
  'src/memory.cpp',
  'src/nodebuilder.cpp',
  'src/node.cpp',
  'src/node_data.cpp',
  'src/nodeevents.cpp',
  'src/null.cpp',
  'src/ostream_wrapper.cpp',
  'src/parse.cpp',
  'src/parser.cpp',
  'src/regex_yaml.cpp',
  'src/scanner.cpp',
  'src/scanscalar.cpp',
  'src/scantag.cpp',
  'src/scantoken.cpp',
  'src/simplekey.cpp',
  'src/singledocparser.cpp',
  'src/stream.cpp',
  'src/tag.cpp',
])

public_headers = files([
  'include/yaml-cpp/anchor.h',
  'include/yaml-cpp/binary.h',
  'include/yaml-cpp/depthguard.h',
  'include/yaml-cpp/dll.h',
  'include/yaml-cpp/emitfromevents.h',
  'include/yaml-cpp/emitterdef.h',
  'include/yaml-cpp/emitter.h',
  'include/yaml-cpp/emittermanip.h',
  'include/yaml-cpp/emitterstyle.h',
  'include/yaml-cpp/eventhandler.h',
  'include/yaml-cpp/exceptions.h',
  'include/yaml-cpp/mark.h',
  'include/yaml-cpp/noexcept.h',
  'include/yaml-cpp/null.h',
  'include/yaml-cpp/ostream_wrapper.h',
  'include/yaml-cpp/parser.h',
  'include/yaml-cpp/stlemitter.h',
  'include/yaml-cpp/traits.h',
  'include/yaml-cpp/yaml.h',
])

public_contrib_headers = files([
  'include/yaml-cpp/contrib/anchordict.h',
  'include/yaml-cpp/contrib/graphbuilder.h',
])

public_node_headers = files([
  'include/yaml-cpp/node/convert.h',
  'include/yaml-cpp/node/emit.h',
  'include/yaml-cpp/node/impl.h',
  'include/yaml-cpp/node/iterator.h',
  'include/yaml-cpp/node/node.h',
  'include/yaml-cpp/node/parse.h',
  'include/yaml-cpp/node/ptr.h',
  'include/yaml-cpp/node/type.h',
])

public_node_detail_headers = files([
  'include/yaml-cpp/node/detail/impl.h',
  'include/yaml-cpp/node/detail/iterator_fwd.h',
  'include/yaml-cpp/node/detail/iterator.h',
  'include/yaml-cpp/node/detail/memory.h',
  'include/yaml-cpp/node/detail/node_data.h',
  'include/yaml-cpp/node/detail/node.h',
  'include/yaml-cpp/node/detail/node_iterator.h',
  'include/yaml-cpp/node/detail/node_ref.h',
])

should_install = not meson.is_subproject()

yaml_cpp_library = static_library('yaml-cpp', sources,
  include_directories : 'include',
  install : should_install,
  cpp_args : ['-DYAML_CPP_STATIC_DEFINE']
  )

if should_install
  install_headers(public_headers, subdir : 'yaml-cpp')
  install_headers(public_contrib_headers, subdir : 'yaml-cpp/contrib')
  install_headers(public_node_headers, subdir : 'yaml-cpp/node')
  install_headers(public_node_detail_headers, subdir : 'yaml-cpp/node/detail')

  pkg = import('pkgconfig')
  pkg.generate(yaml_cpp_library,
    name : 'libyaml-cpp',
    description : 'A YAML parser and emitter in C++',
  )

  # TODO: it seems possible to generate cmake packages using meson
  #       but I do not know how to generate yaml-cpp-targets.cmake
  # cmake = import('cmake')
  # conf = configuration_data()
  # conf.set('CONFIG_INCLUDE_DIRS', '${YAML_CPP_CMAKE_DIR}/../../../include')
  # conf.set('EXPORT_TARGETS', 'yaml-cpp')

  # cmake.configure_package_config_file(
  #   name : 'yaml-cpp',
  #   input : 'yaml-cpp-config.cmake.in',
  #   configuration : conf,
  # )
  # 
  # cmake.write_basic_package_version_file(
  #   name : 'yaml-cpp',
  #   version : meson.project_version(),
  # )
endif

yaml_cpp_dependency = declare_dependency(
  include_directories : 'include',
  link_with : yaml_cpp_library,
  version : meson.project_version(),
)

#gtest_dep = dependency('gtest', fallback : ['gtest', 'gtest_dep'], required : false)
#gmock_dep = dependency('gmock', fallback : ['gtest', 'gmock_dep'], required : false)
#if gtest_dep.found()
#  subdir('test')
#  test('all tests', tests)
#endif
EOT

# FIXME zlib library 
#download "zlib" "https://www.zlib.net/current/zlib.tar.gz" "zlib" "1"
#cat >${TEMP_DIR}/zlib/meson.build <<'EOT'
#sources = [
#  'adler32.c',
#  'compress.c',
#  'crc32.c',
#  'deflate.c',
#  'gzclose.c',
#  'gzlib.c',
#  'gzread.c',
#  'gzwrite.c',
#  'infback.c',
#  'inffast.c',
#  'inflate.c',
#  'inftrees.c',
#  'trees.c',
#  'uncompr.c',
#  'zutil.c',
#]
#
#includes = include_directories('.')
#
#zlib_library = static_library('z', sources,
#  include_directories: [includes],
#  dependencies: [],
#  )
#EOT