#include <allegro.h>

#ifdef WINDOWS
#include <winalleg.h>
#endif

#include <iostream>

#include "saggui/saggui.h"
#include "saggui/saggui_alleg.h"

using namespace std;

static bool leftClick(){
	return (mouse_b & 1);
}

int main(){

	saggui::resourceManager::registerBackend<saggui::ALLEGRO_SAGGUI>();

	saggui::guiManager::setup("allegro");

	if ( ! saggui::guiManager::setupScreen( 640, 480 ) ){
		return 1;
	}

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

	/*
	
	// Bitmap work( 640, 480 );
	// bool quit = false;

	show_mouse( screen );

	bool holdLeft = false;

	// Menu 

	while ( ! key[ KEY_ESC ] ){
		if ( leftClick() && ! holdLeft ){
			holdLeft = true;
		} else if ( ! leftClick() ){
			holdLeft = false;
		}
		rest( 1 );
	}
	*/

	cout << "Exiting.." << endl;
}
