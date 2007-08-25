#ifdef WINDOWS
#define BITMAP dummyBITMAP
#include <windows.h>
#undef BITMAP
#endif

#include "main.h"
#include "init.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "factory/collector.h"
#include "factory/object_factory.h"
#include "factory/heart_factory.h"
#include "factory/font_render.h"
#include "fonts.h"
#include "globals.h"
#include "configuration.h"
#include "level/scene.h"
#include "return_exception.h"
#include "loading.h"
#include "music.h"
#include "object/character.h"
#include "object/effect.h"
#include "object/enemy.h"
#include "object/object.h"
#include "object/object_attack.h"
#include "object/player.h"
#include "object/versus_player.h"
#include "object/versus_enemy.h"
#include "util/bitmap.h"
#include "util/funcs.h"
#include "util/load_exception.h"
#include "util/token.h"
#include "util/token_exception.h"
#include "util/font.h"
#include "util/tokenreader.h"
#include "util/timedifference.h"
#include "select_player.h"
#include "world.h"
#include "versus_world.h"

#include <pthread.h>

/* Global effect for copying */
// static Object * bang = NULL;

#ifndef debug
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
// #define debug
#endif
	
static bool show_loading_screen = true;

static double startingGameSpeed = 1.0;
static int LAZY_KEY_DELAY = 300;
static int gfx = Global::WINDOWED;

static const char * DEFAULT_FONT = "/fonts/arial.ttf";

static int startingLives = 4;

static void stopLoading( pthread_t thread ){
	if ( show_loading_screen ){
		pthread_mutex_lock( &Global::loading_screen_mutex );
		Global::done_loading = true;
		pthread_mutex_unlock( &Global::loading_screen_mutex );

		pthread_join( thread, NULL );
	}
}

static void startLoading( pthread_t * thread ){
	if ( show_loading_screen ){
		pthread_create( thread, NULL, loadingScreen, NULL );
	}
}

/* fade the screen and tell the player they lost */
void fadeOut( const string & message ){
	Bitmap dark( GFX_X, GFX_Y );
	dark.fill( Bitmap::makeColor( 0, 0, 0 ) );
	Global::speed_counter = 0;
	Bitmap::transBlender( 0, 0, 0, 10 );
	int fade = 0;
	const Font & f = Font::getFont( Util::getDataPath() + DEFAULT_FONT, 50, 50 );
	f.printf( 200, 200, Bitmap::makeColor( 255, 0, 0 ), dark, message, 0 );
	while ( fade < 100 ){
		fade++;
	
		Bitmap::transBlender( 0, 0, 0, fade / 2 );

		bool draw = false;
		if ( Global::speed_counter > 0 ){
			draw = true;
			Global::speed_counter = 0;
		}

		if ( draw ){
			dark.drawTrans( 0, 0, *Bitmap::Screen );
		}
		
		while ( Global::speed_counter == 0 ){
			Util::rest( 1 );
		}
	}

	Util::rest( 1000 );
}

/* if given blah.txt and that file exists return blah1.txt
 * returns <file>#.<ext> where # is incremented until the entire
 * filename is unique
 */
static string findNextFile( const char * name ){
	char buf[ 128 ];
	char * extension = strchr( name, '.' );
	char first[ 128 ];
	strncpy( first, name, extension - name );
	first[ extension - name ] = '\0';
	int num = 0;
	sprintf( buf, "%s%d%s", first, num, extension );
	do{
		num += 1;
		sprintf( buf, "%s%d%s", first, num, extension );
	} while ( Util::exists( buf ) );
	return string( buf );
}

static bool playLevel( World & world, Player * player, int helpTime ){
	Keyboard key;
	
	key.setDelay( Keyboard::Key_F2, 100 );
	key.setDelay( Keyboard::Key_F12, 50 );

	key.setDelay( Keyboard::Key_MINUS_PAD, 2 );
	key.setDelay( Keyboard::Key_PLUS_PAD, 2 );
	key.setDelay( Keyboard::Key_P, 100 );

	key.setDelay( Keyboard::Key_F4, 200 );
	
	/* the game graphics are meant for 320x240 and will be stretched
	 * to fit the screen
	 */
	Bitmap work( 320, 240 );
	// Bitmap work( GFX_X, GFX_Y );
	Bitmap screen_buffer( GFX_X, GFX_Y );

	Global::speed_counter = 0;
	Global::second_counter = 0;
	int game_time = 100;
	bool done = false;

	int helpColors[ 100 ];
	Util::blend_palette( helpColors, 100, Bitmap::makeColor( 110, 110, 110 ), Bitmap::makeColor( 255, 255, 255 ) );
	
	double gameSpeed = startingGameSpeed;
	
	double runCounter = 0;
	bool paused = false;
	while ( ! done ){

		bool draw = false;
		key.poll();

		if ( Global::speed_counter > 0 ){
			if ( ! paused ){
				runCounter += Global::speed_counter * gameSpeed;

				while ( runCounter >= 1.0 ){
					draw = true;
					world.act();
					runCounter -= 1.0;

					if ( player->getHealth() <= 0 ){
						player->deathReset();
						if ( player->getLives() == 0 ){
							fadeOut( "You lose" );
							return false;
						}
						world.addObject( player );
					}

					if ( helpTime > 0 ){
						helpTime -= 1;
					}
				}
			}

			const double SPEED_INC = 0.02;
			if ( key[ Keyboard::Key_MINUS_PAD ] ){
				gameSpeed -= SPEED_INC;
				if ( gameSpeed < SPEED_INC ){
					gameSpeed = SPEED_INC;
				}
				cout << "Game speed " << gameSpeed << endl;
			}

			if ( key[ Keyboard::Key_F1 ] ){
				helpTime = helpTime < 110 ? 110 : helpTime;
			}

			if ( key[ Keyboard::Key_P ] ){
				paused = ! paused;
				draw = true;
			}

			if ( key[ Keyboard::Key_PLUS_PAD ] ){
				gameSpeed += SPEED_INC;
				cout << "Game speed " << gameSpeed << endl;
			}

			if ( key[ Keyboard::Key_ENTER_PAD ] ){
				gameSpeed = 1;
				cout << "Game speed " << gameSpeed << endl;
			}

			if ( key[ Keyboard::Key_F2 ] ){
				Global::invertDebug();
			}

			if ( key[ Keyboard::Key_F4 ] ){
				try{
					world.reloadLevel();
					draw = true;
				} catch ( const LoadException & le ){
					cout << "Could not reload world: " << le.getReason() << endl;
				}
			}

			Global::speed_counter = 0;
		}
		
		while ( Global::second_counter > 0 ){
			game_time--;
			Global::second_counter--;
			if ( game_time < 0 )
				game_time = 0;
		}
	
		if ( draw ){
			world.draw( &work );

			work.Stretch( screen_buffer );
			FontRender * render = FontRender::getInstance();
			render->render( &screen_buffer );
	
			const Font & font = Font::getFont( Util::getDataPath() + DEFAULT_FONT, 20, 20 );

			if ( helpTime > 0 ){
				int x = 100;
				int y = screen_buffer.getHeight() / 5;
				int color = helpColors[ helpTime >= 100 ? 99 : helpTime ];
				font.printf( x, y, color, screen_buffer, "Controls", 0 );
				y += font.getHeight() + 1;
				font.printf( x, y, color, screen_buffer, "Up: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getUp() ) );
				y += font.getHeight() + 1;
				font.printf( x, y, color, screen_buffer, "Down: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getDown() ) );
				y += font.getHeight() + 1;
				font.printf( x, y, color, screen_buffer, "Left: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getLeft() ) );
				y += font.getHeight() + 1;
				font.printf( x, y, color, screen_buffer, "Right: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getRight() ) );
				y += font.getHeight() + 1;
				font.printf( x, y, color, screen_buffer, "Jump: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getJump() ) );
				y += font.getHeight() + 1;
				font.printf( x, y, color, screen_buffer, "Attack1: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getAttack1() ) );
				y += font.getHeight() + 1;
				font.printf( x, y, color, screen_buffer, "Attack2: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getAttack2() ) );
				y += font.getHeight() + 1;
				font.printf( x, y, color, screen_buffer, "Attack3: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getAttack3() ) );
				y += font.getHeight() + 1;
				font.printf( x, y, color, screen_buffer, "Press F1 to view this help", 0 );
			}

			if ( paused ){
				screen_buffer.transBlender( 0, 0, 0, 128 );
				screen_buffer.drawingMode( Bitmap::MODE_TRANS );
				screen_buffer.rectangleFill( 0, 0, screen_buffer.getWidth(), screen_buffer.getHeight(), Bitmap::makeColor( 0, 0, 0 ) );
				screen_buffer.drawingMode( Bitmap::MODE_SOLID );
				font.printf( screen_buffer.getWidth() / 2, screen_buffer.getHeight() / 2, Bitmap::makeColor( 255, 255, 255 ), screen_buffer, "Paused", 0 );
			}

			/* getX/Y move when the world is quaking */
			screen_buffer.Blit( world.getX(), world.getY(), *Bitmap::Screen );

			if ( key[ Keyboard::Key_F12 ] ){
				string file = findNextFile( "scr.bmp" );
				cout << "Saved screenshot to " << file << endl;
				work.save( file );
			}

			work.clear();
		}

		while ( Global::speed_counter < 1 ){
			Util::rest( 1 );
			key.poll();
		}

		done |= key[ Keyboard::Key_ESC ] || world.finished();
	}

	if ( key[ Keyboard::Key_ESC ] ){
		while ( key[ Keyboard::Key_ESC ] ){
			key.poll();
			Util::rest( 1 );
		}
		return false;
	}

	return true;
}

static vector< string > readLevels( const string & filename ){
	try{
		TokenReader tr( filename );
		Token * head = tr.readToken();

		vector< string > levels;
		if ( *head == "levels" ){
			while ( head->hasTokens() ){
				string s;
				*head >> s;
				levels.push_back( Util::getDataPath() + s );
			}
		}

		return levels;
	} catch ( const TokenException & lex ){
		cout << "Could not load " << filename << ". Reason: " << lex.getReason() << endl;
		return vector< string >();
	}
}

struct Background{
	string path;
	int z;

	Background():z(0){}
};

static vector< Background > readBackgrounds( const string & path ){
	vector< Background > backgrounds;

	try{
		TokenReader reader( path + "/bgs.txt" );
		Token * head = reader.readToken();

		if ( *head == "backgrounds" ){
			while ( head->hasTokens() ){
				Token * background;
				*head >> background;
				if ( *background == "background" ){
					Token * next;
					Background b;
					for ( int i = 0; i < 2; i++ ){
						*background >> next;
						if ( *next == "path" ){
							*next >> b.path;
						} else if ( *next == "z" ){
							*next >> b.z;
						}
					}
					backgrounds.push_back( b );
				}
			}
		}

	} catch ( const TokenException & ex ){
		cout << "Could not load " + path + "/bgs.txt because " << ex.getReason() << endl;
	}

	/*
	Background b1;
	b1.path = path + "/versus/bg1.png";
	b1.z = 420;
	backgrounds.push_back( b1 );
	*/

	return backgrounds;
}

static void playVersusMode( Character * player1, Character * player2, int round ) throw( ReturnException ){

	player1->setY( 0 );
	player2->setY( 0 );
	player1->setX( 0 );
	player2->setX( 400 );

	player1->setMaxHealth( 150 );
	player2->setMaxHealth( 150 );
	player1->setHealth( 150 );
	player2->setHealth( 150 );

	Keyboard key;

	key.setDelay( Keyboard::Key_P, 100 );

	bool done = false;
	bool paused = false;
	double runCounter = 0;
	double gameSpeed = startingGameSpeed;

	vector< Background > backgrounds;
	backgrounds = readBackgrounds( Util::getDataPath() + "/bgs/versus/" );

	Bitmap background( 640, 480 );
	int z = 400;
	if ( backgrounds.size() != 0 ){
		Background use = backgrounds[ Util::rnd( backgrounds.size() ) ];
		Bitmap b( Util::getDataPath() + "/" + use.path );
		b.Stretch( background );
		z = use.z;
	}
	
	VersusWorld world( z, player1, player2 );
	
	Bitmap work( 640, 480 );
	// Bitmap work( GFX_X, GFX_Y );
	Bitmap screen_buffer( GFX_X, GFX_Y );

	Music::pause();
	Music::fadeIn( 0.3 );
	Music::loadSong( Util::getFiles( Util::getDataPath() + "/music/", "*" ) );
	Music::play();

	int roundColors[ 120 ];
	int showRound = sizeof( roundColors ) / sizeof(int) - 1;
	Util::blend_palette( roundColors, 60, Bitmap::makeColor( 96, 0, 0 ), Bitmap::makeColor( 200, 0, 0 ) );
	Util::blend_palette( roundColors + 60, 60, Bitmap::makeColor( 255, 0, 0 ), Bitmap::makeColor( 96, 0, 0 ) );

	while ( ! done ){

		bool draw = false;
		key.poll();

		if ( Global::speed_counter > 0 ){
			if ( ! paused ){
				runCounter += Global::speed_counter * gameSpeed;

				while ( runCounter >= 1.0 ){
					draw = true;
					world.act();
					runCounter -= 1.0;

					if ( player1->getHealth() <= 0 || player2->getHealth() <= 0 ){
						if ( player1->getHealth() <= 0 && player2->getHealth() > 0 ){
							fadeOut( "Player 2 wins!" );
						} else if ( player1->getHealth() > 0 && player2->getHealth() <= 0 ){
							fadeOut( "Player 1 wins!" );
						} else {
							fadeOut( "Draw!" );
						}
						return;
					}
				}
			}

			const double SPEED_INC = 0.02;
			if ( key[ Keyboard::Key_MINUS_PAD ] ){
				gameSpeed -= SPEED_INC;
				if ( gameSpeed < SPEED_INC ){
					gameSpeed = SPEED_INC;
				}
				cout << "Game speed " << gameSpeed << endl;
			}

			if ( key[ Keyboard::Key_ESC ] ){
				throw ReturnException();
			}

			if ( key[ Keyboard::Key_P ] ){
				paused = ! paused;
				draw = true;
			}

			if ( key[ Keyboard::Key_PLUS_PAD ] ){
				gameSpeed += SPEED_INC;
				cout << "Game speed " << gameSpeed << endl;
			}

			if ( key[ Keyboard::Key_ENTER_PAD ] ){
				gameSpeed = 1;
				cout << "Game speed " << gameSpeed << endl;
			}

			if ( key[ Keyboard::Key_F1 ] ){
				Global::invertDebug();
			}

			Global::speed_counter = 0;
		} else {
			Util::rest( 1 );
		}
		
		/*
		while ( Global::second_counter > 0 ){
			game_time--;
			Global::second_counter--;
			if ( game_time < 0 )
				game_time = 0;
		}
		*/
	
		if ( draw ){
			const Font & font = Font::getFont( Util::getDataPath() + DEFAULT_FONT );
			background.Blit( work );
			world.draw( &work );

			// work.printf( 180, 1, Bitmap::makeColor(255,255,255), (FONT *)all_fonts[ JOHNHANDY_PCX ].dat, "%d", game_time );

			int min_x = (int)(player1->getX() < player2->getX() ? player1->getX() - 50 : player2->getX() - 50);
			int max_x = (int)(player1->getX() > player2->getX() ? player1->getX() + 50 : player2->getX() + 50);
			int min_y = 0;
			// int max_y = screen_buffer.getHeight();

			while ( max_x - min_x < screen_buffer.getWidth() / 2 ){
				max_x += 1;
				min_x -= 1;
			}

			if ( min_x > screen_buffer.getWidth() / 2 ){
				min_x = screen_buffer.getWidth() / 2;
			}
			if ( min_x < 0 ){
				min_x = 0;
			}
			if ( max_x < screen_buffer.getWidth() / 2 ){
				max_x = screen_buffer.getWidth() / 2;
			}
			if ( max_x > screen_buffer.getWidth() ){
				max_x = screen_buffer.getWidth();
			}
	
			/* split is the number of pixels to show in the Y direction */
			int split = screen_buffer.getHeight() * (max_x - min_x) / screen_buffer.getWidth();
			/* cut the difference into two pieces, min_y and max_y */
			min_y = (screen_buffer.getHeight() - split);
			// max_y -= (screen_buffer.getHeight() - split) / 2;

			// work.Stretch( screen_buffer, min_x, min_y, max_x - min_x, max_y - min_y, 0, 0, screen_buffer.getWidth(), screen_buffer.getHeight() );
			work.Blit( screen_buffer );

			if ( showRound > 0 ){
				font.printf( screen_buffer.getWidth() / 2, screen_buffer.getHeight() / 2, roundColors[ showRound ], screen_buffer, "Round %d", 0, round );
				showRound -= 1;
			}

			
			font.printf( 10, 0, Bitmap::makeColor( 255, 255, 255 ), screen_buffer, "%s", 0, player1->getName().c_str() );
			player1->drawLifeBar( 10, font.getHeight(), &screen_buffer );
			font.printf( screen_buffer.getWidth() - 200, 0, Bitmap::makeColor( 255, 255, 255 ), screen_buffer, "%s", 0, player2->getName().c_str() );
			player2->drawLifeBar( screen_buffer.getWidth() - 200, font.getHeight(), &screen_buffer );

			FontRender * render = FontRender::getInstance();
			render->render( &screen_buffer );

			if ( paused ){
				screen_buffer.transBlender( 0, 0, 0, 128 );
				screen_buffer.drawingMode( Bitmap::MODE_TRANS );
				screen_buffer.rectangleFill( 0, 0, screen_buffer.getWidth(), screen_buffer.getHeight(), Bitmap::makeColor( 0, 0, 0 ) );
				screen_buffer.drawingMode( Bitmap::MODE_SOLID );
				const Font & font = Font::getFont( Util::getDataPath() + DEFAULT_FONT );
				font.printf( screen_buffer.getWidth() / 2, screen_buffer.getHeight() / 2, Bitmap::makeColor( 255, 255, 255 ), screen_buffer, "Paused", 0 );
			}

			/* getX/Y move when the world is quaking */
			screen_buffer.Blit( world.getX(), world.getY(), *Bitmap::Screen );

			if ( key[ Keyboard::Key_F12 ] ){
				cout << "Saved screenshot to scr.bmp" << endl;
				work.save( "scr.bmp" );
			}

			work.clear();
		}

		// done |= key[ Keyboard::Key_ESC ] || world.finished();
		done = world.finished();
	}
}

static void realGame( Object * player, const string & levelFile ){

	vector< string > levels = readLevels( levelFile );

	// global_debug = true;

	int showHelp = 300;
	for ( vector< string >::iterator it = levels.begin(); it != levels.end(); it++ ){
		Global::done_loading = false;
		pthread_t loading_screen_thread;

		startLoading( &loading_screen_thread );

		bool gameState = false;
		try {
			World world( player, *it );

			Music::pause();
			Music::fadeIn( 0.3 );
			Music::loadSong( Util::getFiles( Util::getDataPath() + "/music/", "*" ) );
			Music::play();

			Player * playerX = (Player *) player;
			playerX->setY( 200 );
			/* setMoving(false) sets all velocities to 0 */
			playerX->setMoving( false );
			/* but the player is falling so set it back to true */
			playerX->setMoving( true );

			playerX->setStatus( Status_Falling );

			stopLoading( loading_screen_thread );
			

			gameState = playLevel( world, playerX, showHelp );
			showHelp = 0;

		} catch ( const LoadException & le ){
			cout << "Could not load " << *it << " because " << le.getReason() << endl;
			/* if the level couldn't be loaded turn off
			 * the loading screen
			 */
			stopLoading( loading_screen_thread );
		}

		ObjectFactory::destroy();
		HeartFactory::destroy();

		if ( ! gameState ){
			return;
		}

		fadeOut( "Next level" );
	}

	fadeOut( "You win!" );
}

static bool isArg( const char * s1, const char * s2 ){
	return strcasecmp( s1, s2 ) == 0;
}

static const string selectLevelSet( const string & base ) throw( ReturnException ){
	Bitmap::Screen->Blit( Util::getDataPath() + "/paintown-title.png" );
	// Bitmap background( Util::getDataPath() + "/paintown-title.png" );
	int fontY = 20;
	const Font & font = Font::getFont( Util::getDataPath() + DEFAULT_FONT, 20, fontY );
	vector< string > possible = Util::getFiles( base + "/", "*.txt" );
	if ( possible.size() == 0 ){
		return "no-files!!!";
	}

	/*
	for ( vector< string >::iterator it = possible.begin(); it != possible.end(); it++ ){
		string & s = *it;
		s.insert( 0, base + "/" );
	}
	*/
	int choose = 0;

	font.printf( 180, (int)(200 - fontY * 1.2), Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "Select a set of levels to play", 0 );
	for ( unsigned int i = 0; i < possible.size(); i++ ){
		int yellow = Bitmap::makeColor( 255, 255, 0 );
		int white = Bitmap::makeColor( 255, 255, 255 );
		unsigned int color = i == (unsigned) choose ? yellow : white;
		font.printf( 200, (int)(200 + i * fontY * 1.2), color, *Bitmap::Screen, possible[ i ], 0 );
	}

	Keyboard key;
	bool done = false;

	key.setDelay( Keyboard::Key_UP, LAZY_KEY_DELAY );
	key.setDelay( Keyboard::Key_DOWN, LAZY_KEY_DELAY );
	Global::speed_counter = 0;

	while ( ! done ){
		
		key.poll();
		bool draw = false;
		if ( Global::speed_counter > 0 ){
			double think = Global::speed_counter;

			while ( think > 0 ){
				think--;

				if ( key[ Keyboard::Key_UP ] ){
					draw = true;
					choose = (choose - 1 + possible.size()) % possible.size();
				}

				if ( key[ Keyboard::Key_DOWN ] ){
					draw = true;
					choose = (choose + 1 + possible.size()) % possible.size();
				}

				if ( key[ Keyboard::Key_ENTER ] ){
					return possible[ choose ];
				}

				if ( key[ Keyboard::Key_ESC ] ){
					throw ReturnException();
				}
			}

			Global::speed_counter = 0;
		}

		if ( draw ){
			for ( unsigned int i = 0; i < possible.size(); i++ ){
				int yellow = Bitmap::makeColor( 255, 255, 0 );
				int white = Bitmap::makeColor( 255, 255, 255 );
				unsigned int color = i == (unsigned) choose ? yellow : white;
				font.printf( 200, (int)(200 + i * fontY * 1.2), color, *Bitmap::Screen, possible[ i ], 0 );
			}		
		}
		
		while ( Global::speed_counter == 0 ){
			Util::rest( 1 );
			key.poll();
		}
	}

	return "nothing-selected";
}

static int readKey( Keyboard & key ){
	key.wait();
	/*
	key.poll();
	while ( key.keypressed() ){
		key.poll();
		Util::rest( 1 );
	}
	key.clear();
	*/
	key.clear();

	int k = key.readKey();
	key.wait();
	/*
	key.clear();
	key.poll();
	while ( key.keypressed() ){
		key.poll();
		Util::rest( 1 );
	}
	*/

	return k;
}

static void showCredits( const Bitmap & background ){
	Keyboard key;

	const char * credits[] = {
		"Paintown",
		"",
		"Programming",
		"Jon Rafkind",
		"",
		"Contributions",
		"Miguel Gavidia",
		"",
		"Level design",
		"Jon Rafkind",
		"Miguel Gavidia",
		"",
		"Music",
		"aqua.s3m - Purple Motion",
		"c_heaven.xm - One Man Project",
		"elw-sick.xm - elwood",
		"experience.xm - elwood",
		"fall.xm - elwood",
		"kajahtaa.xm - cube",
		"kilimanz.mod - ???",
		"",
		"Email: jon@rafkind.com",
		NULL
	};

	const int maxCredits = sizeof(credits) / sizeof(char*);

	Global::speed_counter = 0;
	int min_y = GFX_Y;
	const Font & font = Font::getFont( Util::getDataPath() + DEFAULT_FONT, 20, 20 );
	Bitmap tmp( GFX_X, GFX_Y );
	while ( ! key[ Keyboard::Key_ESC ] ){

		key.poll();

		bool draw = false;
		if ( Global::speed_counter / 2 > 0 ){
			double think = Global::speed_counter / 2;
			draw = true;

			while ( think > 0 ){
				think--;
				min_y -= 1;
				if ( min_y < -(int)(maxCredits * font.getHeight() * 1.1) ){
					min_y = GFX_Y;
				}
			}

			Global::speed_counter = 0;
		}

		if ( draw ){
			background.Blit( tmp );
			int y = min_y;
			int color = Bitmap::makeColor( 255, 255, 255 );
			for ( const char ** current = credits; *current != 0; current++ ){
				font.printf( 100, y, color, tmp, *current, 0 );	
				y += font.getHeight() + 2;
			}

			tmp.BlitToScreen();
		} else {
			Util::rest( 1 );
		}
	}
}

struct controls{
	struct key{
		char name[ 64 ];
		char description[ 32 ];
		void (Configuration::*set)(int);
		int (Configuration::*get)() const;
	} keys[ 10 ];
	int config;
};

static void setupControls( controls * player, int left, int right, int up, int down, int jump, int attack1, int attack2, int attack3 ){
	player->keys[ left ].get = &Configuration::getLeft;
	player->keys[ left ].set = &Configuration::setLeft;
	sprintf( player->keys[ left ].description, "Left" );
	
	player->keys[ right ].get = &Configuration::getRight;
	player->keys[ right ].set = &Configuration::setRight;
	sprintf( player->keys[ right ].description, "Right" );
	
	player->keys[ up ].get = &Configuration::getUp;
	player->keys[ up ].set = &Configuration::setUp;
	sprintf( player->keys[ up ].description, "Up" );
	
	player->keys[ down ].get = &Configuration::getDown;
	player->keys[ down ].set = &Configuration::setDown;
	sprintf( player->keys[ down ].description, "Down" );
	
	player->keys[ jump ].get = &Configuration::getJump;
	player->keys[ jump ].set = &Configuration::setJump;
	sprintf( player->keys[ jump ].description, "Jump" );
	
	player->keys[ attack1 ].get = &Configuration::getAttack1;
	player->keys[ attack1 ].set = &Configuration::setAttack1;
	sprintf( player->keys[ attack1 ].description, "Attack1" );
	
	player->keys[ attack2 ].get = &Configuration::getAttack2;
	player->keys[ attack2 ].set = &Configuration::setAttack2;
	sprintf( player->keys[ attack2 ].description, "Attack2" );
	
	player->keys[ attack3 ].get = &Configuration::getAttack3;
	player->keys[ attack3 ].set = &Configuration::setAttack3;
	sprintf( player->keys[ attack3 ].description, "Attack3" );

	int all[ 10 ];
	all[ 0 ] = left;
	all[ 1 ] = right;
	all[ 2 ] = up;
	all[ 3 ] = down;
	all[ 4 ] = jump;
	all[ 5 ] = attack1;
	all[ 6 ] = attack2;
	all[ 7 ] = attack3;
	for ( int i = 0; i < 8; i++ ){
		sprintf( player->keys[ i ].name, "%s: %s", player->keys[ i ].description, Keyboard::keyToName( (Configuration::config( player->config ).*(player->keys[ i ].get))() ) );
	}
}

static bool titleScreen(){
	Bitmap background( Util::getDataPath() + "/paintown-title.png" );
	// Bitmap::Screen->Blit( background );
	background.BlitToScreen();
	Music::loadSong( Util::getDataPath() + "/music/aqua.s3m" );

	const int fontY = 20;
	const Font & font = Font::getFont( Util::getDataPath() + DEFAULT_FONT, 20, fontY );

	const int MAIN_PLAY = 0;
	const int MAIN_VERSUS = 1;
	const int MAIN_CHANGE_CONTROLS1 = 2;
	const int MAIN_CHANGE_CONTROLS2 = 3;
	const int MAIN_MORE_OPTIONS = 4;
	const int MAIN_CREDITS = 5;
	const int MAIN_QUIT = 6;
	const char * mainOptions[] = {
		"Adventure mode",
		"Versus mode",
		"Player 1 controls",
		"Player 2 controls",
		"More options",
		"Credits",
		"Quit"
	};
	const unsigned int mainMax = sizeof( mainOptions ) / sizeof( char* );

	const int VERSUS_PLAYER = 0;
	const int VERSUS_COMPUTER = 1;
	const int VERSUS_BACK = 2;
	const char * versusOptions[] = {
		"Player vs Player",
		"Player vs Computer",
		"Back"
	};
	const unsigned int versusMax = sizeof( versusOptions ) / sizeof( char* );

	bool isInvincible = false;
	const unsigned int MORE_INVINCIBLE = 0;
	const unsigned int MORE_GAME_SPEED = 1;
	const unsigned int MORE_VIDEO_MODE = 2;
	const unsigned int MORE_LIVES = 3;
	const unsigned int MORE_BACK = 4;
	char invincible[ 128 ];
	strcpy( invincible, "Invincible: No" );
	char gameSpeed[ 128 ];
	sprintf( gameSpeed, "Game speed: %0.2f", startingGameSpeed );
	char videoMode[ 128 ];
	sprintf( videoMode, gfx == Global::WINDOWED ? "Fullscreen" : "Windowed" );
	char lives[ 128 ];
	sprintf( lives, "Lives: %d", startingLives );
	const char * moreOptions[] = { invincible,
				       gameSpeed,
				       videoMode,
				       lives,
				       "Back"
				       };
	const unsigned int moreMax = sizeof( moreOptions ) / sizeof( char* );

	const unsigned int CONTROL_LEFT = 0;
	const unsigned int CONTROL_RIGHT = 1;
	const unsigned int CONTROL_UP = 2;
	const unsigned int CONTROL_DOWN = 3;
	const unsigned int CONTROL_ATTACK1 = 4;
	const unsigned int CONTROL_ATTACK2 = 5;
	const unsigned int CONTROL_ATTACK3 = 6;
	const unsigned int CONTROL_JUMP = 7;
	const unsigned int CONTROL_BACK = 8;

	controls player1;
	controls player2;

	player1.config = 0;
	setupControls( &player1, CONTROL_LEFT, CONTROL_RIGHT, CONTROL_UP, CONTROL_DOWN, CONTROL_JUMP, CONTROL_ATTACK1, CONTROL_ATTACK2, CONTROL_ATTACK3 );

	player2.config = 1;
	setupControls( &player2, CONTROL_LEFT, CONTROL_RIGHT, CONTROL_UP, CONTROL_DOWN, CONTROL_JUMP, CONTROL_ATTACK1, CONTROL_ATTACK2, CONTROL_ATTACK3 );
	
	const char * controlOptions1[] = { player1.keys[ CONTROL_LEFT ].name,
			                   player1.keys[ CONTROL_RIGHT ].name,
			                   player1.keys[ CONTROL_UP ].name,
			                   player1.keys[ CONTROL_DOWN ].name,
			                   player1.keys[ CONTROL_ATTACK1 ].name,
			                   player1.keys[ CONTROL_ATTACK2 ].name,
			                   player1.keys[ CONTROL_ATTACK3 ].name,
			                   player1.keys[ CONTROL_JUMP ].name,
					   "Back" };

	const unsigned int controlMax1 = sizeof( controlOptions1 ) / sizeof( char* );

	const char * controlOptions2[] = { player2.keys[ CONTROL_LEFT ].name,
			                   player2.keys[ CONTROL_RIGHT ].name,
			                   player2.keys[ CONTROL_UP ].name,
			                   player2.keys[ CONTROL_DOWN ].name,
			                   player2.keys[ CONTROL_ATTACK1 ].name,
			                   player2.keys[ CONTROL_ATTACK2 ].name,
			                   player2.keys[ CONTROL_ATTACK3 ].name,
			                   player2.keys[ CONTROL_JUMP ].name,
					   "Back" };

	const unsigned int controlMax2 = sizeof( controlOptions2 ) / sizeof( char* );


	// font.printf( 1, 1, Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "foo" );
	unsigned int choose = 0;
	const char * const * options = mainOptions;
	unsigned int maxOptions = mainMax;
	for ( unsigned int i = 0; i < maxOptions; i++ ){
		int yellow = Bitmap::makeColor( 255, 255, 0 );
		int white = Bitmap::makeColor( 255, 255, 255 );
		unsigned int color = i == choose ? yellow : white;
		font.printf( 200, (int)(200 + i * fontY * 1.2), color, *Bitmap::Screen, options[ i ], 0 );

	}

	Keyboard key;

	key.setDelay( Keyboard::Key_UP, LAZY_KEY_DELAY );
	key.setDelay( Keyboard::Key_DOWN, LAZY_KEY_DELAY );
	key.setDelay( Keyboard::Key_LEFT, 50 );
	key.setDelay( Keyboard::Key_RIGHT, 50 );

	bool done = false;
	while ( ! done ){
		
		key.poll();
		bool draw = false;
		if ( Global::speed_counter > 0 ){
			double think = Global::speed_counter;

			while ( think > 0 ){
				bool enter = key[ Keyboard::Key_ENTER ];
				think--;

				if ( key[ Keyboard::Key_UP ] ){
					draw = true;
					choose = (choose - 1 + maxOptions) % maxOptions;
				}

				if ( key[ Keyboard::Key_DOWN ] ){
					draw = true;
					choose = (choose + 1 + maxOptions) % maxOptions;
				}

				if ( options == moreOptions && choose == MORE_GAME_SPEED ){
					if ( key[ Keyboard::Key_RIGHT ] ){
						draw = true;
						startingGameSpeed += 0.05;
					}
					if ( key[ Keyboard::Key_LEFT ] ){
						draw = true;
						startingGameSpeed -= 0.05;
						if ( startingGameSpeed < 0.1 ){
							startingGameSpeed = 0.1;
						}
					}
					sprintf( gameSpeed, "Game speed: %0.2f", startingGameSpeed );
				} else if ( options == moreOptions && choose == MORE_LIVES ){
					if ( key[ Keyboard::Key_RIGHT ] ){
						draw = true;
						startingLives += 1;
					}
					if ( key[ Keyboard::Key_LEFT ] ){
						draw = true;
						startingLives -= 1;
					}
					if ( startingLives > 10 ){
						startingLives = 10;
					}
					if ( startingLives < 1 ){
						startingLives = 1;
					}
					sprintf( lives, "Lives: %d", startingLives );
				}

				if ( enter ){
					draw = true;
					if ( options == mainOptions ){
						switch ( choose ){
							case MAIN_QUIT :
							case MAIN_PLAY : {
								done = true;
								break;
							}
							case MAIN_VERSUS : {
								options = versusOptions;
								maxOptions = versusMax;
								choose = 0;
								break;
							}
							case MAIN_CHANGE_CONTROLS1 : {
								options = controlOptions1;
								maxOptions = controlMax1;
								choose = 0;
								break;
							}
							case MAIN_CHANGE_CONTROLS2 : {
								options = controlOptions2;
								maxOptions = controlMax2;
								choose = 0;
								break;
							}
							case MAIN_MORE_OPTIONS : {
								options = moreOptions;
								maxOptions = moreMax;
								choose = 0;
								break;
							}
							case MAIN_CREDITS : {
								showCredits( background );
								key.wait();
								break;
							}
						}
					} else if ( options == versusOptions ){
						switch ( choose ){
							case VERSUS_PLAYER :
							case VERSUS_COMPUTER : {
								done = true;
								break;
							}
							case VERSUS_BACK : {
								options = mainOptions;
								maxOptions = mainMax;
								choose = 0;
								break;
							}
						}
					} else if ( options == moreOptions ){
						switch ( choose ){
							case MORE_INVINCIBLE : {
								isInvincible = ! isInvincible;
								sprintf( invincible, "Invincible: %s", isInvincible ? "Yes" : "No" );
								break;
							}
							case MORE_LIVES : {
								break;
							}
							case MORE_BACK : {
								options = mainOptions;
								maxOptions = mainMax;
								choose = 0;
								break;
							}
							case MORE_GAME_SPEED : {
								break;
							}
							case MORE_VIDEO_MODE : {
								gfx = (gfx == Global::WINDOWED ? Global::FULLSCREEN : Global::WINDOWED);
								sprintf( videoMode, gfx == Global::WINDOWED ? "Fullscreen" : "Windowed" );
								Bitmap::setGraphicsMode( gfx, GFX_X, GFX_Y );
							}
						}
					} else if ( options == controlOptions1 ){
						switch ( choose ){
							case CONTROL_LEFT :
							case CONTROL_RIGHT :
							case CONTROL_UP :
							case CONTROL_DOWN :
							case CONTROL_JUMP :
							case CONTROL_ATTACK1 :
							case CONTROL_ATTACK2 :
							case CONTROL_ATTACK3 : {

								font.printf( 10, 10, Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "Press key for '%s'", 0, player1.keys[ choose ].description );
								int newkey = readKey( key );
								(Configuration::config( player1.config ).*(player1.keys[ choose ].set))( newkey );
								sprintf( player1.keys[ choose ].name, "%s: %s", player1.keys[ choose ].description, Keyboard::keyToName( (Configuration::config( player1.config ).*(player1.keys[ choose ].get))() ) );
								break;
							}
							case CONTROL_BACK : {
								options = mainOptions;
								maxOptions = mainMax;
								choose = 0;

								break;
							}
						}
					} else if ( options == controlOptions2 ){
						switch ( choose ){
							case CONTROL_LEFT :
							case CONTROL_RIGHT :
							case CONTROL_UP :
							case CONTROL_DOWN :
							case CONTROL_JUMP :
							case CONTROL_ATTACK1 :
							case CONTROL_ATTACK2 :
							case CONTROL_ATTACK3 : {

								font.printf( 10, 10, Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "Press key for '%s'", 0, player2.keys[ choose ].description );
								int newkey = readKey( key );
								(Configuration::config( player2.config ).*(player2.keys[ choose ].set))( newkey );
								sprintf( player2.keys[ choose ].name, "%s: %s", player2.keys[ choose ].description, Keyboard::keyToName( (Configuration::config( player2.config ).*(player2.keys[ choose ].get))() ) );
								break;
							}
							case CONTROL_BACK : {
								options = mainOptions;
								maxOptions = mainMax;
								choose = 0;

								break;
							}
						}
					}

					while ( key[ Keyboard::Key_ENTER ] ){
						Util::rest( 1 );
						key.poll();
					}
				}

				// done = key[ Keyboard::Key_ENTER ] || key[ Keyboard::Key_SPACE ] || key[ Keyboard::Key_ESC ];
				if ( key[ Keyboard::Key_ESC ] ){
					draw = true;
					if ( options == mainOptions ){
						done = true;
						choose = MAIN_QUIT;
					} else {
						options = mainOptions;
						maxOptions = mainMax;
					}
				}
				/*
				if ( key[ Keyboard::Key_ENTER ] || key[ Keyboard::Key_SPACE ] ){
				}
				*/
			}

			Global::speed_counter = 0;
		}

		if ( draw ){
			// Bitmap::Screen->Blit( background );
			background.BlitToScreen();
			// Bitmap::Screen->Blit( string( "data/paintown-title.png" ) );
			for ( unsigned int i = 0; i < maxOptions; i++ ){
				int yellow = Bitmap::makeColor( 255, 255, 0 );
				int white = Bitmap::makeColor( 255, 255, 255 );
				unsigned int color = i == choose ? yellow : white;
				font.printf( 200, (int)(200 + i * fontY * 1.2), color, *Bitmap::Screen, options[ i ], 0 );
			}
		}
		
		while ( Global::speed_counter == 0 ){
			Util::rest( 1 );
			key.poll();
		}
	}

	while ( key[ Keyboard::Key_ENTER ] || key[ Keyboard::Key_SPACE ] ){
		key.poll();
		Util::rest( 1 );
	}

	if ( options == mainOptions ){
		switch ( choose ){
			case MAIN_QUIT : {
				return false;
				break;
			}
			
			case MAIN_PLAY : {
				Object * player = NULL;
				try{
					string level = selectLevelSet( Util::getDataPath() + "/levels" );
					key.wait();
					
					player = selectPlayer( isInvincible );
					((Player *)player)->setLives( startingLives );
					realGame( player, level );
				} catch ( const LoadException & le ){
					cout << "Could not load player: " << le.getReason() << endl;
				} catch ( const ReturnException & r ){
					key.wait();
				}
				if ( player != NULL ){
					delete player;
				}
				return true;
				break;
			}
			default : return true;
		}
	} else if ( options == versusOptions ){
		Object * player = NULL;
		Object * enemy = NULL;
		try{
			switch ( choose ){
				case VERSUS_PLAYER : {
					vector<Object *>temp = versusSelect( false );
					player = temp[0];
					enemy = temp[1];
					if(!player || !enemy)throw LoadException("Null player");
					//player = selectPlayer( false );
					//enemy = selectPlayer( false );
					enemy->setAlliance( ALLIANCE_ENEMY );
					
					for ( int i = 0; i < 3; i += 1 ){
						VersusPlayer en( 1, *(Player *) enemy );
						VersusPlayer pl( 0, *(Player *) player );
						playVersusMode( &pl, &en, i + 1 );
					}
					break;
				}
				case VERSUS_COMPUTER : {
					player = selectPlayer( false );
					enemy = selectPlayer( false );
					enemy->setAlliance( ALLIANCE_ENEMY );
					
				       for ( int i = 0; i < 3; i += 1 ){
						VersusEnemy en( *(Player *) enemy );
						VersusPlayer pl( 0, *(Player *) player );
						playVersusMode( &pl, &en, i + 1 );
					}
					break;
				}
			}
			key.wait();
		} catch ( const LoadException & le ){
			cout << "Could not load player: " << le.getReason() << endl;
		} catch ( const ReturnException & r ){
			key.wait();
		}
		if ( player != NULL ){
			delete player;
		}
		if ( enemy != NULL ){
			delete enemy;
		}
		return true;
	}

	key.clear();
	key.poll();
	while ( key.keypressed() ){
		key.poll();
		Util::rest( 1 );
	}

	return false;
}

static void showOptions(){
	cout << "Paintown by Jon Rafkind" << endl;
	cout << "-w : Fullscreen mode" << endl;
	cout << "-d <path> : Use data path of <path>. Default is ./data/" << endl;
	cout << endl;
}

int paintown_main( int argc, char ** argv ){
	
	/* janitor cleans up some global stuff */
	Collector janitor;

	showOptions();
	const char * WINDOWED_ARG = "-w";
	const char * DATAPATH_ARG = "-d";
	
	for ( int q = 1; q < argc; q++ ){
		if ( isArg( argv[ q ], WINDOWED_ARG ) ){
			gfx = Global::FULLSCREEN;
		} else if ( isArg( argv[ q ], DATAPATH_ARG ) ){
			q += 1;
			if ( q < argc ){
				Util::setDataPath( argv[ q ] );
			}
		}
	}
	
	TimeDifference diff;
	diff.startTime();
	if ( ! init( gfx ) ){
		cout << "Could not initialize system" << endl;
	}
	diff.endTime();
	diff.printTime( "Init:" );
	
	/* there can be only one music object. forget stupid
	 * factory crap, just create one here
	 */
	Music m;

	if ( show_loading_screen ){
		pthread_mutex_init( &Global::loading_screen_mutex, NULL );
	}

	while ( titleScreen() != false );

	cout<<"Exiting normally"<<endl;

	return 0;
}
