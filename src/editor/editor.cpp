#include <allegro.h>

#ifdef WINDOWS
#include <winalleg.h>
#endif

#include <iostream>

#include "saggui/saggui.h"
#include "saggui/saggui_alleg.h"

using namespace saggui;
using namespace std;

static bool leftClick(){
	return (mouse_b & 1);
}

int main(){

	resourceManager::registerBackend<saggui::ALLEGRO_SAGGUI>();

	guiManager::setup("allegro");

	if ( ! guiManager::setupScreen( 640, 480 ) ){
		return 1;
	}

	button bu;
	graphic * g = resourceManager::createGraphic();
	g->loadBmp( "test.bmp" );
	bu.setBitmap( g );
	bu.position.set( 20, 20, 100, 100 );

	bool quit = false;
	while ( ! quit ){
		bool draw = false;
		
		draw = true;
		saggui::guiManager::update();

		quit = key[ KEY_ESC ];

		if ( draw ){
			saggui::guiManager::getSystemBuffer()->clearBmp(saggui::rgba::BLACK);
			saggui::guiManager::renderWindows();
		}

		rest( 1 );
	}

	cout << "Exiting.." << endl;
}
