#include "globals.h"
#include <streambuf>
#include "util/funcs.h"
#include "util/bitmap.h"
#include "util/file-system.h"
#include <iostream>
#include <sstream>

using namespace std;

static int major_version = 3;
static int minor_version = 2;

/*
const int MIN_WORLD_Z = 160;
const int MAX_WORLD_Z = 232;
*/

const int ALLIANCE_NONE = 0;
const int ALLIANCE_PLAYER = 1;
const int ALLIANCE_ENEMY = 2;
const int ALLIANCE_FREE_FOR_ALL = 1000;

const int MIN_RELATIVE_DISTANCE = 10;

const char * NAME_FONT = "CENTURYGOTHIC_PCX";

// static bool global_debug = false;
static int global_debug_level = 0;

const char * Global::DEFAULT_FONT = "/fonts/arial.ttf";

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

ostream & Global::debug( int i ){
	if ( global_debug_level >= i ){
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

const int Global::getVersion(){
    return major_version * 100 + minor_version;
}

string Global::getVersionString(){
    ostringstream str;
    str << major_version << "." << minor_version;
    return str.str();
}

const int Global::getDebug(){
	return global_debug_level;
}

void Global::showTitleScreen(){
    Bitmap s( Global::titleScreen() );
    s.BlitToScreen();
}

const std::string Global::titleScreen(){
	return Filesystem::find("/menu/paintown.png");
}
