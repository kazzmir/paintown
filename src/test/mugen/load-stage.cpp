#include <allegro.h>

#include "util/file-system.h"
#include "mugen/mugen_stage.h"
#include "mugen/mugen_exception.h"
#include "globals.h"
#include "input/input-manager.h"

#include <iostream>

using namespace std;

int main(int argc, char ** argv){
    install_allegro(SYSTEM_NONE, &errno, atexit);
    set_color_depth(16);
    set_color_conversion(COLORCONV_NONE);
    InputManager input;

    // for (int i = 0; i < 3; i++){
        try{
            MugenStage stage(Filesystem::find("mugen/stages/kfm.def"));
            stage.load();
        } catch (const MugenException & e){
            Global::debug(0) << "Exception: " << e.getReason() << endl;
        } catch (const Filesystem::NotFound & e){
            Global::debug(0) << "Exception: " << e.getReason() << endl;
        }
    // }
}
