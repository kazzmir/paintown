#include "util/bitmap.h"
#include "util/lit_bitmap.h"
#include "object/object.h"
#include "util/load_exception.h"
#include "util/funcs.h"
#include "object/player.h"
#include "globals.h"
#include "object/display_character.h"
#include "init.h"
// #include "select_player.h"
#include "game.h"
#include "util/font.h"
#include "world.h"
#include "return_exception.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

#include "configuration.h"

using namespace std;

struct playerInfo{
	Character * guy;
	string path;
	playerInfo( Character * guy, string path ):
		guy( guy ),
		path( path ){
	}
};

typedef vector<playerInfo> PlayerVector;
static PlayerVector loadPlayers( const char * path ){
	PlayerVector players;
	vector< string > files = Util::getFiles( Util::getDataPath() + "/" + path, "*" );
	std::sort( files.begin(), files.end() );
	for ( vector< string >::iterator it = files.begin(); it != files.end(); it++ ){
		string file = (*it) + "/" + (*it).substr( (*it).find_last_of( '/' ) + 1 ) + ".txt";
		Global::debug( 1 ) << "Checking " << file << endl;
		if ( Util::exists( file ) ){
			Global::debug( 1 ) << "Loading " << file << endl;
			try{
				players.push_back( playerInfo( new DisplayCharacter( file ), file ) );
			} catch ( const LoadException & le ){
				Global::debug( 0 ) << "Could not load " << file << " because " << le.getReason() << endl;
			}
		}
	}
	return players;
}

/*
template< class Key, class Value >
Key getNth( const map< Key, Value > & m, int i ){
	int count = 0;
	for ( typename map<Key, Value>::const_iterator it = m.begin(); it != m.end(); it++ ){
		if ( count == i ){
			return it->first;
		}
		count += 1;
	}
	return m.begin()->first;
}
*/

Object * Game::selectPlayer( bool invincibile, const string & message ) throw( LoadException, ReturnException ){

	Bitmap background( Global::titleScreen() );

	/* hm, it would be nice to cache this I suppose */
	PlayerVector players = loadPlayers( "players/" );
	
	Keyboard key;

	// Bitmap work( GFX_X / 2, GFX_Y / 2 );
	Bitmap work( GFX_X, GFX_Y );

	/* currently selected character */
	int current = 0;

	int changeRemapKey = Keyboard::Key_TAB;

	const int keyRight = Configuration::config( 0 ).getRight();
	const int keyUp = Configuration::config( 0 ).getUp();
	const int keyDown = Configuration::config( 0 ).getDown();
	const int keyLeft = Configuration::config( 0 ).getLeft();

	key.setDelay( keyRight, 300 );
	key.setDelay( keyUp, 300 );
	key.setDelay( keyDown, 300 );
	key.setDelay( keyLeft, 300 );
	key.setDelay( changeRemapKey, 200 );


	/* preview box for each character */
	Bitmap temp( 120, 120 );
	Bitmap preview( GFX_X / 2, GFX_Y / 2 );
	Bitmap reflection( GFX_X / 2, GFX_Y / 2 );
	

	// const int unselectedColor = Bitmap::makeColor( 255, 0, 0 );
	// const int selectedColor = Bitmap::makeColor( 0, 255, 0 );
	
	const int maxColor = 40;
	int selectedGradient[ maxColor ];
	int unselectedGradient[ maxColor ];
	
	Util::blend_palette( unselectedGradient, maxColor / 2, Bitmap::makeColor( 255, 0, 0 ), Bitmap::makeColor( 255, 0, 0 ) );
	Util::blend_palette( unselectedGradient + maxColor / 2, maxColor / 2, Bitmap::makeColor( 255, 0, 0 ), Bitmap::makeColor( 255, 0, 0 ) );
	
	Util::blend_palette( selectedGradient, maxColor / 2, Bitmap::makeColor( 0, 128, 0 ), Bitmap::makeColor( 0, 255, 0 ) );
	Util::blend_palette( selectedGradient + maxColor / 2, maxColor / 2, Bitmap::makeColor( 0, 255, 0 ), Bitmap::makeColor( 0, 128, 0 ) );
	
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
        double runCounter = 0;
        double gameSpeed = 1;
	Sound beep( Util::getDataPath() + "/sounds/beep1.wav" );

	while ( ! key[ Keyboard::Key_ENTER ] && ! key[ Keyboard::Key_SPACE ] ){
		key.poll();

		Character * ch = (Character *) players[ current ].guy;

		if ( Global::speed_counter > 0 ){
			// double think = Global::speed_counter;
                        runCounter += Global::speed_counter * gameSpeed * Global::LOGIC_MULTIPLIER;
			while ( runCounter >= 1.0 ){
                            runCounter -= 1;
				clock += 1;
			
				if ( clock % 5 == 0 ){
					backgroundX -= 1;
					if ( backgroundX < - work.getWidth() ){
						backgroundX = 0;
					}
				}

				if ( key[ keyLeft ] ){
					current = current - 1;
					beep.play();
				}

				if ( key[ keyRight ] ){
					current = current + 1;
					beep.play();
				}

				if ( key[ keyUp ] ){
					current = current - boxesPerLine;
					beep.play();
				}

				if ( key[ keyDown ] ){
					current = current + boxesPerLine;
					beep.play();
				}

				if ( key[ changeRemapKey ] ){
					ch->nextMap();
				}

				if ( key[ Keyboard::Key_ESC ] ){
					for ( PlayerVector::iterator it = players.begin(); it != players.end(); it++ ){
						delete it->guy;
					}
					throw ReturnException();
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

				// think--;
			}

			Global::speed_counter = 0;
			draw = true;
		}

		if ( draw ){

			// background.Stretch( work );
			background.Blit( backgroundX, 0, work );
			background.Blit( work.getWidth() + backgroundX, 0, work );

			const int stand = 50;
			ch->setFacing( Object::FACING_RIGHT );
			Character copy( *ch );
			copy.setDrawShadow( false );
			copy.setX( preview.getWidth() / 2 );
			copy.setY( 0 );
			copy.setZ( preview.getHeight() - stand );
			preview.fill( Bitmap::MaskColor );
			reflection.fill( Bitmap::MaskColor );
			// preview.fill( 0 );
			// reflection.fill( 0 );

			copy.draw( &preview, 0 );
			preview.drawVFlip( 0, 0, reflection );

			Bitmap::transBlender( 0, 0, 0, 255 );
			LitBitmap s2( reflection );
			s2.draw( 0, preview.getHeight() - stand - stand, preview );
			Bitmap::transBlender( 0, 0, 0, 128 );
			reflection.drawTrans( 0, preview.getHeight() - stand - stand, preview );
			copy.draw( &preview, 0 );

			// reflection.drawCharacter( 0, preview.getHeight() - stand - stand, 0, -1, preview );
			// preview.floodfill( 0, 0, Bitmap::MaskColor );
			// preview.drawTransVFlip( 0, preview.getHeight() - stand - stand, preview );

			// preview.draw( 60, 0, work );
			preview.drawStretched( -GFX_X / 2 + startX / 2, 0, GFX_X, GFX_Y, work );

			const Font & font = Font::getFont( Util::getDataPath() + "/fonts/arial.ttf" );
                        for (int c = 1; c >= 0; c--){
                            int color = 255 - c * 190;
                            int x = 10 + 5 * c;
                            int y = font.getHeight() + 5 + c * 5;
                            font.printf( x, y, Bitmap::makeColor(color, color, color ), work, copy.getName(), 0 );
                        }

			font.printf( 10, 10, Bitmap::makeColor( 255, 255, 255 ), work, message, 0 );

			int x = startX, y = startY;
			unsigned int i;
			for ( i = top; i < players.size() && y + boxSize < GFX_Y; i++ ){
				temp.clear();
				Bitmap box( work, x, y, boxSize, boxSize );
				// int color = unselectedColor;
				int * color = 0;
				Character smaller( *players[ i ].guy );

				color = i == (unsigned int) current ? selectedGradient : unselectedGradient;
				/* draw a border */
				// box.border( 0, 3, color[ clock % maxColor ] );

				smaller.setX( temp.getWidth() / 2 );
				smaller.setY( 0 );
				smaller.setZ( temp.getHeight() );
				smaller.draw( &temp, 0 );
				temp.drawStretched( 0, 0, box.getWidth(), box.getHeight(), box );
				box.border( 0, 3, color[ clock % maxColor ] );
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

			work.BlitToScreen();
			draw = false;
		}

		while ( Global::speed_counter == 0 ){
			key.poll();
			Util::rest( 1 );
		}
	}

	int remap = players[ current ].guy->getCurrentMap();

	/* delete all the preview characters. its ok to delete them
	 * before looking up the selected player in the map
	 * because 'delete' doesn't affect the map, it just changes
	 * memory around.
	 */
	for ( PlayerVector::iterator it = players.begin(); it != players.end(); it++ ){
		delete it->guy;
	}

	Global::debug( 1 ) << "Selected " << players[ current ].path << ". Loading.." << endl;
	Player * player = new Player( players[ current ].path );
	player->setInvincible( invincibile );
	player->setMap( remap );
	player->testAnimation();
	return player;
}

vector<Object *> Game::versusSelect( bool invincible ) throw( LoadException, ReturnException ){
	Bitmap background( Global::titleScreen() );

	/* hm, it would be nice to cache this I suppose */
	PlayerVector players = loadPlayers( "players/" );
	
	Keyboard key;

	// Bitmap work( GFX_X / 2, GFX_Y / 2 );
	Bitmap work( GFX_X, GFX_Y );

	/* currently selected character */
	int current1 = 0;
	int current2 = 0;
	vector<int>remapOrig;
	vector<int>remap1;
	vector<int>remap2;
	for( unsigned int i = 0; i < players.size();++i){
		remapOrig.push_back(players[ i ].guy->getCurrentMap());
		remap1.push_back(players[ i ].guy->getCurrentMap());
		remap2.push_back(players[ i ].guy->getCurrentMap());
	}

	key.setDelay( Configuration::config(0).getRight(), 300 );
	key.setDelay( Configuration::config(0).getUp(), 300 );
	key.setDelay( Configuration::config(0).getDown(), 300 );
	key.setDelay( Configuration::config(0).getLeft(), 300 );
	key.setDelay( Configuration::config(0).getAttack1(), 300 );
	key.setDelay( Configuration::config(0).getAttack2(), 300 );
	key.setDelay( Configuration::config(0).getAttack3(), 300 );
	key.setDelay( Configuration::config(0).getJump(), 200 );
	
	key.setDelay( Configuration::config(1).getRight(), 300 );
	key.setDelay( Configuration::config(1).getUp(), 300 );
	key.setDelay( Configuration::config(1).getDown(), 300 );
	key.setDelay( Configuration::config(1).getLeft(), 300 );
	key.setDelay( Configuration::config(1).getAttack1(), 300 );
	key.setDelay( Configuration::config(1).getAttack2(), 300 );
	key.setDelay( Configuration::config(1).getAttack3(), 300 );
	key.setDelay( Configuration::config(1).getJump(), 200 );

	/* preview box for each character */
	Bitmap temp( 120, 120 );
	Bitmap preview1( GFX_X / 2, GFX_Y / 2 );
	Bitmap preview2( GFX_X / 2, GFX_Y / 2 );

	const int unselectedColor = Bitmap::makeColor( 255, 0, 0 );
	const int selectedColor1 = Bitmap::makeColor( 0, 255, 0 );
	const int selectedColor2 = Bitmap::makeColor( 0, 0, 255 );
	
	Global::speed_counter = 0;
			
	const int boxSize = 80;
	const int startX = 180;
	const int startY = 20;
	const int maxSize = 520;
	const int boxesPerLine = (maxSize - startX) / (boxSize + 10);
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
	bool player1Ready = false;
	bool player2Ready = false;
	bool ok = false;
	while ( !ok ){
		key.poll();

		Character * ch1 = players[ current1 ].guy;
		Character * ch2 = players[ current2 ].guy;	
		/*
		for(unsigned int i = 0; i < players.size();++i){
			((Character *) players[ i ].guy)->setMap(remapOrig[i]);
		}
		*/
		if ( Global::speed_counter > 0 ){
			double think = Global::speed_counter;
			while ( think > 0 ){
				clock += 1;

				for ( PlayerVector::iterator it = players.begin(); it != players.end(); it++ ){
					Character * c = it->guy;
					if ( c == ch1 && c == ch2 ){
						if ( remap1[ current1 ] == remap2[ current2 ] ){
							c->setMap( remap1[ current1 ] );
							if ( c->testAnimation() ){
								c->testReset();
							}
						} else {
							c->setMap( remap1[ current1 ] );
							if ( c->testAnimation() ){
								c->testReset();
							}
							c->setMap( remap2[ current2 ] );
							if ( c->testAnimation() ){
								c->testReset();
							}
						}
					} else if ( c == ch1 ){
						c->setMap( remap1[ current1 ] );
						if ( c->testAnimation() ){
							c->testReset();
						}
					} else if ( c == ch2 ){
						c->setMap( remap2[ current2 ] );
						if ( c->testAnimation() ){
							c->testReset();
						}
					}
				}
			
				if ( clock % 5 == 0 ){
					backgroundX -= 1;
					if ( backgroundX < - work.getWidth() ){
						backgroundX = 0;
					}
				}
				
				if( ! player1Ready ){
					if ( key[ Configuration::config(0).getLeft() ] ){
						current1 = current1 - 1;
					}
	
					if ( key[ Configuration::config(0).getRight() ] ){
						current1 = current1 + 1;
					}
	
					if ( key[ Configuration::config(0).getUp() ] ){
						current1 = current1 - boxesPerLine;
					}
	
					if ( key[ Configuration::config(0).getDown() ] ){
						current1 = current1 + boxesPerLine;
					}
	
					if ( key[ Configuration::config(0).getJump() ] ){
						remap1[current1] = ch1->getNextMap( remap1[ current1 ] );
						//ch1->setMap(remapOrig1[current1]);
					}
					
					if ( key[ Configuration::config(0).getAttack1() ] || key[ Configuration::config(0).getAttack2() ] || key[ Configuration::config(0).getAttack3() ] ){
						player1Ready = true;
					}
	
					if ( current1 < 0 ){
						current1 = 0;
					}
	
					if ( current1 >= (signed) players.size() ){
						current1 = players.size() - 1;
					}
	
					/*
					if ( ch1->testAnimation() ){
						ch1->testReset();
					}
					*/
	
					while ( current1 < top ){
						top -= boxesPerLine;
					}
	
					while ( current1 >= top + boxesPerLine * boxesPerColumn ){
						top += boxesPerLine;
					}
				}
				
				if ( ! player2Ready ){
					if ( key[ Configuration::config(1).getLeft() ] ){
						current2 = current2 - 1;
					}
	
					if ( key[ Configuration::config(1).getRight() ] ){
						current2 = current2 + 1;
					}
	
					if ( key[ Configuration::config(1).getUp() ] ){
						current2 = current2 - boxesPerLine;
					}
	
					if ( key[ Configuration::config(1).getDown() ] ){
						current2 = current2 + boxesPerLine;
					}
	
					if ( key[ Configuration::config(1).getJump() ] ){
						// ch2->nextMap( remap2[ current2 ] );
						remap2[current2] = ch2->getNextMap( remap2[ current2 ] );
						// remap2[current2] = ch2->getCurrentMap();
						//ch2->setMap(remapOrig2[current2]);
					}
					
					if ( key[ Configuration::config(1).getAttack1() ] || key[ Configuration::config(1).getAttack2() ] || key[ Configuration::config(1).getAttack3() ] ){
						player2Ready = true;
					}
	
					if ( current2 < 0 ){
						current2 = 0;
					}
	
					if ( current2 >= (signed) players.size() ){
						current2 = players.size() - 1;
					}
	
					/*
					if ( ch2->testAnimation() ){
						ch2->testReset();
					}
					*/
	
					while ( current2 < top ){
						top -= boxesPerLine;
					}
	
					while ( current2 >= top + boxesPerLine * boxesPerColumn ){
						top += boxesPerLine;
					}
				}

				if ( key[ Keyboard::Key_ESC ] ){
					for ( PlayerVector::iterator it = players.begin(); it != players.end(); it++ ){
						delete it->guy;
					}
					throw ReturnException();
				}

				think--;
			}
			
			if ( player1Ready && player2Ready ){
				ok = true;
			}

			Global::speed_counter = 0;
			draw = true;
		}

		if ( draw ){

			// background.Stretch( work );
			background.Blit( backgroundX, 0, work );
			background.Blit( work.getWidth() + backgroundX, 0, work );

			ch1->setMap(remap1[current1]);
			ch1->setFacing( Object::FACING_RIGHT );
			Character copy1( *ch1 );
			copy1.setX( preview1.getWidth() / 2 );
			copy1.setY( 0 );
			copy1.setZ( preview1.getHeight() - 20 );
			preview1.fill( Bitmap::MaskColor );

			copy1.draw( &preview1, 0 );
			preview1.drawStretched( -GFX_X / 2 + startX / 2, 0, GFX_X, GFX_Y, work );
			const Font & font = Font::getFont( Util::getDataPath() + "/fonts/arial.ttf" );
			font.printf( 10, 10, Bitmap::makeColor( 255, 255, 255 ), work, copy1.getName(), 0 );
			
			ch2->setMap(remap2[current2]);
			ch2->setFacing( Object::FACING_LEFT );

			Character copy2( *ch2 );
			copy2.setX( preview2.getWidth() / 2 );
			copy2.setY( 0 );
			copy2.setZ( preview2.getHeight() - 20 );
			preview2.fill( Bitmap::MaskColor );
			
			copy2.draw( &preview2, 0 );
			preview2.drawStretched( 200, 0, GFX_X, GFX_Y, work );
			font.printf( GFX_Y - 30, 10, Bitmap::makeColor( 255, 255, 255 ), work, copy2.getName(), 0 );
			
			int x = startX, y = startY;
			unsigned int i;
			for ( i = top; i < players.size() && y + boxSize < GFX_Y; i++ ){
				temp.clear();
				Bitmap box( work, x, y, boxSize, boxSize );
				int color = unselectedColor;
				Character smaller( *players[ i ].guy );
				smaller.setFacing( Object::FACING_RIGHT );
				
				color = unselectedColor;

				if ( i == (unsigned)current1 && player1Ready ){
					temp.fill( Bitmap::darken( selectedColor1, 2.6 ) );
				}
				
				if ( i == (unsigned)current2 && player2Ready ){
					temp.fill( Bitmap::darken( selectedColor2, 2.6 ) );
				}
				
				if( (i == (unsigned)current1) && (i == (unsigned)current2) ) {
					/*
					for ( int corner = 0; corner < 3; corner++ ){
						box.hLine(corner,corner,boxSize-1-corner,selectedColor1);
						box.vLine(corner,corner,boxSize,selectedColor1);
						
						box.hLine(corner+1,boxSize-1-corner,boxSize,selectedColor2);
						box.vLine(corner,boxSize-1-corner,boxSize,selectedColor2);
					}
					*/
					/*
					box.hLine(0,0,boxSize-1,selectedColor1);
					box.hLine(1,boxSize-1,boxSize,selectedColor2);
					box.hLine(1,1,boxSize-2,selectedColor1);
					box.hLine(2,boxSize-2,boxSize,selectedColor2);
					box.hLine(2,2,boxSize-3,selectedColor1);
					box.hLine(3,boxSize-3,boxSize,selectedColor2);
					box.vLine(0,0,boxSize,selectedColor1);
					box.vLine(0,boxSize-1,boxSize,selectedColor2);
					box.vLine(1,1,boxSize-1,selectedColor1);
					box.vLine(1,boxSize-2,boxSize-1,selectedColor2);
					box.vLine(2,2,boxSize-2,selectedColor1);
					box.vLine(2,boxSize-3,boxSize-2,selectedColor2);
					*/
					
					smaller.setX( temp.getWidth() / 2 );
					smaller.setY( 0 );
					smaller.setZ( temp.getHeight() );
					smaller.draw( &temp, 0 );
					temp.drawStretched( 0, 0, box.getWidth(), box.getHeight(), box );
					
					/*
					box.hLine(0,0,boxSize-1,selectedColor1);
					box.hLine(1,boxSize-1,boxSize,selectedColor2);
					box.vLine(0,0,boxSize,selectedColor1);
					box.vLine(0,boxSize-1,boxSize,selectedColor2);
					box.hLine(1,1,boxSize-2,selectedColor1);
					box.hLine(2,boxSize-2,boxSize,selectedColor2);
					box.vLine(1,1,boxSize-1,selectedColor1);
					box.vLine(1,boxSize-2,boxSize-1,selectedColor2);
					box.hLine(2,2,boxSize-3,selectedColor1);
					box.hLine(3,boxSize-3,boxSize,selectedColor2);
					box.vLine(2,2,boxSize-2,selectedColor1);
					box.vLine(2,boxSize-3,boxSize-2,selectedColor2);
					*/

					for ( int corner = 0; corner < 3; corner++ ){
						box.hLine(corner,corner,boxSize-1-corner,selectedColor1);
						box.vLine(corner,corner,boxSize,selectedColor1);
						
						box.hLine(corner+1,boxSize-1-corner,boxSize,selectedColor2);
						box.vLine(corner,boxSize-1-corner,boxSize,selectedColor2);
					}
					
					/*
					font.printf( 5, 5, selectedColor1, box, "1", 0 );
					font.printf( (boxSize / 2) + 10, 5, selectedColor2, box, "2", 0 );
					*/
					
				} else if(i == unsigned(current1)) {
					//color = i == (unsigned int) current1 ? selectedColor : unselectedColor;
					/* draw a border */
					box.border( 0, 3, selectedColor1 );
					
					smaller.setX( temp.getWidth() / 2 );
					smaller.setY( 0 );
					smaller.setZ( temp.getHeight() );
					smaller.draw( &temp, 0 );
					temp.drawStretched( 0, 0, box.getWidth(), box.getHeight(), box );
					box.border( 0, 3, selectedColor1 );
					// font.printf( 5, 5, selectedColor1, box, "1", 0 );
				} else if(i == unsigned(current2)) {
					box.border( 0, 3, selectedColor2 );
					
					smaller.setX( temp.getWidth() / 2 );
					smaller.setY( 0 );
					smaller.setZ( temp.getHeight() );
					smaller.draw( &temp, 0 );
					temp.drawStretched( 0, 0, box.getWidth(), box.getHeight(), box );
					box.border( 0, 3, selectedColor2 );
					// font.printf( 5, 5, selectedColor2, box, "2", 0 );
				} else {
					box.border( 0, 3, color );
					
					smaller.setMap( 0 );
					smaller.setX( temp.getWidth() / 2 );
					smaller.setY( 0 );
					smaller.setZ( temp.getHeight() );
					smaller.draw( &temp, 0 );
					temp.drawStretched( 0, 0, box.getWidth(), box.getHeight(), box );
					box.border( 0, 3, color );
				}
				
				x += boxSize + 10;
				if ( x + boxSize + 10 > maxSize ){
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

			work.BlitToScreen();
			draw = false;
		}

		while ( Global::speed_counter == 0 ){
			key.poll();
			Util::rest( 1 );
		}
	}

	//int remap1 = players[ current1 ].guy->getCurrentMap();
	//int remap2 = players[ current2 ].guy->getCurrentMap();

	/* delete all the preview characters. its ok to delete them
	 * before looking up the selected player in the map
	 * because 'delete' doesn't affect the map, it just changes
	 * memory around.
	 */
	for ( PlayerVector::iterator it = players.begin(); it != players.end(); it++ ){
		delete it->guy;
	}

	vector<Object *> tempVec;
	
	Global::debug( 1 ) << "Selected " << players[ current1 ].path << ". Loading.." << endl;
	Player * temp1 = new Player( players[ current1 ].path );
	temp1->setMap( remap1[current1] );
	temp1->testAnimation();
	temp1->setInvincible( invincible );
	tempVec.push_back(temp1);
	
	Global::debug( 1 ) << "Selected " << players[ current2 ].path << ". Loading.." << endl;
	Player * temp2 = new Player( players[ current2 ].path );
	temp2->setMap( remap2[current2] );
	temp2->testAnimation();
	temp2->setInvincible( invincible );
	tempVec.push_back(temp2);
	
	return tempVec;
}
