#ifndef _paintown_game_h
#define _paintown_game_h

#include <vector>
#include <string>

#include "level/utils.h"

class Object;
class Character;
class Bitmap;
class World;

namespace Game{

/* play a single world */
bool playLevel( World & world, const std::vector< Object * > & players, int helpTime );
/* do the required setup to play a single level */
void realGame(const std::vector< Object * > & players, const Level::LevelInfo & levelInfo);

// const Level::LevelInfo selectLevelSet( const std::string & base );

void fadeOut( Bitmap & work, const std::string & message );

Object * selectPlayer( bool invincibile, const std::string & message, const Level::LevelInfo & info);

std::vector<Object *> versusSelect( bool invincible );

void playVersusMode( Character * player1, Character * player2, int round );

}

#endif
