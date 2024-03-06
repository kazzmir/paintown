#!/bin/bash

if [ -d '/home/deck' ]; then
    HOME_DIR="/home/deck"
else
    HOME_DIR="$HOME"
fi

RUNTIME=${HOME_DIR}/.steam/root/ubuntu12_64/steam-runtime-sniper

if [ ! -d "${RUNTIME}" ]; then
    echo "Cannot find runtime: ${RUNTIME}"
    exit 1
fi

PAINTOWN_HOME="$PWD"
#PAINTOWN_HOME=${HOME_DIR}/devkit-game/Paintown

#cd ${PAINTOWN_HOME}

if [ ! -d data ]; then
    wget -qO- https://github.com/kazzmir/paintown/releases/download/v3.6.0/data-3.6.0.zip -O temp.zip && unzip -qo temp.zip && rm temp.zip
fi

${RUNTIME}/run \
-- \
${PAINTOWN_HOME}/paintown \
    -d ${PAINTOWN_HOME}/data