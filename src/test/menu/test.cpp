#include <iostream>
#include "util/init.h"
#include "util/token_exception.h"
#include "util/file-system.h"
#include "util/graphics/bitmap.h"
#include "util/exceptions/load_exception.h"
#include "paintown-engine/game/options.h"
#include "factory/collector.h"
#include "util/menu/menu.h"
#include "util/configuration.h"

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

    Util::Parameter<Util::ReferenceCount<Path::RelativePath> > defaultFont(Font::defaultFont, Util::ReferenceCount<Path::RelativePath>(new Path::RelativePath("fonts/arial.ttf")));

    Util::Parameter<Util::ReferenceCount<Menu::FontInfo> > defaultMenuFont(Menu::menuFontParameter, Util::ReferenceCount<Menu::FontInfo>(new Menu::RelativeFontInfo(*defaultFont.current(), Configuration::getMenuFontWidth(), Configuration::getMenuFontHeight())));

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
    Global::InitConditions conditions;
    conditions.graphics = Global::InitConditions::Disabled;
    Global::init(conditions);
    return paintown_main(argc, argv);
}
#ifdef USE_ALLEGRO
END_OF_MAIN()
#endif
