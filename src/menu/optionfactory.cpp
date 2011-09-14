#include "util/token.h"
#include "optionfactory.h"
#include "util/load_exception.h"
#include "util/gui/context-box.h"
#include "menu_option.h"
#include "options.h"
#include "util/debug.h"

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
    } else if (*tok == "mugen-motif"){
        return new OptionMugenMotif(parent, tok);
    } else if (*tok == "continue"){
        return new OptionContinue(parent, tok);
    } else if (*tok == "sound"){
        return new OptionSound(parent, tok);
    } else if (*tok == "music"){
        return new OptionMusic(parent, tok);
    } else if (*tok == "screen-size"){
        return new OptionScreenSize(parent, tok);
    } else if ( *tok == "npc" ){
        return new OptionNpcBuddies(parent, tok);
    } else if (*tok == "play-mode"){
        return new OptionPlayMode(parent, tok);
    } else if ( *tok == "credits" ){
        // Credits mode
        return new OptionCredits(parent, tok);
    } else if ( *tok == "speed" ){
        // Speed
        return new OptionSpeed(parent, tok);
    } else if ( *tok == "invincible" ){
        // Invincible
        return new OptionInvincible(parent, tok);
    } else if ( *tok == "fullscreen" ){
        // Full screen Selector
        return new OptionFullscreen(parent, tok);
    } else if ( *tok == "quit" ){
        return new OptionQuit(parent, tok);
    } else if ( *tok == "lives" ){
        // Live selector
        return new OptionLives(parent, tok);
    } else if ( *tok == "font-select" ){
        // Font Selector
        return new OptionSelectFont(parent, tok);
    } else if ( *tok == "mugen" ){
        // Mugen Option
        return new OptionMugenMenu(parent, tok);
    } else if ( *tok == "dummy" ){
        // Dummy Option
        return new OptionDummy(parent, tok);
    } else if ( *tok == "platformer" ){
        // Platformer
        return new OptionPlatformer(parent, tok);
    } else {
        Global::debug(0) <<"Unhandled menu attribute: "<<endl;
        tok->print(" ");
    }
    return 0;
}

}
