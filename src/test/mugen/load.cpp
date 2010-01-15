#include <allegro.h>

#include "mugen/character.h"
#include "mugen/mugen_exception.h"

int main(){
    install_allegro(SYSTEM_NONE, &errno, atexit);
    set_color_depth(16);
    set_color_conversion(COLORCONV_NONE);

    try{
        Mugen::Character kfm("kfm");
        kfm.load();
    } catch (const MugenException & e){
    }
}
