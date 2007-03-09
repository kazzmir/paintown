#include <allegro.h>

#ifdef WINDOWS
#include <winalleg.h>
#endif

#include <iostream>
#include <loadpng.h>
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

using namespace std;

static Object * bang = NULL;

void testAnimation( ){

	/*
	Object * xobj;
	try{
		xobj = new Enemy( "data/chars/shermie/shermie.txt" );
	} catch ( const LoadException & e ){
		cout<<"Error loading scene: "<<e.getReason()<<endl;
		return;
	}
	delete xobj;
	return;
	*/

	/*
	Scene * xscene;
	try{
		xscene = new Scene( "data/levels/easy/1aeasy.txt" );
	} catch ( const LoadException & e ){
		cout<<"Error loading scene: "<<e.getReason()<<endl;
		return;
	}
	delete xscene;
	return;
	*/

	global_debug = true;

	const char * maxima_path = "data/chars/maxima/maxima.txt";

	bool show_loading_screen = true;

	/*
	bool resize = false;
	set_resize_screen_callback( resize_callback, (void *)&resize );
	*/
		
	/*
	Bitmap Screen( screen );
	Screen.fill( Bitmap::makeColor( 32, 32, 32 ) );

	Screen.printfNormal( 80, 120, Bitmap::makeColor( 255, 255, 255 ), "just hold on.." );
	*/

	/*
	try{
		ch_orig = new Character( maxima_path );
	} catch ( const LoadException & e ){
		cout<<"Could not load "<< maxima_path << " : " << e.getReason() << endl;
		delete ch_orig;
		return;
	}
	*/

	Global::done_loading = false;
	pthread_t loading_screen_thread;
	pthread_mutex_init( &Global::loading_screen_mutex, NULL );
	// const char * this_level = "Loading Level 1";
	// string * str_level = new string( "Loading Level 1" );
	// const char * this_level = str_level->c_str();
	// cout<<"Loading: "<<this_level<<endl;

	if ( show_loading_screen )
		pthread_create( &loading_screen_thread, NULL, loadingScreen, NULL );

	// Character ch( *who );
	Object * ch = NULL;
	const char * bang_path = "data/misc/flash/flash.txt";
	// Object * enemy = NULL;
	try{
		bang = new Effect( bang_path );
	} catch ( const LoadException & ex ){
		cout<<"Could not load effect: "<< ex.getReason() << endl;
		return;
	}
		
	try{
		ch = new Player( maxima_path );
	} catch ( const LoadException & ex ){
		cout<<"Could not load player: "<< ex.getReason() << endl;
		delete bang;
		return;
	}

	Scene * scene;
	try{
		scene = new Scene( "data/levels/easy/1aeasy.txt" );
	} catch ( const LoadException & e ){
		cout<<"Error loading scene: "<<e.getReason()<<endl;
		delete bang;
		delete ch;
		return;
	}

	if ( show_loading_screen ){
		pthread_mutex_lock( &Global::loading_screen_mutex );
		Global::done_loading = true;
		pthread_mutex_unlock( &Global::loading_screen_mutex );

		pthread_join( loading_screen_thread, NULL );
	}

	if ( !show_loading_screen )
		return;

	ch->setX( 140 );
	ch->setZ( 100 );

	Bitmap Screen( screen );
	// Bitmap * Screen = new Bitmap( screen );
	Bitmap work( 320, 240 );
	// Bitmap * screen_buffer = new Bitmap( GFX_X, GFX_Y );
	Bitmap screen_buffer( GFX_X, GFX_Y );
	Global::speed_counter = 0;

	vector< Object * > objects;
	objects.push_back( ch );
	// objects.push_back( enemy );

	int min_x = 0;
	// double min_x_virtual = 0;

	// int zoom = 20;
	int zoom = 0;

	World world;

	while ( !key[ KEY_ESC ] ){

		bool draw = false;
		// int x = speed_counter;
		// cout<<"Speed: "<<x<<endl;

		if ( Global::speed_counter > 0 ){
			// cout<<"Counter = "<<speed_counter<<endl;
			int think = Global::speed_counter;
			TimeDifference logic_time;
			logic_time.startTime();
			while ( think > 0 ){
				draw = true;
				// doLogic( objects, &world );
				world.act();
				think--;
				
				double mx = ch->getX() - work.getWidth() / 2;
				// double mx = ch->getX() - work.getWidth() / 2;
				if ( min_x < mx ){
					min_x++;
					// min_x_virtual += 1.2;
				}
				if ( min_x + work.getWidth() >= scene->getLimit() ){
					min_x = scene->getLimit() - work.getWidth();
				}
				/*
				if ( min_x >= scene->getLimit() ){
					min_x = scene->getLimit();
				}
				*/
				if ( ch->getX() < min_x ){
					ch->setX( min_x );
				}
				if ( ch->getX() > scene->getLimit() ){
					ch->setX( scene->getLimit() );
				}
				if ( ch->getZ() < MIN_WORLD_Z ){
					ch->setZ( MIN_WORLD_Z );
				}
				if ( ch->getZ() > MAX_WORLD_Z ){
					ch->setZ( MAX_WORLD_Z );
				}

				scene->act( min_x, min_x + work.getWidth(), &objects );

			}
			logic_time.endTime();
			// logic_time.printTime("Logic: ");
			Global::speed_counter = 0;

		}
	
		if ( key[ KEY_R ] ){
			objects.clear();
			double x = ch->getX();
			double y = ch->getZ();
			delete ch;

			try{
				// ch_orig = new Character( maxima_path );
				// ch = new Player( *(Character *)ch_orig );
				ch = new Player( maxima_path );
			} catch ( const LoadException & e ){
				cout<<"Could not load "<<maxima_path<<endl;
				// delete ch_orig;
				return;
			}
			objects.push_back( ch );
			// objects.push_back( enemy );
			ch->setX( x );
			ch->setZ( y );

			while ( key[ KEY_R ] );
		}

		if ( key[ KEY_L ] ){
			// ch->hurt( 1 );
			ch->takeDamage( NULL, 1 );
			while ( key[ KEY_L ] );
		}
		
		if ( key[ KEY_Q ] ){
			for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
				(*it)->print();	
			}
			while ( key[ KEY_Q ] );
		}

		if ( key[ KEY_M ] ){
			zoom = 20;
		}

		if ( draw ){

			map< int, vector<Object*> > object_z;

			for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
				Object * n = *it;
				object_z[ n->getRZ() ].push_back( n );
			}
			
			// min_x = (int)min_x_virtual;
			
			scene->drawBack( min_x, &work );
			for ( map<int,vector<Object *> >::iterator it = object_z.begin(); it != object_z.end(); it++ ){
				vector<Object *> & xx = (*it).second;
				for ( vector<Object *>::iterator mm = xx.begin(); mm != xx.end(); mm++ ){

					(*mm)->draw( &work, min_x );
				}
			}
			scene->drawFront( min_x, &work );

			// work.Blit( 0, 0, Screen );
			// work.Stretch( Screen );
			/*
			Screen.acquire();
			work.StretchBy2( Screen );
			Screen.release();
			*/
			TimeDifference td;
			td.startTime();
			// work.StretchBy2( screen_buffer );
			/*
			work.rectangle( ch->getRX() - ch->getWidth()/2, ch->getRY() - ch->getHeight(),
					ch->getRX() + ch->getWidth()/2, ch->getRY(), Bitmap::makeColor( 255, 255, 255 ) );
			*/
			if ( zoom ){
				zoom--;
				int soap = (20-zoom) * 2;
				int x1 = ch->getRX() - ch->getWidth()/2;
				int y1 = ch->getRY() - ch->getHeight();
				int x2 = ch->getWidth();
				int y2 = ch->getRY();
				x1 -= soap;
				y1 -= soap;
				x2 += soap * 2;
				y2 += soap * 2;
				Bitmap zoomed( work, x1, y1, x2, y2 );
				// zoomed.Stretch( screen_buffer );
				screen_buffer.clear();
				// work.Blit( x1, y1, x2, y2, screen_buffer );
				zoomed.Blit( x1, y1, screen_buffer );
			} else {
				work.Stretch( screen_buffer );
			}
			td.endTime();
			// td.printTime("Stretch");

			td.startTime();
			acquire_screen();
			screen_buffer.Blit( world.getX(), world.getY(), Screen );
			release_screen();
			// work.Blit( 0, 0, Screen );
			td.endTime();
			// td.printTime("Blit");
			// work.Stretch( Screen );

			if ( key[ KEY_F12 ] ){
				work.save( "scr.bmp" );
			}

			work.clear();
		}

	}
	
	delete bang;
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
		delete *it;
	}
	delete scene;

}

char getKey( int index ){
	switch ( index ){
		case 0 : return 'Q';
		case 1 : return 'W';
		case 2 : return 'E';
		case 3 : return 'R';
		case 4 : return 'T';
		case 5 : return 'Y';
		case 6 : return 'U';
		case 7 : return 'I';
		case 8 : return 'O';
		case 9 : return 'P';
		case 10 : return 'A';
		case 11 : return 'S';
		case 12 : return 'D';
		case 13 : return 'F';
		case 14 : return 'G';
		case 15 : return 'H';
		case 16 : return 'J';
		case 17 : return 'K';
		case 18 : return 'L';
		case 19 : return 'Z';
		case 20 : return 'X';
		default : return '!';
	}
}

map< int, int > mapKeys(){
	
	map< int, int > xkey;

	xkey[ KEY_Q ] = 0;
	xkey[ KEY_W ] = 1;
	xkey[ KEY_E ] = 2;
	xkey[ KEY_R ] = 3;
	xkey[ KEY_T ] = 4;
	xkey[ KEY_Y ] = 5;
	xkey[ KEY_U ] = 6;
	xkey[ KEY_I ] = 7;
	xkey[ KEY_O ] = 8;
	xkey[ KEY_P ] = 9;
	xkey[ KEY_A ] = 10;
	xkey[ KEY_S ] = 11;
	xkey[ KEY_D ] = 12;
	xkey[ KEY_F ] = 13;
	xkey[ KEY_G ] = 14;
	xkey[ KEY_H ] = 15;
	xkey[ KEY_J ] = 16;
	xkey[ KEY_K ] = 17;
	xkey[ KEY_L ] = 18;
	xkey[ KEY_Z ] = 19;
	xkey[ KEY_X ] = 20;

	return xkey;
}

void displayLines( const Bitmap & bitmap, int x_original, int y_original, const map< string, Animation * > & lines, int index, const Font & font ){

	int x = x_original;
	int y = y_original;

	int count = 0;
	for ( map< string, Animation * >::const_iterator it = lines.begin(); it != lines.end(); it++ ){
		int color = Bitmap::makeColor( 255, 255, 255 );
		if ( count == index ){
			color = Bitmap::makeColor( 255, 0, 0 );
		}

		// bitmap.printf( x, y, color, &font, "%c: %s", getKey( count ), (*it).first.c_str() );
		font.printf( x, y, color, bitmap, "%c: %s", getKey( count ), (*it).first.c_str() );
		y += font.getHeight() + 2;
		if ( y > 480 - font.getHeight() - 1 ){
			y = y_original;
			x += font.textLength( "x" ) * 20;
		}
		count += 1;
	}
}

void waitOnKey( int xkey ){
	while ( key[ xkey ] ) rest( 1 );
}

void showAnimations( string person, int xmap = 0 ){

	cout << "Loading " << person << "\n";
	global_debug = true;
	/*
	string path( "data/chars/" );
	path += person;
	path += "/";
	path += person;
	path += ".txt";
	*/
	string path = person;

	Character * ch;
	try{
		ch = new Character( path, ALLIANCE_NONE );
	} catch ( const LoadException & ex ){
		cout<<"Could not load "<<path<<". Reason: "<<ex.getReason()<<endl;
		// delete ch;
		return;
	}

	ch->setMap( xmap );

	ch->setX( 320 / 2 );
	ch->setZ( 170 );

	// Bitmap * Screen = new Bitmap( screen );
	Bitmap work( 320, 240 );
	// Bitmap * screen_buffer = new Bitmap( GFX_X, GFX_Y );
	Bitmap screen_buffer( GFX_X, GFX_Y );
	Bitmap Screen( screen );
	Global::speed_counter = 0;

	map< int, int > keyHold;

	double runSpeed = 1.0;
	double runCounter = 0;

	int currentAnimation = 0;

	double stay_x = ch->getX();
	double stay_z = ch->getZ();
	while ( !key[ KEY_ESC ] ){

		bool draw = false;
		// int x = speed_counter;
		// cout<<"Speed: "<<x<<endl;

		if ( Global::speed_counter > 0 ){
			// cout<<"Counter = "<<speed_counter<<endl;
			int think = Global::speed_counter;
			while ( think-- > 0 ){
				runCounter += runSpeed;

				while ( runCounter >= 1.0 ){
					if ( ch->testAnimation() ){
						// cout<<"Reseting position"<<endl;
						ch->setX( stay_x );
						ch->setZ( stay_z );
						ch->setY( 0 );
						ch->testReset();
					}

					runCounter -= 1.0;
				}

				map< int, int > xkey = mapKeys();
				for ( map<int,int>::iterator it = xkey.begin(); it != xkey.end(); it++ ){
					int lkey = (*it).first;
					int num = (*it).second;
					if ( key[ lkey ] ){
						currentAnimation = num;
						ch->testAnimation( num );
						// cout<<"Reseting position"<<endl;
						ch->setX( stay_x );
						ch->setZ( stay_z );
						ch->testReset();
						waitOnKey( lkey );
					}
				}
			}
			// ch->setX( stay_x );
			// ch->setZ( stay_z );
			// ch->setY( 0 );
			// logic_time.printTime("Logic: ");
			Global::speed_counter = 0;
			draw = true;

			if ( key[ KEY_RIGHT ] ){
				// ch->moveX( 1 );
				ch->moveRight( 1 );
				stay_x += 1;
			}

			if ( key[ KEY_UP ] ){
				ch->moveZ( -1 );
				stay_z -= 1;
			}

			if ( key[ KEY_DOWN ] ){
				ch->moveZ( 1 );
				stay_z += 1;
			}

			if ( key[ KEY_LEFT ] ){
				// ch->moveX( -1 );
				ch->moveLeft( 1 );
				stay_x -= 1;
			}

			if ( key[ KEY_ALT ] ){
				if ( keyHold[ KEY_ALT ] == 0 ){
					ch->reverseFacing();
					keyHold[ KEY_ALT ] = 20;
				} else {
					keyHold[ KEY_ALT ] -= 1;
				}
			} else {
				keyHold[ KEY_ALT ] = 0;
			}

			if ( key[ KEY_MINUS_PAD ] ){
				runSpeed -= 0.01;
				if ( runSpeed < 0.01 ){
					runSpeed = 0.01;
				}
			}

			if ( key[ KEY_PLUS_PAD ] ){
				runSpeed += 0.01;
			}

			if ( key[ KEY_ENTER_PAD ] ){
				runSpeed = 1.0;
			}

			if ( key[ KEY_V ] ){
				delete ch;

				try{
					ch = new Character( path, ALLIANCE_NONE );
				} catch ( const LoadException & e ){
					cout<<"Could not load "<<path<<". Reason: "<<e.getReason()<<endl;
					return;
				}
				// cout<<"Reseting position"<<endl;
				ch->setX( stay_x );
				ch->setZ( stay_z );
				ch->setY( 0 );
				ch->testAnimation( currentAnimation );
				waitOnKey( KEY_V );
			}

			if ( key[ KEY_B ] ){
				// ch->hurt( 1 );
				ch->takeDamage( NULL, 1 );
				waitOnKey( KEY_B );
			}

		}

		if ( draw ){
			ch->draw( &work, 0 );

			TimeDifference td;
			td.startTime();
			work.Stretch( screen_buffer );
			td.endTime();
			
			// screen_buffer.printfNormal( 1, 1, Bitmap::makeColor( 255, 255, 255 ), ch->getCurrentMovement()->getCurrentFramePath() );
			Font::getDefaultFont().printf( 1, 1, Bitmap::makeColor( 255, 255, 255 ), screen_buffer, ch->getCurrentMovement()->getCurrentFramePath() );
			// screen_buffer.printfNormal( 1, getDefaultFont().getHeight() + 1, Bitmap::makeColor( 255, 255, 255 ), "Speed %f", runSpeed );
			Font::getDefaultFont().printf( 1, Font::getDefaultFont().getHeight() + 1, Bitmap::makeColor( 255, 255, 255 ), screen_buffer, "Speed %f", runSpeed );

			displayLines( screen_buffer, 10, 400, ch->getMovements(), currentAnimation, Font::getDefaultFont() );

			td.startTime();
			acquire_screen();
			screen_buffer.Blit( 0, 0, Screen );
			release_screen();
			td.endTime();

			if ( key[ KEY_F12 ] ){
				work.save( "scr.bmp" );
			}

			work.fill( Bitmap::makeColor(32,32,32) );
		}

		while ( Global::speed_counter == 0 ){
			rest( 1 );
		}
	}

	waitOnKey( KEY_ESC );
}

int addFile( const char * filename, int attribute, void * param ){
	vector< string > * chars = (vector<string> *) param;
	chars->push_back( filename );
	return 0;
}

int searchDirectory( const char * filename, int attribute, void * param ){
	for_each_file_ex( (string(filename) + "/*.txt").c_str(), FA_NONE, FA_NONE, addFile, param );
	return 0;
}

vector< string > findAvailableCharacters( string base ){
	typedef vector<string>::iterator iterator;
	vector< string > chars;
	for_each_file_ex( (base + "/*").c_str(), FA_DIREC, FA_NONE, searchDirectory, (void *) &chars );
	return chars;
}

string chooseCharacter(){
	Bitmap work( GFX_X, GFX_Y );

	Global::speed_counter = 0;

	bool done = false;

	map< int, int > hold;
	vector< string > characters = findAvailableCharacters( "data/chars" );
	typedef vector< string >::iterator iterator;

	characters.insert( characters.begin(), "Quit" );

	int selected = 0;

	hold[ KEY_DOWN ] = 0;
	hold[ KEY_UP ] = 0;

	while ( ! done ){
		bool draw = false;
		while ( Global::speed_counter > 0 ){
			draw = true;
			Global::speed_counter--;

			if ( key[ KEY_ESC ] ){
				return characters[ 0 ];
			}

			if ( key[ KEY_ENTER ] ){
				while ( key[ KEY_ENTER ] ){
					rest( 1 );
				}
				done = true;
			}

			if ( key[ KEY_UP ] ){
				if ( hold[ KEY_UP ] == 0 ){
					selected -= 1;
					if ( selected < 0 ){
						selected = characters.size() - 1;
					}
				}
				hold[ KEY_UP ] -= 1;
			} else {
				hold[ KEY_UP ] = 0;
			}

			if ( key[ KEY_DOWN ] ){
				if ( hold[ KEY_DOWN ] == 0 ){
					selected = (selected + 1) % characters.size();
					hold[ KEY_DOWN ] = 15;
				}
				hold[ KEY_DOWN ] -= 1;
			} else {
				hold[ KEY_DOWN ] = 0;
			}
		}

		if ( draw ){
			
			const Font & font = Font::getDefaultFont();
			int color = 32;
			work.fill( Bitmap::makeColor( color, color, color ) );

			int y = 100;
			int x = 100;
			int white = Bitmap::makeColor( 0xff, 0xff, 0xff );
			int count = 0;
			for ( iterator it = characters.begin(); it != characters.end(); it++ ){

				if ( count == selected ){
					int background = Bitmap::makeColor( 92, 39, 120 );
					work.rectangleFill( x, y, x + 300, y + font.getHeight(), background );
				}
				// work.printfNormal( x, y, white, (*it).c_str() );
				font.printf( x, y, white, work, (*it).c_str() );

				y += font.getHeight() + 1;
				count += 1;
			}

			work.BlitToScreen();
		}

		while ( Global::speed_counter == 0 ){
			rest( 1 );
		}
	}

	return characters[ selected ];
}

int main(){
	init( GFX_AUTODETECT_WINDOWED );

	string next = chooseCharacter();
	cout << "Next = " << next << endl;
	while ( next != "Quit" ){
		showAnimations( next, 0 );
		next = chooseCharacter();
	}
}
