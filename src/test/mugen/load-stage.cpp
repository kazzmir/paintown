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
#include "input/input-manager.h"

#include <iostream>

using namespace std;

int main(int argc, char ** argv){
#ifdef USE_ALLEGRO
    install_allegro(SYSTEM_NONE, &errno, atexit);
    set_color_depth(16);
    set_color_conversion(COLORCONV_NONE);
#endif
#ifdef USE_SDL
    SDL_Init(SDL_INIT_VIDEO);
#endif
    Bitmap::setFakeGraphicsMode(640, 480);
    InputManager input;

    // for (int i = 0; i < 3; i++){
        try{
            const char * file = "mugen/stages/kfm.def";
            Global::debug(0, "test") << "Loading " << file << endl;
            MugenStage stage(Filesystem::find(Filesystem::RelativePath(file)));
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
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
