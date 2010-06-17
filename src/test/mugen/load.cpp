#include <allegro.h>

#include <iostream>
#include "util/file-system.h"
#include "mugen/character.h"
#include "mugen/exception.h"
#include "mugen/parse-cache.h"
#include "util/timedifference.h"

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
        Global::debug(0, "test") << "Test failure! Couldn't find a file: " << e.getReason() << endl;
    }
}

int main(int argc, char ** argv){
    install_allegro(SYSTEM_NONE, &errno, atexit);
    set_color_depth(16);
    set_color_conversion(COLORCONV_NONE);

    Mugen::ParseCache cache;

    if (argc < 2){
        load("mugen/chars/kfm/kfm.def");
    } else {
        load(argv[1]);
    }

    // for (int i = 0; i < 3; i++){
      // }
}
END_OF_MAIN()
