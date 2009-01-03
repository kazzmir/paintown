/*
#ifdef _WIN32
#define BITMAP dummyBITMAP
#include <windows.h>
#undef BITMAP
#endif

#include "menu/option_credits.h"
#include "menu/menu_global.h"
#include "util/token.h"
#include "globals.h"

#include "init.h"

#include "factory/collector.h"
#include "factory/object_factory.h"
#include "factory/heart_factory.h"
#include "factory/font_render.h"
#include "fonts.h"
#include "globals.h"
#include "configuration.h"
#include "level/scene.h"
#include "return_exception.h"
#include "loading.h"
#include "music.h"
#include "object/character.h"
#include "object/effect.h"
#include "object/enemy.h"
#include "object/object.h"
#include "object/object_attack.h"
#include "object/player.h"
#include "object/versus_player.h"
#include "object/versus_enemy.h"
#include "util/bitmap.h"
#include "util/funcs.h"
#include "util/load_exception.h"
#include "util/token.h"
#include "util/token_exception.h"
#include "util/font.h"
#include "util/tokenreader.h"
#include "util/timedifference.h"
#include "select_player.h"
#include "world.h"
#include "versus_world.h"
#include "menu/menu.h"

#include <pthread.h>

#include <iostream>
*/

#include "util/bitmap.h"
#include "menu/option_credits.h"
#include "menu/menu_global.h"
#include "globals.h"
#include "init.h"
#include "util/funcs.h"
#include "util/keyboard.h"
#include "util/token.h"
#include "util/token_exception.h"
#include "util/font.h"

using namespace std;

static const char * DEFAULT_FONT = "/fonts/arial.ttf";

OptionCredits::OptionCredits( Token * token ) throw( LoadException ):
MenuOption(token, Event),
background(0),
music(""),
color(Bitmap::makeColor(255,255,255)),
title(Bitmap::makeColor(0,255,255)){
	/* Always */
	credits.push_back("Paintown");
	credits.push_back("");
	credits.push_back("Programming");
	credits.push_back("Jon Rafkind");
	credits.push_back("");
	credits.push_back("Contributions");
	credits.push_back("Miguel Gavidia");
	credits.push_back("");
	credits.push_back("Level design");
	credits.push_back("Jon Rafkind");
	credits.push_back("Miguel Gavidia");
	credits.push_back("");
	credits.push_back("Music");
	credits.push_back("aqua.s3m - Purple Motion");
	credits.push_back("c_heaven.xm - One Man Project");
	credits.push_back("elw-sick.xm - elwood");
	credits.push_back("experience.xm - elwood");
	credits.push_back("fall.xm - elwood");
	credits.push_back("kajahtaa.xm - cube");
	credits.push_back("kilimanz.mod - ???");
	credits.push_back("SM_TechTown.it - SaMPLeMaSTeR");
	credits.push_back("");
	credits.push_back("Email: jon@rafkind.com");
	credits.push_back("");
	
	if ( *token != "credits" )
		throw LoadException("Not a credit menu");
	
	while ( token->hasTokens() ){
		try{
			Token * tok;
			*token >> tok;
			if ( *tok == "name" ){
				/* Create an image and push it back on to vector */
				std::string temp;
				*tok >> temp;
				this->setText(temp);
			} else if ( *tok == "music" ) {
				/* Set music for credits */
				*tok >> music;
			} else if ( *tok == "background" ) {
				/* Create an image and push it back on to vector */
				std::string temp;
				*tok >> temp;
				if ( background ){
					delete background;
				}
				background = new Bitmap(Util::getDataPath() +temp);
				if ( background->getError() ){
					delete background;
					background = NULL;
				}
			} else if ( *tok == "additional" ) {
				std::string str;
				while ( tok->hasTokens() ){
					*tok >> str;
					credits.push_back(str);
				}
			} else if ( *tok == "titlecolor" ) {
				int r,b,g;
				*tok >> r >> g >> b;
				title = Bitmap::makeColor( r, b, g );
			} else if ( *tok == "color" ) {
				int r,b,g;
				*tok >> r >> g >> b;
				color = Bitmap::makeColor( r, b, g );
			} else {
				Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
				tok->print(" ");
			}
		} catch ( const TokenException & ex ) {
			string m( "Menu parse error: " );
			m += ex.getReason();
			throw LoadException( m );
		} catch ( const LoadException & ex ) {
			throw ex;
		}
	}
	
	if ( getText().empty() ){
		throw LoadException("No name set, this option should have a name!");
	}
}

OptionCredits::~OptionCredits(){
	if ( background ){
		delete background;
	}
}

void OptionCredits::logic(){
}

void OptionCredits::draw(Bitmap *work){
}

void OptionCredits::run( bool &endGame ){
	Keyboard key;

	const int maxCredits = credits.size();

	Global::speed_counter = 0;
	int min_y = GFX_Y;
	const Font & font = Font::getFont( Util::getDataPath() + DEFAULT_FONT, 20, 20 );
	Bitmap tmp( GFX_X, GFX_Y );
	if ( ! music.empty() ){
		MenuGlobals::setMusic( music );
	}

	while ( ! key[ Keyboard::Key_ESC ] ){

		key.poll();

		bool draw = false;
		if ( Global::speed_counter / 2 > 0 ){
			double think = Global::speed_counter / 2 * Global::LOGIC_MULTIPLIER;
			draw = true;

			while ( think >= 1.0 ){
				think -= 1;
				min_y -= 1;
				if ( min_y < -(int)(maxCredits * font.getHeight() * 1.1) ){
					min_y = GFX_Y;
				}
			}

			Global::speed_counter = 0;
		}

		if ( draw ){
			if ( background ){
				background->Blit( tmp );
			} else {
				tmp.fill( Bitmap::makeColor(0,0,0) );
			}

			int y = min_y;
			vector<std::string>::iterator b = credits.begin();
			vector<std::string>::iterator e = credits.end();
			bool isTitle = true;
			for ( /**/ ; b != e; b++ ){
				if ( isTitle ){
					font.printf( 100, y, title, tmp, (*b), 0 );
					isTitle = false;
				} else {
					font.printf( 100, y, color, tmp, (*b), 0 );
				}
				y += font.getHeight() + 2;

				if ( (*b).empty() ){
					isTitle = true;
				}
			}

			tmp.BlitToScreen();
		} else {
			Util::rest( 1 );
		}
	}
	key.wait();
}
