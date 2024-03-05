#!/bin/bash

PAINTOWN_HOME=/home/deck/devkit-game/Paintown

cd ${PAINTOWN_HOME}

if [ ! -z data ]; then
    wget -qO- https://github.com/kazzmir/paintown/releases/download/v3.6.0/data-3.6.0.zip -O temp.zip && unzip -qo temp.zip && rm temp.zip
fi

/home/deck/.steam/root/ubuntu12_64/steam-runtime-sniper/run \
-- \
${PAINTOWN_HOME}/paintown \
    -d ${PAINTOWN_HOME}/data