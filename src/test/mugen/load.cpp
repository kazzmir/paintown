#ifdef USE_ALLEGRO
#include <allegro.h>
#endif
#ifdef USE_SDL
#include <SDL/SDL.h>
#endif

#include <iostream>
#include "util/file-system.h"
#include "mugen/character.h"
#include "mugen/exception.h"
#include "mugen/parse-cache.h"
#include "util/timedifference.h"
#include "util/bitmap.h"

using namespace std;

static void load(const char * path){
    try{
        TimeDifference diff;
        diff.startTime();
        Global::debug(0) << "Loading " << path << endl;
        Mugen::Character kfm(Filesystem::find(Filesystem::RelativePath(path)));
        kfm.load();
        diff.endTime();
        Global::debug(0, "test") << diff.printTime("Success! Took") << endl;
    } catch (const MugenException & e){
        Global::debug(0, "test") << "Test failure!: " << e.getReason() << endl;
    } catch (const Filesystem::NotFound & e){
        Global::debug(0, "test") << "Test failure! Couldn't find a file: " << e.getTrace() << endl;
    }
}

int main(int argc, char ** argv){
#ifdef USE_ALLEGRO
    install_allegro(SYSTEM_NONE, &errno, atexit);
    set_color_depth(16);
    set_color_conversion(COLORCONV_NONE);
#elif USE_SDL
    SDL_Init(SDL_INIT_VIDEO);
    Bitmap::setFakeGraphicsMode(640, 480);
#endif

    Mugen::ParseCache cache;

    if (argc < 2){
        load("mugen/chars/kfm/kfm.def");
    } else {
        load(argv[1]);
    }

    // for (int i = 0; i < 3; i++){
      // }
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
