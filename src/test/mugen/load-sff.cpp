#include "../common/init.h"

#include <iostream>
#include "util/thread.h"
#include "util/message-queue.h"
#include "util/file-system.h"
#include "mugen/character.h"
#include "mugen/exception.h"
#include "mugen/parse-cache.h"
#include "util/timedifference.h"
#include "util/bitmap.h"
#include "util/debug.h"
#include "mugen/util.h"

using namespace std;

static int load(const char * path){
    // showMemory();
    for (int i = 0; i < 1; i++){
        try{
            Mugen::SpriteMap sprites;
            TimeDifference diff;
            Global::debug(0) << "Loading " << path << endl;
            diff.startTime();
            Mugen::Util::readSprites(Filesystem::AbsolutePath(path), Filesystem::AbsolutePath(), sprites, false);
            diff.endTime();
            Global::debug(0, "test") << diff.printTime("Success! Took") << endl;
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

    int die = 0;
    if (argc < 2){
        die = load("data/mugen/chars/kfm/kfm.sff");
    } else {
        die = load(argv[1]);
    }

    Screen::fakeFinish();

    return die;
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
