FROM ubuntu:22.04

# Common
RUN apt update -y && apt install -y scons meson cmake python2-dev vim-tiny git make g++
# Allegro 4
RUN apt install -y liballegro-dev libpng-dev libfreetype6-dev zlib1g-dev
# Allegro 5
RUN apt install -y liballegro5-dev liballegro-acodec5-dev
# SDL1
RUN apt install -y libsdl1.2-dev libsdl-mixer1.2-dev libpng-dev libfreetype6-dev zlib1g-dev libogg-dev libvorbis-dev libmpg123-dev
# SDL2
RUN apt install -y libsdl2-dev libsdl2-mixer-dev

# rtech1 dependency
ARG RTECH1_GITURL=https://github.com/humbertodias/paintown-rtech1
RUN cd /tmp \
    && git clone ${RTECH1_GITURL} r-tech1 \
    && cd r-tech1 && mkdir build && cd build && cmake .. && make && make install \
    && rm -rf /tmp/r-tech1
