#!/bin/bash

BUILD_DIR="build-mingw"

function download {
    echo "Downloading SDL..."
    wget -q -O- "https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-devel-2.28.5-mingw.tar.gz" | tar -xz -C "build-mingw/SDL" --strip-components=1
}

if [ -z "${BUILD_DIR}/SDL" ]; then
    echo "Downloading SDL..."
    wget -q -O- "https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-devel-2.28.5-mingw.tar.gz" | tar -xz -C "build-mingw/SDL" --strip-components=1
else
    echo "SDL already downloaded..."
fi

