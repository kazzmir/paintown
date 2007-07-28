#include "util/bitmap.h"
#include "object/object.h"
#include "util/load_exception.h"
#include "util/funcs.h"
#include "object/player.h"
#include "init.h"
#include "select_player.h"
#include "factory/font_factory.h"
#include "util/font.h"
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
	Bitmap preview( GFX_X / 2, GFX_Y / 2 );

	const int unselectedColor = Bitmap::makeColor( 255, 0, 0 );
	const int selectedColor = Bitmap::makeColor( 0, 255, 0 );
	
	Global::speed_counter = 0;
			
	const int boxSize = 80;
	const int startX = 300;
	const int startY = 20;
	const int boxesPerLine = (work.getWidth() - startX) / (boxSize + 10);
	const int boxesPerColumn = (work.getHeight() - startY) / (boxSize + 10);
	int backgroundX = 0;
	int top = 0;

	const int maxGradient = 50;
	int gradient[ maxGradient ];	
	/* fade from yellow to some reddish color */
	Util::blend_palette( gradient, 25, Bitmap::makeColor( 255, 255, 0 ), Bitmap::makeColor( 0xff, 0x33, 0x11 ) );
	Util::blend_palette( gradient + 25, 25, Bitmap::makeColor( 0xff, 0x33, 0x11 ), Bitmap::makeColor( 255, 255, 0 ) );

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
					current = current - 1;
				}

				if ( key[ Keyboard::Key_RIGHT ] ){
					current = current + 1;
				}

				if ( key[ Keyboard::Key_UP ] ){
					current = current - boxesPerLine;
				}

				if ( key[ Keyboard::Key_DOWN ] ){
					current = current + boxesPerLine;
				}

				if ( current < 0 ){
					current = 0;
				}

				if ( current >= (signed) players.size() ){
					current = players.size() - 1;
				}

				if ( ch->testAnimation() ){
					ch->testReset();
				}

				while ( current < top ){
					top -= boxesPerLine;
				}

				while ( current >= top + boxesPerLine * boxesPerColumn ){
					top += boxesPerLine;
				}

				think--;
			}

			Global::speed_counter = 0;
			draw = true;
		}

		if ( draw ){

			// background.Stretch( work );
			background.Blit( backgroundX, 0, work );
			background.Blit( work.getWidth() + backgroundX, 0, work );

			ch->setFacing( Object::FACING_RIGHT );
			Character copy( *ch );
			copy.setX( preview.getWidth() / 2 );
			copy.setY( 0 );
			copy.setZ( preview.getHeight() - 20 );
			preview.fill( Bitmap::MaskColor );

			copy.draw( &preview, 0 );
			preview.drawStretched( -GFX_X / 2 + startX / 2, 0, GFX_X, GFX_Y, work );
			const Font & font = Font::getFont( Util::getDataPath() + "/fonts/arial.ttf" );
			font.printf( 10, 10, Bitmap::makeColor( 255, 255, 255 ), work, copy.getName(), 0 );

			int x = startX, y = startY;
			unsigned int i;
			for ( i = top; i < players.size() && y + boxSize < GFX_Y; i++ ){
				temp.clear();
				Bitmap box( work, x, y, boxSize, boxSize );
				int color = unselectedColor;
				Character small( *(players[ i ]) );

				color = i == (unsigned int) current ? selectedColor : unselectedColor;
				/* draw a border */
				box.border( 0, 3, color );

				small.setX( temp.getWidth() / 2 );
				small.setY( 0 );
				small.setZ( temp.getHeight() );
				small.draw( &temp, 0 );
				temp.drawStretched( 0, 0, box.getWidth(), box.getHeight(), box );
				box.border( 0, 3, color );
				x += boxSize + 10;
				if ( x + boxSize + 10 > work.getWidth() ){
					x = startX;
					y += (boxSize + 10);
				}
			}

			if ( top > 0 ){
				int x1 = 80;
				int x2 = 140;
				work.triangle( startX + x1, 8, startX + x2, 8, startX + (x1 + x2) / 2, 3, gradient[ clock % maxGradient ] );
			}

			if ( i < players.size() ){
				int x1 = 80;
				int x2 = 140;
				work.triangle( startX + x1, GFX_Y - 8, startX + x2, GFX_Y - 8, startX + (x1 + x2) / 2, GFX_Y - 3, gradient[ clock % maxGradient ] );
			}

			work.Blit( *Bitmap::Screen );
			draw = false;
		}

		while ( Global::speed_counter == 0 ){
			key.poll();
			Util::rest( 1 );
		}
	}

	/* delete everything but the selected player */
	for ( unsigned int i = 0; i < players.size(); i++ ){	
		if ( i != (unsigned int) current ){
			delete players[ i ];
		}
	}

	Player * player = players[ current ];
	player->setInvincible( invincibile );
	return player;
}
