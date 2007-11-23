#include "network_world_client.h"
#include "network.h"
#include "globals.h"
#include "level/blockobject.h"
#include "object/character.h"
#include "util/funcs.h"
#include "factory/object_factory.h"
#include <pthread.h>
#include <string.h>

static void * handleMessages( void * arg ){
	NetworkWorldClient * world = (NetworkWorldClient *) arg;
	NLsocket socket = world->getServer();
	// pthread_mutex_t * lock = world->getLock();
	
	while ( 1 ){
		char * buffer = new char[ 1024 ];
		memset( buffer, 0, 1024 );
		Network::Message m( socket, buffer );
		// pthread_mutex_lock( lock );
		world->addIncomingMessage( m );
		Global::debug( 0 ) << "Received path '" << buffer << "'" << endl;
		// pthread_mutex_unlock( lock );
	}
}
	
NetworkWorldClient::NetworkWorldClient( NLsocket server, const std::vector< Object * > & players, const string & path, int screen_size ) throw ( LoadException ):
World( players, path, screen_size ),
server( server ){
	pthread_mutex_init( &message_mutex, NULL );
	pthread_create( &message_thread, NULL, handleMessages, this );
}
	
void NetworkWorldClient::addIncomingMessage( const Network::Message & message ){
	pthread_mutex_lock( &message_mutex );
	incoming.push_back( message );
	pthread_mutex_unlock( &message_mutex );
}
	
vector< Network::Message > NetworkWorldClient::getIncomingMessages(){
	vector< Network::Message > m;
	pthread_mutex_lock( &message_mutex );
	m = incoming;
	incoming.clear();
	pthread_mutex_unlock( &message_mutex );
	return m;
}
	
void NetworkWorldClient::handleMessage( Network::Message & message ){
	if ( message.id == 0 ){
		int type;
		message >> type;
		switch ( type ){
			case Network::CREATE_OBJECT : {
				int alliance;
				int id;
				string path = Util::getDataPath() + "/" + message.path;
				message >> alliance;
				message >> id;
				BlockObject block;
				block.setType( ObjectFactory::OBJECT_NETWORK_CHARACTER );
				block.setPath( path );
				Character * character = (Character *) ObjectFactory::createObject( &block );
				if ( character == NULL ){
					Global::debug( 0 ) << "Could not create character!" << endl;
					break;
				}
				character->setId( id );
				character->setAlliance( alliance );
				character->setX( 200 );
				character->setY( 0 );
				character->setZ( 150 );
				addObject( character );
				break;
			}
		}
	} else {
	}

	delete[] message.path;
}

void NetworkWorldClient::act(){
	World::act();

	vector< Network::Message > messages = getIncomingMessages();
	for ( vector< Network::Message >::iterator it = messages.begin(); it != messages.end(); it++ ){
		handleMessage( *it );
	}

	/*
	int ready = 0;
	while ( ready != NL_INVALID ){
		NLsocket read;
		ready = nlPollGroup( server, NL_READ_STATUS, &read, 1, 0 );
		if ( ready == 1 ){
			char buf[ 1024 ];
			Global::debug( 0 ) << "Receiving message from server " << read << endl;
			Network::Message message( read, buf );
			Global::debug( 0 ) << "Received message: " << message.id << endl;
		}
	}
	*/
}
