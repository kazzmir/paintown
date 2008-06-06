#include "menu/option_adventure.h"
#include "menu/menu_global.h"
#include "util/keyboard.h"
#include "util/load_exception.h"
#include "util/token.h"
#include "util/funcs.h"
#include "object/player.h"
#include "object/object.h"
#include "game.h"
#include "globals.h"

#include "menu/menu.h"

#include "gui/lineedit.h"
#include "gui/keyinput_manager.h"

#include <iostream>

using namespace std;

static LineEdit *lineedit =0;

OptionAdventure::OptionAdventure(Token *token) throw( LoadException ):
MenuOption(event){
	if ( *token != "adventure" ){
		throw LoadException("Not an adventure");
	}
	
	while ( token->hasTokens() ){
		try{
			Token * tok;
			*token >> tok;
			if ( *tok == "name" ){
				// Create an image and push it back on to vector
				std::string temp;
				*tok >> temp;
				this->setText(temp);
			} else {
				Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
				tok->print(" ");
			}
		} catch ( const TokenException & ex ) {
			// delete current;
			string m( "Menu parse error: " );
			m += ex.getReason();
			throw LoadException( m );
		} 
	}
	
	if ( getText().empty() ){
		throw LoadException("No name set, this option should have a name!");
	}
	
	/*
	if(!lineedit)lineedit = new LineEdit();
	lineedit->position.x = 20;
	lineedit->position.y = 20;
	lineedit->position.width = 400;
	lineedit->position.height = 30;
	lineedit->position.radius = 5;
	
	lineedit->position.body = Bitmap::makeColor( 255, 255, 255 );
	lineedit->position.border = Bitmap::makeColor( 255, 255, 0 );
	lineedit->setHorizontalAlign(LineEdit::T_Left);
	
	lineedit->setText("Hello mongrel, type something");
	
	keyInputManager::pressed.connect(lineedit,&LineEdit::keyPress);
	
	lineedit->setFocused(true);
	*/
}

OptionAdventure::~OptionAdventure(){
	// Nothing
	if(lineedit)delete lineedit;
}

void OptionAdventure::logic(){
	if ( lineedit ){
		lineedit->setFont(Menu::getFont());
		lineedit->logic();
	}
}

void OptionAdventure::draw(Bitmap *work){
	if ( lineedit ){
		lineedit->render(work);
	}
}

void OptionAdventure::run(bool &endGame){
	Keyboard key;
	Object * player = NULL;
	try{
		string level = Game::selectLevelSet( Util::getDataPath() + "/levels" );
		key.wait();
		
		player = Game::selectPlayer( MenuGlobals::getInvincible() );
		((Player *)player)->setLives( MenuGlobals::getLives() );
		vector< Object * > players;
		players.push_back( player );
		Game::realGame( players, level );
	} catch ( const LoadException & le ){
		Global::debug( 0 ) << "Error while loading: " << le.getReason() << endl;
	} catch ( const ReturnException & r ){
		key.wait();
	}

	if ( player != NULL ){
		delete player;
	}
}
