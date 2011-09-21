#include "main.h"

#ifdef USE_ALLEGRO
#include <allegro.h>

int main( int argc, char ** argv ){
    return paintown_main(argc, argv);
}
END_OF_MAIN()
#endif

#ifdef USE_ALLEGRO5
#ifndef IPHONE
int main(int argc, char ** argv){
    return paintown_main(argc, argv);
}
#endif
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
    return paintown_main(argc, argv);
}
#else
int main( int argc, char ** argv ){
    return paintown_main(argc, argv);
}
#endif
#endif
#endif
