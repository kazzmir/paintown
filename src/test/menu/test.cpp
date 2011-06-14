#ifdef USE_ALLEGRO
#include <allegro.h>
#endif
#ifdef USE_SDL
#include <SDL/SDL.h>
#endif
#ifdef USE_ALLEGRO5
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#endif
#include <iostream>
#include "util/token_exception.h"
#include "util/file-system.h"
#include "util/loadpng/loadpng.h"
#include "util/bitmap.h"
#include "paintown-engine/game/options.h"
#include "factory/collector.h"
#include "menu/menu.h"

using namespace std;

class MainMenuOptionFactory: public Menu::OptionFactory {
public:
    MainMenuOptionFactory(){
    }

    Paintown::OptionFactory paintownFactory;
    
    virtual MenuOption * getOption(const Gui::ContextBox & parent, const Token *token) const {
        MenuOption * get = paintownFactory.getOption(parent, token);
        if (get != NULL){
            return get;
        }
        return Menu::OptionFactory::getOption(parent, token);
    }
};

void load(const char * path){
    for (int i = 0; i < 1; i++){
        MainMenuOptionFactory factory;
        Menu::Menu menu(Filesystem::find(Filesystem::RelativePath(path)), factory);
    }
}

int paintown_main(int argc, char ** argv){
    Collector collector;
    Filesystem::initialize();
    try{
        load("menu/main.txt");
        cout << "Success" << endl;
    } catch (const TokenException & e ){
        cout << "Error: " << e.getTrace() << endl;
    } catch (const LoadException & e){
        cout << "Error: " << e.getTrace() << endl;
    }
    return 0;
}

int main(int argc, char ** argv){
#ifdef USE_ALLEGRO
    install_allegro(SYSTEM_NONE, &errno, atexit);
    set_color_depth(16);
    set_color_conversion(COLORCONV_NONE);
    Graphics::Bitmap::setFakeGraphicsMode(640, 480);
    loadpng_init();
#elif USE_SDL
    SDL_Init(SDL_INIT_VIDEO);
    Graphics::Bitmap::setFakeGraphicsMode(640, 480);
#elif USE_ALLEGRO5
    al_init();
    al_init_image_addon();
    al_init_primitives_addon();
#endif
    return paintown_main(argc, argv);
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
