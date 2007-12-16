#include "network.h"
#include "chat_server.h"
#include "util/bitmap.h"
#include "util/funcs.h"
#include "util/keyboard.h"
#include "util/font.h"
#include "globals.h"
#include "init.h"
#include <iostream>

using namespace std;

static const char * DEFAULT_FONT = "/fonts/arial.ttf";

ChatServer::ChatServer( Network::Socket socket ):
need_update( true ),
socket( socket ),
messages( 400, 350 ),
focus( INPUT_BOX ){
	background = new Bitmap( Util::getDataPath() + "/paintown-title.png" );
}

static char lowerCase( const char * x ){
	if ( x[0] >= 'A' && x[0] <= 'Z' ){
		return x[0] - 'A' + 'a';
	}
	return x[0];
}

void ChatServer::addMessage( const string & s ){
		  messages.addMessage( s );
}

void ChatServer::handleInput( Keyboard & keyboard ){
	vector< int > keys;
	keyboard.readKeys( keys );

	for ( vector< int >::iterator it = keys.begin(); it != keys.end(); it++ ){
		int key = *it;
		if ( key == Keyboard::Key_BACKSPACE ){
			if ( input != "" ){
				input = input.substr( 0, input.length() - 1 );
				needUpdate();
			}
		} else if ( Keyboard::isAlpha( key ) ){
			input += lowerCase( Keyboard::keyToName( key ) );
			needUpdate();
		} else if ( key == Keyboard::Key_SPACE ){
			input += " ";
			needUpdate();
		} else if ( key == Keyboard::Key_ENTER ){
			addMessage( "You: " + input );
			input = "";
			needUpdate();
		}
	}
}

void ChatServer::logic( Keyboard & keyboard ){
	if ( keyboard[ Keyboard::Key_TAB ] ){
		Global::debug( 0 ) << "tab" << endl;
		focus = nextFocus( focus );
		needUpdate();
	}

	switch ( focus ){
		case INPUT_BOX : {
			handleInput( keyboard );
			break;
		}
		case QUIT : {
			break;
		}
	}
}
	
void ChatServer::needUpdate(){
	need_update = true;
}
	
Focus ChatServer::nextFocus( Focus f ){
	switch ( f ){
		case INPUT_BOX : return QUIT;
		case QUIT : return INPUT_BOX;
		default : return INPUT_BOX;
	}
}

bool ChatServer::needToDraw(){
	return need_update;
}

void ChatServer::drawInputBox( int x, int y, const Bitmap & work ){
	const Font & font = Font::getFont( Util::getDataPath() + DEFAULT_FONT, 20, 20 );

	work.drawingMode( Bitmap::MODE_TRANS );
	Bitmap::transBlender( 0, 0, 0, 128 );
	work.rectangleFill( x, y, x + messages.getWidth(), y + font.getHeight() + 1, Bitmap::makeColor( 0, 0, 0 ) );
	work.drawingMode( Bitmap::MODE_SOLID );
	int color = Bitmap::makeColor( 255, 255, 255 );
	if ( focus == INPUT_BOX ){
		color = Bitmap::makeColor( 255, 255, 0 );
	}
	work.rectangle( x, y, x + messages.getWidth(), y + font.getHeight(), color );
	Bitmap input_box( work, x + 1, y, messages.getWidth(), font.getHeight() );
	// font.printf( x + 1, y, Bitmap::makeColor( 255, 255, 255 ), work, input, 0 );
	font.printf( 0, 0, Bitmap::makeColor( 255, 255, 255 ), input_box, input, 0 );

}

void ChatServer::draw( const Bitmap & work ){
	int start_x = 20;
	int start_y = 20;
	const Font & font = Font::getFont( Util::getDataPath() + DEFAULT_FONT, 20, 20 );
	background->Blit( work );
	messages.draw( start_x, start_y, work, font );
		
	drawInputBox( start_x, start_y + messages.getHeight() + 5, work );

	need_update = false;
}
	
void ChatServer::run(){
	Global::speed_counter = 0;
	Bitmap work( GFX_X, GFX_Y );
	Keyboard keyboard;

	keyboard.setAllDelay( 200 );
	keyboard.setDelay( Keyboard::Key_TAB, 200 );

	bool done = false;
	while ( ! done ){
		int think = Global::speed_counter;
		while ( think > 0 ){
			keyboard.poll();
			logic( keyboard );
			think -= 1;
			Global::speed_counter = 0;
			done = keyboard[ Keyboard::Key_ESC ];
		}

		if ( needToDraw() ){
			draw( work );
			work.BlitToScreen();
			work.clear();
		}

		while ( Global::speed_counter == 0 ){
			Util::rest( 1 );
			keyboard.poll();
		}
	}
}
	
ChatServer::~ChatServer(){
	delete background;
}
