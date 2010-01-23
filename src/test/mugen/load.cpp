#include <allegro.h>

#include <iostream>
#include "util/file-system.h"
#include "mugen/character.h"
#include "mugen/mugen_exception.h"

using namespace std;

int main(){
    install_allegro(SYSTEM_NONE, &errno, atexit);
    set_color_depth(16);
    set_color_conversion(COLORCONV_NONE);

    // for (int i = 0; i < 3; i++){
        try{
            Mugen::Character kfm(Filesystem::find("mugen/chars/kfm/kfm.def"));
            kfm.load();
            Global::debug(0, "test") << "Success" << endl;
        } catch (const MugenException & e){
            Global::debug(0, "test") << "Test failure!: " << e.getReason() << endl;
        }
    // }
}
