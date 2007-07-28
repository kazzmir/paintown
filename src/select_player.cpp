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

static vector< Player * > loadPlayers( const char * path ){
	vector< Player * > players;
	vector< string > files = Util::getFiles( Util::getDataPath() + "/" + path, "*" );
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
	return players;
}

Object * selectPlayer( bool invincibile ) throw( LoadException ){
	Bitmap background( Util::getDataPath() + "/paintown-title.png" );

	/* hm, it would be nice to cache this I suppose */
	vector< Player * > players = loadPlayers( "players/" );
	
	Keyboard key;

	// Bitmap work( GFX_X / 2, GFX_Y / 2 );
	Bitmap work( GFX_X, GFX_Y );

	/* currently selected character */
	int current = 0;

	key.setDelay( Keyboard::Key_RIGHT, 300 );
	key.setDelay( Keyboard::Key_UP, 300 );
	key.setDelay( Keyboard::Key_DOWN, 300 );
	key.setDelay( Keyboard::Key_LEFT, 300 );

	/* preview box for each character */
	Bitmap temp( 120, 120 );

	const int unselectedColor = Bitmap::makeColor( 255, 0, 0 );
	const int selectedColor = Bitmap::makeColor( 0, 255, 0 );
	
	Global::speed_counter = 0;
			
	const int boxSize = 60;
	const int startX = 150;
	const int boxesPerLine = (work.getWidth() - startX) / (boxSize + 10);
	int backgroundX = 0;

	bool draw = true;
	unsigned int clock = 0;
	while ( ! key[ Keyboard::Key_ENTER ] && ! key[ Keyboard::Key_SPACE ] ){
		key.poll();

		Character * ch = (Character *) players[ current ];

		if ( Global::speed_counter > 0 ){
			double think = Global::speed_counter;
			while ( think > 0 ){
				clock += 1;
			
				if ( clock % 5 == 0 ){
					backgroundX -= 1;
					if ( backgroundX < - work.getWidth() ){
						backgroundX = 0;
					}
				}

				if ( key[ Keyboard::Key_LEFT ] ){
					current = (current - 1 + players.size()) % players.size();
				}

				if ( key[ Keyboard::Key_RIGHT ] ){
					current = (current + 1) % players.size();
				}

				if ( key[ Keyboard::Key_UP ] ){
					current = (current - boxesPerLine + players.size()) % players.size();
				}

				if ( key[ Keyboard::Key_DOWN ] ){
					current = (current + boxesPerLine) % players.size();
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

			// background.Stretch( work );
			// background.drawStretched( backgroundX, 0, work.getWidth(), work.getHeight(), work );
			// background.drawStretched( work.getWidth() + backgroundX, 0, work.getWidth(), work.getHeight(), work );
			background.Blit( backgroundX, 0, work );
			background.Blit( work.getWidth() + backgroundX, 0, work );

			ch->setFacing( Object::FACING_RIGHT );
			Character copy( *ch );
			copy.setX( 80 );
			copy.setY( 0 );
			copy.setZ( 210 );

			copy.draw( &work, 0 );

			int x = startX, y = 20;
			for ( unsigned int i = 0; i < 12; i++ ){
				temp.clear();
				if ( i < players.size() ){
					Character small( *(players[ i ]) );
					int color = i == (unsigned int) current ? selectedColor : unselectedColor;
					/* draw a border */
					temp.border( 0, 3, color );
					
					small.setX( temp.getWidth() / 2 );
					small.setY( 0 );
					small.setZ( temp.getHeight() );
					small.draw( &temp, 0 );
				}
				temp.drawStretched( x, y, boxSize, boxSize, work );
				x += boxSize + 10;
				if ( x + boxSize + 10 > work.getWidth() ){
					x = startX;
					y += (boxSize + 10);
				}
			}

			work.Blit( *Bitmap::Screen );
			// work.Stretch( *Bitmap::Screen );
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
