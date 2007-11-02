#include "menu/option_adventure.h"
#include "util/token.h"

#include "init.h"

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

#include <iostream>

using namespace std;

static int LAZY_KEY_DELAY = 300;
static const char * DEFAULT_FONT = "/fonts/arial.ttf";

static bool show_loading_screen = true;
static double startingGameSpeed = 1.0;

/* fade the screen and tell the player they lost */
void FadeOut( const string & message ){
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
							FadeOut( "You lose" );
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
				Global::debug( 3 ) << "Game speed " << gameSpeed << endl;
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
				Global::debug( 3 ) << "Game speed " << gameSpeed << endl;
			}

			if ( key[ Keyboard::Key_ENTER_PAD ] ){
				gameSpeed = 1;
				Global::debug( 3 ) << "Game speed " << gameSpeed << endl;
			}

			if ( key[ Keyboard::Key_F4 ] ){
				try{
					world.reloadLevel();
					draw = true;
				} catch ( const LoadException & le ){
					Global::debug( 0 ) << "Could not reload world: " << le.getReason() << endl;
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
				Global::debug( 2 ) << "Saved screenshot to " << file << endl;
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

static void startLoading( pthread_t * thread ){
	if ( show_loading_screen ){
		pthread_create( thread, NULL, loadingScreen, NULL );
	}
}

static void stopLoading( pthread_t thread ){
	if ( show_loading_screen ){
		pthread_mutex_lock( &Global::loading_screen_mutex );
		Global::done_loading = true;
		pthread_mutex_unlock( &Global::loading_screen_mutex );

		pthread_join( thread, NULL );
	}
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
		Global::debug( 0 ) << "Could not load " << filename << ". Reason: " << lex.getReason() << endl;
		return vector< string >();
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

			//Music::pause();
			//Music::fadeIn( 0.3 );
			//Music::loadSong( Util::getFiles( Util::getDataPath() + "/music/", "*" ) );
			//Music::play();

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
			Global::debug( 0 ) << "Could not load " << *it << " because " << le.getReason() << endl;
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

		FadeOut( "Next level" );
	}

	FadeOut( "You win!" );
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

OptionAdventure::OptionAdventure(Token *token)throw( LoadException ) : MenuOption(event)
{
	if ( *token != "adventure" )
		throw LoadException("Not an adventure");
	
	while ( token->hasTokens() )
	{
		try 
		{
			Token * tok;
			*token >> tok;
			if ( *tok == "name" )
			{
				// Create an image and push it back on to vector
				std::string temp;
				*tok >> temp;
				this->setText(temp);
			}
			else 
			{
				Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
				tok->print(" ");
			}
		} 
		catch ( const TokenException & ex )
		{
			// delete current;
			string m( "Menu parse error: " );
			m += ex.getReason();
			throw LoadException( m );
		} 
		catch ( const LoadException & ex )
		{
			// delete current;
			throw ex;
		}
	}
	
	if(getText().empty())throw LoadException("No name set, this option should have a name!");
}

OptionAdventure::~OptionAdventure()
{
	// Nothing
}

void OptionAdventure::logic()
{
}

void OptionAdventure::draw(Bitmap *work)
{
}

void OptionAdventure::run(bool &endGame)
{
	Keyboard key;
	Object * player = NULL;
	try{
		string level = selectLevelSet( Util::getDataPath() + "/levels" );
		key.wait();
		
		player = selectPlayer( false );
		((Player *)player)->setLives( 10 );
		realGame( player, level );
	} catch ( const LoadException & le ){
		Global::debug( 0 ) << "Could not load player: " << le.getReason() << endl;
	} catch ( const ReturnException & r ){
		key.wait();
	}
	if ( player != NULL ){
		delete player;
	}
}

