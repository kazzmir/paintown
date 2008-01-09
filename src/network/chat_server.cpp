#include "util/bitmap.h"
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

Client::Client( Network::Socket socket, ChatServer * server, unsigned int id ):
socket( socket ),
server( server ),
id( id ),
alive( true ),
started( false ){
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
			Global::debug( 1 ) << "Client " << client->getId() << " waiting for message" << endl;
			Network::Message message( client->getSocket() );
			Global::debug( 1 ) << client->getId() << " Got a message: '" << message.path << "'" << endl;
			int type;
			message >> type;
			switch ( type ){
				case CHANGE_NAME : {
					message << client->getId();
					client->getServer()->sendMessage( message, client->getId() );
					client->setName( message.path );
					client->getServer()->needUpdate();
					break;
				}
				case ADD_MESSAGE : {
					client->getServer()->addMessage( client->getName() + ":" + message.path, client->getId() );
					break;
				}
				case OK_TO_START : {
					client->setOk();
					break;
				}
			}
			done = ! client->isAlive();
		} catch ( const Network::NetworkException & e ){
			Global::debug( 0 ) << "Client input " << client->getId() << " died" << endl;
			done = true;
		}
	}

	Global::debug( 1 ) << client->getId() << " is done" << endl;
	
	if ( client->canKill() ){
		Global::debug( 0 ) << "Input thread killing client" << endl;
		client->getServer()->killClient( client );
	}

	return NULL;
}

static void * clientOutput( void * client_ ){
	Client * client = (Client *) client_;
	bool done = false;
	while ( ! done ){
		Network::Message message;
		done = ! client->isAlive();
		if ( client->getOutgoing( message ) != false ){
			Global::debug( 1 ) << "Sending a message to " << client->getId() << endl;
			try{
				/*
				Network::Message net;
				net.path = message;
				net.send( client->getSocket() );
				*/
				message.send( client->getSocket() );
			} catch ( const Network::NetworkException & e ){
				Global::debug( 0 ) << "Client output " << client->getId() << " died" << endl;
				done = true;
			}
		} else {
			Util::rest( 1 );
		}
	}

	if ( client->canKill() ){
		Global::debug( 0 ) << "Output thread killing client" << endl;
		client->getServer()->killClient( client );
	}

	return NULL;
}
	
bool Client::canKill(){
	bool f;
	pthread_mutex_lock( &lock );
	f = alive;
	alive = false;
	pthread_mutex_unlock( &lock );
	return f;
}

bool Client::getOutgoing( Network::Message & m ){
	bool has;
	pthread_mutex_lock( &lock );
	has = ! outgoing.empty();
	if ( has ){
		m = outgoing.front();
		outgoing.erase( outgoing.begin() );
	}
	pthread_mutex_unlock( &lock );
	return has;
}

void Client::addOutputMessage( const Network::Message & s ){
	pthread_mutex_lock( &lock );
	outgoing.push_back( s );
	pthread_mutex_unlock( &lock );
}

void Client::startThreads(){
	pthread_mutex_lock( &lock );
	if ( ! started ){
		pthread_create( &inputThread, NULL, clientInput, this );
		pthread_create( &outputThread, NULL, clientOutput, this );
		started = true;
	}
	pthread_mutex_unlock( &lock );
}

static void * acceptConnections( void * server_ ){
	bool done = false;
	ChatServer * server = (ChatServer *) server_;
	Network::Socket socket = server->getSocket();
	while ( ! done ){
		Global::debug( 0 ) << "Accept more connections" << endl;
		done = ! server->isAccepting();
		try{
			server->addConnection( Network::accept( socket ) );
		} catch ( const Network::NoConnectionsPendingException & e ){
		} catch ( const Network::NetworkException & e ){
			Global::debug( 0 ) << "Error accepting connections: " << e.getMessage() << endl;
			done = true;
		}
		Util::rest( 1 );
	}

	return NULL;
}

ChatServer::ChatServer( const string & name, Network::Socket socket ):
need_update( true ),
socket( socket ),
messages( 400, 350 ),
focus( INPUT_BOX ),
client_id( 1 ),
name( name ),
accepting( true ){
	background = new Bitmap( Util::getDataPath() + "/paintown-title.png" );

	Network::listen( socket );
	pthread_mutex_init( &lock, NULL );
	pthread_create( &acceptThread, NULL, acceptConnections, this );
}
	
bool ChatServer::isAccepting(){
	bool f;
	pthread_mutex_lock( &lock );
	f = accepting;	
	pthread_mutex_unlock( &lock );
	return f;
}

void ChatServer::stopAccepting(){
	Global::debug( 0 ) << "Stop accepting" << endl;
	pthread_mutex_lock( &lock );
	Network::close( getSocket() );
	accepting = false;
	pthread_mutex_unlock( &lock );
	Global::debug( 0 ) << "Waiting for accepting thread to stop" << endl;
	pthread_join( acceptThread, NULL );
	Global::debug( 0 ) << "Not accepting any connections" << endl;
}

void ChatServer::addConnection( Network::Socket s ){
	Client * client = new Client( s, this, clientId() );

	{
		Network::Message message;
		message << ADD_BUDDY;
		message << 0;
		message.path = getName();
		client->addOutputMessage( message );
	}

	pthread_mutex_lock( &lock );
	for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++ ){
		Client * c = *it;
		Network::Message message;
		message << ADD_BUDDY;
		message << c->getId();
		message.path = c->getName();
		client->addOutputMessage( message );
	}
	pthread_mutex_unlock( &lock );

	Global::debug( 1 ) << "Adding client " << client->getId() << endl;

	addMessage( "** A client joined", 0 );

	Network::Message message;
	message << ADD_BUDDY;
	message << client->getId();
	sendMessage( message, 0 );

	pthread_mutex_lock( &lock );
	clients.push_back( client );
	pthread_mutex_unlock( &lock );
}

static char lowerCase( const char * x ){
	if ( x[0] >= 'A' && x[0] <= 'Z' ){
		return x[0] - 'A' + 'a';
	}
	return x[0];
}

void ChatServer::sendMessage( const Network::Message & message, unsigned int id ){
	pthread_mutex_lock( &lock );
	for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++ ){
		Client * c = *it;
		if ( c->getId() != id ){
			c->addOutputMessage( message );
		}
	}
	pthread_mutex_unlock( &lock );
}

void ChatServer::addMessage( const string & s, unsigned int id ){
	Network::Message message;
	message << ADD_MESSAGE;
	message.path = s;
	pthread_mutex_lock( &lock );
	messages.addMessage( s );
	needUpdate();
	pthread_mutex_unlock( &lock );

	sendMessage( message, id );
	/*
	for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++ ){
		Client * c = *it;
		if ( c->getId() != id ){
			c->addOutputMessage( message );
		}
	}
	*/
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
			addMessage( name + ": " + input, 0 );
			input = "";
			needUpdate();
		}
	}
}
	
void ChatServer::shutdownClientThreads(){
	pthread_mutex_lock( &lock );
	for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++ ){
		Client * c = *it;
		c->kill();
	}
	pthread_mutex_unlock( &lock );
	Network::Message message;
	message << START_THE_GAME;
	sendMessage( message, 0 );

	for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++ ){
		Client * c = *it;
		Global::debug( 0 ) << "Waiting for client " << c->getId() << " to finish input/output threads" << endl;
		pthread_join( c->getInputThread(), NULL );
		pthread_join( c->getOutputThread(), NULL );
		Global::debug( 0 ) << "Client " << c->getId() << " is done" << endl;
	}
}
	
vector< Network::Socket > ChatServer::getConnectedClients(){
	vector< Network::Socket > sockets;

	for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++ ){
		Client * c = *it;
		if ( c->isOk() ){
			sockets.push_back( c->getSocket() );
		}
	}

	return sockets;
}

void ChatServer::killAllClients(){
	vector< Client * > all;
	pthread_mutex_lock( &lock );
	all = clients;
	pthread_mutex_unlock( &lock );
	for ( vector< Client * >::iterator it = all.begin(); it != all.end(); it++ ){
		killClient( *it );
	}
}

void ChatServer::killClient( Client * c ){
	int id = c->getId();
	string name = c->getName();
	pthread_mutex_lock( &lock );
	for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); ){
		Client * client = *it;
		if ( client == c ){
			Global::debug( 1 ) << "Killing client " << c->getId() << endl;
			c->kill();
			Global::debug( 1 ) << "Closing client socket " << c->getSocket() << endl;
			Network::close( c->getSocket() );
			/* It looks like the client that called killClient is waiting
			 * for itself to exit but pthread_join won't block if the
			 * argument is the same as the calling thread, so its ok.
			 */
			Global::debug( 1 ) << "Waiting for input thread to die" << endl;
			pthread_join( c->getInputThread(), NULL );
			Global::debug( 1 ) << "Waiting for output thread to die" << endl;
			pthread_join( c->getOutputThread(), NULL );
			Global::debug( 1 ) << "Deleting client" << endl;
			/* delete can be moved to the input/output thread exit part
			 * if need be.
			 */
			delete client;
			it = clients.erase( it );
		} else {
			it++;
		}
	}
	needUpdate();
	pthread_mutex_unlock( &lock );
	addMessage( "** " + name + " quit", 0 );
	Network::Message remove;
	remove << REMOVE_BUDDY;
	remove << id;
	sendMessage( remove, 0 );
}

bool ChatServer::logic( Keyboard & keyboard ){
	if ( keyboard[ Keyboard::Key_TAB ] ){
		focus = nextFocus( focus );
		needUpdate();
	}

	switch ( focus ){
		case INPUT_BOX : {
			handleInput( keyboard );
			break;
		}
		case START_GAME : {
			if ( keyboard[ Keyboard::Key_ENTER ] ){
				return true;
			}
			break;
		}
		case QUIT : {
			if ( keyboard[ Keyboard::Key_ENTER ] ){
				return true;
			}
			break;
		}
	}

	return false;
}
	
void ChatServer::needUpdate(){
	need_update = true;
}
	
Focus ChatServer::nextFocus( Focus f ){
	switch ( f ){
		case INPUT_BOX : return START_GAME;
		case START_GAME : return QUIT;
		case QUIT : return INPUT_BOX;
		default : return INPUT_BOX;
	}
}

bool ChatServer::needToDraw(){
	return need_update;
}

void ChatServer::drawInputBox( int x, int y, const Bitmap & work ){
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
	
int ChatServer::focusColor( Focus f ){
	if ( f == focus ){
		return Bitmap::makeColor( 255, 255, 0 );
	}
	return Bitmap::makeColor( 255, 255, 255 );
}

void ChatServer::draw( const Bitmap & work ){
	int start_x = 20;
	int start_y = 20;
	const Font & font = Font::getFont( Util::getDataPath() + Global::DEFAULT_FONT, 20, 20 );
	background->Blit( work );
	messages.draw( start_x, start_y, work, font );
		
	drawInputBox( start_x, start_y + messages.getHeight() + 5, work );

	drawBuddyList( start_x + messages.getWidth() + 10, start_y, work, font );

	font.printf( start_x, start_y + messages.getHeight() + 5 + font.getHeight() * 2 + 5, focusColor( START_GAME ), work, "Start the game", 0 );
	font.printf( start_x + font.textLength( "Start the game" ) + 20, start_y + messages.getHeight() + 5 + font.getHeight() * 2 + 5, focusColor( QUIT ), work, "Quit", 0 );

	need_update = false;
}

void ChatServer::startThreadsHack(){
	pthread_mutex_lock( &lock );
	for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++ ){
		Client * c = *it;
		c->startThreads();
	}
	pthread_mutex_unlock( &lock );
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
			startThreadsHack();
			keyboard.poll();
			done = logic( keyboard );
			think -= 1;
			Global::speed_counter = 0;
			if ( keyboard[ Keyboard::Key_ESC ] || done && focus == QUIT ){
				addMessage( "** Server quit", 0 );
				stopAccepting();
				killAllClients();
				done = true;
			} else if ( done && focus == START_GAME ){
				stopAccepting();
				shutdownClientThreads();
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
}
	
ChatServer::~ChatServer(){
	delete background;
	for ( vector< Client * >::iterator it = clients.begin(); it != clients.end(); it++ ){
		delete *it;
	}
}
