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
  && install_sdl2
}
