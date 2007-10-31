#include <allegro.h>

#ifdef WINDOWS
#include <winalleg.h>
#endif

#include <iostream>
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
#include "util/bitmap.h"
#include "util/funcs.h"
#include "util/font.h"
#include "util/load_exception.h"
#include "util/timedifference.h"
#include "object/animation.h"
#include "world.h"

#include "init.h"

#include "menu/menu.h"
#include "util/token_exception.h"
#include "util/load_exception.h"

using namespace std;

int main(){
	init( GFX_AUTODETECT_WINDOWED );
	
	Menu game;
	try
	{
		game.load( Util::getDataPath() + "menu/main.txt");
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
	game.run();
	
	return 0;
}
END_OF_MAIN()
