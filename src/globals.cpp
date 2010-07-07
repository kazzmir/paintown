#include "util/bitmap.h"
#include "globals.h"
#include <streambuf>
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/message-queue.h"
#include "loading.h"
#include <iostream>
#include <sstream>

using namespace std;

static int major_version = 3;
static int minor_version = 4;
static int micro_version = 0;

/*
const int MIN_WORLD_Z = 160;
const int MAX_WORLD_Z = 232;
*/

const int ALLIANCE_NONE = 0;
const int ALLIANCE_PLAYER = 1;
const int ALLIANCE_ENEMY = 2;
const int ALLIANCE_FREE_FOR_ALL = 1000;

const int MIN_RELATIVE_DISTANCE = 10;

// const char * NAME_FONT = "CENTURYGOTHIC_PCX";

// static bool global_debug = false;
static int global_debug_level = 0;

const Filesystem::RelativePath Global::DEFAULT_FONT = Filesystem::RelativePath("fonts/arial.ttf");

class nullstreambuf_t: public std::streambuf {
public:
	nullstreambuf_t():std::streambuf(){
	}
};

static nullstreambuf_t nullstreambuf;

class nullcout_t: public std::ostream {
public:
	nullcout_t():std::ostream(&nullstreambuf){
	}
};

static nullcout_t nullcout;

ostream & Global::debug(int i, const string & context){
    if ( global_debug_level >= i ){
        std::cout << "[" << i << ":" << context << "] ";
        return std::cout;
    }
    return nullcout;
}

void Global::setDebug( int i ){
	global_debug_level = i;
}

namespace Global{
    int do_shutdown = 0;
}

bool Global::shutdown(){
    return do_shutdown > 0;
}

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

int Global::getDebug(){
    return global_debug_level;
}

void Global::showTitleScreen(){
    Bitmap s(Global::titleScreen().path());
    s.BlitToScreen();
}

const Filesystem::AbsolutePath Global::titleScreen(){
    return Filesystem::find(Filesystem::RelativePath("menu/paintown.png"));
}

namespace Global{


/* just some random number I picked out of thin air */
const unsigned int MagicId = 0x0dff2110;

/* should support infinite queues eventually */
static MessageQueue * current = NULL;
void registerInfo(MessageQueue * queue){
    current = queue;
}

void unregisterInfo(MessageQueue * queue){
    if (current == queue){
        current = NULL;
    }
}

void info(const std::string & str){
    if (current != NULL){
        current->add(str);
    }
}

int getScreenWidth(){
    return Bitmap::getScreenWidth();
}

int getScreenHeight(){
    return Bitmap::getScreenHeight();
}


}
