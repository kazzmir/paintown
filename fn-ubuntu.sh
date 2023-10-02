install_rtech1(){
  URL=$1
  TDIR=$(mktemp -d)
  cd $TDIR \
      && git clone $URL . \
      && mkdir build && cd build \
      && cmake .. && make && make install \
      && rm -rf $TDIR
}

install_allegro4(){
  apt install -y scons liballegro-dev libpng-dev libfreetype6-dev g++ zlib1g-dev python2-dev python3-dev make
}

install_allegro5(){
  apt install -y liballegro5-dev liballegro-acodec5-dev
}

install_sdl1(){
  apt install -y scons libsdl1.2-dev libpng-dev libfreetype6-dev g++ zlib1g-dev python2-dev python3-dev make libogg-dev libvorbis-dev libmpg123-dev
}

install_sdl2(){
  apt install -y libsdl2-dev libsdl2-mixer-dev
}

install_dependencies(){
  install_allegro4 \
  && install_allegro5 \
  && install_sdl1 \
  && install_sdl2 \
  && install_rtech1 "https://github.com/humbertodias/paintown-rtech1"
}
