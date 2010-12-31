#include "util/token.h"
#include "optionfactory.h"
#include "util/load_exception.h"
#include "menu_option.h"
#include "options.h"
#include "util/debug.h"

using namespace std;

MenuOption * OptionFactory::getOption(const Token *token){
    const Token * tok;
    token->view() >> tok;
    if ( *tok == "menu" ){
        // Create a sub menu
	const Token * typeToken = tok->findToken("_/type");
	if (typeToken != NULL){
	    std::string type;
	    typeToken->view() >> type;
	    if (type == "tabbed"){
		return new OptionTabMenu(tok);
	    }
	}
        return new OptionMenu(tok);
    } else if (*tok == "tabmenu" ){
        // Create a tab menu
        return new OptionTabMenu(tok);
    } else if ( *tok == "key" ){
        // Reconfigure a given key
        return new OptionKey(tok);
    } else if ( *tok == "joystick" ){
        // Reconfigure a given joystick button
        return new OptionJoystick(tok);
    } else if (*tok == "choose-language"){
        return new OptionLanguage(tok);
    } else if (*tok == "return"){
        return new OptionReturn(tok);
    } else if (*tok == "continue"){
        return new OptionContinue(tok);
    } else if (*tok == "sound"){
        return new OptionSound(tok);
    } else if (*tok == "music"){
        return new OptionMusic(tok);
    } else if ( *tok == "adventure" ){
        // Adventure mode
        return new OptionAdventure(tok);
    } else if ( *tok == "adventure-cpu" ){
        return new OptionAdventureCpu(tok);
#ifdef HAVE_NETWORKING
    } else if ( *tok == "network-host" ){
        return new OptionNetworkHost(tok);
    } else if ( *tok == "network-join" ){
        return new OptionNetworkJoin(tok);
#endif
    } else if (*tok == "change-mod"){
        return new OptionChangeMod(tok);
    } else if (*tok == "screen-size"){
        return new OptionScreenSize(tok);
    } else if ( *tok == "npc" ){
        return new OptionNpcBuddies(tok);
    } else if (*tok == "play-mode"){
        return new OptionPlayMode(tok);
    } else if ( *tok == "credits" ){
        // Credits mode
        return new OptionCredits(tok);
    } else if ( *tok == "speed" ){
        // Speed
        return new OptionSpeed(tok);
    } else if ( *tok == "invincible" ){
        // Invincible
        return new OptionInvincible(tok);
    } else if ( *tok == "fullscreen" ){
        // Invincible
        return new OptionFullscreen(tok);
    } else if ( *tok == "quit" ){
        return new OptionQuit(tok);
    } else if ( *tok == "lives" ){
        // Invincible
        return new OptionLives(tok);
    } else if ( *tok == "font-select" ){
        // Invincible
        return new OptionSelectFont(tok);
    } else if ( *tok == "mugen" ){
        // Invincible
        return new OptionMugenMenu(tok);
    } else if ( *tok == "dummy" ){
        // Invincible
        return new OptionDummy(tok);
    } else {
        Global::debug(0) <<"Unhandled menu attribute: "<<endl;
        tok->print(" ");
    }
    return 0;
}
