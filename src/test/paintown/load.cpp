#ifdef USE_ALLEGRO
#include <allegro.h>
#endif
#ifdef USE_SDL
#include <SDL/SDL.h>
#endif
#ifdef USE_ALLEGRO5
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#endif

#include <iostream>
#include "util/message-queue.h"
#include "util/file-system.h"
#include "util/input/input-source.h"
#include "util/timedifference.h"
#include "util/bitmap.h"
#include "util/sound.h"
#include "paintown-engine/game/mod.h"
#include "paintown-engine/object/player.h"
#include "factory/collector.h"

/*
#include <sstream>
#include <unistd.h>
*/

using namespace std;

/*
static int getPid(){
    return getpid();
}

static void runPmap(int pid){
    ostringstream out;
    out << "pmap " << pid;
    system(out.str().c_str());
}

static void showMemory(){
    runPmap(getPid());
}
*/

static int load(const char * path){
    // showMemory();
    for (int i = 0; i < 1; i++){
        try{
            TimeDifference diff;
            diff.startTime();
            Global::debug(0) << "Loading " << path << endl;
            Paintown::Player player(Storage::instance().find(Filesystem::RelativePath(path)), new InputSource());
            diff.endTime();
            Global::debug(0, "test") << diff.printTime("Success! Took") << endl;
        } catch (const Filesystem::NotFound & e){
            Global::debug(0, "test") << "Test failure! Couldn't find a file: " << e.getTrace() << endl;
            return 1;
        }
    }
    return 0;
    // showMemory();
}

int paintown_main(int argc, char ** argv){
#ifdef USE_ALLEGRO
    install_allegro(SYSTEM_NONE, &errno, atexit);
    set_color_depth(16);
    set_color_conversion(COLORCONV_NONE);
#elif USE_SDL
    SDL_Init(SDL_INIT_VIDEO);
    Graphics::Bitmap::setFakeGraphicsMode(640, 480);
#elif USE_ALLEGRO5
    al_init();
    al_init_image_addon();
    al_init_primitives_addon();
#endif
    Collector janitor;
    Sound::initialize();
    Util::Thread::initializeLock(&Global::messageLock);

    Paintown::Mod::loadDefaultMod();
    Global::setDebug(1);

    int die = 0;
    if (argc < 2){
        die = load("players/akuma/akuma.txt");
    } else {
        die = load(argv[1]);
    }

#ifdef USE_SDL
    SDL_Quit();
#endif
    Sound::uninitialize();

    // for (int i = 0; i < 3; i++){
      // }
    return die;
}

int main(int argc, char ** argv){
    return paintown_main(argc, argv);
}

#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
