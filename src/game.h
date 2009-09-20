#ifndef _paintown_game_h
#define _paintown_game_h

#include <vector>
#include <string>

#include "util/load_exception.h"
#include "level/utils.h"
#include "return_exception.h"
#include "util/file-system.h"

class Object;
class Character;
class Bitmap;
class World;

namespace Game{

/* play a single world */
bool playLevel( World & world, const std::vector< Object * > & players, int helpTime );
/* do the required setup to play a single level */
void realGame(const std::vector< Object * > & players, const Level::LevelInfo & levelInfo);

const Level::LevelInfo selectLevelSet( const std::string & base ) throw (LoadException, Filesystem::NotFound);

void fadeOut( Bitmap & work, const std::string & message );

Object * selectPlayer( bool invincibile, const std::string & message, const Level::LevelInfo & info) throw( LoadException, ReturnException );

std::vector<Object *> versusSelect( bool invincible ) throw( LoadException, ReturnException );

void playVersusMode( Character * player1, Character * player2, int round ) throw( ReturnException );

}

#endif
