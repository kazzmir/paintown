#include <allegro.h>

#include <iostream>
#include "util/file-system.h"
#include "mugen/character.h"
#include "mugen/exception.h"
#include "mugen/parse-cache.h"
#include "util/timedifference.h"

using namespace std;

int main(int argc, char ** argv){
    install_allegro(SYSTEM_NONE, &errno, atexit);
    set_color_depth(16);
    set_color_conversion(COLORCONV_NONE);
    Mugen::ParseCache cache;

    // for (int i = 0; i < 3; i++){
        try{
            TimeDifference diff;
            diff.startTime();
            Mugen::Character kfm(Filesystem::find(Filesystem::RelativePath("mugen/chars/kfm/kfm.def")));
            kfm.load();
            diff.endTime();
            Global::debug(0, "test") << "Success! Took " << diff.printTime() << endl;
        } catch (const MugenException & e){
            Global::debug(0, "test") << "Test failure!: " << e.getReason() << endl;
        }
    // }
}
END_OF_MAIN()
