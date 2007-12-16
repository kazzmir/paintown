#include "menu/optionfactory.h"
#include "util/token.h"
#include "menu/menu_option.h"
#include "menu/option_adventure.h"
#include "menu/option_background.h"
#include "menu/option_credits.h"
#include "menu/option_invincible.h"
#include "menu/option_key.h"
#include "menu/option_menu.h"
#include "menu/option_versus.h"
#include "menu/option_speed.h"
#include "menu/option_fullscreen.h"
#include "menu/option_lives.h"
#include "globals.h"

MenuOption *getOption(Token *token)
{
	Token * tok;
	*token >> tok;
	if ( *tok == "menu" )
	{
		// Create a sub menu
		return new OptionMenu(tok);
	}
	else if ( *tok == "key" )
	{
		// Reconfigure a given key
		return new OptionKey(tok);
	}
	else if ( *tok == "adventure" )
	{
		// Adventure mode
		return new OptionAdventure(tok);
	}
	else if ( *tok == "versus" )
	{
		// Versus mode
		return new OptionVersus(tok);
	}
	else if ( *tok == "credits" )
	{
		// Credits mode
		return new OptionCredits(tok);
	}
	else if ( *tok == "speed" )
	{
		// Speed
		return new OptionSpeed(tok);
	}
	else if ( *tok == "invincible" )
	{
		// Invincible
		return new OptionInvincible(tok);
	}
	else if ( *tok == "fullscreen" )
	{
		// Invincible
		return new OptionFullscreen(tok);
	}
	else if ( *tok == "lives" )
	{
		// Invincible
		return new OptionLives(tok);
	}
	else 
	{
		Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
		tok->print(" ");
	}
	return 0;
}

