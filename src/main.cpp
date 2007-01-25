#include <allegro.h>

#ifdef WINDOWS
#include <winalleg.h>
#endif

#include <iostream>
#include <loadpng.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

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
#include "util/load_exception.h"
#include "util/timedifference.h"
#include "world.h"

#define GFX_X 640
#define GFX_Y 480

/* Global effect for copying */
Object * bang = NULL;

/*
#define GFX_X 1280
#define GFX_Y 960
*/

volatile int speed_counter = 0;
volatile int second_counter = 0;

void inc_speed_counter() {
	speed_counter++;
}
END_OF_FUNCTION( inc_speed_counter );

void inc_second_counter() {
	second_counter++;
}
END_OF_FUNCTION( inc_second_counter );



#ifndef debug
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
// #define debug
#endif

DATAFILE * all_fonts;

void init( int gfx ){

	cout<<"Allegro init: "<<allegro_init()<<endl;
	cout<<"Loadpng init: "<<loadpng_init()<<endl;
	cout<<"Install timer: "<<install_timer()<<endl;
	
	set_volume_per_voice( 0 );
	cout<<"Install sound: "<<install_sound( DIGI_AUTODETECT, MIDI_NONE, "" )<<endl;

	cout<<"Install keyboard: "<<install_keyboard()<<endl;
	cout<<"Install mouse: "<<install_mouse()<<endl;
	loadpng_init();
	set_color_depth( 16 );
	// int gfx = GFX_AUTODETECT_WINDOWED;
	// gfx = GFX_AUTODETECT_FULLSCREEN;
	cout<<"Set gfx mode: "<<set_gfx_mode( gfx, GFX_X, GFX_Y, 0, 0 )<<endl;
	cout<<"Can triple buffer = "<<(gfx_capabilities & GFX_CAN_TRIPLE_BUFFER)<<endl;

	LOCK_VARIABLE( speed_counter );
	LOCK_VARIABLE( second_counter );
	LOCK_FUNCTION( (void *)inc_speed_counter );
	LOCK_FUNCTION( (void *)inc_second_counter );
	cout<<"Install timer: "<<install_int_ex( inc_speed_counter, BPS_TO_TIMER( 90 ) )<<endl;
	cout<<"Install second timer: "<<install_int_ex( inc_second_counter, BPS_TO_TIMER( 1 ) )<<endl;
	srand( time( NULL ) );
	all_fonts = load_datafile( "data/fonts.dat" );
	if ( ! all_fonts ){
		cout << "Could not load data/fonts.dat!" << endl;
		exit( 1 );
	}
	cout<<"Loaded fonts " << endl;

	cout<<"-- END init --"<<endl;

}

/*
void doLogic( vector< Object * > & objects, World * world ){
	vector< Object * > added_effects;
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); ){
		Object * good = *it;
		good->act( &objects, world );


		/ * Check for collisions * /
		if ( good->isAttacking() ){
			// ObjectAttack * o_good = dynamic_cast<ObjectAttack*>( good );
			ObjectAttack * o_good = (ObjectAttack *)good;
			for ( vector<Object*>::iterator fight = objects.begin(); fight != objects.end(); fight++){
				if ( (*fight)->isCollidable( good ) && good->isCollidable( *fight ) ){

					// cout<<"Attacking"<<endl;
					if ( good->ZDistance( *fight ) < MIN_RELATIVE_DISTANCE && (*fight)->collision( o_good ) ){ 

						// cout<<"Attacked " << *fight << " with animation "<< good->getAttackName() << " ticket " << o_good->getTicket() << endl;

						// if ( good->isAttacking() ){
							int x = 0, y = 0;
							// o_good->getAttackCoords(x,y);
							x = (*fight)->getX();
							y = (*fight)->getRY() - (*fight)->getHeight() + (*fight)->getHeight() / 3;
							
							Object * addx = bang->copy();
							addx->setX( x );
							addx->setY( 0 );
							addx->setZ( y+addx->getHeight()/2 );
							addx->setHealth( 1 );
							added_effects.push_back( addx );

							(*fight)->collided( o_good );
							(*fight)->takeDamage( o_good, o_good->getDamage() );
						// }
					}
				}
			}
		}

		if ( (*it)->getHealth() <= 0 ){
			delete *it;
			it = objects.erase( it );
		} else ++it;
		// it++;
	}
	objects.insert( objects.end(), added_effects.begin(), added_effects.end() );
}
*/

/*
void resize_callback( int w, int h, void * data ){
	*(bool *)data = true;
}
*/

pthread_mutex_t loading_screen_mutex;
bool done_loading = false;

void realGame( Object * player ){
	// global_debug = true;
	bool show_loading_screen = true;

	done_loading = false;
	pthread_t loading_screen_thread;
	
	if ( show_loading_screen )
		pthread_create( &loading_screen_thread, NULL, loadingScreen, NULL );

	World world = World( player, "data/levels/easy/1aeasy.txt" );
	
	if ( show_loading_screen ){
		pthread_mutex_lock( &loading_screen_mutex );
		done_loading = true;
		pthread_mutex_unlock( &loading_screen_mutex );

		pthread_join( loading_screen_thread, NULL );
	}

	Bitmap Screen( screen );
	Bitmap work( 320, 240 );
	Bitmap screen_buffer( Screen.getWidth(), Screen.getHeight() );
	speed_counter = 0;
	second_counter = 0;
	int game_time = 100;

	/* end of init stuff */
	
	while ( !key[ KEY_ESC ] ){

		bool draw = false;

		if ( speed_counter > 0 ){
			int think = speed_counter;
			while ( think > 0 ){
				draw = true;
				world.act();
				think--;
			}
			speed_counter = 0;
		}
		
		while ( second_counter > 0 ){
			game_time--;
			second_counter--;
			if ( game_time < 0 )
				game_time = 0;
		}
	
		if ( draw ){
			world.draw( &work );

			// work.printf( 180, 1, Bitmap::makeColor(255,255,255), (FONT *)all_fonts[ JOHNHANDY_PCX ].dat, "%d", game_time );

			work.Stretch( screen_buffer );
			FontRender * render = FontRender::getInstance();
			render->render( &screen_buffer );

			acquire_screen();
			screen_buffer.Blit( world.getX(), world.getY(), Screen );
			release_screen();

			if ( key[ KEY_F12 ] ){
				work.save( "scr.bmp" );
			}

			work.clear();
		}

		while ( speed_counter == 0 ){
			rest( 1 );
		}
	}

}

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

	done_loading = false;
	pthread_t loading_screen_thread;
	pthread_mutex_init( &loading_screen_mutex, NULL );
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
		pthread_mutex_lock( &loading_screen_mutex );
		done_loading = true;
		pthread_mutex_unlock( &loading_screen_mutex );

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
	speed_counter = 0;

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

		if ( speed_counter > 0 ){
			// cout<<"Counter = "<<speed_counter<<endl;
			int think = speed_counter;
			TimeDifference logic_time;
			logic_time.startTime();
			while ( think > 0 ){
				draw = true;
				// doLogic( objects, &world );
				world.act();
				think--;
				
				int mx = ch->getX() - work.getWidth() / 2;
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
			speed_counter = 0;

		}
	
		if ( key[ KEY_R ] ){
			objects.clear();
			int x = ch->getX();
			int y = ch->getZ();
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
				object_z[ n->getZ() ].push_back( n );
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

void showAnimations( const char * person, int xmap = 0 ){

	global_debug = true;
	string path( "data/chars/" );
	path += person;
	path += "/";
	path += person;
	path += ".txt";

	Object * ch;
	try{
		ch = new Character( path, ALLIANCE_NONE );
	} catch ( const LoadException & ex ){
		cout<<"Could not load "<<path<<". Reason: "<<ex.getReason()<<endl;
		// delete ch;
		return;
	}

	((Character *)ch)->setMap( xmap );

	ch->setX( 40 );
	ch->setZ( 170 );

	// Bitmap * Screen = new Bitmap( screen );
	Bitmap work( 320, 240 );
	// Bitmap * screen_buffer = new Bitmap( GFX_X, GFX_Y );
	Bitmap screen_buffer( GFX_X, GFX_Y );
	Bitmap Screen( screen );
	speed_counter = 0;

	int stay_x = ch->getX();
	int stay_z = ch->getZ();
	while ( !key[ KEY_ESC ] ){

		bool draw = false;
		// int x = speed_counter;
		// cout<<"Speed: "<<x<<endl;

		if ( speed_counter > 0 ){
			// cout<<"Counter = "<<speed_counter<<endl;
			int think = speed_counter;
			while ( think-- > 0 ){
				if ( ((Character *)ch)->testAnimation() ){
					// cout<<"Reseting position"<<endl;
					ch->setX( stay_x );
					ch->setZ( stay_z );
					ch->setY( 0 );
					((Character *)ch)->testReset();
				}

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
				for ( map<int,int>::iterator it = xkey.begin(); it != xkey.end(); it++ ){
					int lkey = (*it).first;
					int num = (*it).second;
					if ( key[ lkey ] ){
						((Character *)ch)->testAnimation( num );
						// cout<<"Reseting position"<<endl;
						ch->setX( stay_x );
						ch->setZ( stay_z );
						((Character *)ch)->testReset();
					}
				}
			}
			// ch->setX( stay_x );
			// ch->setZ( stay_z );
			// ch->setY( 0 );
			// logic_time.printTime("Logic: ");
			speed_counter = 0;
			draw = true;
		}

		if ( key[ KEY_RIGHT ] ){
			ch->moveX( 1 );
			stay_x++;
		}

		if ( key[ KEY_LEFT ] ){
			ch->moveX( -1 );
			stay_x--;
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

			while ( key[ KEY_V ] );
		}

		if ( key[ KEY_B ] ){
			// ch->hurt( 1 );
			ch->takeDamage( NULL, 1 );
			while ( key[ KEY_B ] );
		}


		if ( draw ){

			// cout<<"X: "<<ch->getX()<<" Z:"<<ch->getZ()<<endl;
			ch->draw( &work, 0 );

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
			work.Stretch( screen_buffer );
			td.endTime();
			// td.printTime("Stretch");

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

	}
	
}

int main( int argc, char ** argv ){
	
	Collector janitor;
	int gfx = GFX_AUTODETECT_WINDOWED;

	bool tester = false;
	const char * name = NULL;
	int xmap = 0;
	
	for ( int q = 1; q < argc; q++ ){
		if ( strcasecmp( argv[q], "-w" ) == 0 ){
			gfx = GFX_AUTODETECT_FULLSCREEN;
		} else if ( strcasecmp( argv[q], "-t" ) == 0 ){
			q++;
			tester = true;
			if ( q < argc )
				name = argv[q];
			q++;
			if ( q < argc )
				xmap = atoi( argv[q] );
		}
	}
	
	TimeDifference diff;
	diff.startTime();
	init( gfx );
	diff.endTime();
	diff.printTime("Init: ");

	if ( tester ){
		// char * name = argv[ 1 ];
		showAnimations( name, xmap );
		return 0;
	}

	/*
	while ( !key[ KEY_ESC ] ){
		Collector Blah;
		testAnimation();
	}
	*/

	// loadingScreen( NULL );

	pthread_mutex_init( &loading_screen_mutex, NULL );
	
	pthread_t loading_screen_thread;
	pthread_create( &loading_screen_thread, NULL, loadingScreen, NULL );

	const char * player_path = "data/chars/maxima/maxima.txt";
	// const char * player_path = "data/chars/gangmember/gangmember.txt";

	Object * ch;
	try{
		ch = new Player( player_path );
	} catch ( const LoadException & ex ){
		cout<<"Could not load player: "<< ex.getReason() << endl;
		return 1;
	}

	pthread_mutex_lock( &loading_screen_mutex );
	done_loading = true;
	pthread_mutex_unlock( &loading_screen_mutex );
	pthread_join( loading_screen_thread, NULL );

	realGame( ch );

	delete ch;

	// testAnimation( );

	unload_datafile( all_fonts );
	cout<<"Exiting normally"<<endl;

	return 0;

}
END_OF_MAIN()
