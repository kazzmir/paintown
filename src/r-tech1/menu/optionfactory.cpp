#include "r-tech1/token.h"
#include "r-tech1/menu/optionfactory.h"
#include "r-tech1/gui/context-box.h"
#include "r-tech1/menu/menu_option.h"
#include "r-tech1/menu/options.h"
#include "r-tech1/debug.h"

using namespace std;

namespace Menu{

OptionFactory::OptionFactory(){
}

OptionFactory::~OptionFactory(){
}

MenuOption * OptionFactory::getOption(const Gui::ContextBox & parent, const Token *token) const {
    const Token * tok;
    token->view() >> tok;
    if (*tok == "menu"){
        // Create a sub menu
        const Token * typeToken = tok->findToken("_/type");
        if (typeToken != NULL){
            std::string type;
            typeToken->view() >> type;
            if (type == "tabbed"){
                return new OptionTabMenu(parent, tok, *this);
            }
        }
        return new OptionMenu(parent, tok, *this);
    } else if (*tok == "tabmenu" ){
        // Create a tab menu
        return new OptionTabMenu(parent, tok, *this);
    } else if ( *tok == "key" ){
        // Reconfigure a given key
        return new OptionKey(parent, tok);
    } else if ( *tok == "joystick" ){
        // Reconfigure a given joystick button
        return new OptionJoystick(parent, tok);
    } else if (*tok == "choose-language"){
        return new OptionLanguage(parent, tok);
    } else if (*tok == "return"){
        return new OptionReturn(parent, tok);
    } else if (*tok == "continue"){
        return new OptionContinue(parent, tok);
    } else if (*tok == "sound"){
        return new OptionSound(parent, tok);
    } else if (*tok == "music"){
        return new OptionMusic(parent, tok);
    } else if (*tok == "screen-size"){
        return new OptionScreenSize(parent, tok);
    } else if (*tok == "quality-filter"){
        return new OptionQualityFilter(parent, tok);
    } else if (*tok == "fps"){
        return new OptionFps(parent, tok);
    } else if ( *tok == "credits" ){
        // Credits mode
        return new OptionCredits(parent, tok);
    } else if ( *tok == "speed" ){
        // Speed
        return new OptionSpeed(parent, tok);
    } else if ( *tok == "fullscreen" ){
        // Full screen Selector
        return new OptionFullscreen(parent, tok);
    } else if ( *tok == "quit" ){
        return new OptionQuit(parent, tok);
    } else if ( *tok == "font-select" ){
        // Font Selector
        return new OptionSelectFont(parent, tok);
    } else if ( *tok == "dummy" ){
        // Dummy Option
        return new OptionDummy(parent, tok);
    } else if (*tok == "network-join"){
        DebugLog << "Warning: ignoring 'network-join' option" << endl;
        // return new OptionDummy(parent, tok);
        return nullptr;
    } else if (*tok == "network-host"){
        DebugLog << "Warning: ignoring 'network-host' option" << endl;
        // return new OptionDummy(parent, tok);
        return nullptr;
    } else {
        Global::debug(0) << "Unhandled menu option: "<<endl;
        tok->print(" ");
    }
    return NULL;
}

}
