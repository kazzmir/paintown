#include "menu/option_adventure_cpu.h"
#include "util/token.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "game.h"
#include "globals.h"
#include "object/object.h"
#include "object/player.h"
#include "object/buddy_player.h"
#include "level/utils.h"
#include "util/funcs.h"

/* todo: replace keyboard with input-map */
#include "input/keyboard.h"

#include <sstream>

using namespace std;

OptionAdventureCpu::OptionAdventureCpu(Token *token) throw( LoadException ):
MenuOption(token, Event){
    if (*token != "adventure-cpu"){
        throw LoadException("Not an adventure");
    }

    readName(token);
}

OptionAdventureCpu::~OptionAdventureCpu(){
}

void OptionAdventureCpu::logic(){
}

void OptionAdventureCpu::run(bool &endGame){
    int max_buddies = MenuGlobals::getNpcBuddies();

    Keyboard key;
    Object * player = NULL;
    vector< Object * > buddies;
    try{
        //string level = Game::selectLevelSet( Util::getDataPath() + "/levels" );
        Level::LevelInfo info = MenuGlobals::doLevelMenu("/levels",parent);
        key.wait();
        player = Game::selectPlayer(MenuGlobals::getInvincible(), "Pick a player", info);
        player->setObjectId(-1);
        ((Player *)player)->setLives( MenuGlobals::getLives() );
        vector< Object * > players;
        players.push_back( player );

        for ( int i = 0; i < max_buddies; i++ ){
            ostringstream out;
            out << "Pick buddy " << i+1;
            Object * b = Game::selectPlayer(false, out.str(), info);
            buddies.push_back( b );
            Object * buddy = new BuddyPlayer( (Character *) player, *(Character *) b );
            /* buddies start at -2 and go down */
            buddy->setObjectId(-(i + 2));
            buddies.push_back( buddy );
            players.push_back( buddy );
        }
        Game::realGame(players, info);
    } catch ( const LoadException & le ){
        Global::debug( 0 ) << "Could not load player: " << le.getReason() << endl;
    } catch ( const ReturnException & r ){
        key.wait();
    }
    if ( player != NULL ){
        delete player;
    }
    for ( vector< Object * >::iterator it = buddies.begin(); it != buddies.end(); it++ ){
        delete *it;
    }
}
