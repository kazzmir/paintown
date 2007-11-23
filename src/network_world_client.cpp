#include "network_world_client.h"
#include "network.h"
#include "level/scene.h"
#include "globals.h"
#include "level/blockobject.h"
#include "util/funcs.h"
#include "factory/object_factory.h"
#include <pthread.h>
#include <string.h>

#include "object/character.h"
#include "object/cat.h"

static void * handleMessages( void * arg ){
	NetworkWorldClient * world = (NetworkWorldClient *) arg;
	NLsocket socket = world->getServer();
	// pthread_mutex_t * lock = world->getLock();
	
	while ( 1 ){
		Network::Message m( socket );
		// pthread_mutex_lock( lock );
		world->addIncomingMessage( m );
		Global::debug( 0 ) << "Received path '" << m.path << "'" << endl;
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

bool NetworkWorldClient::uniqueObject( Object * object ){
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
		Object * o = *it;
		if ( o->getId() == object->getId() ){
			return false;
		}
	}
	return true;
}
	
void NetworkWorldClient::handleMessage( Network::Message & message ){
	if ( message.id == 0 ){
		int type;
		message >> type;
		switch ( type ){
			case Network::CREATE_CHARACTER : {
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
				Global::debug( 0 ) << "Create '" << path << "' with id " << id << " alliance " << alliance << endl;
				character->setId( id );
				character->setAlliance( alliance );
				character->setX( 200 );
				character->setY( 0 );
				character->setZ( 150 );
				if ( uniqueObject( character ) ){
					addObject( character );
				} else {
					delete character;
				}
				break;
			}
			case Network::CREATE_CAT : {
				int id;
				message >> id;
				string path = Util::getDataPath() + "/" + message.path;
				BlockObject block;
				block.setType( ObjectFactory::OBJECT_CAT );
				block.setPath( path );
				block.setCoords( 200, 150 );
				Cat * cat = (Cat *) ObjectFactory::createObject( &block );
				if ( cat == NULL ){
					Global::debug( 0 ) << "Could not create cat" << endl;
					break;
				}

				cat->setY( 0 );
				if ( uniqueObject( cat ) ){
					addObject( cat );
				} else {
					delete cat;
				}

				break;
			}
		}
	} else {
	}
}
	
void NetworkWorldClient::doScene( int min_x, int max_x ){
	vector< Object * > objs;
	scene->act( min_x, max_x, &objs );
}

void NetworkWorldClient::act(){

	vector< Object * > added_effects;
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
		Object * o = *it;
		o->act( &objects, this, &added_effects );
		if ( o->getZ() < getMinimumZ() ){
			o->setZ( getMinimumZ() );
		}
		if ( o->getZ() > getMaximumZ() ){
			o->setZ( getMaximumZ() );
		}
	}
	objects.insert( objects.end(), added_effects.begin(), added_effects.end() );

	double lowest = 9999999;
	for ( vector< PlayerTracker >::iterator it = players.begin(); it != players.end(); it++ ){
		Object * player = it->player;
		double mx = player->getX() - screen_size / 2;
		if ( it->min_x < mx ){
			it->min_x++;
		}
	
		if ( it->min_x + screen_size >= scene->getLimit() ){
			it->min_x = scene->getLimit() - screen_size;
		}

		if ( it->min_x < lowest ){
			lowest = it->min_x;
		}
		
		if ( player->getX() < it->min_x ){
			player->setX( it->min_x );
		}

		if ( player->getX() > scene->getLimit() ){
			player->setX( scene->getLimit() );
		}
		if ( player->getZ() < getMinimumZ() ){
			player->setZ( getMinimumZ() );
		}
		if ( player->getZ() > getMaximumZ() ){
			player->setZ( getMaximumZ() );
		}
	}

	doScene( 0, 0 );

	vector< Network::Message > messages = getIncomingMessages();
	for ( vector< Network::Message >::iterator it = messages.begin(); it != messages.end(); it++ ){
		handleMessage( *it );
	}
}
