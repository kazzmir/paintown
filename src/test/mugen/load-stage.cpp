#include "../common/init.h"

#include "util/message-queue.h"
#include "util/thread.h"
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
    Screen::init();
    Collector janitor;
    Graphics::Bitmap::setFakeGraphicsMode(640, 480);
    InputManager input;
    Util::Thread::initializeLock(&Global::messageLock);
    
    int die = 0;
    // for (int i = 0; i < 3; i++){
        try{
            const char * file = "mugen/stages/kfm.def";
            Global::debug(0, "test") << "Loading " << file << endl;
            Mugen::Stage stage(Storage::instance().find(Filesystem::RelativePath(file)));
            stage.load();
            Global::debug(0, "test") << "Success" << endl;
        } catch (const MugenException & e){
            Global::debug(0, "test") << "Exception: " << e.getReason() << endl;
            die = -1;
        } catch (const Filesystem::NotFound & e){
            Global::debug(0, "test") << "Exception: " << e.getTrace() << endl;
            die = -1;
        }
    // }
    Screen::finish();
    return die;
}

int main(int argc, char ** argv){
    return paintown_main(argc, argv);
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
