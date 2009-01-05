#include "util/bitmap.h"
#include "game.h"
#include "music.h"
#include "util/funcs.h"
#include "util/font.h"
#include "configuration.h"
#include "object/object.h"
#include "object/character.h"
#include "object/player.h"
#include "factory/object_factory.h"
#include "factory/heart_factory.h"
#include "level/utils.h"
#include "factory/font_render.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "util/keyboard.h"
#include "globals.h"
#include "script/script.h"
#include "loading.h"
#include "network/network.h"
#include "world.h"
#include "versus_world.h"
#include "init.h"
#include <iostream>
#include <math.h>

using namespace std;

static const char * DEFAULT_FONT = "/fonts/arial.ttf";
static int LAZY_KEY_DELAY = 300;
static bool show_loading_screen = true;

namespace Game{

struct Background{
	string path;
	int z;

	Background():z(0){}
};
	
static double startingGameSpeed(){
	return 1.0;
}

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

static Network::Message removeMessage( int id ){
	Network::Message message;

	message.id = 0;
	message << World::REMOVE;
	message << id;

	return message;
}

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
		Global::debug( 0 ) << "Could not load " + path + "/bgs.txt because " << ex.getReason() << endl;
	}

	/*
	Background b1;
	b1.path = path + "/versus/bg1.png";
	b1.z = 420;
	backgrounds.push_back( b1 );
	*/

	return backgrounds;
}

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
	} while ( num != 0 && Util::exists( buf ) );
	return string( buf );
}

static void drawHelp( const Font & font, int x, int y, int color, Bitmap & buffer ){
	font.printf( x, y, color, buffer, "Controls", 0 );
	y += font.getHeight() + 1;
	font.printf( x, y, color, buffer, "Up: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getUp() ) );
	y += font.getHeight() + 1;
	font.printf( x, y, color, buffer, "Down: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getDown() ) );
	y += font.getHeight() + 1;
	font.printf( x, y, color, buffer, "Left: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getLeft() ) );
	y += font.getHeight() + 1;
	font.printf( x, y, color, buffer, "Right: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getRight() ) );
	y += font.getHeight() + 1;
	font.printf( x, y, color, buffer, "Jump: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getJump() ) );
	y += font.getHeight() + 1;
	font.printf( x, y, color, buffer, "Attack1: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getAttack1() ) );
	y += font.getHeight() + 1;
	font.printf( x, y, color, buffer, "Attack2: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getAttack2() ) );
	y += font.getHeight() + 1;
	font.printf( x, y, color, buffer, "Attack3: %s", 0,  Keyboard::keyToName( Configuration::config( 0 ).getAttack3() ) );
	y += font.getHeight() + 1;
	font.printf( x, y, color, buffer, "Tab to hide/show minimap", 0 );
	y += font.getHeight() + 1;
	font.printf( x, y, color, buffer, "Press F1 to view this help", 0 );
}

static bool playLevel( World & world, const vector< Object * > & players, int helpTime ){
	Keyboard key;
	
	key.setDelay( Keyboard::Key_F2, 100 );
	key.setDelay( Keyboard::Key_F12, 50 );

	if ( Global::getDebug() > 0 ){
		key.setDelay( Keyboard::Key_MINUS_PAD, 2 );
		key.setDelay( Keyboard::Key_PLUS_PAD, 2 );
		key.setDelay( Keyboard::Key_F4, 200 );
		key.setDelay( Keyboard::Key_F8, 300 );
	}

	key.setDelay( Keyboard::Key_P, 100 );
	key.setDelay( Keyboard::Key_TAB, 300 );
	
	/* the game graphics are meant for 320x240 and will be stretched
	 * to fit the screen
	 */
	Bitmap work( 320, 240 );
	// Bitmap work( GFX_X, GFX_Y );
	Bitmap screen_buffer( GFX_X, GFX_Y );

        world.getEngine()->createWorld(world);

	Global::speed_counter = 0;
	Global::second_counter = 0;
	// int game_time = 100;
        int frames = 0;
        const int max_fps_index = 5;
        double fps[max_fps_index] = {0,0,0,0,0};
        int fps_index = 0;
        bool show_fps = false;
	bool done = false;

	double gameSpeed = startingGameSpeed();
	
	double runCounter = 0;
	bool paused = false;
	while ( ! done ){

		bool draw = false;
		key.poll();

		if ( Global::speed_counter > 0 ){
			if ( ! paused ){
				runCounter += Global::speed_counter * gameSpeed * Global::LOGIC_MULTIPLIER;

				while ( runCounter >= 1.0 ){
					draw = true;
					world.act();
                                        world.getEngine()->tick();
					runCounter -= 1.0;

					for ( vector< Object * >::const_iterator it = players.begin(); it != players.end(); it++ ){
						Character * player = (Character *) *it;
						if ( player->getHealth() <= 0 ){
							if ( player->spawnTime() == 0 ){
								player->deathReset();
								if ( player->getLives() == 0 ){
									fadeOut( screen_buffer, "You lose" );
									return false;
								}
								world.addMessage( removeMessage( player->getId() ) );
								world.addObject( player );
								world.addMessage( player->getCreateMessage() );
								world.addMessage( player->movedMessage() );
								world.addMessage( player->animationMessage() );
							}
						}
					}

					if ( helpTime > 0 ){
						helpTime -= 2;
					}
				}
			}

			if ( key[ Keyboard::Key_F1 ] ){
				helpTime = helpTime < 260 ? 260 : helpTime;
			}
                        
                        if ( key[ Keyboard::Key_F9 ] ){
                            show_fps = true;
                        }
                        if ( key[ Keyboard::Key_F10 ] ){
                            show_fps = false;
                        }

			if ( key[ Keyboard::Key_P ] ){
				paused = ! paused;
				draw = true;
			}

			if ( key[ Keyboard::Key_TAB ] ){
				world.drawMiniMaps( ! world.shouldDrawMiniMaps() );
			}
			
			/*
			if ( key[ Keyboard::Key_F8 ] ){
				world.killAllHumans( player );
			}
			*/

			if ( Global::getDebug() > 0 ){
				const double SPEED_INC = 0.02;
				if ( key[ Keyboard::Key_PLUS_PAD ] ){
					gameSpeed += SPEED_INC;
					Global::debug( 3 ) << "Game speed " << gameSpeed << endl;
				}

				if ( key[ Keyboard::Key_MINUS_PAD ] ){
					gameSpeed -= SPEED_INC;
					if ( gameSpeed < SPEED_INC ){
						gameSpeed = SPEED_INC;
					}
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
			}

			Global::speed_counter = 0;
		}
		
                /*
		while ( Global::second_counter > 0 ){
			game_time--;
			Global::second_counter--;
			if ( game_time < 0 )
				game_time = 0;
		}
                */
                if ( Global::second_counter > 0 ){
                    fps[fps_index] = (double) frames / (double) Global::second_counter;
                    fps_index = (fps_index+1) % max_fps_index;
                    Global::second_counter = 0;
                    frames = 0;
                }
	
		if ( draw ){
                    frames += 1;
			world.draw( &work );

			work.Stretch( screen_buffer );
			FontRender * render = FontRender::getInstance();
			render->render( &screen_buffer );
	
			const Font & font = Font::getFont( Util::getDataPath() + DEFAULT_FONT, 20, 20 );

			if ( helpTime > 0 ){
				int x = 100;
				int y = screen_buffer.getHeight() / 5;
				int color = Bitmap::makeColor( 255, 255, 255 );
				Bitmap::transBlender( 0, 0, 0, helpTime > 255 ? 255 : helpTime  );
				screen_buffer.drawingMode( Bitmap::MODE_TRANS );
				drawHelp( font, x, y, color, screen_buffer );
				screen_buffer.drawingMode( Bitmap::MODE_SOLID );
			}

			if ( paused ){
				screen_buffer.transBlender( 0, 0, 0, 128 );
				screen_buffer.drawingMode( Bitmap::MODE_TRANS );
				screen_buffer.rectangleFill( 0, 0, screen_buffer.getWidth(), screen_buffer.getHeight(), Bitmap::makeColor( 0, 0, 0 ) );
				screen_buffer.drawingMode( Bitmap::MODE_SOLID );
				font.printf( screen_buffer.getWidth() / 2, screen_buffer.getHeight() / 2, Bitmap::makeColor( 255, 255, 255 ), screen_buffer, "Paused", 0 );
			}
                        
                        double real_fps = 0;
                        for ( int i = 0; i < max_fps_index; i++ ){
                            real_fps += fps[i];
                        }
                        real_fps /= max_fps_index;
                        if ( show_fps ){
                            font.printf( screen_buffer.getWidth() - 120, 10, Bitmap::makeColor(255,255,255), screen_buffer, "FPS: %0.2f", 0, real_fps );
                        }

			/* getX/Y move when the world is quaking */
			screen_buffer.BlitToScreen( world.getX(), world.getY() );

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

        bool force_quit = key[Keyboard::Key_ESC];

        if (!force_quit){
            work.clear();
            Sound snapshot(Util::getDataPath() + "/sounds/snapshot.wav");
            for (deque<Bitmap*>::const_iterator it = world.getScreenshots().begin(); it != world.getScreenshots().end(); it++){
                Bitmap * shot = *it;
                int angle = Util::rnd(13) - 6;

                /*
                int gap = 4;
                int x = Util::rnd(work.getWidth() - 2 * work.getWidth() / gap) + work.getWidth() / gap;
                int y = Util::rnd(work.getHeight() - 2 * work.getHeight() / gap) + work.getHeight() / gap;
                double scale = 1.0 - log(world.getScreenshots().size()+1) / 9.0;
                shot->greyScale().drawPivot(shot->getWidth() / 2, shot->getHeight() / 2, x, y, angle, scale, work);
                */

                int x = work.getWidth() / 2;
                int y = work.getHeight() / 2;
                double scale = 0.9;
                shot->border(0, 1, Bitmap::makeColor(64,64,64));
                shot->greyScale().drawPivot(shot->getWidth() / 2, shot->getHeight() / 2, x, y, angle, scale, work);
                work.Stretch( screen_buffer );
                screen_buffer.BlitToScreen();
                snapshot.play();
                Util::rest(1500);
            }
            Util::rest(2000);
        }
        
        world.getEngine()->destroyWorld(world);

        if (force_quit){
            while ( key[ Keyboard::Key_ESC ] ){
                key.poll();
                Util::rest( 1 );
            }
            return false;
        }

	return true;
}

void realGame( const vector< Object * > & players, const string & levelFile ){

	vector< string > levels = Level::readLevels( levelFile );

	// global_debug = true;

	int showHelp = 800;
	for ( vector< string >::iterator it = levels.begin(); it != levels.end(); it++ ){
		Global::done_loading = false;
		pthread_t loading_screen_thread;

		startLoading( &loading_screen_thread );

		bool gameState = false;
		try {
			// vector< Object * > players;
			// players.push_back( player );

                        /*
                        Global::debug(0) << "Memory debug loop" << endl;
                        for (int i = 0; i < 1000; i++){
                            World world( players, *it );
                            ObjectFactory::destroy();
                            HeartFactory::destroy();
                        }
                        */

			World world( players, *it );

			Music::pause();
			Music::fadeIn( 0.3 );
			Music::loadSong( Util::getFiles( Util::getDataPath() + "/music/", "*" ) );
			Music::play();

			for ( vector< Object * >::const_iterator it = players.begin(); it != players.end(); it++ ){
				Player * playerX = (Player *) *it;
				playerX->setY( 200 );
				/* setMoving(false) sets all velocities to 0 */
				playerX->setMoving( false );
				/* but the player is falling so set it back to true */
				playerX->setMoving( true );

				playerX->setStatus( Status_Falling );
			}

			stopLoading( loading_screen_thread );

                        gameState = playLevel( world, players, showHelp );
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

		// fadeOut( "Next level" );
	}

        /* fix.. */
	// fadeOut( "You win!" );
}

const string selectLevelSet( const string & base ) throw( ReturnException ){
        Bitmap background( Global::titleScreen() );
	// Bitmap::Screen->Blit( Global::titleScreen() );

	// Bitmap background( Util::getDataPath() + "/paintown-title.png" );
	int fontY = 20;
	const Font & font = Font::getFont( Util::getDataPath() + DEFAULT_FONT, 20, fontY );
	vector<string> possible = Util::getFiles( base + "/", "*.txt" );
        for ( vector<string>::iterator it = possible.begin(); it != possible.end(); it++ ){
            string & path = *it;
            path.erase(0, Util::getDataPath().length() + 1);
        }

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

	font.printf( 180, (int)(200 - fontY * 1.2), Bitmap::makeColor( 255, 255, 255 ), background, "Select a set of levels to play", 0 );
	for ( unsigned int i = 0; i < possible.size(); i++ ){
		int yellow = Bitmap::makeColor( 255, 255, 0 );
		int white = Bitmap::makeColor( 255, 255, 255 );
		unsigned int color = i == (unsigned) choose ? yellow : white;
		font.printf( 200, (int)(200 + i * fontY * 1.2), color, background, possible[ i ], 0 );
	}
        background.BlitToScreen();

	Keyboard key;
	bool done = false;

	key.setDelay( Keyboard::Key_UP, LAZY_KEY_DELAY );
	key.setDelay( Keyboard::Key_DOWN, LAZY_KEY_DELAY );
	Global::speed_counter = 0;

	key.wait();

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
					return Util::getDataPath() + "/" + possible[ choose ];
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
				font.printf( 200, (int)(200 + i * fontY * 1.2), color, background, possible[ i ], 0 );
			}
                        background.BlitToScreen();
		}
		
		while ( Global::speed_counter == 0 ){
			Util::rest( 1 );
			key.poll();
		}
	}

	return "nothing-selected";
}

void fadeOut( Bitmap & work, const string & message ){
	Bitmap dark( GFX_X, GFX_Y );
	dark.clear();
	Bitmap::transBlender( 0, 0, 0, 128 );

	dark.drawTrans( 0, 0, work );
	
	const Font & f = Font::getFont( Util::getDataPath() + DEFAULT_FONT, 50, 50 );
	f.printf( 200, 200, Bitmap::makeColor( 255, 0, 0 ), work, message, 0 );
        work.BlitToScreen();

	Util::rest( 2000 );
}

void playVersusMode( Character * player1, Character * player2, int round ) throw( ReturnException ){

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
	double gameSpeed = startingGameSpeed();

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
				runCounter += Global::speed_counter * gameSpeed * Global::LOGIC_MULTIPLIER;

				while ( runCounter >= 1.0 ){
					draw = true;
					world.act();
					runCounter -= 1.0;

					if ( player1->getHealth() <= 0 || player2->getHealth() <= 0 ){
						if ( player1->getHealth() <= 0 && player2->getHealth() > 0 ){
							fadeOut( screen_buffer, "Player 2 wins!" );
						} else if ( player1->getHealth() > 0 && player2->getHealth() <= 0 ){
							fadeOut( screen_buffer, "Player 1 wins!" );
						} else {
							fadeOut( screen_buffer, "Draw!" );
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
				Global::debug( 3 ) << "Game speed " << gameSpeed << endl;
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
				Global::debug( 3 ) << "Game speed " << gameSpeed << endl;
			}

			if ( key[ Keyboard::Key_ENTER_PAD ] ){
				gameSpeed = 1;
				Global::debug( 3 ) << "Game speed " << gameSpeed << endl;
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
			
			/*

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
	
			/ * split is the number of pixels to show in the Y direction * /
			int split = screen_buffer.getHeight() * (max_x - min_x) / screen_buffer.getWidth();
			/ * cut the difference into two pieces, min_y and max_y * /
			min_y = (screen_buffer.getHeight() - split);
			// max_y -= (screen_buffer.getHeight() - split) / 2;

			// work.Stretch( screen_buffer, min_x, min_y, max_x - min_x, max_y - min_y, 0, 0, screen_buffer.getWidth(), screen_buffer.getHeight() );
			
			*/

			// work.Blit( screen_buffer );
			const double divider = 5;
			const double x_distance = screen_buffer.getWidth() / divider;
			double min_x_1 = player1->getX() - x_distance;
			double max_x_1 = player1->getX() + x_distance;
			double min_x_2 = player2->getX() - x_distance;
			double max_x_2 = player2->getX() + x_distance;

			if ( min_x_1 < 0 ){
				max_x_1 += 0 - min_x_1;
				min_x_1 = 0;
			}
			if ( max_x_1 > screen_buffer.getWidth() ){
				min_x_1 -= max_x_1 - screen_buffer.getWidth();
				max_x_1 = screen_buffer.getWidth();
			}
			
			if ( min_x_2 < 0 ){
				max_x_2 += 0 - min_x_2;
				min_x_2 = 0;
			}
			if ( max_x_2 > screen_buffer.getWidth() ){
				min_x_2 -= max_x_2 - screen_buffer.getWidth();
				max_x_2 = screen_buffer.getWidth();
			}

			if ( (min_x_1 < min_x_2 && max_x_1 > min_x_2) ||
			     (min_x_2 < min_x_1 && max_x_2 > min_x_1) ||
			     (min_x_1 == min_x_2) ){
			     /* the players are close enough together to show
			      * them in the same screen
			      */

				double space = x_distance * 4 - fabs( player1->getX() - player2->getX() );
				double p1 = player1->getX() < player2->getX() ? player1->getX() : player2->getX();
				double p2 = player1->getX() >= player2->getX() ? player1->getX() : player2->getX();
				double x1 = p1 - space / 2;
				double x2 = p2 + space / 2;

				if ( x2 > screen_buffer.getWidth() ){
					x1 -= x2 - screen_buffer.getWidth();
					x2 = screen_buffer.getWidth();
				} else if ( x1 < 0 ){
					x2 += 0 - x1;
					x1 = 0;
				}
				

				/*
				int y1 = (int)(distance / 2 - screen_buffer.getHeight() / divider);
				int y2 = plane + screen_buffer.getHeight();
				*/
				double visible = screen_buffer.getHeight() * (divider - 1) / divider;
				double y1 = world.getMinimumZ() - visible / 2;
				double y2 = world.getMinimumZ() + visible / 2;
				if ( y1 < 0 ){
					y2 += - y1;
					y1 = 0;
				} else if ( y2 > screen_buffer.getHeight() ){
					y1 -= y2 - screen_buffer.getHeight();
					y2 = screen_buffer.getHeight();
				}

				work.Stretch( screen_buffer, (int)x1, (int)y1, (int)(x2 - x1), (int)(y2 - y1), 0, 0, screen_buffer.getWidth(), screen_buffer.getHeight() );

			} else {
				/* split screen */

				/*
				int y1 = (int)(screen_buffer.getHeight() / divider);
				int y2 = screen_buffer.getHeight();
				*/

				double visible = screen_buffer.getHeight() * (divider - 1) / divider;
				double y1 = world.getMinimumZ() - visible / 2;
				double y2 = world.getMinimumZ() + visible / 2;
				if ( y1 < 0 ){
					y2 += - y1;
					y1 = 0;
				} else if ( y2 > screen_buffer.getHeight() ){
					y1 -= y2 - screen_buffer.getHeight();
					y2 = screen_buffer.getHeight();
				}
				int p = player1->getX() < player2->getX() ? 0 : screen_buffer.getWidth() / 2;
				work.Stretch( screen_buffer, (int)min_x_1, (int)y1, (int)(max_x_1 - min_x_1), (int)(y2 - y1), p, 0, screen_buffer.getWidth() / 2, screen_buffer.getHeight() );
				work.Stretch( screen_buffer, (int)min_x_2, (int)y1, (int)(max_x_2 - min_x_2), (int)(y2 - y1), screen_buffer.getWidth() / 2 - p, 0, screen_buffer.getWidth() / 2, screen_buffer.getHeight() );
			}
			     

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
			screen_buffer.BlitToScreen( world.getX(), world.getY() );

			if ( key[ Keyboard::Key_F12 ] ){
				Global::debug( 2 ) << "Saved screenshot to scr.bmp" << endl;
				work.save( "scr.bmp" );
			}

			work.clear();
		}

		// done |= key[ Keyboard::Key_ESC ] || world.finished();
		done = world.finished();
	}
}

}
