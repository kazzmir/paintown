#include "globals.h"
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

