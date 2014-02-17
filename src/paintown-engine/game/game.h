#ifndef _paintown_game_h
#define _paintown_game_h

#include <vector>
#include <string>

#include "../level/utils.h"
#include "util/file-system.h"

namespace Graphics{
class Bitmap;
}
class World;

namespace Util{
    template<class X>
    class Future;
}

namespace Paintown{
    class Object;
    class Character;
    class Player;
}

namespace Game{

extern const int Ticks;

/* play a single world */
bool playLevel( World & world, const std::vector< Paintown::Object * > & players);
/* do the required setup to play a single level */
void realGame(const std::vector<Util::Future<Paintown::Object *>* > & players, const Level::LevelInfo & levelInfo);

/* run a multi-player local game */
void realGameLocal(const std::vector<Util::Future<Paintown::Object*> * > & futurePlayers, const Level::LevelInfo & levelInfo);

// const Level::LevelInfo selectLevelSet( const std::string & base );

void fadeOut( const Graphics::Bitmap & work, const std::string & message );

// Filesystem::AbsolutePath selectPlayer(const std::string & message, const Level::LevelInfo & info, int & remap);

std::vector<Paintown::Object *> versusSelect( bool invincible );

// void playVersusMode( Paintown::Character * player1, Paintown::Character * player2, int round );

}

#endif
