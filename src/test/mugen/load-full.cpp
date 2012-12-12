#include "../common/init.h"

#include "configuration.h"
#include "util/message-queue.h"
#include <iostream>
#include "util/file-system.h"
#include "mugen/character.h"
#include "mugen/exception.h"
#include "mugen/stage.h"
#include "mugen/parse-cache.h"
#include "util/timedifference.h"
#include "util/graphics/bitmap.h"
#include "util/debug.h"
#include "util/input/input-manager.h"

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
            Mugen::Character kfm1(Storage::instance().find(Filesystem::RelativePath(path)), 1);
            kfm1.load();
            Mugen::Character kfm2(Storage::instance().find(Filesystem::RelativePath(path)), 2);
            kfm2.load();
            const char * file = "mugen/stages/kfm.def";
            Global::debug(0, "test") << "Loading " << file << endl;
            Mugen::Stage stage(Storage::instance().find(Filesystem::RelativePath(file)));
            stage.load();

            diff.endTime();
            Global::debug(0, "test") << diff.printTime("Success! Took") << endl;
            /*
               int * x = new int[1 << 21];
               delete[] x;
               */
        } catch (const MugenException & e){
            Global::debug(0, "test") << "Test failure!: " << e.getReason() << endl;
            return 1;
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
    InputManager input;
    Util::Thread::initializeLock(&Global::messageLock);
    Configuration::loadConfigurations();
    // Filesystem::initialize();

    Global::setDebug(0);
    Mugen::ParseCache cache;

    int die = 0;
    if (argc < 2){
        die = load("mugen/chars/kfm/kfm.def");
    } else {
        die = load(argv[1]);
    }

    Screen::fakeFinish();

    // for (int i = 0; i < 3; i++){
      // }
    return die;
}

int main(int argc, char ** argv){
    paintown_main(argc, argv);
}

#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
