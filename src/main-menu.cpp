#include <sstream>

#include "factory/collector.h"
#include "network/network.h"
#include "util/token_exception.h"
#include "music.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "game/input-manager.h"
#include "util/timedifference.h"
#include "util/funcs.h"
#include "globals.h"
#include "configuration.h"
#include "init.h"

using namespace std;

static int gfx = Global::WINDOWED;

static bool isArg( const char * s1, const char * s2 ){
	return strcasecmp( s1, s2 ) == 0;
}

static void showOptions(){
	Global::debug( 0 ) << "Paintown by Jon Rafkind" << endl;
	Global::debug( 0 ) << "-w : Fullscreen mode" << endl;
	Global::debug( 0 ) << "-d <path> : Use data path of <path>. Default is " << Util::getDataPath() << endl;
	Global::debug( 0 ) << "-l # : Enable debug statements. Higher numbers gives more debugging. Default is 0. Example: -l 3" << endl;
	Global::debug( 0 ) << "-m : Turn off music" << endl;
	Global::debug( 0 ) << endl;
}

int paintown_main( int argc, char ** argv ){
	
	bool music_on = true;
	Collector janitor;
	showOptions();
	const char * WINDOWED_ARG = "-w";
	const char * DATAPATH_ARG = "-d";
	const char * DEBUG_ARG = "-l";
	const char * MUSIC_ARG = "-m";

	Global::setDebug( 0 );
	
	for ( int q = 1; q < argc; q++ ){
		if ( isArg( argv[ q ], WINDOWED_ARG ) ){
			gfx = Global::FULLSCREEN;
		} else if ( isArg( argv[ q ], DATAPATH_ARG ) ){
			q += 1;
			if ( q < argc ){
				Util::setDataPath( argv[ q ] );
			}
		} else if ( isArg( argv[ q ], MUSIC_ARG ) ){
			music_on = false;
		} else if ( isArg( argv[ q ], DEBUG_ARG ) ){
			q += 1;
			if ( q < argc ){
				istringstream i( argv[ q ] );
				int f;
				i >> f;
				Global::setDebug( f );
			}
		}
	}

	TimeDifference diff;
	diff.startTime();
	if ( ! Global::init( gfx ) ){
		Global::debug( 0 ) << "Could not initialize system" << endl;
		return -1;
	} else {
		MenuGlobals::setFullscreen((gfx == Global::FULLSCREEN ? true : false));
	}
	diff.endTime();
	diff.printTime( "Init:" );
	
        InputManager input;
	Music music(music_on);
	try{
		Menu game;
		game.load(Util::getDataPath() + "/menu/main.txt");
		game.run();
	} catch (const TokenException & ex){
		Global::debug(0) << "There was a problem with the token. Error was:\n  " << ex.getReason() << endl;
		return -1;
	} catch (const LoadException & ex){
		Global::debug(0) << "There was a problem loading the main menu. Error was:\n  " << ex.getReason() << endl;
		return -1;
        }

	Configuration::saveConfiguration();
	
	return 0;
}
