#include "util/token.h"
#include "menu/optionfactory.h"
#include "menu/menu_option.h"
#include "menu/option_adventure.h"
#include "menu/option_adventure_cpu.h"
#include "menu/option_credits.h"
#include "menu/option_invincible.h"
#include "menu/option_key.h"
#include "menu/option_menu.h"
#include "menu/option_playmode.h"
#include "menu/option_network_host.h"
#include "menu/option_network_join.h"
#include "menu/option_versus.h"
#include "menu/option_select_font.h"
#include "menu/option_screen_size.h"
#include "menu/option_speed.h"
#include "menu/option_fullscreen.h"
#include "menu/option_lives.h"
#include "menu/option_npc_buddies.h"
#include "menu/option_quit.h"
#include "menu/option_dummy.h"
#include "globals.h"

#include "menu/option_mugen_menu.h"

MenuOption *getOption(Token *token){
	Token * tok;
	*token >> tok;
	if ( *tok == "menu" ){
		// Create a sub menu
		return new OptionMenu(tok);
	} else if ( *tok == "key" ){
		// Reconfigure a given key
		return new OptionKey(tok);
	} else if ( *tok == "adventure" ){
		// Adventure mode
		return new OptionAdventure(tok);
	} else if ( *tok == "adventure-cpu" ){
		return new OptionAdventureCpu(tok);
	} else if ( *tok == "versus" ){
		// Versus mode
		return new OptionVersus(tok);
	} else if ( *tok == "network-host" ){
		return new OptionNetworkHost(tok);
	} else if ( *tok == "network-join" ){
		return new OptionNetworkJoin(tok);
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
		Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
		tok->print(" ");
	}
	return 0;
}
