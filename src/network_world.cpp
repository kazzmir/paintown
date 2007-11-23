#include "object/object.h"
#include "network_world.h"
#include "network.h"
#include "globals.h"

using namespace std;

NetworkWorld::NetworkWorld( const vector< NLsocket > & sockets, const vector< Object * > & players, const string & path, int screen_size ) throw ( LoadException ):
World( players, path, screen_size ),
sockets( sockets ){
	for ( vector< PlayerTracker >::iterator it = this->players.begin(); it != this->players.end(); it++ ){
		Object * object = it->player;
		addMessage( object->getCreateMessage() );
	}
}
	
void NetworkWorld::addMessage( Network::Message m ){
	messages.push_back( m );
}

void NetworkWorld::sendMessage( const Network::Message & message, NLsocket socket ){
	Global::debug( 0 ) << "sending message to client" << endl;
	Network::send16( socket, message.id );
	Network::sendBytes( socket, message.data, Network::DATA_SIZE );
	if ( message.path != NULL ){
		Network::send16( socket, strlen( message.path ) + 1 );
		Network::sendStr( socket, message.path );
	} else {
		Network::send16( socket, -1 );
	}
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
