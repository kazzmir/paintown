#!/bin/bash

TEMP_DIR=".tmp"
BUILD_DIR="build-mingw"

mkdir -p ${BUILD_DIR}/cross-env

function download () {
    NAME="$1"
    URL="$2"
    FOLDER="$3"

    if [ ! -d "${TEMP_DIR}/${FOLDER}" ]; then
        echo "Downloading ${NAME}..."
        mkdir -p ${TEMP_DIR}/${FOLDER}
        wget -q -O- "${URL}" | tar -xz -C "${TEMP_DIR}/${FOLDER}" --strip-components=1
    else
        echo "${NAME} already downloaded..."
    fi

    if [ -d "${TEMP_DIR}/${FOLDER}/x86_64-w64-mingw32" ]; then
        cp -R ${TEMP_DIR}/${FOLDER}/x86_64-w64-mingw32/* ${BUILD_DIR}/cross-env
    fi
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
    fi
}

download "SDL" "https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-devel-2.28.5-mingw.tar.gz" "SDL"
download "SDL_image" "https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.2/SDL2_image-devel-2.8.2-mingw.tar.gz" "SDL_image"
download "SDL_ttf" "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.20.2/SDL2_ttf-devel-2.20.2-mingw.tar.gz" "SDL_ttf"
download "SDL_mixer" "https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.6.3/SDL2_mixer-devel-2.6.3-mingw.tar.gz" "SDL_mixer"
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

includes = include_directories('../../build-mingw/cross-env/include/SDL2')

sdl_graphics_library = static_library('SDL2_gfx', sources,
  include_directories: [includes],
  dependencies: [sdl_dependency, zlib_dependency],
  )
EOT