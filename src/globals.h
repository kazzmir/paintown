#ifndef _global_things_h
#define _global_things_h

static bool global_debug;

/*
extern const int MIN_WORLD_Z;
extern const int MAX_WORLD_Z;
*/

extern const int ALLIANCE_NONE;
extern const int ALLIANCE_PLAYER;
extern const int ALLIANCE_ENEMY;

extern const int MIN_RELATIVE_DISTANCE;

extern const char * NAME_FONT;

inline bool globalDebug(){
	return global_debug;
}

inline void disableDebug(){
	global_debug = false;
}

inline void enableDebug(){
	global_debug = true;
}

#endif
