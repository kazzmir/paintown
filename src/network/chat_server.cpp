#include "network.h"
#include "chat_server.h"
#include "chat.h"
#include "util/bitmap.h"
#include "util/funcs.h"
#include "util/keyboard.h"
#include "util/font.h"
#include "globals.h"
#include "init.h"
#include <iostream>

#include <signal.h>

using namespace std;

static const char * DEFAULT_FONT = "/fonts/arial.ttf";

Client::Client( Network::Socket socket, ChatServer * server, unsigned int id ):
socket( socket ),
server( server ),
id( id ),
alive( true ){
	pthread_mutex_init( &lock, NULL );
}
	
Client::~Client(){
}
	
string Client::getName(){
	string s;
	pthread_mutex_lock( &lock );
	s = name;
	pthread_mutex_unlock( &lock );
	return s;
}

void Client::kill(){
	pthread_mutex_lock( &lock );
	alive = false;
	pthread_mutex_unlock( &lock );
}

bool Client::isAlive(){
	bool b;
	pthread_mutex_lock( &lock );
	b = alive;
	pthread_mutex_unlock( &lock );
	return b;
}

void Client::setName( const std::string & s ){
	pthread_mutex_lock( &lock );
	name = s;
	pthread_mutex_unlock( &lock );
}

static void * clientInput( void * client_ ){
	Client * client = (Client *) client_;
	bool done = false;
	while ( ! done ){
		try{
			Network::Message message( client->getSocket() );
			Global::debug( 1 ) << "Got a message: '" << message.path << "'" << endl;
			int type;
			message >> type;
			switch ( type ){
				case CHANGE_NAME : {
					client->setName( message.path );
					client->getServer()->needUpdate();
					break;
				}
				case ADD_MESSAGE : {
					client->getServer()->addMessage( client->getName() + ":" + message.path, client->getId() );
					break;
				}
			}
			done = ! client->isAlive();
		} catch ( const Network::NetworkException & e ){
			Global::debug( 0 ) << "Client input " << client->getId() << " died" << endl;
			done = true;
		}
	}
	
	if ( client->isAlive() ){
		client->getServer()->killClient( client );
	}

	return NULL;
}

static void * clientOutput( void * client_ ){
	Client * client = (Client *) client_;
	bool done = false;
	while ( ! done ){
		string message;
		done = ! client->isAlive();
		if ( client->getOutgoing( message ) != false ){
			Global::debug( 0 ) << "Sending a message to " << client->getId() << endl;
			try{
				Network::Message net;
				net.path = message;
				net.send( client->getSocket() );
			} catch ( const Network::NetworkException & e ){
				Global::debug( 0 ) << "Client output " << client->getId() << " died" << endl;
				done = true;
			}
		} else {
			Util::rest( 1 );
		}
	}

	if ( client->isAlive() ){
		client->getServer()->killClient( client );
	}

	return NULL;
}
	
bool Client::getOutgoing( string & s ){
	string message;
	bool has;
	pthread_mutex_lock( &lock );
	has = ! outgoing.empty();
	if ( has ){
		s = outgoing.front();
		outgoing.erase( outgoing.begin() );
	}
	pthread_mutex_unlock( &lock );
	return has;
}

void Client::addOutputMessage( const std::string & s ){
	pthread_mutex_lock( &lock );
	outgoing.push_back( s );
	pthread_mutex_unlock( &lock );
}

void Client::startThreads(){
	pthread_create( &inputThread, NULL, clientInput, this );
	pthread_create( &outputThread, NULL, clientOutput, this );
}

static void * acceptConnections( void * server_ ){
	bool done = false;
	ChatServer * server = (ChatServer *) server_;
	Network::Socket socket = server->getSocket();
	while ( ! done ){
		try{
			server->addConnection( Network::accept( socket ) );
		} catch ( const Network::NoConnectionsPendingException & e ){
		} catch ( const Network::NetworkException & e ){
			Global::debug( 0 ) << "Error accepting connections: " << e.getMessage() << endl;
		}
		Util::rest( 1 );
	}

	return NULL;
}

ChatServer::ChatServer( Network::Socket socket ):
need_update( true ),
socket( socket ),
messages( 400, 350 ),
focus( INPUT_BOX ),
client_id( 1 ){
	background = new Bitmap( Util::getDataPath() + "/paintown-title.png" );

	Network::listen( socket );
	pthread_mutex_init( &lock, NULL );
	pthread_create( &acceptThread, NULL, acceptConnections, this );
}

void ChatServer::addConnection( Network::Socket s ){
	pthread_mutex_lock( &lock );
	Client * client = new Client( s, this, clientId() );
	Global::debug( 0 ) << "Adding client " << client->getId() << endl;
	clients.push_back( client );
	pthread_mutex_unlock( &lock );
	client->startThreads();
}

static char lowerCase( const char * x ){
	if ( x[0] >= 'A' && x[0] <= 'Z' ){
		return x[0] - 'A' + 'a';
	}
	return x[0];
}

void ChatServer::addMessage( const string & s, unsigned int id ){
	pthread_mutex_lock( &lock );
	messages.addMessage( s );
	for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++ ){
		Client * c = *it;
		if ( c->getId() != id ){
			c->addOutputMessage( s );
		}
	}
	needUpdate();
	pthread_mutex_unlock( &lock );
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
			addMessage( "Server: " + input, 0 );
			input = "";
			needUpdate();
		}
	}
}
	
void ChatServer::killClient( Client * c ){
	pthread_mutex_lock( &lock );
	for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); ){
		Client * client = *it;
		if ( client == c ){
			Global::debug( 0 ) << "Killing socket" << endl;
			c->kill();
			Network::close( c->getSocket() );
			Global::debug( 0 ) << "Waiting for input thread to die" << endl;
			pthread_join( c->getInputThread(), NULL );
			Global::debug( 0 ) << "Waiting for output thread to die" << endl;
			pthread_join( c->getOutputThread(), NULL );
			Global::debug( 0 ) << "Deleting client" << endl;
			delete client;
			it = clients.erase( it );
		} else {
			it++;
		}
	}
	needUpdate();
	pthread_mutex_unlock( &lock );
}

void ChatServer::logic( Keyboard & keyboard ){
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

void ChatServer::drawBuddyList( int x, int y, const Bitmap & work, const Font & font ){
	Bitmap buddyList( work, x, y, GFX_X - x - 5, 200 );
	buddyList.drawingMode( Bitmap::MODE_TRANS );
	Bitmap::transBlender( 0, 0, 0, 128 );
	buddyList.rectangleFill( 0, 0, buddyList.getWidth(), buddyList.getHeight(), Bitmap::makeColor( 0, 0, 0 ) );
	buddyList.drawingMode( Bitmap::MODE_SOLID );
	buddyList.rectangle( 0, 0, buddyList.getWidth() -1, buddyList.getHeight() - 1, Bitmap::makeColor( 255, 255, 255 ) );
	int fy = 1;
	for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++ ){
		Client * client = *it;
		const string & name = client->getName();
		font.printf( 1, fy, Bitmap::makeColor( 255, 255, 255 ), buddyList, name, 0 );
		fy += font.getHeight();
	}
}

void ChatServer::draw( const Bitmap & work ){
	int start_x = 20;
	int start_y = 20;
	const Font & font = Font::getFont( Util::getDataPath() + DEFAULT_FONT, 20, 20 );
	background->Blit( work );
	messages.draw( start_x, start_y, work, font );
		
	drawInputBox( start_x, start_y + messages.getHeight() + 5, work );

	drawBuddyList( start_x + messages.getWidth() + 10, start_y, work, font );

	need_update = false;
}
	
void ChatServer::run(){
	Global::speed_counter = 0;
	Bitmap work( GFX_X, GFX_Y );
	Keyboard keyboard;

	keyboard.setAllDelay( 200 );
	keyboard.setDelay( Keyboard::Key_TAB, 200 );
	keyboard.setDelay( Keyboard::Key_ESC, 0 );

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
