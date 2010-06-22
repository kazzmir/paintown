#include <allegro.h>
#include <iostream>
#include "util/token_exception.h"
#include "util/file-system.h"
#include "loadpng/loadpng.h"
#include "factory/collector.h"
#include "menu/menu.h"

using namespace std;

void load(const char * path){
    Menu menu;
    menu.load(Filesystem::find(Filesystem::RelativePath(path)));
}

int paintown_main(int argc, char ** argv){
    Collector collector;
    try{
        load("menu/main.txt");
        cout << "Success" << endl;
    } catch (const TokenException & e ){
        cout << "Error: " << e.getReason() << endl;
    } catch (const LoadException & e){
        cout << "Error: " << e.getReason() << endl;
    }
    return 0;
}

int main(int argc, char ** argv){
    install_allegro(SYSTEM_NONE, &errno, atexit);
    set_color_depth(16);
    set_color_conversion(COLORCONV_NONE);
    loadpng_init();
    return paintown_main(argc, argv);
}
END_OF_MAIN()
