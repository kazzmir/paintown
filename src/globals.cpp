#include "globals.h"
#include <streambuf>
#include <iostream>

using namespace std;

/*
const int MIN_WORLD_Z = 160;
const int MAX_WORLD_Z = 232;
*/

const int ALLIANCE_NONE = 0;
const int ALLIANCE_PLAYER = 1;
const int ALLIANCE_ENEMY = 2;

const int MIN_RELATIVE_DISTANCE = 10;

const char * NAME_FONT = "CENTURYGOTHIC_PCX";

static bool global_debug = false;
static int global_debug_level = 0;

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

bool Global::globalDebug(){
	return global_debug;
}

void Global::invertDebug(){
	global_debug = ! global_debug;
	cout << "Debug set to " << global_debug << endl;
}

void Global::disableDebug(){
	global_debug = false;
	cout << "Debugging disabled" << endl;
}

void Global::enableDebug(){
	global_debug = true;
	cout << "Debugging enabled" << endl;
}

