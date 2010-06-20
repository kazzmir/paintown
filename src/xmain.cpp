#include "main.h"

#ifdef USE_ALLEGRO
#include <allegro.h>

int main( int argc, char ** argv ){
    return paintown_main(argc, argv);
}
END_OF_MAIN()
#endif

#ifdef MINPSPW
#include <pspkernel.h>
// PSP_MODULE_INFO("Paintown", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
PSP_HEAP_SIZE_MAX();
#endif

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
