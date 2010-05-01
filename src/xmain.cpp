#ifdef USE_ALLEGRO
#include <allegro.h>
#include "main.h"

int main( int argc, char ** argv ){
    return paintown_main(argc, argv);
}
END_OF_MAIN()
#endif

#ifdef USE_SDL
#include "main.h"
int main( int argc, char ** argv ){
    return paintown_main(argc, argv);
}
#endif
