#include "util/graphics/bitmap.h"
#include "globals.h"
#include <streambuf>
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/message-queue.h"
#include "util/thread.h"
#include <iostream>
#include <sstream>

using namespace std;

static int major_version = 3;
static int minor_version = 6;
static int micro_version = 1;

/*
const int MIN_WORLD_Z = 160;
const int MAX_WORLD_Z = 232;
*/

int Global::getVersion(int major, int minor, int micro){
    return major * 1000 + minor * 100 + micro;
}

int Global::getVersion(){
    return getVersion(major_version, minor_version, micro_version);
}

string Global::getVersionString(){
    ostringstream str;
    str << major_version << "." << minor_version << "." << micro_version;
    return str.str();
}

void Global::showTitleScreen(){
    Graphics::Bitmap s(*Storage::instance().open(titleScreen()));
    s.BlitToScreen();
}

const Filesystem::AbsolutePath Global::titleScreen(){
    return Storage::instance().find(Filesystem::RelativePath("menu/paintown.png"));
}

namespace Global{

/* just some random number I picked out of thin air */
const unsigned int MagicId = 0x0dff2110;

/*
int getScreenWidth(){
    return Graphics::Bitmap::getScreenWidth();
}

int getScreenHeight(){
    return Graphics::Bitmap::getScreenHeight();
}
*/

}
