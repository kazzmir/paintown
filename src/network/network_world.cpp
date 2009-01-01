#include "object/object.h"
#include "object/character.h"
#include "network_world.h"
#include "network.h"
#include "util/funcs.h"
#include "level/scene.h"
#include "globals.h"

using namespace std;

struct Stuff{
    Network::Socket socket;
    NetworkWorld * world;
};

static std::ostream & debug( int level ){
	Global::debug( level ) << "[network-world] ";
	return Global::debug( level );
}

static void * handleMessages( void * arg ){
	Stuff * s = (Stuff *) arg;
	Network::Socket socket = s->socket;
	NetworkWorld * world = s->world;
	
	try{
		while ( world->isRunning() ){
			Network::Message m( socket );
			// pthread_mutex_lock( lock );
			world->addIncomingMessage( m, socket );
			debug( 2 ) << "Received path '" << m.path << "'" << endl;
			// pthread_mutex_unlock( lock );
		}
	} catch ( const Network::NetworkException & ne ){
		debug( 0 ) << "Network exception: " << ne.getMessage() << endl;
	}

	delete s;

	return NULL;
}

NetworkWorld::NetworkWorld( const vector< NLsocket > & sockets, const vector< Object * > & players, const string & path, int screen_size ) throw ( LoadException ):
World( players, path, screen_size ),
sockets( sockets ),
sent_messages( 0 ),
running( true ){
    Object::networkid_t max_id = 0;

	pthread_mutex_init( &message_mutex, NULL );
	pthread_mutex_init( &running_mutex, NULL );

	for ( vector< PlayerTracker >::iterator it = this->players.begin(); it != this->players.end(); it++ ){
		Object * object = it->player;
		addMessage( object->getCreateMessage() );
		if ( object->getId() > max_id ){
			max_id = object->getId();
		}
	}
	this->id = max_id + 1;

}

void NetworkWorld::startMessageHandlers(){
    for ( vector<Network::Socket>::const_iterator it = sockets.begin(); it != sockets.end(); it++ ){
        Stuff * s = new Stuff;
        s->socket = *it;
        s->world = this;
        pthread_t thread;
        pthread_create( &thread, NULL, handleMessages, s );
        threads.push_back( thread );
    }
}

void NetworkWorld::waitForHandlers(){
    for (vector<pthread_t>::iterator it = threads.begin(); it != threads.end(); it++){
        pthread_t & thread = *it;
        pthread_join(thread, NULL);
    }
}

NetworkWorld::~NetworkWorld(){
	stopRunning();

	/*
	for ( vector< pthread_t >::iterator it = threads.begin(); it != threads.end(); it++ ){
		const pthread_t & thread = *it;
		pthread_join( thread, NULL );
	}
	*/
}
	
void NetworkWorld::addObject( Object * o ){
	if ( o->getId() != (Object::networkid_t) -1 ){
		for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
			Object * obj = *it;
			if ( obj->getId() == o->getId() ){
				return;
			}
		}
	}

	World::addObject( o );
}
	
void NetworkWorld::addMessage( Network::Message m, Network::Socket from ){
	pthread_mutex_lock( &message_mutex );
	Packet p( m, from );
	outgoing.push_back( p );
	pthread_mutex_unlock( &message_mutex );
}

void NetworkWorld::addIncomingMessage( const Network::Message & message, Network::Socket from ){
	pthread_mutex_lock( &message_mutex );
	incoming.push_back( message );
	pthread_mutex_unlock( &message_mutex );
	addMessage( message, from );
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

void NetworkWorld::sendMessage( const Network::Message & message, Network::Socket socket ){
	// message.send( socket );
}
	
Network::Message NetworkWorld::finishMessage(){
	Network::Message message;

	message << FINISH;

	return message;
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

Object * NetworkWorld::findObject( Object::networkid_t id ){
	for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
		Object * o = *it;
		if ( o->getId() == id ){
			return o;
		}
	}
	return NULL;
}
	
void NetworkWorld::handleMessage( Network::Message & message ){
	if ( message.id == 0 ){
		int type;
		message >> type;
		switch ( type ){
			case GRAB : {
                                Object::networkid_t grabbing;
                                Object::networkid_t grabbed;
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
			case THROWN : {
                              Object::networkid_t grabbing;
                              Object::networkid_t grabbed;
				message >> grabbing;
				message >> grabbed;
				Character * c_grabbing = (Character *) findObject( grabbing );
				Character * c_grabbed = (Character *) findObject( grabbed );
				if ( c_grabbed != NULL && c_grabbing != NULL ){
					debug( 2 ) << grabbing << " threw " << grabbed << endl;
					c_grabbed->setFacing( c_grabbing->getOppositeFacing() );
					c_grabbed->thrown();
					addMessage( c_grabbed->movedMessage() );
					addMessage( c_grabbed->fallMessage( 3.2, 5.0 ) );
					c_grabbed->fall( 3.2, 5.0 );
					c_grabbing->setStatus( Status_Ground );
					debug( 2 ) << grabbed << " status is " << c_grabbed->getStatus() << endl;
				}
				break;
			}
		}
	} else {
		Object * o = findObject( message.id );
		if ( o != NULL ){
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

void NetworkWorld::flushOutgoing(){
	vector< Packet > packets;
	pthread_mutex_lock( &message_mutex );
	packets = outgoing;
	outgoing.clear();
	pthread_mutex_unlock( &message_mutex );

        for (vector<Network::Socket>::iterator socket = sockets.begin(); socket != sockets.end(); socket++){
            vector<Network::Message*> messages;
            for ( vector< Packet >::iterator it = packets.begin(); it != packets.end(); it++ ){
                Network::Message & message = (*it).message;
		Network::Socket from = (*it).socket;
                if (from != *socket){
                    messages.push_back(&message);
                }
            }

            Network::sendAllMessages(messages, *socket);
        }

#if 0
	/* TODO: combine packets together into one big bundle to save TCP ack's */
	for ( vector< Packet >::iterator it = packets.begin(); it != packets.end(); it++ ){
		Network::Message & m = (*it).message;
		Network::Socket from = (*it).socket;
		sent_messages += 1;
		for ( vector< Network::Socket >::iterator socket = sockets.begin(); socket != sockets.end(); ){
			try{
				if ( from != *socket ){
					debug( 1 ) << "Send message " << sent_messages << " to " << *socket << endl;
					m.send( *socket );
					debug( 1 ) << "Sent message" << endl;
				}
				socket++;
			} catch ( const Network::NetworkException & ne ){
				/* TODO: remove character if the socket dies */
				socket = sockets.erase( socket );
				debug( 0 ) << "Network exception: " << ne.getMessage() << endl;
			}
		}
	}
#endif
}
	
void NetworkWorld::act(){
	World::act();

	vector< Network::Message > messages = getIncomingMessages();
	for ( vector< Network::Message >::iterator it = messages.begin(); it != messages.end(); it++ ){
		handleMessage( *it );
	}

	flushOutgoing();
}
