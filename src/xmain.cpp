#include "r-tech1/main.h"

#ifdef USE_ALLEGRO
#include <allegro.h>

int main(int argc, char ** argv){
    return rtech_main(argc, argv);
}
END_OF_MAIN()
#endif

#ifdef USE_ALLEGRO5
#include <allegro5/allegro.h>
int main(int argc, char ** argv){
    return rtech_main(argc, argv);
}
#endif

#ifdef MINPSPW
#include <pspkernel.h>
// Define only on windows because main is redefined as SDL_main (from SDL_psp_main.c comments)
#ifdef INCLUDE_SCE_MODULE_INFO
PSP_MODULE_INFO("Paintown", 0, 1, 1);
#endif 
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
PSP_HEAP_SIZE_MAX();
#endif

#ifndef NACL
#ifdef USE_SDL
#ifdef USE_SDL_MAIN
extern "C" int SDL_main(int argc, char ** argv){
    return rtech_main(argc, argv);
}
#else
int main( int argc, char ** argv ){
    return rtech_main(argc, argv);
}
#endif
#endif
#endif

#ifdef USE_SDL2
#ifndef WINDOWS
int main( int argc, char ** argv ){
    return rtech_main(argc, argv);
}
#else
// https://wiki.libsdl.org/SDL2/FAQWindows#i_get_undefined_reference_to_sdl_main_...
#include <SDL2/SDL.h>
int main(int argv, char *args[]){
    return rtech_main(argv, args);
}
#endif
#endif