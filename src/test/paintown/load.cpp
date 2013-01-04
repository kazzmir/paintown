#include "../common/init.h"

#include <iostream>
#include "util/configuration.h"
#include "util/font.h"
#include "util/message-queue.h"
#include "util/file-system.h"
#include "util/input/input-source.h"
#include "util/timedifference.h"
#include "util/graphics/bitmap.h"
#include "util/sound/sound.h"
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
            Paintown::Player player(Storage::instance().find(Filesystem::RelativePath(path)), Util::ReferenceCount<InputSource>(new InputSource()));
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
    Screen::fakeInit();
    Collector janitor;
    Sound::initialize();
    Util::Thread::initializeLock(&MessageQueue::messageLock);

    Util::Parameter<Util::ReferenceCount<Path::RelativePath> > defaultFont(Font::defaultFont, Util::ReferenceCount<Path::RelativePath>(new Path::RelativePath("fonts/arial.ttf")));
    Configuration::loadConfigurations();
    Paintown::Mod::loadDefaultMod();
    Global::setDebug(1);

    int die = 0;
    if (argc < 2){
        die = load("players/akuma/akuma.txt");
    } else {
        die = load(argv[1]);
    }

    Screen::fakeFinish();
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
