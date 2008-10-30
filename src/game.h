#ifndef _paintown_game_h
#define _paintown_game_h

#include <vector>
#include <string>

#include "util/load_exception.h"
#include "return_exception.h"

class Object;
class Character;
class Bitmap;

namespace Game{

void realGame( const std::vector< Object * > & players, const std::string & levelFile );
const string selectLevelSet( const std::string & base ) throw( ReturnException );
void fadeOut( Bitmap & work, const std::string & message );

Object * selectPlayer( bool invincibile, const std::string & message = "" ) throw( LoadException, ReturnException );

vector<Object *> versusSelect( bool invincible ) throw( LoadException, ReturnException );

void playVersusMode( Character * player1, Character * player2, int round ) throw( ReturnException );

}

#endif
