#include "util/bitmap.h"
#include "menu/option_adventure.h"
#include "menu/menu_global.h"

/* todo: replace keyboard with input-map */
#include "input/keyboard.h"
#include "util/load_exception.h"
#include "util/token.h"
#include "util/funcs.h"
#include "level/utils.h"
#include "object/player.h"
#include "object/object.h"
#include "game.h"
#include "globals.h"

#include "menu/menu.h"

#include <iostream>

using namespace std;

OptionAdventure::OptionAdventure(Token *token) throw( LoadException ):
MenuOption(token, Event){
    if ( *token != "adventure" ){
        throw LoadException("Not an adventure");
    }

    LanguageString name;
    while (token->hasTokens()){
        try{
            Token * tok;
            *token >> tok;
            string temp;
            
            if (tok->match("name", temp)){
                name.add(temp, LanguageString::defaultLanguage());
            }

            Token::Matcher matcher = tok->getMatcher("name/language");
            string language, words;
            while (matcher.match(language, words)){
                name.add(words, language);
            }

            /*
            if ( *tok == "name" ){
                // Create an image and push it back on to vector
                std::string temp;
                *tok >> temp;
                this->setText(temp);
            } else {
                Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
            */
        } catch ( const TokenException & ex ) {
            // delete current;
            string m( "Menu parse error: " );
            m += ex.getReason();
            throw LoadException( m );
        } 
    }
    this->setText(name);

    if (getText().empty()){
        throw LoadException("No name set, this option should have a name!");
    }
}

OptionAdventure::~OptionAdventure(){
	// Nothing
}

void OptionAdventure::logic(){
}

void OptionAdventure::run(bool &endGame){
    Keyboard key;
    Object * player = NULL;
    try{
        //string level = Game::selectLevelSet( Util::getDataPath() + "/levels" );
        Level::LevelInfo info = MenuGlobals::doLevelMenu("/levels",parent);

        /*
        if (level.empty()){
            Global::debug(0) << "*bug* Level name is empty?" << endl;
            throw an error or something
            return;
        */
        key.wait();

        player = Game::selectPlayer(MenuGlobals::getInvincible(), "Pick a player", info);
        player->setObjectId(-1);
        ((Player *)player)->setLives( MenuGlobals::getLives() );
        vector< Object * > players;
        players.push_back( player );
        Game::realGame(players, info);
    } catch ( const LoadException & le ){
        Global::debug( 0 ) << "Error while loading: " << le.getReason() << endl;
    } catch ( const ReturnException & r ){
        /* Game::selectPlayer can throw ReturnException, he will wait
         * for the keypress to be released, so we don't have to do it
         */
        // key.wait();
    }

    /* player will be null if an exception occurred before selectPlayer was called */
    if ( player != NULL ){
        delete player;
    }
}
