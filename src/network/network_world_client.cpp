#include "util/bitmap.h"
#include "network_world_client.h"
#include "network.h"
#include "level/scene.h"
#include "globals.h"
#include "level/blockobject.h"
#include "util/funcs.h"
#include "object/player.h"
#include "factory/object_factory.h"
#include <pthread.h>
#include <string.h>

#include "object/character.h"
#include "object/cat.h"
#include "object/item.h"

static void * handleMessages( void * arg ){
	NetworkWorldClient * world = (NetworkWorldClient *) arg;
	NLsocket socket = world->getServer();
	// pthread_mutex_t * lock = world->getLock();
	unsigned int received = 0;
	
	try{
		while ( world->isRunning() ){
			received += 1;
			Global::debug( 1 ) << "Receiving message " << received << endl;
			Network::Message m( socket );
			// pthread_mutex_lock( lock );
			world->addIncomingMessage( m );
			Global::debug( 2 ) << "Received path '" << m.path << "'" << endl;
			// pthread_mutex_unlock( lock );
		}
	} catch ( const Network::NetworkException & n ){
		Global::debug( 0 ) << "Network exception: " << n.getMessage() << endl;
	}

	Global::debug( 0 ) << "Client input stopped" << endl;

	return NULL;
}
	
NetworkWorldClient::NetworkWorldClient( Network::Socket server, const std::vector< Object * > & players, const string & path, unsigned int id, int screen_size ) throw ( LoadException ):
World( players, path, screen_size ),
server( server ),
world_finished( false ),
id( id ),
running( true ){
	objects.clear();
	pthread_mutex_init( &message_mutex, NULL );
	pthread_mutex_init( &running_mutex, NULL );
	pthread_create( &message_thread, NULL, handleMessages, this );
}
	
NetworkWorldClient::~NetworkWorldClient(){
	Global::debug( 1 ) << "Destroy client world" << endl;
	stopRunning();
	pthread_join( message_thread, NULL );
}
	
bool NetworkWorldClient::isRunning(){
	pthread_mutex_lock( &running_mutex );
	bool b = running;
	pthread_mutex_unlock( &running_mutex );
	return b;
}

void NetworkWorldClient::stopRunning(){
	pthread_mutex_lock( &running_mutex );
	running = false;
	pthread_mutex_unlock( &running_mutex );
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

bool NetworkWorldClient::uniqueObject( unsigned int id ){
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
		Object * o = *it;
		if ( o->getId() == id ){
			return false;
		}
	}
	return true;
}

void NetworkWorldClient::handleCreateCharacter( Network::Message & message ){
	int alliance;
	int id;
	int map;
	string path = Util::getDataPath() + "/" + message.path;
	message >> alliance >> id >> map;
	if ( uniqueObject( id ) ){
		if ( (unsigned int) id == this->id ){
			Player * p = (Player *) players[ 0 ].player;
			p->deathReset();
			addObject( p );
		} else {
			BlockObject block;
			block.setType( ObjectFactory::OBJECT_NETWORK_CHARACTER );
			block.setMap( map );
			block.setPath( path );
			Character * character = (Character *) ObjectFactory::createObject( &block );
			if ( character == NULL ){
				Global::debug( 0 ) << "Could not create character!" << endl;
				return;
			}
			Global::debug( 1 ) << "Create '" << path << "' with id " << id << " alliance " << alliance << endl;
			character->setId( id );
			character->setAlliance( alliance );
			/* TODO: should these values be hard-coded? */
			character->setX( 200 );
			character->setY( 0 );
			character->setZ( 150 );
			addObject( character );
		}
	} else {
		Global::debug( 1 ) << id << " is not unique" << endl;
	}
}

void NetworkWorldClient::handleCreateCat( Network::Message & message ){
	int id;
	message >> id;
	if ( uniqueObject( id ) ){
		string path = Util::getDataPath() + "/" + message.path;
		BlockObject block;
		block.setType( ObjectFactory::OBJECT_CAT );
		block.setPath( path );
		/* TODO: should these values be hard-coded? */
		block.setCoords( 200, 150 );
		Cat * cat = (Cat *) ObjectFactory::createObject( &block );
		if ( cat == NULL ){
			Global::debug( 0 ) << "Could not create cat" << endl;
			return;
		}

		cat->setY( 0 );
		cat->setId( (unsigned int) -1 );
		addObject( cat );
	}
}
	
const bool NetworkWorldClient::finished() const {
	return world_finished;
}

Object * NetworkWorldClient::removeObject( unsigned int id ){
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); ){
		Object * o = *it;
		if ( o->getId() == id ){
			it = objects.erase( it );
			return o;
		} else {
			it++;
		}
	}
	return NULL;
}

void NetworkWorldClient::handleCreateItem( Network::Message & message ){
	int id;
	message >> id;
	if ( uniqueObject( id ) ){
		int x, z;
		int value;
		message >> x >> z >> value;
		string path = Util::getDataPath() + "/" + message.path;
		BlockObject block;
		block.setType( ObjectFactory::OBJECT_ITEM );
		block.setPath( path );
		/* TODO: dont hard-code this */
		block.setStimulationType( "health" );
		block.setStimulationValue( value );
		block.setCoords( x, z );
		Item * item = (Item *) ObjectFactory::createObject( &block );
		if ( item == NULL ){
			Global::debug( 0 ) << "Could not create item" << endl;
			return;
		}

		item->setY( 0 );
		item->setId( id );
		addObject( item );
	}
}

void NetworkWorldClient::handleCreateBang( Network::Message & message ){
	int x, y, z;
	message >> x >> y >> z;
	Object * addx = bang->copy();
	addx->setX( x );
	addx->setY( 0 );
	addx->setZ( y+addx->getHeight()/2 );
	addx->setHealth( 1 );
	addx->setId( (unsigned int) -1 );
	addObject( addx );
}

Object * NetworkWorldClient::findObject( unsigned int id ){
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
		Object * o = *it;
		if ( o->getId() == id ){
			return o;
		}
	}
	return NULL;
}
	
void NetworkWorldClient::handleMessage( Network::Message & message ){
	if ( message.id == 0 ){
		int type;
		message >> type;
		switch ( type ){
			case CREATE_CHARACTER : {
				handleCreateCharacter( message );
				break;
			}
			case CREATE_CAT : {
				handleCreateCat( message );	
				break;
			}
			case CREATE_BANG : {
				handleCreateBang( message );
				break;
			}
			case CREATE_ITEM : {
				handleCreateItem( message );
				break;
			}
			case NEXT_BLOCK : {
				int block;
				message >> block;
				scene->advanceBlocks( block );
				break;
			}
			case GRAB : {
				int grabbing;
				int grabbed;
				message >> grabbing;
				message >> grabbed;
				Character * c_grabbing = (Character *) findObject( grabbing );
				Character * c_grabbed = (Character *) findObject( grabbed );
				if ( c_grabbing != NULL && c_grabbed != NULL ){
					c_grabbed->grabbed( c_grabbing );
					c_grabbing->setLink( c_grabbed );
				}
				break;
			}
			case DELETE_OBJ : {
				int id;
				message >> id;
				Object * o = removeObject( id );
				if ( o != NULL ){
					delete o;
				}
				break;
			}
			case IGNORE : {
				break;
			}
			case REMOVE : {
				int id;
				message >> id;
				removeObject( id );
				break;
			}
			case FINISH : {

				Global::debug( 1 ) << "Received finish message" << endl;
				world_finished = true;
				break;
			}
			case NOTHING : {
				Global::debug( 0 ) << "Invalid message. Data dump" << endl;
				for ( int i = 0; i < Network::DATA_SIZE; i++ ){
					Global::debug( 0 ) << (int) message.data[ i ] << " ";
				}
				Global::debug( 0 ) << endl;
				break;
			}
		}
	} else {
		for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
			Object * o = *it;
			if ( o->getId() == message.id ){
				o->interpretMessage( message );
			}
		}
	}
}

void NetworkWorldClient::addMessage( Network::Message m, Network::Socket from ){
	if ( m.id == id || m.id == 0 ){
		outgoing.push_back( m );
	}
}
	
void NetworkWorldClient::doScene( int min_x, int max_x ){
	vector< Object * > objs;
	scene->act( min_x, max_x, &objs );
	for ( vector< Object * >::iterator it = objs.begin(); it != objs.end(); it++ ){
		delete *it;
	}
}

void NetworkWorldClient::sendMessage( const Network::Message & message, NLsocket socket ){
	message.send( socket );
}

void NetworkWorldClient::act(){
	
	if ( quake_time > 0 ){
		quake_time--;
	}

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
		
		/*
		if ( player->getX() < it->min_x ){
			player->setX( it->min_x );
		}
		*/
		if ( player->getX() < 0 ){
			player->setX( 0 );
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

	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); ){
		if ( (*it)->getHealth() <= 0 ){
			(*it)->died( added_effects );
			if ( ! isPlayer( *it ) ){
				delete *it;
			}
			it = objects.erase( it );
		} else ++it;
	}

	for ( vector< Network::Message >::iterator it = outgoing.begin(); it != outgoing.end(); it++ ){
		Network::Message & m = *it;
		sendMessage( m, getServer() );
	}
	outgoing.clear();
	
	for ( vector< Object * >::iterator it = added_effects.begin(); it != added_effects.end(); ){
		Object * o = *it;
		o->setId( (unsigned int) -1 );
		it++;
	}
	objects.insert( objects.end(), added_effects.begin(), added_effects.end() );
}
