#include <allegro.h>

#ifdef _WIN32
#include <winalleg.h>
#endif

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <vector>
#include <map>

#include "factory/collector.h"
#include "factory/font_render.h"
#include "fonts.h"
#include "globals.h"
#include "level/scene.h"
#include "loading.h"
#include "object/character.h"
#include "object/effect.h"
#include "object/enemy.h"
#include "object/object.h"
#include "object/object_attack.h"
#include "object/player.h"
#include "music.h"
#include "util/bitmap.h"
#include "util/funcs.h"
#include "util/font.h"
#include "util/load_exception.h"
#include "util/timedifference.h"
#include "object/animation.h"
#include "world.h"

#include "init.h"

#include "menu/menu.h"
#include "menu/menu_global.h"
#include "util/token_exception.h"
#include "util/load_exception.h"

using namespace std;


static int gfx = Global::WINDOWED;

static bool isArg( const char * s1, const char * s2 ){
	return strcasecmp( s1, s2 ) == 0;
}

static void showOptions(){
	cout << "Paintown by Jon Rafkind" << endl;
	cout << "-w : Fullscreen mode" << endl;
	cout << "-d <path> : Use data path of <path>. Default is ./data/" << endl;
	cout << "-l # : Enable debug statements. Higher numbers gives more debugging. Default is 0. Example: -l 3" << endl;
	cout << endl;
}

int main( int argc, char ** argv ){
	
	showOptions();
	const char * WINDOWED_ARG = "-w";
	const char * DATAPATH_ARG = "-d";
	const char * DEBUG_ARG = "-l";

	Global::setDebug( 0 );
	
	for ( int q = 1; q < argc; q++ ){
		if ( isArg( argv[ q ], WINDOWED_ARG ) ){
			gfx = Global::FULLSCREEN;
		} else if ( isArg( argv[ q ], DATAPATH_ARG ) ){
			q += 1;
			if ( q < argc ){
				Util::setDataPath( argv[ q ] );
			}
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
	
	if ( ! Global::init( gfx ) ){
		Global::debug( 0 ) << "Could not initialize system" << endl;
	}
	//else MenuGlobals::setFullscreen((gfx == Global::FULLSCREEN ? true : false));
	
	Music music( true );
	try
	{
		Menu game;
		game.load( Util::getDataPath() + "menu/main.txt");
		game.run();
	}
	catch ( const TokenException & ex )
	{
		// delete current;
		cout << "There was a problem with the token. Error was:\n" << ex.getReason() << "\n";
		return 0;
	} 
	catch ( const LoadException & ex )
	{
		// delete current;
		cout << "There was a problem loading the main menu. Error was:\n" << ex.getReason() << "\n";
		return 0;
	}
	
	return 0;
}
END_OF_MAIN()
