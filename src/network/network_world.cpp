#include "object/object.h"
#include "object/character.h"
#include "game/adventure_world.h"
#include "network_world.h"
#include "network.h"
#include "util/funcs.h"
#include "util/system.h"
#include "level/scene.h"
#include "level/cacher.h"
#include "script/script.h"
#include "globals.h"
#include <sstream>

using namespace std;

struct Stuff{
    Network::Socket socket;
    NetworkWorld * world;
};

static std::ostream & debug( int level ){
    return Global::debug(level, "network-world");
}

static void * handleMessages( void * arg ){
    Stuff * s = (Stuff *) arg;
    Network::Socket socket = s->socket;
    NetworkWorld * world = s->world;

    unsigned int id = 1;
    try{
        while ( world->isRunning() ){
            Network::Message m(socket);
            ostringstream context;
            context << __FILE__ << " " << (System::currentMicroseconds() / 1000);
            Global::debug(2, context.str()) << "Received message " << id << " with path '" << m.path << "'" << endl;
            id += 1;
            // pthread_mutex_lock( lock );
            world->addIncomingMessage(m, socket);
            // debug(2, __FILE__) << "Received path '" << m.path << "'" << endl;
            // pthread_mutex_unlock( lock );
        }
    } catch (const Network::MessageEnd & end){
        debug(1) << "Closed connection with socket " << socket << endl;
    } catch ( const Network::NetworkException & ne ){
        debug( 0 ) << "Network exception: " << ne.getMessage() << endl;
    }

    delete s;

    return NULL;
}

NetworkWorld::NetworkWorld( vector< Network::Socket > & sockets, const vector< Object * > & players, const map<Object*, Network::Socket> & characterToClient, const string & path, int screen_size ) throw ( LoadException ):
AdventureWorld( players, path, new Level::DefaultCacher(), screen_size ),
sockets(sockets),
characterToClient(characterToClient),
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

	AdventureWorld::addObject( o );
}
	
void NetworkWorld::addMessage( Network::Message m, Network::Socket from, Network::Socket to){
    if (m.id == (uint32_t) -1){
        ostringstream out;
        out << "Message not properly formed: ";
        for ( int i = 0; i < Network::DATA_SIZE; i++ ){
            out << (int) m.data[ i ] << " ";
        }
        throw Network::NetworkException(out.str());
    }
    pthread_mutex_lock( &message_mutex );
    /* if the server generates a message `from' will probably be 0.
     * if the message should go to a specific client `to' will be non-zero.
     */
    Packet p(m, from, to);
    outgoing.push_back( p );
    pthread_mutex_unlock( &message_mutex );
}

void NetworkWorld::addIncomingMessage( const Network::Message & message, Network::Socket from ){
	pthread_mutex_lock( &message_mutex );
	incoming.push_back(message);
	pthread_mutex_unlock( &message_mutex );

        /* by default all messages get relayed to all clients, but a client
         * shouldn't send its own message back to itself so it populates the
         * `from' field.
         */
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

/* what is this method for? */
void NetworkWorld::sendMessage( const Network::Message & message, Network::Socket socket ){
	// message.send( socket );
}
	
Network::Message NetworkWorld::finishMessage(){
    Network::Message message;
    message.id = 0;
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

static Network::Message pausedMessage(){
    Network::Message message;
    message.id = 0;
    message << World::PAUSE;

    return message;
}

static Network::Message unpausedMessage(){
    Network::Message message;
    message.id = 0;
    message << World::UNPAUSE;

    return message;
}

void NetworkWorld::changePause(){
    AdventureWorld::changePause();
    if (isPaused()){
        addMessage(pausedMessage());
    } else {
        addMessage(unpausedMessage());
    }
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

Object * NetworkWorld::findNetworkObject( Object::networkid_t id ){
    for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); it++ ){
        Object * o = *it;
        if ( o->getId() == id ){
            return o;
        }
    }
    return NULL;
}

void NetworkWorld::handlePing(Network::Message & message){
    Network::Message out;
    unsigned int ping_id;
    message >> ping_id;
    out.id = 0;
    out << World::PING_REPLY;
    out << ping_id;
    /* only send ping reply to the client that asked for it */
    addMessage(out, 0, message.readFrom);
}

void NetworkWorld::removePlayer(Object * player){
    for ( vector< PlayerTracker >::iterator it = players.begin(); it != players.end(); ){
        PlayerTracker & tracker = *it;
        if (tracker.player == player){
            void * handle = tracker.script;
            if (handle != NULL){
                Script::Engine::getEngine()->destroyObject(handle);
            }
            tracker.player->setScriptObject(NULL);
            
            it = players.erase(it);
        } else {
            it++;
        }
    }

    for ( vector< Object * >::iterator it = objects.begin(); it != objects.end(); ){
        Object * o = *it;
        if (o->getId() == player->getId()){
            it = objects.erase(it);
        } else {
            it++;
        }
    }
}

void NetworkWorld::removeSocket(Network::Socket socket){
    for (vector<Network::Socket>::iterator it = sockets.begin(); it != sockets.end(); ){
        if (socket == *it){
            it = sockets.erase(it);
        } else {
            it++;
        }
    }
}
                            
Object * NetworkWorld::findPlayerFromSocket(Network::Socket socket){
    for (map<Object*, Network::Socket>::iterator it = characterToClient.begin(); it != characterToClient.end(); it++){
        Object * who = (*it).first;
        Network::Socket search = (*it).second;
        if (search == socket){
            return who;
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
				Character * c_grabbing = (Character *) findNetworkObject( grabbing );
				Character * c_grabbed = (Character *) findNetworkObject( grabbed );
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
				Character * c_grabbing = (Character *) findNetworkObject( grabbing );
				Character * c_grabbed = (Character *) findNetworkObject( grabbed );
				if ( c_grabbed != NULL && c_grabbing != NULL ){
					debug( 2 ) << grabbing << " threw " << grabbed << endl;
					c_grabbed->setFacing( c_grabbing->getOppositeFacing() );
					c_grabbed->thrown();
					addMessage( c_grabbed->movedMessage() );
                                        /* don't hardcode the fall numbers */
					addMessage( c_grabbed->fallMessage( 3.2, 5.0 ) );
					c_grabbed->fall( 3.2, 5.0 );
					c_grabbing->setStatus( Status_Ground );
					debug( 2 ) << grabbed << " status is " << c_grabbed->getStatus() << endl;
				}
				break;
			}
                        case QUIT : {
                            /* when a client quits we have to
                             * 1. delete that player from existence, send DELETE_OBJ
                             * 2. stop the socket input threads 
                             * 3. remove the socket from our list of clients
                             * The thread will die on its own so we don't need to kill it.
                             */
                            Object * player = findPlayerFromSocket(message.readFrom);
                            addMessage(deleteMessage(player->getId()));
                            removePlayer(player);
                            Network::close(message.readFrom);
                            removeSocket(message.readFrom);

                            /* TODO: add a warning message to the user
                             * that a client just quit.
                             */
                            break;
                        }
                        case PING_REQUEST : {
                            handlePing(message);
                            break;
                        }
                        case PAUSE : {
                            this->pause();

                            /* I dont think I need to send another pause message
                             * because all client messages get sent to each other
                             * anyway.
                             */
                            addMessage(pausedMessage());
                            break;
                        }
                        case UNPAUSE : {
                            this->unpause();
                            addMessage(unpausedMessage());
                            break;
                        }
		}
	} else {
		Object * o = findNetworkObject( message.id );
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
            Network::Socket to = (*it).to;

            /* send the socket if
             * 1. the sender is not the same as the receiver and
             * 2. a) either the receiver is not specified or
             *    b) the receiver is the right one
             */
            if (from != *socket && (to == 0 || to == *socket)){
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
    AdventureWorld::act();

    vector< Network::Message > messages = getIncomingMessages();
    for ( vector< Network::Message >::iterator it = messages.begin(); it != messages.end(); it++ ){
        handleMessage( *it );
    }

    flushOutgoing();
}
