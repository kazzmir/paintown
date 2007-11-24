#include "object/object.h"
#include "network_world.h"
#include "network.h"
#include "level/scene.h"
#include "globals.h"

using namespace std;

struct Stuff{
	NLsocket socket;
	NetworkWorld * world;
};

static void * handleMessages( void * arg ){
	Stuff * s = (Stuff *) arg;
	NLsocket socket = s->socket;
	NetworkWorld * world = s->world;
	
	try{
		while ( world->isRunning() ){
			Network::Message m( socket );
			// pthread_mutex_lock( lock );
			world->addIncomingMessage( m );
			Global::debug( 2 ) << "Received path '" << m.path << "'" << endl;
			// pthread_mutex_unlock( lock );
		}
	} catch ( const Network::NetworkException & ne ){
		Global::debug( 0 ) << "Network exception" << endl;
	}

	delete s;

	return NULL;
}

NetworkWorld::NetworkWorld( const vector< NLsocket > & sockets, const vector< Object * > & players, const string & path, int screen_size ) throw ( LoadException ):
World( players, path, screen_size ),
sockets( sockets ),
id( 3 ),
running( true ){
	for ( vector< PlayerTracker >::iterator it = this->players.begin(); it != this->players.end(); it++ ){
		Object * object = it->player;
		addMessage( object->getCreateMessage() );
	}

	pthread_mutex_init( &message_mutex, NULL );
	pthread_mutex_init( &running_mutex, NULL );

	for ( vector< NLsocket >::const_iterator it = sockets.begin(); it != sockets.end(); it++ ){
		Stuff * s = new Stuff;
		s->socket = *it;
		s->world = this;
		pthread_t thread;
		pthread_create( &thread, NULL, handleMessages, s );
		threads.push_back( thread );
	}
}
	
NetworkWorld::~NetworkWorld(){
	stopRunning();

	for ( vector< pthread_t >::iterator it = threads.begin(); it != threads.end(); it++ ){
		const pthread_t & thread = *it;
		pthread_join( thread, NULL );
	}
}
	
void NetworkWorld::addMessage( Network::Message m ){
	outgoing.push_back( m );
}

void NetworkWorld::addIncomingMessage( const Network::Message & message ){
	pthread_mutex_lock( &message_mutex );
	incoming.push_back( message );
	pthread_mutex_unlock( &message_mutex );
}
	
void NetworkWorld::stopRunning(){
	pthread_mutex_lock( &running_mutex );
	running = false;
	pthread_mutex_unlock( &running_mutex );
}

bool NetworkWorld::isRunning(){
	pthread_mutex_lock( &running_mutex );
	bool b = running;
	pthread_mutex_unlock( &running_mutex );
	return b;
}

void NetworkWorld::sendMessage( const Network::Message & message, NLsocket socket ){
	Global::debug( 2 ) << "Sending message to client" << endl;
	Network::send16( socket, message.id );
	Global::debug( 2 ) << "Sent message id " << message.id << endl;
	Network::sendBytes( socket, message.data, Network::DATA_SIZE );
	if ( message.path != "" ){
		Global::debug( 2 ) << "Send message length " << message.path.length() << endl;
		Network::send16( socket, message.path.length() + 1 );
		Global::debug( 2 ) << "Send path '" << message.path << "'" << endl;
		Network::sendStr( socket, message.path );
	} else {
		Network::send16( socket, -1 );
	}
}

Network::Message NetworkWorld::nextBlockMessage( int block ){
	Network::Message message;

	message.id = 0;
	message << NEXT_BLOCK;
	message << block;

	return message;
}

void NetworkWorld::doScene( int min_x, int max_x ){
	vector< Object * > obj;
	int block = scene->getBlock();
	scene->act( min_x, max_x, &obj );
	if ( scene->getBlock() != block ){
		addMessage( nextBlockMessage( scene->getBlock() ) );
	}

	for ( vector< Object * >::iterator it = obj.begin(); it != obj.end(); it++ ){
		Object * m = *it;
		m->setId( nextId() );
		addMessage( m->getCreateMessage() );
		addMessage( m->movedMessage() );
	}

	objects.insert( objects.end(), obj.begin(), obj.end() );
}
	
void NetworkWorld::handleMessage( Network::Message & message ){
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
		Object * o = *it;
		if ( o->getId() == message.id ){
			o->interpretMessage( message );
		}
	}
}

vector< Network::Message > NetworkWorld::getIncomingMessages(){
	vector< Network::Message > m;
	pthread_mutex_lock( &message_mutex );
	m = incoming;
	incoming.clear();
	pthread_mutex_unlock( &message_mutex );
	return m;
}
	
void NetworkWorld::act(){
	World::act();

	vector< Network::Message > messages = getIncomingMessages();
	for ( vector< Network::Message >::iterator it = messages.begin(); it != messages.end(); it++ ){
		handleMessage( *it );
	}

	for ( vector< Network::Message >::iterator it = outgoing.begin(); it != outgoing.end(); it++ ){
		Network::Message & m = *it;
		for ( vector< NLsocket >::iterator socket = sockets.begin(); socket != sockets.end(); socket++ ){
			sendMessage( m, *socket );
		}
	}
	outgoing.clear();
}
