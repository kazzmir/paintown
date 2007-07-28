#include "object/object.h"
#include "util/load_exception.h"
#include "util/funcs.h"
#include "object/player.h"
#include "init.h"
#include "select_player.h"
#include "util/bitmap.h"
#include "world.h"
#include <iostream>

using namespace std;

Object * selectPlayer( bool invincibile ) throw( LoadException ){
	Bitmap background( Util::getDataPath() + "/paintown-title.png" );

	/* hm, it would be nice to cache this I suppose */
	vector< Player * > players;

	vector< string > files = Util::getFiles( Util::getDataPath() + "/players/", "*" );
	for ( vector< string >::iterator it = files.begin(); it != files.end(); it++ ){
		string file = (*it) + "/" + (*it).substr( (*it).find_last_of( '/' ) + 1 ) + ".txt";
		cout << "Checking " << file << endl;
		if ( Util::exists( file ) ){
			cout << "Loading " << file << endl;
			try{
				players.push_back( new Player( file ) );
			} catch ( const LoadException & le ){
				cout << "Could not load " << file << " because " << le.getReason() << endl;
			}
		}
	}

	Keyboard key;

	Bitmap work( GFX_X / 2, GFX_Y / 2 );

	Global::speed_counter = 0;

	// vector< Object * > temp;
	// World world;

	int current = 0;

	key.setDelay( Keyboard::Key_RIGHT, 300 );
	key.setDelay( Keyboard::Key_UP, 300 );
	key.setDelay( Keyboard::Key_DOWN, 300 );
	key.setDelay( Keyboard::Key_LEFT, 300 );

	Bitmap temp( 120, 120 );

	const int unselectedColor = Bitmap::makeColor( 255, 0, 0 );
	const int selectedColor = Bitmap::makeColor( 0, 255, 0 );

	bool draw = true;
	while ( ! key[ Keyboard::Key_ENTER ] && ! key[ Keyboard::Key_SPACE ] ){
		key.poll();

		Character * ch = (Character *) players[ current ];

		if ( Global::speed_counter > 0 ){
			double think = Global::speed_counter;
			while ( think > 0 ){

				if ( key[ Keyboard::Key_LEFT ] ){
					current = (current - 1 + players.size()) % players.size();
				}

				if ( key[ Keyboard::Key_RIGHT ] ){
					current = (current + 1) % players.size();
				}

				if ( key[ Keyboard::Key_UP ] ){
					current = (current - 3 + players.size()) % players.size();
				}

				if ( key[ Keyboard::Key_DOWN ] ){
					current = (current + 3) % players.size();
				}

				if ( ch->testAnimation() ){
					ch->testReset();
				}

				think--;
			}

			Global::speed_counter = 0;
			draw = true;
		}

		if ( draw ){

			ch->setFacing( Object::FACING_RIGHT );
			Character copy( *ch );
			copy.setX( 80 );
			copy.setY( 0 );
			copy.setZ( 210 );

			background.Stretch( work );
			copy.draw( &work, 0 );

			int startX = 150;
			int x = startX, y = 20;
			for ( unsigned int i = 0; i < 12; i++ ){
				temp.clear();
				if ( i < players.size() ){
					Character small( *(players[ i ]) );
					int color = i == (unsigned int) current ? selectedColor : unselectedColor;
					/* draw a border */
					temp.border( 0, 3, color );
					/*
					for ( int box = 0; box < 3; box++ ){
						temp.rectangle( box, box, temp.getWidth() - 1 - box, temp.getHeight() - 1 - box, color );
					}
					*/
					small.setX( temp.getWidth() / 2 );
					small.setY( 0 );
					small.setZ( temp.getHeight() );
					small.draw( &temp, 0 );
				}
				temp.drawStretched( x, y, 40, 40, work );
				x += 50;
				if ( x > startX + 50 * 2 ){
					x = startX;
					y += 50;
				}
			}

			work.Stretch( *Bitmap::Screen );
			draw = false;
		}

		while ( Global::speed_counter == 0 ){
			key.poll();
			Util::rest( 1 );
		}
	}

	// Object * v = new Player( *(Character *)*current1 );
	for ( unsigned int i = 0; i < players.size(); i++ ){	
		if ( i != (unsigned int) current ){
			delete players[ i ];
		}
	}

	Player * player = players[ current ];
	player->setInvincible( invincibile );
	return player;
}
