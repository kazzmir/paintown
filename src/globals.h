#ifndef _global_things_h
#define _global_things_h

#include <ostream>
#include <string>

class Bitmap;
class MessageQueue;

extern const int ALLIANCE_NONE;
extern const int ALLIANCE_PLAYER;
extern const int ALLIANCE_ENEMY;
extern const int ALLIANCE_FREE_FOR_ALL;

extern const int MIN_RELATIVE_DISTANCE;

extern const char * NAME_FONT;

namespace Global{

extern const char * DEFAULT_FONT;

const int getVersion();
std::string getVersionString();

bool shutdown();

void setDebug( int i );
const int getDebug();
std::ostream & debug( int i );

void showTitleScreen();
const std::string titleScreen();

void registerInfo(MessageQueue *);
void unregisterInfo(MessageQueue *);
void info(const std::string & str);

}

#endif
