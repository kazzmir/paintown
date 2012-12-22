#ifndef _paintown_global_things_h
#define _paintown_global_things_h

#include <ostream>
#include <string>
#include "util/file-system.h"

class Bitmap;
class MessageQueue;

namespace Global{
	
extern const unsigned int MagicId;

/* The current version */
int getVersion();
/* The current version as a string */
std::string getVersionString();

/* Turn some major.minor.micro into a single version number for comparisons */
int getVersion(int major, int minor, int micro);

void showTitleScreen();
const Filesystem::AbsolutePath titleScreen();

/* the real, physical screen size */
/*
int getScreenWidth();
int getScreenHeight();
*/

}

#endif
