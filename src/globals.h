#ifndef _global_things_h
#define _global_things_h

#include <ostream>

extern const int ALLIANCE_NONE;
extern const int ALLIANCE_PLAYER;
extern const int ALLIANCE_ENEMY;

extern const int MIN_RELATIVE_DISTANCE;

extern const char * NAME_FONT;

namespace Global{

extern const char * DEFAULT_FONT;

void setDebug( int i );
const int getDebug();
std::ostream & debug( int i ); 

void showTitleScreen();

}

#endif
