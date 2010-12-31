#ifndef _paintown_global_things_h
#define _paintown_global_things_h

#include <ostream>
#include <string>
#include "util/file-system.h"

class Bitmap;
class MessageQueue;

/* put this in an enum */
extern const int ALLIANCE_NONE;
extern const int ALLIANCE_PLAYER;
extern const int ALLIANCE_ENEMY;
extern const int ALLIANCE_FREE_FOR_ALL;

/* who uses this? it probably doesnt belong here */
extern const int MIN_RELATIVE_DISTANCE;

// extern const char * NAME_FONT;

// #include "debug.h"

namespace Global{
	
extern const unsigned int MagicId;

extern const Filesystem::RelativePath DEFAULT_FONT;

/* The current version */
int getVersion();
/* The current version as a string */
std::string getVersionString();

/* Turn some major.minor.micro into a single version number for comparisons */
int getVersion(int major, int minor, int micro);

bool shutdown();

void showTitleScreen();
const Filesystem::AbsolutePath titleScreen();

void registerInfo(MessageQueue *);
void unregisterInfo(MessageQueue *);
void info(const std::string & str);

/* the real, physical screen size */
int getScreenWidth();
int getScreenHeight();

}

#endif
