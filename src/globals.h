#ifndef _paintown_global_things_h
#define _paintown_global_things_h

#include <ostream>
#include <string>
#include "util/file-system.h"

class Bitmap;
class MessageQueue;

namespace Global{
	
void showTitleScreen();
const Filesystem::AbsolutePath titleScreen();

/* the real, physical screen size */
/*
int getScreenWidth();
int getScreenHeight();
*/

}

#endif
