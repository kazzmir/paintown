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

DATA_PATH=data

if [ ! -d ${DATA_PATH} ]; then
    wget -qO- https://github.com/kazzmir/paintown/releases/download/v3.6.0/data-3.6.0.zip -O temp.zip && unzip -qo temp.zip && rm temp.zip
    mv data-3.6.0 data
fi

if [ $# -eq 0 ]; then
    ${RUNTIME}/run \
    -- \
    ${PAINTOWN_HOME}/paintown \
        -d ${PAINTOWN_HOME}/${DATA_PATH}
else
    ${RUNTIME}/run \
    -- \
    ${PAINTOWN_HOME}/paintown \
        $1 \
        $2 \
        $3 \
        $4 \
        $5 \
        $6 \
        $7 \
        $8 \
        $9 \
        $10
fi