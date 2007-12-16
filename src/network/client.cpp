#include "client.h"
#include "util/keyboard.h"
#include "init.h"
#include "globals.h"
#include "util/funcs.h"
#include "util/font.h"
#include <string>

using namespace std;

namespace Network{

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

static void handleHostInput( string & str, const vector< int > & keys ){
	for ( vector< int >::const_iterator it = keys.begin(); it != keys.end(); it++ ){
		const int & key = *it;
		if ( Keyboard::isAlpha( key ) || key == Keyboard::Key_STOP ){
			str += lowerCase( Keyboard::keyToName( key ) );
		} else if ( key == Keyboard::Key_BACKSPACE ){
			if ( str != "" ){
				str = str.substr( 0, str.length() - 1 );
			}
		}
	}
}

static void handlePortInput( string & str, const vector< int > & keys ){
	for ( vector< int >::const_iterator it = keys.begin(); it != keys.end(); it++ ){
		const int & key = *it;
		if ( Keyboard::isNumber( key ) || key == Keyboard::Key_STOP ){
			str += Keyboard::keyToName( key );
		} else if ( key == Keyboard::Key_BACKSPACE ){
			if ( str != "" ){
				str = str.substr( 0, str.length() - 1 );
			}
		}
	}
}

void networkClient(){
	Global::showTitleScreen();
	Global::speed_counter = 0;
	Keyboard keyboard;
	keyboard.setAllDelay( 200 );

	string host = "localhost";
	string port = "7887";

	enum Focus{
		HOST, PORT, CONNECT, BACK
	};

	const Font & font = Font::getFont( Util::getDataPath() + Global::DEFAULT_FONT, 20, 20 );
	const int inputBoxLength = font.textLength( "a" ) * 30;
	
	font.printf( 20, 100 - font.getHeight() - 1, Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "Host", 0 );
	Bitmap hostBox( *Bitmap::Screen, 20, 100, inputBoxLength, font.getHeight() );
	Bitmap copyHostBox( hostBox.getWidth(), hostBox.getHeight() );
	hostBox.Blit( copyHostBox );

	font.printf( 20, 100 + font.getHeight() * 2 - font.getHeight() - 1, Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "Port", 0 );
	Bitmap portBox( *Bitmap::Screen, 20, 100 + font.getHeight() * 2, inputBoxLength, font.getHeight() );
	Bitmap copyPortBox( portBox.getWidth(), portBox.getHeight() );
	portBox.Blit( copyPortBox );
			
	font.printf( 20, 20, Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "Press TAB to cycle the next input", 0 );

	Focus focus = HOST;

	bool done = false;
	bool draw = true;
	while ( ! done ){
		int think = Global::speed_counter;
		while ( think > 0 ){
			think -= 1;
			Global::speed_counter = 0;
			keyboard.poll();

			if ( keyboard[ Keyboard::Key_TAB ] ){
				draw = true;
				switch ( focus ){
					case HOST : focus = PORT; break;
					case PORT : focus = CONNECT; break;
					case CONNECT : focus = BACK; break;
					case BACK : focus = HOST; break;
					default : focus = HOST;
				}
			}

			if ( keyboard[ Keyboard::Key_ESC ] ){
				done = true;
			}

			if ( keyboard[ Keyboard::Key_ENTER ] ){
				if ( focus == BACK ){
					done = true;
				}
			}

			vector< int > keys;
			keyboard.readKeys( keys );
			switch ( focus ){
				case HOST : {
					draw = true;
					handleHostInput( host, keys );
					break;
				}
				case PORT : {
					draw = true;
					handlePortInput( port, keys );
					break;
				}
				default : {
					break;
				}
			}
		}

		if ( draw ){
			draw = false;
			int focusColor = Bitmap::makeColor( 255, 255, 0 );
			int unFocusColor = Bitmap::makeColor( 255, 255, 255 );

			drawBox( hostBox, copyHostBox, host, font, focus == HOST );
			drawBox( portBox, copyPortBox, port, font, focus == PORT );
			font.printf( 20, 100 + font.getHeight() * 5, focus == CONNECT ? focusColor : unFocusColor, *Bitmap::Screen, "Connect", 0 );
			font.printf( 20, 100 + font.getHeight() * 6 + 5, focus == BACK ? focusColor : unFocusColor, *Bitmap::Screen, "Back", 0 );
		}

		while ( Global::speed_counter == 0 ){
			Util::rest( 1 );
			keyboard.poll();
		}
	}
}

}
