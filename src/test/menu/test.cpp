#include "../common/init.h"
#include <iostream>
#include "util/token_exception.h"
#include "util/file-system.h"
#include "util/bitmap.h"
#include "paintown-engine/game/options.h"
#include "factory/collector.h"
#include "util/menu/menu.h"

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
        Menu::Menu menu(Storage::instance().find(Filesystem::RelativePath(path)), factory);
    }
}

int paintown_main(int argc, char ** argv){
    Collector collector;
    try{
        load("menu/main.txt");
        cout << "Success" << endl;
    } catch (const TokenException & e ){
        cout << "Error: " << e.getTrace() << endl;
    } catch (const LoadException & e){
        cout << "Error: " << e.getTrace() << endl;
    }
    Screen::fakeFinish();
    return 0;
}

int main(int argc, char ** argv){
    Screen::fakeInit();
    return paintown_main(argc, argv);
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
