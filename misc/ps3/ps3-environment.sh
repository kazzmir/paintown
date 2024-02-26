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
        if [ ! -d build ]; then 
            mkdir build 
            cd build
            cmake $CMAKE_OPTIONS ..
            make 
            if [ $MAKE_INSTALL == 'true' ]; then
                make install
            fi
        else
            echo "${FOLDER} already compiled..."
        fi 
    )
}

git_clone https://github.com/libsdl-org/SDL SDL2 SDL
cmake_install SDL "-DSDL_STATIC=ON 
-DBUILD_SHARED_LIBS=OFF 
-DSDL_TEST_ENABLED_BY_DEFAULT=OFF 
-DSDL_DISABLE_IMMINTRIN_H=ON
-DSDL_SHARED_ENABLED_BY_DEFAULT=OFF
-DSDL_LOADSO_ENABLED_BY_DEFAULT=OFF
-DSDL_STATIC_ENABLED_BY_DEFAULT=ON
-DSDL_PTHREADS_ENABLED_BY_DEFAULT=OFF
"
 
apt install -y libpng-dev libjpeg-dev libtiff-dev libwebp-dev
git_clone https://github.com/libsdl-org/SDL_image SDL2 SDL_image
cmake_install SDL_image "-DBUILD_SHARED_LIBS=OFF -DSDL2IMAGE_SAMPLES=OFF"

apt install -y libfreetype-dev
git_clone https://github.com/libsdl-org/SDL_ttf SDL2 SDL_ttf
cmake_install SDL_ttf "-DSDL_STATIC=ON -DBUILD_SHARED_LIBS=OFF -DSDL2TTF_SAMPLES=OFF"

apt install -y libxmp-dev libwavpack-dev libmpg123-dev libvorbis-dev
git_clone http://github.com/libsdl-org/SDL_mixer SDL2 SDL_mixer
cmake_install SDL_mixer "-DBUILD_SHARED_LIBS=OFF 
-DSDL2MIXER_MIDI=OFF 
-DSDL2MIXER_VENDORED=OFF 
-DSDL2MIXER_MOD=OFF 
-DSDL2MIXER_SAMPLES=OFF 
-DSDL2MIXER_OPUS=OFF
-DCMAKE_POSITION_INDEPENDENT_CODE=OFF
"

git_clone https://github.com/jbeder/yaml-cpp master yaml_cpp
cmake_install yaml_cpp "-DYAML_BUILD_SHARED_LIBS=OFF -DYAML_ENABLE_PIC=OFF -DYAML_CPP_INSTALL=ON"

git_clone https://github.com/mupfdev/SDL2_gfx main SDL_gfx
cmake_install SDL_gfx "-DBUILD_SHARED_LIBS=OFF" false
cp $TEMP_DIR/SDL_gfx/build/libSDL2_gfx.a /usr/local/lib/
echo 'prefix=/usr/local
exec_prefix=${prefix}
libdir=${prefix}/lib
includedir=${prefix}/include

Name: SDL2_gfx
Description: drawing and graphical effects extension for SDL
Version: 1.0.6
Requires: sdl2 >= 2.0.0
Libs: -L${libdir} -lSDL2_gfx
Cflags: -I${includedir}/SDL2' > /usr/local/lib/pkgconfig/SDL2_gfx.pc