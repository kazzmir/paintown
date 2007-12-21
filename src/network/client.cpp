#include "util/bitmap.h"
#include "client.h"
#include "util/keyboard.h"
#include "init.h"
#include "globals.h"
#include "util/funcs.h"
#include "util/font.h"
#include "world.h"
#include "object/character.h"
#include "select_player.h"
#include "return_exception.h"
#include "chat_client.h"
#include "network.h"
#include <string>
#include <sstream>

using namespace std;

namespace Network{

static void playGame( Socket socket ){
	Character * player = (Character *) selectPlayer( false, "Pick a player" );
	string path = player->getPath();
	path.erase( 0, Util::getDataPath().length() );

	/* send the path of the chosen player */
	Network::Message create;
	create << World::CREATE_CHARACTER;
	create.path = path;
	create.send( socket );

	/* get the id from the server */
	Network::Message myid( socket );
	int type;
	myid >> type;
	if ( type == World::SET_ID ){
		int id;
		myid >> id;
		player->setId( id );
		Global::debug( 0 ) << "Client id is " << id << endl;
	} else {
		Global::debug( 0 ) << "Bogus message, expected SET_ID: " << type << endl;
	}
}

static void drawBox( const Bitmap & area, const Bitmap & copy, const string & str, const Font & font, bool hasFocus ){
	copy.Blit( area );
	area.drawingMode( Bitmap::MODE_TRANS );
	area.rectangleFill( 0, 0, area.getWidth(), area.getHeight(), Bitmap::makeColor( 0, 0, 0 ) );
	area.drawingMode( Bitmap::MODE_SOLID );
	int color = Bitmap::makeColor( 255, 255, 255 );
	if ( hasFocus ){
		color = Bitmap::makeColor( 255, 255, 0 );
	}
	area.rectangle( 0, 0, area.getWidth() - 1, area.getHeight() - 1, color );
	font.printf( 1, 0, Bitmap::makeColor( 255, 255, 255 ), area, str, 0 );
}

static char lowerCase( const char * x ){
	if ( x[0] >= 'A' && x[0] <= 'Z' ){
		return x[0] - 'A' + 'a';
	}
	return x[0];
}

static bool handleNameInput( string & str, const vector< int > & keys ){
	bool cy = false;
	for ( vector< int >::const_iterator it = keys.begin(); it != keys.end(); it++ ){
		const int & key = *it;
		if ( str.length() < 16 && Keyboard::isAlpha( key ) ){
			str += lowerCase( Keyboard::keyToName( key ) );
			cy = true;
		} else if ( key == Keyboard::Key_BACKSPACE ){
			if ( str != "" ){
				str = str.substr( 0, str.length() - 1 );
				cy = true;
			}
		}
	}
	return cy;
}

static bool handleHostInput( string & str, const vector< int > & keys ){
	bool cy = false;
	for ( vector< int >::const_iterator it = keys.begin(); it != keys.end(); it++ ){
		const int & key = *it;
		if ( Keyboard::isAlpha( key ) || key == Keyboard::Key_STOP ){
			str += lowerCase( Keyboard::keyToName( key ) );
			cy = true;
		} else if ( key == Keyboard::Key_BACKSPACE ){
			if ( str != "" ){
				str = str.substr( 0, str.length() - 1 );
				cy = true;
			}
		}
	}
	return cy;
}

static bool handlePortInput( string & str, const vector< int > & keys ){
	bool cy = false;
	for ( vector< int >::const_iterator it = keys.begin(); it != keys.end(); it++ ){
		const int & key = *it;
		if ( Keyboard::isNumber( key ) || key == Keyboard::Key_STOP ){
			str += Keyboard::keyToName( key );
			cy = true;
		} else if ( key == Keyboard::Key_BACKSPACE ){
			if ( str != "" ){
				str = str.substr( 0, str.length() - 1 );
			cy = true;
			}
		}
	}
	return cy;
}

static void popup( const Font & font, const string & message ){
	int length = font.textLength( message.c_str() ) + 20; 
	Bitmap area( *Bitmap::Screen, GFX_X / 2 - length / 2, 220, length, font.getHeight() * 3 );
	area.drawingMode( Bitmap::MODE_TRANS );
	area.rectangleFill( 0, 0, area.getWidth(), area.getHeight(), Bitmap::makeColor( 64, 0, 0 ) );
	area.drawingMode( Bitmap::MODE_SOLID );
	int color = Bitmap::makeColor( 255, 255, 255 );
	area.rectangle( 0, 0, area.getWidth() - 1, area.getHeight() - 1, color );
	font.printf( 10, area.getHeight() / 2, Bitmap::makeColor( 255, 255, 255 ), area, message, 0 );
}

static const char * getANumber(){
	switch( Util::rnd( 10 ) ){
		case 0 : return "0";
		case 1 : return "1";
		case 2 : return "2";
		case 3 : return "3";
		case 4 : return "4";
		case 5 : return "5";
		case 6 : return "6";
		case 7 : return "7";
		case 8 : return "8";
		case 9 : return "9";
		default : return "0";
	}
}


void networkClient(){
	Global::showTitleScreen();
	Global::speed_counter = 0;
	Keyboard keyboard;
	keyboard.setAllDelay( 200 );

	string name = string("player") + getANumber() + getANumber();
	string host = "localhost";
	string port = "7887";

	enum Focus{
		NAME, HOST, PORT, CONNECT, BACK
	};

	const Font & font = Font::getFont( Util::getDataPath() + Global::DEFAULT_FONT, 20, 20 );
	const int inputBoxLength = font.textLength( "a" ) * 30;
	const int min_y = 140;

	font.printf( 20, min_y - font.getHeight() * 3 - 1, Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "Name", 0 );
	Bitmap nameBox( *Bitmap::Screen, 20, min_y - font.getHeight() * 2, inputBoxLength, font.getHeight() );
	Bitmap copyNameBox( nameBox.getWidth(), nameBox.getHeight() );
	nameBox.Blit( copyNameBox );
	
	font.printf( 20, min_y - font.getHeight() - 1, Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "Host", 0 );
	Bitmap hostBox( *Bitmap::Screen, 20, min_y, inputBoxLength, font.getHeight() );
	Bitmap copyHostBox( hostBox.getWidth(), hostBox.getHeight() );
	hostBox.Blit( copyHostBox );

	font.printf( 20, min_y + font.getHeight() * 2 - font.getHeight() - 1, Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "Port", 0 );
	Bitmap portBox( *Bitmap::Screen, 20, min_y + font.getHeight() * 2, inputBoxLength, font.getHeight() );
	Bitmap copyPortBox( portBox.getWidth(), portBox.getHeight() );
	portBox.Blit( copyPortBox );
			
	font.printf( 20, 20, Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "Press TAB to cycle the next input", 0 );

	Focus focus = NAME;

	bool done = false;
	bool draw = true;
	while ( ! done ){
		int think = Global::speed_counter;
		while ( think > 0 ){
			think -= 1;
			keyboard.poll();

			if ( keyboard[ Keyboard::Key_TAB ] ){
				draw = true;
				switch ( focus ){
					case NAME : focus = HOST; break;
					case HOST : focus = PORT; break;
					case PORT : focus = CONNECT; break;
					case CONNECT : focus = BACK; break;
					case BACK : focus = NAME; break;
					default : focus = HOST;
				}
			}

			if ( keyboard[ Keyboard::Key_ESC ] ){
				throw ReturnException();
			}

			if ( keyboard[ Keyboard::Key_ENTER ] ){
				switch ( focus ){
					case NAME :
					case HOST :
					case PORT : break;
					case CONNECT : {
						done = true;
						try{
							istringstream is( port );
							int porti;
							is >> porti;
							Network::Socket socket = Network::connect( host, porti );
							ChatClient chat( socket, name );
							chat.run();
							if ( chat.isFinished() ){
								playGame( socket );
							}
							Network::close( socket );
						} catch ( const NetworkException & e ){
							popup( font, e.getMessage() );
							keyboard.wait();
							keyboard.readKey();
							Global::showTitleScreen();
							font.printf( 20, min_y - font.getHeight() * 3 - 1, Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "Name", 0 );
							font.printf( 20, min_y - font.getHeight() - 1, Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "Host", 0 );
							font.printf( 20, min_y + font.getHeight() * 2 - font.getHeight() - 1, Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "Port", 0 );
							font.printf( 20, 20, Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "Press TAB to cycle the next input", 0 );
							done = false;
							draw = true;
							think = 0;
						}
						break;
					}
					case BACK : done = true; break;
				}
			}

			vector< int > keys;
			keyboard.readKeys( keys );
			switch ( focus ){
				case HOST : {
					draw = draw || handleHostInput( host, keys );
					break;
				}
				case PORT : {
					draw = draw || handlePortInput( port, keys );
					break;
				}
				case NAME : {
					draw = draw || handleNameInput( name, keys );
					break;
				}
				default : {
					break;
				}
			}
			
			Global::speed_counter = 0;
		}

		if ( draw ){
			draw = false;
			int focusColor = Bitmap::makeColor( 255, 255, 0 );
			int unFocusColor = Bitmap::makeColor( 255, 255, 255 );

			drawBox( nameBox, copyNameBox, name, font, focus == NAME );
			drawBox( hostBox, copyHostBox, host, font, focus == HOST );
			drawBox( portBox, copyPortBox, port, font, focus == PORT );
			font.printf( 20, min_y + font.getHeight() * 5, focus == CONNECT ? focusColor : unFocusColor, *Bitmap::Screen, "Connect", 0 );
			font.printf( 20, min_y + font.getHeight() * 6 + 5, focus == BACK ? focusColor : unFocusColor, *Bitmap::Screen, "Back", 0 );
		}

		while ( Global::speed_counter == 0 ){
			Util::rest( 1 );
			keyboard.poll();
		}
	}
}

}
