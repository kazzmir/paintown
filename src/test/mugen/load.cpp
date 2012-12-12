#include "../common/init.h"

#include <iostream>
#include "util/thread.h"
#include "util/message-queue.h"
#include "util/file-system.h"
#include "mugen/character.h"
#include "mugen/exception.h"
#include "mugen/parse-cache.h"
#include "util/timedifference.h"
#include "util/graphics/bitmap.h"
#include "util/debug.h"

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

static void mount(const char * path){
    Filesystem::AbsolutePath container(path);
    Filesystem::AbsolutePath where(container.getDirectory());
    Storage::instance().addOverlay(container, where);
}

static int load(const char * path){
    // showMemory();
    for (int i = 0; i < 1; i++){
        try{
            TimeDifference diff;
            diff.startTime();
            Global::debug(0) << "Loading " << path << endl;
            Mugen::Character kfm(Storage::instance().find(Filesystem::RelativePath(path)), 0);
            kfm.load();
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

int main(int argc, char ** argv){
    Screen::fakeInit();

    Global::setDebug(1);
    Mugen::ParseCache cache;
    Util::Thread::initializeLock(&Global::messageLock);

    int die = 0;
    if (argc < 2){
        die = load("mugen/chars/kfm/kfm.def");
    } else {
        if (argc > 2){
            mount(argv[1]);
            die = load(argv[2]);
        } else {
            die = load(argv[1]);
        }
    }

    Screen::fakeFinish();

    // for (int i = 0; i < 3; i++){
      // }
    return die;
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
