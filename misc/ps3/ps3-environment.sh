#!/bin/bash -e

TEMP_DIR=".tmp"
BUILD_DIR="build-ps3"

function git_clone () {
    URL="$1"
    BRANCH="$2"
    FOLDER="$3"

    if [ ! -d "${TEMP_DIR}/${FOLDER}" ]; then
        echo "Checking out ${NAME}..."
        git clone ${URL} -b ${BRANCH} --single-branch -- ${TEMP_DIR}/${FOLDER}
    else
        echo "${FOLDER} already downloaded..."
    fi
}

function cmake_install () {
    FOLDER="$1"
    CMAKE_OPTIONS="$2"
    MAKE_INSTALL="${3:-true}"
    (
        cd "$TEMP_DIR/$FOLDER" ;  \
        # if [ ! -d build ]; then 
            mkdir -p build 
            cd build
            cmake $CMAKE_OPTIONS ..
            make 
            if [ $MAKE_INSTALL == 'true' ]; then
                make install
            fi
        # else
        #     echo "${FOLDER} already compiled..."
        # fi 
    )
}

apt update
# git_clone https://github.com/libsdl-org/SDL SDL2 SDL
# cmake_install SDL "-DSDL_STATIC=ON 
# -DBUILD_SHARED_LIBS=OFF 
# -DSDL_TEST_ENABLED_BY_DEFAULT=OFF 
# -DSDL_DISABLE_IMMINTRIN_H=ON
# -DSDL_SHARED_ENABLED_BY_DEFAULT=OFF
# -DSDL_LOADSO_ENABLED_BY_DEFAULT=OFF
# -DSDL_STATIC_ENABLED_BY_DEFAULT=ON
# -DSDL_PTHREADS_ENABLED_BY_DEFAULT=OFF
# -DCMAKE_INSTALL_PREFIX=/ps3dev/portlibs/ppu
# "

export PKG_CONFIG_PATH=/ps3dev/portlibs/ppu/lib/pkgconfig
 
# apt install -y libpng-dev libjpeg-dev libtiff-dev libwebp-dev
git_clone https://github.com/libsdl-org/SDL_image SDL2 SDL_image
# cmake_install SDL_image "-DBUILD_SHARED_LIBS=OFF -DSDL2IMAGE_SAMPLES=OFF 
# -DSDL3IMAGE_TIF=OFF
# -DSDL3IMAGE_WEBP=OFF
# -DSDL2IMAGE_DEPS_SHARED=OFF
# -DCMAKE_INSTALL_PREFIX=/ps3dev/portlibs/ppu
# -DCMAKE_C_COMPILER=ppu-gcc
# -DCMAKE_CXX_COMPILER=ppu-g++
# "

echo "project('sdl2-image', ['c', 'cpp'], default_options: ['cpp_std=c++11'], version: '4.0')
feature_args = ['-DLOAD_BMP', '-DLOAD_PNG', '-DLOAD_JPG', '-DSDL_DISABLE_IMMINTRIN_H=ON']
other_dependencies = []
sdl2_dep = dependency('sdl2', required: true, static: true)
other_dependencies += [dependency('libpng', required: true, static: true),
                       dependency('libjpeg', required: true, static: true),]

sdl_image_library = static_library('SDL2_image',
  'src/IMG_avif.c',
  'src/IMG_bmp.c',
  'src/IMG.c',
  'src/IMG_gif.c',
  'src/IMG_jpg.c',
  'src/IMG_jxl.c',
  'src/IMG_lbm.c',
  'src/IMG_pcx.c',
  'src/IMG_png.c',
  'src/IMG_pnm.c',
  'src/IMG_qoi.c',
  'src/IMG_stb.c',
  'src/IMG_svg.c',
  'src/IMG_tga.c',
  'src/IMG_tif.c',
  'src/IMG_webp.c',
  'src/IMG_WIC.c',
  'src/IMG_xcf.c',
  'src/IMG_xpm.c',
  'src/IMG_xv.c',
  'src/IMG_xxx.c',
  c_args: feature_args,
  include_directories: ['.','src','include', '/usr/include'],
  dependencies: [other_dependencies, sdl2_dep])

sdl_image_dependency = declare_dependency(include_directories: ['.','src','include'],
  link_with: sdl_image_library)" > .tmp/SDL_image/meson.build

# apt install -y libfreetype-dev libavif-dev
git_clone https://github.com/libsdl-org/SDL_ttf SDL2 SDL_ttf
# cmake_install SDL_ttf "-DSDL_STATIC=ON -DBUILD_SHARED_LIBS=OFF -DSDL2TTF_SAMPLES=OFF
# -DCMAKE_INSTALL_PREFIX=/ps3dev/portlibs/ppu
# -DSDL_DISABLE_IMMINTRIN_H=ON
# -DCMAKE_C_COMPILER=ppu-gcc
# -DCMAKE_CXX_COMPILER=ppu-g++
# "
echo "project('sdl2-ttf', ['c', 'cpp'], default_options: ['cpp_std=c++11'], version: '4.0')
feature_args = ['-DSDL_DISABLE_IMMINTRIN_H=ON', '-DDSDL2TTF_SAMPLES=OFF']
other_dependencies = []
sdl2_dep = dependency('sdl2', required: true, static: true)

sdl_ttf_library = static_library('SDL2_ttf',
  'SDL_ttf.c',
  'glfont.c',
  c_args: feature_args,
  include_directories: ['.','src','include', '/usr/include'],
  dependencies: [other_dependencies, sdl2_dep])

sdl_ttf_dependency = declare_dependency(include_directories: ['.','src','include'],
  link_with: sdl_ttf_library)" > .tmp/SDL_ttf/meson.build

# apt install -y libxmp-dev libwavpack-dev libmpg123-dev libvorbis-dev
git_clone http://github.com/libsdl-org/SDL_mixer SDL2 SDL_mixer
cmake_install SDL_mixer "-DBUILD_SHARED_LIBS=OFF 
-DSDL2MIXER_MIDI=OFF 
-DSDL2MIXER_VENDORED=OFF 
-DSDL2MIXER_MOD=OFF 
-DSDL2MIXER_SAMPLES=OFF 
-DSDL2MIXER_OPUS=OFF
-DSDL2MIXER_WAVPACK=OFF
-DSDL2MIXER_WAVE=OFF
-DSDL2MIXER_DEPS_SHARED=OFF
-DCMAKE_POSITION_INDEPENDENT_CODE=OFF
-DCMAKE_INSTALL_PREFIX=/ps3dev/portlibs/ppu
-DCMAKE_C_COMPILER=ppu-gcc
-DCMAKE_CXX_COMPILER=ppu-g++
"

git_clone https://github.com/jbeder/yaml-cpp master yaml_cpp
cmake_install yaml_cpp "-DYAML_BUILD_SHARED_LIBS=OFF -DYAML_ENABLE_PIC=OFF -DYAML_CPP_INSTALL=ON
-DCMAKE_INSTALL_PREFIX=/ps3dev/portlibs/ppu
-DCMAKE_C_COMPILER=ppu-gcc
-DCMAKE_CXX_COMPILER=ppu-g++
"

git_clone https://github.com/mupfdev/SDL2_gfx main SDL_gfx
cmake_install SDL_gfx "-DBUILD_SHARED_LIBS=OFF
-DCMAKE_INSTALL_PREFIX=/ps3dev/portlibs/ppu
-DCMAKE_C_COMPILER=ppu-gcc
-DCMAKE_CXX_COMPILER=ppu-g++
" false
cp $TEMP_DIR/SDL_gfx/build/libSDL2_gfx.a /ps3dev/portlibs/ppu/lib
echo 'prefix=/ps3dev/portlibs/ppu
exec_prefix=${prefix}
libdir=${prefix}/lib
includedir=${prefix}/include

Name: SDL2_gfx
Description: drawing and graphical effects extension for SDL
Version: 1.0.6
Requires: sdl2 >= 2.0.0
Libs: -L${libdir} -lSDL2_gfx
Cflags: -I${includedir}/SDL2' > /ps3dev/portlibs/ppu/lib/pkgconfig/SDL2_gfx.pc