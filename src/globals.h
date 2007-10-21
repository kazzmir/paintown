#ifndef _global_things_h
#define _global_things_h

#include <ostream>

/*
extern const int MIN_WORLD_Z;
extern const int MAX_WORLD_Z;
*/

extern const int ALLIANCE_NONE;
extern const int ALLIANCE_PLAYER;
extern const int ALLIANCE_ENEMY;

extern const int MIN_RELATIVE_DISTANCE;

extern const char * NAME_FONT;

namespace Global{

bool globalDebug();
void invertDebug();
void disableDebug();
void enableDebug();

void setDebug( int i );

std::ostream & debug( int i ); 

}

#endif
