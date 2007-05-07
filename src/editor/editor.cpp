#include <allegro.h>

#ifdef WINDOWS
#include <winalleg.h>
#endif

#include <iostream>

#include "saggui/saggui.h"
#include "saggui/saggui_alleg.h"

using namespace saggui;
using namespace std;

class myclass: public has_slots<>{
public:
	myclass(){
	}

	void blah( const coord & c){
		cout << "Got a coord or whatever" << endl;
	}
};

int main(){

	resourceManager::registerBackend<saggui::ALLEGRO_SAGGUI>();

	guiManager::setup("allegro");

	if ( ! guiManager::setupScreen( 640, 480 ) ){
		return 1;
	}

	guiTimer timer;

	myclass m;
	button bu, bu2, bu3;
	graphic * g = resourceManager::createGraphic();
	g->loadBmp( "test.bmp" );
	bu.setBitmap( g );
	bu.position.set( 20, 20, 20, 20 );
	bu.onLeftClick.connect( &m, &myclass::blah );
	/*
	bu2.setBitmap( g );
	bu2.position.set( 20, bu.position.getY() + 25, 20, 20 );
	bu3.setBitmap( g );
	bu3.position.set( 20, bu2.position.getY() + 25, 20, 20 );
	*/

	scrollBar scroll;
	scroll.position.set( 10, 10, 100, 100 );
	scroll.addChild( &bu );
	scroll.setMaxSize( 100 );
	scroll.setType( AT_Vertical );

	bool quit = false;
	timer.reset();
	while ( ! quit ){
		bool draw = false;
		
		while ( timer.msecs() > 1000 / 60 ){
			draw = true;
			guiManager::update();
			timer.reset();
			quit = key[ KEY_ESC ];
		}

		if ( draw ){
			guiManager::getSystemBuffer()->clearBmp(saggui::rgba::BLACK);
			guiManager::renderWindows();
		}

		while ( timer.msecs() < 1000 / 60 ){
			timer.sleep( 1 );
		}
	}

	cout << "Exiting.." << endl;
}
