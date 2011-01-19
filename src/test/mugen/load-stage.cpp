#ifdef USE_ALLEGRO
#include <allegro.h>
#elif USE_SDL
#include <SDL/SDL.h>
#endif

#include "util/bitmap.h"
#include "util/file-system.h"
#include "mugen/stage.h"
#include "mugen/exception.h"
#include "globals.h"
#include "util/debug.h"
#include "util/input/input-manager.h"
#include "factory/collector.h"

#include <iostream>

using namespace std;

int paintown_main(int argc, char ** argv){
#ifdef USE_ALLEGRO
    install_allegro(SYSTEM_NONE, &errno, atexit);
    set_color_depth(16);
    set_color_conversion(COLORCONV_NONE);
#endif
#ifdef USE_SDL
    SDL_Init(SDL_INIT_VIDEO);
#endif
    Collector janitor;
    Bitmap::setFakeGraphicsMode(640, 480);
    InputManager input;

    // for (int i = 0; i < 3; i++){
        try{
            const char * file = "mugen/stages/kfm.def";
            Global::debug(0, "test") << "Loading " << file << endl;
            Mugen::Stage stage(Filesystem::find(Filesystem::RelativePath(file)));
            stage.load();
            Global::debug(0, "test") << "Success" << endl;
        } catch (const MugenException & e){
            Global::debug(0, "test") << "Exception: " << e.getReason() << endl;
        } catch (const Filesystem::NotFound & e){
            Global::debug(0, "test") << "Exception: " << e.getTrace() << endl;
        }
    // }
#ifdef USE_SDL
        SDL_Quit();
#endif

}

int main(int argc, char ** argv){
    paintown_main(argc, argv);
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
