#include "util/bitmap.h"
#include "network.h"
#include "chat_client.h"
#include "chat.h"
#include "util/bitmap.h"
#include "init.h"
#include "globals.h"
#include "util/keyboard.h"
#include "util/font.h"
#include "util/funcs.h"

static void * serverInput( void * client_ ){
	ChatClient * client = (ChatClient *) client_;
	bool done = false;
	while ( ! done ){
		try{
			Network::Message message( client->getSocket() );
			int x;
			ChatType kind;
			message >> x;
			kind = (ChatType) x;
			switch ( kind ){
				case ADD_MESSAGE : {
					client->addMessage( message.path, 0 );
					break;
				}
				case CHANGE_NAME : {
					int id;
					string name;
					message >> id;
					name = message.path;
					client->changeName( id, name );
					break;
				}
				case REMOVE_BUDDY : {
					int id;
					message >> id;
					client->removeBuddy( id );
					break;
				}
				case START_THE_GAME : {
					/* shut down threads and prepare to play */
					break;
				}
				case ADD_BUDDY : {
					int id;
					string name;
					message >> id;
					name = message.path;
					client->addBuddy( id, name );
					break;
				}
			}
		} catch ( const Network::NetworkException & e ){
			Global::debug( 0 ) << "Input thread died" << endl;
			done = true;
		}
	}

	return NULL;
}

ChatClient::ChatClient( Network::Socket socket, const string & name ):
need_update( true ),
messages( 400, 300 ),
socket( socket ),
focus( INPUT_BOX ){
	background = new Bitmap( Util::getDataPath() + "/paintown-title.png" );
	pthread_mutex_init( &lock, NULL );

	pthread_create( &inputThread, NULL, serverInput, this );

	try{
		Network::Message nameMessage;
		nameMessage.path = name;
		nameMessage << CHANGE_NAME;
		nameMessage.send( socket );
	} catch ( const Network::NetworkException & n ){
		Global::debug( 0 ) << "Could not send username: " << n.getMessage() << endl;
	}
}

bool ChatClient::needToDraw(){
	return need_update;
}

Focus ChatClient::nextFocus( Focus f ){
	switch ( f ){
		case INPUT_BOX : return QUIT;
		case QUIT : return INPUT_BOX;
		default : return INPUT_BOX;
	}
}
	
void ChatClient::addBuddy( int id, const std::string & s ){
	Buddy b;
	b.id = id;
	b.name = s;
	pthread_mutex_lock( &lock );
	buddies.push_back( b );
	needUpdate();
	pthread_mutex_unlock( &lock );
}
	
void ChatClient::changeName( int id, const std::string & s ){
	pthread_mutex_lock( &lock );
	for ( vector< Buddy >::iterator it = buddies.begin(); it != buddies.end(); it++ ){
		Buddy & b = *it;
		if ( b.id == id ){
			b.name = s;
		}
	}
	needUpdate();
	pthread_mutex_unlock( &lock );
}

void ChatClient::removeBuddy( int id ){
	pthread_mutex_lock( &lock );
	for ( vector< Buddy >::iterator it = buddies.begin(); it != buddies.end(); ){
		const Buddy & b = *it;
		if ( b.id == id ){
			it = buddies.erase( it );
		} else {
			it ++;
		}
	}
	needUpdate();
	pthread_mutex_unlock( &lock );
}

void ChatClient::addMessage( const string & s, unsigned int id ){
	pthread_mutex_lock( &lock );
	messages.addMessage( s );
	needUpdate();
	pthread_mutex_unlock( &lock );
}

static char lowerCase( const char * x ){
	if ( x[0] >= 'A' && x[0] <= 'Z' ){
		return x[0] - 'A' + 'a';
	}
	return x[0];
}

bool ChatClient::sendMessage( const string & message ){
	try{
		Network::Message net;
		net << ADD_MESSAGE;
		net.path = message;
		net.send( socket );
		return true;
	} catch ( const Network::NetworkException & e ){
		Global::debug( 0 ) << "Client could not send message" << endl;
	}
	return false;
}
	
void ChatClient::popup( Keyboard & key, const std::string & str ){
	const Font & font = Font::getFont( Util::getDataPath() + Global::DEFAULT_FONT, 20, 20 );
	int length = font.textLength( str.c_str() ) + 20;
	int height = font.getHeight() * 2;
	Bitmap area( *Bitmap::Screen, GFX_X / 2 - length / 2, GFX_Y / 2, length, height );
	area.drawingMode( Bitmap::MODE_TRANS );
	area.rectangleFill( 0, 0, area.getWidth(), area.getHeight(), Bitmap::makeColor( 64, 0, 0 ) );
	area.drawingMode( Bitmap::MODE_SOLID );
	int color = Bitmap::makeColor( 255, 255, 255 );
	area.rectangle( 0, 0, area.getWidth() - 1, area.getHeight() - 1, color );
	font.printf( 10, area.getHeight() / 3, Bitmap::makeColor( 255, 255, 255 ), area, str, 0 );

	key.wait();
	key.readKey();
}

void ChatClient::handleInput( Keyboard & keyboard ){
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
			addMessage( "You: " + input, 0 );
			if ( ! sendMessage( input ) ){
				popup( keyboard, "Could not send message" );
			}
			input = "";
			needUpdate();
		}
	}
}

void ChatClient::needUpdate(){
	need_update = true;
}
	
void ChatClient::logic( Keyboard & keyboard ){
	if ( keyboard[ Keyboard::Key_TAB ] ){
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

void ChatClient::drawInputBox( int x, int y, const Bitmap & work ){
	const Font & font = Font::getFont( Util::getDataPath() + Global::DEFAULT_FONT, 20, 20 );

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

void ChatClient::drawBuddies( const Bitmap & area, int x, int y, const Font & font ){
	Bitmap buddyList( area, x, y, GFX_X - x - 5, 200 );
	buddyList.drawingMode( Bitmap::MODE_TRANS );
	Bitmap::transBlender( 0, 0, 0, 128 );
	buddyList.rectangleFill( 0, 0, buddyList.getWidth(), buddyList.getHeight(), Bitmap::makeColor( 0, 0, 0 ) );
	buddyList.drawingMode( Bitmap::MODE_SOLID );
	buddyList.rectangle( 0, 0, buddyList.getWidth() -1, buddyList.getHeight() - 1, Bitmap::makeColor( 255, 255, 255 ) );
	int fy = 1;
	for ( vector< Buddy >::iterator it = buddies.begin(); it != buddies.end(); it++ ){
		Buddy & buddy = *it;
		const string & name = buddy.name;
		font.printf( 1, fy, Bitmap::makeColor( 255, 255, 255 ), buddyList, name, 0 );
		fy += font.getHeight();
	}
}
	
void ChatClient::draw( const Bitmap & work ){
	int start_x = 20;
	int start_y = 20;
	const Font & font = Font::getFont( Util::getDataPath() + Global::DEFAULT_FONT, 20, 20 );
	background->Blit( work );
	messages.draw( start_x, start_y, work, font );
	drawInputBox( start_x, start_y + messages.getHeight() + 5, work );
	drawBuddies( work, start_x + messages.getWidth() + 10, start_y, font );
	need_update = false;
}
	
void ChatClient::killInputThread(){
	Global::debug( 0 ) << "Killing input socket" << endl;
	Network::close( getSocket() );
	Global::debug( 0 ) << "Waiting for input thread to die" << endl;
	pthread_join( inputThread, NULL );
	Global::debug( 0 ) << "Input thread killed" << endl;
}

void ChatClient::run(){
	Global::speed_counter = 0;
	Bitmap work( GFX_X, GFX_Y );
	Keyboard keyboard;

	keyboard.setAllDelay( 200 );
	keyboard.setDelay( Keyboard::Key_TAB, 200 );
	keyboard.setDelay( Keyboard::Key_ESC, 0 );

	bool done = false;
	bool kill = false;
	while ( ! done ){
		int think = Global::speed_counter;
		while ( think > 0 ){
			keyboard.poll();
			logic( keyboard );
			think -= 1;
			Global::speed_counter = 0;
			if ( keyboard[ Keyboard::Key_ESC ] ){
				kill = true;
				done = true;
			}
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

	if ( kill ){
		killInputThread();
	}
}

ChatClient::~ChatClient(){
	delete background;
}

