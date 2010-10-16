#ifdef USE_ALLEGRO
#include <allegro.h>
#endif
#ifdef USE_SDL
#include <SDL/SDL.h>
#endif
#include <iostream>
#include "util/token_exception.h"
#include "util/file-system.h"
#include "loadpng/loadpng.h"
#include "util/bitmap.h"
#include "factory/collector.h"
#include "menu/menu.h"

using namespace std;

void load(const char * path){
    for (int i = 0; i < 10; i++){
        Menu::Menu menu(Filesystem::find(Filesystem::RelativePath(path)));
    }
}

int paintown_main(int argc, char ** argv){
    Collector collector;
    try{
        load("menu/main.txt");
        cout << "Success" << endl;
    } catch (const TokenException & e ){
        cout << "Error: " << e.getTrace() << endl;
    } catch (const LoadException & e){
        cout << "Error: " << e.getTrace() << endl;
    }
    return 0;
}

int main(int argc, char ** argv){
#ifdef USE_ALLEGRO
    install_allegro(SYSTEM_NONE, &errno, atexit);
    set_color_depth(16);
    set_color_conversion(COLORCONV_NONE);
    Bitmap::setFakeGraphicsMode(640, 480);
    loadpng_init();
#elif USE_SDL
    SDL_Init(SDL_INIT_VIDEO);
    Bitmap::setFakeGraphicsMode(640, 480);
#endif
    return paintown_main(argc, argv);
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
