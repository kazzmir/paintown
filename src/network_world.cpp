#include "object/object.h"
#include "network_world.h"
#include "network.h"
#include "level/scene.h"
#include "globals.h"

using namespace std;

NetworkWorld::NetworkWorld( const vector< NLsocket > & sockets, const vector< Object * > & players, const string & path, int screen_size ) throw ( LoadException ):
World( players, path, screen_size ),
sockets( sockets ),
id( 3 ){
	for ( vector< PlayerTracker >::iterator it = this->players.begin(); it != this->players.end(); it++ ){
		Object * object = it->player;
		addMessage( object->getCreateMessage() );
	}
}
	
void NetworkWorld::addMessage( Network::Message m ){
	messages.push_back( m );
}

void NetworkWorld::sendMessage( const Network::Message & message, NLsocket socket ){
	Global::debug( 0 ) << "Sending message to client" << endl;
	Network::send16( socket, message.id );
	Global::debug( 0 ) << "Sent message id " << message.id << endl;
	Network::sendBytes( socket, message.data, Network::DATA_SIZE );
	if ( message.path != "" ){
		Global::debug( 0 ) << "Send message length " << message.path.length() << endl;
		Network::send16( socket, message.path.length() + 1 );
		Global::debug( 0 ) << "Send path '" << message.path << "'" << endl;
		Network::sendStr( socket, message.path );
	} else {
		Network::send16( socket, -1 );
	}
}
	
void NetworkWorld::doScene( int min_x, int max_x ){
	vector< Object * > obj;
	scene->act( min_x, max_x, &obj );

	for ( vector< Object * >::iterator it = obj.begin(); it != obj.end(); it++ ){
		Object * m = *it;
		m->setId( nextId() );
		addMessage( m->getCreateMessage() );
	}

	objects.insert( objects.end(), obj.begin(), obj.end() );
}
	
void NetworkWorld::act(){
	World::act();
	for ( vector< Network::Message >::iterator it = messages.begin(); it != messages.end(); it++ ){
		Network::Message & m = *it;
		for ( vector< NLsocket >::iterator socket = sockets.begin(); socket != sockets.end(); socket++ ){
			sendMessage( m, *socket );
		}
	}
	messages.clear();
}
